/*
 * Your License or Copyright can go here
 */

#pragma once

#include <QtCore/QMutex>
#include <QtCore/QProcess>
#include <QtCore/QSharedPointer>
#include <QtCore/QString>
#include <QtCore/QWaitCondition>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

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
  PYB11_PROPERTY(QString PackageLocation READ getPackageLocation WRITE setPackageLocation)
  PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)
  PYB11_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)
  PYB11_PROPERTY(DataArrayPath FeatureCentroidArrayPath READ getFeatureCentroidArrayPath WRITE setFeatureCentroidArrayPath)

  PYB11_PROPERTY(bool RefineMesh READ getRefineMesh WRITE setRefineMesh)
  PYB11_PROPERTY(float MaxRadiusEdgeRatio READ getMaxRadiusEdgeRatio WRITE setMaxRadiusEdgeRatio)
  PYB11_PROPERTY(float MinDihedralAngle READ getMinDihedralAngle WRITE setMinDihedralAngle)
  PYB11_PROPERTY(bool LimitTetrahedraVolume READ getLimitTetrahedraVolume WRITE setLimitTetrahedraVolume)
  PYB11_PROPERTY(float MaxTetrahedraVolume READ getMaxTetrahedraVolume WRITE setMaxTetrahedraVolume)
  PYB11_PROPERTY(int OptimizationLevel READ getOptimizationLevel WRITE setOptimizationLevel)
  PYB11_PROPERTY(bool IncludeHolesUsingPhaseID READ getIncludeHolesUsingPhaseID WRITE setIncludeHolesUsingPhaseID)
  PYB11_PROPERTY(int PhaseID READ getPhaseID WRITE setPhaseID)

  PYB11_PROPERTY(QString TetDataContainerName READ getTetDataContainerName WRITE setTetDataContainerName)
  PYB11_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)
  PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

  PYB11_PROPERTY(QString GmshSTLFileName READ getGmshSTLFileName WRITE setGmshSTLFileName)
  PYB11_PROPERTY(int MeshFileFormat READ getMeshFileFormat WRITE setMeshFileFormat)

  PYB11_PROPERTY(QString NetgenSTLFileName READ getNetgenSTLFileName WRITE setNetgenSTLFileName)
  PYB11_PROPERTY(int MeshSize READ getMeshSize WRITE setMeshSize)

public:
  SIMPL_SHARED_POINTERS(Export3dSolidMesh)
  SIMPL_FILTER_NEW_MACRO(Export3dSolidMesh)
  SIMPL_TYPE_MACRO_SUPER(Export3dSolidMesh, AbstractFilter)

  ~Export3dSolidMesh() override;

  SIMPL_FILTER_PARAMETER(int, MeshingPackage)
  Q_PROPERTY(int MeshingPackage READ getMeshingPackage WRITE setMeshingPackage)

  SIMPL_FILTER_PARAMETER(QString, outputPath)
  Q_PROPERTY(QString outputPath READ getoutputPath WRITE setoutputPath)

  SIMPL_FILTER_PARAMETER(QString, PackageLocation)
  Q_PROPERTY(QString PackageLocation READ getPackageLocation WRITE setPackageLocation)

  SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
  Q_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
  Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureEulerAnglesArrayPath)
  Q_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureCentroidArrayPath)
  Q_PROPERTY(DataArrayPath FeatureCentroidArrayPath READ getFeatureCentroidArrayPath WRITE setFeatureCentroidArrayPath)

  SIMPL_FILTER_PARAMETER(bool, RefineMesh)
  Q_PROPERTY(bool RefineMesh READ getRefineMesh WRITE setRefineMesh)

  SIMPL_FILTER_PARAMETER(float, MaxRadiusEdgeRatio)
  Q_PROPERTY(float MaxRadiusEdgeRatio READ getMaxRadiusEdgeRatio WRITE setMaxRadiusEdgeRatio)

  SIMPL_FILTER_PARAMETER(float, MinDihedralAngle)
  Q_PROPERTY(float MinDihedralAngle READ getMinDihedralAngle WRITE setMinDihedralAngle)

  SIMPL_FILTER_PARAMETER(bool, LimitTetrahedraVolume)
  Q_PROPERTY(bool LimitTetrahedraVolume READ getLimitTetrahedraVolume WRITE setLimitTetrahedraVolume)

  SIMPL_FILTER_PARAMETER(float, MaxTetrahedraVolume)
  Q_PROPERTY(float MaxTetrahedraVolume READ getMaxTetrahedraVolume WRITE setMaxTetrahedraVolume)

  SIMPL_FILTER_PARAMETER(int, OptimizationLevel)
  Q_PROPERTY(int OptimizationLevel READ getOptimizationLevel WRITE setOptimizationLevel)

  SIMPL_FILTER_PARAMETER(bool, IncludeHolesUsingPhaseID)
  Q_PROPERTY(bool IncludeHolesUsingPhaseID READ getIncludeHolesUsingPhaseID WRITE setIncludeHolesUsingPhaseID)

  SIMPL_FILTER_PARAMETER(int, PhaseID)
  Q_PROPERTY(int PhaseID READ getPhaseID WRITE setPhaseID)

  SIMPL_FILTER_PARAMETER(QString, TetDataContainerName)
  Q_PROPERTY(QString TetDataContainerName READ getTetDataContainerName WRITE setTetDataContainerName)

  SIMPL_FILTER_PARAMETER(QString, VertexAttributeMatrixName)
  Q_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)

  SIMPL_FILTER_PARAMETER(QString, CellAttributeMatrixName)
  Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

  SIMPL_FILTER_PARAMETER(QString, GmshSTLFileName)
  Q_PROPERTY(QString GmshSTLFileName READ getGmshSTLFileName WRITE setGmshSTLFileName)

  SIMPL_FILTER_PARAMETER(int, MeshFileFormat)
  Q_PROPERTY(int MeshFileFormat READ getMeshFileFormat WRITE setMeshFileFormat)

  SIMPL_FILTER_PARAMETER(QString, NetgenSTLFileName)
  Q_PROPERTY(QString NetgenSTLFileName READ getNetgenSTLFileName WRITE setNetgenSTLFileName)

  SIMPL_FILTER_PARAMETER(int, MeshSize)
  Q_PROPERTY(int MeshSize READ getMeshSize WRITE setMeshSize)

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

  void runPackage(const QString& file, const QString& meshFile);
  void mergeMesh(const QString& mergefile, const QString& indivFile);

  void createTetgenInpFile(const QString& file, MeshIndexType numNodes, float* nodes, MeshIndexType numTri, MeshIndexType* triangles, size_t numfeatures, float* centroid);

  QWaitCondition m_WaitCondition;
  QMutex m_Mutex;
  bool m_Pause = false;
  QSharedPointer<QProcess> m_ProcessPtr;
  QStringList arguments;

  void scanTetGenFile(const QString& fileEle, const QString& fileNode, DataContainer* dataContainer, AttributeMatrix* vertexAttributeMatrix, AttributeMatrix* cellAttributeMatrix);

public:
  /* Rule of 5: All special member functions should be defined if any are defined.
   * https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-default-operation-define-or-delete-them-all
   */
  Export3dSolidMesh(const Export3dSolidMesh&) = delete;            // Copy Constructor Not Implemented
  Export3dSolidMesh& operator=(const Export3dSolidMesh&) = delete; // Copy Assignment Not Implemented
  Export3dSolidMesh(Export3dSolidMesh&&) = delete;                 // Move Constructor Not Implemented
  Export3dSolidMesh& operator=(Export3dSolidMesh&&) = delete;      // Move Assignment Not Implemented
};
