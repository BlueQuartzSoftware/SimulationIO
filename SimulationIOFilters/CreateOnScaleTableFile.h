/*
 * Your License or Copyright can go here
 */

#pragma once

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "SimulationIO/SimulationIODLLExport.h"

/**
 * @brief The CreateOnScaleTableFile class. See [Filter documentation](@ref CreateOnScaleTableFile) for details.
 */
class SimulationIO_EXPORT CreateOnScaleTableFile : public AbstractFilter
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(CreateFEAInputFiles SUPERCLASS AbstractFilter)
  PYB11_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)
  PYB11_PROPERTY(QString OutputFilePrefix READ getOutputFilePrefix WRITE setOutputFilePrefix)
  PYB11_PROPERTY(DataArrayPath PzflexFeatureIdsArrayPath READ getPzflexFeatureIdsArrayPath WRITE setPzflexFeatureIdsArrayPath)
  PYB11_PROPERTY(IntVec3Type NumKeypoints READ getNumKeypoints WRITE setNumKeypoints)
  PYB11_PROPERTY(DataArrayPath PhaseNamesArrayPath READ getPhaseNamesArrayPath WRITE setPhaseNamesArrayPath)

public:
  SIMPL_SHARED_POINTERS(CreateOnScaleTableFile)
  SIMPL_STATIC_NEW_MACRO(CreateOnScaleTableFile)
  SIMPL_TYPE_MACRO_SUPER(CreateOnScaleTableFile, AbstractFilter)

  virtual ~CreateOnScaleTableFile();

  SIMPL_FILTER_PARAMETER(QString, OutputPath)
  Q_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)

  SIMPL_FILTER_PARAMETER(QString, OutputFilePrefix)
  Q_PROPERTY(QString OutputFilePrefix READ getOutputFilePrefix WRITE setOutputFilePrefix)

  SIMPL_FILTER_PARAMETER(DataArrayPath, PzflexFeatureIdsArrayPath)
  Q_PROPERTY(DataArrayPath PzflexFeatureIdsArrayPath READ getPzflexFeatureIdsArrayPath WRITE setPzflexFeatureIdsArrayPath)

  SIMPL_FILTER_PARAMETER(IntVec3Type, NumKeypoints)
  Q_PROPERTY(IntVec3Type NumKeypoints READ getNumKeypoints WRITE setNumKeypoints)

  SIMPL_FILTER_PARAMETER(DataArrayPath, PhaseNamesArrayPath)
  Q_PROPERTY(DataArrayPath PhaseNamesArrayPath READ getPhaseNamesArrayPath WRITE setPhaseNamesArrayPath)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  const QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
   */
  const QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  const QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  const QUuid getUuid() override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  const QString getHumanLabel() const override;
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

public:
  CreateOnScaleTableFile(const CreateOnScaleTableFile&) = delete;            // Copy Constructor Not Implemented
  CreateOnScaleTableFile(CreateOnScaleTableFile&&) = delete;                 // Move Constructor Not Implemented
  CreateOnScaleTableFile& operator=(const CreateOnScaleTableFile&) = delete; // Copy Assignment Not Implemented
  CreateOnScaleTableFile& operator=(CreateOnScaleTableFile&&) = delete;      // Move Assignment Not Implemented
};
