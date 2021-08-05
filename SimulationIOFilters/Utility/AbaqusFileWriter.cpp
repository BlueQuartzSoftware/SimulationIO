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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "AbaqusFileWriter.h"

#include <iomanip>

#include <QtCore/QDir>
#include <QtCore/QTextStream>

#include "SIMPLib/Math/SIMPLibMath.h"

#include "SimulationIO/SimulationIOFilters/Utility/EntriesHelper.h"

bool AbaqusFileWriter::write(AbstractFilter* filter, const ImageGeom& imageGeom, const DataArray<int32_t>& featureIds, const DataArray<int32_t>& cellPhases, const DataArray<float>& cellEulerAngles,
                             const DynamicTableData& matConst, const QString& outputPath, const QString& filePrefix, const QString& jobName, int32_t numDepvar, int32_t numUserOutVar)
{
  SizeVec3Type dims = imageGeom.getDimensions();
  FloatVec3Type spacing = imageGeom.getSpacing();
  FloatVec3Type origin = imageGeom.getOrigin();

  auto featureIdsData = featureIds.getPointer(0);
  auto cellPhasesData = cellPhases.getPointer(0);
  auto cellEulerAnglesData = cellEulerAngles.getPointer(0);

  // Find total number of grain ids

  auto maxElement = std::max_element(featureIds.begin(), featureIds.end());

  if(maxElement == featureIds.end())
  {
    return false;
  }

  int32_t maxGrainId = *maxElement;

  // Create files

  QString nodesFileName = filePrefix + "_nodes.inp";
  QString elemsFileName = filePrefix + "_elems.inp";
  QString sectsFileName = filePrefix + "_sects.inp";
  QString elsetFileName = filePrefix + "_elset.inp";
  QString masterFileName = filePrefix + ".inp";

  QString nodesFilePath = outputPath + QDir::separator() + nodesFileName;
  QString elemsFilePath = outputPath + QDir::separator() + elemsFileName;
  QString sectsFilePath = outputPath + QDir::separator() + sectsFileName;
  QString elsetFilePath = outputPath + QDir::separator() + elsetFileName;
  QString masterFilePath = outputPath + QDir::separator() + masterFileName;

  QFile nodesFile(nodesFilePath);

  if(!nodesFile.open(QIODevice::OpenModeFlag::WriteOnly))
  {
    QString ss = QObject::tr("Error opening ABAQUS nodes file '%1'").arg(nodesFilePath);
    if(filter != nullptr)
    {
      filter->setErrorCondition(-10220, ss);
    }
    return false;
  }

  int32_t ne_x = dims[0];
  int32_t ne_y = dims[1];
  int32_t ne_z = dims[2];

  int32_t nnode_x = ne_x + 1;
  int32_t nnode_y = ne_y + 1;
  int32_t nnode_z = ne_z + 1;

  QTextStream nodesStream(&nodesFile);

  nodesStream << "*NODE, NSET=ALLNODES\n";

  nodesStream.setRealNumberNotation(QTextStream::RealNumberNotation::FixedNotation);
  nodesStream.setRealNumberPrecision(3);

  for(int32_t k = 0; k < nnode_z; k++)
  {
    if(filter != nullptr)
    {
      if(filter->getCancel())
      {
        return false;
      }
      QString ss = QObject::tr("File [1/5] Writing Z=%1/%2 Layer Nodes").arg(k).arg(nnode_z);
      filter->notifyStatusMessage(ss);
    }
    std::stringstream ss;

    for(int32_t j = 0; j < nnode_y; j++)
    {
      for(int32_t i = 0; i < nnode_x; i++)
      {
        int32_t index = k * nnode_x * nnode_y + j * nnode_x + i;

        float value1 = origin[0] + (i * spacing[0]);
        float value2 = origin[1] + (j * spacing[1]);
        float value3 = origin[2] + (k * spacing[2]);

        ss << std::fixed << std::setprecision(3) << index + 1 << ", " << value1 << ", " << value2 << ", " << value3 << "\n";
      }
    }
    nodesStream << QString::fromStdString(ss.str());
  }

  // notifyStatusMessage("Finished Writing ABAQUS Nodes File");
  QFile elemsFile(elemsFilePath);

  if(!elemsFile.open(QIODevice::OpenModeFlag::WriteOnly))
  {
    QString ss = QObject::tr("Error opening ABAQUS connectivity file '%1'").arg(elemsFilePath);
    if(filter != nullptr)
    {
      filter->setErrorCondition(-10220, ss);
    }
    return false;
  }

  QTextStream elemsStream(&elemsFile);

  elemsStream.setRealNumberNotation(QTextStream::RealNumberNotation::FixedNotation);
  elemsStream.setRealNumberPrecision(3);

  elemsStream << "*ELEMENT, TYPE=C3D8R, ELSET=ALLELEMENTS\n";

  for(int32_t k = 0; k < ne_z; k++)
  {
    if(filter != nullptr)
    {
      if(filter->getCancel())
      {
        return false;
      }
      QString ss = QObject::tr("File [2/5] Writing Z=%1/%2 Layer Elements").arg(k).arg(ne_z);
      filter->notifyStatusMessage(ss);
    }
    std::stringstream ss;
    for(int32_t j = 0; j < ne_y; j++)
    {
      for(int32_t i = 0; i < ne_x; i++)
      {
        int32_t eindex = k * ne_x * ne_y + j * ne_x + i;
        int32_t index = k * nnode_x * nnode_y + j * nnode_x + i + 1;

        int32_t value1 = index;
        int32_t value2 = index + 1;
        int32_t value3 = index + nnode_x + 1;
        int32_t value4 = index + nnode_x;
        int32_t value5 = index + nnode_x * nnode_y;
        int32_t value6 = index + 1 + nnode_x * nnode_y;
        int32_t value7 = index + nnode_x + nnode_x * nnode_y + 1;
        int32_t value8 = index + nnode_x + nnode_x * nnode_y;

        ss << std::fixed << std::setprecision(3) << eindex + 1 << ", " << value1 << ", " << value2 << ", " << value3 << ", " << value4 << ", " << value5 << ", " << value6 << ", " << value7 << ", "
           << value8 << "\n";
      }
    }
    elemsStream << QString::fromStdString(ss.str());
  }

  // notifyStatusMessage("Finished Writing ABAQUS Elements Connectivity File");

  std::vector<int32_t> phaseId(maxGrainId, 0);
  std::vector<float> orient(maxGrainId * 3, 0);

  int32_t totalPoints = imageGeom.getNumberOfElements();

  if(filter != nullptr)
  {
    if(filter->getCancel())
    {
      return false;
    }
    QString ss = QObject::tr("Generating Grain Data from Phase and Orientations...");
    filter->notifyStatusMessage(ss);
  }

  // This function is essentially creating the "Feature Level" or Grain Level mapping of Grain Id to Phase and Euler Angle
  // While this is fine for the Phases Array, the Eulers data may not be completely correct as the last Euler Entry found
  // for each grain will be used instead of something like an "average" orientation.
  // We also loop over all the voxels once instead of looping over all of the voxels for each grain.
  for(int32_t i = 0; i < totalPoints; i++)
  {
    size_t grainId = static_cast<size_t>(featureIdsData[i]);
    if(grainId > 0)
    {
      phaseId[grainId - 1] = cellPhasesData[i];
      orient[(grainId - 1) * 3] = cellEulerAnglesData[i * 3] * 180.0 * SIMPLib::Constants::k_1OverPiD;
      orient[(grainId - 1) * 3 + 1] = cellEulerAnglesData[i * 3 + 1] * 180.0 * SIMPLib::Constants::k_1OverPiD;
      orient[(grainId - 1) * 3 + 2] = cellEulerAnglesData[i * 3 + 2] * 180.0 * SIMPLib::Constants::k_1OverPiD;
    }
  }

  QFile elsetFile(elsetFilePath);

  if(!elsetFile.open(QIODevice::OpenModeFlag::WriteOnly))
  {
    QString ss = QObject::tr("Error opening ABAQUS element file '%1'").arg(elsetFilePath);
    if(filter != nullptr)
    {
      filter->setErrorCondition(-10220, ss);
    }
    return false;
  }
  if(filter != nullptr)
  {
    if(filter->getCancel())
    {
      return false;
    }
    QString ss = QObject::tr("File [3/5] Writing ELSET information to file....");
    filter->notifyStatusMessage(ss);
  }

  for(int32_t voxelId = 1; voxelId <= maxGrainId; voxelId++)
  {
    std::stringstream ss;
    size_t elementPerLine = 0;
    ss << "*Elset, elset=Grain" << voxelId << "_Phase" << phaseId[voxelId - 1] << "_set\n";

    for(int32_t i = 0; i < totalPoints; i++)
    {
      if(featureIdsData[i] == voxelId)
      {
        if(elementPerLine != 0) // no comma at start
        {
          if((elementPerLine % 16) != 0u) // 16 per line
          {
            ss << ", ";
          }
          else
          {
            ss << ",\n";
          }
        }
        ss << static_cast<uint64_t>(i + 1);
        elementPerLine++;
      }
    }
    ss << "\n";
    elsetFile.write(ss.str().c_str(), ss.str().size());
  }

  // notifyStatusMessage("Finished Writing ABAQUS Element Sets File");

  if(filter != nullptr)
  {
    if(filter->getCancel())
    {
      return false;
    }
    QString ss = QObject::tr("File [4/5] Writing Master File.....");
    filter->notifyStatusMessage(ss);
  }

  QFile masterFile(masterFilePath);

  if(!masterFile.open(QIODevice::OpenModeFlag::WriteOnly))
  {
    QString ss = QObject::tr("Error opening ABAQUS input file '%1'").arg(masterFilePath);
    if(filter != nullptr)
    {
      filter->setErrorCondition(-10220, ss);
    }
    return false;
  }

  QTextStream masterStream(&masterFile);

  masterStream << "*Heading\n";
  masterStream << jobName << "\n";
  masterStream << QString("** Job name : %1\n").arg(jobName);
  masterStream << "*Preprint, echo = NO, model = NO, history = NO, contact = NO\n";
  masterStream << "**\n";
  masterStream << QString("*Include, Input = %1\n").arg(nodesFileName);
  masterStream << QString("*Include, Input = %1\n").arg(elemsFileName);
  masterStream << QString("*Include, Input = %1\n").arg(sectsFileName);
  masterStream << QString("*Include, Input = %1\n").arg(elsetFileName);
  masterStream << "**\n";

  masterStream.setRealNumberNotation(QTextStream::RealNumberNotation::FixedNotation);
  masterStream.setRealNumberPrecision(3);

  std::vector<std::vector<double>> MatConst = matConst.getTableData();

  size_t numMatConst = MatConst.size();

  for(int32_t i = 1; i <= maxGrainId; i++)
  {
    masterStream << QString("*Material, name = Grain%1_Phase%2_set\n").arg(i).arg(phaseId[i - 1]);
    masterStream << "*Depvar\n";
    masterStream << numDepvar << "\n";
    masterStream << QString("*User Material, constants = %1\n").arg(numMatConst + 5);
    masterStream << QString("%1, %2, %3, %4, %5").arg(i).arg(phaseId[i - 1]).arg(orient[(i - 1) * 3], 0, 'f', 3).arg(orient[(i - 1) * 3 + 1], 0, 'f', 3).arg(orient[(i - 1) * 3 + 2], 0, 'f', 3);

    auto func = [&MatConst](size_t i) { return QString::number(MatConst[i][0], 'f', 3); };

    EntriesHelper::writeEntries(masterStream, func, numMatConst, 8, ", ", "\n", 5);

    masterStream << "\n";
    masterStream << "*User Output Variables\n";
    masterStream << numUserOutVar << "\n";
  }

  // We are now defining the sections, which is for each grain
  if(filter != nullptr)
  {
    if(filter->getCancel())
    {
      return false;
    }
    QString ss = QObject::tr("File [5/5] Writing Sections File.....");
    filter->notifyStatusMessage(ss);
  }

  QFile sectsFile(sectsFilePath);
  if(!sectsFile.open(QIODevice::OpenModeFlag::WriteOnly))
  {
    QString ss = QObject::tr("Error opening ABAQUS sections file '%1'").arg(sectsFilePath);
    if(filter != nullptr)
    {
      filter->setErrorCondition(-10220, ss);
    }
    return false;
  }
  QTextStream sectsStream(&sectsFile);
  for(int32_t grain = 1; grain <= maxGrainId; grain++)
  {
    sectsStream << QString("*Solid Section, elset=Grain%1_Phase%2_set, material=Grain%3_Phase%4_mat\n").arg(grain).arg(phaseId[grain - 1]).arg(grain).arg(phaseId[grain - 1]);
  }

  // notifyStatusMessage("Finished Writing ABAQUS Sections File");

  return true;
}
