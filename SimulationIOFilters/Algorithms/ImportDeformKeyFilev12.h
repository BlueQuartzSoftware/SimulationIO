#pragma once

#include "SimulationIO/SimulationIODLLExport.h"

#include "SIMPLib/Geometry/QuadGeom.h"

class ImportDeformKeyFilev12Filter;
class DataContainer;

namespace SimulationIO
{

struct SimulationIO_EXPORT ImportDeformKeyFilev12InputValues
{
  std::string deformInputFile;
  std::string dataContainerName;
  std::string vertexAttributeMatrixName;
  std::string cellAttributeMatrixName;
};

/**
 * @class ImportDeformKeyFilev12
 * @brief This class reads a DEFORM v12 key file. It reads the quadrilateral mesh data (nodal coordinates
 * and connectivity), and the value of variables such as stress, strain, ndtmp, etc at cells and nodes.
 *
 * It saves the quad geometry from the file to the given data container and creates the data arrays from
 * the file in the given vertex and cell attribute matrices.
 */

class SimulationIO_EXPORT ImportDeformKeyFilev12
{
public:
  ImportDeformKeyFilev12(ImportDeformKeyFilev12InputValues* inputValues, ImportDeformKeyFilev12Filter* filter = nullptr);
  ~ImportDeformKeyFilev12() noexcept;

  ImportDeformKeyFilev12(const ImportDeformKeyFilev12&) = delete;
  ImportDeformKeyFilev12(ImportDeformKeyFilev12&&) noexcept = delete;
  ImportDeformKeyFilev12& operator=(const ImportDeformKeyFilev12&) = delete;
  ImportDeformKeyFilev12& operator=(ImportDeformKeyFilev12&&) noexcept = delete;

  void readDEFORMFile(DataContainer* dataContainer, AttributeMatrix* vertexAttributeMatrix, AttributeMatrix* cellAttributeMatrix, bool allocate = true);

private:
  const ImportDeformKeyFilev12InputValues* m_InputValues = nullptr;
  ImportDeformKeyFilev12Filter* m_Filter = nullptr;

  void readProcessDefinition(std::ifstream& inStream, size_t& lineCount);
  void readStoppingAndStepControls(std::ifstream& inStream, size_t& lineCount);
  void readIterationControls(std::ifstream& inStream, size_t& lineCount);
  void readProcessingConditions(std::ifstream& inStream, size_t& lineCount);
  void readUserDefinedVariables(std::ifstream& inStream, size_t& lineCount);
  void readPropertyDataOfMaterial(std::ifstream& inStream, size_t& lineCount);
  void readInterMaterialData(std::ifstream& inStream, size_t& lineCount);
  void readInterObjectData(std::ifstream& inStream, size_t& lineCount);
  void readDataForObject(std::ifstream& inStream, size_t& lineCount, DataContainer* dataContainer, AttributeMatrix* vertexAttributeMatrix, AttributeMatrix* cellAttributeMatrix, bool allocate);

  void findNextSection(std::ifstream& inStream, size_t& lineCount);

  SharedVertexList::Pointer readVertexCoordinates(std::ifstream& inStream, size_t& lineCount, AttributeMatrix* vertexAttrMat, size_t numVerts);
  QuadGeom::Pointer readQuadGeometry(std::ifstream& inStream, size_t& lineCount, AttributeMatrix* cellAttrMat, SharedVertexList::Pointer vertexPtr, DataContainer* dataContainer, size_t numCells);
  void readDataArray(std::ifstream& inStream, size_t& lineCount, AttributeMatrix* attrMat, const std::string& dataArrayName, size_t arrayTupleSize, bool allocate);

  std::vector<std::string> getNextLineTokens(std::ifstream& inStream, size_t& lineCount);
  size_t parse_ull(const std::string& token, size_t lineCount);

  void tryNotifyErrorMessage(int code, const QString& messageText);
  void tryNotifyWarningMessage(int code, const QString& messageText);
  void tryNotifyStatusMessage(const QString& messageText);
  bool shouldCancel();
};

} // namespace SimulationIO
