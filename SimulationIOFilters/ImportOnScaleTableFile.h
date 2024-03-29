/* ============================================================================
 * Copyright (c) 2020 BlueQuartz Software, LLC
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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>

#include <QtCore/QFile>
#include <QtCore/QScopedPointer>
#include <QtGui/QAbstractUndoItem>
#include <QtWidgets/QWidget>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SimulationIO/SimulationIOConstants.h"
#include "SimulationIO/SimulationIODLLExport.h"

// our PIMPL private class
class ImportOnScaleTableFilePrivate;

/**
 * @brief The ImportOnScaleTableFile class. See [Filter documentation](@ref importonscaletablefile) for details.
 */
class SimulationIO_EXPORT ImportOnScaleTableFile : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(ImportOnScaleTableFile SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(ImportOnScaleTableFile)
  PYB11_FILTER_NEW_MACRO(ImportOnScaleTableFile)

  PYB11_PROPERTY(DataArrayPath VolumeDataContainerName READ getVolumeDataContainerName WRITE setVolumeDataContainerName)
  PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
  PYB11_PROPERTY(QString PhaseAttributeMatrixName READ getPhaseAttributeMatrixName WRITE setPhaseAttributeMatrixName)
  PYB11_PROPERTY(QString MaterialNameArrayName READ getMaterialNameArrayName WRITE setMaterialNameArrayName)
  PYB11_PROPERTY(QString RectGridGeometryDesc READ getRectGridGeometryDesc)

  PYB11_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
  PYB11_PROPERTY(FloatVec3Type Origin READ getOrigin WRITE setOrigin)
  PYB11_PROPERTY(FloatVec3Type Spacing READ getSpacing WRITE setSpacing)
  PYB11_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = ImportOnScaleTableFile;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for ImportOnScaleTableFile
   */
  QString getNameOfClass() const override;

  /**
   * @brief Returns the name of the class for ImportOnScaleTableFile
   */
  static QString ClassName();

  ~ImportOnScaleTableFile() override;

  /**
   * @brief Setter property for VolumeDataContainerName
   */
  void setVolumeDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for VolumeDataContainerName
   * @return Value of VolumeDataContainerName
   */
  DataArrayPath getVolumeDataContainerName() const;
  Q_PROPERTY(DataArrayPath VolumeDataContainerName READ getVolumeDataContainerName WRITE setVolumeDataContainerName)

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
   * @brief Setter property for InputFile
   */
  void setInputFile(const QString& value);
  /**
   * @brief Getter property for InputFile
   * @return Value of InputFile
   */
  QString getInputFile() const;
  Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

  /**
   * @brief Setter property for Origin
   */
  void setOrigin(const FloatVec3Type& value);
  /**
   * @brief Getter property for Origin
   * @return Value of Origin
   */
  FloatVec3Type getOrigin() const;
  Q_PROPERTY(FloatVec3Type Origin READ getOrigin WRITE setOrigin)

  /**
   * @brief Setter property for Spacing
   */
  void setSpacing(const FloatVec3Type& value);
  /**
   * @brief Getter property for Spacing
   * @return Value of Spacing
   */
  FloatVec3Type getSpacing() const;
  Q_PROPERTY(FloatVec3Type Spacing READ getSpacing WRITE setSpacing)

  /**
   * @brief Setter property for FileWasRead
   */
  void setFileWasRead(bool value);
  /**
   * @brief Getter property for FileWasRead
   * @return Value of FileWasRead
   */
  bool getFileWasRead() const;
  Q_PROPERTY(bool FileWasRead READ getFileWasRead)

  /**
   * @brief Setter property for FeatureIdsArrayName
   */
  void setFeatureIdsArrayName(const QString& value);
  /**
   * @brief Getter property for FeatureIdsArrayName
   * @return Value of FeatureIdsArrayName
   */
  QString getFeatureIdsArrayName() const;
  Q_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

  /**
   * @brief Setter property for PhaseAttributeMatrixName
   */
  void setPhaseAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for PhaseAttributeMatrixName
   * @return Value of PhaseAttributeMatrixName
   */
  QString getPhaseAttributeMatrixName() const;
  Q_PROPERTY(QString PhaseAttributeMatrixName READ getPhaseAttributeMatrixName WRITE setPhaseAttributeMatrixName)

  /**
   * @brief Setter property for MaterialNameArrayName
   */
  void setMaterialNameArrayName(const QString& value);
  /**
   * @brief Getter property for MaterialNameArrayName
   * @return Value of MaterialNameArrayName
   */
  QString getMaterialNameArrayName() const;
  Q_PROPERTY(QString MaterialNameArrayName READ getMaterialNameArrayName WRITE setMaterialNameArrayName)

  /**
   * @brief getNewBoxDimensions
   * @return
   */
  QString getRectGridGeometryDesc();
  Q_PROPERTY(QString RectGridGeometryDesc READ getRectGridGeometryDesc)

  /**
   * @brief Setter property for Dims
   */
  void setDims(const std::vector<size_t>& value);
  /**
   * @brief Getter property for Dims
   * @return Value of Dims
   */
  std::vector<size_t> getDims() const;

  /**
   * @brief Setter property for Dims
   */
  void setCoords(const std::vector<FloatArrayType::Pointer>& value);
  /**
   * @brief Getter property for Dims
   * @return Value of Dims
   */
  std::vector<FloatArrayType::Pointer> getCoords() const;

  /**
   * @brief Setter property for Dims
   */
  void setNames(const std::vector<QString>& value);
  /**
   * @brief Getter property for Dims
   * @return Value of Dims
   */
  std::vector<QString> getNames() const;

  /**
   * @brief Setter property for InputFile_Cache
   */
  void setInputFile_Cache(const QString& value);
  /**
   * @brief Getter property for InputFile_Cache
   * @return Value of InputFile_Cache
   */
  QString getInputFile_Cache() const;

  /**
   * @brief Setter property for LastRead
   */
  void setLastRead(const QDateTime& value);
  /**
   * @brief Getter property for LastRead
   * @return Value of LastRead
   */
  QDateTime getLastRead() const;

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
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

public Q_SLOTS:
  /**
   * @brief flushCache Clears the input file cache
   */
  void flushCache();

protected:
  ImportOnScaleTableFile();

  /**
   * @brief readHeader Reimplemented from @see FileReader class
   */
  int32_t readHeader(QFile& fileStream);

  /**
   * @brief readFile Reimplemented from @see FileReader class
   */
  int32_t readFile(QFile& fileStream);

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  std::weak_ptr<Int32ArrayType> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;

  std::weak_ptr<StringDataArray> m_MaterialNamesPtr;

  DataArrayPath m_VolumeDataContainerName = {"DataContainer", "", ""};
  QString m_CellAttributeMatrixName = {"Table Data"};
  QString m_InputFile = {""};
  FloatVec3Type m_Origin = {0.0F, 0.0F, 0.0F};
  FloatVec3Type m_Spacing = {1.0F, 1.0F, 1.0F};
  bool m_FileWasRead = {false};
  QString m_FeatureIdsArrayName = {"Material"};

  QString m_MaterialNameArrayName = {"Material Names"};
  QString m_PhaseAttributeMatrixName = {"Phase Info"};

  SizeVec3Type m_Dims = {0, 0, 0};

  struct ImportOnScaleTableFilePrivate;
  std::unique_ptr<ImportOnScaleTableFilePrivate> const d_ptr;

public:
  ImportOnScaleTableFile(const ImportOnScaleTableFile&) = delete;            // Copy Constructor Not Implemented
  ImportOnScaleTableFile& operator=(const ImportOnScaleTableFile&) = delete; // Copy Assignment Not Implemented
  ImportOnScaleTableFile(ImportOnScaleTableFile&&) = delete;                 // Move Constructor Not Implemented
  ImportOnScaleTableFile& operator=(ImportOnScaleTableFile&&) = delete;      // Move Assignment Not Implemented
};
