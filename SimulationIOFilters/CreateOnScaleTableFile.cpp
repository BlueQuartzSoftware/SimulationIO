/*
 * Your License or Copyright can go here
 */

#include "CreateOnScaleTableFile.h"

#include <QtCore/QDir>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "SimulationIO/SimulationIOConstants.h"
#include "SimulationIO/SimulationIOVersion.h"

#include "SimulationIO/SimulationIOFilters/Utility/OnScaleTableFileWriter.h"

struct CreateOnScaleTableFile::Impl
{
  DataArray<int32_t>::WeakPointer m_FeatureIdsPtr;
  StringDataArray::WeakPointer m_PhaseNamesPtr;

  void reset()
  {
    m_FeatureIdsPtr.reset();
    m_PhaseNamesPtr.reset();
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateOnScaleTableFile::CreateOnScaleTableFile()
: p_Impl(std::make_unique<Impl>())
, m_OutputPath("")
, m_OutputFilePrefix("")
, m_PzflexFeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_PhaseNamesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::EnsembleAttributeMatrixName, SIMPL::EnsembleData::PhaseName)
, m_NumKeypoints({2, 2, 2})
{
  initialize();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateOnScaleTableFile::~CreateOnScaleTableFile() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateOnScaleTableFile::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateOnScaleTableFile::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_OUTPUT_PATH_FP("Output Path ", OutputPath, FilterParameter::Parameter, CreateOnScaleTableFile, "*", "*"));
  parameters.push_back(SIMPL_NEW_STRING_FP("Output File Prefix", OutputFilePrefix, FilterParameter::Parameter, CreateOnScaleTableFile));

  {
    parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Number of Keypoints", NumKeypoints, FilterParameter::Parameter, CreateOnScaleTableFile));
    parameters.push_back(SeparatorFilterParameter::New("Ensemble Data", FilterParameter::RequiredArray));
    {
      DataArraySelectionFilterParameter::RequirementType req =
          DataArraySelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyPrimitive, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Image);
      parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phase Names", PhaseNamesArrayPath, FilterParameter::RequiredArray, CreateOnScaleTableFile, req));
    }
  }

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", PzflexFeatureIdsArrayPath, FilterParameter::RequiredArray, CreateOnScaleTableFile, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateOnScaleTableFile::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOutputPath(reader->readString("OutputPath", getOutputPath()));
  setOutputFilePrefix(reader->readString("OutputFilePrefix", getOutputFilePrefix()));
  setNumKeypoints(reader->readIntVec3("NumKeypoints", getNumKeypoints()));
  setPzflexFeatureIdsArrayPath(reader->readDataArrayPath("PzflexFeatureIdsArrayPath", getPzflexFeatureIdsArrayPath()));
  setPhaseNamesArrayPath(reader->readDataArrayPath("PhaseNamesArrayPath", getPhaseNamesArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateOnScaleTableFile::dataCheck()
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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateOnScaleTableFile::preflight()
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
void CreateOnScaleTableFile::execute()
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
}
//
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateOnScaleTableFile::newFilterInstance(bool copyFilterParameters) const
{
  CreateOnScaleTableFile::Pointer filter = CreateOnScaleTableFile::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateOnScaleTableFile::getCompiledLibraryName() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateOnScaleTableFile::getBrandingString() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateOnScaleTableFile::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SimulationIO::Version::Major() << "." << SimulationIO::Version::Minor() << "." << SimulationIO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateOnScaleTableFile::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateOnScaleTableFile::getSubGroupName() const
{
  return "SimulationIO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateOnScaleTableFile::getHumanLabel() const
{
  return "Create OnScale Table File";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid CreateOnScaleTableFile::getUuid()
{
  return QUuid("{8efc447d-1c92-5ec5-885c-60b4a597835c}");
}
