/*
 * Your License or Copyright can go here
 */

#include "CreateMultiOnScaleTableFile.h"

#include <QtCore/QDir>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "SimulationIO/SimulationIOConstants.h"
#include "SimulationIO/SimulationIOVersion.h"

#include "SimulationIO/SimulationIOFilters/Utility/OnScaleTableFileWriter.h"

struct CreateMultiOnScaleTableFile::Impl
{
  StringDataArray::WeakPointer m_PhaseNamesPtr;
  std::vector<std::pair<DataArrayPath, DataArray<int32_t>::WeakPointer>> m_FeatureIdsList;

  Impl() = default;

  ~Impl() = default;

  Impl(const Impl&) = delete;
  Impl(Impl&&) = delete;
  Impl& operator=(const Impl&) = delete;
  Impl& operator=(Impl&&) = delete;

  void resetDataArrays()
  {
    m_PhaseNamesPtr.reset();
    m_FeatureIdsList.clear();
  }
};

// -----------------------------------------------------------------------------
CreateMultiOnScaleTableFile::CreateMultiOnScaleTableFile()
: p_Impl(std::make_unique<Impl>())
, m_NumKeypoints(2, 2, 2)
, m_PhaseNamesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::EnsembleAttributeMatrixName, SIMPL::EnsembleData::PhaseName)
{
  initialize();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
CreateMultiOnScaleTableFile::~CreateMultiOnScaleTableFile() = default;

// -----------------------------------------------------------------------------
void CreateMultiOnScaleTableFile::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
void CreateMultiOnScaleTableFile::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_OUTPUT_PATH_FP("Output Path ", OutputPath, FilterParameter::Parameter, CreateMultiOnScaleTableFile, "*", "*"));
  parameters.push_back(SIMPL_NEW_STRING_FP("Data Container Prefix", DataContainerPrefix, FilterParameter::Parameter, CreateMultiOnScaleTableFile));
  parameters.push_back(SIMPL_NEW_STRING_FP("Matrix Name", MatrixName, FilterParameter::Parameter, CreateMultiOnScaleTableFile));
  parameters.push_back(SIMPL_NEW_STRING_FP("Array Name", ArrayName, FilterParameter::Parameter, CreateMultiOnScaleTableFile));

  parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Number of Keypoints", NumKeypoints, FilterParameter::Parameter, CreateMultiOnScaleTableFile));
  parameters.push_back(SeparatorFilterParameter::New("Ensemble Data", FilterParameter::RequiredArray));

  parameters.push_back(SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Selected Arrays", SelectedArrays, FilterParameter::Category::Parameter, CreateMultiOnScaleTableFile));

  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyPrimitive, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phase Names", PhaseNamesArrayPath, FilterParameter::RequiredArray, CreateMultiOnScaleTableFile, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void CreateMultiOnScaleTableFile::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOutputPath(reader->readString("OutputPath", getOutputPath()));
  setDataContainerPrefix(reader->readString("DataContainerPrefix", getDataContainerPrefix()));
  setMatrixName(reader->readString("MatrixName", getMatrixName()));
  setArrayName(reader->readString("ArrayName", getArrayName()));
  setSelectedArrays(reader->readString("SelectedArrays", getSelectedArrays()));
  setNumKeypoints(reader->readIntVec3("NumKeypoints", getNumKeypoints()));
  setPhaseNamesArrayPath(reader->readDataArrayPath("PhaseNamesArrayPath", getPhaseNamesArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
void CreateMultiOnScaleTableFile::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  p_Impl->resetDataArrays();
  m_SelectedArrays.clear();

  if(m_OutputPath.isEmpty())
  {
    QString ss = QObject::tr("The output path must be set");
    setErrorCondition(-12001, ss);
  }

  QDir dir(m_OutputPath);
  if(!dir.exists())
  {
    QString ss = QObject::tr("The directory path for the output file does not exist. DREAM.3D will attempt to create this path during execution of the filter");
    setWarningCondition(-10400, ss);
  }

  auto dca = getDataContainerArray();

  auto dcList = dca->getDataContainers();

  std::vector<DataContainer::Pointer> availableDataContainers;
  QString prefix = m_DataContainerPrefix;
  std::copy_if(dcList.cbegin(), dcList.cend(), std::back_inserter(availableDataContainers), [prefix](const DataContainer::ConstPointer& dc) { return dc->getName().startsWith(prefix); });

  for(DataContainer::ConstPointer dc : availableDataContainers)
  {
    auto geom = dc->getGeometry();
    if(geom == nullptr)
    {
      continue;
    }
    if(geom->getGeometryType() != IGeometry::Type::Image)
    {
      continue;
    }
    if(!dc->doesAttributeMatrixExist(m_MatrixName))
    {
      continue;
    }
    auto matrix = dc->getAttributeMatrix(m_MatrixName);
    if(matrix == nullptr)
    {
      continue;
    }
    if(!matrix->doesAttributeArrayExist(m_ArrayName))
    {
      continue;
    }
    if(matrix->getType() != AttributeMatrix::Type::Cell)
    {
      continue;
    }
    auto dataArray = matrix->getAttributeArrayAs<Int32ArrayType>(m_ArrayName);
    if(dataArray == nullptr)
    {
      continue;
    }

    auto path = dataArray->getDataArrayPath();

    p_Impl->m_FeatureIdsList.push_back({path, dataArray});

    m_SelectedArrays += path.serialize() + "\n";
  }

  std::vector<size_t> cDims{1};

  p_Impl->m_PhaseNamesPtr = getDataContainerArray()->getPrereqArrayFromPath<StringDataArray, AbstractFilter>(this, getPhaseNamesArrayPath(), cDims);
}

// -----------------------------------------------------------------------------
void CreateMultiOnScaleTableFile::preflight()
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
void CreateMultiOnScaleTableFile::execute()
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
    setErrorCondition(-10401, ss);
    return;
  }

  auto phaseNames = p_Impl->m_PhaseNamesPtr.lock();
  if(phaseNames == nullptr)
  {
    QString ss = QObject::tr("Error obtaining phase names data array '%1'").arg(m_PhaseNamesArrayPath.serialize());
    setErrorCondition(-10402, ss);
    return;
  }

  for(const auto& dataArrayPair : p_Impl->m_FeatureIdsList)
  {
    auto path = dataArrayPair.first;
    auto dataArrayPtr = dataArrayPair.second;
    auto featureIds = dataArrayPtr.lock();
    if(featureIds == nullptr)
    {
      QString ss = QObject::tr("Error obtaining feature ids data array '%1'").arg(path.serialize());
      setErrorCondition(-10403, ss);
      return;
    }

    QString dcName = path.getDataContainerName();

    DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(dcName);
    if(dc == nullptr)
    {
      QString ss = QObject::tr("Error obtaining data container '%1'").arg(dcName);
      setErrorCondition(-10404, ss);
      return;
    }

    auto imageGeom = dc->getGeometryAs<ImageGeom>();
    if(imageGeom == nullptr)
    {
      QString ss = QObject::tr("Error obtaining image geometry from data container '%1'").arg(dcName);
      setErrorCondition(-10406, ss);
      return;
    }

    if(!OnScaleTableFileWriter::write(*imageGeom, *phaseNames, *featureIds, m_OutputPath, dcName, m_NumKeypoints))
    {
      QString ss = QObject::tr("Error writing file at '%1' for DataContainer '%2'").arg(m_OutputPath, dcName);
      setErrorCondition(-10407, ss);
      return;
    }
  }
}
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateMultiOnScaleTableFile::newFilterInstance(bool copyFilterParameters) const
{
  CreateMultiOnScaleTableFile::Pointer filter = CreateMultiOnScaleTableFile::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
QString CreateMultiOnScaleTableFile::getCompiledLibraryName() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
QString CreateMultiOnScaleTableFile::getBrandingString() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
QString CreateMultiOnScaleTableFile::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SimulationIO::Version::Major() << "." << SimulationIO::Version::Minor() << "." << SimulationIO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
QString CreateMultiOnScaleTableFile::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
QString CreateMultiOnScaleTableFile::getSubGroupName() const
{
  return "SimulationIO";
}

// -----------------------------------------------------------------------------
QString CreateMultiOnScaleTableFile::getHumanLabel() const
{
  return "Create Multi OnScale Table File";
}

// -----------------------------------------------------------------------------
QUuid CreateMultiOnScaleTableFile::getUuid() const
{
  return QUuid("{d5873836-f150-5fc9-9bc8-0bc837bd8dd4}");
}

// -----------------------------------------------------------------------------
CreateMultiOnScaleTableFile::Pointer CreateMultiOnScaleTableFile::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
CreateMultiOnScaleTableFile::Pointer CreateMultiOnScaleTableFile::New()
{
  struct make_shared_enabler : public CreateMultiOnScaleTableFile
  {
  };
  return std::make_shared<make_shared_enabler>();
}

// -----------------------------------------------------------------------------
QString CreateMultiOnScaleTableFile::getNameOfClass() const
{
  return ClassName();
}

// -----------------------------------------------------------------------------
QString CreateMultiOnScaleTableFile::ClassName()
{
  return QString("CreateMultiOnScaleTableFile");
}

// -----------------------------------------------------------------------------
void CreateMultiOnScaleTableFile::setOutputPath(const QString& value)
{
  m_OutputPath = value;
}

// -----------------------------------------------------------------------------
QString CreateMultiOnScaleTableFile::getOutputPath() const
{
  return m_OutputPath;
}

// -----------------------------------------------------------------------------
void CreateMultiOnScaleTableFile::setDataContainerPrefix(const QString& value)
{
  m_DataContainerPrefix = value;
}

// -----------------------------------------------------------------------------
QString CreateMultiOnScaleTableFile::getDataContainerPrefix() const
{
  return m_DataContainerPrefix;
}

// -----------------------------------------------------------------------------
void CreateMultiOnScaleTableFile::setMatrixName(const QString& value)
{
  m_MatrixName = value;
}

// -----------------------------------------------------------------------------
QString CreateMultiOnScaleTableFile::getMatrixName() const
{
  return m_MatrixName;
}

// -----------------------------------------------------------------------------
void CreateMultiOnScaleTableFile::setArrayName(const QString& value)
{
  m_ArrayName = value;
}

// -----------------------------------------------------------------------------
QString CreateMultiOnScaleTableFile::getArrayName() const
{
  return m_ArrayName;
}

// -----------------------------------------------------------------------------
void CreateMultiOnScaleTableFile::setSelectedArrays(const QString& value)
{
  m_SelectedArrays = value;
}

// -----------------------------------------------------------------------------
QString CreateMultiOnScaleTableFile::getSelectedArrays() const
{
  return m_SelectedArrays;
}

// -----------------------------------------------------------------------------
void CreateMultiOnScaleTableFile::setNumKeypoints(const IntVec3Type& value)
{
  m_NumKeypoints = value;
}

// -----------------------------------------------------------------------------
IntVec3Type CreateMultiOnScaleTableFile::getNumKeypoints() const
{
  return m_NumKeypoints;
}

// -----------------------------------------------------------------------------
void CreateMultiOnScaleTableFile::setPhaseNamesArrayPath(const DataArrayPath& value)
{
  m_PhaseNamesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CreateMultiOnScaleTableFile::getPhaseNamesArrayPath() const
{
  return m_PhaseNamesArrayPath;
}
