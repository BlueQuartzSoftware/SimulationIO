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

#pragma once

#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include "SIMPLib/Common/SIMPLArray.hpp"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "SimulationIO/SimulationIOFilters/Utility/EntriesHelper.h"

namespace OnScaleTableFileWriter
{
void writeCoords(QTextStream& stream, const QString& text, size_t maxIndex, float origin, float spacing);

/**
 * @brief Writes a OnScale table file. All elements of featureIds must be >=0. Returns true on success.
 * @param imageGeom
 * @param phaseNames
 * @param featureIds
 * @param outputPath
 * @param filePrefix
 * @param numKeypoints
 * @return
 */
template <class T>
std::pair<int, QString> write(const ImageGeom& imageGeom, const StringDataArray& phaseNames, const DataArray<T>& featureIds, const QString& outputPath, const QString& filePrefix,
                              const IntVec3Type& numKeypoints)
{
  SizeVec3Type dims = imageGeom.getDimensions();
  FloatVec3Type spacing = imageGeom.getSpacing();
  FloatVec3Type origin = imageGeom.getOrigin();

  size_t numTuples = phaseNames.getNumberOfTuples();

  // Find total number of grain ids

  auto maxElement = std::max_element(featureIds.begin(), featureIds.end());

  if(maxElement == featureIds.end())
  {
    QString ss = QObject::tr("Could not find max feature id");
    return {-1, ss};
  }

  size_t maxGrainId = static_cast<size_t>(*maxElement);

  size_t totalPoints = imageGeom.getNumberOfElements();

  QString masterFilePath = outputPath + QDir::separator() + filePrefix + ".flxtbl";

  QFile masterFile(masterFilePath);

  if(!masterFile.open(QIODevice::OpenModeFlag::WriteOnly))
  {
    QString ss = QObject::tr("Error writing PZFLEX input file '%1'").arg(masterFilePath);
    return {-2, ss};
  }

  QTextStream masterStream(&masterFile);

  masterStream.setRealNumberNotation(QTextStream::RealNumberNotation::FixedNotation);
  masterStream.setRealNumberPrecision(3);

  masterStream << "hedr 0\n";
  masterStream << "info 1\n";

  size_t ne_x = dims[0];
  size_t ne_y = dims[1];
  size_t ne_z = dims[2];

  size_t nnode_x = ne_x + 1;
  size_t nnode_y = ne_y + 1;
  size_t nnode_z = ne_z + 1;

  writeCoords(masterStream, "xcrd", nnode_x, origin[0], spacing[0]);
  writeCoords(masterStream, "ycrd", nnode_y, origin[1], spacing[1]);
  writeCoords(masterStream, "zcrd", nnode_z, origin[2], spacing[2]);

  masterStream << "keypoints\n";
  masterStream << QString("%1 %2 %3\n").arg(numKeypoints[0]).arg(numKeypoints[1]).arg(numKeypoints[2]);

  masterStream << "divisions\n";
  masterStream << QString("%1 %2 %3\n").arg(ne_x).arg(ne_y).arg(ne_z);

  masterStream << QString("name %1\n").arg(maxGrainId);

  if(numTuples - 1 >= maxGrainId)
  {
    for(size_t i = 1; i <= maxGrainId; i++)
    {
      masterStream << phaseNames.getValue(i) << " ";
    }
  }
  else
  {
    for(size_t i = 1; i <= numTuples - 1; i++)
    {
      masterStream << phaseNames.getValue(i) << " ";
    }
    for(size_t i = numTuples; i <= maxGrainId; ++i)
    {
      masterStream << QString("Phase_%1 ").arg(i);
    }
  }

  masterStream << "\n";
  masterStream << QString("matr %1\n").arg(ne_x * ne_y * ne_z);

  auto func = [&featureIds](size_t i) { return QString::number(featureIds.at(i)); };

  EntriesHelper::writeEntries(masterStream, func, totalPoints, 40);

  return {0, QObject::tr("")};
}
} // namespace OnScaleTableFileWriter
