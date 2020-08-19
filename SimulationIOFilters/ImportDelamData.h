/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
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
 * The code contained herein was partially funded by the followig contracts:
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SimulationIO/SimulationIOPlugin.h"

/**
 * @brief The ImportDelamData class. See [Filter documentation](@ref importdelamdata) for details.
 */
class SimulationIO_EXPORT ImportDelamData : public AbstractFilter
{
  Q_OBJECT

  // clang-format off
  PYB11_BEGIN_BINDINGS(ImportDelamData SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(ImportDelamData)
  PYB11_FILTER_NEW_MACRO(ImportDelamData)
  PYB11_PROPERTY(QString BvidFile READ getBvidFile WRITE setBvidFile)
  PYB11_PROPERTY(QString BvidStdOutFile READ getBvidStdOutFile WRITE setBvidStdOutFile)
  PYB11_PROPERTY(float InterfaceThickness READ getInterfaceThickness WRITE setInterfaceThickness)
  PYB11_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)
  PYB11_END_BINDINGS()
  // clang-format on

public:

  using Self = ImportDelamData;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for ImportDelamData
   */
  QString getNameOfClass() const override;

  /**
   * @brief Returns the name of the class for ImportDelamData
   */
  static QString ClassName();

  ~ImportDelamData() override;

  /**
   * @brief Setter property for CSDGMFile
   */
  void setCSDGMFile(const QString& value);

  /**
   * @brief Getter property for CSDGMFile
   * @return Value of CSDGMFile
   */
  QString getCSDGMFile() const;
  Q_PROPERTY(QString CSDGMFile READ getCSDGMFile WRITE setCSDGMFile)

  /**
   * @brief Setter property for BvidStdOutFile
   */
  void setBvidStdOutFile(const QString& value);

  /**
   * @brief Getter property for BvidStdOutFile
   * @return Value of BvidStdOutFile
   */
  QString getBvidStdOutFile() const;
  Q_PROPERTY(QString BvidStdOutFile READ getBvidStdOutFile WRITE setBvidStdOutFile)

  /**
   * @brief Setter property for InterfaceThickness
   */
  void setInterfaceThickness(float value);

  /**
   * @brief Getter property for InterfaceThickness
   * @return Value of InterfaceThickness
   */
  float getInterfaceThickness() const;
  Q_PROPERTY(float InterfaceThickness READ getInterfaceThickness WRITE setInterfaceThickness)

  /**
   * @brief Setter property for DataContainerName
   */
  void setDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for DataContainerName
   * @return Value of DataContainerName
   */
  DataArrayPath getDataContainerName() const;
  Q_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)

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
   * @brief Setter property for DataArrayName
   */
  void setDataArrayName(const QString& value);

  /**
   * @brief Getter property for DataArrayName
   * @return Value of DataArrayName
   */
  QString getDataArrayName() const;
  Q_PROPERTY(QString DataArrayName READ getDataArrayName WRITE setDataArrayName)

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

protected:
  ImportDelamData();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  QString m_CSDGMFile = {};
  QString m_BvidStdOutFile = {};
  float m_InterfaceThickness = 0.001f;
  DataArrayPath m_DataContainerName = DataArrayPath("RectGridDataContainer", "", "");
  QString m_CellAttributeMatrixName = "CellData";
  QString m_DataArrayName = "DataArray";

  std::optional<float> m_Lcx = {};
  std::optional<float> m_Ex = {};
  std::optional<int> m_CzmLayers = {};
  std::optional<int> m_NumPlies = {};
  std::optional<float> m_TotalThk = {};
  size_t m_Xcrd = {};
  size_t m_Ycrd = {};
  size_t m_Zcrd = {};

  /**
   * @brief Reads the Lcx, Ex, CzmLayers, NumPlies, and TotalThk values out of the Bvid stdout file
   */
  void readBvidStdOutFile();

  /**
   * @brief Gets the line count from the Bvid file.  This is used to determine how many tuples the created attribute matrix should have
   * @return
   */
  size_t getBvidFileLineCount();

  /**
   * @brief Reads the data in the CSDGM file and populates the created Data Array
   */
  void readCSDGMFile();

public:
  ImportDelamData(const ImportDelamData&) = delete;            // Copy Constructor Not Implemented
  ImportDelamData& operator=(const ImportDelamData&) = delete; // Copy Assignment Not Implemented
  ImportDelamData(ImportDelamData &&) = delete;                // Move Constructor Not Implemented
  ImportDelamData& operator=(ImportDelamData&&) = delete;      // Move Assignment Not Implemented
};

