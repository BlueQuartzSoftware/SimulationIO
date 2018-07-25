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
                               "ElementSet"}; //ABAQUS
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
  fprintf(f, "step = %s\n",step.toLatin1().data());
  fprintf(f, "instanceName = %s\n",instanceName.toLatin1().data());

  fprintf(f, "fileName = odbFilePath + odbName + '.odb'\n"); 
  fprintf(f, "odb = openOdb(path = fileName)\n");   
  fprintf(f, "I1 = odb.rootAssembly.instances[instanceName].elementSets[elSet]\n"); 
  fprintf(f, "fieldOut = odb.steps[step].frames[frameNum].fieldOutputs[outputVar].getSubset(region=I1).values\n"); 

  notifyStatusMessage(getHumanLabel(), "Writing ABAQUS python script");
  fclose(f);
  return err;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
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
  return "SimulationIO";
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

