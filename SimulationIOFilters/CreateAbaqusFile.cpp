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

#include "CreateAbaqusFile.h"

#include <QtCore/QDir>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
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
  Int32ArrayType::ConstWeakPointer m_FeatureIdsPtr;
  Int32ArrayType::ConstWeakPointer m_CellPhasesPtr;
  FloatArrayType::ConstWeakPointer m_CellEulerAnglesPtr;

  Impl() = default;

  ~Impl() = default;

  Impl(const Impl&) = delete;
  Impl(Impl&&) = delete;
  Impl& operator=(const Impl&) = delete;
  Impl& operator=(Impl&&) = delete;

  void resetDataArrays()
  {
    m_FeatureIdsPtr.reset();
    m_CellPhasesPtr.reset();
    m_CellEulerAnglesPtr.reset();
  }
};

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
CreateAbaqusFile::~CreateAbaqusFile() = default;

// -----------------------------------------------------------------------------
void CreateAbaqusFile::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

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
void CreateAbaqusFile::dataCheck()
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
QString CreateAbaqusFile::getCompiledLibraryName() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
QString CreateAbaqusFile::getBrandingString() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
QString CreateAbaqusFile::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SimulationIO::Version::Major() << "." << SimulationIO::Version::Minor() << "." << SimulationIO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
QString CreateAbaqusFile::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
QString CreateAbaqusFile::getSubGroupName() const
{
  return "SimulationIO";
}

// -----------------------------------------------------------------------------
QString CreateAbaqusFile::getHumanLabel() const
{
  return "Create Abaqus File";
}

// -----------------------------------------------------------------------------
QUuid CreateAbaqusFile::getUuid() const
{
  return QUuid("{d702beff-eb02-5ee1-a76a-79d5b56ec730}");
}

// -----------------------------------------------------------------------------
CreateAbaqusFile::Pointer CreateAbaqusFile::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
CreateAbaqusFile::Pointer CreateAbaqusFile::New()
{
  struct make_shared_enabler : public CreateAbaqusFile
  {
  };
  return std::make_shared<make_shared_enabler>();
}

// -----------------------------------------------------------------------------
QString CreateAbaqusFile::getNameOfClass() const
{
  return ClassName();
}

// -----------------------------------------------------------------------------
QString CreateAbaqusFile::ClassName()
{
  return QString("CreateAbaqusFile");
}

// -----------------------------------------------------------------------------
void CreateAbaqusFile::setOutputPath(const QString& value)
{
  m_OutputPath = value;
}

// -----------------------------------------------------------------------------
QString CreateAbaqusFile::getOutputPath() const
{
  return m_OutputPath;
}

// -----------------------------------------------------------------------------
void CreateAbaqusFile::setOutputFilePrefix(const QString& value)
{
  m_OutputFilePrefix = value;
}

// -----------------------------------------------------------------------------
QString CreateAbaqusFile::getOutputFilePrefix() const
{
  return m_OutputFilePrefix;
}

// -----------------------------------------------------------------------------
void CreateAbaqusFile::setJobName(const QString& value)
{
  m_JobName = value;
}

// -----------------------------------------------------------------------------
QString CreateAbaqusFile::getJobName() const
{
  return m_JobName;
}

// -----------------------------------------------------------------------------
void CreateAbaqusFile::setNumDepvar(int value)
{
  m_NumDepvar = value;
}

// -----------------------------------------------------------------------------
int CreateAbaqusFile::getNumDepvar() const
{
  return m_NumDepvar;
}

// -----------------------------------------------------------------------------
void CreateAbaqusFile::setNumUserOutVar(int value)
{
  m_NumUserOutVar = value;
}

// -----------------------------------------------------------------------------
int CreateAbaqusFile::getNumUserOutVar() const
{
  return m_NumUserOutVar;
}

// -----------------------------------------------------------------------------
void CreateAbaqusFile::setMatConst(const DynamicTableData& value)
{
  m_MatConst = value;
}

// -----------------------------------------------------------------------------
DynamicTableData CreateAbaqusFile::getMatConst() const
{
  return m_MatConst;
}

// -----------------------------------------------------------------------------
void CreateAbaqusFile::setAbqFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_AbqFeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CreateAbaqusFile::getAbqFeatureIdsArrayPath() const
{
  return m_AbqFeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void CreateAbaqusFile::setCellEulerAnglesArrayPath(const DataArrayPath& value)
{
  m_CellEulerAnglesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CreateAbaqusFile::getCellEulerAnglesArrayPath() const
{
  return m_CellEulerAnglesArrayPath;
}

// -----------------------------------------------------------------------------
void CreateAbaqusFile::setCellPhasesArrayPath(const DataArrayPath& value)
{
  m_CellPhasesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CreateAbaqusFile::getCellPhasesArrayPath() const
{
  return m_CellPhasesArrayPath;
}
