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

#include <memory>

#include "SIMPLib/Common/SIMPLArray.hpp"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "SimulationIO/SimulationIODLLExport.h"

/**
 * @brief The CreateMultiOnScaleTableFile class. See [Filter documentation](@ref CreateMultiOnScaleTableFile) for details.
 */
class SimulationIO_EXPORT CreateMultiOnScaleTableFile : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(CreateMultiOnScaleTableFile SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(CreateMultiOnScaleTableFile)
  PYB11_STATIC_NEW_MACRO(CreateMultiOnScaleTableFile)
  PYB11_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)
  PYB11_PROPERTY(QString DataContainerPrefix READ getDataContainerPrefix WRITE setDataContainerPrefix)
  PYB11_PROPERTY(QString MatrixName READ getMatrixName WRITE setMatrixName)
  PYB11_PROPERTY(QString ArrayName READ getArrayName WRITE setArrayName)
  PYB11_PROPERTY(QString SelectedArrays READ getSelectedArrays WRITE setSelectedArrays)
  PYB11_PROPERTY(IntVec3Type NumKeypoints READ getNumKeypoints WRITE setNumKeypoints)
  PYB11_PROPERTY(DataArrayPath PhaseNamesArrayPath READ getPhaseNamesArrayPath WRITE setPhaseNamesArrayPath)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = CreateMultiOnScaleTableFile;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for CreateDataArray
   */
  QString getNameOfClass() const override;

  /**
   * @brief Returns the name of the class for CreateDataArray
   */
  static QString ClassName();

  ~CreateMultiOnScaleTableFile() override;

  /**
   * @brief Getter property for OutputPath
   * @return
   */
  QString getOutputPath() const;

  /**
   * @brief Setter property for OutputPath
   * @param value
   */
  void setOutputPath(const QString& value);
  Q_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)

  /**
   * @brief Getter property for DataContainerPrefix
   * @return
   */
  QString getDataContainerPrefix() const;

  /**
   * @brief Setter property for DataContainerPrefix
   * @param value
   */
  void setDataContainerPrefix(const QString& value);
  Q_PROPERTY(QString DataContainerPrefix READ getDataContainerPrefix WRITE setDataContainerPrefix)

  /**
   * @brief Getter property for MatrixName
   * @return
   */
  QString getMatrixName() const;

  /**
   * @brief Setter property for MatrixName
   * @param value
   */
  void setMatrixName(const QString& value);
  Q_PROPERTY(QString MatrixName READ getMatrixName WRITE setMatrixName)

  /**
   * @brief Getter property for ArrayName
   * @return
   */
  QString getArrayName() const;

  /**
   * @brief Setter property for ArrayName
   * @param value
   */
  void setArrayName(const QString& value);
  Q_PROPERTY(QString ArrayName READ getArrayName WRITE setArrayName)

  /**
   * @brief Getter property for SelectedArrays
   * @return
   */
  QString getSelectedArrays() const;

  /**
   * @brief Setter property for SelectedArrays
   * @param value
   */
  void setSelectedArrays(const QString& value);
  Q_PROPERTY(QString SelectedArrays READ getSelectedArrays WRITE setSelectedArrays)

  /**
   * @brief Getter property for NumKeypoints
   * @return
   */
  IntVec3Type getNumKeypoints() const;

  /**
   * @brief Setter property for NumKeypoints
   * @param value
   */
  void setNumKeypoints(const IntVec3Type& value);
  Q_PROPERTY(IntVec3Type NumKeypoints READ getNumKeypoints WRITE setNumKeypoints)

  /**
   * @brief Getter property for PhaseNamesArrayPath
   * @return
   */
  DataArrayPath getPhaseNamesArrayPath() const;

  /**
   * @brief Setter property for PhaseNamesArrayPath
   * @param value
   */
  void setPhaseNamesArrayPath(const DataArrayPath& value);
  Q_PROPERTY(DataArrayPath PhaseNamesArrayPath READ getPhaseNamesArrayPath WRITE setPhaseNamesArrayPath)

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
   * @brief This method will instantiate all the end user settable options/parameters
   * for this filter
   */
  void setupFilterParameters() override;

  /**
   * @brief This method will read the options from a file
   * @param reader The reader that is used to read the options from a file
   */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

  /**
   * @brief Reimplemented from @see AbstractFilter class
   */
  void execute() override;

protected:
  CreateMultiOnScaleTableFile();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  struct Impl;
  std::unique_ptr<Impl> p_Impl;

  QString m_OutputPath;
  QString m_DataContainerPrefix;
  QString m_MatrixName;
  QString m_ArrayName;
  QString m_SelectedArrays;
  IntVec3Type m_NumKeypoints;
  DataArrayPath m_PhaseNamesArrayPath;

public:
  CreateMultiOnScaleTableFile(const CreateMultiOnScaleTableFile&) = delete;            // Copy Constructor Not Implemented
  CreateMultiOnScaleTableFile(CreateMultiOnScaleTableFile&&) = delete;                 // Move Constructor Not Implemented
  CreateMultiOnScaleTableFile& operator=(const CreateMultiOnScaleTableFile&) = delete; // Copy Assignment Not Implemented
  CreateMultiOnScaleTableFile& operator=(CreateMultiOnScaleTableFile&&) = delete;      // Move Assignment Not Implemented
};
