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

#include "ExportMultiOnScaleTableFile.h"

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
#include "SimulationIO/SimulationIOFilters/ExportOnScaleTableFile.h"

struct ExportMultiOnScaleTableFile::Impl
{
  std::vector<DataArrayPath> m_FeatureIdsList;

  Impl() = default;

  ~Impl() = default;

  Impl(const Impl&) = delete;
  Impl(Impl&&) = delete;
  Impl& operator=(const Impl&) = delete;
  Impl& operator=(Impl&&) = delete;

  void reset()
  {
    m_FeatureIdsList.clear();
  }
};

// -----------------------------------------------------------------------------
ExportMultiOnScaleTableFile::ExportMultiOnScaleTableFile()
: p_Impl(std::make_unique<Impl>())
, m_NumKeypoints(2, 2, 2)
, m_PhaseNamesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::EnsembleAttributeMatrixName, SIMPL::EnsembleData::PhaseName)
{
  initialize();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
ExportMultiOnScaleTableFile::~ExportMultiOnScaleTableFile() = default;

// -----------------------------------------------------------------------------
void ExportMultiOnScaleTableFile::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
void ExportMultiOnScaleTableFile::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_OUTPUT_PATH_FP("Output Path ", OutputPath, FilterParameter::Parameter, ExportMultiOnScaleTableFile, "*", "*"));
  parameters.push_back(SIMPL_NEW_STRING_FP("Data Container Prefix", DataContainerPrefix, FilterParameter::Parameter, ExportMultiOnScaleTableFile));
  parameters.push_back(SIMPL_NEW_STRING_FP("Matrix Name", MatrixName, FilterParameter::Parameter, ExportMultiOnScaleTableFile));
  parameters.push_back(SIMPL_NEW_STRING_FP("Array Name", ArrayName, FilterParameter::Parameter, ExportMultiOnScaleTableFile));

  parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Number of Keypoints", NumKeypoints, FilterParameter::Parameter, ExportMultiOnScaleTableFile));
  parameters.push_back(SeparatorFilterParameter::New("Ensemble Data", FilterParameter::RequiredArray));

  parameters.push_back(SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Selected Arrays", SelectedArrays, FilterParameter::Category::Parameter, ExportMultiOnScaleTableFile));

  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyPrimitive, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phase Names", PhaseNamesArrayPath, FilterParameter::RequiredArray, ExportMultiOnScaleTableFile, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void ExportMultiOnScaleTableFile::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
void ExportMultiOnScaleTableFile::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  p_Impl->reset();
  m_SelectedArrays.clear();

  if(m_OutputPath.isEmpty())
  {
    QString ss = QObject::tr("The output path must be set");
    setErrorCondition(-10400, ss);
  }

  QDir dir(m_OutputPath);
  if(!dir.exists())
  {
    QString ss = QObject::tr("The directory path for the output file does not exist. DREAM.3D will attempt to create this path during execution of the filter");
    setWarningCondition(-10401, ss);
  }

  auto dca = getDataContainerArray();

  if(dca == nullptr)
  {
    QString ss = QObject::tr("Unable to get DataContainerArray");
    setErrorCondition(-10402, ss);
    return;
  }

  std::vector<size_t> cDims{1};

  StringDataArray::ConstPointer phaseNamesPtr = dca->getPrereqArrayFromPath<StringDataArray>(this, getPhaseNamesArrayPath(), cDims);

  if(phaseNamesPtr == nullptr)
  {
    return;
  }

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
      QString ss = QObject::tr("Unable to obtain AttributeMatrix for '%1'").arg(m_MatrixName);
      setWarningCondition(-10403, ss);
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
    auto dataArray = matrix->getAttributeArray(m_ArrayName);
    if(dataArray == nullptr)
    {
      QString ss = QObject::tr("Unable to obtain DataArray for '%1'").arg(m_ArrayName);
      setWarningCondition(-10404, ss);
      continue;
    }

    if(dataArray->getComponentDimensions() != cDims)
    {
      QString ss = QObject::tr("Wrong number of components for '%1'").arg(m_ArrayName);
      setWarningCondition(-10405, ss);
      continue;
    }

    DataArrayPath path = dataArray->getDataArrayPath();

    p_Impl->m_FeatureIdsList.push_back(path);

    m_SelectedArrays += path.serialize() + "\n";
  }
}

// -----------------------------------------------------------------------------
void ExportMultiOnScaleTableFile::execute()
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
    setErrorCondition(-10406, ss);
    return;
  }

  ExportOnScaleTableFile::Pointer createOnScaleFilter = ExportOnScaleTableFile::New();
  createOnScaleFilter->setDataContainerArray(getDataContainerArray());

  createOnScaleFilter->setOutputPath(m_OutputPath);
  createOnScaleFilter->setNumKeypoints(m_NumKeypoints);
  createOnScaleFilter->setPhaseNamesArrayPath(m_PhaseNamesArrayPath);

  for(const auto& featureIdsPath : p_Impl->m_FeatureIdsList)
  {
    QString dcName = featureIdsPath.getDataContainerName();

    createOnScaleFilter->setOutputFilePrefix(dcName);
    createOnScaleFilter->setPzflexFeatureIdsArrayPath(featureIdsPath);

    createOnScaleFilter->execute();

    int error = createOnScaleFilter->getErrorCode();
    if(error < 0)
    {
      setErrorCondition(-10407, QObject::tr("ExportOnScaleTableFile sub-filter failed with error code %1").arg(error));
      return;
    }
  }
}
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ExportMultiOnScaleTableFile::newFilterInstance(bool copyFilterParameters) const
{
  ExportMultiOnScaleTableFile::Pointer filter = ExportMultiOnScaleTableFile::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
QString ExportMultiOnScaleTableFile::getCompiledLibraryName() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
QString ExportMultiOnScaleTableFile::getBrandingString() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
QString ExportMultiOnScaleTableFile::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SimulationIO::Version::Major() << "." << SimulationIO::Version::Minor() << "." << SimulationIO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
QString ExportMultiOnScaleTableFile::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
QString ExportMultiOnScaleTableFile::getSubGroupName() const
{
  return "SimulationIO";
}

// -----------------------------------------------------------------------------
QString ExportMultiOnScaleTableFile::getHumanLabel() const
{
  return "Create Multi OnScale Table File";
}

// -----------------------------------------------------------------------------
QUuid ExportMultiOnScaleTableFile::getUuid() const
{
  return QUuid("{d5873836-f150-5fc9-9bc8-0bc837bd8dd4}");
}

// -----------------------------------------------------------------------------
ExportMultiOnScaleTableFile::Pointer ExportMultiOnScaleTableFile::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
ExportMultiOnScaleTableFile::Pointer ExportMultiOnScaleTableFile::New()
{
  struct make_shared_enabler : public ExportMultiOnScaleTableFile
  {
  };
  return std::make_shared<make_shared_enabler>();
}

// -----------------------------------------------------------------------------
QString ExportMultiOnScaleTableFile::getNameOfClass() const
{
  return ClassName();
}

// -----------------------------------------------------------------------------
QString ExportMultiOnScaleTableFile::ClassName()
{
  return QString("ExportMultiOnScaleTableFile");
}

// -----------------------------------------------------------------------------
void ExportMultiOnScaleTableFile::setOutputPath(const QString& value)
{
  m_OutputPath = value;
}

// -----------------------------------------------------------------------------
QString ExportMultiOnScaleTableFile::getOutputPath() const
{
  return m_OutputPath;
}

// -----------------------------------------------------------------------------
void ExportMultiOnScaleTableFile::setDataContainerPrefix(const QString& value)
{
  m_DataContainerPrefix = value;
}

// -----------------------------------------------------------------------------
QString ExportMultiOnScaleTableFile::getDataContainerPrefix() const
{
  return m_DataContainerPrefix;
}

// -----------------------------------------------------------------------------
void ExportMultiOnScaleTableFile::setMatrixName(const QString& value)
{
  m_MatrixName = value;
}

// -----------------------------------------------------------------------------
QString ExportMultiOnScaleTableFile::getMatrixName() const
{
  return m_MatrixName;
}

// -----------------------------------------------------------------------------
void ExportMultiOnScaleTableFile::setArrayName(const QString& value)
{
  m_ArrayName = value;
}

// -----------------------------------------------------------------------------
QString ExportMultiOnScaleTableFile::getArrayName() const
{
  return m_ArrayName;
}

// -----------------------------------------------------------------------------
void ExportMultiOnScaleTableFile::setSelectedArrays(const QString& value)
{
  m_SelectedArrays = value;
}

// -----------------------------------------------------------------------------
QString ExportMultiOnScaleTableFile::getSelectedArrays() const
{
  return m_SelectedArrays;
}

// -----------------------------------------------------------------------------
void ExportMultiOnScaleTableFile::setNumKeypoints(const IntVec3Type& value)
{
  m_NumKeypoints = value;
}

// -----------------------------------------------------------------------------
IntVec3Type ExportMultiOnScaleTableFile::getNumKeypoints() const
{
  return m_NumKeypoints;
}

// -----------------------------------------------------------------------------
void ExportMultiOnScaleTableFile::setPhaseNamesArrayPath(const DataArrayPath& value)
{
  m_PhaseNamesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ExportMultiOnScaleTableFile::getPhaseNamesArrayPath() const
{
  return m_PhaseNamesArrayPath;
}
