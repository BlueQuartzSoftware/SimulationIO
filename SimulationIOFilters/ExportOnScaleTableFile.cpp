/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The code contained herein was partially funded by the followig contracts:
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ExportOnScaleTableFile.h"

#include <QtCore/QDir>

#include <Eigen/Dense>

#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/IFilterFactory.hpp"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/RectGridGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "SimulationIO/SimulationIOConstants.h"
#include "SimulationIO/SimulationIOVersion.h"

#include "SimulationIO/SimulationIOFilters/Utility/OnScaleTableFileWriter.h"

namespace
{
std::vector<std::vector<double>> matrixToTable(const Eigen::Matrix3f& matrix)
{
  std::vector<std::vector<double>> tableData;

  for(size_t i = 0; i < 3; i++)
  {
    std::vector<double> row;
    for(size_t j = 0; j < 3; j++)
    {
      row.push_back(matrix(i, j));
    }
    tableData.emplace_back(row);
  }

  return tableData;
}

int rotateData(DataContainerArray::Pointer dca, const Eigen::Matrix3f& rotationMatrix, const DataArrayPath& cellMatrixPath)
{
  const QString filtName = "RotateSampleRefFrame";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
  if(filterFactory == nullptr)
  {
    return 0;
  }

  AbstractFilter::Pointer filter = filterFactory->create();

  if(filter == nullptr)
  {
    return 0;
  }

  filter->setDataContainerArray(dca);

  DynamicTableData table(matrixToTable(rotationMatrix));

  QVariant value;

  value.setValue(1);
  if(!filter->setProperty("RotationRepresentationChoice", value))
  {
    return 0;
  }

  value.setValue(table);
  if(!filter->setProperty("RotationTable", value))
  {
    return 0;
  }

  value.setValue(cellMatrixPath);
  if(!filter->setProperty("CellAttributeMatrixPath", value))
  {
    return 0;
  }

  filter->execute();

  return filter->getErrorCode();
}

template <class T>
bool convertDataArrayPtr(IDataArray::ConstPointer dataArray, std::weak_ptr<const DataArray<T>>& weakPtr, ExportOnScaleTableFile* filter)
{
  auto ptr = std::dynamic_pointer_cast<const DataArray<T>>(dataArray);
  if(ptr == nullptr)
  {
    QString ss = QObject::tr("Unable to get '%1'").arg(dataArray->getDataArrayPath().serialize());
    filter->setErrorCondition(-10109, ss);
    return false;
  }

  weakPtr = ptr;

  return true;
}

enum class Axis : int32_t
{
  X = 0,
  Y = 1,
  Z = 2
};

// Rotate a given number of radians around the given axis
Eigen::Matrix3f createRotationMatrix(float angle, Axis axis)
{
  float cosA = std::cos(angle);
  float sinA = std::sin(angle);

  Eigen::Matrix3f matrix;

  switch(axis)
  {
  case Axis::X: {
    matrix << Eigen::Vector3f{1, 0, 0}, Eigen::Vector3f{0, cosA, sinA}, Eigen::Vector3f{0, -sinA, cosA};
  }
  break;
  case Axis::Y: {
    matrix << Eigen::Vector3f{cosA, 0, -sinA}, Eigen::Vector3f{0, 1, 0}, Eigen::Vector3f{sinA, 0, cosA};
  }
  break;
  case Axis::Z: {
    matrix << Eigen::Vector3f{cosA, sinA, 0}, Eigen::Vector3f{-sinA, cosA, 0}, Eigen::Vector3f{0, 0, 1};
  }
  break;
  default:
    break;
  }

  return matrix;
}

// Find rotation matrix to order dimensions from largest to smallest
Eigen::Matrix3f determineRotationMatrix(const std::array<size_t, 3>& dims)
{
  std::vector<Eigen::Matrix3f> rotationMatrices;

  auto _dims = dims;
  auto dimsSorted = dims;

  std::sort(dimsSorted.begin(), dimsSorted.end(), std::greater<size_t>());

  // Compare against sorted array to determine necessary swaps

  if(_dims[0] != dimsSorted[0])
  {
    if(_dims[1] == dimsSorted[0])
    {
      // Swap x and y
      std::swap(_dims[0], _dims[1]);
      rotationMatrices.emplace_back(createRotationMatrix(SIMPLib::Constants::k_PiOver2D, Axis::Z));
    }
    else
    {
      // Swap x and z
      std::swap(_dims[0], _dims[2]);
      rotationMatrices.emplace_back(createRotationMatrix(SIMPLib::Constants::k_PiOver2D, Axis::Y));
    }
  }

  // x is now correct

  if(_dims[1] != dimsSorted[1])
  {
    // Swap y and z
    std::swap(_dims[1], _dims[2]);
    rotationMatrices.emplace_back(createRotationMatrix(SIMPLib::Constants::k_PiOver2D, Axis::X));
  }

  Eigen::Matrix3f identity = Eigen::Matrix3f::Identity();

  Eigen::Matrix3f matrix = std::accumulate(rotationMatrices.cbegin(), rotationMatrices.cend(), identity, [](const Eigen::Matrix3f& a, const Eigen::Matrix3f& b) -> Eigen::Matrix3f { return b * a; });

  return matrix;
}

template <class T>
bool writeOnScaleFile(std::weak_ptr<const DataArray<T>> featureIdsPtr, const StringDataArray& phaseNames, const QString& outputPath, const QString& outputFilePrefix, const IntVec3Type& numKeypoints,
                      ExportOnScaleTableFile* filter)
{
  auto featureIds = featureIdsPtr.lock();
  if(featureIds == nullptr)
  {
    QString ss = QObject::tr("Error obtaining feature ids data array '%1'").arg(featureIds->getDataArrayPath().serialize());
    filter->setErrorCondition(-10110, ss);
    return false;
  }

  DataArrayPath matrixPath = featureIds->getParentPath();

  DataContainerArray::Pointer dca = filter->getDataContainerArray();

  if(dca == nullptr)
  {
    QString ss = QObject::tr("Error obtaining feature ids data array '%1'").arg(featureIds->getDataArrayPath().serialize());
    filter->setErrorCondition(-10111, ss);
    return false;
  }

  AttributeMatrix::Pointer matrix = dca->getAttributeMatrix(matrixPath);

  if(matrix == nullptr)
  {
    QString ss = QObject::tr("Error obtaining feature ids data array '%1'").arg(featureIds->getDataArrayPath().serialize());
    filter->setErrorCondition(-10112, ss);
    return false;
  }

  DataArrayPath containerPath = matrix->getParentPath();

  DataContainer::Pointer dc = dca->getDataContainer(containerPath);

  if(dc == nullptr)
  {
    QString ss = QObject::tr("Error obtaining feature ids data array '%1'").arg(featureIds->getDataArrayPath().serialize());
    filter->setErrorCondition(-10113, ss);
    return false;
  }

  DataContainerArray::Pointer dcaRotated = DataContainerArray::New();

  DataContainer::Pointer dcRotated = DataContainer::New(dc->getName());
  dcRotated->setGeometry(dc->getGeometry()->deepCopy());

  AttributeMatrix::Pointer matrixRotated = AttributeMatrix::New(matrix->getTupleDimensions(), matrix->getName(), matrix->getType());

  IDataArray::Pointer featureIdsRotatedPtr = featureIds->deepCopy();
  matrixRotated->addOrReplaceAttributeArray(featureIdsRotatedPtr);

  dcRotated->addOrReplaceAttributeMatrix(matrixRotated);

  dcaRotated->addOrReplaceDataContainer(dcRotated);

  std::vector<size_t> tupleDims = matrix->getTupleDimensions();

  if(tupleDims.size() != 3)
  {
    QString ss = QObject::tr("Invalid matrix dims");
    filter->setErrorCondition(-10114, ss);
    return false;
  }

  std::array<size_t, 3> dims;

  std::copy(tupleDims.cbegin(), tupleDims.cend(), dims.begin());

  if(!std::is_sorted(dims.cbegin(), dims.cend(), std::greater<size_t>()))
  {
    Eigen::Matrix3f rotationMatrix = determineRotationMatrix(dims);

    int error = rotateData(dcaRotated, rotationMatrix, matrixPath);

    if(error < 0)
    {
      QString ss = QObject::tr("Data rotation with RotateSampleRefFrame sub-filter failed with error code %1").arg(error);
      filter->setErrorCondition(-10115, ss);
      return false;
    }
  }

  auto featureIdsRotated = matrixRotated->getAttributeArrayAs<DataArray<T>>(featureIdsRotatedPtr->getName());

  if(featureIdsRotated == nullptr)
  {
    QString ss = QObject::tr("Failed to obtain data");
    filter->setErrorCondition(-10116, ss);
    return false;
  }

  IGeometryGrid::Pointer geomRotated = dcRotated->getPrereqGeometry<IGeometryGrid>(filter);
  if(geomRotated == nullptr)
  {
    QString ss = QObject::tr("The data container '%1' does not have a rectilinear grid or image geometry.");
    filter->setErrorCondition(-10117, ss);
    return false;
  }
  auto result = OnScaleTableFileWriter::write(geomRotated, phaseNames, *featureIdsRotated, outputPath, outputFilePrefix, numKeypoints);

  int error = result.first;
  QString errorString = result.second;

  if(error < 0)
  {
    QString ss = QObject::tr("Error writing file at '%1'| %2").arg(outputPath, errorString);
    filter->setErrorCondition(-10118, ss);
    return false;
  }

  return true;
}
} // namespace

struct ExportOnScaleTableFile::Impl
{
  Int8ArrayType::ConstWeakPointer m_FeatureIds8Ptr;
  Int16ArrayType::ConstWeakPointer m_FeatureIds16Ptr;
  Int32ArrayType::ConstWeakPointer m_FeatureIds32Ptr;
  Int64ArrayType::ConstWeakPointer m_FeatureIds64Ptr;

  UInt8ArrayType::ConstWeakPointer m_FeatureIdsU8Ptr;
  UInt16ArrayType::ConstWeakPointer m_FeatureIdsU16Ptr;
  UInt32ArrayType::ConstWeakPointer m_FeatureIdsU32Ptr;
  UInt64ArrayType::ConstWeakPointer m_FeatureIdsU64Ptr;

  QString m_Type;

  StringDataArray::ConstWeakPointer m_PhaseNamesPtr;

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
ExportOnScaleTableFile::ExportOnScaleTableFile()
: p_Impl(std::make_unique<Impl>())
{
  initialize();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
ExportOnScaleTableFile::~ExportOnScaleTableFile() = default;

// -----------------------------------------------------------------------------
void ExportOnScaleTableFile::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
void ExportOnScaleTableFile::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_OUTPUT_PATH_FP("Output Path ", OutputPath, FilterParameter::Category::Parameter, ExportOnScaleTableFile));
  parameters.push_back(SIMPL_NEW_STRING_FP("Output File Prefix", OutputFilePrefix, FilterParameter::Category::Parameter, ExportOnScaleTableFile));

  parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Number of Keypoints", NumKeypoints, FilterParameter::Category::Parameter, ExportOnScaleTableFile));
  parameters.push_back(SeparatorFilterParameter::Create("Ensemble Data", FilterParameter::Category::RequiredArray));

  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyPrimitive, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phase Names", PhaseNamesArrayPath, FilterParameter::Category::RequiredArray, ExportOnScaleTableFile, req));
  }

  parameters.push_back(SeparatorFilterParameter::Create("Cell Data", FilterParameter::Category::RequiredArray));

  {
    QVector<QString> types{SIMPL::TypeNames::Int8,  SIMPL::TypeNames::Int16,  SIMPL::TypeNames::Int32,  SIMPL::TypeNames::Int64,
                           SIMPL::TypeNames::UInt8, SIMPL::TypeNames::UInt16, SIMPL::TypeNames::UInt32, SIMPL::TypeNames::UInt64};
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(types, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", PzflexFeatureIdsArrayPath, FilterParameter::Category::RequiredArray, ExportOnScaleTableFile, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void ExportOnScaleTableFile::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
void ExportOnScaleTableFile::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  p_Impl->resetDataArrays();

  if(m_OutputPath.isEmpty())
  {
    QString ss = QObject::tr("The output path must be set");
    setErrorCondition(-10100, ss);
  }

  QDir dir(m_OutputPath);
  if(!dir.exists())
  {
    QString ss = QObject::tr("The directory path for the output file does not exist. DREAM.3D will attempt to create this path during execution of the filter");
    setWarningCondition(-10101, ss);
  }

  std::vector<size_t> cDims{1};

  StringDataArray::ConstPointer phaseNamesPtr = getDataContainerArray()->getPrereqArrayFromPath<StringDataArray>(this, getPhaseNamesArrayPath(), cDims);

  if(phaseNamesPtr == nullptr)
  {
    return;
  }

  p_Impl->m_PhaseNamesPtr = phaseNamesPtr;

  IDataArray::ConstPointer featureIdsPtr = getDataContainerArray()->getPrereqIDataArrayFromPath(this, getPzflexFeatureIdsArrayPath());

  if(featureIdsPtr == nullptr)
  {
    return;
  }

  if(cDims != featureIdsPtr->getComponentDimensions())
  {
    QString ss = QObject::tr("Wrong component dimensions for '%1'").arg(getPzflexFeatureIdsArrayPath().serialize());
    setErrorCondition(-10104, ss);
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
    setErrorCondition(-10105, ss);
  }
}

// -----------------------------------------------------------------------------
void ExportOnScaleTableFile::execute()
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
    setErrorCondition(-10106, ss);
    return;
  }

  DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(getPzflexFeatureIdsArrayPath().getDataContainerName());
  if(dc == nullptr)
  {
    QString ss = QObject::tr("Error obtaining data container '%1'").arg(m_PzflexFeatureIdsArrayPath.getDataContainerName());
    setErrorCondition(-10107, ss);
    return;
  }

  auto phaseNames = p_Impl->m_PhaseNamesPtr.lock();
  if(phaseNames == nullptr)
  {
    QString ss = QObject::tr("Error obtaining phase names data array '%1'").arg(m_PhaseNamesArrayPath.serialize());
    setErrorCondition(-10109, ss);
    return;
  }

  if(p_Impl->m_Type == SIMPL::TypeNames::Int8)
  {
    if(!writeOnScaleFile(p_Impl->m_FeatureIds8Ptr, *phaseNames, m_OutputPath, m_OutputFilePrefix, m_NumKeypoints, this))
    {
      return;
    }
  }
  else if(p_Impl->m_Type == SIMPL::TypeNames::Int16)
  {
    if(!writeOnScaleFile(p_Impl->m_FeatureIds16Ptr, *phaseNames, m_OutputPath, m_OutputFilePrefix, m_NumKeypoints, this))
    {
      return;
    }
  }
  else if(p_Impl->m_Type == SIMPL::TypeNames::Int32)
  {
    if(!writeOnScaleFile(p_Impl->m_FeatureIds32Ptr, *phaseNames, m_OutputPath, m_OutputFilePrefix, m_NumKeypoints, this))
    {
      return;
    }
  }
  else if(p_Impl->m_Type == SIMPL::TypeNames::Int64)
  {
    if(!writeOnScaleFile(p_Impl->m_FeatureIds64Ptr, *phaseNames, m_OutputPath, m_OutputFilePrefix, m_NumKeypoints, this))
    {
      return;
    }
  }
  else if(p_Impl->m_Type == SIMPL::TypeNames::UInt8)
  {
    if(!writeOnScaleFile(p_Impl->m_FeatureIdsU8Ptr, *phaseNames, m_OutputPath, m_OutputFilePrefix, m_NumKeypoints, this))
    {
      return;
    }
  }
  else if(p_Impl->m_Type == SIMPL::TypeNames::UInt16)
  {
    if(!writeOnScaleFile(p_Impl->m_FeatureIdsU16Ptr, *phaseNames, m_OutputPath, m_OutputFilePrefix, m_NumKeypoints, this))
    {
      return;
    }
  }
  else if(p_Impl->m_Type == SIMPL::TypeNames::UInt32)
  {
    if(!writeOnScaleFile(p_Impl->m_FeatureIdsU32Ptr, *phaseNames, m_OutputPath, m_OutputFilePrefix, m_NumKeypoints, this))
    {
      return;
    }
  }
  else if(p_Impl->m_Type == SIMPL::TypeNames::UInt64)
  {
    if(!writeOnScaleFile(p_Impl->m_FeatureIdsU64Ptr, *phaseNames, m_OutputPath, m_OutputFilePrefix, m_NumKeypoints, this))
    {
      return;
    }
  }
  else
  {
    QString ss = QObject::tr("Invalid type of '%1' for '%2'. Must be an integer type").arg(p_Impl->m_Type).arg(getPzflexFeatureIdsArrayPath().serialize());
    setErrorCondition(-10110, ss);
  }
}

// -----------------------------------------------------------------------------
AbstractFilter::Pointer ExportOnScaleTableFile::newFilterInstance(bool copyFilterParameters) const
{
  ExportOnScaleTableFile::Pointer filter = ExportOnScaleTableFile::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
QString ExportOnScaleTableFile::getCompiledLibraryName() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
QString ExportOnScaleTableFile::getBrandingString() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
QString ExportOnScaleTableFile::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SimulationIO::Version::Major() << "." << SimulationIO::Version::Minor() << "." << SimulationIO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
QString ExportOnScaleTableFile::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
QString ExportOnScaleTableFile::getSubGroupName() const
{
  return "SimulationIO";
}

// -----------------------------------------------------------------------------
QString ExportOnScaleTableFile::getHumanLabel() const
{
  return "Create OnScale Table File";
}

// -----------------------------------------------------------------------------
QUuid ExportOnScaleTableFile::getUuid() const
{
  return QUuid("{8efc447d-1c92-5ec5-885c-60b4a597835c}");
}

// -----------------------------------------------------------------------------
ExportOnScaleTableFile::Pointer ExportOnScaleTableFile::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
ExportOnScaleTableFile::Pointer ExportOnScaleTableFile::New()
{
  struct make_shared_enabler : public ExportOnScaleTableFile
  {
  };
  return std::make_shared<make_shared_enabler>();
}

// -----------------------------------------------------------------------------
QString ExportOnScaleTableFile::getNameOfClass() const
{
  return ClassName();
}

// -----------------------------------------------------------------------------
QString ExportOnScaleTableFile::ClassName()
{
  return QString("ExportOnScaleTableFile");
}

// -----------------------------------------------------------------------------
QString ExportOnScaleTableFile::getOutputPath() const
{
  return m_OutputPath;
}

// -----------------------------------------------------------------------------
void ExportOnScaleTableFile::setOutputPath(const QString& value)
{
  m_OutputPath = value;
}

// -----------------------------------------------------------------------------
void ExportOnScaleTableFile::setOutputFilePrefix(const QString& value)
{
  m_OutputFilePrefix = value;
}

// -----------------------------------------------------------------------------
QString ExportOnScaleTableFile::getOutputFilePrefix() const
{
  return m_OutputFilePrefix;
}

// -----------------------------------------------------------------------------
void ExportOnScaleTableFile::setPzflexFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_PzflexFeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ExportOnScaleTableFile::getPzflexFeatureIdsArrayPath() const
{
  return m_PzflexFeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void ExportOnScaleTableFile::setNumKeypoints(const IntVec3Type& value)
{
  m_NumKeypoints = value;
}

// -----------------------------------------------------------------------------
IntVec3Type ExportOnScaleTableFile::getNumKeypoints() const
{
  return m_NumKeypoints;
}

// -----------------------------------------------------------------------------
void ExportOnScaleTableFile::setPhaseNamesArrayPath(const DataArrayPath& value)
{
  m_PhaseNamesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ExportOnScaleTableFile::getPhaseNamesArrayPath() const
{
  return m_PhaseNamesArrayPath;
}
