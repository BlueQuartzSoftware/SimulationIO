/*
 * Your License or Copyright can go here
 */

#pragma once

#include <memory>

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/CoreFilters/FileWriter.h"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/MeshStructs.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "SimulationIO/SimulationIODLLExport.h"

/**
 * @brief The CreateFEAInputFiles class. See [Filter documentation](@ref createfeainputfiles) for details.
 */
class SimulationIO_EXPORT CreateFEAInputFiles : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(CreateFEAInputFiles SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(CreateFEAInputFiles)
  PYB11_STATIC_NEW_MACRO(CreateFEAInputFiles)
  PYB11_FILTER_PARAMETER(int, FEAPackage)
  PYB11_FILTER_PARAMETER(QString, JobName)
  PYB11_FILTER_PARAMETER(QString, OutputPath)
  PYB11_FILTER_PARAMETER(QString, OutputFilePrefix)
  PYB11_FILTER_PARAMETER(int, NumDepvar)
  PYB11_FILTER_PARAMETER(int, NumMatConst)
  PYB11_FILTER_PARAMETER(int, NumUserOutVar)
  PYB11_FILTER_PARAMETER(DataArrayPath, AbqFeatureIdsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, PzflexFeatureIdsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellEulerAnglesArrayPath)
  PYB11_FILTER_PARAMETER(IntVec3Type, NumKeypoints)
  PYB11_FILTER_PARAMETER(int, NumClusters)
  PYB11_FILTER_PARAMETER(DynamicTableData, MatConst)
  PYB11_FILTER_PARAMETER(DataArrayPath, PhaseNamesArrayPath)
  PYB11_PROPERTY(int FEAPackage READ getFEAPackage WRITE setFEAPackage)
  PYB11_PROPERTY(QString JobName READ getJobName WRITE setJobName)
  PYB11_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)
  PYB11_PROPERTY(QString OutputFilePrefix READ getOutputFilePrefix WRITE setOutputFilePrefix)
  PYB11_PROPERTY(int NumDepvar READ getNumDepvar WRITE setNumDepvar)
  PYB11_PROPERTY(int NumMatConst READ getNumMatConst WRITE setNumMatConst)
  PYB11_PROPERTY(int NumUserOutVar READ getNumUserOutVar WRITE setNumUserOutVar)
  PYB11_PROPERTY(DataArrayPath AbqFeatureIdsArrayPath READ getAbqFeatureIdsArrayPath WRITE setAbqFeatureIdsArrayPath)
  PYB11_PROPERTY(DataArrayPath PzflexFeatureIdsArrayPath READ getPzflexFeatureIdsArrayPath WRITE setPzflexFeatureIdsArrayPath)
  PYB11_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath CellEulerAnglesArrayPath READ getCellEulerAnglesArrayPath WRITE setCellEulerAnglesArrayPath)
  PYB11_PROPERTY(IntVec3Type NumKeypoints READ getNumKeypoints WRITE setNumKeypoints)
  PYB11_PROPERTY(int NumClusters READ getNumClusters WRITE setNumClusters)
  PYB11_PROPERTY(DynamicTableData MatConst READ getMatConst WRITE setMatConst)
  PYB11_PROPERTY(DataArrayPath PhaseNamesArrayPath READ getPhaseNamesArrayPath WRITE setPhaseNamesArrayPath)
#endif

public:
  using Self = CreateFEAInputFiles;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for CreateFEAInputFiles
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for CreateFEAInputFiles
   */
  static QString ClassName();

  virtual ~CreateFEAInputFiles();

  /**
   * @brief Setter property for FEAPackage
   */
  void setFEAPackage(int value);
  /**
   * @brief Getter property for FEAPackage
   * @return Value of FEAPackage
   */
  int getFEAPackage() const;

  Q_PROPERTY(int FEAPackage READ getFEAPackage WRITE setFEAPackage)

  /**
   * @brief Setter property for JobName
   */
  void setJobName(const QString& value);
  /**
   * @brief Getter property for JobName
   * @return Value of JobName
   */
  QString getJobName() const;

  Q_PROPERTY(QString JobName READ getJobName WRITE setJobName)

  /**
   * @brief Setter property for OutputPath
   */
  void setOutputPath(const QString& value);
  /**
   * @brief Getter property for OutputPath
   * @return Value of OutputPath
   */
  QString getOutputPath() const;

  Q_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)

  /**
   * @brief Setter property for OutputFilePrefix
   */
  void setOutputFilePrefix(const QString& value);
  /**
   * @brief Getter property for OutputFilePrefix
   * @return Value of OutputFilePrefix
   */
  QString getOutputFilePrefix() const;

  Q_PROPERTY(QString OutputFilePrefix READ getOutputFilePrefix WRITE setOutputFilePrefix)

  /**
   * @brief Setter property for NumDepvar
   */
  void setNumDepvar(int value);
  /**
   * @brief Getter property for NumDepvar
   * @return Value of NumDepvar
   */
  int getNumDepvar() const;

  Q_PROPERTY(int NumDepvar READ getNumDepvar WRITE setNumDepvar)

  /**
   * @brief Setter property for NumMatConst
   */
  void setNumMatConst(int value);
  /**
   * @brief Getter property for NumMatConst
   * @return Value of NumMatConst
   */
  int getNumMatConst() const;

  Q_PROPERTY(int NumMatConst READ getNumMatConst WRITE setNumMatConst)

  /**
   * @brief Setter property for NumUserOutVar
   */
  void setNumUserOutVar(int value);
  /**
   * @brief Getter property for NumUserOutVar
   * @return Value of NumUserOutVar
   */
  int getNumUserOutVar() const;

  Q_PROPERTY(int NumUserOutVar READ getNumUserOutVar WRITE setNumUserOutVar)

  /**
   * @brief Setter property for AbqFeatureIdsArrayPath
   */
  void setAbqFeatureIdsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for AbqFeatureIdsArrayPath
   * @return Value of AbqFeatureIdsArrayPath
   */
  DataArrayPath getAbqFeatureIdsArrayPath() const;

  Q_PROPERTY(DataArrayPath AbqFeatureIdsArrayPath READ getAbqFeatureIdsArrayPath WRITE setAbqFeatureIdsArrayPath)

  /**
   * @brief Setter property for PzflexFeatureIdsArrayPath
   */
  void setPzflexFeatureIdsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for PzflexFeatureIdsArrayPath
   * @return Value of PzflexFeatureIdsArrayPath
   */
  DataArrayPath getPzflexFeatureIdsArrayPath() const;

  Q_PROPERTY(DataArrayPath PzflexFeatureIdsArrayPath READ getPzflexFeatureIdsArrayPath WRITE setPzflexFeatureIdsArrayPath)

  /**
   * @brief Setter property for CellPhasesArrayPath
   */
  void setCellPhasesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CellPhasesArrayPath
   * @return Value of CellPhasesArrayPath
   */
  DataArrayPath getCellPhasesArrayPath() const;

  Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

  /**
   * @brief Setter property for CellEulerAnglesArrayPath
   */
  void setCellEulerAnglesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CellEulerAnglesArrayPath
   * @return Value of CellEulerAnglesArrayPath
   */
  DataArrayPath getCellEulerAnglesArrayPath() const;

  Q_PROPERTY(DataArrayPath CellEulerAnglesArrayPath READ getCellEulerAnglesArrayPath WRITE setCellEulerAnglesArrayPath)

  /**
   * @brief Setter property for NumKeypoints
   */
  void setNumKeypoints(const IntVec3Type& value);
  /**
   * @brief Getter property for NumKeypoints
   * @return Value of NumKeypoints
   */
  IntVec3Type getNumKeypoints() const;

  Q_PROPERTY(IntVec3Type NumKeypoints READ getNumKeypoints WRITE setNumKeypoints)

  /**
   * @brief Setter property for NumClusters
   */
  void setNumClusters(int value);
  /**
   * @brief Getter property for NumClusters
   * @return Value of NumClusters
   */
  int getNumClusters() const;

  Q_PROPERTY(int NumClusters READ getNumClusters WRITE setNumClusters)

  /**
   * @brief Setter property for MatConst
   */
  void setMatConst(const DynamicTableData& value);
  /**
   * @brief Getter property for MatConst
   * @return Value of MatConst
   */
  DynamicTableData getMatConst() const;

  Q_PROPERTY(DynamicTableData MatConst READ getMatConst WRITE setMatConst)

  /**
   * @brief Setter property for PhaseNamesArrayPath
   */
  void setPhaseNamesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for PhaseNamesArrayPath
   * @return Value of PhaseNamesArrayPath
   */
  DataArrayPath getPhaseNamesArrayPath() const;

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
  CreateFEAInputFiles();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_CellPhasesPtr;
  int32_t* m_CellPhases = nullptr;
  std::weak_ptr<DataArray<float>> m_CellEulerAnglesPtr;
  float* m_CellEulerAngles = nullptr;

  int m_FEAPackage = {};
  QString m_JobName = {};
  QString m_OutputPath = {};
  QString m_OutputFilePrefix = {};
  int m_NumDepvar = {};
  int m_NumMatConst = {};
  int m_NumUserOutVar = {};
  DataArrayPath m_AbqFeatureIdsArrayPath = {};
  DataArrayPath m_PzflexFeatureIdsArrayPath = {};
  DataArrayPath m_CellPhasesArrayPath = {};
  DataArrayPath m_CellEulerAnglesArrayPath = {};
  IntVec3Type m_NumKeypoints = {};
  int m_NumClusters = {};
  DynamicTableData m_MatConst = {};
  DataArrayPath m_PhaseNamesArrayPath = {};

  // DEFINE_DATAARRAY_VARIABLE(QString, PhaseNames)

  StringDataArray::WeakPointer m_PhaseNamesPtr;

public:
  CreateFEAInputFiles(const CreateFEAInputFiles&) = delete;            // Copy Constructor Not Implemented
  CreateFEAInputFiles(CreateFEAInputFiles&&) = delete;                 // Move Constructor Not Implemented
  CreateFEAInputFiles& operator=(const CreateFEAInputFiles&) = delete; // Copy Assignment Not Implemented
  CreateFEAInputFiles& operator=(CreateFEAInputFiles&&) = delete;      // Move Assignment Not Implemented
};
