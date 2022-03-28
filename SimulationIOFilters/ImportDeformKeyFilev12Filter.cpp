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
#include "SimulationIO/SimulationIOVersion.h"

namespace fs = std::filesystem;

enum class DataArrayType : uint8_t
{
  VERTEX,
  CELL
};

struct DataArrayMetadata
{
  std::string name;
  size_t tupleCount;
  size_t componentCount;
  DataArrayType type;
};

struct FileCache
{
  std::string inputFile;
  std::vector<DataArrayMetadata> dataArrays;
  size_t vertexAttrMatTupleCount;
  size_t cellAttrMatTupleCount;
  fs::file_time_type timeStamp;

  void flush()
  {
    inputFile.clear();
    dataArrays.clear();
    timeStamp = fs::file_time_type();
  }
};

namespace
{
const std::string k_CompleteStr = "DEFORM Key File: Import Complete";
const std::string k_CanceledStr = "DEFORM Key File: Import Canceled";
const std::string k_IncompleteWithErrorsStr = "DEFORM Key File: Import Incomplete With Errors";
FileCache Cache;
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
    return;
  }

  if(!fs::exists(m_DEFORMInputFile.toStdString()))
  {
    QString ss = QObject::tr("The input file does not exist: '%1'").arg(getDEFORMInputFile());
    setErrorCondition(-388, ss);
    return;
  }

  if(fs::path(m_DEFORMInputFile.toStdString()).extension() != ".key")
  {
    QString ss = QObject::tr("The input file is not a valid DEFORM key file.");
    setErrorCondition(-389, ss);
    return;
  }

  // Create the output Data Container
  DataContainer::Pointer dc = getDataContainerArray()->createNonPrereqDataContainer(this, getDataContainerName());
  if(getErrorCode() < 0)
  {
    return;
  }

  // Create our output Vertex and Cell Matrix objects
  std::vector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer vertexAttrMat = dc->createNonPrereqAttributeMatrix(this, getVertexAttributeMatrixName(), tDims, AttributeMatrix::Type::Vertex);
  if(getErrorCode() < 0)
  {
    return;
  }
  AttributeMatrix::Pointer cellAttrMat = dc->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell);
  if(getErrorCode() < 0)
  {
    return;
  }

  SimulationIO::ImportDeformKeyFilev12InputValues inputValues;
  inputValues.deformInputFile = getDEFORMInputFile().toStdString();
  inputValues.dataContainerName = getDataContainerName().toStdString();
  inputValues.vertexAttributeMatrixName = getVertexAttributeMatrixName().toStdString();
  inputValues.cellAttributeMatrixName = getCellAttributeMatrixName().toStdString();

  // Read from the file if we are executing, the input file has changed, or the input file's time stamp is out of date.
  // Otherwise, read from the cache
  if(!getInPreflight() || getDEFORMInputFile().toStdString() != Cache.inputFile || Cache.timeStamp < fs::last_write_time(getDEFORMInputFile().toStdString()))
  {
    // Read the file
    SimulationIO::ImportDeformKeyFilev12 algorithm(&inputValues, this);
    algorithm.readDEFORMFile(dc.get(), vertexAttrMat.get(), cellAttrMat.get(), !getInPreflight());

    // Cache the results
    std::vector<DataArrayMetadata> dataArrays;

    Cache.vertexAttrMatTupleCount = vertexAttrMat->getNumberOfTuples();
    Cache.cellAttrMatTupleCount = cellAttrMat->getNumberOfTuples();

    for(const auto& vertexArray : *vertexAttrMat)
    {
      dataArrays.push_back({vertexArray->getName().toStdString(), vertexArray->getNumberOfTuples(), static_cast<size_t>(vertexArray->getNumberOfComponents()), DataArrayType::VERTEX});
    }
    for(const auto& cellArray : *cellAttrMat)
    {
      dataArrays.push_back({cellArray->getName().toStdString(), cellArray->getNumberOfTuples(), static_cast<size_t>(cellArray->getNumberOfComponents()), DataArrayType::CELL});
    }

    Cache.inputFile = getDEFORMInputFile().toStdString();
    Cache.dataArrays = dataArrays;
    Cache.timeStamp = fs::last_write_time(getDEFORMInputFile().toStdString());
  }
  else
  {
    // Read from the cache
    setDEFORMInputFile(QString::fromStdString(Cache.inputFile));

    std::vector<size_t> tDims(1, Cache.vertexAttrMatTupleCount);
    vertexAttrMat->resizeAttributeArrays(tDims);

    tDims[0] = Cache.cellAttrMatTupleCount;
    cellAttrMat->resizeAttributeArrays(tDims);

    for(const DataArrayMetadata& daMetadata : Cache.dataArrays)
    {
      std::vector<size_t> cDims(1, static_cast<size_t>(daMetadata.componentCount));
      FloatArrayType::Pointer data = FloatArrayType::CreateArray(daMetadata.tupleCount, cDims, QString::fromStdString(daMetadata.name), false);

      if(daMetadata.type == DataArrayType::VERTEX)
      {
        vertexAttrMat->insertOrAssign(data);
      }
      else if(daMetadata.type == DataArrayType::CELL)
      {
        cellAttrMat->insertOrAssign(data);
      }
      else
      {
        QString msg = QString("Unable to determine the type for cached data array \"%1\".  The type must be either vertex or cell.").arg(QString::fromStdString(daMetadata.name));
        setErrorCondition(-2020, msg);
      }
    }
  }
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12Filter::execute()
{
  clearErrorCode();
  clearWarningCode();

  dataCheck();

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
