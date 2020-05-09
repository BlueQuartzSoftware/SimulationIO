/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "ExportLAMMPSFile.h"

#include <QtCore/QTextStream>

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/Utilities/FileSystemPathHelper.h"
#include "SIMPLib/Utilities/SIMPLibEndian.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "SimulationIO/SimulationIOConstants.h"
#include "SimulationIO/SimulationIOVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExportLAMMPSFile::ExportLAMMPSFile()
: m_LammpsFile("")
, m_AtomFeatureLabelsPath(SIMPL::Defaults::VertexDataContainerName, SIMPL::Defaults::VertexAttributeMatrixName, SIMPL::VertexData::AtomFeatureLabels)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExportLAMMPSFile::~ExportLAMMPSFile() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportLAMMPSFile::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("LAMMPS File", LammpsFile, FilterParameter::Parameter, ExportLAMMPSFile));

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Vertex, IGeometry::Type::Vertex);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Atom Feature Labels", AtomFeatureLabelsPath, FilterParameter::RequiredArray, ExportLAMMPSFile, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportLAMMPSFile::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setLammpsFile(reader->readString("LammpsFile", getLammpsFile()));
  setAtomFeatureLabelsPath(reader->readDataArrayPath("AtomFeatureLabelsPath", getAtomFeatureLabelsPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportLAMMPSFile::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportLAMMPSFile::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  FileSystemPathHelper::CheckOutputFile(this, "Output LAMMPS File", getLammpsFile(), true);

  getDataContainerArray()->getPrereqGeometryFromDataContainer<VertexGeom>(this, getAtomFeatureLabelsPath().getDataContainerName());

  DataContainer::Pointer v = getDataContainerArray()->getDataContainer(getAtomFeatureLabelsPath().getDataContainerName());
  if(nullptr == v.get())
  {
    setErrorCondition(-38401, "DataContainer not found");
    return;
  }
  VertexGeom::Pointer vertices = v->getPrereqGeometry<VertexGeom>(this);
  if(getErrorCode() < 0)
  {
    return;
  }

  // We MUST have Nodes
  if(nullptr == vertices->getVertices().get())
  {
    setErrorCondition(-38400, "VertexDataContainer missing Nodes");
  }

  QVector<DataArrayPath> dataArrayPaths;

  std::vector<size_t> cDims(1, 1);

  m_AtomFeatureLabelsPtr =
      getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>>(this, getAtomFeatureLabelsPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AtomFeatureLabelsPtr.lock())                                                                      /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AtomFeatureLabels = m_AtomFeatureLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getAtomFeatureLabelsPath());
  }

  getDataContainerArray()->validateNumberOfTuples(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportLAMMPSFile::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer v = getDataContainerArray()->getDataContainer(getAtomFeatureLabelsPath().getDataContainerName());
  VertexGeom::Pointer vertices = v->getGeometryAs<VertexGeom>();
  int64_t numAtoms = vertices->getNumberOfVertices();

  // find total number of Atom Types
  int32_t numAtomTypes = 0;
  for(int32_t i = 0; i < numAtoms; i++) // find number of grainIds
  {
    if(m_AtomFeatureLabels[i] > numAtomTypes)
    {
      numAtomTypes = m_AtomFeatureLabels[i];
    }
  }

  FILE* lammpsFile = nullptr;
  lammpsFile = fopen(m_LammpsFile.toLatin1().data(), "wb");
  if(nullptr == lammpsFile)
  {
    QString ss = QObject::tr(": Error creating LAMMPS output file '%1'").arg(getLammpsFile());
    setErrorCondition(-11000, ss);
    return;
  }

  float xMin = 1000000000.0;
  float xMax = 0.0;
  float yMin = 1000000000.0;
  float yMax = 0.0;
  float zMin = 1000000000.0;
  float zMax = 0.0;
  int dummy = 0;
  float pos[3] = {0.0f, 0.0f, 0.0f};

  for(int64_t i = 0; i < numAtoms; i++)
  {
    vertices->getCoords(i, pos);
    if(pos[0] < xMin)
    {
      xMin = pos[0];
    }
    if(pos[0] > xMax)
    {
      xMax = pos[0];
    }
    if(pos[1] < yMin)
    {
      yMin = pos[1];
    }
    if(pos[1] > yMax)
    {
      yMax = pos[1];
    }
    if(pos[2] < zMin)
    {
      zMin = pos[2];
    }
    if(pos[2] > zMax)
    {
      zMax = pos[2];
    }
  }

  fprintf(lammpsFile, "LAMMPS data file\n");
  fprintf(lammpsFile, "\n");
  fprintf(lammpsFile, "%lld atoms\n", (long long int)(numAtoms));
  fprintf(lammpsFile, "\n");
  fprintf(lammpsFile, "%lld atom types\n", (long long int)(numAtomTypes));
  fprintf(lammpsFile, "\n");
  fprintf(lammpsFile, "%f %f xlo xhi\n", xMin, xMax);
  fprintf(lammpsFile, "%f %f ylo yhi\n", yMin, yMax);
  fprintf(lammpsFile, "%f %f zlo zhi\n", zMin, zMax);
  fprintf(lammpsFile, "\n");
  fprintf(lammpsFile, "Atoms\n");
  fprintf(lammpsFile, "\n");

  // Write the Atom positions (Vertices)
  for(int64_t i = 0; i < numAtoms; i++)
  {
    vertices->getCoords(i, pos);
    fprintf(lammpsFile, "%lld %d %f %f %f %d %d %d\n", (long long int)(i + 1), m_AtomFeatureLabels[i], pos[0], pos[1], pos[2], dummy, dummy, dummy); // Write the positions to the output file
  }

  fprintf(lammpsFile, "\n");

  // Close the input and output files
  fclose(lammpsFile);

  clearErrorCode();
  clearWarningCode();
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ExportLAMMPSFile::newFilterInstance(bool copyFilterParameters) const
{
  ExportLAMMPSFile::Pointer filter = ExportLAMMPSFile::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExportLAMMPSFile::getCompiledLibraryName() const
{
  return SimulationIOConstants::SimulationIOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExportLAMMPSFile::getBrandingString() const
{
  return "SimulationIO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExportLAMMPSFile::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SimulationIO::Version::Major() << "." << SimulationIO::Version::Minor() << "." << SimulationIO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExportLAMMPSFile::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ExportLAMMPSFile::getUuid() const
{
  return QUuid("{33c10889-4cdc-5992-ae00-1795e9bee022}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExportLAMMPSFile::getSubGroupName() const
{
  return "SimulationIO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExportLAMMPSFile::getHumanLabel() const
{
  return "Export LAMMPS Data File";
}

// -----------------------------------------------------------------------------
ExportLAMMPSFile::Pointer ExportLAMMPSFile::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ExportLAMMPSFile> ExportLAMMPSFile::New()
{
  struct make_shared_enabler : public ExportLAMMPSFile
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ExportLAMMPSFile::getNameOfClass() const
{
  return QString("ExportLAMMPSFile");
}

// -----------------------------------------------------------------------------
QString ExportLAMMPSFile::ClassName()
{
  return QString("ExportLAMMPSFile");
}

// -----------------------------------------------------------------------------
void ExportLAMMPSFile::setLammpsFile(const QString& value)
{
  m_LammpsFile = value;
}

// -----------------------------------------------------------------------------
QString ExportLAMMPSFile::getLammpsFile() const
{
  return m_LammpsFile;
}

// -----------------------------------------------------------------------------
void ExportLAMMPSFile::setAtomFeatureLabelsPath(const DataArrayPath& value)
{
  m_AtomFeatureLabelsPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ExportLAMMPSFile::getAtomFeatureLabelsPath() const
{
  return m_AtomFeatureLabelsPath;
}
