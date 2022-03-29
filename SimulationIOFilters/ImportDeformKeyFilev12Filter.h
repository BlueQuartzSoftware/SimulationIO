/* ============================================================================
 * Copyright (c) 2022-2022 BlueQuartz Software, LLC
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SimulationIO/SimulationIODLLExport.h"
#include "SimulationIO/SimulationIOFilters/Algorithms/ImportDeformKeyFilev12.h"

class AttributeMatrix;
class DataContainer;

// our PIMPL private class
class ImportDeformKeyFilev12Private;

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
  PYB11_PROPERTY(bool VerboseOutput READ getVerboseOutput WRITE setVerboseOutput)
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
   * @brief Setter property for VerboseOutput
   */
  void setVerboseOutput(bool value);
  /**
   * @brief Getter property for VerboseOutput
   * @return Value of VerboseOutput
   */
  bool getVerboseOutput() const;
  Q_PROPERTY(bool VerboseOutput READ getVerboseOutput WRITE setVerboseOutput)

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
  bool m_VerboseOutput = false;

  QScopedPointer<ImportDeformKeyFilev12Private> const d_ptr;

public:
  ImportDeformKeyFilev12Filter(const ImportDeformKeyFilev12Filter&) = delete;            // Copy Constructor Not Implemented
  ImportDeformKeyFilev12Filter& operator=(const ImportDeformKeyFilev12Filter&) = delete; // Copy Assignment Not Implemented
  ImportDeformKeyFilev12Filter(ImportDeformKeyFilev12Filter&&) = delete;                 // Move Constructor Not Implemented
  ImportDeformKeyFilev12Filter& operator=(ImportDeformKeyFilev12Filter&&) = delete;      // Move Assignment Not Implemented
};
