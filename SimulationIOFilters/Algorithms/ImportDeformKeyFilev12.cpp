#include "ImportDeformKeyFilev12.h"

#include <QtCore/QDateTime>

#include <fstream>

#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Utilities/StringUtilities.hpp"

#include "SimulationIO/SimulationIOFilters/ImportDeformKeyFilev12Filter.h"

using namespace SimulationIO;

namespace
{
const std::vector<std::string> k_FileTitle = {"DEFORM-2D", "KEYWORD", "FILE", "(Qt)"};
const std::vector<std::string> k_ProcessDefinition = {"Process", "Definition"};
const std::vector<std::string> k_StoppingAndStepControls = {"Stopping", "&", "Step", "Controls"};
const std::vector<std::string> k_IterationControls = {"Iteration", "Controls"};
const std::vector<std::string> k_ProcessingConditions = {"Processing", "Conditions"};
const std::vector<std::string> k_UserDefinedVariables = {"User", "Defined", "Variables"};
const std::vector<std::string> k_PropertyDataOfMaterial = {"Property", "Data", "of", "Material"};
const std::vector<std::string> k_InterMaterialData = {"Inter-Material", "Data"};
const std::vector<std::string> k_InterObjectData = {"Inter-Object", "Data"};
const std::vector<std::string> k_DataForObject = {"Data", "for", "Object", "#"};
} // namespace

// -----------------------------------------------------------------------------
ImportDeformKeyFilev12::ImportDeformKeyFilev12(ImportDeformKeyFilev12InputValues* inputValues, ImportDeformKeyFilev12Filter* filter)
: m_InputValues(inputValues)
, m_Filter(filter)
{
}

// -----------------------------------------------------------------------------
ImportDeformKeyFilev12::~ImportDeformKeyFilev12() noexcept = default;

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::readDEFORMFile(DataContainer* dataContainer, AttributeMatrix* vertexAttributeMatrix, AttributeMatrix* cellAttributeMatrix, bool allocate)
{
  std::ifstream inStream(m_InputValues->deformInputFile);
  size_t lineCount = 0;

  std::string word;
  std::string buf;
  std::vector<std::string> tokens; /* vector to store the split data */

  SharedVertexList::Pointer vertexPtr = SharedVertexList::NullPointer();
  QuadGeom::Pointer quadGeomPtr = QuadGeom::NullPointer();

  while(inStream.peek() != EOF)
  {
    if(shouldCancel())
    {
      return;
    }

    tokens = getNextLineTokens(inStream, lineCount);

    if(tokens.size() <= 1)
    {
      // This is either an empty line or a singular star comment
      continue;
    }

    // Erase the star
    tokens.erase(tokens.begin());

    if(tokens == k_ProcessDefinition)
    {
      // Skip the comment line that ends the header
      getNextLineTokens(inStream, lineCount);

      // Read "Process Definition" section
      readProcessDefinition(inStream, lineCount);
      continue;
    }
    else if(tokens == k_StoppingAndStepControls)
    {
      // Skip the comment line that ends the header
      getNextLineTokens(inStream, lineCount);

      // Read "Stopping & Step Controls" section
      readStoppingAndStepControls(inStream, lineCount);
      continue;
    }
    else if(tokens == k_IterationControls)
    {
      // Skip the comment line that ends the header
      getNextLineTokens(inStream, lineCount);

      // Read "Iteration Controls" section
      readIterationControls(inStream, lineCount);
      continue;
    }
    else if(tokens == k_ProcessingConditions)
    {
      // Skip the comment line that ends the header
      getNextLineTokens(inStream, lineCount);

      // Read "Processing Conditions" section
      readProcessingConditions(inStream, lineCount);
      continue;
    }
    else if(tokens == k_UserDefinedVariables)
    {
      // Skip the comment line that ends the header
      getNextLineTokens(inStream, lineCount);

      // Read "User Defined Variables" section
      readUserDefinedVariables(inStream, lineCount);
      continue;
    }
    else if(tokens == k_InterMaterialData)
    {
      // Skip the comment line that ends the header
      getNextLineTokens(inStream, lineCount);

      // Read "Inter-Material Data" section
      readInterMaterialData(inStream, lineCount);
      continue;
    }
    else if(tokens == k_InterObjectData)
    {
      // Skip the comment line that ends the header
      getNextLineTokens(inStream, lineCount);

      // Read "Inter-Object Data" section
      readInterObjectData(inStream, lineCount);
      continue;
    }
    else if(tokens.size() == 5)
    {
      std::vector<std::string> tmp = tokens;
      tmp.erase(tmp.begin() + 1);
      if(tmp == k_FileTitle)
      {
        // File title.  Continue...
        continue;
      }

      tmp = tokens;
      tmp.pop_back();

      if(tmp == k_PropertyDataOfMaterial)
      {
        // Skip the comment line that ends the header
        getNextLineTokens(inStream, lineCount);

        // Read "Property Data of Material" section
        readPropertyDataOfMaterial(inStream, lineCount);
        continue;
      }
      else if(tmp == k_DataForObject)
      {
        // Skip the comment line that ends the header
        getNextLineTokens(inStream, lineCount);

        // Read "Data For Object" section
        readDataForObject(inStream, lineCount, dataContainer, vertexAttributeMatrix, cellAttributeMatrix, allocate);

        if(m_Filter != nullptr && m_Filter->getErrorCode() < 0)
        {
          return;
        }

        continue;
      }
    }

    // Unrecognized section
    QString msg = QString("Warning at line %1: Unrecognized section \"%2\"").arg(QString::number(lineCount), QString::fromStdString(SIMPL::StringUtilities::join(tokens, ' ')));
    tryNotifyWarningMessage(-2010, msg);
  }
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::readProcessDefinition(std::ifstream& inStream, size_t& lineCount)
{
  // We currently don't care about the "Process Definition" section...
  findNextSection(inStream, lineCount);
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::readStoppingAndStepControls(std::ifstream& inStream, size_t& lineCount)
{
  // We currently don't care about the "Stopping & Step Controls" section...
  findNextSection(inStream, lineCount);
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::readIterationControls(std::ifstream& inStream, size_t& lineCount)
{
  // We currently don't care about the "Iteration Controls" section...
  findNextSection(inStream, lineCount);
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::readProcessingConditions(std::ifstream& inStream, size_t& lineCount)
{
  // We currently don't care about the "Processing Conditions" section...
  findNextSection(inStream, lineCount);
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::readUserDefinedVariables(std::ifstream& inStream, size_t& lineCount)
{
  // We currently don't care about the "User Defined Variables" section...
  findNextSection(inStream, lineCount);
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::readPropertyDataOfMaterial(std::ifstream& inStream, size_t& lineCount)
{
  // We currently don't care about the "Property Data of Material" section...
  findNextSection(inStream, lineCount);
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::readInterMaterialData(std::ifstream& inStream, size_t& lineCount)
{
  // We currently don't care about the "Inter-Material Data" section...
  findNextSection(inStream, lineCount);
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::readInterObjectData(std::ifstream& inStream, size_t& lineCount)
{
  // We currently don't care about the "Inter-Object Data" section...
  findNextSection(inStream, lineCount);
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::readDataForObject(std::ifstream& inStream, size_t& lineCount, DataContainer* dataContainer, AttributeMatrix* vertexAttributeMatrix, AttributeMatrix* cellAttributeMatrix,
                                               bool allocate)
{
  SharedVertexList::Pointer vertexPtr = SharedVertexList::NullPointer();
  QuadGeom::Pointer quadGeomPtr = QuadGeom::NullPointer();

  while(inStream.peek() != EOF)
  {
    if(shouldCancel())
    {
      return;
    }

    std::vector<std::string> tokens = getNextLineTokens(inStream, lineCount);

    if(tokens.empty())
    {
      // This is an empty line
      continue;
    }

    std::string word = tokens.at(0);

    if(tokens.size() == 1 && word == "*")
    {
      // We are at the next header, so we are done with this section
      return;
    }
    else if(word == "RZ")
    {
      size_t numVerts = parse_ull(tokens.at(2), lineCount);
      if(m_Filter != nullptr && m_Filter->getErrorCode() < 0)
      {
        return;
      }

      vertexPtr = readVertexCoordinates(inStream, lineCount, vertexAttributeMatrix, numVerts);
    }
    else if(word == "ELMCON")
    {
      size_t numCells = parse_ull(tokens.at(2), lineCount);
      if(m_Filter != nullptr && m_Filter->getErrorCode() < 0)
      {
        return;
      }

      quadGeomPtr = readQuadGeometry(inStream, lineCount, cellAttributeMatrix, vertexPtr, dataContainer, numCells);
    }
    else if(tokens.size() >= 3 && (vertexPtr != SharedVertexList::NullPointer() || quadGeomPtr != QuadGeom::NullPointer()))
    {
      // This is most likely the beginning of a data array
      std::string dataArrayName = tokens.at(0);
      size_t tupleCount = parse_ull(tokens.at(2), lineCount);
      if(m_Filter != nullptr && m_Filter->getErrorCode() < 0)
      {
        return;
      }

      if(vertexPtr != SharedVertexList::NullPointer() && tupleCount == vertexPtr->getNumberOfTuples())
      {
        QString statusMsg = QString("Reading Vertex Data: %1").arg(QString::fromStdString(dataArrayName));
        tryNotifyStatusMessage(statusMsg);

        readDataArray(inStream, lineCount, vertexAttributeMatrix, dataArrayName, tupleCount, allocate);
      }
      else if(quadGeomPtr != QuadGeom::NullPointer() && tupleCount == quadGeomPtr->getNumberOfQuads())
      {
        QString statusMsg = QString("Reading Cell Data: %1").arg(QString::fromStdString(dataArrayName));
        tryNotifyStatusMessage(statusMsg);

        readDataArray(inStream, lineCount, cellAttributeMatrix, dataArrayName, tupleCount, allocate);
      }
    }
    else
    {
      continue;
    }
  }
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::findNextSection(std::ifstream& inStream, size_t& lineCount)
{
  std::vector<std::string> tokens;
  while(inStream.peek() != EOF)
  {
    tokens = getNextLineTokens(inStream, lineCount);
    if(!tokens.empty() && tokens.at(0) == "*")
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
  tryNotifyStatusMessage(statusMsg);

  // Read or Skip past all the vertex data
  for(size_t i = 0; i < numVerts; i++)
  {
    if(shouldCancel())
    {
      return SharedVertexList::NullPointer();
    }

    tokens = getNextLineTokens(inStream, lineCount);

    try
    {
      vertex[3 * i] = std::stof(tokens[1]);
    } catch(const std::exception& e)
    {
      QString msg = QString("Error at line %1: Unable to convert vertex coordinate %2's 1st string value \"%3\" to float.  Threw standard exception with text: \"%4\"")
                        .arg(QString::number(lineCount), QString::number(i + 1), QString::fromStdString(tokens[1]), e.what());
      tryNotifyErrorMessage(-2001, msg);
      return SharedVertexList::NullPointer();
    }

    try
    {
      vertex[3 * i + 1] = std::stof(tokens[2]);
    } catch(const std::exception& e)
    {
      QString msg = QString("Error at line %1: Unable to convert vertex coordinate %2's 2nd string value \"%3\" to float.  Threw standard exception with text: \"%4\"")
                        .arg(QString::number(lineCount), QString::number(i + 1), QString::fromStdString(tokens[2]), e.what());
      tryNotifyErrorMessage(-2002, msg);
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
  tryNotifyStatusMessage(statusMsg);
  cellAttrMat->resizeAttributeArrays(tDims);
  QuadGeom::Pointer quadGeomPtr = QuadGeom::CreateGeometry(static_cast<int64_t>(numCells), vertexPtr, SIMPL::Geometry::QuadGeometry, true);
  quadGeomPtr->setSpatialDimensionality(2);
  dataContainer->setGeometry(quadGeomPtr);
  MeshIndexType* quads = quadGeomPtr->getQuadPointer(0);

  for(size_t i = 0; i < numCells; i++)
  {
    if(shouldCancel())
    {
      return QuadGeom::NullPointer();
    }

    tokens = getNextLineTokens(inStream, lineCount);

    // Subtract one from the node number because DEFORM starts at node 1 and we start at node 0
    try
    {
      quads[4 * i] = std::stoi(tokens[1]) - 1;
    } catch(const std::exception& e)
    {
      QString msg = QString("Error at line %1: Unable to convert quad %2's 1st string value \"%3\" to integer.  Threw standard exception with text: \"%4\"")
                        .arg(QString::number(lineCount), QString::number(i + 1), QString::fromStdString(tokens[1]), e.what());
      tryNotifyErrorMessage(-2004, msg);
      return QuadGeom::NullPointer();
    }

    try
    {
      quads[4 * i + 1] = std::stoi(tokens[2]) - 1;
    } catch(const std::exception& e)
    {
      QString msg = QString("Error at line %1: Unable to convert quad %2's 2nd string value \"%3\" to integer.  Threw standard exception with text: \"%4\"")
                        .arg(QString::number(lineCount), QString::number(i + 1), QString::fromStdString(tokens[2]), e.what());
      tryNotifyErrorMessage(-2005, msg);
      return QuadGeom::NullPointer();
    }

    try
    {
      quads[4 * i + 2] = std::stoi(tokens[3]) - 1;
    } catch(const std::exception& e)
    {
      QString msg = QString("Error at line %1: Unable to convert quad %2's 3rd string value \"%3\" to integer.  Threw standard exception with text: \"%4\"")
                        .arg(QString::number(lineCount), QString::number(i + 1), QString::fromStdString(tokens[3]), e.what());
      tryNotifyErrorMessage(-2005, msg);
      return QuadGeom::NullPointer();
    }

    try
    {
      quads[4 * i + 3] = std::stoi(tokens[4]) - 1;
    } catch(const std::exception& e)
    {
      QString msg = QString("Error at line %1: Unable to convert quad %2's 4th string value \"%3\" to integer.  Threw standard exception with text: \"%4\"")
                        .arg(QString::number(lineCount), QString::number(i + 1), QString::fromStdString(tokens[4]), e.what());
      tryNotifyErrorMessage(-2006, msg);
      return QuadGeom::NullPointer();
    }
  }

  return quadGeomPtr;
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::readDataArray(std::ifstream& inStream, size_t& lineCount, AttributeMatrix* attrMat, const std::string& dataArrayName, size_t arrayTupleSize, bool allocate)
{
  FloatArrayType::Pointer data = FloatArrayType::NullPointer();
  for(size_t i = 0; i < arrayTupleSize; i++)
  {
    if(shouldCancel())
    {
      return;
    }

    std::vector<std::string> tokens = getNextLineTokens(inStream, lineCount);

    if(i == 0)
    {
      int32_t numComp = tokens.size() - 1;
      std::vector<size_t> cDims(1, static_cast<size_t>(numComp));
      data = FloatArrayType::CreateArray(arrayTupleSize, cDims, QString::fromStdString(dataArrayName), allocate);
      attrMat->insertOrAssign(data);
    }

    if(allocate)
    {
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
          tryNotifyErrorMessage(-2008, msg);
          return;
        }
        data->setComponent(i, c, value);
      }
    }
  }
}

// -----------------------------------------------------------------------------
std::vector<std::string> ImportDeformKeyFilev12::getNextLineTokens(std::ifstream& inStream, size_t& lineCount)
{
  std::vector<std::string> tokens; /* vector to store the split data */
  std::string buf;
  std::getline(inStream, buf);
  buf = SIMPL::StringUtilities::trimmed(buf);
  buf = SIMPL::StringUtilities::simplified(buf);
  tokens = SIMPL::StringUtilities::split(buf, ' ');
  lineCount++;

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
    tryNotifyErrorMessage(-2000, msg);
    return 0;
  }

  return value;
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::tryNotifyErrorMessage(int code, const QString& messageText)
{
  if(m_Filter == nullptr)
  {
    return;
  }

  m_Filter->setErrorCondition(code, messageText);
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::tryNotifyWarningMessage(int code, const QString& messageText)
{
  if(m_Filter == nullptr)
  {
    return;
  }

  m_Filter->setWarningCondition(code, messageText);
}

// -----------------------------------------------------------------------------
void ImportDeformKeyFilev12::tryNotifyStatusMessage(const QString& messageText)
{
  if(m_Filter == nullptr)
  {
    return;
  }

  m_Filter->notifyStatusMessage(messageText);
}

// -----------------------------------------------------------------------------
bool ImportDeformKeyFilev12::shouldCancel()
{
  if(m_Filter == nullptr)
  {
    return false;
  }

  return m_Filter->getCancel();
}
