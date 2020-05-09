/*
 * Your License or Copyright can go here
 */

#pragma once

#include <memory>

#include <QtCore/QMutex>
#include <QtCore/QProcess>
#include <QtCore/QSharedPointer>
#include <QtCore/QString>
#include <QtCore/QWaitCondition>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Geometry/IGeometry.h"

#include "SimulationIO/SimulationIOPlugin.h"

class QProcess;
class DataContainer;

/**
 * @brief The Export3dSolidMesh class. See [Filter documentation](@ref export3dsolidmesh) for details.
 */
class SimulationIO_EXPORT Export3dSolidMesh : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(Export3dSolidMesh SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(Export3dSolidMesh)
  PYB11_FILTER_NEW_MACRO(Export3dSolidMesh)
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
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = Export3dSolidMesh;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static std::shared_ptr<Export3dSolidMesh> New();

  /**
   * @brief Returns the name of the class for Export3dSolidMesh
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for Export3dSolidMesh
   */
  static QString ClassName();

  ~Export3dSolidMesh() override;

  /**
   * @brief Setter property for MeshingPackage
   */
  void setMeshingPackage(int value);
  /**
   * @brief Getter property for MeshingPackage
   * @return Value of MeshingPackage
   */
  int getMeshingPackage() const;
  Q_PROPERTY(int MeshingPackage READ getMeshingPackage WRITE setMeshingPackage)

  /**
   * @brief Setter property for outputPath
   */
  void setoutputPath(const QString& value);
  /**
   * @brief Getter property for outputPath
   * @return Value of outputPath
   */
  QString getoutputPath() const;
  Q_PROPERTY(QString outputPath READ getoutputPath WRITE setoutputPath)

  /**
   * @brief Setter property for PackageLocation
   */
  void setPackageLocation(const QString& value);
  /**
   * @brief Getter property for PackageLocation
   * @return Value of PackageLocation
   */
  QString getPackageLocation() const;
  Q_PROPERTY(QString PackageLocation READ getPackageLocation WRITE setPackageLocation)

  /**
   * @brief Setter property for SurfaceMeshFaceLabelsArrayPath
   */
  void setSurfaceMeshFaceLabelsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SurfaceMeshFaceLabelsArrayPath
   * @return Value of SurfaceMeshFaceLabelsArrayPath
   */
  DataArrayPath getSurfaceMeshFaceLabelsArrayPath() const;
  Q_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)

  /**
   * @brief Setter property for FeaturePhasesArrayPath
   */
  void setFeaturePhasesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeaturePhasesArrayPath
   * @return Value of FeaturePhasesArrayPath
   */
  DataArrayPath getFeaturePhasesArrayPath() const;
  Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

  /**
   * @brief Setter property for FeatureEulerAnglesArrayPath
   */
  void setFeatureEulerAnglesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeatureEulerAnglesArrayPath
   * @return Value of FeatureEulerAnglesArrayPath
   */
  DataArrayPath getFeatureEulerAnglesArrayPath() const;
  Q_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)

  /**
   * @brief Setter property for FeatureCentroidArrayPath
   */
  void setFeatureCentroidArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeatureCentroidArrayPath
   * @return Value of FeatureCentroidArrayPath
   */
  DataArrayPath getFeatureCentroidArrayPath() const;
  Q_PROPERTY(DataArrayPath FeatureCentroidArrayPath READ getFeatureCentroidArrayPath WRITE setFeatureCentroidArrayPath)

  /**
   * @brief Setter property for RefineMesh
   */
  void setRefineMesh(bool value);
  /**
   * @brief Getter property for RefineMesh
   * @return Value of RefineMesh
   */
  bool getRefineMesh() const;
  Q_PROPERTY(bool RefineMesh READ getRefineMesh WRITE setRefineMesh)

  /**
   * @brief Setter property for MaxRadiusEdgeRatio
   */
  void setMaxRadiusEdgeRatio(float value);
  /**
   * @brief Getter property for MaxRadiusEdgeRatio
   * @return Value of MaxRadiusEdgeRatio
   */
  float getMaxRadiusEdgeRatio() const;
  Q_PROPERTY(float MaxRadiusEdgeRatio READ getMaxRadiusEdgeRatio WRITE setMaxRadiusEdgeRatio)

  /**
   * @brief Setter property for MinDihedralAngle
   */
  void setMinDihedralAngle(float value);
  /**
   * @brief Getter property for MinDihedralAngle
   * @return Value of MinDihedralAngle
   */
  float getMinDihedralAngle() const;
  Q_PROPERTY(float MinDihedralAngle READ getMinDihedralAngle WRITE setMinDihedralAngle)

  /**
   * @brief Setter property for LimitTetrahedraVolume
   */
  void setLimitTetrahedraVolume(bool value);
  /**
   * @brief Getter property for LimitTetrahedraVolume
   * @return Value of LimitTetrahedraVolume
   */
  bool getLimitTetrahedraVolume() const;
  Q_PROPERTY(bool LimitTetrahedraVolume READ getLimitTetrahedraVolume WRITE setLimitTetrahedraVolume)

  /**
   * @brief Setter property for MaxTetrahedraVolume
   */
  void setMaxTetrahedraVolume(float value);
  /**
   * @brief Getter property for MaxTetrahedraVolume
   * @return Value of MaxTetrahedraVolume
   */
  float getMaxTetrahedraVolume() const;
  Q_PROPERTY(float MaxTetrahedraVolume READ getMaxTetrahedraVolume WRITE setMaxTetrahedraVolume)

  /**
   * @brief Setter property for OptimizationLevel
   */
  void setOptimizationLevel(int value);
  /**
   * @brief Getter property for OptimizationLevel
   * @return Value of OptimizationLevel
   */
  int getOptimizationLevel() const;
  Q_PROPERTY(int OptimizationLevel READ getOptimizationLevel WRITE setOptimizationLevel)

  /**
   * @brief Setter property for IncludeHolesUsingPhaseID
   */
  void setIncludeHolesUsingPhaseID(bool value);
  /**
   * @brief Getter property for IncludeHolesUsingPhaseID
   * @return Value of IncludeHolesUsingPhaseID
   */
  bool getIncludeHolesUsingPhaseID() const;
  Q_PROPERTY(bool IncludeHolesUsingPhaseID READ getIncludeHolesUsingPhaseID WRITE setIncludeHolesUsingPhaseID)

  /**
   * @brief Setter property for PhaseID
   */
  void setPhaseID(int value);
  /**
   * @brief Getter property for PhaseID
   * @return Value of PhaseID
   */
  int getPhaseID() const;
  Q_PROPERTY(int PhaseID READ getPhaseID WRITE setPhaseID)

  /**
   * @brief Setter property for TetDataContainerName
   */
  void setTetDataContainerName(const QString& value);
  /**
   * @brief Getter property for TetDataContainerName
   * @return Value of TetDataContainerName
   */
  QString getTetDataContainerName() const;
  Q_PROPERTY(QString TetDataContainerName READ getTetDataContainerName WRITE setTetDataContainerName)

  /**
   * @brief Setter property for VertexAttributeMatrixName
   */
  void setVertexAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for VertexAttributeMatrixName
   * @return Value of VertexAttributeMatrixName
   */
  QString getVertexAttributeMatrixName() const;
  Q_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)

  /**
   * @brief Setter property for CellAttributeMatrixName
   */
  void setCellAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for CellAttributeMatrixName
   * @return Value of CellAttributeMatrixName
   */
  QString getCellAttributeMatrixName() const;
  Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

  /**
   * @brief Setter property for GmshSTLFileName
   */
  void setGmshSTLFileName(const QString& value);
  /**
   * @brief Getter property for GmshSTLFileName
   * @return Value of GmshSTLFileName
   */
  QString getGmshSTLFileName() const;
  Q_PROPERTY(QString GmshSTLFileName READ getGmshSTLFileName WRITE setGmshSTLFileName)

  /**
   * @brief Setter property for MeshFileFormat
   */
  void setMeshFileFormat(int value);
  /**
   * @brief Getter property for MeshFileFormat
   * @return Value of MeshFileFormat
   */
  int getMeshFileFormat() const;
  Q_PROPERTY(int MeshFileFormat READ getMeshFileFormat WRITE setMeshFileFormat)

  /**
   * @brief Setter property for NetgenSTLFileName
   */
  void setNetgenSTLFileName(const QString& value);
  /**
   * @brief Getter property for NetgenSTLFileName
   * @return Value of NetgenSTLFileName
   */
  QString getNetgenSTLFileName() const;
  Q_PROPERTY(QString NetgenSTLFileName READ getNetgenSTLFileName WRITE setNetgenSTLFileName)

  /**
   * @brief Setter property for MeshSize
   */
  void setMeshSize(int value);
  /**
   * @brief Getter property for MeshSize
   * @return Value of MeshSize
   */
  int getMeshSize() const;
  Q_PROPERTY(int MeshSize READ getMeshSize WRITE setMeshSize)

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

protected:
  Export3dSolidMesh();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

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
  bool m_IncludeHolesUsingPhaseID = {false};
  int m_PhaseID = {2};

  std::weak_ptr<DataArray<float>> m_FeatureEulerAnglesPtr;
  float* m_FeatureEulerAngles = nullptr;
  std::weak_ptr<DataArray<float>> m_FeatureCentroidPtr;
  float* m_FeatureCentroid = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_FeaturePhasesPtr;
  int32_t* m_FeaturePhases = nullptr;

  int m_MeshingPackage = {0};
  QString m_outputPath = {};
  QString m_PackageLocation = {};
  DataArrayPath m_SurfaceMeshFaceLabelsArrayPath = {SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels};
  DataArrayPath m_FeaturePhasesArrayPath = {SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Phases};
  DataArrayPath m_FeatureEulerAnglesArrayPath = {SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::EulerAngles};
  DataArrayPath m_FeatureCentroidArrayPath = {SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Centroids};
  bool m_RefineMesh = {true};
  float m_MaxRadiusEdgeRatio = {2.0f};
  float m_MinDihedralAngle = {0.0f};
  bool m_LimitTetrahedraVolume = {false};
  float m_MaxTetrahedraVolume = {0.1f};
  int m_OptimizationLevel = {2};
  QString m_TetDataContainerName = {SIMPL::Defaults::TetrahedralDataContainerName};
  QString m_VertexAttributeMatrixName = {SIMPL::Defaults::VertexAttributeMatrixName};
  QString m_CellAttributeMatrixName = {SIMPL::Defaults::CellAttributeMatrixName};
  QString m_GmshSTLFileName = {};
  int m_MeshFileFormat = {0};
  QString m_NetgenSTLFileName = {};
  int m_MeshSize = {0};

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
