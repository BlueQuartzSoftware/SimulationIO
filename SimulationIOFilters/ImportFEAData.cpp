/*
 * Your License or Copyright can go here
 */

#include "ImportFEAData.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>
#include <QtCore/QString>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.h"
#include "SIMPLib/DataContainers/DataContainerBundle.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/HexahedralGeom.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/QuadGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Utilities/TimeUtilities.h"

#include "SimulationIO/SimulationIOConstants.h"
#include "SimulationIO/SimulationIOVersion.h"

#define READ_DEF_PT_TRACKING_TIME_INDEX "Time Index"

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
, m_DEFORMPointTrackInputFile("")
, m_TimeSeriesBundleName(SIMPL::Defaults::TimeSeriesBundleName)
, m_ImportSingleTimeStep(false)
, m_SingleTimeStepValue(0)
, m_SelectedTimeArrayName(SimulationIOConstants::DEFORMData::Time)
, m_SelectedTimeStepArrayName(SimulationIOConstants::DEFORMData::Step)
, m_SelectedPointNumArrayName(SimulationIOConstants::DEFORMData::PointNum)
, m_SelectedXCoordArrayName(SimulationIOConstants::DEFORMData::RXCoord)
, m_SelectedYCoordArrayName(SimulationIOConstants::DEFORMData::ZYCoord)
{
  m_BundleMetaDataAMName = DataContainerBundle::GetMetaDataName();

  m_DataTypes[SimulationIOConstants::DEFORMData::DamageFactor] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::EffStrain] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::EffStrainRate] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::EffStress] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::MaxPrincStrain] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::MaxPrincStrainRate] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::MaxPrincStress] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::MeanStress] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::RelativeDensity] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::StrokeX] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::StrokeY] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::Temperature] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::XRStrain] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::XRStrainRate] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::XRStress] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::XYRZStrain] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::XYRZStrainRate] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::XYRZStress] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::YZStrain] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::YZStrainRate] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::YZStress] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::ZThetaStrain] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::ZThetaStrainRate] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::ZThetaStress] = SIMPL::TypeNames::Float;

  m_DataTypes[SimulationIOConstants::DEFORMData::PointNum] = SIMPL::TypeNames::Int32;

  // Vertex Coords
  m_DataTypes[SimulationIOConstants::DEFORMData::RXCoord] = SIMPL::TypeNames::Float;
  m_DataTypes[SimulationIOConstants::DEFORMData::ZYCoord] = SIMPL::TypeNames::Float;

  // Meta Data
  m_DataTypes[SimulationIOConstants::DEFORMData::Step] = SIMPL::TypeNames::Int32;
  m_DataTypes[SimulationIOConstants::DEFORMData::Time] = SIMPL::TypeNames::Float;

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
  m_CachedFileName = QString("");
  if(m_InStream.isOpen())
  {
    m_InStream.close();
  }
  m_DataTypes.clear();
  m_NamePointerMap.clear();
  m_NumBlocks = 0;
  m_NumPoints = 0;
  m_NumTimeSteps = 0;
  m_LinesPerBlock = 0;
  m_HeaderIsComplete = false;
  m_selectedTimeStepValue = 0;
  m_selectedTimeStep = false;

  m_BundleMetaDataAMName = QString("");
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
    choices.push_back("DEFORM_POINT_TRACK");
    parameter->setChoices(choices);
    QStringList linkedProps = {"odbName",
                               "odbFilePath",
                               "InstanceName",
			       "Step", 
			       "FrameNumber",    
			       "OutputVariable",    
                               "ElementSet",
                               "DEFORMInputFile",
                               "BSAMInputFile",
                               "DEFORMPointTrackInputFile",
                               "ImportSingleTimeStep",
                               "SingleTimeStepValue",
                               "TimeSeriesBundleName"}; 
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


  {
    parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", DEFORMPointTrackInputFile, FilterParameter::Parameter, ImportFEAData, "", "*.RST", 4));

    QStringList linkedProps("SingleTimeStepValue");
    parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Read Single Time Step", ImportSingleTimeStep, FilterParameter::Parameter, ImportFEAData, linkedProps, 4));
    linkedProps.clear();
    parameters.push_back(SIMPL_NEW_INTEGER_FP("Time Step", SingleTimeStepValue, FilterParameter::Parameter, ImportFEAData, 4));

    parameters.push_back(SeparatorFilterParameter::New("", FilterParameter::CreatedArray));
    parameters.push_back(SIMPL_NEW_STRING_FP("Time Series Bundle Name", TimeSeriesBundleName, FilterParameter::CreatedArray, ImportFEAData,4));
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

  setDEFORMPointTrackInputFile(reader->readString("InputFile", getDEFORMPointTrackInputFile()));
  //setDataContainerName(reader->readString("DataContainerName", getDataContainerName()));
  setTimeSeriesBundleName(reader->readString("TimeSeriesBundleName", getTimeSeriesBundleName()));

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void ImportFEAData::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);

  m_Pause = false;
  m_ProcessPtr.reset();

  switch(m_FEAPackage)
    {
    case 1: // BSAM
      {
	QFileInfo fi(m_BSAMInputFile);
  if(!fi.exists())
  {
    QString ss = QObject::tr("The input file does not exist: '%1'").arg(getBSAMInputFile());
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(m_BSAMInputFile.isEmpty())
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
  if(!fi.exists())
  {
    QString ss = QObject::tr("The input file does not exist: '%1'").arg(getDEFORMInputFile());
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(m_DEFORMInputFile.isEmpty())
  {
    QString ss = QObject::tr("The input file must be set for property %1").arg("InputFile");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, -1);
  }

  break;
      }
    case 4: // DEFORM POINT TRACK
      {
	QFileInfo fi(m_DEFORMPointTrackInputFile);
  if(!fi.exists())
  {
    QString ss = QObject::tr("The input file does not exist: '%1'").arg(getDEFORMPointTrackInputFile());
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(m_DEFORMPointTrackInputFile.isEmpty())
  {
    QString ss = QObject::tr("The input file must be set for property %1").arg("InputFile");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, -1);
  }

  // If any of the checks above threw errors then we can not go any further so bail out now.
	if(getErrorCondition() < 0)
	  {
	    return;
	  }

	// Create the time series bundle
	DataContainerBundle::Pointer dcb = DataContainerBundle::New(getTimeSeriesBundleName());
	getDataContainerArray()->addDataContainerBundle(dcb);

	// Add the names of the arrays within the MetaData AttributeMatrix of each data container stored in the bundle
	// that define how/why the bundle was created.
	QStringList metaArrayList;
	metaArrayList << getSelectedTimeArrayName() << getSelectedTimeStepArrayName() << READ_DEF_PT_TRACKING_TIME_INDEX;
	dcb->setMetaDataArrays(metaArrayList);

	m_InStream.setFileName(getDEFORMPointTrackInputFile());

	// Read the Header of the file to figure out what arrays we have
	readHeader(m_InStream);

	// Close the file if we are preflighting
  if(getInPreflight())
  {
    m_InStream.close();
  }

  // Make sure we did not have any errors
	if(getErrorCondition() < 0)
	  {
	    QString ss = QObject::tr("Error reading header information from file: '%1'").arg(getDEFORMPointTrackInputFile());
	    setErrorCondition(-389);
	    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
	    return;
	  }

	if(getImportSingleTimeStep() != m_selectedTimeStep || getSingleTimeStepValue() != m_selectedTimeStepValue)
	  {
	    if(getImportSingleTimeStep() && getSingleTimeStepValue() >= m_NumTimeSteps)
	      {
		QString ss = QObject::tr("Please select a timestep in the range");
		setErrorCondition(-91010);
		notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
		return;
	      }
	    m_selectedTimeStep = getImportSingleTimeStep();
	    m_selectedTimeStepValue = getSingleTimeStepValue();
	  }	

	// Now generate the complete set of Data Containers for our Time Steps, Each Data Container has an AttributeMatrix with the set of data arrays
	for(int t = 0; t < m_NumTimeSteps; ++t)
	  {

	    if(m_selectedTimeStep && t != m_selectedTimeStepValue)
	      {
		continue;
	      }

	    // Create the output Data Container for the first time step
	    QString dcName = getDataContainerName() + "_" + QString::number(t);
	    DataContainer::Pointer v = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, dcName);
	    if(getErrorCondition() < 0)
	      {
		QString ss = QObject::tr("Error Creating Vertex Data Container with name '%1' for Time Step %2").arg(dcName).arg(t);
		setErrorCondition(-390);
		notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
		return;
	      }

	    VertexGeom::Pointer vertices = VertexGeom::CreateGeometry(m_NumPoints, SIMPL::Geometry::VertexGeometry, !getInPreflight());
	    v->setGeometry(vertices);

	    QVector<size_t> tDims(1, m_NumPoints);
	    AttributeMatrix::Pointer vertexAttrMat = v->createNonPrereqAttributeMatrix(this, SIMPL::Defaults::VertexAttributeMatrixName, tDims, AttributeMatrix::Type::Vertex);
	    if(getErrorCondition() < 0)
	      {
		QString ss = QObject::tr("Error Creating AttributeMatrix with name '%1' for Time Step %2").arg(SIMPL::Defaults::VertexAttributeMatrixName).arg(t);
		setErrorCondition(-390);
		notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
		return;
	      }

	    QMapIterator<QString, SimulationIO::DeformDataParser::Pointer> parserIter(m_NamePointerMap);
	    while(parserIter.hasNext())
	      {
		parserIter.next();
		QString name = parserIter.key();
		SimulationIO::DeformDataParser::Pointer parser = parserIter.value();
		IDataArray::Pointer dataPtr = parser->initializeNewDataArray(m_NumPoints, name, !getInPreflight()); // Get a copy of the DataArray

    if((getInPreflight()))
    {
      if((name.compare(getSelectedTimeArrayName()) != 0) && (name.compare(getSelectedTimeStepArrayName()) != 0) && (name.compare(getSelectedPointNumArrayName()) != 0) &&
         (name.compare(getSelectedXCoordArrayName()) != 0) && (name.compare(getSelectedYCoordArrayName()) != 0))
      {
        vertexAttrMat->addAttributeArray(dataPtr->getName(), dataPtr);
      }
    }
    else
		  {
		    vertexAttrMat->addAttributeArray(dataPtr->getName(), dataPtr);
		  }
	      }

	    // Generate the AttributeMatrix that will serve as the Meta-Data information for the DataContainerBundle
	    QVector<size_t> bundleAttrDims(1, 1);
	    AttributeMatrix::Pointer metaData = v->createNonPrereqAttributeMatrix(this, m_BundleMetaDataAMName, bundleAttrDims, AttributeMatrix::Type::MetaData);
	    if(getErrorCondition() < 0)
	      {
	      }

	    QVector<size_t> cDims(1, 1);
	    metaData->createNonPrereqArray<FloatArrayType, AbstractFilter, float>(this, getSelectedTimeArrayName(), 0.0f, cDims);
	    metaData->createNonPrereqArray<Int32ArrayType, AbstractFilter, int32_t>(this, getSelectedTimeStepArrayName(), 0, cDims);
	    metaData->createNonPrereqArray<Int32ArrayType, AbstractFilter, int32_t>(this, READ_DEF_PT_TRACKING_TIME_INDEX, 0, cDims);
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
  //initialize();
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
	AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell);
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
	AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell);
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
	AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell);
	if(getErrorCondition() < 0)
	  {
	    return;
	  }
	
	scanDEFORMFile(m.get(), vertexAttrMat.get(), cellAttrMat.get());

	notifyStatusMessage(getHumanLabel(), "Import Complete");
	
	break;
      }
    case 4:// DEFORM POINT TRACK
      {
	for(size_t i = 0; i < m_NumTimeSteps; i++)
	  {
	    QString ss = QObject::tr("Starting to read time step %1 of %2").arg(i).arg(m_NumTimeSteps);
	    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
	    readTimeStep(m_InStream, i);
	  }
	
	/* Let the GUI know we are done with this filter */
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
  fprintf(f, "E1 = odb.rootAssembly.instances[instanceName].elementSets[elSet]\n"); 

  fprintf(f, "n2 = len(E1.elements)\n"); 
  fprintf(f, "print 'ELEMENTS %cd', n2 \n",'%'); 
  fprintf(f, "for element in E1.elements  \n");
  fprintf(f, "    print '%c5d %c8s' %c (element.label,element.type)\n",'%','%','%'); 
  fprintf(f, "    for nodeNum in element.connectivity:\n"); 
  fprintf(f, "        print '%c4d', nodeNum\n",'%'); 
  fprintf(f, "    print\n"); 

  fprintf(f, "n1 = len(E1.nodes)\n"); 
  fprintf(f, "print 'NODES %cd', n1 \n",'%'); 
  fprintf(f, "for node in E1.nodes  \n");
  fprintf(f, "    print '%c5d', node.label",'%');  
  fprintf(f, "    print node.coordinates  \n");  
 
  fprintf(f, "fieldOut = odb.steps[step].frames[frameNum].fieldOutputs[outputVar].getSubset(region=E1).values\n\n"); 

  fprintf(f, "outTxtFile = odbFilePath + odbName + '.dat'\n");
  fprintf(f, "fid = open(outTxtFile, ""a"")\n");
  fprintf(f, "fid.write(%s)\n",outputVar.toLatin1().data());
  fprintf(f, "for j in range(len(fieldOut)):\n");
  fprintf(f, "        nelem = E1.elements[j].label\n");
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

  qint64 dataOffset = inStream.pos();

  // Cell Connectivity section
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
  QVector<size_t> tDims(1, numCells);
  cellAttrMat->resizeAttributeArrays(tDims);
  QString eleType = tokens.at(2);

  if ( eleType == 'CPE4' || eleType == 'CPS4' )
    {
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
      tDims[0] = numVerts;
      //      QVector<size_t> tDims(1, numVerts);
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
      
      inStream.seek(dataOffset);

      while(word.compare("ELEMENT") != 0)
	{
	  buf = inStream.readLine();
	  buf = buf.trimmed();
	  buf = buf.simplified();
	  tokens = buf.split(' ');
	  word = tokens.at(0);
	}

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

  while(!inStream.atEnd())
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

	  if (dataArrayName == "S")
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

  if ( eleType == 'C3D8' )
    {
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
      tDims[0] = numVerts;
      vertexAttrMat->resizeAttributeArrays(tDims);
      
      SharedVertexList::Pointer vertexPtr = HexahedralGeom::CreateSharedVertexList(static_cast<int64_t>(numVerts), allocate);
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
	      vertex[3 * i + 2] = tokens[3].toFloat(&ok);
	    }
	}
      
      inStream.seek(dataOffset);

      while(word.compare("ELEMENT") != 0)
	{
	  buf = inStream.readLine();
	  buf = buf.trimmed();
	  buf = buf.simplified();
	  tokens = buf.split(' ');
	  word = tokens.at(0);
	}

      HexahedralGeom::Pointer hexGeomPtr = HexahedralGeom::CreateGeometry(static_cast<int64_t>(numCells), vertexPtr, SIMPL::Geometry::HexahedralGeometry, allocate);
      hexGeomPtr->setSpatialDimensionality(3);
      dataContainer->setGeometry(hexGeomPtr);
      int64_t* hexs = hexGeomPtr->getHexPointer(0);

      for(size_t i = 0; i < numCells; i++)
	{
	  buf = inStream.readLine();
	  if(allocate)
	    {
	      buf = buf.trimmed();
	      buf = buf.simplified();
	      tokens = buf.split(' ');
	      // Subtract one from the node number because ABAQUS starts at node 1 and we start at node 0
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
      // End reading of the connectivity

      // Start reading any additional vertex or cell data arrays

  while(!inStream.atEnd())
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

	  if (dataArrayName == "S")
	    {
	      int32_t numComp = 6;
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
  while(!inStream.atEnd())
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

  dataArrayName = "STRESS";
  FloatArrayType::Pointer stressdata = FloatArrayType::NullPointer();
  int32_t numStrComp = 6;
  cDims[0] = static_cast<size_t>(numStrComp);
  stressdata = FloatArrayType::CreateArray(numVerts, cDims, dataArrayName, allocate);
  vertexAttrMat->addAttributeArray(stressdata->getName(), stressdata);

  dataArrayName = "STRAIN";
  FloatArrayType::Pointer straindata = FloatArrayType::NullPointer();
  straindata = FloatArrayType::CreateArray(numVerts, cDims, dataArrayName, allocate);
  vertexAttrMat->addAttributeArray(straindata->getName(), straindata);

  dataArrayName = "CLUSTER";
  Int32ArrayType::Pointer clusterdata = Int32ArrayType::NullPointer();
  int32_t numClusterComp = 1;
  cDims[0] = static_cast<size_t>(numClusterComp);
  clusterdata = Int32ArrayType::CreateArray(numVerts, cDims, dataArrayName, allocate);
  vertexAttrMat->addAttributeArray(clusterdata->getName(), clusterdata);

  dataArrayName = "VA";
  FloatArrayType::Pointer vadata = FloatArrayType::NullPointer();
  int32_t numVaComp = 4;
  cDims[0] = static_cast<size_t>(numVaComp);
  vadata = FloatArrayType::CreateArray(numVerts, cDims, dataArrayName, allocate);
  vertexAttrMat->addAttributeArray(vadata->getName(), vadata);

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
	
      for(int32_t c = 0; c < numStrComp; c++)
        {
          float value = tokens[c + 6].toFloat(&ok);
          stressdata->setComponent(i, c, value);
        }

      for(int32_t c = 0; c < numStrComp; c++)
        {
          float value = tokens[c + 12].toFloat(&ok);
          straindata->setComponent(i, c, value);
        }

      for(int32_t c = 0; c < numClusterComp; c++)
        {
          int32_t cvalue = tokens[c + 18].toInt(&ok);
          clusterdata->setComponent(i, c, cvalue);
        }

      for(int32_t c = 0; c < numVaComp; c++)
        {
          float value = tokens[c + 20].toFloat(&ok);
          vadata->setComponent(i, c, value);
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

void ImportFEAData::readHeader(QFile& reader)
{
  setErrorCondition(0);
  setWarningCondition(0);

  if(!reader.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("The Input Point Tracking file could not be opened.").arg(getDEFORMPointTrackInputFile());
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QByteArray buf;
  m_HeaderIsComplete = false;
  qint32 lineNum = 0;

  QString origHeader;
  m_LinesPerBlock = 0;
  QList<QByteArray> headerLines;
  while(!reader.atEnd() && !m_HeaderIsComplete)
  {
    buf = reader.readLine();
    lineNum++;
    if(buf[0] != '*')
    {
      m_HeaderIsComplete = true;
      continue;
    }

    // Append the line to the complete header
    origHeader.append(QString(buf));

    // remove the newline at the end of the line
    buf.chop(1);
    headerLines.push_back(buf);
    if(buf.contains("Each Record contains"))
    {
      m_LinesPerBlock = atoi(buf.split(' ').at(4).constData());
    }
  }

  // Now start building up our QList of Tokens which are the names
  QList<QByteArray> blockTokens;
  // buf still contains the first line of the header section that describes the column names of the data
  int numLinesToRead = m_LinesPerBlock * 2;
  bool processLine = true;
  for(int i = 0; i < numLinesToRead; ++i)
  {
    buf = buf.trimmed();
    // We only want to actually process every OTHER line at this point
    if(processLine)
    {
      // Find the ':' character so we know how many characters from the front of the buffer to remove
      for(int c = 0; c < buf.size(); ++c)
      {
        if(buf[c] == ':')
        {
          buf = buf.mid(c + 2);
          c = buf.size();
          continue;
        }
      }
      // We do this because if the '/' character is in the Name of a DataContainer, Attribut Matrix
      // or Data Array, HDF5 will have an issue trying to write a data set with that name
      buf.replace('/', '_');

      // Check to make sure the last char is NOT a ',' as the Qt API will keep an empty token for us.
      if(buf[buf.size() - 1] == ',')
      {
        buf = buf.remove(buf.size() - 1, 1);
      }

      blockTokens = blockTokens + buf.split(',');
    }
    processLine = !processLine;
    buf = reader.readLine();
    lineNum++;
  }
  m_LinesPerBlock = m_LinesPerBlock + 1; // This compensates for the extra Line Break at the start of each block

  qint32 currentLineNum = lineNum;
  // Store the current byte position in the file so that we can come back to it if needed.
  qint64 dataOffset = reader.pos();

  // Now scan to the end so we can figure out how many lines, blocks, and nodes we have.
  qint32 p = 0;
  QVector<QByteArray> lastBlock(m_LinesPerBlock);
  while(!reader.atEnd())
  {
    lastBlock[p] = reader.readLine();
    lineNum++;
    p++;
    if(p == m_LinesPerBlock)
    {
      p = 0;
    }
  }

  // Now we are at the end of the file and reset back to the data section
  reader.seek(dataOffset);

  // Now that we have all the tokens, lets parse through them and create our Map of Names<==>Parsers
  QListIterator<QByteArray> blockIter(blockTokens);
  int index = 0;
  while(blockIter.hasNext())
  {
    const QByteArray& current = blockIter.next();
    QString name = QString::fromLatin1(current).trimmed();
    QString value = m_DataTypes[name];

    if(value.compare(SIMPL::TypeNames::Float) == 0)
    {
      FloatArrayType::Pointer data = FloatArrayType::CreateArray(0, name, false);
      SimulationIO::FloatParser::Pointer dparser = SimulationIO::FloatParser::New(data, name, index);
      m_NamePointerMap.insert(name, dparser);
    }
    else if(value.compare(SIMPL::TypeNames::Int32) == 0)
    {
      Int32ArrayType::Pointer data = Int32ArrayType::CreateArray(0, name, false);
      SimulationIO::Int32Parser::Pointer dparser = SimulationIO::Int32Parser::New(data, name, index);
      m_NamePointerMap.insert(name, dparser);
    }
    else
    {
      FloatArrayType::Pointer data = FloatArrayType::CreateArray(0, name, false);
      SimulationIO::FloatParser::Pointer dparser = SimulationIO::FloatParser::New(data, name, index);
      m_NamePointerMap.insert(name, dparser);
      QString ss = QObject::tr("Data Block Column Name '%1' was unknown. We will parse the data as Floating point Data (32 Bit)").arg(name);
      setWarningCondition(-1);
      notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
    }
    index++;
  }

  // Look for the Parser for the 'Point #' variable so we can use it to correctly parse the value from the last block
  SimulationIO::DeformDataParser::Pointer parser = m_NamePointerMap[SimulationIOConstants::DEFORMData::PointNum];
  if(nullptr == parser.get())
  {
    Q_ASSERT_X(nullptr != parser.get(), "Point # data column was not found in the Deform Data File", "");
  }

  QVector<QByteArray> tokens = splitDataBlock(lastBlock);
  bool ok = false;
  m_NumPoints = tokens[parser->getColumnIndex()].toInt(&ok);
  m_NumBlocks = (lineNum - currentLineNum) / m_LinesPerBlock;
  m_NumTimeSteps = m_NumBlocks / m_NumPoints;

  qDebug() << "numPoints: " << m_NumPoints;
  qDebug() << "numBlocks: " << m_NumBlocks;
  qDebug() << "numTimeSteps: " << m_NumTimeSteps;

  m_DataArrayList.clear();

  // Now just redo the Parser with the correct number of points to allocate
  blockIter.toFront();
  index = 0;
  while(blockIter.hasNext())
  {
    const QByteArray& current = blockIter.next();
    QString name = QString::fromLatin1(current).trimmed();
    QString value = m_DataTypes[name];

    if(value.compare(SIMPL::TypeNames::Float) == 0)
    {
      FloatArrayType::Pointer data = FloatArrayType::CreateArray(m_NumPoints, name, false);
      SimulationIO::FloatParser::Pointer dparser = SimulationIO::FloatParser::New(data, name, index);
      m_NamePointerMap.insert(name, dparser);
    }
    else if(value.compare(SIMPL::TypeNames::Int32) == 0)
    {
      Int32ArrayType::Pointer data = Int32ArrayType::CreateArray(m_NumPoints, name, false);
      SimulationIO::Int32Parser::Pointer dparser = SimulationIO::Int32Parser::New(data, name, index);
      m_NamePointerMap.insert(name, dparser);
    }
    else
    {
      FloatArrayType::Pointer data = FloatArrayType::CreateArray(m_NumPoints, name, false);
      SimulationIO::FloatParser::Pointer dparser = SimulationIO::FloatParser::New(data, name, index);
      m_NamePointerMap.insert(name, dparser);
      QString ss = QObject::tr("Data Block Column Name '%1' was unknown. We will parse the data as Floating point Data (32 Bit)").arg(name);
      setWarningCondition(-1);
      notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
    }
    m_DataArrayList << name;
    index++;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QByteArray> ImportFEAData::splitDataBlock(QVector<QByteArray>& dataBlock)
{
  QVector<QByteArray> tokens;
  for(int i = 0; i < dataBlock.size(); ++i)
  {
    QList<QByteArray> lineTokens = dataBlock[i].trimmed().split(' ');
    for(int j = 0; j < lineTokens.size(); ++j)
    {
      if(!lineTokens[j].isEmpty())
      {
        tokens.push_back(lineTokens[j]);
      }
    }
  }
  return tokens;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportFEAData::readTimeStep(QFile& reader, qint32 t)
{

  // Skip past the data if we are not reading this time step.
  if(m_selectedTimeStep && t != m_selectedTimeStepValue)
  {
    QString ss = QObject::tr("Skipping time step %1 of %2").arg(t).arg(m_NumTimeSteps);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    for(size_t nodeIdx = 0; nodeIdx < m_NumPoints*m_LinesPerBlock; ++nodeIdx)
    {
      QByteArray line = reader.readLine();
    }
    return;
  }

  QString dcName = getDataContainerName() + "_" + QString::number(t);

  DataContainer::Pointer v = getDataContainerArray()->getDataContainer(dcName);
  VertexGeom::Pointer vertices = v->getGeometryAs<VertexGeom>();

  AttributeMatrix::Pointer attrMat = v->getAttributeMatrix(SIMPL::Defaults::VertexAttributeMatrixName);

  // Vertex Coords for each Vertex
  FloatArrayType::Pointer xCoordsPtr = FloatArrayType::NullPointer();
  FloatArrayType::Pointer yCoordsPtr = FloatArrayType::NullPointer();

  // We want this array because we are going to delete it as it has redundant information
  IDataArray::Pointer pointNumPtr = IDataArray::NullPointer();

  // Meta Data Information arrays
  Int32ArrayType::Pointer timeStepPtr = Int32ArrayType::NullPointer();
  FloatArrayType::Pointer timeValuePtr = FloatArrayType::NullPointer();

  QMapIterator<QString, SimulationIO::DeformDataParser::Pointer> parserIter(m_NamePointerMap);
  while(parserIter.hasNext())
  {
    parserIter.next();
    QString name = parserIter.key();
    SimulationIO::DeformDataParser::Pointer parser = parserIter.value();
    IDataArray::Pointer data = attrMat->getAttributeArray(name);
    if(!data->isAllocated())
    {
      qDebug() << name << " is NOT allocated";
    }
    parser->setDataArray(data);

    if(data->getName().compare(getSelectedTimeArrayName()) == 0)
    {
      timeValuePtr = std::dynamic_pointer_cast<FloatArrayType>(data);
    }
    else if(data->getName().compare(getSelectedTimeStepArrayName()) == 0)
    {
      timeStepPtr = std::dynamic_pointer_cast<Int32ArrayType>(data);
    }
    else if(data->getName().compare(getSelectedPointNumArrayName()) == 0)
    {
      pointNumPtr = std::dynamic_pointer_cast<Int32ArrayType>(data);
    }
    else if(data->getName().compare(getSelectedXCoordArrayName()) == 0)
    {
      xCoordsPtr = std::dynamic_pointer_cast<FloatArrayType>(data);
    }
    else if(data->getName().compare(getSelectedYCoordArrayName()) == 0)
    {
      yCoordsPtr = std::dynamic_pointer_cast<FloatArrayType>(data);
    }
  }

  for(int nodeIdx = 0; nodeIdx < m_NumPoints; ++nodeIdx)
  {
    QVector<QByteArray> tokens = tokenizeNodeBlock(reader);
    parseDataTokens(tokens, nodeIdx);
  }

  // Assign Vertices for this time step
  vertices->resizeVertexList(m_NumPoints);
  float* vertex = vertices->getVertexPointer(0);

  for(int v = 0; v < m_NumPoints; ++v)
  {
    vertex[3 * v] = xCoordsPtr->getValue(v);
    vertex[3 * v + 1] = yCoordsPtr->getValue(v);
    vertex[3 * v + 2] = 0.0f; // The Z Coord is not laid out in the file
  }

  // Remove the Coordinates arrays from the AttributeMatrix
  attrMat->removeAttributeArray(xCoordsPtr->getName());
  attrMat->removeAttributeArray(yCoordsPtr->getName());

  // Remove the Point # Array from the AttrMat
  attrMat->removeAttributeArray(pointNumPtr->getName());

  // Generate the AttributeMatrix that will serve as the Meta-Data information for the DataContainerBundle
  AttributeMatrix::Pointer tsbAttrMat = v->getAttributeMatrix(m_BundleMetaDataAMName);
  if(getErrorCondition() < 0)
  {
    return;
  }

  // Remove the Arrays from the Vertex AttributeMatrix. We still have references to the DataArray objects so they will not get deleted.
  attrMat->removeAttributeArray(timeValuePtr->getName());
  attrMat->removeAttributeArray(timeStepPtr->getName());

  // Resize the arrays to 1 value. This will KEEP the first value of the array
  timeValuePtr->resize(1);
  timeStepPtr->resize(1);

  //
  tsbAttrMat->addAttributeArray(timeValuePtr->getName(), timeValuePtr);
  tsbAttrMat->addAttributeArray(timeStepPtr->getName(), timeStepPtr);

  QVector<size_t> cDims(1, 1);
  Int32ArrayType::Pointer timeIndexArray = Int32ArrayType::CreateArray(1, cDims, READ_DEF_PT_TRACKING_TIME_INDEX, true);
  timeIndexArray->setValue(0, t);
  tsbAttrMat->addAttributeArray(timeIndexArray->getName(), timeIndexArray);

  IDataContainerBundle::Pointer bundle = getDataContainerArray()->getDataContainerBundle(getTimeSeriesBundleName());
  if(nullptr != bundle.get())
  {
    bundle->addDataContainer(getDataContainerArray()->getDataContainer(dcName));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void ImportFEAData::parseDataTokens(QVector<QByteArray>& tokens, qint32 nodeIdx)
{
  QMapIterator<QString, SimulationIO::DeformDataParser::Pointer> parserIter(m_NamePointerMap);
  while(parserIter.hasNext())
  {
    parserIter.next();
    QString name = parserIter.key();
    SimulationIO::DeformDataParser::Pointer parser = parserIter.value();
    parser->parse(tokens.at(parser->getColumnIndex()), nodeIdx);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

QVector<QByteArray> ImportFEAData::tokenizeNodeBlock(QFile& reader)
{
  QVector<QByteArray> values;

  reader.readLine(); // Read the first blank line
  for(int l = 0; l < m_LinesPerBlock - 1; ++l)
  {
    QList<QByteArray> tokens = reader.readLine().trimmed().simplified().split(' ');
    QListIterator<QByteArray> iter(tokens);
    while(iter.hasNext())
    {
      values.push_back(iter.next());
    }
  }
  return values;
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

