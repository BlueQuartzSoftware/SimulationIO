/*
 * Your License or Copyright can go here
 */

#include "CreateOnScaleTableFile.h"

#include <QtCore/QDir>

#include "SIMPLib/DataContainers/DataContainerArray.h"
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

namespace
{
template <class T>
bool convertDataArrayPtr(IDataArray::ConstPointer dataArray, std::weak_ptr<const DataArray<T>>& weakPtr, CreateOnScaleTableFile* filter)
{
  auto ptr = std::dynamic_pointer_cast<const DataArray<T>>(dataArray);
  if(ptr == nullptr)
  {
    QString ss = QObject::tr("Unable to get '%1'").arg(dataArray->getDataArrayPath().serialize());
    filter->setErrorCondition(-10151, ss);
    return false;
  }

  weakPtr = ptr;

  return true;
}

template <class T>
bool writeOnScaleFile(std::weak_ptr<const DataArray<T>> featureIdsPtr, const ImageGeom& imageGeom, const StringDataArray& phaseNames, const QString& outputPath, const QString& outputFilePrefix,
                      const IntVec3Type& numKeypoints, CreateOnScaleTableFile* filter)
{
  auto featureIds = featureIdsPtr.lock();
  if(featureIds == nullptr)
  {
    QString ss = QObject::tr("Error obtaining feature ids data array '%1'").arg(featureIds->getDataArrayPath().serialize());
    filter->setErrorCondition(-10105, ss);
    return false;
  }

  if(!OnScaleTableFileWriter::write(imageGeom, phaseNames, *featureIds, outputPath, outputFilePrefix, numKeypoints))
  {
    QString ss = QObject::tr("Error writing file at '%1'").arg(outputPath);
    filter->setErrorCondition(-10106, ss);
    return false;
  }

  return true;
}
} // namespace

struct CreateOnScaleTableFile::Impl
{
  std::weak_ptr<const DataArray<int8_t>> m_FeatureIds8Ptr;
  std::weak_ptr<const DataArray<int16_t>> m_FeatureIds16Ptr;
  std::weak_ptr<const DataArray<int32_t>> m_FeatureIds32Ptr;
  std::weak_ptr<const DataArray<int64_t>> m_FeatureIds64Ptr;

  std::weak_ptr<const DataArray<uint8_t>> m_FeatureIdsU8Ptr;
  std::weak_ptr<const DataArray<uint16_t>> m_FeatureIdsU16Ptr;
  std::weak_ptr<const DataArray<uint32_t>> m_FeatureIdsU32Ptr;
  std::weak_ptr<const DataArray<uint64_t>> m_FeatureIdsU64Ptr;

  QString m_Type;

  std::weak_ptr<const StringDataArray> m_PhaseNamesPtr;

  Impl() = default;

  ~Impl() = default;

  Impl(const Impl&) = delete;
  Impl(Impl&&) = delete;
  Impl& operator=(const Impl&) = delete;
  Impl& operator=(Impl&&) = delete;

  void resetDataArrays()
  {
    m_Type.clear();

    m_FeatureIds8Ptr.reset();
    m_FeatureIds16Ptr.reset();
    m_FeatureIds32Ptr.reset();
    m_FeatureIds64Ptr.reset();

    m_FeatureIdsU8Ptr.reset();
    m_FeatureIdsU16Ptr.reset();
    m_FeatureIdsU32Ptr.reset();
    m_FeatureIdsU64Ptr.reset();

    m_PhaseNamesPtr.reset();
  }
};

// -----------------------------------------------------------------------------
CreateOnScaleTableFile::CreateOnScaleTableFile()
: p_Impl(std::make_unique<Impl>())
{
  initialize();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
CreateOnScaleTableFile::~CreateOnScaleTableFile() = default;

// -----------------------------------------------------------------------------
void CreateOnScaleTableFile::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
void CreateOnScaleTableFile::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_OUTPUT_PATH_FP("Output Path ", OutputPath, FilterParameter::Parameter, CreateOnScaleTableFile, "*", "*"));
  parameters.push_back(SIMPL_NEW_STRING_FP("Output File Prefix", OutputFilePrefix, FilterParameter::Parameter, CreateOnScaleTableFile));

  parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Number of Keypoints", NumKeypoints, FilterParameter::Parameter, CreateOnScaleTableFile));
  parameters.push_back(SeparatorFilterParameter::New("Ensemble Data", FilterParameter::RequiredArray));

  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyPrimitive, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phase Names", PhaseNamesArrayPath, FilterParameter::RequiredArray, CreateOnScaleTableFile, req));
  }

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));

  {
    QVector<QString> types{SIMPL::TypeNames::Int8,  SIMPL::TypeNames::Int16,  SIMPL::TypeNames::Int32,  SIMPL::TypeNames::Int64,
                           SIMPL::TypeNames::UInt8, SIMPL::TypeNames::UInt16, SIMPL::TypeNames::UInt32, SIMPL::TypeNames::UInt64};
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(types, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", PzflexFeatureIdsArrayPath, FilterParameter::RequiredArray, CreateOnScaleTableFile, req));
  }

  setFilterParameters(parameters);
}

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
void CreateOnScaleTableFile::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  p_Impl->resetDataArrays();

  if(m_OutputPath.isEmpty())
  {
    QString ss = QObject::tr("The output path must be set");
    setErrorCondition(-12001, ss);
  }

  QDir dir(m_OutputPath);
  if(!dir.exists())
  {
    QString ss = QObject::tr("The directory path for the output file does not exist. DREAM.3D will attempt to create this path during execution of the filter");
    setWarningCondition(-10100, ss);
  }

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getPzflexFeatureIdsArrayPath().getDataContainerName());

  std::vector<size_t> cDims{1};

  StringDataArray::ConstPointer phaseNamesPtr = getDataContainerArray()->getPrereqArrayFromPath<StringDataArray, AbstractFilter>(this, getPhaseNamesArrayPath(), cDims);

  if(phaseNamesPtr == nullptr)
  {
    return;
  }

  p_Impl->m_PhaseNamesPtr = phaseNamesPtr;

  IDataArray::ConstPointer featureIdsPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getPzflexFeatureIdsArrayPath());

  if(featureIdsPtr == nullptr)
  {
    return;
  }

  if(cDims != featureIdsPtr->getComponentDimensions())
  {
    QString ss = QObject::tr("Wrong component dimensions for '%1'").arg(getPzflexFeatureIdsArrayPath().serialize());
    setErrorCondition(-10152, ss);
    return;
  }

  p_Impl->m_Type = featureIdsPtr->getTypeAsString();

  if(p_Impl->m_Type == SIMPL::TypeNames::Int8)
  {
    convertDataArrayPtr(featureIdsPtr, p_Impl->m_FeatureIds8Ptr, this);
  }
  else if(p_Impl->m_Type == SIMPL::TypeNames::Int16)
  {
    convertDataArrayPtr(featureIdsPtr, p_Impl->m_FeatureIds16Ptr, this);
  }
  else if(p_Impl->m_Type == SIMPL::TypeNames::Int32)
  {
    convertDataArrayPtr(featureIdsPtr, p_Impl->m_FeatureIds32Ptr, this);
  }
  else if(p_Impl->m_Type == SIMPL::TypeNames::Int64)
  {
    convertDataArrayPtr(featureIdsPtr, p_Impl->m_FeatureIds64Ptr, this);
  }
  else if(p_Impl->m_Type == SIMPL::TypeNames::UInt8)
  {
    convertDataArrayPtr(featureIdsPtr, p_Impl->m_FeatureIdsU8Ptr, this);
  }
  else if(p_Impl->m_Type == SIMPL::TypeNames::UInt16)
  {
    convertDataArrayPtr(featureIdsPtr, p_Impl->m_FeatureIdsU16Ptr, this);
  }
  else if(p_Impl->m_Type == SIMPL::TypeNames::UInt32)
  {
    convertDataArrayPtr(featureIdsPtr, p_Impl->m_FeatureIdsU32Ptr, this);
  }
  else if(p_Impl->m_Type == SIMPL::TypeNames::UInt64)
  {
    convertDataArrayPtr(featureIdsPtr, p_Impl->m_FeatureIdsU64Ptr, this);
  }
  else
  {
    QString ss = QObject::tr("Invalid type of '%1' for '%2'. Must be an integer type").arg(p_Impl->m_Type).arg(getPzflexFeatureIdsArrayPath().serialize());
    setErrorCondition(-10150, ss);
  }
}

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

  QDir dir;
  if(!dir.mkpath(m_OutputPath))
  {
    QString ss = QObject::tr("Error creating path '%1'").arg(m_OutputPath);
    setErrorCondition(-10101, ss);
    return;
  }

  DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(getPzflexFeatureIdsArrayPath().getDataContainerName());
  if(dc == nullptr)
  {
    QString ss = QObject::tr("Error obtaining data container '%1'").arg(m_PzflexFeatureIdsArrayPath.getDataContainerName());
    setErrorCondition(-10102, ss);
    return;
  }

  auto imageGeom = dc->getGeometryAs<ImageGeom>();
  if(imageGeom == nullptr)
  {
    QString ss = QObject::tr("Error obtaining image geometry from data container '%1'").arg(m_PzflexFeatureIdsArrayPath.getDataContainerName());
    setErrorCondition(-10103, ss);
    return;
  }

  auto phaseNames = p_Impl->m_PhaseNamesPtr.lock();
  if(phaseNames == nullptr)
  {
    QString ss = QObject::tr("Error obtaining phase names data array '%1'").arg(m_PhaseNamesArrayPath.serialize());
    setErrorCondition(-10104, ss);
    return;
  }

  if(p_Impl->m_Type == SIMPL::TypeNames::Int8)
  {
    if(!writeOnScaleFile(p_Impl->m_FeatureIds8Ptr, *imageGeom, *phaseNames, m_OutputPath, m_OutputFilePrefix, m_NumKeypoints, this))
    {
      return;
    }
  }
  else if(p_Impl->m_Type == SIMPL::TypeNames::Int16)
  {
    if(!writeOnScaleFile(p_Impl->m_FeatureIds16Ptr, *imageGeom, *phaseNames, m_OutputPath, m_OutputFilePrefix, m_NumKeypoints, this))
    {
      return;
    }
  }
  else if(p_Impl->m_Type == SIMPL::TypeNames::Int32)
  {
    if(!writeOnScaleFile(p_Impl->m_FeatureIds32Ptr, *imageGeom, *phaseNames, m_OutputPath, m_OutputFilePrefix, m_NumKeypoints, this))
    {
      return;
    }
  }
  else if(p_Impl->m_Type == SIMPL::TypeNames::Int64)
  {
    if(!writeOnScaleFile(p_Impl->m_FeatureIds64Ptr, *imageGeom, *phaseNames, m_OutputPath, m_OutputFilePrefix, m_NumKeypoints, this))
    {
      return;
    }
  }
  else if(p_Impl->m_Type == SIMPL::TypeNames::UInt8)
  {
    if(!writeOnScaleFile(p_Impl->m_FeatureIdsU8Ptr, *imageGeom, *phaseNames, m_OutputPath, m_OutputFilePrefix, m_NumKeypoints, this))
    {
      return;
    }
  }
  else if(p_Impl->m_Type == SIMPL::TypeNames::UInt16)
  {
    if(!writeOnScaleFile(p_Impl->m_FeatureIdsU16Ptr, *imageGeom, *phaseNames, m_OutputPath, m_OutputFilePrefix, m_NumKeypoints, this))
    {
      return;
    }
  }
  else if(p_Impl->m_Type == SIMPL::TypeNames::UInt32)
  {
    if(!writeOnScaleFile(p_Impl->m_FeatureIdsU32Ptr, *imageGeom, *phaseNames, m_OutputPath, m_OutputFilePrefix, m_NumKeypoints, this))
    {
      return;
    }
  }
  else if(p_Impl->m_Type == SIMPL::TypeNames::UInt64)
  {
    if(!writeOnScaleFile(p_Impl->m_FeatureIdsU64Ptr, *imageGeom, *phaseNames, m_OutputPath, m_OutputFilePrefix, m_NumKeypoints, this))
    {
      return;
    }
  }
  else
  {
    QString ss = QObject::tr("Invalid type of '%1' for '%2'. Must be an integer type").arg(p_Impl->m_Type).arg(getPzflexFeatureIdsArrayPath().serialize());
    setErrorCondition(-10151, ss);
  }
}

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
QString CreateOnScaleTableFile::getCompiledLibraryName() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
QString CreateOnScaleTableFile::getBrandingString() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
QString CreateOnScaleTableFile::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SimulationIO::Version::Major() << "." << SimulationIO::Version::Minor() << "." << SimulationIO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
QString CreateOnScaleTableFile::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
QString CreateOnScaleTableFile::getSubGroupName() const
{
  return "SimulationIO";
}

// -----------------------------------------------------------------------------
QString CreateOnScaleTableFile::getHumanLabel() const
{
  return "Create OnScale Table File";
}

// -----------------------------------------------------------------------------
QUuid CreateOnScaleTableFile::getUuid() const
{
  return QUuid("{8efc447d-1c92-5ec5-885c-60b4a597835c}");
}

// -----------------------------------------------------------------------------
CreateOnScaleTableFile::Pointer CreateOnScaleTableFile::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
CreateOnScaleTableFile::Pointer CreateOnScaleTableFile::New()
{
  struct make_shared_enabler : public CreateOnScaleTableFile
  {
  };
  return std::make_shared<make_shared_enabler>();
}

// -----------------------------------------------------------------------------
QString CreateOnScaleTableFile::getNameOfClass() const
{
  return ClassName();
}

// -----------------------------------------------------------------------------
QString CreateOnScaleTableFile::ClassName()
{
  return QString("CreateOnScaleTableFile");
}

// -----------------------------------------------------------------------------
QString CreateOnScaleTableFile::getOutputPath() const
{
  return m_OutputPath;
}

// -----------------------------------------------------------------------------
void CreateOnScaleTableFile::setOutputPath(const QString& value)
{
  m_OutputPath = value;
}

// -----------------------------------------------------------------------------
void CreateOnScaleTableFile::setOutputFilePrefix(const QString& value)
{
  m_OutputFilePrefix = value;
}

// -----------------------------------------------------------------------------
QString CreateOnScaleTableFile::getOutputFilePrefix() const
{
  return m_OutputFilePrefix;
}

// -----------------------------------------------------------------------------
void CreateOnScaleTableFile::setPzflexFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_PzflexFeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CreateOnScaleTableFile::getPzflexFeatureIdsArrayPath() const
{
  return m_PzflexFeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void CreateOnScaleTableFile::setNumKeypoints(const IntVec3Type& value)
{
  m_NumKeypoints = value;
}

// -----------------------------------------------------------------------------
IntVec3Type CreateOnScaleTableFile::getNumKeypoints() const
{
  return m_NumKeypoints;
}

// -----------------------------------------------------------------------------
void CreateOnScaleTableFile::setPhaseNamesArrayPath(const DataArrayPath& value)
{
  m_PhaseNamesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CreateOnScaleTableFile::getPhaseNamesArrayPath() const
{
  return m_PhaseNamesArrayPath;
}
