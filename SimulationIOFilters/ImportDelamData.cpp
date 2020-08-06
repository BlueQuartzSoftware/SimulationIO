/*
 * Your License or Copyright can go here
 */

#include "ImportDelamData.h"

#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/RectGridGeom.h"

#include "SimulationIO/SimulationIOConstants.h"
#include "SimulationIO/SimulationIOVersion.h"

enum createdPathID : RenameDataPath::DataID_t
{
  DataContainerID = 1,
  AttributeMatrixID21 = 21,
  DataArrayID30 = 30
};

// -----------------------------------------------------------------------------
ImportDelamData::ImportDelamData()
{
  initialize();
}

// -----------------------------------------------------------------------------
ImportDelamData::~ImportDelamData() = default;

// -----------------------------------------------------------------------------
void ImportDelamData::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
void ImportDelamData::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Bvid File", BvidFile, FilterParameter::Parameter, ImportDelamData, "*.txt"));
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Bvid StdOut File", BvidStdOutFile, FilterParameter::Parameter, ImportDelamData, "*.txt"));
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Interface Thickness", InterfaceThickness, FilterParameter::Parameter, ImportDelamData));

  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Data Container", DataContainerName, FilterParameter::CreatedArray, ImportDelamData));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix", CellAttributeMatrixName, FilterParameter::CreatedArray, ImportDelamData));
  parameters.push_back(SIMPL_NEW_STRING_FP("Data Array", DataArrayName, FilterParameter::CreatedArray, ImportDelamData));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void ImportDelamData::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  QFileInfo fi(m_BvidFile);
  if(m_BvidFile.isEmpty())
  {
    QString ss = QObject::tr("Bvid File is empty.  Please enter a file path.");
    setErrorCondition(-2000, ss);
    return;
  }

  if(!fi.isFile() || fi.suffix() != "txt")
  {
    QString ss = QObject::tr("Bvid File is not a text file.  Please enter the path to a text file.");
    setErrorCondition(-2001, ss);
    return;
  }

  fi.setFile(m_BvidStdOutFile);
  if(m_BvidStdOutFile.isEmpty())
  {
    QString ss = QObject::tr("Bvid StdOut File is empty.  Please enter a file path.");
    setErrorCondition(-2002, ss);
    return;
  }

  if(!fi.isFile() || fi.completeSuffix() != "txt")
  {
    QString ss = QObject::tr("Bvid StdOut File is not a text file.  Please enter the path to a text file.");
    setErrorCondition(-2003, ss);
    return;
  }

  readBvidStdOutFile();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainerShPtr dc = getDataContainerArray()->createNonPrereqDataContainer(this, getDataContainerName(), DataContainerID);

  m_Xcrd = static_cast<int>(m_Lcx / m_Ex) + 1;
  m_Ycrd = m_Xcrd;
  m_Zcrd = 2 * m_CzmLayers + 2;

  float translate_x = m_Ex * static_cast<int>(m_Lcx / m_Ex / 2);
  float translate_y = translate_x;

  FloatArrayType::Pointer xBounds = FloatArrayType::CreateArray(m_Xcrd, SIMPL::Geometry::xBoundsList, true);
  std::generate(xBounds->begin(), xBounds->end(), [n = -translate_x, this]() mutable { return n += m_Ex; });

  FloatArrayType::Pointer yBounds = FloatArrayType::CreateArray(m_Ycrd, SIMPL::Geometry::yBoundsList, true);
  std::generate(yBounds->begin(), yBounds->end(), [n = -translate_y, this]() mutable { return n += m_Ex; });

  FloatArrayType::Pointer zBounds = FloatArrayType::CreateArray(m_Zcrd, SIMPL::Geometry::zBoundsList, true);
  float z_0 = 0.0f;
  float z_1 = (m_TotalThk / m_NumPlies) - m_InterfaceThickness;
  zBounds->setValue(0, z_0);
  zBounds->setValue(1, z_1);

  int currentIdx = 2;
  for(int i = 1; i <= m_NumPlies - 1; i++)
  {
    float z_0 = (i * m_TotalThk / m_NumPlies) + m_InterfaceThickness;
    float z_1 = ((i + 1) * m_TotalThk / m_NumPlies) - m_InterfaceThickness;
    zBounds->setValue(currentIdx++, z_0);
    zBounds->setValue(currentIdx++, z_1);
  }

  RectGridGeom::Pointer geom = RectGridGeom::CreateGeometry(SIMPL::Geometry::RectGridGeometry);
  geom->setDimensions({m_Xcrd - 1, m_Ycrd - 1, m_Zcrd - 1});
  geom->setXBounds(xBounds);
  geom->setYBounds(yBounds);
  geom->setZBounds(zBounds);
  dc->setGeometry(geom);

  size_t totalLines = getBvidFileLineCount();
  if(totalLines == 0)
  {
    return;
  }

  AttributeMatrixShPtr am =
      dc->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), geom->getDimensions().toContainer<std::vector<size_t>>(), AttributeMatrix::Type::Cell, AttributeMatrixID21);
  am->createNonPrereqArray<FloatArrayType>(this, getDataArrayName(), 0, {1}, DataArrayID30);
}

// -----------------------------------------------------------------------------
void ImportDelamData::readBvidStdOutFile()
{
  QFile bvidStdOutFile(m_BvidStdOutFile);
  if(!bvidStdOutFile.open(QFile::ReadOnly))
  {
    QString ss = QObject::tr("Cannot open Bvid StdOut File for reading.");
    setErrorCondition(-2004, ss);
    return;
  }

  QString bvidLine = bvidStdOutFile.readLine();
  while(!bvidLine.isEmpty())
  {
    QStringList tokens = bvidLine.split(' ', QString::SplitBehavior::SkipEmptyParts);
    if(tokens.size() >= 2 && tokens[1] == "0/90ply")
    {
      bool ok = false;
      float ex = tokens.last().toFloat(&ok);
      if(!ok)
      {
        QString ss = QObject::tr("Cannot read Elx value from Bvid StdOut File.");
        setErrorCondition(-2005, ss);
        return;
      }
      m_Ex = ex / 3;
    }
    else if(!tokens.empty() && tokens.first() == "Previous")
    {
      bool ok = false;
      float lcx = tokens.last().toFloat(&ok);
      if(!ok)
      {
        QString ss = QObject::tr("Cannot read Lcx value from Bvid StdOut File.");
        setErrorCondition(-2006, ss);
        return;
      }
      m_Lcx = lcx;
    }
    else if(tokens.size() >= 4 && tokens[3] == "CZM")
    {
      bool ok = false;
      int czmLayers = tokens.last().toInt(&ok);
      if(!ok)
      {
        QString ss = QObject::tr("Cannot read total number of CZM layers from Bvid StdOut File.");
        setErrorCondition(-2007, ss);
        return;
      }
      m_CzmLayers = czmLayers;
    }
    else if(tokens.size() >= 4 && tokens[3] == "plies")
    {
      bool ok = false;
      int num_plies = tokens.last().toInt(&ok);
      if(!ok)
      {
        QString ss = QObject::tr("Cannot read total number of plies from Bvid StdOut File.");
        setErrorCondition(-2008, ss);
        return;
      }
      m_NumPlies = num_plies;
    }
    else if(tokens.size() >= 2 && tokens[1] == "CPT:")
    {
      bool ok = false;
      float total_thk = tokens.last().toFloat(&ok);
      if(!ok)
      {
        QString ss = QObject::tr("Cannot read total CPT from Bvid StdOut File.");
        setErrorCondition(-2009, ss);
        return;
      }
      m_TotalThk = total_thk;
    }

    bvidLine = bvidStdOutFile.readLine();
  }

  bvidStdOutFile.close();
}

// -----------------------------------------------------------------------------
size_t ImportDelamData::getBvidFileLineCount()
{
  QFile bvidFile(m_BvidFile);
  if(!bvidFile.open(QFile::ReadOnly))
  {
    QString ss = QObject::tr("Cannot open Bvid File for reading.");
    setErrorCondition(-2010, ss);
    return 0;
  }

  size_t lineCount = 0;
  while(!bvidFile.readLine().isEmpty())
  {
    lineCount++;
  }

  bvidFile.close();

  return lineCount;
}

// -----------------------------------------------------------------------------
void ImportDelamData::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  readBvidFile();
}

// -----------------------------------------------------------------------------
void ImportDelamData::readBvidFile()
{
  QFile bvidFile(m_BvidFile);
  if(!bvidFile.open(QFile::ReadOnly))
  {
    QString ss = QObject::tr("Cannot open Bvid File for reading.");
    setErrorCondition(-2011, ss);
    return;
  }

  QString bvidLine = bvidFile.readLine();
  size_t lineCount = 1;
  while(!bvidLine.isEmpty())
  {
    QStringList tokens = bvidLine.split(' ', QString::SplitBehavior::SkipEmptyParts);
    if(tokens.size() != 4)
    {
      std::stringstream ss;
      ss << "Bvid File line " << lineCount << ": line does not have 4 values.";
      setErrorCondition(-2012, QString::fromStdString(ss.str()));
      return;
    }

    bool ok = false;
    float xCoord = tokens[0].toFloat(&ok);
    if(!ok)
    {
      std::stringstream ss;
      ss << "Bvid File line " << lineCount << ": Could not convert X value to a float.";
      setErrorCondition(-2013, QString::fromStdString(ss.str()));
      return;
    }

    float yCoord = tokens[1].toFloat(&ok);
    if(!ok)
    {
      std::stringstream ss;
      ss << "Bvid File line " << lineCount << ": Could not convert Y value to a float.";
      setErrorCondition(-2014, QString::fromStdString(ss.str()));
      return;
    }

    float zCoord = tokens[2].toFloat(&ok);
    if(!ok)
    {
      std::stringstream ss;
      ss << "Bvid File line " << lineCount << ": Could not convert Z value to a float.";
      setErrorCondition(-2015, QString::fromStdString(ss.str()));
      return;
    }

    float value = tokens[3].toFloat(&ok);
    if(!ok)
    {
      std::stringstream ss;
      ss << "Bvid File line " << lineCount << ": Could not convert 4th value to a float.";
      setErrorCondition(-2016, QString::fromStdString(ss.str()));
      return;
    }

    DataContainer& dc = *getDataContainerArray()->getDataContainer(getDataContainerName());
    AttributeMatrix& am = *dc.getAttributeMatrix(getCellAttributeMatrixName());
    FloatArrayType& da = *am.getAttributeArrayAs<FloatArrayType>(getDataArrayName());

    RectGridGeom& geom = *dc.getGeometryAs<RectGridGeom>();
    std::optional<size_t> idxOpt = geom.getIndex(xCoord, yCoord, zCoord);
    if(!idxOpt.has_value())
    {
      std::stringstream ss;
      ss << "Bvid File line " << lineCount << ": X,Y,Z coordinate is outside the geometry bounds.";
      setErrorCondition(-2017, QString::fromStdString(ss.str()));
      return;
    }
    size_t idx = *idxOpt;

    // Set Value into Array
    da.setValue(idx, value);

    bvidLine = bvidFile.readLine();
    lineCount++;
  }

  bvidFile.close();
}

// -----------------------------------------------------------------------------
AbstractFilter::Pointer ImportDelamData::newFilterInstance(bool copyFilterParameters) const
{
  ImportDelamData::Pointer filter = ImportDelamData::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
QString ImportDelamData::getCompiledLibraryName() const
{ 
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
QString ImportDelamData::getBrandingString() const
{
  return "SimulationIO";
}

// -----------------------------------------------------------------------------
QString ImportDelamData::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SimulationIO::Version::Major() << "." << SimulationIO::Version::Minor() << "." << SimulationIO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
QString ImportDelamData::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
QString ImportDelamData::getSubGroupName() const
{ 
  return "SimulationIO"; 
}

// -----------------------------------------------------------------------------
QString ImportDelamData::getHumanLabel() const
{
  return "Import Delam Data";
}

// -----------------------------------------------------------------------------
QUuid ImportDelamData::getUuid() const
{
  return QUuid("{f3def19a-b932-5ce7-9796-9e9476a29e1a}");
}

// -----------------------------------------------------------------------------
ImportDelamData::Pointer ImportDelamData::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ImportDelamData> ImportDelamData::New()
{
  struct make_shared_enabler : public ImportDelamData
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ImportDelamData::getNameOfClass() const
{
  return QString("ImportDelamData");
}

// -----------------------------------------------------------------------------
QString ImportDelamData::ClassName()
{
  return QString("ImportDelamData");
}

// -----------------------------------------------------------------------------
void ImportDelamData::setBvidFile(const QString& value)
{
  m_BvidFile = value;
}

// -----------------------------------------------------------------------------
QString ImportDelamData::getBvidFile() const
{
  return m_BvidFile;
}

// -----------------------------------------------------------------------------
void ImportDelamData::setBvidStdOutFile(const QString& value)
{
  m_BvidStdOutFile = value;
}

// -----------------------------------------------------------------------------
QString ImportDelamData::getBvidStdOutFile() const
{
  return m_BvidStdOutFile;
}

// -----------------------------------------------------------------------------
void ImportDelamData::setInterfaceThickness(float value)
{
  m_InterfaceThickness = value;
}

// -----------------------------------------------------------------------------
float ImportDelamData::getInterfaceThickness() const
{
  return m_InterfaceThickness;
}

// -----------------------------------------------------------------------------
void ImportDelamData::setDataContainerName(const DataArrayPath& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ImportDelamData::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void ImportDelamData::setCellAttributeMatrixName(const QString& value)
{
  m_CellAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString ImportDelamData::getCellAttributeMatrixName() const
{
  return m_CellAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void ImportDelamData::setDataArrayName(const QString& value)
{
  m_DataArrayName = value;
}

// -----------------------------------------------------------------------------
QString ImportDelamData::getDataArrayName() const
{
  return m_DataArrayName;
}
