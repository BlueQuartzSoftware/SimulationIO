/*
 * Your License or Copyright can go here
 */

#include "Export3dSolidMesh.h"

#include <QtCore/QDir>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SIMPLib/FilterParameters/DynamicTableFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Utilities/FileSystemPathHelper.h"

#include "SimulationIO/SimulationIOConstants.h"
#include "SimulationIO/SimulationIOVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Export3dSolidMesh::Export3dSolidMesh()  
: AbstractFilter()
, m_MeshingPackage(0)
, m_SurfaceMeshFaceLabelsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels)
, m_FeaturePhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Phases)
, m_FeatureEulerAnglesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::EulerAngles)
, m_FeatureCentroidArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Centroids)
, m_FeatureEulerAngles(nullptr)
, m_FeaturePhases(nullptr)
, m_FeatureCentroid(nullptr)

{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Export3dSolidMesh::~Export3dSolidMesh() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Export3dSolidMesh::initialize()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Export3dSolidMesh::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Meshing package");
    parameter->setPropertyName("MeshingPackage");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(Export3dSolidMesh, this, MeshingPackage));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(Export3dSolidMesh, this, MeshingPackage));
    QVector<QString> choices;
    choices.push_back("TetGen");
    parameter->setChoices(choices);
    QStringList linkedProps = {"FaceFeatureIdsArrayPath"};
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Labels", SurfaceMeshFaceLabelsArrayPath, FilterParameter::RequiredArray, Export3dSolidMesh, req));
  }

  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
      DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Euler Angles", FeatureEulerAnglesArrayPath, FilterParameter::RequiredArray, Export3dSolidMesh, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
      DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", FeaturePhasesArrayPath, FilterParameter::RequiredArray, Export3dSolidMesh, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
      DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Centroids", FeatureCentroidArrayPath, FilterParameter::RequiredArray, Export3dSolidMesh, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void Export3dSolidMesh::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath()));
  setFeatureEulerAnglesArrayPath(reader->readDataArrayPath("FeatureEulerAnglesArrayPath", getFeatureEulerAnglesArrayPath()));
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath()));
  setFeatureCentroidArrayPath(reader->readDataArrayPath("FeatureCentroidArrayPath", getFeatureCentroidArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Export3dSolidMesh::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0); 

  QVector<DataArrayPath> dataArrayPaths;
  QVector<size_t> cDims(1, 1);

  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(),
                                                                                                           cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeaturePhasesPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getFeaturePhasesArrayPath());
  }
  
  cDims[0] = 3;
  m_FeatureEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getFeatureEulerAnglesArrayPath(),
													      cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureEulerAnglesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
    {
      dataArrayPaths.push_back(getFeatureEulerAnglesArrayPath());
    }

  cDims[0] = 3;
  m_FeatureCentroidPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getFeatureCentroidArrayPath(),
													      cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureCentroidPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_FeatureCentroid = m_FeatureCentroidPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
    {
      dataArrayPaths.push_back(getFeatureCentroidArrayPath());
    }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Export3dSolidMesh::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void Export3dSolidMesh::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());
  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();  

  int64_t numNodes = triangleGeom->getNumberOfVertices();
  float* nodes = triangleGeom->getVertexPointer(0);

  int64_t numTri = triangleGeom->getNumberOfTris();
  int64_t* triangles = triangleGeom->getTriPointer(0);

  size_t numfeatures = m_FeatureEulerAnglesPtr.lock()->getNumberOfTuples();


  if (getCancel()) { return; }
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer Export3dSolidMesh::newFilterInstance(bool copyFilterParameters) const
{
  Export3dSolidMesh::Pointer filter = Export3dSolidMesh::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Export3dSolidMesh::getCompiledLibraryName() const
{ 
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Export3dSolidMesh::getBrandingString() const
{
  return "SimulationIO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Export3dSolidMesh::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SimulationIO::Version::Major() << "." << SimulationIO::Version::Minor() << "." << SimulationIO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Export3dSolidMesh::getGroupName() const
{ 
  return SIMPL::FilterGroups::Unsupported; 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Export3dSolidMesh::getSubGroupName() const
{ 
  return "SimulationIO"; 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Export3dSolidMesh::getHumanLabel() const
{ 
  return "Export 3d Solid Mesh"; 
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid Export3dSolidMesh::getUuid()
{
  return QUuid("{fcff3b03-bff6-5511-bc65-5e558d12f0a6}");
}

