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
 * @class ConditionalSetValue
 * @brief This filter replaces values in the target array with a user specified value
 * where a bool mask array specifies.
 */

class SimulationIO_EXPORT ImportDeformKeyFilev12
{
public:
  ImportDeformKeyFilev12(DataContainerArray& data, ImportDeformKeyFilev12InputValues* inputValues, ImportDeformKeyFilev12Filter* filter = nullptr);
  ~ImportDeformKeyFilev12() noexcept;

  ImportDeformKeyFilev12(const ImportDeformKeyFilev12&) = delete;
  ImportDeformKeyFilev12(ImportDeformKeyFilev12&&) noexcept = delete;
  ImportDeformKeyFilev12& operator=(const ImportDeformKeyFilev12&) = delete;
  ImportDeformKeyFilev12& operator=(ImportDeformKeyFilev12&&) noexcept = delete;

  void operator()();

private:
  DataContainerArray& m_DataStructure;
  const ImportDeformKeyFilev12InputValues* m_InputValues = nullptr;
  ImportDeformKeyFilev12Filter* m_Filter = nullptr;

  void scanDEFORMFile(DataContainer* dataContainer, AttributeMatrix* vertexAttributeMatrix, AttributeMatrix* cellAttributeMatrix);

  SharedVertexList::Pointer readVertexCoordinates(std::ifstream& inStream, size_t& lineCount, AttributeMatrix* vertexAttrMat, size_t numVerts);
  QuadGeom::Pointer readQuadGeometry(std::ifstream& inStream, size_t& lineCount, AttributeMatrix* cellAttrMat, SharedVertexList::Pointer vertexPtr, DataContainer* dataContainer, size_t numCells);
  void readDataArray(std::ifstream& inStream, size_t& lineCount, AttributeMatrix* attrMat, const std::string& dataArrayName, size_t arrayTupleSize);

  std::vector<std::string> getNextLineTokens(std::ifstream& inStream);
  size_t parse_ull(const std::string& token, size_t lineCount);
  size_t parseTotalQuads(const std::vector<std::string>& tokens, size_t lineCount);

  void tryNotifyErrorMessage(int code, const QString& messageText);
  void tryNotifyStatusMessage(const QString& messageText);
  bool shouldCancel();
};

} // namespace SimulationIO
