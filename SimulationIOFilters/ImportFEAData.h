/*
 * Your License or Copyright can go here
 */

#pragma once

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/Common/Constants.h"

/**
 * @brief The ImportFEAData class. See [Filter documentation](@ref importfeadata) for details.
 */
class ImportFEAData : public AbstractFilter
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(CreateGeometry SUPERCLASS AbstractFilter)
  PYB11_PROPERTY(int FEAPackage READ getFEAPackage WRITE setFEAPackage)
  PYB11_PROPERTY(QString odbName READ getodbName WRITE setodbName)
  PYB11_PROPERTY(QString odbFilePath READ getodbFilePath WRITE setodbFilePath)
  PYB11_PROPERTY(QString InstanceName READ getInstanceName WRITE setInstanceName)
  PYB11_PROPERTY(QString Step READ getStep WRITE setStep)
  PYB11_PROPERTY(int FrameNumber READ getFrameNumber WRITE setFrameNumber)
  PYB11_PROPERTY(QString OutputVariable READ getOutputVariable WRITE setOutputVariable)
  PYB11_PROPERTY(QString ElementSet READ getElementSet WRITE setElementSet)
  public:
    SIMPL_SHARED_POINTERS(ImportFEAData)
    SIMPL_FILTER_NEW_MACRO(ImportFEAData)
    SIMPL_TYPE_MACRO_SUPER(ImportFEAData, AbstractFilter)

    ~ImportFEAData() override;

    SIMPL_FILTER_PARAMETER(int, FEAPackage)
    Q_PROPERTY(int FEAPackage READ getFEAPackage WRITE setFEAPackage)

    SIMPL_FILTER_PARAMETER(QString, odbName)
    Q_PROPERTY(QString odbName READ getodbName WRITE setodbName)

    SIMPL_FILTER_PARAMETER(QString, odbFilePath)
    Q_PROPERTY(QString odbFilePath READ getodbFilePath WRITE setodbFilePath)

    SIMPL_FILTER_PARAMETER(QString, InstanceName)
    Q_PROPERTY(QString InstanceName READ getInstanceName WRITE setInstanceName)

    SIMPL_FILTER_PARAMETER(QString, Step)
    Q_PROPERTY(QString Step READ getStep WRITE setStep)

    SIMPL_FILTER_PARAMETER(int, FrameNumber)
    Q_PROPERTY(int FrameNumber READ getFrameNumber WRITE setFrameNumber)
    
    SIMPL_FILTER_PARAMETER(QString, OutputVariable)
    Q_PROPERTY(QString OutputVariable READ getOutputVariable WRITE setOutputVariable)

    SIMPL_FILTER_PARAMETER(QString, ElementSet)
    Q_PROPERTY(QString ElementSet READ getElementSet WRITE setElementSet)

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
     * @brief readFilterParameters Reimplemented from @see AbstractFilter class
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
    ImportFEAData();

    /**
    * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
    */
    void dataCheck();

    /**
    * @brief Initializes all the private instance variables.
    */
    void initialize();
  private:

  
  public:
    /* Rule of 5: All special member functions should be defined if any are defined.
    * https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-default-operation-define-or-delete-them-all
    */
    ImportFEAData(const ImportFEAData&) = delete;             // Copy Constructor Not Implemented
    ImportFEAData& operator=(const ImportFEAData&) = delete;  // Copy Assignment Not Implemented
    ImportFEAData(ImportFEAData&&) = delete;                  // Move Constructor Not Implemented
    ImportFEAData& operator=(ImportFEAData&&) = delete;       // Move Assignment Not Implemented

};

