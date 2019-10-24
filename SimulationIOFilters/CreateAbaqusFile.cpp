/*
 * Your License or Copyright can go here
 */

#include "CreateAbaqusFile.h"

#include <QtCore/QDir>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicTableFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "SimulationIO/SimulationIOConstants.h"
#include "SimulationIO/SimulationIOVersion.h"

#include "SimulationIO/SimulationIOFilters/Utility/AbaqusFileWriter.h"

struct CreateAbaqusFile::Impl
{
  DataArray<int32_t>::WeakPointer m_FeatureIdsPtr;
  DataArray<int32_t>::WeakPointer m_CellPhasesPtr;
  DataArray<float>::WeakPointer m_CellEulerAnglesPtr;

  void reset()
  {
    m_FeatureIdsPtr.reset();
    m_CellPhasesPtr.reset();
    m_CellEulerAnglesPtr.reset();
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateAbaqusFile::CreateAbaqusFile()
: p_Impl(std::make_unique<Impl>())
, m_NumDepvar(1)
, m_NumUserOutVar(1)
, m_AbqFeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_CellEulerAnglesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::EulerAngles)
, m_CellPhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Phases)
{
  initialize();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateAbaqusFile::~CreateAbaqusFile() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateAbaqusFile::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateAbaqusFile::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_OUTPUT_PATH_FP("Output Path ", OutputPath, FilterParameter::Parameter, CreateAbaqusFile, "*", "*"));
  parameters.push_back(SIMPL_NEW_STRING_FP("Output File Prefix", OutputFilePrefix, FilterParameter::Parameter, CreateAbaqusFile));

  parameters.push_back(SIMPL_NEW_STRING_FP("Job Name", JobName, FilterParameter::Parameter, CreateAbaqusFile));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Number of Solution Dependent State Variables", NumDepvar, FilterParameter::Parameter, CreateAbaqusFile));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Number of User Output Variables", NumUserOutVar, FilterParameter::Parameter, CreateAbaqusFile));

  // Table - Dynamic rows and fixed columns
  {
    QStringList cHeaders;
    cHeaders << "Values";
    std::vector<std::vector<double>> defaultTable(6, std::vector<double>(1, 0.0));
    m_MatConst.setColHeaders(cHeaders);
    m_MatConst.setTableData(defaultTable);
    m_MatConst.setDynamicRows(true);
    parameters.push_back(SIMPL_NEW_DYN_TABLE_FP("Material Constants", MatConst, FilterParameter::Parameter, CreateAbaqusFile));
  }

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", AbqFeatureIdsArrayPath, FilterParameter::RequiredArray, CreateAbaqusFile, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Euler Angles", CellEulerAnglesArrayPath, FilterParameter::RequiredArray, CreateAbaqusFile, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", CellPhasesArrayPath, FilterParameter::RequiredArray, CreateAbaqusFile, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateAbaqusFile::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOutputPath(reader->readString("OutputPath", getOutputPath()));
  setOutputFilePrefix(reader->readString("OutputFilePrefix", getOutputFilePrefix()));
  setJobName(reader->readString("JobName", getJobName()));
  setNumDepvar(reader->readValue("NumDepvar", getNumDepvar()));
  setNumUserOutVar(reader->readValue("NumUserOutVar", getNumUserOutVar()));
  setMatConst(reader->readDynamicTableData("MatConst", getMatConst()));
  setAbqFeatureIdsArrayPath(reader->readDataArrayPath("AbqFeatureIdsArrayPath", getAbqFeatureIdsArrayPath()));
  setCellEulerAnglesArrayPath(reader->readDataArrayPath("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath()));
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateAbaqusFile::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  p_Impl->reset();

  if(m_OutputPath.isEmpty())
  {
    QString ss = QObject::tr("The output path must be set");
    setErrorCondition(-12001, ss);
  }

  QDir dir(m_OutputPath);
  if(!dir.exists())
  {
    QString ss = QObject::tr("The directory path for the output file does not exist. DREAM.3D will attempt to create this path during execution of the filter");
    setWarningCondition(-10200, ss);
  }

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getAbqFeatureIdsArrayPath().getDataContainerName());

  QVector<DataArrayPath> dataArrayPaths;

  std::vector<size_t> cDims{1};

  p_Impl->m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getAbqFeatureIdsArrayPath(), cDims);

  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getAbqFeatureIdsArrayPath());
  }

  p_Impl->m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), cDims);
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getCellPhasesArrayPath());
  }

  cDims[0] = 3;
  p_Impl->m_CellEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellEulerAnglesArrayPath(), cDims);
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getCellEulerAnglesArrayPath());
  }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateAbaqusFile::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateAbaqusFile::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  if(getCancel())
  {
    return;
  }

  QDir dir;
  if(!dir.mkpath(m_OutputPath))
  {
    QString ss = QObject::tr("Error creating path '%1'").arg(m_OutputPath);
    setErrorCondition(-10201, ss);
    return;
  }

  DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(getAbqFeatureIdsArrayPath().getDataContainerName());
  if(dc == nullptr)
  {
    QString ss = QObject::tr("Error obtaining data container '%1'").arg(m_AbqFeatureIdsArrayPath.getDataContainerName());
    setErrorCondition(-10202, ss);
    return;
  }

  auto imageGeom = dc->getGeometryAs<ImageGeom>();
  if(imageGeom == nullptr)
  {
    QString ss = QObject::tr("Error obtaining image geometry from data container '%1'").arg(m_AbqFeatureIdsArrayPath.getDataContainerName());
    setErrorCondition(-10203, ss);
    return;
  }

  auto featureIds = p_Impl->m_FeatureIdsPtr.lock();
  if(featureIds == nullptr)
  {
    QString ss = QObject::tr("Error obtaining feature ids data array'%1'").arg(m_AbqFeatureIdsArrayPath.serialize());
    setErrorCondition(-10204, ss);
    return;
  }

  auto cellPhases = p_Impl->m_CellPhasesPtr.lock();
  if(cellPhases == nullptr)
  {
    QString ss = QObject::tr("Error obtaining cell phases data array'%1'").arg(m_CellPhasesArrayPath.serialize());
    setErrorCondition(-10205, ss);
    return;
  }

  auto cellEulerAngles = p_Impl->m_CellEulerAnglesPtr.lock();
  if(cellEulerAngles == nullptr)
  {
    QString ss = QObject::tr("Error obtaining cell Euler angles data array'%1'").arg(m_CellEulerAnglesArrayPath.serialize());
    setErrorCondition(-10206, ss);
    return;
  }

  if(!AbaqusFileWriter::write(*imageGeom, *featureIds, *cellPhases, *cellEulerAngles, m_MatConst, m_OutputPath, m_OutputFilePrefix, m_JobName, m_NumDepvar, m_NumUserOutVar))
  {
    QString ss = QObject::tr("Error writing file at '%1'").arg(m_OutputPath);
    setErrorCondition(-10207, ss);
    return;
  }
}
//
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateAbaqusFile::newFilterInstance(bool copyFilterParameters) const
{
  CreateAbaqusFile::Pointer filter = CreateAbaqusFile::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateAbaqusFile::getCompiledLibraryName() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateAbaqusFile::getBrandingString() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateAbaqusFile::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SimulationIO::Version::Major() << "." << SimulationIO::Version::Minor() << "." << SimulationIO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateAbaqusFile::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateAbaqusFile::getSubGroupName() const
{
  return "SimulationIO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateAbaqusFile::getHumanLabel() const
{
  return "Create Abaqus File";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid CreateAbaqusFile::getUuid()
{
  return QUuid("{d702beff-eb02-5ee1-a76a-79d5b56ec730}");
}
