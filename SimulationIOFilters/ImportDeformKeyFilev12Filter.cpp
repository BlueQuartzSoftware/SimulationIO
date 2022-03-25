/*
 * Your License or Copyright can go here
 */

#include "ImportDeformKeyFilev12Filter.h"

#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#include "SimulationIO/SimulationIOConstants.h"
#include "SimulationIO/SimulationIOFilters/Algorithms/ImportDeformKeyFilev12.h"
#include "SimulationIO/SimulationIOVersion.h"

namespace fs = std::filesystem;

namespace
{
const std::string k_CompleteStr = "DEFORM Key File: Import Complete";
const std::string k_CanceledStr = "DEFORM Key File: Import Canceled";
const std::string k_IncompleteWithErrorsStr = "DEFORM Key File: Import Incomplete With Errors";
} // namespace

// -----------------------------------------------------------------------------
ImportDeformKeyFilev12Filter::ImportDeformKeyFilev12Filter() = default;

// -----------------------------------------------------------------------------
ImportDeformKeyFilev12Filter::~ImportDeformKeyFilev12Filter() = default;

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12Filter::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  {
    parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", DEFORMInputFile, FilterParameter::Category::Parameter, ImportDeformKeyFilev12Filter, "", "*.DAT", 2));
  }

  parameters.push_back(SIMPL_NEW_STRING_FP("Data Container Name", DataContainerName, FilterParameter::Category::CreatedArray, ImportDeformKeyFilev12Filter));
  parameters.push_back(SIMPL_NEW_STRING_FP("Vertex Attribute Matrix Name", VertexAttributeMatrixName, FilterParameter::Category::CreatedArray, ImportDeformKeyFilev12Filter));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix Name", CellAttributeMatrixName, FilterParameter::Category::CreatedArray, ImportDeformKeyFilev12Filter));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12Filter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDEFORMInputFile(reader->readString("InputFile", getDEFORMInputFile()));
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName()));
  setVertexAttributeMatrixName(reader->readString("VertexAttributeMatrixName", getVertexAttributeMatrixName()));
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12Filter::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);

  if(m_DEFORMInputFile.toStdString().empty())
  {
    QString ss = QObject::tr("The input file must be set for property %1").arg("InputFile");
    setErrorCondition(-1, ss);
  }

  if(!fs::exists(m_DEFORMInputFile.toStdString()))
  {
    QString ss = QObject::tr("The input file does not exist: '%1'").arg(getDEFORMInputFile());
    setErrorCondition(-388, ss);
  }

  // Create the output Data Container
  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer(this, getDataContainerName());
  if(getErrorCode() < 0)
  {
    return;
  }

  // Create our output Vertex and Cell Matrix objects
  std::vector<size_t> tDims(1, 0);
  m->createNonPrereqAttributeMatrix(this, getVertexAttributeMatrixName(), tDims, AttributeMatrix::Type::Vertex);
  if(getErrorCode() < 0)
  {
    return;
  }
  m->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell);
  if(getErrorCode() < 0)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12Filter::execute()
{
  clearErrorCode();
  clearWarningCode();

  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  AttributeMatrix::Pointer vertexAttrMat = m->getAttributeMatrix(getVertexAttributeMatrixName());
  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());

  SimulationIO::ImportDeformKeyFilev12InputValues inputValues;
  inputValues.deformInputFile = getDEFORMInputFile().toStdString();
  inputValues.dataContainerName = getDataContainerName().toStdString();
  inputValues.vertexAttributeMatrixName = getVertexAttributeMatrixName().toStdString();
  inputValues.cellAttributeMatrixName = getCellAttributeMatrixName().toStdString();

  SimulationIO::ImportDeformKeyFilev12(*getDataContainerArray(), &inputValues, this)();

  if(getErrorCode() > 0)
  {
    notifyStatusMessage(QString::fromStdString(k_IncompleteWithErrorsStr));
  }
  else if(getCancel())
  {
    notifyStatusMessage(QString::fromStdString(k_CanceledStr));
  }
  else
  {
    notifyStatusMessage(QString::fromStdString(k_CompleteStr));
  }
}

// -----------------------------------------------------------------------------
AbstractFilter::Pointer ImportDeformKeyFilev12Filter::newFilterInstance(bool copyFilterParameters) const
{
  ImportDeformKeyFilev12Filter::Pointer filter = ImportDeformKeyFilev12Filter::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12Filter::getCompiledLibraryName() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12Filter::getBrandingString() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12Filter::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SimulationIO::Version::Major() << "." << SimulationIO::Version::Minor() << "." << SimulationIO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12Filter::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12Filter::getSubGroupName() const
{
  return "SimulationIO";
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12Filter::getHumanLabel() const
{
  return "Import Deform Key File (v12)";
}

// -----------------------------------------------------------------------------
QUuid ImportDeformKeyFilev12Filter::getUuid() const
{
  return QUuid("{3c6337da-e232-4420-a5ca-451496748d88}");
}

// -----------------------------------------------------------------------------
ImportDeformKeyFilev12Filter::Pointer ImportDeformKeyFilev12Filter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ImportDeformKeyFilev12Filter> ImportDeformKeyFilev12Filter::New()
{
  struct make_shared_enabler : public ImportDeformKeyFilev12Filter
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12Filter::getNameOfClass() const
{
  return QString("ImportDeformKeyFilev12Filter");
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12Filter::ClassName()
{
  return QString("ImportDeformKeyFilev12Filter");
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12Filter::setDEFORMInputFile(const QString& value)
{
  m_DEFORMInputFile = value;
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12Filter::getDEFORMInputFile() const
{
  return m_DEFORMInputFile;
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12Filter::setDataContainerName(const QString& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12Filter::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12Filter::setVertexAttributeMatrixName(const QString& value)
{
  m_VertexAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12Filter::getVertexAttributeMatrixName() const
{
  return m_VertexAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12Filter::setCellAttributeMatrixName(const QString& value)
{
  m_CellAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12Filter::getCellAttributeMatrixName() const
{
  return m_CellAttributeMatrixName;
}
