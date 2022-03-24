/*
 * Your License or Copyright can go here
 */

#include "ImportDeformKeyFilev12.h"



#include <fstream>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Utilities/StringUtilities.hpp"

#include "SimulationIO/SimulationIOConstants.h"
#include "SimulationIO/SimulationIOVersion.h"

#define READ_DEF_PT_TRACKING_TIME_INDEX "Time Index"

namespace
{
const std::string k_CompleteStr = "DEFORM Key File: Import Complete";
const std::string k_CanceledStr = "DEFORM Key File: Import Canceled";
const std::string k_IncompleteWithErrorsStr = "DEFORM Key File: Import Incomplete With Errors";
} // namespace

// -----------------------------------------------------------------------------
ImportDeformKeyFilev12::ImportDeformKeyFilev12() = default;

// -----------------------------------------------------------------------------
ImportDeformKeyFilev12::~ImportDeformKeyFilev12() = default;

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  {
    parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", DEFORMInputFile, FilterParameter::Category::Parameter, ImportDeformKeyFilev12, "", "*.DAT", 2));
  }

  parameters.push_back(SIMPL_NEW_STRING_FP("Data Container Name", DataContainerName, FilterParameter::Category::CreatedArray, ImportDeformKeyFilev12));
  parameters.push_back(SIMPL_NEW_STRING_FP("Vertex Attribute Matrix Name", VertexAttributeMatrixName, FilterParameter::Category::CreatedArray, ImportDeformKeyFilev12));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix Name", CellAttributeMatrixName, FilterParameter::Category::CreatedArray, ImportDeformKeyFilev12));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDEFORMInputFile(reader->readString("InputFile", getDEFORMInputFile()));
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName()));
  setVertexAttributeMatrixName(reader->readString("VertexAttributeMatrixName", getVertexAttributeMatrixName()));
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);

  if(!fs::exists(m_DEFORMInputFile.toStdString()))
  {
    QString ss = QObject::tr("The input file does not exist: '%1'").arg(getDEFORMInputFile());
    setErrorCondition(-388, ss);
  }

  if(fs::is_empty(m_DEFORMInputFile.toStdString()))
  {
    QString ss = QObject::tr("The input file must be set for property %1").arg("InputFile");
    setErrorCondition(-1, ss);
  }

  // Create the output Data Container
  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer(this, getDataContainerName());
  if(getErrorCode() < 0)
  {
    return;
  }

  // Create our output Vertex and Cell Matrix objects
  std::vector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer vertexAttrMat = m->createNonPrereqAttributeMatrix(this, getVertexAttributeMatrixName(), tDims, AttributeMatrix::Type::Vertex);
  if(getErrorCode() < 0)
  {
    return;
  }
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell);
  if(getErrorCode() < 0)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::execute()
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

  scanDEFORMFile(m.get(), vertexAttrMat.get(), cellAttrMat.get());

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
void ImportDeformKeyFilev12::scanDEFORMFile(DataContainer* dataContainer, AttributeMatrix* vertexAttributeMatrix, AttributeMatrix* cellAttributeMatrix)
{
  std::ifstream inStream(getDEFORMInputFile().toStdString());
  size_t lineCount = 0;

  std::string word;
  std::string buf;
  std::vector<std::string> tokens; /* vector to store the split data */

  SharedVertexList::Pointer vertexPtr = SharedVertexList::NullPointer();
  QuadGeom::Pointer quadGeomPtr = QuadGeom::NullPointer();

  while(inStream.peek() != EOF)
  {
    if(getCancel())
    {
      return;
    }

    tokens = getNextLineTokens(inStream);
    lineCount++;

    if(tokens.empty())
    {
      continue;
    }

    word = tokens.at(0);

    if(word == "RZ")
    {
      size_t numVerts = parse_ull(tokens.at(2), lineCount);
      if(getErrorCode() < 0)
      {
        return;
      }

      vertexPtr = readVertexCoordinates(inStream, lineCount, vertexAttributeMatrix, numVerts);
    }
    else if(word == "ELMCON")
    {
      size_t numCells = parse_ull(tokens.at(2), lineCount);
      if(getErrorCode() < 0)
      {
        return;
      }

      quadGeomPtr = readQuadGeometry(inStream, lineCount, cellAttributeMatrix, vertexPtr, dataContainer, numCells);
    }
    else if(word == "*")
    {
      // This is a comment
      continue;
    }
    else if(tokens.size() >= 3 && (vertexPtr != SharedVertexList::NullPointer() || quadGeomPtr != QuadGeom::NullPointer()))
    {
      // This is most likely the beginning of a data array
      std::string dataArrayName = tokens.at(0);
      size_t tupleCount = parse_ull(tokens.at(2), lineCount);
      if(getErrorCode() < 0)
      {
        return;
      }

      if(vertexPtr != SharedVertexList::NullPointer() && tupleCount == vertexPtr->getNumberOfTuples())
      {
        QString statusMsg = QString("Reading Vertex Data: %1").arg(QString::fromStdString(dataArrayName));
        notifyStatusMessage(statusMsg);

        readDataArray(inStream, lineCount, vertexAttributeMatrix, dataArrayName, tupleCount);
      }
      else if(quadGeomPtr != QuadGeom::NullPointer() && tupleCount == quadGeomPtr->getNumberOfQuads())
      {
        QString statusMsg = QString("Reading Cell Data: %1").arg(QString::fromStdString(dataArrayName));
        notifyStatusMessage(statusMsg);

        readDataArray(inStream, lineCount, cellAttributeMatrix, dataArrayName, tupleCount);
      }
    }

    if(getErrorCode() < 0)
    {
      return;
    }
  }
}

// -----------------------------------------------------------------------------
SharedVertexList::Pointer ImportDeformKeyFilev12::readVertexCoordinates(std::ifstream& inStream, size_t& lineCount, AttributeMatrix* vertexAttrMat, size_t numVerts)
{
  std::string buf;
  std::vector<std::string> tokens; /* vector to store the split data */

  // Set the number of vertices and then create vertices array and resize vertex attr mat.
  std::vector<size_t> tDims(1, numVerts);
  vertexAttrMat->resizeAttributeArrays(tDims);

  SharedVertexList::Pointer vertexPtr = QuadGeom::CreateSharedVertexList(static_cast<int64_t>(numVerts), true);
  float* vertex = vertexPtr->getPointer(0);
  QString statusMsg = QString("DEFORM Data File: Number of Vertex Points = %1").arg(QString::number(numVerts));
  notifyStatusMessage(statusMsg);

  // Read or Skip past all the vertex data
  for(size_t i = 0; i < numVerts; i++)
  {
    if(getCancel())
    {
      return SharedVertexList::NullPointer();
    }

    tokens = getNextLineTokens(inStream);
    lineCount++;

    try
    {
      vertex[3 * i] = std::stof(tokens[1]);
    } catch(const std::exception& e)
    {
      QString msg = QString("Error at line %1: Unable to convert vertex coordinate %2's 1st string value \"%3\" to float.  Threw standard exception with text: \"%4\"")
                        .arg(QString::number(lineCount), QString::number(i + 1), QString::fromStdString(tokens[1]), e.what());
      setErrorCondition(-2001, msg);
      return SharedVertexList::NullPointer();
    }

    try
    {
      vertex[3 * i + 1] = std::stof(tokens[2]);
    } catch(const std::exception& e)
    {
      QString msg = QString("Error at line %1: Unable to convert vertex coordinate %2's 2nd string value \"%3\" to float.  Threw standard exception with text: \"%4\"")
                        .arg(QString::number(lineCount), QString::number(i + 1), QString::fromStdString(tokens[2]), e.what());
      setErrorCondition(-2002, msg);
      return SharedVertexList::NullPointer();
    }

    vertex[3 * i + 2] = 0.0f;
  }

  return vertexPtr;
}

// -----------------------------------------------------------------------------
QuadGeom::Pointer ImportDeformKeyFilev12::readQuadGeometry(std::ifstream& inStream, size_t& lineCount, AttributeMatrix* cellAttrMat, SharedVertexList::Pointer vertexPtr, DataContainer* dataContainer,
                                                           size_t numCells)
{
  std::string buf;
  std::vector<std::string> tokens; /* vector to store the split data */

  // Set the number of cells and then create cells array and resize cell attr mat.
  std::vector<size_t> tDims(1, numCells);
  QString statusMsg = QString("DEFORM Data File: Number of Quad Cells = %1").arg(QString::number(numCells));
  notifyStatusMessage(statusMsg);
  cellAttrMat->resizeAttributeArrays(tDims);
  QuadGeom::Pointer quadGeomPtr = QuadGeom::CreateGeometry(static_cast<int64_t>(numCells), vertexPtr, SIMPL::Geometry::QuadGeometry, true);
  quadGeomPtr->setSpatialDimensionality(2);
  dataContainer->setGeometry(quadGeomPtr);
  MeshIndexType* quads = quadGeomPtr->getQuadPointer(0);

  for(size_t i = 0; i < numCells; i++)
  {
    if(getCancel())
    {
      return QuadGeom::NullPointer();
    }

    tokens = getNextLineTokens(inStream);
    lineCount++;

    // Subtract one from the node number because DEFORM starts at node 1 and we start at node 0
    try
    {
      quads[4 * i] = std::stoi(tokens[1]) - 1;
    } catch(const std::exception& e)
    {
      QString msg = QString("Error at line %1: Unable to convert quad %2's 1st string value \"%3\" to integer.  Threw standard exception with text: \"%4\"")
                        .arg(QString::number(lineCount), QString::number(i + 1), QString::fromStdString(tokens[1]), e.what());
      setErrorCondition(-2004, msg);
      return QuadGeom::NullPointer();
    }

    try
    {
      quads[4 * i + 1] = std::stoi(tokens[2]) - 1;
    } catch(const std::exception& e)
    {
      QString msg = QString("Error at line %1: Unable to convert quad %2's 2nd string value \"%3\" to integer.  Threw standard exception with text: \"%4\"")
                        .arg(QString::number(lineCount), QString::number(i + 1), QString::fromStdString(tokens[2]), e.what());
      setErrorCondition(-2005, msg);
      return QuadGeom::NullPointer();
    }

    try
    {
      quads[4 * i + 2] = std::stoi(tokens[3]) - 1;
    } catch(const std::exception& e)
    {
      QString msg = QString("Error at line %1: Unable to convert quad %2's 3rd string value \"%3\" to integer.  Threw standard exception with text: \"%4\"")
                        .arg(QString::number(lineCount), QString::number(i + 1), QString::fromStdString(tokens[3]), e.what());
      setErrorCondition(-2005, msg);
      return QuadGeom::NullPointer();
    }

    try
    {
      quads[4 * i + 3] = std::stoi(tokens[4]) - 1;
    } catch(const std::exception& e)
    {
      QString msg = QString("Error at line %1: Unable to convert quad %2's 4th string value \"%3\" to integer.  Threw standard exception with text: \"%4\"")
                        .arg(QString::number(lineCount), QString::number(i + 1), QString::fromStdString(tokens[4]), e.what());
      setErrorCondition(-2006, msg);
      return QuadGeom::NullPointer();
    }
  }

  return quadGeomPtr;
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::readDataArray(std::ifstream& inStream, size_t& lineCount, AttributeMatrix* attrMat, const std::string& dataArrayName, size_t arrayTupleSize)
{
  FloatArrayType::Pointer data = FloatArrayType::NullPointer();
  for(size_t i = 0; i < arrayTupleSize; i++)
  {
    if(getCancel())
    {
      return;
    }

    std::vector<std::string> tokens = getNextLineTokens(inStream);
    lineCount++;

    if(i == 0)
    {
      int32_t numComp = tokens.size() - 1;
      std::vector<size_t> cDims(1, static_cast<size_t>(numComp));
      data = FloatArrayType::CreateArray(arrayTupleSize, cDims, QString::fromStdString(dataArrayName), true);
      attrMat->insertOrAssign(data);
    }

    for(int32_t c = 0; c < data->getNumberOfComponents(); c++)
    {
      float value = 0.0f;
      try
      {
        value = std::stof(tokens[c + 1]);
      } catch(const std::exception& e)
      {
        QString msg = QString("Error at line %1: Unable to convert data array %2's string value \"%3\" to float.  Threw standard exception with text: \"%4\"")
                          .arg(QString::number(lineCount), data->getName(), QString::fromStdString(tokens[c + 1]), e.what());
        setErrorCondition(-2008, msg);
        return;
      }
      data->setComponent(i, c, value);
    }
  }
}

// -----------------------------------------------------------------------------
std::vector<std::string> ImportDeformKeyFilev12::getNextLineTokens(std::ifstream& inStream)
{
  std::vector<std::string> tokens; /* vector to store the split data */
  std::string buf;
  std::getline(inStream, buf);
  buf = SIMPL::StringUtilities::trimmed(buf);
  buf = SIMPL::StringUtilities::simplified(buf);
  tokens = SIMPL::StringUtilities::split(buf, ' ');

  return tokens;
}

// -----------------------------------------------------------------------------
size_t ImportDeformKeyFilev12::parse_ull(const std::string& token, size_t lineCount)
{
  size_t value = 0;
  try
  {
    value = std::stoull(token);
  } catch(const std::exception& e)
  {
    QString msg = QString("Error at line %1: Unable to convert string value \"%2\" to unsigned long long.  Threw standard exception with text: \"%3\"")
                      .arg(QString::number(lineCount), QString::fromStdString(token), e.what());
    setErrorCondition(-2000, msg);
    return 0;
  }

  return value;
}

// -----------------------------------------------------------------------------
size_t ImportDeformKeyFilev12::parseTotalQuads(const std::vector<std::string>& tokens, size_t lineCount)
{
  size_t numCells = 0;
  try
  {
    numCells = std::stoull(tokens.at(2));
  } catch(const std::exception& e)
  {
    QString msg = QString("Error at line %1: Unable to convert total quads string value \"%2\" to unsigned long long.  Threw standard exception with text: \"%3\"")
                      .arg(QString::number(lineCount), QString::fromStdString(tokens.at(2)), e.what());
    setErrorCondition(-2003, msg);
    return 0;
  }

  return numCells;
}

// -----------------------------------------------------------------------------
AbstractFilter::Pointer ImportDeformKeyFilev12::newFilterInstance(bool copyFilterParameters) const
{
  ImportDeformKeyFilev12::Pointer filter = ImportDeformKeyFilev12::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12::getCompiledLibraryName() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12::getBrandingString() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SimulationIO::Version::Major() << "." << SimulationIO::Version::Minor() << "." << SimulationIO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12::getSubGroupName() const
{
  return "SimulationIO";
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12::getHumanLabel() const
{
  return "Import Deform Key File (v12)";
}

// -----------------------------------------------------------------------------
QUuid ImportDeformKeyFilev12::getUuid() const
{
  return QUuid("{3c6337da-e232-4420-a5ca-451496748d88}");
}

// -----------------------------------------------------------------------------
ImportDeformKeyFilev12::Pointer ImportDeformKeyFilev12::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ImportDeformKeyFilev12> ImportDeformKeyFilev12::New()
{
  struct make_shared_enabler : public ImportDeformKeyFilev12
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12::getNameOfClass() const
{
  return QString("ImportDeformKeyFilev12");
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12::ClassName()
{
  return QString("ImportDeformKeyFilev12");
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::setDEFORMInputFile(const QString& value)
{
  m_DEFORMInputFile = value;
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12::getDEFORMInputFile() const
{
  return m_DEFORMInputFile;
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::setDataContainerName(const QString& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::setVertexAttributeMatrixName(const QString& value)
{
  m_VertexAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12::getVertexAttributeMatrixName() const
{
  return m_VertexAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::setCellAttributeMatrixName(const QString& value)
{
  m_CellAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString ImportDeformKeyFilev12::getCellAttributeMatrixName() const
{
  return m_CellAttributeMatrixName;
}
