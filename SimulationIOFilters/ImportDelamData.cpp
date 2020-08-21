/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The code contained herein was partially funded by the followig contracts:
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ImportDelamData.h"

#include <QtCore/QFileInfo>

#include "SIMPLib/DataArrays/DataArray.hpp"
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

  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("CSDGM File", CSDGMFile, FilterParameter::Parameter, ImportDelamData, "*.txt"));
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Bvid StdOut File", BvidStdOutFile, FilterParameter::Parameter, ImportDelamData, "*.txt"));
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Interface Thickness", InterfaceThickness, FilterParameter::Parameter, ImportDelamData));

  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Data Container", DataContainerPath, FilterParameter::CreatedArray, ImportDelamData));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix", CellAttributeMatrixName, FilterParameter::CreatedArray, ImportDelamData));
  parameters.push_back(SIMPL_NEW_STRING_FP("Data Array", DataArrayName, FilterParameter::CreatedArray, ImportDelamData));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void ImportDelamData::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  QFileInfo fi(m_CSDGMFile);
  if(m_CSDGMFile.isEmpty())
  {
    QString ss = QObject::tr("Bvid File is empty.  Please enter a file path.");
    setErrorCondition(-2000, ss);
    return;
  }

  if(!fi.isFile() || fi.suffix().toLower() != "txt")
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

  if(!fi.isFile() || fi.suffix().toLower() != "txt")
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

  DataContainerShPtr dc = getDataContainerArray()->createNonPrereqDataContainer(this, getDataContainerPath(), DataContainerID);

  m_Xcrd = static_cast<int>(*m_Lcx / *m_Ex) + 1;
  m_Ycrd = m_Xcrd;
  m_Zcrd = 2 * (*m_CzmLayers) + 2;

  float translate_x = *m_Ex * static_cast<int>(*m_Lcx / *m_Ex / 2);
  float translate_y = translate_x;

  FloatArrayType::Pointer xBounds = FloatArrayType::CreateArray(m_Xcrd, SIMPL::Geometry::xBoundsList, true);
  std::generate(xBounds->begin(), xBounds->end(), [n = -translate_x, this]() mutable { return n += *m_Ex; });

  FloatArrayType::Pointer yBounds = FloatArrayType::CreateArray(m_Ycrd, SIMPL::Geometry::yBoundsList, true);
  std::generate(yBounds->begin(), yBounds->end(), [n = -translate_y, this]() mutable { return n += *m_Ex; });

  FloatArrayType::Pointer zBounds = FloatArrayType::CreateArray(m_Zcrd, SIMPL::Geometry::zBoundsList, true);
  float z_0 = 0.0f;
  float z_1 = (*m_TotalThk / *m_NumPlies) - m_InterfaceThickness;
  zBounds->setValue(0, z_0);
  zBounds->setValue(1, z_1);

  int currentIdx = 2;
  for(int i = 1; i <= *m_NumPlies - 1; i++)
  {
    float z_0 = (i * *m_TotalThk / *m_NumPlies) + m_InterfaceThickness;
    float z_1 = ((i + 1) * *m_TotalThk / *m_NumPlies) - m_InterfaceThickness;
    zBounds->setValue(currentIdx++, z_0);
    zBounds->setValue(currentIdx++, z_1);
  }

  RectGridGeom::Pointer geom = RectGridGeom::CreateGeometry(SIMPL::Geometry::RectGridGeometry);
  geom->setDimensions({m_Xcrd - 1, m_Ycrd - 1, m_Zcrd - 1});
  geom->setXBounds(xBounds);
  geom->setYBounds(yBounds);
  geom->setZBounds(zBounds);
  dc->setGeometry(geom);

  std::vector<size_t> dims = geom->getDimensions().toContainer<std::vector<size_t>>();
  AttributeMatrixShPtr am = dc->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), dims, AttributeMatrix::Type::Cell, AttributeMatrixID21);
  am->createNonPrereqArray<FloatArrayType>(this, getDataArrayName(), 0, {1}, DataArrayID30);
}

// -----------------------------------------------------------------------------
void ImportDelamData::readBvidStdOutFile()
{
  m_Lcx.reset();
  m_Ex.reset();
  m_CzmLayers.reset();
  m_NumPlies.reset();
  m_TotalThk.reset();

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
    else if(tokens.size() >= 2 && tokens[1] == "CPT:")
    {
      bool ok = false;
      float total_thk = tokens.last().toFloat(&ok);
      if(!ok)
      {
        QString ss = QObject::tr("Cannot read total CPT from Bvid StdOut File.");
        setErrorCondition(-2007, ss);
        return;
      }
      m_TotalThk = total_thk;
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
    else if(tokens.size() >= 4 && tokens[3] == "CZM")
    {
      bool ok = false;
      int czmLayers = tokens.last().toInt(&ok);
      if(!ok)
      {
        QString ss = QObject::tr("Cannot read total number of CZM layers from Bvid StdOut File.");
        setErrorCondition(-2009, ss);
        return;
      }
      m_CzmLayers = czmLayers;
    }

    bvidLine = bvidStdOutFile.readLine();
  }

  if(!m_Ex.has_value())
  {
    QString ss = QObject::tr("Cannot find Elx value from Bvid StdOut File.");
    setErrorCondition(-2010, ss);
    return;
  }
  if(!m_Lcx.has_value())
  {
    QString ss = QObject::tr("Cannot find Lcx value from Bvid StdOut File.");
    setErrorCondition(-2011, ss);
    return;
  }
  if(!m_TotalThk.has_value())
  {
    QString ss = QObject::tr("Cannot find total CPT from Bvid StdOut File.");
    setErrorCondition(-2012, ss);
    return;
  }
  if(!m_NumPlies.has_value())
  {
    QString ss = QObject::tr("Cannot find total number of plies from Bvid StdOut File.");
    setErrorCondition(-2013, ss);
    return;
  }
  if(!m_CzmLayers.has_value())
  {
    QString ss = QObject::tr("Cannot find total number of CZM layers from Bvid StdOut File.");
    setErrorCondition(-2014, ss);
    return;
  }

  bvidStdOutFile.close();
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

  readCSDGMFile();
}

// -----------------------------------------------------------------------------
void ImportDelamData::readCSDGMFile()
{
  QFile csdgmFile(m_CSDGMFile);
  if(!csdgmFile.open(QFile::ReadOnly))
  {
    QString ss = QObject::tr("Cannot open Bvid File for reading.");
    setErrorCondition(-2016, ss);
    return;
  }

  QString csdgmLine = csdgmFile.readLine();
  size_t lineNum = 1;
  while(!csdgmLine.isEmpty())
  {
    QStringList tokens = csdgmLine.split(' ', QString::SplitBehavior::SkipEmptyParts);
    if(tokens.size() != 4)
    {
      std::stringstream ss;
      ss << "CSDGM File line " << lineNum << ": line does not have 4 values.";
      setErrorCondition(-2017, QString::fromStdString(ss.str()));
      return;
    }

    bool ok = false;
    float xCoord = tokens[0].toFloat(&ok);
    if(!ok)
    {
      std::stringstream ss;
      ss << "CSDGM File line " << lineNum << ": Could not convert X value to a float.";
      setErrorCondition(-2018, QString::fromStdString(ss.str()));
      return;
    }

    float yCoord = tokens[1].toFloat(&ok);
    if(!ok)
    {
      std::stringstream ss;
      ss << "CSDGM File line " << lineNum << ": Could not convert Y value to a float.";
      setErrorCondition(-2019, QString::fromStdString(ss.str()));
      return;
    }

    float zCoord = tokens[2].toFloat(&ok);
    if(!ok)
    {
      std::stringstream ss;
      ss << "CSDGM File line " << lineNum << ": Could not convert Z value to a float.";
      setErrorCondition(-2020, QString::fromStdString(ss.str()));
      return;
    }

    float value = tokens[3].toFloat(&ok);
    if(!ok)
    {
      std::stringstream ss;
      ss << "CSDGM File line " << lineNum << ": Could not convert 4th value to a float.";
      setErrorCondition(-2021, QString::fromStdString(ss.str()));
      return;
    }

    DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(getDataContainerPath());
    if(dc == nullptr)
    {
      std::stringstream ss;
      std::string dcName = getDataContainerPath().getDataContainerName().toStdString();
      ss << "Could not get data container '" << dcName << "' from data container array.";
      setErrorCondition(-2022, QString::fromStdString(ss.str()));
      return;
    }
    AttributeMatrix::Pointer am = dc->getAttributeMatrix(getCellAttributeMatrixName());
    if(am == nullptr)
    {
      std::stringstream ss;
      std::string amName = getCellAttributeMatrixName().toStdString();
      ss << "Could not get attribute matrix '" << amName << "' from data container array.";
      setErrorCondition(-2023, QString::fromStdString(ss.str()));
      return;
    }
    FloatArrayType::Pointer da = am->getAttributeArrayAs<FloatArrayType>(getDataArrayName());
    if(da == nullptr)
    {
      std::stringstream ss;
      std::string daName = getDataArrayName().toStdString();
      ss << "Could not get data array '" << daName << "' from data container array.";
      setErrorCondition(-2024, QString::fromStdString(ss.str()));
      return;
    }

    RectGridGeom::Pointer geom = dc->getGeometryAs<RectGridGeom>();
    if(geom == nullptr)
    {
      std::stringstream ss;
      std::string dcName = getDataContainerPath().getDataContainerName().toStdString();
      ss << "Could not get rectilinear grid geometry from data container '" << dcName << "'.";
      setErrorCondition(-2025, QString::fromStdString(ss.str()));
      return;
    }
    std::optional<size_t> idxOpt = geom->getIndex(xCoord, yCoord, zCoord);
    if(!idxOpt.has_value())
    {
      std::stringstream ss;
      ss << "CSDGM File line " << lineNum << ": X,Y,Z coordinate is outside the geometry bounds.";
      setErrorCondition(-2026, QString::fromStdString(ss.str()));
      return;
    }
    size_t idx = *idxOpt;

    // Set Value into Array
    da->setValue(idx, value);

    csdgmLine = csdgmFile.readLine();
    lineNum++;
  }

  csdgmFile.close();
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
void ImportDelamData::setCSDGMFile(const QString& value)
{
  m_CSDGMFile = value;
}

// -----------------------------------------------------------------------------
QString ImportDelamData::getCSDGMFile() const
{
  return m_CSDGMFile;
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
void ImportDelamData::setDataContainerPath(const DataArrayPath& value)
{
  m_DataContainerPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ImportDelamData::getDataContainerPath() const
{
  return m_DataContainerPath;
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
