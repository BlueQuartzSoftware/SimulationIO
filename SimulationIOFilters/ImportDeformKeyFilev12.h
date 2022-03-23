/*
 * Your License or Copyright can go here
 */

#pragma once

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/QuadGeom.h"

#include "SimulationIO/SimulationIODLLExport.h"

class AttributeMatrix;
class DataContainer;

/**
 * @brief The ImportDeformKeyFilev12 class. See [Filter documentation](@ref ImportDeformKeyFilev12) for details.
 */
class SimulationIO_EXPORT ImportDeformKeyFilev12 : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(ImportDeformKeyFilev12 SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(ImportDeformKeyFilev12)
  PYB11_FILTER_NEW_MACRO(ImportDeformKeyFilev12)
  PYB11_PROPERTY(QString DEFORMInputFile READ getDEFORMInputFile WRITE setDEFORMInputFile)
  PYB11_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)
  PYB11_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)
  PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = ImportDeformKeyFilev12;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static std::shared_ptr<ImportDeformKeyFilev12> New();

  /**
   * @brief Returns the name of the class for ImportDeformKeyFilev12
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ImportDeformKeyFilev12
   */
  static QString ClassName();

  ~ImportDeformKeyFilev12() override;

  /**
   * @brief Setter property for DEFORMInputFile
   */
  void setDEFORMInputFile(const QString& value);
  /**
   * @brief Getter property for DEFORMInputFile
   * @return Value of DEFORMInputFile
   */
  QString getDEFORMInputFile() const;
  Q_PROPERTY(QString DEFORMInputFile READ getDEFORMInputFile WRITE setDEFORMInputFile)

  /**
   * @brief Setter property for DataContainerName
   */
  void setDataContainerName(const QString& value);
  /**
   * @brief Getter property for DataContainerName
   * @return Value of DataContainerName
   */
  QString getDataContainerName() const;
  Q_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)

  /**
   * @brief Setter property for VertexAttributeMatrixName
   */
  void setVertexAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for VertexAttributeMatrixName
   * @return Value of VertexAttributeMatrixName
   */
  QString getVertexAttributeMatrixName() const;
  Q_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)

  /**
   * @brief Setter property for CellAttributeMatrixName
   */
  void setCellAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for CellAttributeMatrixName
   * @return Value of CellAttributeMatrixName
   */
  QString getCellAttributeMatrixName() const;
  Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
   */
  QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  QUuid getUuid() const override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  QString getHumanLabel() const override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief readFilterParameters Reimplemented from @see AbstractFilter class
   */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

protected:
  ImportDeformKeyFilev12();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

private:
  QString m_DEFORMInputFile = {""};
  QString m_DataContainerName = {SIMPL::Defaults::DataContainerName};
  QString m_VertexAttributeMatrixName = {SIMPL::Defaults::VertexAttributeMatrixName};
  QString m_CellAttributeMatrixName = {SIMPL::Defaults::CellAttributeMatrixName};

  void scanDEFORMFile(DataContainer* dataContainer, AttributeMatrix* vertexAttributeMatrix, AttributeMatrix* cellAttributeMatrix);

  SharedVertexList::Pointer readVertexCoordinates(std::ifstream& inStream, size_t& lineCount, AttributeMatrix* vertexAttrMat, size_t numVerts);
  QuadGeom::Pointer readQuadGeometry(std::ifstream& inStream, size_t& lineCount, AttributeMatrix* cellAttrMat, SharedVertexList::Pointer vertexPtr, DataContainer* dataContainer, size_t numCells);
  void readDataArray(std::ifstream& inStream, size_t& lineCount, AttributeMatrix* attrMat, const std::string& dataArrayName, size_t arrayTupleSize);

  std::vector<std::string> getNextLineTokens(std::ifstream& inStream);
  size_t parse_ull(const std::string& token, size_t lineCount);
  size_t parseTotalQuads(const std::vector<std::string>& tokens, size_t lineCount);

public:
  ImportDeformKeyFilev12(const ImportDeformKeyFilev12&) = delete;            // Copy Constructor Not Implemented
  ImportDeformKeyFilev12& operator=(const ImportDeformKeyFilev12&) = delete; // Copy Assignment Not Implemented
  ImportDeformKeyFilev12(ImportDeformKeyFilev12&&) = delete;                 // Move Constructor Not Implemented
  ImportDeformKeyFilev12& operator=(ImportDeformKeyFilev12&&) = delete;      // Move Assignment Not Implemented
};
