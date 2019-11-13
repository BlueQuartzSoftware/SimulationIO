/*
 * Your License or Copyright can go here
 */

#pragma once

#include <memory>

#include "SIMPLib/Common/SIMPLArray.hpp"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "SimulationIO/SimulationIODLLExport.h"

/**
 * @brief The CreateOnScaleTableFile class. See [Filter documentation](@ref CreateOnScaleTableFile) for details.
 */
class SimulationIO_EXPORT CreateOnScaleTableFile : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(CreateOnScaleTableFile SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(CreateOnScaleTableFile)
  PYB11_STATIC_NEW_MACRO(CreateOnScaleTableFile)
  PYB11_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)
  PYB11_PROPERTY(QString OutputFilePrefix READ getOutputFilePrefix WRITE setOutputFilePrefix)
  PYB11_PROPERTY(DataArrayPath PzflexFeatureIdsArrayPath READ getPzflexFeatureIdsArrayPath WRITE setPzflexFeatureIdsArrayPath)
  PYB11_PROPERTY(IntVec3Type NumKeypoints READ getNumKeypoints WRITE setNumKeypoints)
  PYB11_PROPERTY(DataArrayPath PhaseNamesArrayPath READ getPhaseNamesArrayPath WRITE setPhaseNamesArrayPath)
#endif

public:
  using Self = CreateOnScaleTableFile;
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

  ~CreateOnScaleTableFile() override;

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
   * @brief Getter property for OutputFilePrefix
   * @return
   */
  QString getOutputFilePrefix() const;

  /**
   * @brief Setter property for OutputFilePrefix
   * @param value
   */
  void setOutputFilePrefix(const QString& value);

  Q_PROPERTY(QString OutputFilePrefix READ getOutputFilePrefix WRITE setOutputFilePrefix)

  /**
   * @brief Getter property for PzflexFeatureIdsArrayPath
   * @return
   */
  DataArrayPath getPzflexFeatureIdsArrayPath() const;

  /**
   * @brief Setter property for PzflexFeatureIdsArrayPath
   * @param value
   */
  void setPzflexFeatureIdsArrayPath(const DataArrayPath& value);

  Q_PROPERTY(DataArrayPath PzflexFeatureIdsArrayPath READ getPzflexFeatureIdsArrayPath WRITE setPzflexFeatureIdsArrayPath)

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

  /**
   * @brief This function runs some sanity checks on the DataContainer and inputs
   * in an attempt to ensure the filter can process the inputs.
   */
  void preflight() override;

signals:
  /**
   * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
   * be pushed from a user-facing control (such as a widget)
   * @param filter Filter instance pointer
   */
  void updateFilterParameters(AbstractFilter* filter);

  /**
   * @brief parametersChanged Emitted when any Filter parameter is changed internally
   */
  void parametersChanged();

  /**
   * @brief preflightAboutToExecute Emitted just before calling dataCheck()
   */
  void preflightAboutToExecute();

  /**
   * @brief preflightExecuted Emitted just after calling dataCheck()
   */
  void preflightExecuted();

protected:
  CreateOnScaleTableFile();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  struct Impl;
  std::unique_ptr<Impl> p_Impl;

  QString m_OutputPath;
  QString m_OutputFilePrefix;
  DataArrayPath m_PzflexFeatureIdsArrayPath;
  IntVec3Type m_NumKeypoints;
  DataArrayPath m_PhaseNamesArrayPath;

public:
  CreateOnScaleTableFile(const CreateOnScaleTableFile&) = delete;            // Copy Constructor Not Implemented
  CreateOnScaleTableFile(CreateOnScaleTableFile&&) = delete;                 // Move Constructor Not Implemented
  CreateOnScaleTableFile& operator=(const CreateOnScaleTableFile&) = delete; // Copy Assignment Not Implemented
  CreateOnScaleTableFile& operator=(CreateOnScaleTableFile&&) = delete;      // Move Assignment Not Implemented
};
