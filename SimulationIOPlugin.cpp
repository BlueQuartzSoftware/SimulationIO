/*
 * Your License or Copyright can go here
 */

#include "SimulationIOPlugin.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/IFilterFactory.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"

#include "SimulationIO/SimulationIOConstants.h"
#include "SimulationIO/SimulationIOVersion.h"

// Include the MOC generated file for this class
#include "moc_SimulationIOPlugin.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SimulationIOPlugin::SimulationIOPlugin() :
  m_Version(SimulationIO::Version::Package()),                            
  m_CompatibilityVersion(SimulationIO::Version::Package()), 
  m_Vendor("Open-Source"),
  m_URL("http://www.github.com/dream3d/SimulationIO"),
  m_Location(""),                                 
  m_Description("Description"),                   
  m_Copyright(""),                               
  //m_Version("0.1.0"),                            // Initialize SimulationIO's Version Number Here
  //m_CompatibilityVersion("0.1.0"), // Initialize SimulationIO's Compatibility Version Number Here
  //m_Vendor("Vendor Name"),                                // Initialize SimulationIO's Vendor Name Here
  //m_URL("URL"),                                           // Initialize Company URL Here
  //m_Location("Location"),                                 // Initialize SimulationIO library Location Here
  //m_Description("Description"),                           // Initialize SimulationIO's Description Here
  //m_Copyright("Copyright"),                               // Initialize SimulationIO's Copyright Here
m_Filters(QList<QString>()),                        // Initialize SimulationIO's List of Dependencies Here
m_DidLoad(false)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SimulationIOPlugin::~SimulationIOPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SimulationIOPlugin::getPluginName()
{
  return (SimulationIOConstants::SimulationIOPluginDisplayName);
}

// -----------------------------------------------------------------------------
QString SimulationIOPlugin::getPluginFileName()
{
  return SimulationIOConstants::SimulationIOPluginFile;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SimulationIOPlugin::getPluginDisplayName()
{
  return SimulationIOConstants::SimulationIOPluginDisplayName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SimulationIOPlugin::getPluginBaseName()
{
  return SimulationIOConstants::SimulationIOBaseName;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SimulationIOPlugin::getVersion()
{
  return m_Version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SimulationIOPlugin::getCompatibilityVersion()
{
  return m_CompatibilityVersion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SimulationIOPlugin::getVendor()
{
  return m_Vendor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SimulationIOPlugin::getURL()
{
  return m_URL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SimulationIOPlugin::getLocation()
{
  return m_Location;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SimulationIOPlugin::getDescription()
{
  /* PLEASE UPDATE YOUR PLUGIN'S DESCRIPTION FILE.
  It is located at SimulationIO/Resources/SimulationIO/SimulationIODescription.txt */

  QFile licenseFile(":/SimulationIO/SimulationIODescription.txt");
  QFileInfo licenseFileInfo(licenseFile);
  QString text = "<<--Description was not read-->>";

  if ( licenseFileInfo.exists() )
  {
    if ( licenseFile.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
      QTextStream in(&licenseFile);
      text = in.readAll();
    }
  }
  return text;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SimulationIOPlugin::getCopyright()
{
  return m_Copyright;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SimulationIOPlugin::getLicense()
{
  /* PLEASE UPDATE YOUR PLUGIN'S LICENSE FILE.
  It is located at SimulationIO/Resources/SimulationIO/SimulationIOLicense.txt */

  QFile licenseFile(":/SimulationIO/SimulationIOLicense.txt");
  QFileInfo licenseFileInfo(licenseFile);
  QString text = "<<--License was not read-->>";

  if ( licenseFileInfo.exists() )
  {
    if ( licenseFile.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
      QTextStream in(&licenseFile);
      text = in.readAll();
    }
  }
  return text;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QString, QString> SimulationIOPlugin::getThirdPartyLicenses()
{
  QMap<QString, QString> licenseMap;
  QList<QString> fileStrList;
  fileStrList.push_back(":/ThirdParty/HDF5.txt");
  fileStrList.push_back(":/ThirdParty/Boost.txt");
  fileStrList.push_back(":/ThirdParty/Qt.txt");
  fileStrList.push_back(":/ThirdParty/Qwt.txt");

  for (QList<QString>::iterator iter = fileStrList.begin(); iter != fileStrList.end(); iter++)
  {
    QFile file(*iter);
    QFileInfo licenseFileInfo(file);

    if ( licenseFileInfo.exists() )
    {
      if ( file.open(QIODevice::ReadOnly | QIODevice::Text) )
      {
        QTextStream in(&file);
        licenseMap.insert(licenseFileInfo.baseName(), in.readAll());
      }
    }
  }

  return licenseMap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SimulationIOPlugin::getDidLoad()
{
  return m_DidLoad;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SimulationIOPlugin::setDidLoad(bool didLoad)
{
  m_DidLoad = didLoad;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SimulationIOPlugin::setLocation(QString filePath)
{
  m_Location = filePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SimulationIOPlugin::writeSettings(QSettings& prefs)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SimulationIOPlugin::readSettings(QSettings& prefs)
{
}

#include "SimulationIOFilters/RegisterKnownFilters.cpp"

#include "FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"

