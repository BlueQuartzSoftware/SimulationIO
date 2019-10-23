/*
 * Your License or Copyright can go here
 */

#include "CreateFEAInputFiles.h"

#include <QtCore/QDir>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SIMPLib/FilterParameters/DynamicTableFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Utilities/FileSystemPathHelper.h"

#include "SimulationIO/SimulationIOConstants.h"
#include "SimulationIO/SimulationIOVersion.h"

#include "SimulationIO/SimulationIOFilters/Utility/AbaqusFileWriter.h"
#include "SimulationIO/SimulationIOFilters/Utility/BsamFileWriter.h"
#include "SimulationIO/SimulationIOFilters/Utility/OnScaleTableFileWriter.h"

struct CreateFEAInputFiles::Impl
{
  DataArray<int32_t>::WeakPointer m_FeatureIdsPtr;
  DataArray<int32_t>::WeakPointer m_CellPhasesPtr;
  DataArray<float>::WeakPointer m_CellEulerAnglesPtr;

  StringDataArray::WeakPointer m_PhaseNamesPtr;

  void reset()
  {
    m_FeatureIdsPtr.reset();
    m_CellPhasesPtr.reset();
    m_CellEulerAnglesPtr.reset();
    m_PhaseNamesPtr.reset();
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateFEAInputFiles::CreateFEAInputFiles()
: p_Impl(std::make_unique<Impl>())
, m_FEAPackage(0)
, m_JobName("")
, m_OutputPath("")
, m_OutputFilePrefix("")
, m_NumDepvar(1)
, m_NumMatConst(6)
, m_NumUserOutVar(1)
, m_AbqFeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_PzflexFeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_CellPhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Phases)
, m_CellEulerAnglesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::EulerAngles)
, m_NumClusters(1)
, m_PhaseNamesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::EnsembleAttributeMatrixName, SIMPL::EnsembleData::PhaseName)
, m_NumKeypoints({2, 2, 2})
{
  initialize();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateFEAInputFiles::~CreateFEAInputFiles() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFEAInputFiles::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFEAInputFiles::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("FEA package");
    parameter->setPropertyName("FEAPackage");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(CreateFEAInputFiles, this, FEAPackage));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(CreateFEAInputFiles, this, FEAPackage));
    QVector<QString> choices;
    choices.push_back("ABAQUS");
    choices.push_back("PZFLEX");
    choices.push_back("BSAM");
    parameter->setChoices(choices);
    QStringList linkedProps = {
        "JobName",      "NumDepvar",   "NumMatConst",        "NumUserOutVar", "MatConst", "AbqFeatureIdsArrayPath", "PzflexFeatureIdsArrayPath", "CellEulerAnglesArrayPath", "CellPhasesArrayPath",
        "NumKeypoints", "NumClusters", "PhaseNamesArrayPath"};
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  parameters.push_back(SIMPL_NEW_OUTPUT_PATH_FP("Output Path ", OutputPath, FilterParameter::Parameter, CreateFEAInputFiles, "*", "*"));
  parameters.push_back(SIMPL_NEW_STRING_FP("Output File Prefix", OutputFilePrefix, FilterParameter::Parameter, CreateFEAInputFiles));

  {
    parameters.push_back(SIMPL_NEW_STRING_FP("Job Name", JobName, FilterParameter::Parameter, CreateFEAInputFiles, 0));
    parameters.push_back(SIMPL_NEW_INTEGER_FP("Number of Solution Dependent State Variables", NumDepvar, FilterParameter::Parameter, CreateFEAInputFiles, 0));
    parameters.push_back(SIMPL_NEW_INTEGER_FP("Number of Material Constants", NumMatConst, FilterParameter::Parameter, CreateFEAInputFiles, 0));
    parameters.push_back(SIMPL_NEW_INTEGER_FP("Number of User Output Variables", NumUserOutVar, FilterParameter::Parameter, CreateFEAInputFiles, 0));
    // Table - Dynamic rows and fixed columns
    {
      QStringList cHeaders;
      cHeaders << "Values";
      std::vector<std::vector<double>> defaultTable(1, std::vector<double>(1, 0.0));
      m_MatConst.setColHeaders(cHeaders);
      m_MatConst.setTableData(defaultTable);
      m_MatConst.setDynamicRows(true);
      parameters.push_back(SIMPL_NEW_DYN_TABLE_FP("Material Constants", MatConst, FilterParameter::Parameter, CreateFEAInputFiles, 0));
    }
  }

  {
    parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Number of Keypoints", NumKeypoints, FilterParameter::Parameter, CreateFEAInputFiles, 1));
    parameters.push_back(SeparatorFilterParameter::New("Ensemble Data", FilterParameter::RequiredArray));
    {
      DataArraySelectionFilterParameter::RequirementType req =
          DataArraySelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyPrimitive, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Image);
      parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phase Names", PhaseNamesArrayPath, FilterParameter::RequiredArray, CreateFEAInputFiles, req, 1));
    }
  }

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", AbqFeatureIdsArrayPath, FilterParameter::RequiredArray, CreateFEAInputFiles, req, 0));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Euler Angles", CellEulerAnglesArrayPath, FilterParameter::RequiredArray, CreateFEAInputFiles, req, 0));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", CellPhasesArrayPath, FilterParameter::RequiredArray, CreateFEAInputFiles, req, 0));
  }

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", PzflexFeatureIdsArrayPath, FilterParameter::RequiredArray, CreateFEAInputFiles, req, 1));
  }

  {
    parameters.push_back(SIMPL_NEW_INTEGER_FP("Number of Clusters", NumClusters, FilterParameter::Parameter, CreateFEAInputFiles, 2));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFEAInputFiles::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFEAPackage(reader->readValue("FEAPackage", getFEAPackage()));
  setOutputPath(reader->readString("OutputPath", getOutputPath()));
  setOutputFilePrefix(reader->readString("OutputFilePrefix", getOutputFilePrefix()));
  setJobName(reader->readString("JobName", getJobName()));
  setNumDepvar(reader->readValue("NumDepvar", getNumDepvar()));
  setNumMatConst(reader->readValue("NumMatConst", getNumMatConst()));
  setNumUserOutVar(reader->readValue("NumUserOutVar", getNumUserOutVar()));
  setMatConst(reader->readDynamicTableData("MatConst", getMatConst()));
  setNumKeypoints(reader->readIntVec3("NumKeypoints", getNumKeypoints()));
  setAbqFeatureIdsArrayPath(reader->readDataArrayPath("AbqFeatureIdsArrayPath", getAbqFeatureIdsArrayPath()));
  setCellEulerAnglesArrayPath(reader->readDataArrayPath("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath()));
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath()));
  setPzflexFeatureIdsArrayPath(reader->readDataArrayPath("PzflexFeatureIdsArrayPath", getPzflexFeatureIdsArrayPath()));
  setNumClusters(reader->readValue("NumClusters", getNumClusters()));
  setPhaseNamesArrayPath(reader->readDataArrayPath("PhaseNamesArrayPath", getPhaseNamesArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFEAInputFiles::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  p_Impl->reset();

  if(m_OutputPath.isEmpty())
  {
    QString ss = QObject::tr("The output path must be set");
    setErrorCondition(-12001, ss);
  }

  QFileInfo fi(m_OutputPath);
  QDir parentPath = fi.path();
  if(!parentPath.exists())
  {
    QString ss = QObject::tr("The directory path for the output file does not exist. DREAM.3D will attempt to create this path during execution of the filter");
    setWarningCondition(-10100, ss);
  }

  switch(m_FEAPackage)
  {
  case 0: // ABAQUS
  {
    getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getAbqFeatureIdsArrayPath().getDataContainerName());

    QVector<DataArrayPath> dataArrayPaths;

    std::vector<size_t> cDims{1};

    p_Impl->m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getAbqFeatureIdsArrayPath(), cDims);
    if(!p_Impl->m_FeatureIdsPtr.expired())
    {
    }
    if(getErrorCode() >= 0)
    {
      dataArrayPaths.push_back(getAbqFeatureIdsArrayPath());
    }

    p_Impl->m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), cDims);
    if(nullptr != p_Impl->m_CellPhasesPtr.lock())
    {
    }
    if(getErrorCode() >= 0)
    {
      dataArrayPaths.push_back(getCellPhasesArrayPath());
    }

    cDims[0] = 3;
    p_Impl->m_CellEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellEulerAnglesArrayPath(), cDims);
    if(nullptr != p_Impl->m_CellEulerAnglesPtr.lock())
    {
    }
    if(getErrorCode() >= 0)
    {
      dataArrayPaths.push_back(getCellEulerAnglesArrayPath());
    }

    getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);

    break;
  }
  case 1:
  {
    getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getPzflexFeatureIdsArrayPath().getDataContainerName());

    QVector<DataArrayPath> dataArrayPaths;

    std::vector<size_t> cDims(1, 1);

    p_Impl->m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getPzflexFeatureIdsArrayPath(), cDims);
    if(nullptr != p_Impl->m_FeatureIdsPtr.lock())
    {
    }
    if(getErrorCode() >= 0)
    {
      dataArrayPaths.push_back(getPzflexFeatureIdsArrayPath());
    }

    p_Impl->m_PhaseNamesPtr = getDataContainerArray()->getPrereqArrayFromPath<StringDataArray, AbstractFilter>(this, getPhaseNamesArrayPath(), cDims);

    if(getErrorCode() >= 0)
    {
      dataArrayPaths.push_back(getPhaseNamesArrayPath());
    }

    break;
  }
  }
  //
  //
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFEAInputFiles::preflight()
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
void CreateFEAInputFiles::execute()
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

  // Check Output Path

  QDir dir;
  if(!dir.mkpath(m_OutputPath))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(m_OutputPath);
    setErrorCondition(-1, ss);
    return;
  }

  switch(m_FEAPackage)
  {
  case 0: // ABAQUS
  {
    DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(getAbqFeatureIdsArrayPath().getDataContainerName());
    if(dc == nullptr)
    {
      QString ss = QObject::tr("The data container cannot be found");
      setErrorCondition(-95009, ss);
      return;
    }

    auto imageGeom = dc->getGeometryAs<ImageGeom>();
    if(imageGeom == nullptr)
    {
      return;
    }

    auto featureIds = p_Impl->m_FeatureIdsPtr.lock();
    if(featureIds == nullptr)
    {
      return;
    }

    auto cellPhases = p_Impl->m_CellPhasesPtr.lock();
    if(cellPhases == nullptr)
    {
      return;
    }

    auto cellEulerAngles = p_Impl->m_CellEulerAnglesPtr.lock();
    if(cellEulerAngles == nullptr)
    {
      return;
    }

    if(!AbaqusFileWriter::write(*imageGeom, *featureIds, *cellPhases, *cellEulerAngles, m_MatConst, m_OutputPath, m_OutputFilePrefix, m_JobName, m_NumDepvar, m_NumMatConst, m_NumUserOutVar))
    {
      return;
    }
    break;
  }
  case 1: // PZFLEX
  {
    DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(getPzflexFeatureIdsArrayPath().getDataContainerName());
    if(dc == nullptr)
    {
      return;
    }

    auto imageGeom = dc->getGeometryAs<ImageGeom>();
    if(imageGeom == nullptr)
    {
      return;
    }

    auto phaseNames = p_Impl->m_PhaseNamesPtr.lock();
    if(phaseNames == nullptr)
    {
      return;
    }

    auto featureIds = p_Impl->m_FeatureIdsPtr.lock();
    if(featureIds == nullptr)
    {
      return;
    }

    if(!OnScaleTableFileWriter::write(*imageGeom, *phaseNames, *featureIds, m_OutputPath, m_OutputFilePrefix, m_NumKeypoints))
    {
      return;
    }
    break;
  }
  case 2: // BSAM
  {
    if(!BsamFileWriter::write(m_OutputPath, m_OutputFilePrefix, m_NumClusters))
    {
      return;
    }
    break;
  }
  }
}
//
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateFEAInputFiles::newFilterInstance(bool copyFilterParameters) const
{
  CreateFEAInputFiles::Pointer filter = CreateFEAInputFiles::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateFEAInputFiles::getCompiledLibraryName() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateFEAInputFiles::getBrandingString() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateFEAInputFiles::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SimulationIO::Version::Major() << "." << SimulationIO::Version::Minor() << "." << SimulationIO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateFEAInputFiles::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateFEAInputFiles::getSubGroupName() const
{
  return "SimulationIO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateFEAInputFiles::getHumanLabel() const
{
  return "Create FEA Input Files";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid CreateFEAInputFiles::getUuid()
{
  return QUuid("{e7f02408-6c01-5b56-b970-7813e64c12e2}");
}
