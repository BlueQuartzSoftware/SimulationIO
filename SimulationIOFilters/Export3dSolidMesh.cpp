/*
 * Your License or Copyright can go here
 */

#include "Export3dSolidMesh.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SIMPLib/FilterParameters/DynamicTableFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/TetrahedralGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Utilities/FileSystemPathHelper.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "SimulationIO/SimulationIOConstants.h"
#include "SimulationIO/SimulationIOVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Export3dSolidMesh::Export3dSolidMesh()
: m_outputPath("")
, m_PackageLocation("")
, m_GmshSTLFileName("")
, m_NetgenSTLFileName("")
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
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Export3dSolidMesh::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Meshing package");
    parameter->setPropertyName("MeshingPackage");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(Export3dSolidMesh, this, MeshingPackage));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(Export3dSolidMesh, this, MeshingPackage));
    QVector<QString> choices;
    choices.push_back("TetGen");
    choices.push_back("Netgen");
    choices.push_back("Gmsh");
    parameter->setChoices(choices);
    QStringList linkedProps = {"SurfaceMeshFaceLabelsArrayPath",
                               "FeaturePhasesArrayPath",
                               "FeatureCentroidArrayPath",
                               "RefineMesh",
                               "MaxRadiusEdgeRatio",
                               "MinDihedralAngle",
                               "OptimizationLevel",
                               "LimitTetrahedraVolume",
                               "MaxTetrahedraVolume",
                               "IncludeHolesUsingPhaseID",
                               "PhaseID",
                               "TetDataContainerName",
                               "VertexAttributeMatrixName",
                               "CellAttributeMatrixName",
                               "GmshSTLFileName",
                               "NetgenSTLFileName",
                               "MeshSize",
                               "MeshFileFormat"};
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
    linkedProps.clear();
  }

  parameters.push_back(SIMPL_NEW_OUTPUT_PATH_FP("Path", outputPath, FilterParameter::Parameter, Export3dSolidMesh, "*", "*"));
  parameters.push_back(SIMPL_NEW_OUTPUT_PATH_FP("Package Location", PackageLocation, FilterParameter::Parameter, Export3dSolidMesh, "*", "*"));

  {
    parameters.push_back(SIMPL_NEW_STRING_FP("STL File Prefix", NetgenSTLFileName, FilterParameter::Parameter, Export3dSolidMesh, 1));
  }

  {
    parameters.push_back(SIMPL_NEW_STRING_FP("STL File Prefix", GmshSTLFileName, FilterParameter::Parameter, Export3dSolidMesh, 2));
  }

  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Labels", SurfaceMeshFaceLabelsArrayPath, FilterParameter::RequiredArray, Export3dSolidMesh, req, 0));
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
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", FeaturePhasesArrayPath, FilterParameter::RequiredArray, Export3dSolidMesh, req, 0));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Centroids", FeatureCentroidArrayPath, FilterParameter::RequiredArray, Export3dSolidMesh, req, 0));
  }

  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Mesh File Format");
    parameter->setPropertyName("MeshFileFormat");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(Export3dSolidMesh, this, MeshFileFormat));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(Export3dSolidMesh, this, MeshFileFormat));

    QVector<QString> choices;
    choices.push_back("msh");
    choices.push_back("inp");
    parameter->setChoices(choices);
    parameter->setGroupIndex(2);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  parameters.push_back(SeparatorFilterParameter::New("Mesh Quality Options", FilterParameter::Parameter));
  {
    QStringList linkedProps = {"MaxRadiusEdgeRatio", "MinDihedralAngle"};
    parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Refine Mesh (q)", RefineMesh, FilterParameter::Parameter, Export3dSolidMesh, linkedProps, 0));
    linkedProps.clear();
    parameters.push_back(SIMPL_NEW_FLOAT_FP("Maximum Radius-Edge Ratio", MaxRadiusEdgeRatio, FilterParameter::Parameter, Export3dSolidMesh, 0));
    parameters.push_back(SIMPL_NEW_FLOAT_FP("Minimum Dihedral Angle", MinDihedralAngle, FilterParameter::Parameter, Export3dSolidMesh, 0));
    parameters.push_back(SIMPL_NEW_INTEGER_FP("Optimization Level (O)", OptimizationLevel, FilterParameter::Parameter, Export3dSolidMesh, 0));
  }
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Mesh Size");
    parameter->setPropertyName("MeshSize");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(Export3dSolidMesh, this, MeshSize));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(Export3dSolidMesh, this, MeshSize));

    QVector<QString> choices;
    choices.push_back("very coarse");
    choices.push_back("coarse");
    choices.push_back("moderate");
    choices.push_back("fine");
    choices.push_back("very fine");
    parameter->setChoices(choices);
    parameter->setGroupIndex(1);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  {
    parameters.push_back(SeparatorFilterParameter::New("Topology Options", FilterParameter::Parameter));
    QStringList linkedProps = {"MaxTetrahedraVolume"};
    parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Limit Tetrahedra Volume (a)", LimitTetrahedraVolume, FilterParameter::Parameter, Export3dSolidMesh, linkedProps, 0));
    linkedProps.clear();
    parameters.push_back(SIMPL_NEW_FLOAT_FP("Maximum Tetrahedron Volume", MaxTetrahedraVolume, FilterParameter::Parameter, Export3dSolidMesh, 0));
  }

  {
    parameters.push_back(SeparatorFilterParameter::New("Holes in the Mesh", FilterParameter::Parameter));
    QStringList linkedProps = {"PhaseID"};
    parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Include Holes Using PhaseID", IncludeHolesUsingPhaseID, FilterParameter::Parameter, Export3dSolidMesh, linkedProps, 0));
    linkedProps.clear();
    parameters.push_back(SIMPL_NEW_INTEGER_FP("PhaseID", PhaseID, FilterParameter::Parameter, Export3dSolidMesh, 0));
  }

  {
    parameters.push_back(SeparatorFilterParameter::New("", FilterParameter::CreatedArray));
    parameters.push_back(SIMPL_NEW_STRING_FP("Data Container Name", TetDataContainerName, FilterParameter::CreatedArray, Export3dSolidMesh, 0));
    parameters.push_back(SIMPL_NEW_STRING_FP("Vertex Attribute Matrix Name", VertexAttributeMatrixName, FilterParameter::CreatedArray, Export3dSolidMesh, 0));
    parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix Name", CellAttributeMatrixName, FilterParameter::CreatedArray, Export3dSolidMesh, 0));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void Export3dSolidMesh::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setoutputPath(reader->readString("outputPath", getoutputPath()));
  setPackageLocation(reader->readString("PackageLocation", getPackageLocation()));
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath()));
  setFeatureEulerAnglesArrayPath(reader->readDataArrayPath("FeatureEulerAnglesArrayPath", getFeatureEulerAnglesArrayPath()));
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath()));
  setFeatureCentroidArrayPath(reader->readDataArrayPath("FeatureCentroidArrayPath", getFeatureCentroidArrayPath()));
  setTetDataContainerName(reader->readString("DataContainerName", getTetDataContainerName()));
  setVertexAttributeMatrixName(reader->readString("VertexAttributeMatrixName", getVertexAttributeMatrixName()));
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName()));
  setNetgenSTLFileName(reader->readString("NetgenSTLFileName", getNetgenSTLFileName()));
  setMeshSize(reader->readValue("MeshSize", getMeshSize()));
  setGmshSTLFileName(reader->readString("GmshSTLFileName", getGmshSTLFileName()));
  setMeshFileFormat(reader->readValue("MeshFileFormat", getMeshFileFormat()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Export3dSolidMesh::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  m_Pause = false;
  m_ProcessPtr.reset();

  switch(m_MeshingPackage)
  {
  case 0: // TetGen
  {
    if(getRefineMesh())
    {
      if(getMaxRadiusEdgeRatio() <= 0)
      {
        setErrorCondition(-1, "Maximum radius-edge ratio must be greater than 0");
      }
      if(getMinDihedralAngle() < 0)
      {
        setErrorCondition(-1, "Minimum dihedral angle must be 0 or greater");
      }
    }

    if(getLimitTetrahedraVolume())
    {
      if(getMaxTetrahedraVolume() <= 0)
      {
        setErrorCondition(-1, "Maximum tetrahedron volume must be greater than 0");
      }
    }

    if(getOptimizationLevel() < 0 || getOptimizationLevel() > 10)
    {
      setErrorCondition(-1, "Optimization level must be on the interval [0, 10]");
    }

    QVector<DataArrayPath> dataArrayPaths;
    std::vector<size_t> cDims(1, 1);

    m_FeaturePhasesPtr =
        getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>>(this, getFeaturePhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_FeaturePhasesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCode() >= 0)
    {
      dataArrayPaths.push_back(getFeaturePhasesArrayPath());
    }

    cDims[0] = 3;
    m_FeatureEulerAnglesPtr =
        getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>>(this, getFeatureEulerAnglesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_FeatureEulerAnglesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCode() >= 0)
    {
      dataArrayPaths.push_back(getFeatureEulerAnglesArrayPath());
    }

    cDims[0] = 3;
    m_FeatureCentroidPtr =
        getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>>(this, getFeatureCentroidArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_FeatureCentroidPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_FeatureCentroid = m_FeatureCentroidPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCode() >= 0)
    {
      dataArrayPaths.push_back(getFeatureCentroidArrayPath());
    }

    getDataContainerArray()->validateNumberOfTuples(this, dataArrayPaths);

    // Create the output Data Container
    DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer(this, getTetDataContainerName());
    if(getErrorCode() < 0)
    {
      return;
    }

    // Create our output Vertex and Cell Matrix objects
    std::vector<size_t> tDims(1, 0);
    AttributeMatrix::Pointer vertexAttrMat = m->createNonPrereqAttributeMatrix(this, getVertexAttributeMatrixName(), tDims, AttributeMatrix::Type::Vertex);
    if(getErrorCode() < 0)
    {
      return;
    }
    AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell);
    if(getErrorCode() < 0)
    {
      return;
    }

    SharedVertexList::Pointer tetvertexPtr = TetrahedralGeom::CreateSharedVertexList(0);
    TetrahedralGeom::Pointer tetGeomPtr = TetrahedralGeom::CreateGeometry(0, tetvertexPtr, SIMPL::Geometry::TetrahedralGeometry, !getInPreflight());
    m->setGeometry(tetGeomPtr);

    break;
  }

  case 1: {
    QVector<DataArrayPath> dataArrayPaths;
    std::vector<size_t> cDims(1, 1);
    cDims[0] = 3;
    m_FeatureEulerAnglesPtr =
        getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>>(this, getFeatureEulerAnglesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_FeatureEulerAnglesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCode() >= 0)
    {
      dataArrayPaths.push_back(getFeatureEulerAnglesArrayPath());
    }

    break;
  }
  case 2: {
    QVector<DataArrayPath> dataArrayPaths;
    std::vector<size_t> cDims(1, 1);
    cDims[0] = 3;
    m_FeatureEulerAnglesPtr =
        getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>>(this, getFeatureEulerAnglesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_FeatureEulerAnglesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCode() >= 0)
    {
      dataArrayPaths.push_back(getFeatureEulerAnglesArrayPath());
    }

    break;
  }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Export3dSolidMesh::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  QDir dir;
  if(!dir.mkpath(m_outputPath))
  {
    QString ss = QObject::tr("Output directory does not exist '%1'").arg(m_outputPath);
    setErrorCondition(-1, ss);
    return;
  }

  switch(m_MeshingPackage)
  {
  case 0: // TetGen
  {
    DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());
    TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();

    MeshIndexType numNodes = triangleGeom->getNumberOfVertices();
    float* nodes = triangleGeom->getVertexPointer(0);

    MeshIndexType numTri = triangleGeom->getNumberOfTris();
    MeshIndexType* triangles = triangleGeom->getTriPointer(0);

    size_t numfeatures = m_FeatureEulerAnglesPtr.lock()->getNumberOfTuples();

    // creating TetGen input file
    QString tetgenInpFile = m_outputPath + QDir::separator() + "tetgenInp.smesh";

    createTetgenInpFile(tetgenInpFile, numNodes, nodes, numTri, triangles, numfeatures, m_FeatureCentroid);

    // running TetGen
    runPackage(tetgenInpFile, tetgenInpFile);

    DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getTetDataContainerName());
    AttributeMatrix::Pointer vertexAttrMat = m->getAttributeMatrix(getVertexAttributeMatrixName());
    AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());

    QString tetgenEleFile = m_outputPath + QDir::separator() + "tetgenInp.1.ele";
    QString tetgenNodeFile = m_outputPath + QDir::separator() + "tetgenInp.1.node";
    scanTetGenFile(tetgenEleFile, tetgenNodeFile, m.get(), vertexAttrMat.get(), cellAttrMat.get());

    break;
  }
  case 1: // Netgen
  {

    size_t numfeatures = m_FeatureEulerAnglesPtr.lock()->getNumberOfTuples();
    QString netgenMeshFile;
    QString mergedMesh = m_NetgenSTLFileName + "MergedMesh.vol";
    QString workPath = m_outputPath;
    QString asciiSTLFile;
    QString binSTLFile;

    for(size_t i = 1; i < numfeatures; i++)
    {
      asciiSTLFile = m_NetgenSTLFileName + QString("Feature_") + QString::number(i) + ".stl";
      binSTLFile = m_NetgenSTLFileName + QString("Feature_") + QString::number(i) + ".stlb";

      QDir::setCurrent(workPath);
      QFile::copy(asciiSTLFile, binSTLFile);
    }

    for(size_t i = 1; i < numfeatures; i++)
    {

      binSTLFile = m_NetgenSTLFileName + QString("Feature_") + QString::number(i) + ".stlb";
      netgenMeshFile = m_NetgenSTLFileName + QString("Feature_") + QString::number(i) + ".vol";

      // running Netgen
      runPackage(binSTLFile, netgenMeshFile);
    }

    netgenMeshFile = m_NetgenSTLFileName + QString("Feature_") + QString::number(1) + ".vol";
    QDir::setCurrent(workPath);
    QFile::copy(netgenMeshFile, mergedMesh);

    if(numfeatures > 2)
    {
      for(size_t i = 2; i < numfeatures; i++)
      {

        netgenMeshFile = m_NetgenSTLFileName + QString("Feature_") + QString::number(i) + ".vol";

        // running Netgen
        mergeMesh(mergedMesh, netgenMeshFile);
      }
    }

    for(size_t i = 1; i < numfeatures; i++)
    {
      binSTLFile = m_NetgenSTLFileName + QString("Feature_") + QString::number(i) + ".stlb";

      QDir::setCurrent(workPath);
      QFile::remove(binSTLFile);
    }

    break;
  }
  case 2: // Gmsh
  {

    size_t numfeatures = m_FeatureEulerAnglesPtr.lock()->getNumberOfTuples();
    // creating Gmsh .geo file
    QString gmshGeoFile = m_outputPath + QDir::separator() + "gmsh.geo";
    QString STLFileNamewExt;

    FILE* f1 = fopen(gmshGeoFile.toLatin1().data(), "wb");
    if(nullptr == f1)
    {
      QString ss = QObject::tr("Error creating Gmsh geo file '%1'").arg(gmshGeoFile);
      setErrorCondition(-1, ss);
    }

    for(size_t i = 1; i < numfeatures; i++)
    {

      STLFileNamewExt = m_GmshSTLFileName + QString("Feature_") + QString::number(i) + ".stl";
      fprintf(f1, "Merge \"%s\";\n", STLFileNamewExt.toLatin1().data());
      fprintf(f1, "Surface Loop(%zu) = {%zu};\n", i, i);
      fprintf(f1, "Volume(%zu) = {%zu};\n", i, i);
    }

    fprintf(f1, "Mesh 3;\n");
    fprintf(f1, "Coherence Mesh;\n");
    if(m_MeshFileFormat == 1)
    {
      fprintf(f1, "Save \"gmsh.inp\";\n");
    }
    else
    {
      fprintf(f1, "Save \"gmsh.msh\";\n");
    }

    fclose(f1);

    // running Gmsh
    runPackage(gmshGeoFile, gmshGeoFile);

    break;
  }
  }

  if(getCancel())
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Export3dSolidMesh::createTetgenInpFile(const QString& file, MeshIndexType numNodes, float* nodes, MeshIndexType numTri, MeshIndexType* triangles, size_t numfeatures, float* centroid)
{
  FILE* f1 = fopen(file.toLatin1().data(), "wb");
  if(nullptr == f1)
  {
    QString ss = QObject::tr("Error writing tetGen input file '%1'").arg(file);
    setErrorCondition(-1, ss);
  }

  fprintf(f1, "# Part 1 - node list\n");
  fprintf(f1, "%lld 3 0 0\n", static_cast<long long int>(numNodes));
  for(MeshIndexType k = 0; k < numNodes; k++)
  {
    fprintf(f1, "%lld %.3f %.3f %.3f\n", static_cast<long long int>(k + 1), nodes[k * 3], nodes[k * 3 + 1], nodes[k * 3 + 2]);
  }

  fprintf(f1, "# Part 2 - element list\n");
  fprintf(f1, "%lld 0\n", static_cast<long long int>(numTri));
  for(MeshIndexType k = 0; k < numTri; k++)
  {
    fprintf(f1, "3 %lld %lld %lld\n", static_cast<long long int>(triangles[k * 3] + 1), static_cast<long long int>(triangles[k * 3 + 1] + 1), static_cast<long long int>(triangles[k * 3 + 2] + 1));
  }

  fprintf(f1, "# Part 3 - hole list\n");
  if(m_IncludeHolesUsingPhaseID)
  {
    size_t numHoles = 0;
    size_t jj = 0;
    //      Int32ArrayType::Pointer m_HolesIDPtr = Int32ArrayType::CreateArray(numfeatures, "HOLESID_INTERNAL_USE_ONLY", true);
    // int32_t* m_HolesID = m_HolesIDPtr->getPointer(0);

    for(size_t ii = 1; ii < numfeatures; ii++)
    {
      if(m_FeaturePhases[ii] == m_PhaseID)
      {
        numHoles = numHoles + 1;
        //      m_HolesID[ii] = 1;
      }
    }
    fprintf(f1, "%zu\n", numHoles);
    for(size_t ii = 1; ii < numfeatures; ii++)
    {
      //	  if (m_HolesID[ii] == 1 )
      if(m_FeaturePhases[ii] == m_PhaseID)
      {
        fprintf(f1, "%zu %.3f %.3f %.3f\n", jj + 1, centroid[ii * 3], centroid[ii * 3 + 1], centroid[ii * 3 + 2]);
      }
    }
  }
  else
  {
    fprintf(f1, "0\n");
  }

  fprintf(f1, "# Part 4 - region list\n");
  fprintf(f1, "%zu 0\n", numfeatures - 1);
  for(size_t i = 1; i < numfeatures; i++)
  {
    fprintf(f1, "%zu %.3f %.3f %.3f %zu\n", i, centroid[i * 3], centroid[i * 3 + 1], centroid[i * 3 + 2], i);
  }

  fclose(f1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Export3dSolidMesh::runPackage(const QString& file, const QString& meshFile)
{

  QString program;
  QString switches;
  QStringList arguments;

  program = m_PackageLocation + QDir::separator();

  switch(m_MeshingPackage)
  {
  case 0: // TetGen
  {
    // cmd to run: "tetgen -pYAqOa file

    switches = "-pYAO" + QString::number(m_OptimizationLevel);

    if(m_RefineMesh)
    {
      QString tmp = "q" + QString::number(m_MaxRadiusEdgeRatio) + "/" + QString::number(m_MinDihedralAngle);
      switches += tmp;
    }

    if(m_LimitTetrahedraVolume)
    {
      QString tmp = "a" + QString::number(m_MaxTetrahedraVolume);
      switches += tmp;
    }

    program += "tetgen";

    arguments << switches << file;

    break;
  }
  case 1: {

    // cmd to run: "netgen file.stlb -batchmode -verycoarse/coarse/moderate/fine/veryfine -meshfile=output filename

    switches = "-";
    if(m_MeshSize == 0)
    {
      switches += "verycoarse";
    }
    else if(m_MeshSize == 1)
    {
      switches += "coarse";
    }
    else if(m_MeshSize == 2)
    {
      switches += "moderate";
    }
    else if(m_MeshSize == 3)
    {
      switches += "fine";
    }
    else if(m_MeshSize == 4)
    {
      switches += "veryfine";
    }

    program += "netgen";

    QString switchMeshFile;

    switchMeshFile = "-meshfile=";
    switchMeshFile += meshFile;

    arguments << file << "-batchmode" << switchMeshFile << "-V" << switches;

    break;
  }
  case 2: {
    // QString switch1;
    // QString switch2;

    // switch1 = "-format";

    // cmd to run: "gmsh file -
    //    if(m_MeshFileFormat == 1)
    //  {
    //  switch2 = "inp";
    // }
    // else
    // {
    //   switch2 = "auto";
    // }

    switches = "-";
    program += "gmsh";
    arguments << file << switches;

    break;
  }
  }

  m_ProcessPtr = QSharedPointer<QProcess>(new QProcess(nullptr));

  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

#if defined(Q_OS_MAC)
  if(m_MeshingPackage == 1)
  {
    QString env_PYTHONPATH = m_PackageLocation + QDir::separator() + QString("..") + QDir::separator() + QString("Resources") + QDir::separator() + QString("lib") + QDir::separator() +
                             QString("python3.7") + QDir::separator() + QString("site-packages");
    QString env_NETGENDIR = m_PackageLocation;
    QString env_DYLD_LIBRARYPATH = m_PackageLocation;

    env.insert("PYTHONPATH", env_PYTHONPATH);
    env.insert("NETGENDIR", env_NETGENDIR);
    env.insert("DYLD_LIBRARY_PATH", env_DYLD_LIBRARYPATH);

    //      env.insert("PYTHONPATH", "/Applications/Netgen.app/Contents/Resources/lib/python3.7/site-packages:.");
    //  env.insert("NETGENDIR", "/Applications/Netgen.app/Contents/MacOS");
    // env.insert("DYLD_LIBRARY_PATH", "/Applications/Netgen.app/Contents/MacOS");
    // env.insert("PATH", "$NETGENDIR:$PATH");
  }
#endif
  m_ProcessPtr->setProcessEnvironment(env);

  qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
  qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");
  connect(m_ProcessPtr.data(), SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processHasFinished(int, QProcess::ExitStatus)), Qt::QueuedConnection);
  connect(m_ProcessPtr.data(), SIGNAL(error(QProcess::ProcessError)), this, SLOT(processHasErroredOut(QProcess::ProcessError)), Qt::QueuedConnection);
  connect(m_ProcessPtr.data(), SIGNAL(readyReadStandardError()), this, SLOT(sendErrorOutput()), Qt::QueuedConnection);
  connect(m_ProcessPtr.data(), SIGNAL(readyReadStandardOutput()), this, SLOT(sendStandardOutput()), Qt::QueuedConnection);

  m_ProcessPtr->setWorkingDirectory(m_outputPath);
  m_ProcessPtr->start(program, arguments);
  m_ProcessPtr->waitForStarted(2000);
  m_ProcessPtr->waitForFinished(-1);

  notifyStatusMessage("Finished running Package");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Export3dSolidMesh::mergeMesh(const QString& mergeFile, const QString& meshFile)
{

  QString program;
  QStringList arguments;

  QString switch1;
  switch1 = "-inputmeshfile=";
  switch1 += mergeFile;

  QString switch2;
  switch2 = "-mergefile=";
  switch2 += meshFile;

  QString switch3;
  switch3 = "-meshfile=";
  switch3 += mergeFile;

  program = m_PackageLocation + QDir::separator() + "netgen";

  // cmd to run: netgen -inputmeshfile=m.vol -mergefile=c.vol -batchmode -meshfile=m.vol

  arguments << "-batchmode" << switch1 << switch2 << switch3 << "-V";

  m_ProcessPtr = QSharedPointer<QProcess>(new QProcess(nullptr));

  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

#if defined(Q_OS_MAC)
  QString env_PYTHONPATH = m_PackageLocation + QDir::separator() + QString("..") + QDir::separator() + QString("Resources") + QDir::separator() + QString("lib") + QDir::separator() +
                           QString("python3.7") + QDir::separator() + QString("site-packages");
  QString env_NETGENDIR = m_PackageLocation;
  QString env_DYLD_LIBRARYPATH = m_PackageLocation;

  env.insert("PYTHONPATH", env_PYTHONPATH);
  env.insert("NETGENDIR", env_NETGENDIR);
  env.insert("DYLD_LIBRARY_PATH", env_DYLD_LIBRARYPATH);
#endif

  // env.insert("PYTHONPATH", "/Applications/Netgen.app/Contents/Resources/lib/python3.7/site-packages:.");
  // env.insert("NETGENDIR", "/Applications/Netgen.app/Contents/MacOS");
  // env.insert("DYLD_LIBRARY_PATH", "/Applications/Netgen.app/Contents/MacOS");
  // env.insert("PATH", "$NETGENDIR:$PATH");

  m_ProcessPtr->setProcessEnvironment(env);

  qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
  qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");
  connect(m_ProcessPtr.data(), SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processHasFinished(int, QProcess::ExitStatus)), Qt::QueuedConnection);
  connect(m_ProcessPtr.data(), SIGNAL(error(QProcess::ProcessError)), this, SLOT(processHasErroredOut(QProcess::ProcessError)), Qt::QueuedConnection);
  connect(m_ProcessPtr.data(), SIGNAL(readyReadStandardError()), this, SLOT(sendErrorOutput()), Qt::QueuedConnection);
  connect(m_ProcessPtr.data(), SIGNAL(readyReadStandardOutput()), this, SLOT(sendStandardOutput()), Qt::QueuedConnection);

  m_ProcessPtr->setWorkingDirectory(m_outputPath);
  m_ProcessPtr->start(program, arguments);
  m_ProcessPtr->waitForStarted(2000);
  m_ProcessPtr->waitForFinished(-1);

  notifyStatusMessage("Merging individual volume meshes");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Export3dSolidMesh::processHasFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
  if(getCancel())
  {
  }
  else if(exitStatus == QProcess::CrashExit)
  {
    QString ss = QObject::tr("The process crashed during its exit.");
    setErrorCondition(-4003, ss);
  }
  else if(exitCode < 0)
  {
    QString ss = QObject::tr("The process finished with exit code %1.").arg(QString::number(exitCode));
    setErrorCondition(-4004, ss);
  }
  else if(getErrorCode() >= 0)
  {
  }

  m_Pause = false;
  m_WaitCondition.wakeAll();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Export3dSolidMesh::processHasErroredOut(QProcess::ProcessError error)
{
  if(getCancel())
  {
    QString ss = QObject::tr("The process was killed by the user.");
    setWarningCondition(-4004, ss);
  }
  else if(error == QProcess::FailedToStart)
  {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString pathEnv = env.value("PATH");

    QString ss = QObject::tr("The package failed to start. Either the package is missing, or you may have insufficient permissions \
or the path containing the executble is not in the system's environment path. PATH=%1.\n Try using the absolute path to the executable.")
                     .arg(pathEnv);
    setErrorCondition(-4005, ss);
  }
  else if(error == QProcess::Crashed)
  {
    QString ss = QObject::tr("The process crashed some time after starting successfully.");
    setErrorCondition(-4006, ss);
  }
  else if(error == QProcess::Timedout)
  {
    QString ss = QObject::tr("The process timed out.");
    setErrorCondition(-4007, ss);
  }
  else if(error == QProcess::WriteError)
  {
    QString ss = QObject::tr("An error occurred when attempting to write to the process.");
    setErrorCondition(-4008, ss);
  }
  else if(error == QProcess::ReadError)
  {
    QString ss = QObject::tr("An error occurred when attempting to read from the process.");
    setErrorCondition(-4009, ss);
  }
  else
  {
    QString ss = QObject::tr("An unknown error occurred.");
    setErrorCondition(-4010, ss);
  }

  m_Pause = false;
  m_WaitCondition.wakeAll();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Export3dSolidMesh::sendErrorOutput()
{
  if(m_ProcessPtr.data() != nullptr)
  {
    QString error = m_ProcessPtr->readAllStandardError();
    if(error[error.size() - 1] == '\n')
    {
      error.chop(1);
    }
    notifyStatusMessage(error);
    m_WaitCondition.wakeAll();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Export3dSolidMesh::sendStandardOutput()
{
  if(m_ProcessPtr.data() != nullptr)
  {
    notifyStatusMessage(m_ProcessPtr->readAllStandardOutput());
    m_WaitCondition.wakeAll();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Export3dSolidMesh::scanTetGenFile(const QString& fileEle, const QString& fileNode, DataContainer* dataContainer, AttributeMatrix* vertexAttrMat, AttributeMatrix* cellAttrMat)
{

  bool allocate = true;
  bool ok = false;
  QFile inStreamNode(fileNode);
  QFile inStreamEle(fileEle);

  if(!inStreamEle.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Input file could not be opened: %1").arg(fileEle);
    setErrorCondition(-100, ss);
    return;
  }

  if(!inStreamNode.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Input file could not be opened: %1").arg(fileNode);
    setErrorCondition(-100, ss);
    return;
  }

  QByteArray bufNode;
  QByteArray bufEle;
  QList<QByteArray> tokensNode;
  QList<QByteArray> tokensEle;

  bufEle = inStreamEle.readLine();
  bufEle = bufEle.trimmed();
  bufEle = bufEle.simplified();
  tokensEle = bufEle.split(' ');
  size_t numCells = tokensEle.at(0).toULongLong(&ok);
  std::vector<size_t> tDims(1, numCells);
  cellAttrMat->resizeAttributeArrays(tDims);

  bufNode = inStreamNode.readLine();
  bufNode = bufNode.trimmed();
  bufNode = bufNode.simplified();
  tokensNode = bufNode.split(' ');
  size_t numVerts = tokensNode.at(0).toULongLong(&ok);
  tDims[0] = numVerts;
  vertexAttrMat->resizeAttributeArrays(tDims);

  TetrahedralGeom::Pointer tetGeomPtr = dataContainer->getGeometryAs<TetrahedralGeom>();
  tetGeomPtr->resizeTetList(numCells);
  tetGeomPtr->resizeVertexList(numVerts);

  //  SharedVertexList::Pointer tetvertexPtr = TetrahedralGeom::CreateSharedVertexList(static_cast<int64_t>(numVerts), allocate);
  float* tetvertex = tetGeomPtr->getVertexPointer(0);
  for(size_t i = 0; i < numVerts; i++)
  {
    bufNode = inStreamNode.readLine();
    bufNode = bufNode.trimmed();
    bufNode = bufNode.simplified();
    tokensNode = bufNode.split(' ');
    tetvertex[3 * i] = tokensNode[1].toFloat(&ok);
    tetvertex[3 * i + 1] = tokensNode[2].toFloat(&ok);
    tetvertex[3 * i + 2] = tokensNode[3].toFloat(&ok);
  }

  //  SharedVertexList::Pointer tetvertexPtr = TetrahedralGeom::CreateSharedVertexList(static_cast<int64_t>(numVerts), allocate);
  // TetrahedralGeom::Pointer tetGeomPtr = TetrahedralGeom::CreateGeometry(static_cast<int64_t>(numCells), tetvertexPtr, SIMPL::Geometry::TetrahedralGeometry, allocate);
  //  tetGeomPtr->setSpatialDimensionality(3);

  MeshIndexType* tets = tetGeomPtr->getTetPointer(0);

  QString dataArrayName = "FeatureIDs";
  Int32ArrayType::Pointer featureIDsdata = Int32ArrayType::NullPointer();
  int32_t numComp = 1;
  std::vector<size_t> cDims(1, static_cast<size_t>(numComp));
  featureIDsdata = Int32ArrayType::CreateArray(numCells, cDims, dataArrayName, allocate);
  cellAttrMat->insertOrAssign(featureIDsdata);

  dataArrayName = "Euler Angles";
  FloatArrayType::Pointer eulerangles = FloatArrayType::NullPointer();
  numComp = 3;
  cDims[0] = static_cast<size_t>(numComp);
  eulerangles = FloatArrayType::CreateArray(numCells, cDims, dataArrayName, allocate);
  cellAttrMat->insertOrAssign(eulerangles);

  dataArrayName = "Phases";
  Int32ArrayType::Pointer phasesdata = Int32ArrayType::NullPointer();
  numComp = 1;
  cDims[0] = static_cast<size_t>(numComp);
  phasesdata = Int32ArrayType::CreateArray(numCells, cDims, dataArrayName, allocate);
  cellAttrMat->insertOrAssign(phasesdata);

  for(size_t i = 0; i < numCells; i++)
  {
    bufEle = inStreamEle.readLine();
    bufEle = bufEle.trimmed();
    bufEle = bufEle.simplified();
    tokensEle = bufEle.split(' ');
    tets[4 * i] = tokensEle[1].toInt(&ok) - 1;
    tets[4 * i + 1] = tokensEle[2].toInt(&ok) - 1;
    tets[4 * i + 2] = tokensEle[3].toInt(&ok) - 1;
    tets[4 * i + 3] = tokensEle[4].toInt(&ok) - 1;

    int32_t value = tokensEle[5].toInt(&ok);
    featureIDsdata->setComponent(i, 0, value);

    int32_t pvalue = m_FeaturePhases[value];
    phasesdata->setComponent(i, 0, pvalue);

    for(size_t j = 0; j < 3; j++)
    {
      float evalue = m_FeatureEulerAngles[3 * value + j];
      eulerangles->setComponent(i, j, evalue);
    }
  }
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
QString Export3dSolidMesh::getCompiledLibraryName() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Export3dSolidMesh::getBrandingString() const
{
  return "SimulationIO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Export3dSolidMesh::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SimulationIO::Version::Major() << "." << SimulationIO::Version::Minor() << "." << SimulationIO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Export3dSolidMesh::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Export3dSolidMesh::getSubGroupName() const
{
  return "SimulationIO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Export3dSolidMesh::getHumanLabel() const
{
  return "Export 3d Solid Mesh";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid Export3dSolidMesh::getUuid() const
{
  return QUuid("{fcff3b03-bff6-5511-bc65-5e558d12f0a6}");
}

// -----------------------------------------------------------------------------
Export3dSolidMesh::Pointer Export3dSolidMesh::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<Export3dSolidMesh> Export3dSolidMesh::New()
{
  struct make_shared_enabler : public Export3dSolidMesh
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString Export3dSolidMesh::getNameOfClass() const
{
  return QString("Export3dSolidMesh");
}

// -----------------------------------------------------------------------------
QString Export3dSolidMesh::ClassName()
{
  return QString("Export3dSolidMesh");
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setMeshingPackage(int value)
{
  m_MeshingPackage = value;
}

// -----------------------------------------------------------------------------
int Export3dSolidMesh::getMeshingPackage() const
{
  return m_MeshingPackage;
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setoutputPath(const QString& value)
{
  m_outputPath = value;
}

// -----------------------------------------------------------------------------
QString Export3dSolidMesh::getoutputPath() const
{
  return m_outputPath;
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setPackageLocation(const QString& value)
{
  m_PackageLocation = value;
}

// -----------------------------------------------------------------------------
QString Export3dSolidMesh::getPackageLocation() const
{
  return m_PackageLocation;
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setSurfaceMeshFaceLabelsArrayPath(const DataArrayPath& value)
{
  m_SurfaceMeshFaceLabelsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath Export3dSolidMesh::getSurfaceMeshFaceLabelsArrayPath() const
{
  return m_SurfaceMeshFaceLabelsArrayPath;
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setFeaturePhasesArrayPath(const DataArrayPath& value)
{
  m_FeaturePhasesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath Export3dSolidMesh::getFeaturePhasesArrayPath() const
{
  return m_FeaturePhasesArrayPath;
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setFeatureEulerAnglesArrayPath(const DataArrayPath& value)
{
  m_FeatureEulerAnglesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath Export3dSolidMesh::getFeatureEulerAnglesArrayPath() const
{
  return m_FeatureEulerAnglesArrayPath;
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setFeatureCentroidArrayPath(const DataArrayPath& value)
{
  m_FeatureCentroidArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath Export3dSolidMesh::getFeatureCentroidArrayPath() const
{
  return m_FeatureCentroidArrayPath;
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setRefineMesh(bool value)
{
  m_RefineMesh = value;
}

// -----------------------------------------------------------------------------
bool Export3dSolidMesh::getRefineMesh() const
{
  return m_RefineMesh;
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setMaxRadiusEdgeRatio(float value)
{
  m_MaxRadiusEdgeRatio = value;
}

// -----------------------------------------------------------------------------
float Export3dSolidMesh::getMaxRadiusEdgeRatio() const
{
  return m_MaxRadiusEdgeRatio;
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setMinDihedralAngle(float value)
{
  m_MinDihedralAngle = value;
}

// -----------------------------------------------------------------------------
float Export3dSolidMesh::getMinDihedralAngle() const
{
  return m_MinDihedralAngle;
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setLimitTetrahedraVolume(bool value)
{
  m_LimitTetrahedraVolume = value;
}

// -----------------------------------------------------------------------------
bool Export3dSolidMesh::getLimitTetrahedraVolume() const
{
  return m_LimitTetrahedraVolume;
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setMaxTetrahedraVolume(float value)
{
  m_MaxTetrahedraVolume = value;
}

// -----------------------------------------------------------------------------
float Export3dSolidMesh::getMaxTetrahedraVolume() const
{
  return m_MaxTetrahedraVolume;
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setOptimizationLevel(int value)
{
  m_OptimizationLevel = value;
}

// -----------------------------------------------------------------------------
int Export3dSolidMesh::getOptimizationLevel() const
{
  return m_OptimizationLevel;
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setTetDataContainerName(const QString& value)
{
  m_TetDataContainerName = value;
}

// -----------------------------------------------------------------------------
QString Export3dSolidMesh::getTetDataContainerName() const
{
  return m_TetDataContainerName;
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setVertexAttributeMatrixName(const QString& value)
{
  m_VertexAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString Export3dSolidMesh::getVertexAttributeMatrixName() const
{
  return m_VertexAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setCellAttributeMatrixName(const QString& value)
{
  m_CellAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString Export3dSolidMesh::getCellAttributeMatrixName() const
{
  return m_CellAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setGmshSTLFileName(const QString& value)
{
  m_GmshSTLFileName = value;
}

// -----------------------------------------------------------------------------
QString Export3dSolidMesh::getGmshSTLFileName() const
{
  return m_GmshSTLFileName;
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setMeshFileFormat(int value)
{
  m_MeshFileFormat = value;
}

// -----------------------------------------------------------------------------
int Export3dSolidMesh::getMeshFileFormat() const
{
  return m_MeshFileFormat;
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setNetgenSTLFileName(const QString& value)
{
  m_NetgenSTLFileName = value;
}

// -----------------------------------------------------------------------------
QString Export3dSolidMesh::getNetgenSTLFileName() const
{
  return m_NetgenSTLFileName;
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setMeshSize(int value)
{
  m_MeshSize = value;
}

// -----------------------------------------------------------------------------
int Export3dSolidMesh::getMeshSize() const
{
  return m_MeshSize;
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setIncludeHolesUsingPhaseID(bool value)
{
  m_IncludeHolesUsingPhaseID = value;
}

// -----------------------------------------------------------------------------
bool Export3dSolidMesh::getIncludeHolesUsingPhaseID() const
{
  return m_IncludeHolesUsingPhaseID;
}

// -----------------------------------------------------------------------------
void Export3dSolidMesh::setPhaseID(int value)
{
  m_PhaseID = value;
}

// -----------------------------------------------------------------------------
int Export3dSolidMesh::getPhaseID() const
{
  return m_PhaseID;
}
