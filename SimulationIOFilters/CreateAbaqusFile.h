/*
 * Your License or Copyright can go here
 */

#pragma once

#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "SimulationIO/SimulationIODLLExport.h"

/**
 * @brief The CreateAbaqusFile class. See [Filter documentation](@ref CreateAbaqusFile) for details.
 */
class SimulationIO_EXPORT CreateAbaqusFile : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(CreateAbaqusFile SUPERCLASS AbstractFilter)
  PYB11_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)
  PYB11_PROPERTY(QString OutputFilePrefix READ getOutputFilePrefix WRITE setOutputFilePrefix)
  PYB11_PROPERTY(QString JobName READ getJobName WRITE setJobName)
  PYB11_PROPERTY(int NumDepvar READ getNumDepvar WRITE setNumDepvar)
  PYB11_PROPERTY(int NumUserOutVar READ getNumUserOutVar WRITE setNumUserOutVar)
  PYB11_PROPERTY(DynamicTableData MatConst READ getMatConst WRITE setMatConst)
  PYB11_PROPERTY(DataArrayPath AbqFeatureIdsArrayPath READ getAbqFeatureIdsArrayPath WRITE setAbqFeatureIdsArrayPath)
  PYB11_PROPERTY(DataArrayPath CellEulerAnglesArrayPath READ getCellEulerAnglesArrayPath WRITE setCellEulerAnglesArrayPath)
  PYB11_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)
#endif

public:
  using Self = CreateAbaqusFile;
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

  ~CreateAbaqusFile() override;

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
   * @brief Getter property for JobName
   * @return
   */
  QString getJobName() const;

  /**
   * @brief Setter property for JobName
   * @param value
   */
  void setJobName(const QString& value);

  Q_PROPERTY(QString JobName READ getJobName WRITE setJobName)

  /**
   * @brief Getter property for NumDepvar
   * @return
   */
  int getNumDepvar() const;

  /**
   * @brief Setter property for NumDepvar
   * @param value
   */
  void setNumDepvar(int value);

  Q_PROPERTY(int NumDepvar READ getNumDepvar WRITE setNumDepvar)

  /**
   * @brief Getter property for NumUserOutVar
   * @return
   */
  int getNumUserOutVar() const;

  /**
   * @brief Setter property for NumUserOutVar
   * @param value
   */
  void setNumUserOutVar(int value);

  Q_PROPERTY(int NumUserOutVar READ getNumUserOutVar WRITE setNumUserOutVar)

  /**
   * @brief Getter property for MatConst
   * @return
   */
  DynamicTableData getMatConst() const;

  /**
   * @brief Setter property for MatConst
   * @param value
   */
  void setMatConst(const DynamicTableData& value);

  Q_PROPERTY(DynamicTableData MatConst READ getMatConst WRITE setMatConst)

  /**
   * @brief Getter property for AbqFeatureIdsArrayPath
   * @return
   */
  DataArrayPath getAbqFeatureIdsArrayPath() const;

  /**
   * @brief Setter property for AbqFeatureIdsArrayPath
   * @param value
   */
  void setAbqFeatureIdsArrayPath(const DataArrayPath& value);

  Q_PROPERTY(DataArrayPath AbqFeatureIdsArrayPath READ getAbqFeatureIdsArrayPath WRITE setAbqFeatureIdsArrayPath)

  /**
   * @brief Getter property for CellEulerAnglesArrayPath
   * @return
   */
  DataArrayPath getCellEulerAnglesArrayPath() const;

  /**
   * @brief Setter property for CellEulerAnglesArrayPath
   * @param value
   */
  void setCellEulerAnglesArrayPath(const DataArrayPath& value);

  Q_PROPERTY(DataArrayPath CellEulerAnglesArrayPath READ getCellEulerAnglesArrayPath WRITE setCellEulerAnglesArrayPath)

  /**
   * @brief Getter property for CellPhasesArrayPath
   * @return
   */
  DataArrayPath getCellPhasesArrayPath() const;

  /**
   * @brief Setter property for CellPhasesArrayPath
   * @param value
   */
  void setCellPhasesArrayPath(const DataArrayPath& value);

  Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

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
  CreateAbaqusFile();

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
  QString m_JobName;
  int m_NumDepvar;
  int m_NumUserOutVar;
  DynamicTableData m_MatConst;
  DataArrayPath m_AbqFeatureIdsArrayPath;
  DataArrayPath m_CellEulerAnglesArrayPath;
  DataArrayPath m_CellPhasesArrayPath;

public:
  CreateAbaqusFile(const CreateAbaqusFile&) = delete;            // Copy Constructor Not Implemented
  CreateAbaqusFile(CreateAbaqusFile&&) = delete;                 // Move Constructor Not Implemented
  CreateAbaqusFile& operator=(const CreateAbaqusFile&) = delete; // Copy Assignment Not Implemented
  CreateAbaqusFile& operator=(CreateAbaqusFile&&) = delete;      // Move Assignment Not Implemented
};