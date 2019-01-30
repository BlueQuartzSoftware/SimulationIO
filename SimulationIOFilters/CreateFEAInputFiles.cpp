/*
 * Your License or Copyright can go here
 */

#include "CreateFEAInputFiles.h"

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
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Utilities/FileSystemPathHelper.h"

#include "SimulationIO/SimulationIOConstants.h"
#include "SimulationIO/SimulationIOVersion.h"

#define LLU_CAST(arg) static_cast<unsigned long long int>(arg)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateFEAInputFiles::CreateFEAInputFiles()
  : m_FEAPackage(0)
  , m_JobName("")
  , m_OutputPath("")
  , m_OutputFilePrefix("")
  , m_NumDepvar(1)
  , m_NumMatConst(6)
  , m_NumUserOutVar(1)
  , m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
  , m_CellPhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Phases)
  , m_CellEulerAnglesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::EulerAngles)
  , m_DelamMat("")
  , m_NumClusters(1)
  , m_UseMeshCreatedByDREAM3D(true)
{
  initialize();

  m_NumElem.x = 20;
  m_NumElem.y = 20;
  m_NumElem.z = 20;

  m_NumKeypoints.x = 2;
  m_NumKeypoints.y = 2;
  m_NumKeypoints.z = 2;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateFEAInputFiles::~CreateFEAInputFiles() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFEAInputFiles::initialize()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFEAInputFiles::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("FEA package");
    parameter->setPropertyName("FEAPackage");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(CreateFEAInputFiles, this, FEAPackage));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(CreateFEAInputFiles, this, FEAPackage));
    QVector<QString> choices;
    choices.push_back("ABAQUS");
    choices.push_back("PZFLEX");
    choices.push_back("BSAM");
    parameter->setChoices(choices);
    QStringList linkedProps = {"JobName", "NumElem", "NumDepvar", "NumMatConst", "NumUserOutVar", "MatConst", "CellEulerAnglesArrayPath", "CellPhasesArrayPath", "DelamMat", "NumKeypoints", "NumClusters", "ClusterData", "UseMeshCreatedByDREAM3D"};
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  parameters.push_back(SIMPL_NEW_OUTPUT_PATH_FP("Output Path ", OutputPath, FilterParameter::Parameter, CreateFEAInputFiles,"*" ,"*" ));
  parameters.push_back(SIMPL_NEW_STRING_FP("Output File Prefix", OutputFilePrefix, FilterParameter::Parameter, CreateFEAInputFiles));

  {
    QStringList linkedProps = {"NumElem"};
    parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Use Mesh Created By DREAM.3D", UseMeshCreatedByDREAM3D, FilterParameter::Parameter, CreateFEAInputFiles, linkedProps, 0));
    linkedProps.clear();

    parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Number of Elements", NumElem, FilterParameter::Parameter, CreateFEAInputFiles, 0));
    parameters.push_back(SIMPL_NEW_STRING_FP("Job Name", JobName, FilterParameter::Parameter, CreateFEAInputFiles,0));
    parameters.push_back(SIMPL_NEW_INTEGER_FP("Number of Solution Dependent State Variables", NumDepvar, FilterParameter::Parameter, CreateFEAInputFiles, 0));
    parameters.push_back(SIMPL_NEW_INTEGER_FP("Number of Material Constants", NumMatConst, FilterParameter::Parameter, CreateFEAInputFiles, 0));
    parameters.push_back(SIMPL_NEW_INTEGER_FP("Number of User Output Variables", NumUserOutVar, FilterParameter::Parameter, CreateFEAInputFiles, 0));
    // Table - Dynamic rows and fixed columns
    {
      QStringList cHeaders;
      cHeaders << "Values";
      std::vector<std::vector<double>> defaultTable(1, std::vector<double>(1, 0.0));
      m_MatConst.setColHeaders(cHeaders);
      m_MatConst.setTableData(defaultTable);
      m_MatConst.setDynamicRows(true);
      parameters.push_back(SIMPL_NEW_DYN_TABLE_FP("Material Constants", MatConst, FilterParameter::Parameter, CreateFEAInputFiles, 0));
    }
  }

  {
    parameters.push_back(SIMPL_NEW_STRING_FP("Delamination material", DelamMat, FilterParameter::Parameter, CreateFEAInputFiles, 1));
    parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Number of Keypoints", NumKeypoints, FilterParameter::Parameter, CreateFEAInputFiles, 1));
  }

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
      DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, CreateFEAInputFiles, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
      DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Euler Angles", CellEulerAnglesArrayPath, FilterParameter::RequiredArray, CreateFEAInputFiles, req, 0));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
      DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", CellPhasesArrayPath, FilterParameter::RequiredArray, CreateFEAInputFiles, req, 0));
  }

  {
    parameters.push_back(SIMPL_NEW_INTEGER_FP("Number of Clusters", NumClusters, FilterParameter::Parameter, CreateFEAInputFiles, 2));

    // Table - Dynamic rows and fixed columns
    {
      QStringList cHeaders;
      cHeaders << "Elements"
	       << "Nodes"
	       << "Sets";
      std::vector<std::vector<double>> defaultTable(1, std::vector<double>(3, 0.0));
      m_ClusterData.setColHeaders(cHeaders);
      m_ClusterData.setTableData(defaultTable);
      m_ClusterData.setDynamicRows(true);
      parameters.push_back(SIMPL_NEW_DYN_TABLE_FP("Cluster Data", ClusterData, FilterParameter::Parameter, CreateFEAInputFiles, 2));
    }
    
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFEAInputFiles::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOutputPath(reader->readString("OutputPath", getOutputPath()));
  setOutputFilePrefix(reader->readString("OutputFilePrefix", getOutputFilePrefix()));
  setNumElem(reader->readIntVec3("Number of Elements", getNumElem()));
  setCellEulerAnglesArrayPath(reader->readDataArrayPath("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath()));
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setJobName(reader->readString("JobName", getJobName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFEAInputFiles::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  //  FileSystemPathHelper::CheckOutputFile(this, "Output File Path", getOutputPath(), true);

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  QVector<DataArrayPath> dataArrayPaths;

  QVector<size_t> cDims(1, 1);

  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
    {
      dataArrayPaths.push_back(getFeatureIdsArrayPath());
    }

  switch(m_FEAPackage)
    {
    case 0: // ABAQUS
      {	

	m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(),
													      cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
	if(nullptr != m_CellPhasesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
	  {
	    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
	  } /* Now assign the raw pointer to data from the DataArray<T> object */
	if(getErrorCondition() >= 0)
	  {
	    dataArrayPaths.push_back(getCellPhasesArrayPath());
	  }
	
	cDims[0] = 3;
	m_CellEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellEulerAnglesArrayPath(),
														 cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
	if(nullptr != m_CellEulerAnglesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
	  {
	    m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
	  } /* Now assign the raw pointer to data from the DataArray<T> object */
	if(getErrorCondition() >= 0)
	  {
	    dataArrayPaths.push_back(getCellEulerAnglesArrayPath());
	  }

	getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);

	break;
      }
    }
  //
  //
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFEAInputFiles::preflight()
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
void CreateFEAInputFiles::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0) { return; }
  //
  //
  // Check Output Path
  QDir dir;
  if(!dir.mkpath(m_OutputPath))
    {
      QString ss = QObject::tr("Error creating parent path '%1'").arg(m_OutputPath);
      setErrorCondition(-1);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  //
  //
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName());
  
  size_t dims[3] = {0, 0, 0};
  std::tie(dims[0], dims[1], dims[2]) = m->getGeometryAs<ImageGeom>()->getDimensions();
  float res[3] = {0.0f, 0.0f, 0.0f};
  m->getGeometryAs<ImageGeom>()->getResolution(res);
  float origin[3] = {0.0f, 0.0f, 0.0f};
  m->getGeometryAs<ImageGeom>()->getOrigin(origin);
  //
  // find total number of Grain Ids
  int32_t maxGrainId = 0;
  int32_t totalPoints = m->getGeometryAs<ImageGeom>()->getNumberOfElements();
  for(int32_t i = 0; i < totalPoints; i++) // find number of grainIds
    {
      if(m_FeatureIds[i] > maxGrainId)
	{
	  maxGrainId = m_FeatureIds[i];
	}
    }
  //
  //
  switch(m_FEAPackage)
    {
    case 0: // ABAQUS
      {	
	//
	// Create file names
	QString nodesFile = m_OutputPath + QDir::separator() + m_OutputFilePrefix + "_nodes.inp";
	QString elemsFile = m_OutputPath + QDir::separator() + m_OutputFilePrefix + "_elems.inp";
	QString sectsFile = m_OutputPath + QDir::separator() + m_OutputFilePrefix + "_sects.inp";
	QString elsetFile = m_OutputPath + QDir::separator() + m_OutputFilePrefix + "_elset.inp";
	QString masterFile = m_OutputPath + QDir::separator() + m_OutputFilePrefix + ".inp";
	QList<QString> fileNames;
	fileNames << nodesFile << elemsFile << sectsFile << elsetFile << masterFile;
	//
	//
	FILE* f1 = fopen(fileNames.at(0).toLatin1().data(), "wb");
	if(nullptr == f1)
	  {
	    QString ss = QObject::tr("Error writing nodes file '%1'").arg(nodesFile);
	    setErrorCondition(-1);
	    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
	  }
	
	FILE* f2 = fopen(fileNames.at(1).toLatin1().data(), "wb");
	if(nullptr == f2)
	  {
	    QString ss = QObject::tr("Error writing connectivity file '%1'").arg(elemsFile);
	    setErrorCondition(-1);
	    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
	  }
	
	FILE* f3 = fopen(fileNames.at(2).toLatin1().data(), "wb");
	if(nullptr == f3)
	  {
	    QString ss = QObject::tr("Error writing ABAQUS input file '%1'").arg(sectsFile);
	    setErrorCondition(-1);
	    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
	  }

	FILE* f4 = fopen(fileNames.at(3).toLatin1().data(), "wb");
	if(nullptr == f4)
	  {
	    QString ss = QObject::tr("Error writing ABAQUS input file '%1'").arg(elsetFile);
	    setErrorCondition(-1);
	    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
	  }

	FILE* f5 = fopen(fileNames.at(4).toLatin1().data(), "wb");
	if(nullptr == f5)
	  {
	    QString ss = QObject::tr("Error writing ABAQUS input file '%1'").arg(masterFile);
	    setErrorCondition(-1);
	    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
	  }
	
	//
	fprintf(f5, "*Heading\n");
	fprintf(f5, "%s\n", m_JobName.toLatin1().data());
	fprintf(f5, "** Job name : %s\n", m_JobName.toLatin1().data());
	fprintf(f5, "*Preprint, echo = NO, model = NO, history = NO, contact = NO\n");
	//
	int32_t ne_x,ne_y,ne_z;
	int32_t nnode_x,nnode_y,nnode_z;
	int32_t index;

	ne_x = m_NumElem.x;
	ne_y = m_NumElem.y;
	ne_z = m_NumElem.z;
	
	nnode_x = ne_x + 1;
	nnode_y = ne_y + 1;
	nnode_z = ne_z + 1;	
	//

	FloatArrayType::Pointer m_coordLengthPtr = FloatArrayType::CreateArray(3*nnode_x*nnode_y*nnode_z , "NODAL_COORDINATES_INTERNAL_USE_ONLY");
	float* m_coord = m_coordLengthPtr->getPointer(0);
	
	fprintf(f1,"*NODE, NSET=ALLNODES\n");  
	fprintf(f5,"*NODE, NSET=ALLNODES\n");  
	
	for(int32_t k = 0; k < nnode_z; k++)
	  {
	    for(int32_t j = 0; j < nnode_y; j++)
	      {
		for(int32_t i = 0; i < nnode_x; i++)
		  {
		    index = k*nnode_x*nnode_y+j*nnode_x+i;
		    m_coord[index*3] = origin[0] + (i * res[0]);
		    m_coord[index*3+1] = origin[1] + (j * res[1]);
		    m_coord[index*3+2] = origin[2] + (k * res[2]);
		  }
	      }
	  }
	//	
	//
	for(int32_t k = 0; k < nnode_z; k++)
	  {
	    for(int32_t j = 0; j < nnode_y; j++)
	      {
		for(int32_t i = 0; i < nnode_x; i++)
		  {
		    index = k*nnode_x*nnode_y+j*nnode_x+i;
		    fprintf(f1,"%d, %.3f, %.3f, %.3f\n", index+1, m_coord[index*3],m_coord[index*3+1],m_coord[index*3+2]);  
		    fprintf(f5,"%d, %.3f, %.3f, %.3f\n", index+1, m_coord[index*3],m_coord[index*3+1],m_coord[index*3+2]);  
		  }
	      }
	  }
	//	
	Int32ArrayType::Pointer m_connLengthPtr = Int32ArrayType::CreateArray(8*ne_x*ne_y*ne_z , "CONNECTIVITY_INTERNAL_USE_ONLY");
	int32_t* m_conn = m_connLengthPtr->getPointer(0);
	
	fprintf(f2,"*ELEMENTS, TYPE=C3D8R, ELSET=ALLELEMENTS\n");  
	fprintf(f5,"*ELEMENTS, TYPE=C3D8R, ELSET=ALLELEMENTS\n");  
	
	for(int32_t k = 0; k < ne_z; k++)
	  {
	    for(int32_t j = 0; j < ne_y; j++)
	      {
		for(int32_t i = 0; i < ne_x; i++)
		  {
		    index = k*nnode_x*nnode_y + j*nnode_x + i + 1;
		    int32_t eindex =  k*ne_x*ne_y + j*ne_x + i;    
		    m_conn[eindex*8] = index;
		    m_conn[eindex*8+1] = index + 1;
		    m_conn[eindex*8+2] = index + nnode_x+1;
		    m_conn[eindex*8+3] = index + nnode_x ;
		    m_conn[eindex*8+4] = index + nnode_x*nnode_y;
		    m_conn[eindex*8+5] = index + 1 + nnode_x*nnode_y;
		    m_conn[eindex*8+6] = index + nnode_x + nnode_x*nnode_y + 1;
		    m_conn[eindex*8+7] = index + nnode_x + nnode_x*nnode_y;
		  }
	      }
	  }
  
	for(int32_t k = 0; k < ne_z; k++)
	  {
	    for(int32_t j = 0; j < ne_y; j++)
	      {
		for(int32_t i = 0; i < ne_x; i++)
		  {
		    index =  k*ne_x*ne_y + j*ne_x + i;
		    fprintf(f2,"%d, %d, %d, %d, %d, %d, %d, %d, %d\n", index+1, m_conn[index*8],m_conn[index*8+1],m_conn[index*8+2],m_conn[index*8+3],m_conn[index*8+4],m_conn[index*3+5],m_conn[index*8+6],m_conn[index*8+7]);  
		    fprintf(f5,"%d, %d, %d, %d, %d, %d, %d, %d, %d\n", index+1, m_conn[index*8],m_conn[index*8+1],m_conn[index*8+2],m_conn[index*8+3],m_conn[index*8+4],m_conn[index*3+5],m_conn[index*8+6],m_conn[index*8+7]);  
		  }
	      }
	  }
	//  
	Int32ArrayType::Pointer m_phaseIdLengthPtr = Int32ArrayType::CreateArray(maxGrainId , "PHASEID_INTERNAL_USE_ONLY");
	int32_t* m_phaseId = m_phaseIdLengthPtr->getPointer(0);
	
	FloatArrayType::Pointer m_orientLengthPtr = FloatArrayType::CreateArray(maxGrainId*3, "ORIENTATION_INTERNAL_USE_ONLY");
	float* m_orient = m_orientLengthPtr->getPointer(0);

	int32_t grainId = 1;
	while(grainId <= maxGrainId)
	  {
	    for(int32_t i = 0; i < totalPoints; i++)
	      {
		if(m_FeatureIds[i] == grainId)
		  {
		    m_phaseId[grainId-1] = m_CellPhases[i];
		    m_orient[(grainId - 1)*3] = m_CellEulerAngles[i * 3] * 180.0 * SIMPLib::Constants::k_1OverPi;
		    m_orient[(grainId - 1)*3 + 1] = m_CellEulerAngles[i * 3 + 1] * 180.0 * SIMPLib::Constants::k_1OverPi;
		    m_orient[(grainId - 1)*3 + 2] = m_CellEulerAngles[i * 3 + 2] * 180.0 * SIMPLib::Constants::k_1OverPi;
		  }
	      }
	    grainId++;
	  }
	
	//

	int32_t voxelId = 1;
	while(voxelId <= maxGrainId)
	  {
	    size_t elementPerLine = 0;
	    fprintf(f4, "*Elset, elset=Grain%d_Phase%d_set\n", voxelId, m_phaseId[voxelId-1] );
	    fprintf(f5, "*Elset, elset=Grain%d_Phase%d_set\n", voxelId, m_phaseId[voxelId-1] );

	    for(int32_t i = 0; i < totalPoints; i++)
	      {
		if(m_FeatureIds[i] == voxelId)
		  {
		    if(elementPerLine != 0) // no comma at start
		      {
			if((elementPerLine % 16) != 0u) // 16 per line
			  {
			    fprintf(f4, ", ");
			    fprintf(f5, ", ");
			  }
			else
			  {
			    fprintf(f4, ",\n");
			    fprintf(f5, ",\n");
			  }
		      }
		    fprintf(f4, "%llu", static_cast<unsigned long long int>(i + 1));
		    fprintf(f5, "%llu", static_cast<unsigned long long int>(i + 1));
		    elementPerLine++;
		  }
	      }
	    fprintf(f4, "\n");
	    fprintf(f5, "\n");
	    voxelId++;
	  }
	//


	std::vector<std::vector<double>> MatConst = m_MatConst.getTableData();

	for(int32_t i = 1; i <= maxGrainId; i++)
	  {
	    fprintf(f5, "*Material, name = Grain%d_Phase%d_set\n", i, m_phaseId[i-1] );
	    fprintf(f5, "*Depvar\n");
	    fprintf(f5, "%d\n", m_NumDepvar);
	    fprintf(f5, "*User Material, constants = %d\n", m_NumMatConst);
	    fprintf(f5, "%d, %d, %.3f, %.3f, %.3f", i, m_phaseId[i - 1], m_orient[(i - 1) * 3], m_orient[(i - 1) * 3 + 1], m_orient[(i - 1) * 3 + 2]);
	    size_t entriesPerLine = 5;
	    for(int32_t j = 0; j < m_NumMatConst - 5; j++)
	      {
		if(entriesPerLine != 0) // no comma at start
		  {
		    if((entriesPerLine % 8) != 0u) // 8 per line
		      {
			fprintf(f5, ",  ");
		      }
		    else
		      {
			fprintf(f5, "\n");
			entriesPerLine = 0;
		      }
		  }
		fprintf(f5,"%.3f",MatConst[j][0]);
		entriesPerLine++;
	      }
	    fprintf(f5, "\n");
	    fprintf(f5, "*User Output Variables\n");
	    fprintf(f5, "%d\n", m_NumUserOutVar);
	  }
	//
	// We are now defining the sections, which is for each grain
	int32_t grain = 1;
	while(grain <= maxGrainId)
	  {
	    fprintf(f3, "*Solid Section, elset=Grain%d_Phase%d_set, material=Grain%d_Phase%d_mat\n", grain, m_phaseId[grain-1], grain, m_phaseId[grain-1]);
	    fprintf(f5, "*Solid Section, elset=Grain%d_Phase%d_set, material=Grain%d_Phase%d_mat\n", grain, m_phaseId[grain-1], grain, m_phaseId[grain-1]);
	    grain++;
	  }
	
	fclose(f1);
	fclose(f2);
	fclose(f3);
	fclose(f4);
	fclose(f5);
	break;
      }
    case 1: // PZFLEX
      {
	//
	QString masterFile = m_OutputPath + QDir::separator() + m_OutputFilePrefix + ".flxtbl";
	//
	FILE* f = fopen(masterFile.toLatin1().data(), "wb");
	if(nullptr == f)
	  {
	    QString ss = QObject::tr("Error writing PZFLEX input file '%1'").arg(masterFile);
	    setErrorCondition(-1);
	    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
	  }	
	//
	fprintf(f, "hedr 0\n");
	fprintf(f, "info 1\n");
	//
	float tempCoord = 0.0f;
	int32_t ne_x,ne_y,ne_z;
	int32_t nnode_x,nnode_y,nnode_z;
	
	ne_x = dims[0];
	ne_y = dims[1];
	ne_z = dims[2];
	
	nnode_x = ne_x + 1;
	nnode_y = ne_y + 1;
	nnode_z = ne_z + 1;

	//
	fprintf(f, "xcrd %d\n",nnode_x);
	size_t entriesPerLine = 0;
	for(int32_t i = 0; i < nnode_x; i++)
	  {
	    if(entriesPerLine != 0) // no space at start
	      {
		if((entriesPerLine % 6) != 0u) // 6 per line
		  {
		    fprintf(f, " ");
		  }
		else
		  {
		    fprintf(f, "\n");
		    entriesPerLine = 0;
		  }
	      }
	    tempCoord = origin[0] + (i * res[0]);
	    fprintf(f,"%.3f",tempCoord);
	    entriesPerLine++;
	  }
	fprintf(f, "\n");
	//
	fprintf(f, "ycrd %d\n",nnode_y);
	entriesPerLine = 0;
	for(int32_t i = 0; i < nnode_y; i++)
	  {
	    if(entriesPerLine != 0) // no space at start
	      {
		if((entriesPerLine % 6) != 0u) // 6 per line
		  {
		    fprintf(f, " ");
		  }
		else
		  {
		    fprintf(f, "\n");
		    entriesPerLine = 0;
		  }
	      }
	    tempCoord = origin[1] + (i * res[1]);
	    fprintf(f,"%.3f",tempCoord);
	    entriesPerLine++;
	  }
	fprintf(f, "\n");
	//
	fprintf(f, "zcrd %d\n",nnode_z);
	entriesPerLine = 0;
	for(int32_t i = 0; i < nnode_z; i++)
	  {
	    if(entriesPerLine != 0) // no space at start
	      {
		if((entriesPerLine % 6) != 0u) // 6 per line
		  {
		    fprintf(f, " ");
		  }
		else
		  {
		    fprintf(f, "\n");
		    entriesPerLine = 0;
		  }
	      }
	    tempCoord = origin[2] + (i * res[2]);
	    fprintf(f,"%.3f",tempCoord);
	    entriesPerLine++;
	  }
	fprintf(f, "\n");
	//
	fprintf(f, "keypoints\n");
	fprintf(f, "%d %d %d\n", m_NumKeypoints.x, m_NumKeypoints.y, m_NumKeypoints.z);
	//
	fprintf(f, "divisions\n");
	fprintf(f, "%d %d %d\n",ne_x,ne_y,ne_z);
	//
	fprintf(f, "name %d\n", maxGrainId);
	//
	fprintf(f, "void %s\n", m_DelamMat.toLatin1().data());
	//
	fprintf(f, "matr %d\n", ne_x * ne_y * ne_z);
	//
	entriesPerLine = 0;
	for(int32_t i = 0; i < totalPoints; i++)
	  {
	    if(entriesPerLine != 0) // no space at start
	      {
		if((entriesPerLine % 40) != 0u) // 6 per line
		  {
		    fprintf(f, " ");
		  }
		else
		  {
		    fprintf(f, "\n");
		    entriesPerLine = 0;
		  }
	      }
	    fprintf(f,"%d",m_FeatureIds[i]);
	    entriesPerLine++;
	  }
	//
	//
	fclose(f);
	//
	break;
      }
    case 2: // BSAM
      {
	//
	QString masterFile = m_OutputPath + QDir::separator() + m_OutputFilePrefix + ".flxtbl";
	//
	FILE* f = fopen(masterFile.toLatin1().data(), "wb");
	if(nullptr == f)
	  {
	    QString ss = QObject::tr("Error writing PZFLEX input file '%1'").arg(masterFile);
	    setErrorCondition(-1);
	    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
	  }	
	//
	fprintf(f, "hedr 0\n");
	fprintf(f, "info 1\n");
	//
	fclose(f);
	break;
      }
    }
 
  //
  //
  if(getCancel())
    {
      return;
    }
  //
  //

}
//
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateFEAInputFiles::newFilterInstance(bool copyFilterParameters) const
{
  CreateFEAInputFiles::Pointer filter = CreateFEAInputFiles::New();
  if(copyFilterParameters)
    {
      copyFilterParameterInstanceVariables(filter.get());
    }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateFEAInputFiles::getCompiledLibraryName() const
{ 
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateFEAInputFiles::getBrandingString() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateFEAInputFiles::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SimulationIO::Version::Major() << "." << SimulationIO::Version::Minor() << "." << SimulationIO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateFEAInputFiles::getGroupName() const
{ 
  return SIMPL::FilterGroups::Unsupported; 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateFEAInputFiles::getSubGroupName() const
{ 
  return "SimulationIO"; 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateFEAInputFiles::getHumanLabel() const
{ 
  return "Create FEA Input Files"; 
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid CreateFEAInputFiles::getUuid()
{
  return QUuid("{e7f02408-6c01-5b56-b970-7813e64c12e2}");
}

