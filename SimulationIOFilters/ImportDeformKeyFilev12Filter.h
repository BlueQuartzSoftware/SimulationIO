/*
 * Your License or Copyright can go here
 */

#pragma once

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SimulationIO/SimulationIODLLExport.h"

class AttributeMatrix;
class DataContainer;

/**
 * @brief The ImportDeformKeyFilev12Filter class. See [Filter documentation](@ref ImportDeformKeyFilev12Filter) for details.
 */
class SimulationIO_EXPORT ImportDeformKeyFilev12Filter : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(ImportDeformKeyFilev12Filter SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(ImportDeformKeyFilev12Filter)
  PYB11_FILTER_NEW_MACRO(ImportDeformKeyFilev12Filter)
  PYB11_PROPERTY(QString DEFORMInputFile READ getDEFORMInputFile WRITE setDEFORMInputFile)
  PYB11_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)
  PYB11_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)
  PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = ImportDeformKeyFilev12Filter;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static std::shared_ptr<ImportDeformKeyFilev12Filter> New();

  /**
   * @brief Returns the name of the class for ImportDeformKeyFilev12Filter
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ImportDeformKeyFilev12Filter
   */
  static QString ClassName();

  ~ImportDeformKeyFilev12Filter() override;

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
  ImportDeformKeyFilev12Filter();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

private:
  QString m_DEFORMInputFile = {""};
  QString m_DataContainerName = {SIMPL::Defaults::DataContainerName};
  QString m_VertexAttributeMatrixName = {SIMPL::Defaults::VertexAttributeMatrixName};
  QString m_CellAttributeMatrixName = {SIMPL::Defaults::CellAttributeMatrixName};

public:
  ImportDeformKeyFilev12Filter(const ImportDeformKeyFilev12Filter&) = delete;            // Copy Constructor Not Implemented
  ImportDeformKeyFilev12Filter& operator=(const ImportDeformKeyFilev12Filter&) = delete; // Copy Assignment Not Implemented
  ImportDeformKeyFilev12Filter(ImportDeformKeyFilev12Filter&&) = delete;                 // Move Constructor Not Implemented
  ImportDeformKeyFilev12Filter& operator=(ImportDeformKeyFilev12Filter&&) = delete;      // Move Assignment Not Implemented
};
