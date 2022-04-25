/*
 * Your License or Copyright can go here
 */

#pragma once

#include <QtCore/QString>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/CoreFilters/FileWriter.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/MeshStructs.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/FilterParameters/DynamicTableData.h"

#include "SimulationIO/SimulationIOPlugin.h"

/**
 * @brief The ExportDAMASKFiles class. See [Filter documentation](@ref exportdamaskfiles) for details.
 */
class SimulationIO_EXPORT ExportDAMASKFiles : public AbstractFilter
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(ExportDAMASKFiles SUPERCLASS AbstractFilter)
  PYB11_PROPERTY(int DataFormat READ getDataFormat WRITE setDataFormat)
  PYB11_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)
  PYB11_PROPERTY(QString GeometryFileName READ getGeometryFileName WRITE setGeometryFileName)
  PYB11_PROPERTY(int HomogenizationIndex READ getHomogenizationIndex WRITE setHomogenizationIndex)
  PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
  PYB11_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath CellEulerAnglesArrayPath READ getCellEulerAnglesArrayPath WRITE setCellEulerAnglesArrayPath)
  PYB11_PROPERTY(bool CompressGeomFile READ getCompressGeomFile WRITE setCompressGeomFile)

  public:
    SIMPL_SHARED_POINTERS(ExportDAMASKFiles)
    SIMPL_FILTER_NEW_MACRO(ExportDAMASKFiles)
    SIMPL_TYPE_MACRO_SUPER(ExportDAMASKFiles, AbstractFilter)

    ~ExportDAMASKFiles() override;

    SIMPL_FILTER_PARAMETER(int, DataFormat)
    Q_PROPERTY(int DataFormat READ getDataFormat WRITE setDataFormat)

    SIMPL_FILTER_PARAMETER(QString, OutputPath)
    Q_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)

    SIMPL_FILTER_PARAMETER(QString, GeometryFileName)
    Q_PROPERTY(QString GeometryFileName READ getGeometryFileName WRITE setGeometryFileName)

    SIMPL_FILTER_PARAMETER(int, HomogenizationIndex)
    Q_PROPERTY(int HomogenizationIndex READ getHomogenizationIndex WRITE setHomogenizationIndex)

    SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
    Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, CellEulerAnglesArrayPath)
    Q_PROPERTY(DataArrayPath CellEulerAnglesArrayPath READ getCellEulerAnglesArrayPath WRITE setCellEulerAnglesArrayPath)

    SIMPL_FILTER_PARAMETER(bool, CompressGeomFile)
    Q_PROPERTY(bool CompressGeomFile READ getCompressGeomFile WRITE setCompressGeomFile)

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
     * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
     */
    void setupFilterParameters() override;

  /**
   * @brief This method will read the options from a file
   * @param reader The reader that is used to read the options from a file
   */
    void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

    /**
     * @brief execute Reimplemented from @see AbstractFilter class
     */
    void execute() override;

    /**
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    void preflight() override;

  Q_SIGNALS:
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
    ExportDAMASKFiles();

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
    /* Rule of 5: All special member functions should be defined if any are defined.
    * https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-default-operation-define-or-delete-them-all
    */
    ExportDAMASKFiles(const ExportDAMASKFiles&) = delete;             // Copy Constructor Not Implemented
    ExportDAMASKFiles& operator=(const ExportDAMASKFiles&) = delete;  // Copy Assignment Not Implemented
    ExportDAMASKFiles(ExportDAMASKFiles&&) = delete;                  // Move Constructor Not Implemented
    ExportDAMASKFiles& operator=(ExportDAMASKFiles&&) = delete;       // Move Assignment Not Implemented

};

