/*
 * Your License or Copyright can go here
 */

#pragma once

#include <QtCore/QString>

#include "SIMPLib/CoreFilters/FileWriter.h"
#include "SIMPLib/DataArrays/StringDataArray.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/MeshStructs.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/FilterParameters/DynamicTableData.h"

#include "SimulationIO/SimulationIODLLExport.h"

/**
 * @brief The CreateFEAInputFiles class. See [Filter documentation](@ref createfeainputfiles) for details.
 */
class SimulationIO_EXPORT CreateFEAInputFiles : public AbstractFilter
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(CreateFEAInputFiles SUPERCLASS AbstractFilter)
  PYB11_PROPERTY(int FEAPackage READ getFEAPackage WRITE setFEAPackage)
  PYB11_PROPERTY(QString JobName READ getJobName WRITE setJobName)
  PYB11_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)
  PYB11_PROPERTY(QString OutputFilePrefix READ getOutputFilePrefix WRITE setOutputFilePrefix)
    //  PYB11_PROPERTY(IntVec3_t NumElem READ getNumElem WRITE setNumElem)
  PYB11_PROPERTY(int NumDepvar READ getNumDepvar WRITE setNumDepvar)
  PYB11_PROPERTY(int NumMatConst READ getNumMatConst WRITE setNumMatConst)
  PYB11_PROPERTY(int NumUserOutVar READ getNumUserOutVar WRITE setNumUserOutVar)
  PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
  PYB11_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath CellEulerAnglesArrayPath READ getCellEulerAnglesArrayPath WRITE setCellEulerAnglesArrayPath)
  PYB11_PROPERTY(IntVec3_t NumKeypoints READ getNumKeypoints WRITE setNumKeypoints)
  PYB11_PROPERTY(QString DelamMat READ getDelamMat WRITE setDelamMat)
  PYB11_PROPERTY(int NumClusters READ getNumClusters WRITE setNumClusters)
  PYB11_PROPERTY(DynamicTableData ClusterData READ getClusterData WRITE setClusterData)
  PYB11_PROPERTY(DynamicTableData MatConst READ getMatConst WRITE setMatConst)

    //  PYB11_PROPERTY(bool UseMeshCreatedByDREAM3D READ getUseMeshCreatedByDREAM3D WRITE setUseMeshCreatedByDREAM3D)

public:
  SIMPL_SHARED_POINTERS(CreateFEAInputFiles)
  SIMPL_STATIC_NEW_MACRO(CreateFEAInputFiles)
  SIMPL_TYPE_MACRO_SUPER(CreateFEAInputFiles, AbstractFilter)

  virtual ~CreateFEAInputFiles();

  SIMPL_FILTER_PARAMETER(int, FEAPackage)
  Q_PROPERTY(int FEAPackage READ getFEAPackage WRITE setFEAPackage)

  SIMPL_FILTER_PARAMETER(QString, JobName)
  Q_PROPERTY(QString JobName READ getJobName WRITE setJobName)

  SIMPL_FILTER_PARAMETER(QString, OutputPath)
  Q_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)

  SIMPL_FILTER_PARAMETER(QString, OutputFilePrefix)
  Q_PROPERTY(QString OutputFilePrefix READ getOutputFilePrefix WRITE setOutputFilePrefix)

  // SIMPL_FILTER_PARAMETER(IntVec3_t, NumElem)
  // Q_PROPERTY(IntVec3_t NumElem READ getNumElem WRITE setNumElem)

  SIMPL_FILTER_PARAMETER(int, NumDepvar)
  Q_PROPERTY(int NumDepvar READ getNumDepvar WRITE setNumDepvar)

  SIMPL_FILTER_PARAMETER(int, NumMatConst)
  Q_PROPERTY(int NumMatConst READ getNumMatConst WRITE setNumMatConst)

  SIMPL_FILTER_PARAMETER(int, NumUserOutVar)
  Q_PROPERTY(int NumUserOutVar READ getNumUserOutVar WRITE setNumUserOutVar)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
  Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, CellEulerAnglesArrayPath)
  Q_PROPERTY(DataArrayPath CellEulerAnglesArrayPath READ getCellEulerAnglesArrayPath WRITE setCellEulerAnglesArrayPath)

  SIMPL_FILTER_PARAMETER(IntVec3_t, NumKeypoints)
  Q_PROPERTY(IntVec3_t NumKeypoints READ getNumKeypoints WRITE setNumKeypoints)

  SIMPL_FILTER_PARAMETER(QString, DelamMat)
  Q_PROPERTY(QString DelamMat READ getDelamMat WRITE setDelamMat)

  SIMPL_FILTER_PARAMETER(int, NumClusters)
  Q_PROPERTY(int NumClusters READ getNumClusters WRITE setNumClusters)

  SIMPL_FILTER_PARAMETER(DynamicTableData, ClusterData)
  Q_PROPERTY(DynamicTableData ClusterData READ getClusterData WRITE setClusterData)

  SIMPL_FILTER_PARAMETER(DynamicTableData, MatConst)
  Q_PROPERTY(DynamicTableData MatConst READ getMatConst WRITE setMatConst)

  //  SIMPL_FILTER_PARAMETER(bool, UseMeshCreatedByDREAM3D)
  //  Q_PROPERTY(bool UseMeshCreatedByDREAM3D READ getUseMeshCreatedByDREAM3D WRITE setUseMeshCreatedByDREAM3D)

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
  DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)
  DEFINE_DATAARRAY_VARIABLE(int32_t, CellPhases)
  DEFINE_DATAARRAY_VARIABLE(float, CellEulerAngles)

public:
  CreateFEAInputFiles(const CreateFEAInputFiles&) = delete;            // Copy Constructor Not Implemented
  CreateFEAInputFiles(CreateFEAInputFiles&&) = delete;                 // Move Constructor Not Implemented
  CreateFEAInputFiles& operator=(const CreateFEAInputFiles&) = delete; // Copy Assignment Not Implemented
  CreateFEAInputFiles& operator=(CreateFEAInputFiles&&) = delete;      // Move Assignment Not Implemented
};

