/*
 * Your License or Copyright can go here
 */

#include "ImportFEAData.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Utilities/TimeUtilities.h"
#include "SIMPLib/Geometry/QuadGeom.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"

#include "SimulationIO/SimulationIOConstants.h"
#include "SimulationIO/SimulationIOVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportFEAData::ImportFEAData()  
: m_FEAPackage(0)
, m_odbName("")
, m_odbFilePath("")
, m_InstanceName("Part-1-1")
, m_Step("Step-1")
, m_FrameNumber(1)
, m_OutputVariable("S")
, m_ElementSet("NALL")
, m_DataContainerName(SIMPL::Defaults::DataContainerName)
, m_VertexAttributeMatrixName(SIMPL::Defaults::VertexAttributeMatrixName)
, m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportFEAData::~ImportFEAData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportFEAData::initialize()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportFEAData::setupFilterParameters()
{
    FilterParameterVector parameters;
 {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("FEA package");
    parameter->setPropertyName("FEAPackage");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(ImportFEAData, this, FEAPackage));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(ImportFEAData, this, FEAPackage));
    QVector<QString> choices;
    choices.push_back("ABAQUS");
    choices.push_back("BSAM");
    choices.push_back("PZFLEX");
    choices.push_back("DEFORM");
    parameter->setChoices(choices);
    QStringList linkedProps = {"odbName",
                               "odbFilePath",
                               "InstanceName",
			       "Step", 
			       "FrameNumber",    
			       "OutputVariable",    
                               "ElementSet",
                               "DataContainerName",
                               "VertexAttributeMatrixName",
                               "CellAttributeMatrixName"}; 
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  {
    parameters.push_back(SIMPL_NEW_STRING_FP("odb Name", odbName, FilterParameter::Parameter, ImportFEAData, 0));
    parameters.push_back(SIMPL_NEW_OUTPUT_PATH_FP("odb File Path", odbFilePath, FilterParameter::Parameter, ImportFEAData,"*" ,"*" ,0));
    parameters.push_back(SIMPL_NEW_STRING_FP("Instance Name", InstanceName, FilterParameter::Parameter, ImportFEAData, 0));
    parameters.push_back(SIMPL_NEW_STRING_FP("Step", Step, FilterParameter::Parameter, ImportFEAData, 0));
    parameters.push_back(SIMPL_NEW_INTEGER_FP("Frame Number", FrameNumber, FilterParameter::Parameter, ImportFEAData, 0));
    parameters.push_back(SIMPL_NEW_STRING_FP("Output Variable", OutputVariable, FilterParameter::Parameter, ImportFEAData, 0));
    parameters.push_back(SIMPL_NEW_STRING_FP("Element Set", ElementSet, FilterParameter::Parameter, ImportFEAData, 0));
    parameters.push_back(SIMPL_NEW_STRING_FP("Data Container Name", DataContainerName, FilterParameter::CreatedArray, ImportFEAData, 0));
    parameters.push_back(SIMPL_NEW_STRING_FP("Vertex Attribute Matrix Name", VertexAttributeMatrixName, FilterParameter::CreatedArray, ImportFEAData, 0));    
    parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix Name", CellAttributeMatrixName, FilterParameter::CreatedArray, ImportFEAData, 0));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportFEAData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setodbName(reader->readString("odbName", getodbName()));
  setodbFilePath(reader->readString("odbFilePath", getodbFilePath()));
  setInstanceName(reader->readString("InstanceName", getInstanceName()));
  setStep(reader->readString("Step", getStep()));
  setFrameNumber(reader->readValue("FrameNumber", getFrameNumber()));
  setOutputVariable(reader->readString("OutputVariable", getOutputVariable()));
  setElementSet(reader->readString("ElementSet", getElementSet()));
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName()));
  setVertexAttributeMatrixName(reader->readString("VertexAttributeMatrixName", getVertexAttributeMatrixName()));
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void ImportFEAData::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportFEAData::preflight()
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
void ImportFEAData::execute()
{
  initialize();
  int32_t err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  switch(m_FEAPackage)
    {
    case 0: // ABAQUS
      {
	// Check Output Path
	QDir dir;
	if(!dir.mkpath(m_odbFilePath))
	  {
	    QString ss = QObject::tr("Error creating parent path '%1'").arg(m_odbFilePath);
	    setErrorCondition(-1);
	    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
	    return;
	  }

	// Create ABAQUS python script
	QString abqpyscr = m_odbFilePath + QDir::separator() + m_odbName + ".py";

	err = writeABQpyscr(abqpyscr, m_odbName, m_odbFilePath, m_InstanceName, m_Step, m_FrameNumber, m_OutputVariable, m_ElementSet); 
	if(err < 0)
	  {
	    QString ss = QObject::tr("Error writing ABAQUS python script '%1'").arg(abqpyscr);
	    setErrorCondition(-1);
	    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
	    return;
	  }

	// Create the output Data Container
	DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
	if(getErrorCondition() < 0)
	  {
	    return;
	  }
	
	// Create our output Vertex and Cell Matrix objects
	QVector<size_t> tDims(1, 0);
	AttributeMatrix::Pointer vertexAttrMat = m->createNonPrereqAttributeMatrix(this, getVertexAttributeMatrixName(), tDims, AttributeMatrix::Type::Vertex);
	if(getErrorCondition() < 0)
	  {
	    return;
	  }
	AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Face);
	if(getErrorCondition() < 0)
	  {
	    return;
	  }
	
	QString outTxtFile = m_odbFilePath + QDir::separator() + m_odbName + ".dat";
	scanABQFile(outTxtFile, m.get(), vertexAttrMat.get(), cellAttrMat.get());
		
	break;
      }
    }

  if (getCancel()) { return; }
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

int32_t ImportFEAData::writeABQpyscr(const QString& file, QString odbName, QString odbFilePath, QString instanceName, QString step, int frameNum, QString outputVar, QString elSet) 
{
  int32_t err = 0;
  FILE* f = nullptr;
  f = fopen(file.toLatin1().data(), "wb");
  if(nullptr == f)
    {
      return -1;
    }
  
  fprintf(f, "from sys import *\n");
  fprintf(f, "from string import *\n");
  fprintf(f, "from math import *\n");
  fprintf(f, "from odbAccess import *\n");
  fprintf(f, "from abaqusConstants import *\n");
  fprintf(f, "from odbMaterial import *\n");
  fprintf(f, "from odbSection import *\n");
  fprintf(f, "\n");
  fprintf(f, "import os\n");    

  fprintf(f, "odbName = %s\n",odbName.toLatin1().data());
  fprintf(f, "odbFilePath = %s\n",odbFilePath.toLatin1().data());
  fprintf(f, "elSet = %s\n",elSet.toLatin1().data());
  fprintf(f, "outputVar = %s\n",outputVar.toLatin1().data());
  fprintf(f, "frameNum = %d\n",frameNum);
  fprintf(f, "step = '%s'\n",step.toLatin1().data());
  fprintf(f, "instanceName = %s\n",instanceName.toLatin1().data());

  fprintf(f, "odbfileName = odbFilePath + odbName + '.odb'\n"); 
  fprintf(f, "odb = openOdb(path = odbfileName)\n");   
  fprintf(f, "I1 = odb.rootAssembly.instances[instanceName].elementSets[elSet]\n"); 
  fprintf(f, "fieldOut = odb.steps[step].frames[frameNum].fieldOutputs[outputVar].getSubset(region=I1).values\n"); 

  fprintf(f, "outTxtFile = odbFilePath + odbName + '.dat'\n");
  fprintf(f, "fid = open(outTxtFile, ""a)");
  fprintf(f, "fid.write('%s\n)",outputVar.toLatin1().data());
  fprintf(f, "for j in range(len(fieldOut)):");
  fprintf(f, "        nelem = I1.elements[j].label");
  fprintf(f, "        fid.write(str(nelem) + '  ' + str(fieldOut.data[0]) + '  ' + str(fieldOut.data[1]) + '  ' + str(fieldOut.data[2]) + '  ' + str(fieldOut.data[3]) + '  ' + str(fieldOut.data[4]) + '  ' + str(fieldOut.data[5]))");
  fprintf(f,"fid.close()");
  notifyStatusMessage(getHumanLabel(), "ABAQUS python script");
  fclose(f);
  return err;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportFEAData::scanABQFile(const QString& file, DataContainer* dataContainer, AttributeMatrix* vertexAttrMat, AttributeMatrix* cellAttrMat)
{
  bool allocate = true;
  
  QFile inStream;
  inStream.setFileName(file);

  QByteArray buf;
  QList<QByteArray> tokens; /* vector to store the split data */

  bool ok = false;
  QString word("");

  // Read until you get to the vertex block
  while(word.compare("NODE") != 0)
  {
    buf = inStream.readLine();
    buf = buf.trimmed();
    buf = buf.simplified();
    tokens = buf.split(' ');
    word = tokens.at(0);
  }

  // Set the number of vertices and then create vertices array and resize vertex attr mat.
  size_t numVerts = tokens.at(1).toULongLong(&ok);
  QVector<size_t> tDims(1, numVerts);
  vertexAttrMat->resizeAttributeArrays(tDims);

  SharedVertexList::Pointer vertexPtr = QuadGeom::CreateSharedVertexList(static_cast<int64_t>(numVerts), allocate);
  float* vertex = vertexPtr->getPointer(0);

  // Read or Skip past all the vertex data
  for(size_t i = 0; i < numVerts; i++)
  {
    buf = inStream.readLine();
    if(allocate)
    {
      buf = buf.trimmed();
      buf = buf.simplified();
      tokens = buf.split(' ');
      vertex[3 * i] = tokens[1].toFloat(&ok);
      vertex[3 * i + 1] = tokens[2].toFloat(&ok);
      vertex[3 * i + 2] = 0.0;
    }
  }
  // We should now be at the Cell Connectivity section
  // Read until you get to the element block
  while(word.compare("ELEMENT") != 0)
  {
    buf = inStream.readLine();
    buf = buf.trimmed();
    buf = buf.simplified();
    tokens = buf.split(' ');
    word = tokens.at(0);
  }
  // Set the number of cells and then create cells array and resize cell attr mat.
  size_t numCells = tokens.at(1).toULongLong(&ok);
  tDims[0] = numCells;
  cellAttrMat->resizeAttributeArrays(tDims);

  QuadGeom::Pointer quadGeomPtr = QuadGeom::CreateGeometry(static_cast<int64_t>(numCells), vertexPtr, SIMPL::Geometry::QuadGeometry, allocate);
  quadGeomPtr->setSpatialDimensionality(2);
  dataContainer->setGeometry(quadGeomPtr);
  int64_t* quads = quadGeomPtr->getQuadPointer(0);

  for(size_t i = 0; i < numCells; i++)
  {
    buf = inStream.readLine();
    if(allocate)
    {
      buf = buf.trimmed();
      buf = buf.simplified();
      tokens = buf.split(' ');
      // Subtract one from the node number because ABAQUS starts at node 1 and we start at node 0
      quads[4 * i] = tokens[1].toInt(&ok) - 1;
      quads[4 * i + 1] = tokens[2].toInt(&ok) - 1;
      quads[4 * i + 2] = tokens[3].toInt(&ok) - 1;
      quads[4 * i + 3] = tokens[4].toInt(&ok) - 1;
    }
  }
  // End reading of the connectivity
  // Start reading any additional vertex or cell data arrays

  while(inStream.atEnd() == false)
  {
    // Now we are reading either cell or vertex data based on the number of items
    // being read. First Gobble up blank lines that might possibly be at the end of the file
    buf.clear();
    while(buf.size() == 0 && !inStream.atEnd())
    {
      buf = inStream.readLine();
      buf = buf.trimmed();
      buf = buf.simplified();
      tokens = buf.split(' ');
    }
    if(inStream.atEnd())
    {
      return;
    }
    QString dataArrayName = tokens.at(0);
    size_t count = numCells;

    // Read a Data set
    FloatArrayType::Pointer data = FloatArrayType::NullPointer();

    if (dataArrayName == "STRESS")
      {
	int32_t numComp = 4;
	QVector<size_t> cDims(1, static_cast<size_t>(numComp));
	data = FloatArrayType::CreateArray(count, cDims, dataArrayName, allocate);
	cellAttrMat->addAttributeArray(data->getName(), data);
	
	for(size_t i = 0; i < count; i++)
	  {
	    if(allocate)
	      {
		for(int32_t c = 0; c < numComp; c++)
		  {
		    float value = tokens[c + 1].toFloat(&ok);
		    data->setComponent(i, c, value);
		  }
	      } 
	  }
      }
    //
    //
  }  
}
//
//
//
AbstractFilter::Pointer ImportFEAData::newFilterInstance(bool copyFilterParameters) const
{
  ImportFEAData::Pointer filter = ImportFEAData::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportFEAData::getCompiledLibraryName() const
{ 
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportFEAData::getBrandingString() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportFEAData::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SimulationIO::Version::Major() << "." << SimulationIO::Version::Minor() << "." << SimulationIO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportFEAData::getGroupName() const
{ 
  return SIMPL::FilterGroups::Unsupported; 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportFEAData::getSubGroupName() const
{ 
  return "SimulationIO"; 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportFEAData::getHumanLabel() const
{ 
  return "Import FEA Data"; 
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ImportFEAData::getUuid()
{
  return QUuid("{248fdae8-a623-511b-8d09-5368c793c04d}");
}

