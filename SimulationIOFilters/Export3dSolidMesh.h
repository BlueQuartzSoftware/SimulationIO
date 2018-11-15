/*
 * Your License or Copyright can go here
 */

#pragma once

#include <QtCore/QProcess>
#include <QtCore/QWaitCondition>
#include <QtCore/QMutex>
#include <QtCore/QSharedPointer>
#include <QtCore/QFile>
#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/Common/Constants.h"

class QProcess;

#include "SimulationIO/SimulationIOPlugin.h"

/**
 * @brief The Export3dSolidMesh class. See [Filter documentation](@ref export3dsolidmesh) for details.
 */
class SimulationIO_EXPORT Export3dSolidMesh : public AbstractFilter
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(Export3dSolidMesh SUPERCLASS AbstractFilter)
  PYB11_PROPERTY(int MeshingPackage READ getMeshingPackage WRITE setMeshingPackage)
  PYB11_PROPERTY(QString outputPath READ getoutputPath WRITE setoutputPath)
  PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)
  PYB11_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)
  PYB11_PROPERTY(DataArrayPath FeatureCentroidArrayPath READ getFeatureCentroidArrayPath WRITE setFeatureCentroidArrayPath)
    
  PYB11_PROPERTY(QString TetDataContainerName READ getTetDataContainerName WRITE setTetDataContainerName)
  PYB11_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)
  PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

  public:
    SIMPL_SHARED_POINTERS(Export3dSolidMesh)
    SIMPL_FILTER_NEW_MACRO(Export3dSolidMesh)
    SIMPL_TYPE_MACRO_SUPER(Export3dSolidMesh, AbstractFilter)

    ~Export3dSolidMesh() override;

    SIMPL_FILTER_PARAMETER(int, MeshingPackage)
    Q_PROPERTY(int MeshingPackage READ getMeshingPackage WRITE setMeshingPackage)

    SIMPL_FILTER_PARAMETER(QString, outputPath)
    Q_PROPERTY(QString outputPath READ getoutputPath WRITE setoutputPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)
    
    SIMPL_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
    Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
    
    SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureEulerAnglesArrayPath)
    Q_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureCentroidArrayPath)
    Q_PROPERTY(DataArrayPath FeatureCentroidArrayPath READ getFeatureCentroidArrayPath WRITE setFeatureCentroidArrayPath)

    SIMPL_FILTER_PARAMETER(QString, TetDataContainerName)
    Q_PROPERTY(QString TetDataContainerName READ getTetDataContainerName WRITE setTetDataContainerName)

    SIMPL_FILTER_PARAMETER(QString, VertexAttributeMatrixName)
    Q_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)

    SIMPL_FILTER_PARAMETER(QString, CellAttributeMatrixName)
    Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

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
    Export3dSolidMesh();

    /**
    * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
    */
    void dataCheck();

    /**
    * @brief Initializes all the private instance variables.
    */
    void initialize();

  protected slots:
      void processHasFinished(int exitCode, QProcess::ExitStatus exitStatus);
      void processHasErroredOut(QProcess::ProcessError error);
      void sendErrorOutput();
      void sendStandardOutput();

  private:
    DEFINE_DATAARRAY_VARIABLE(float, FeatureEulerAngles)
    DEFINE_DATAARRAY_VARIABLE(float, FeatureCentroid)
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeaturePhases)

    void runTetgen(const QString& file); 

    void createTetgenInpFile(const QString& file, int64_t numNodes, float* nodes, int64_t numTri, int64_t* triangles, size_t numfeatures, float* centroid); 

    QWaitCondition m_WaitCondition;
    QMutex m_Mutex;                                              
    bool m_Pause = false;
    QSharedPointer<QProcess> m_ProcessPtr;                           
    QStringList arguments;
  
  public:
    /* Rule of 5: All special member functions should be defined if any are defined.
    * https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-default-operation-define-or-delete-them-all
    */
    Export3dSolidMesh(const Export3dSolidMesh&) = delete;             // Copy Constructor Not Implemented
    Export3dSolidMesh& operator=(const Export3dSolidMesh&) = delete;  // Copy Assignment Not Implemented
    Export3dSolidMesh(Export3dSolidMesh&&) = delete;                  // Move Constructor Not Implemented
    Export3dSolidMesh& operator=(Export3dSolidMesh&&) = delete;       // Move Assignment Not Implemented

};

