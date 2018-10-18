/*
 * Your License or Copyright can go here
 */

#include "ImportFEAData.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>

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
#include "SIMPLib/Geometry/HexahedralGeom.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/SIMPLibVersion.h"

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
, m_DEFORMInputFile("")
, m_BSAMInputFile("")
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

  m_Pause = false;
  m_ProcessPtr.reset();

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
                               "DEFORMInputFile",
                               "BSAMInputFile"}; 
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
  }

  {
    parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", BSAMInputFile, FilterParameter::Parameter, ImportFEAData, "", "*.DAT",1));
  }

  {
    parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", DEFORMInputFile, FilterParameter::Parameter, ImportFEAData, "", "*.DAT",3));
  }

  parameters.push_back(SIMPL_NEW_STRING_FP("Data Container Name", DataContainerName, FilterParameter::CreatedArray, ImportFEAData));
  parameters.push_back(SIMPL_NEW_STRING_FP("Vertex Attribute Matrix Name", VertexAttributeMatrixName, FilterParameter::CreatedArray, ImportFEAData));    
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix Name", CellAttributeMatrixName, FilterParameter::CreatedArray, ImportFEAData));

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
  setDEFORMInputFile(reader->readString("InputFile", getDEFORMInputFile()));
  setBSAMInputFile(reader->readString("InputFile", getBSAMInputFile()));
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

  switch(m_FEAPackage)
    {
    case 1: // BSAM
      {
	QFileInfo fi(m_BSAMInputFile);
	if(fi.exists() == false)
	  {
	    QString ss = QObject::tr("The input file does not exist: '%1'").arg(getBSAMInputFile());
	    setErrorCondition(-388);
	    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
	  }
	
	if(m_BSAMInputFile.isEmpty() == true)
	  {
	    QString ss = QObject::tr("The input file must be set for property %1").arg("InputFile");
	    setErrorCondition(-1);
	    notifyErrorMessage(getHumanLabel(), ss, -1);
	  }
	
	break;
      }

    case 3: // DEFORM
      {
	QFileInfo fi(m_DEFORMInputFile);
	if(fi.exists() == false)
	  {
	    QString ss = QObject::tr("The input file does not exist: '%1'").arg(getDEFORMInputFile());
	    setErrorCondition(-388);
	    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
	  }
	
	if(m_DEFORMInputFile.isEmpty() == true)
	  {
	    QString ss = QObject::tr("The input file must be set for property %1").arg("InputFile");
	    setErrorCondition(-1);
	    notifyErrorMessage(getHumanLabel(), ss, -1);
	  }
	
	break;
      }
    }
 
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
	    QString ss = QObject::tr("Error in accessing odb file '%1'").arg(m_odbFilePath);
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

	// Running ABAQUS python script
	runABQpyscr(abqpyscr); 

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

    case 1: // BSAM
      {
	// Create the output Data Container
	DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
	if(getErrorCondition() < 0)
	  {
	    return;
	  }
	
	// Create our output Vertex Matrix objects
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

	scanBSAMFile(m.get(), vertexAttrMat.get(), cellAttrMat.get());

	notifyStatusMessage(getHumanLabel(), "Import Complete");
	
	break;	
      }
    case 2: // PZFLEX
      {
	break;	
      }
    case 3: // DEFORM
      {
	
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
	
	scanDEFORMFile(m.get(), vertexAttrMat.get(), cellAttrMat.get());

	notifyStatusMessage(getHumanLabel(), "Import Complete");
	
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
  fprintf(f, "fieldOut = odb.steps[step].frames[frameNum].fieldOutputs[outputVar].getSubset(region=I1).values\n\n"); 

  fprintf(f, "outTxtFile = odbFilePath + odbName + '.dat'\n");
  fprintf(f, "fid = open(outTxtFile, ""a"")\n");
  fprintf(f, "fid.write('%s)\n",outputVar.toLatin1().data());
  fprintf(f, "for j in range(len(fieldOut)):\n");
  fprintf(f, "        nelem = I1.elements[j].label\n");
  fprintf(f, "        fid.write(str(nelem) + '  ' + str(fieldOut.data[0]) + '  ' + str(fieldOut.data[1]) + '  ' + str(fieldOut.data[2]) + '  ' + str(fieldOut.data[3]) + '  ' + str(fieldOut.data[4]) + '  ' + str(fieldOut.data[5]))\n");
  fprintf(f,"fid.close()");
  notifyStatusMessage(getHumanLabel(), "Finished writing ABAQUS python script");
  fclose(f);

  return err;

}

//
//
//

void ImportFEAData::runABQpyscr(const QString& file) 
{
  //cmd to run: "abaqus python filename.py

  QString program = "abaqus";
  QStringList arguments;
  arguments << "python" << file;

  m_ProcessPtr = QSharedPointer<QProcess>(new QProcess(nullptr));
  qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
  qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");
  connect(m_ProcessPtr.data(), SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processHasFinished(int, QProcess::ExitStatus)), Qt::QueuedConnection);
  connect(m_ProcessPtr.data(), SIGNAL(error(QProcess::ProcessError)), this, SLOT(processHasErroredOut(QProcess::ProcessError)), Qt::QueuedConnection);
  connect(m_ProcessPtr.data(), SIGNAL(readyReadStandardError()), this, SLOT(sendErrorOutput()), Qt::QueuedConnection);
  connect(m_ProcessPtr.data(), SIGNAL(readyReadStandardOutput()), this, SLOT(sendStandardOutput()), Qt::QueuedConnection);

  m_ProcessPtr->setWorkingDirectory(m_odbFilePath);
  m_ProcessPtr->start(program, arguments);
  m_ProcessPtr->waitForStarted(2000);
  m_ProcessPtr->waitForFinished();

  notifyStatusMessage(getHumanLabel(), "Finished running ABAQUS python script");

}

//
//
//

void ImportFEAData::processHasFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
  if(getCancel())
  {
  }
  else if(exitStatus == QProcess::CrashExit)
  {
    QString ss = QObject::tr("The process crashed during its exit.");
    setErrorCondition(-4003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if(exitCode < 0)
  {
    QString ss = QObject::tr("The process finished with exit code %1.").arg(QString::number(exitCode));
    setErrorCondition(-4004);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if(getErrorCondition() >= 0)
  {
    notifyStatusMessage(getHumanLabel(), "Complete");
  }

  m_Pause = false;
  m_WaitCondition.wakeAll();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void ImportFEAData::processHasErroredOut(QProcess::ProcessError error)
{
  if(getCancel())
  {
    QString ss = QObject::tr("The process was killed by the user.");
    setWarningCondition(-4004);
    notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
  }
  else if(error == QProcess::FailedToStart)
  {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString pathEnv = env.value("PATH");

    QString ss = QObject::tr("The process failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program \
or the path containing the executble is not in the system's environment path. PATH=%1.\n Try using the absolute path to the executable.")
                     .arg(pathEnv);
    setErrorCondition(-4005);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if(error == QProcess::Crashed)
  {
    QString ss = QObject::tr("The process crashed some time after starting successfully.");
    setErrorCondition(-4006);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if(error == QProcess::Timedout)
  {
    QString ss = QObject::tr("The process timed out.");
    setErrorCondition(-4007);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if(error == QProcess::WriteError)
  {
    QString ss = QObject::tr("An error occurred when attempting to write to the process.");
    setErrorCondition(-4008);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if(error == QProcess::ReadError)
  {
    QString ss = QObject::tr("An error occurred when attempting to read from the process.");
    setErrorCondition(-4009);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else
  {
    QString ss = QObject::tr("An unknown error occurred.");
    setErrorCondition(-4010);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  m_Pause = false;
  m_WaitCondition.wakeAll();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void ImportFEAData::sendErrorOutput()
{
  if(m_ProcessPtr.data() != nullptr)
  {
    QString error = m_ProcessPtr->readAllStandardError();
    if(error[error.size() - 1] == '\n')
    {
      error.chop(1);
    }
    notifyStandardOutputMessage(getHumanLabel(), getPipelineIndex() + 1, error);
    m_WaitCondition.wakeAll();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void ImportFEAData::sendStandardOutput()
{
  if(m_ProcessPtr.data() != nullptr)
  {
    notifyStandardOutputMessage(getHumanLabel(), getPipelineIndex() + 1, m_ProcessPtr->readAllStandardOutput());
    m_WaitCondition.wakeAll();
  }
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

void ImportFEAData::scanDEFORMFile(DataContainer* dataContainer, AttributeMatrix* vertexAttrMat, AttributeMatrix* cellAttrMat)
{
  bool allocate = true;

  QFile inStream;
  inStream.setFileName(getDEFORMInputFile());
  if(!inStream.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Input file could not be opened: %1").arg(getDEFORMInputFile());
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QByteArray buf;
  QList<QByteArray> tokens; /* vector to store the split data */

  bool ok = false;
  QString word("");

  // Read until you get to the vertex block
  while(word.compare("RZ") != 0)
  {
    buf = inStream.readLine();
    buf = buf.trimmed();
    buf = buf.simplified();
    tokens = buf.split(' ');
    word = tokens.at(0);
  }

  // Set the number of vertices and then create vertices array and resize vertex attr mat.
  size_t numVerts = tokens.at(2).toULongLong(&ok);
  QVector<size_t> tDims(1, numVerts);
  vertexAttrMat->resizeAttributeArrays(tDims);

  SharedVertexList::Pointer vertexPtr = QuadGeom::CreateSharedVertexList(static_cast<int64_t>(numVerts), allocate);
  float* vertex = vertexPtr->getPointer(0);
  QString status;
  QTextStream ss(&status);
  ss << "DEFORM Data File: Number of Vetex Points=" << numVerts;
  notifyStatusMessage(getHumanLabel(), status);

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
  while(word.compare("ELMCON") != 0)
  {
    buf = inStream.readLine();
    buf = buf.trimmed();
    buf = buf.simplified();
    tokens = buf.split(' ');
    word = tokens.at(0);
  }
  // Set the number of cells and then create cells array and resize cell attr mat.
  size_t numCells = tokens.at(2).toULongLong(&ok);
  tDims[0] = numCells;
  status = "";
  ss << "DEFORM Data File: Number of Quad Cells=" << numCells;
  notifyStatusMessage(getHumanLabel(), status);
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
      // Subtract one from the node number because DEFORM starts at node 1 and we start at node 0
      quads[4 * i] = tokens[1].toInt(&ok) - 1;
      quads[4 * i + 1] = tokens[2].toInt(&ok) - 1;
      quads[4 * i + 2] = tokens[3].toInt(&ok) - 1;
      quads[4 * i + 3] = tokens[4].toInt(&ok) - 1;
    }
  }
  // End reading of the connectivity
  // Start reading any additional vertex or cell data arrays

  status = "";
  ss << "Scanning for Vertex & Cell data....";
  notifyStatusMessage(getHumanLabel(), status);
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
    size_t count = tokens.at(2).toULongLong(&ok);

    if(count != numVerts && count != numCells)
    {
      setErrorCondition(-96000);
      QString msg = QString("Reading %1 Data from DEFORM data file, data array does not have a number of entries (%2) equal to the number of vertices (%3) or cells (%4)")
                        .arg(dataArrayName)
                        .arg(count)
                        .arg(numVerts)
                        .arg(numCells);
      notifyErrorMessage(getHumanLabel(), msg, getErrorCondition());
      return;
    }
    if(inStream.atEnd())
    {
      return;
    }

    // Read a Data set
    FloatArrayType::Pointer data = FloatArrayType::NullPointer();
    for(size_t i = 0; i < count; i++)
    {
      int32_t numComp = 0;
      buf = inStream.readLine();
      buf = buf.trimmed();
      buf = buf.simplified();
      tokens = buf.split(' ');
      numComp = tokens.count() - 1;
      if(i == 0)
      {
        QVector<size_t> cDims(1, static_cast<size_t>(numComp));
        data = FloatArrayType::CreateArray(count, cDims, dataArrayName, allocate);
        if(count == numVerts)
        {
          vertexAttrMat->addAttributeArray(data->getName(), data);
          status = "";
          ss << "Reading Vertex Data: " << data->getName();
          notifyStatusMessage(getHumanLabel(), status);
        }
        else if(count == numCells)
        {
          cellAttrMat->addAttributeArray(data->getName(), data);
          status = "";
          ss << "Reading Cell Data: " << data->getName();
          notifyStatusMessage(getHumanLabel(), status);
        }
      }
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
}

//
//
//

void ImportFEAData::scanBSAMFile(DataContainer* dataContainer, AttributeMatrix* vertexAttrMat, AttributeMatrix* cellAttrMat)
{
  bool allocate = true;

  QFile inStream;
  inStream.setFileName(getBSAMInputFile());
  if(!inStream.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Input file could not be opened: %1").arg(getBSAMInputFile());
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QByteArray buf;
  QList<QByteArray> tokens; /* vector to store the split data */

  bool ok = false;
  QString word("");

  // Read until you get to the vertex block
  while(word.compare("n=") != 0)
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
  QString status;
  QTextStream ss(&status);
  ss << "BSAM Data File: Number of Vetex Points=" << numVerts;
  notifyStatusMessage(getHumanLabel(), status);
  SharedVertexList::Pointer vertexPtr = HexahedralGeom::CreateSharedVertexList(static_cast<int64_t>(numVerts), allocate);
  float* vertex = vertexPtr->getPointer(0);

  // Set the number of cells and then create cells array and resize cell attr mat.
  size_t numCells = tokens.at(4).toULongLong(&ok);
  tDims[0] = numCells;
  status = "";
  ss << "BSAM Data File: Number of Quad Cells=" << numCells;
  notifyStatusMessage(getHumanLabel(), status);
  cellAttrMat->resizeAttributeArrays(tDims);
  HexahedralGeom::Pointer hexGeomPtr = HexahedralGeom::CreateGeometry(static_cast<int64_t>(numCells), vertexPtr, SIMPL::Geometry::HexahedralGeometry, allocate);
  hexGeomPtr->setSpatialDimensionality(3);
  dataContainer->setGeometry(hexGeomPtr);
  int64_t* hexs = hexGeomPtr->getHexPointer(0);

  QString dataArrayName = "DISPLACEMENT";
  FloatArrayType::Pointer dispdata = FloatArrayType::NullPointer();
  int32_t numDispComp = 3;
  QVector<size_t> cDims(1, static_cast<size_t>(numDispComp));
  dispdata = FloatArrayType::CreateArray(numVerts, cDims, dataArrayName, allocate);
  vertexAttrMat->addAttributeArray(dispdata->getName(), dispdata);

  // Read or Skip past all the vertex data
  for(size_t i = 0; i < numVerts; i++)
  {
    buf = inStream.readLine();
    if(allocate)
    {
      buf = buf.trimmed();
      buf = buf.simplified();
      tokens = buf.split(' ');
      vertex[3 * i] = tokens[0].toFloat(&ok);
      vertex[3 * i + 1] = tokens[1].toFloat(&ok);
      vertex[3 * i + 2] = tokens[2].toFloat(&ok);

      for(int32_t c = 0; c < numDispComp; c++)
        {
          float value = tokens[c + 3].toFloat(&ok);
          dispdata->setComponent(i, c, value);
        }
	
    }
  }

  for(size_t i = 0; i < numCells; i++)
    {
      buf = inStream.readLine();
      if(allocate)
	{
	  buf = buf.trimmed();
	  buf = buf.simplified();
	  tokens = buf.split(' ');
	  // Subtract one from the node number because BSAM starts at node 1 and we start at node 0
	  hexs[8 * i] = tokens[0].toInt(&ok) - 1;
	  hexs[8 * i + 1] = tokens[1].toInt(&ok) - 1;
	  hexs[8 * i + 2] = tokens[2].toInt(&ok) - 1;
	  hexs[8 * i + 3] = tokens[3].toInt(&ok) - 1;
	  hexs[8 * i + 4] = tokens[4].toInt(&ok) - 1;
	  hexs[8 * i + 5] = tokens[5].toInt(&ok) - 1;
	  hexs[8 * i + 6] = tokens[6].toInt(&ok) - 1;
	  hexs[8 * i + 7] = tokens[7].toInt(&ok) - 1;
	}
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

