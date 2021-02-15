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

#include "CreateBsamFile.h"

#include <QtCore/QDir>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#include "SimulationIO/SimulationIOConstants.h"
#include "SimulationIO/SimulationIOVersion.h"

#include "SimulationIO/SimulationIOFilters/Utility/BsamFileWriter.h"

// -----------------------------------------------------------------------------
CreateBsamFile::CreateBsamFile()
: m_NumClusters(1)
{
  initialize();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
CreateBsamFile::~CreateBsamFile() = default;

// -----------------------------------------------------------------------------
void CreateBsamFile::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
void CreateBsamFile::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_OUTPUT_PATH_FP("Output Path ", OutputPath, FilterParameter::Category::Parameter, CreateBsamFile, "*", "*"));
  parameters.push_back(SIMPL_NEW_STRING_FP("Output File Prefix", OutputFilePrefix, FilterParameter::Category::Parameter, CreateBsamFile));

  parameters.push_back(SIMPL_NEW_INTEGER_FP("Number of Clusters", NumClusters, FilterParameter::Category::Parameter, CreateBsamFile));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void CreateBsamFile::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOutputPath(reader->readString("OutputPath", getOutputPath()));
  setOutputFilePrefix(reader->readString("OutputFilePrefix", getOutputFilePrefix()));
  setNumClusters(reader->readValue("NumClusters", getNumClusters()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
void CreateBsamFile::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  if(m_OutputPath.isEmpty())
  {
    QString ss = QObject::tr("The output path must be set");
    setErrorCondition(-12001, ss);
  }

  QDir dir(m_OutputPath);
  if(!dir.exists())
  {
    QString ss = QObject::tr("The directory path for the output file does not exist. DREAM.3D will attempt to create this path during execution of the filter");
    setWarningCondition(-10300, ss);
  }
}

// -----------------------------------------------------------------------------
void CreateBsamFile::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  if(getCancel())
  {
    return;
  }

  QDir dir;
  if(!dir.mkpath(m_OutputPath))
  {
    QString ss = QObject::tr("Error creating path '%1'").arg(m_OutputPath);
    setErrorCondition(-10301, ss);
    return;
  }

  if(!BsamFileWriter::write(m_OutputPath, m_OutputFilePrefix, m_NumClusters))
  {
    QString ss = QObject::tr("Error writing file at '%1'").arg(m_OutputPath);
    setErrorCondition(-10302, ss);
    return;
  }
}

// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateBsamFile::newFilterInstance(bool copyFilterParameters) const
{
  CreateBsamFile::Pointer filter = CreateBsamFile::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
QString CreateBsamFile::getCompiledLibraryName() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
QString CreateBsamFile::getBrandingString() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
QString CreateBsamFile::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SimulationIO::Version::Major() << "." << SimulationIO::Version::Minor() << "." << SimulationIO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
QString CreateBsamFile::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
QString CreateBsamFile::getSubGroupName() const
{
  return "SimulationIO";
}

// -----------------------------------------------------------------------------
QString CreateBsamFile::getHumanLabel() const
{
  return "Create BSAM File";
}

// -----------------------------------------------------------------------------
QUuid CreateBsamFile::getUuid() const
{
  return QUuid("{542c2417-1685-54e5-9322-f60792e9176c}");
}

// -----------------------------------------------------------------------------
CreateBsamFile::Pointer CreateBsamFile::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
CreateBsamFile::Pointer CreateBsamFile::New()
{
  struct make_shared_enabler : public CreateBsamFile
  {
  };
  return std::make_shared<make_shared_enabler>();
}

// -----------------------------------------------------------------------------
QString CreateBsamFile::getNameOfClass() const
{
  return ClassName();
}

// -----------------------------------------------------------------------------
QString CreateBsamFile::ClassName()
{
  return QString("CreateBsamFile");
}

// -----------------------------------------------------------------------------
void CreateBsamFile::setOutputPath(const QString& value)
{
  m_OutputPath = value;
}

// -----------------------------------------------------------------------------
QString CreateBsamFile::getOutputPath() const
{
  return m_OutputPath;
}

// -----------------------------------------------------------------------------
void CreateBsamFile::setOutputFilePrefix(const QString& value)
{
  m_OutputFilePrefix = value;
}

// -----------------------------------------------------------------------------
QString CreateBsamFile::getOutputFilePrefix() const
{
  return m_OutputFilePrefix;
}

// -----------------------------------------------------------------------------
void CreateBsamFile::setNumClusters(int value)
{
  m_NumClusters = value;
}

// -----------------------------------------------------------------------------
int CreateBsamFile::getNumClusters() const
{
  return m_NumClusters;
}
