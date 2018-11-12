/*
 * Your License or Copyright can go here
 */

#include "Export3dSolidMesh.h"

#include "SIMPLib/Common/Constants.h"



#include "SimulationIO/SimulationIOConstants.h"
#include "SimulationIO/SimulationIOVersion.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Export3dSolidMesh::Export3dSolidMesh()  
: AbstractFilter()
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

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Export3dSolidMesh::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  
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

  if (getCancel()) { return; }

  if (getWarningCondition() < 0)
  {
    QString ss = QObject::tr("Some warning message");
    setWarningCondition(-88888888);
    notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
  }

  if (getErrorCondition() < 0)
  {
    QString ss = QObject::tr("Some error message");
    setErrorCondition(-99999999);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

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

