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

#include "BsamFileWriter.h"

#include <QtCore/QDir>
#include <QtCore/QTextStream>

bool BsamFileWriter::write(const QString& outputPath, const QString& filePrefix, int32_t numClusters)
{
  QString masterFilePath = outputPath + QDir::separator() + filePrefix + ".in";

  QFile masterFile(masterFilePath);
  if(!masterFile.open(QIODevice::OpenModeFlag::WriteOnly))
  {
    QString ss = QObject::tr("BSAM file can not be created: %1").arg(masterFilePath);
    return false;
  }

  QTextStream masterStream(&masterFile);

  masterStream << "***********************************************\n";
  masterStream << "**** BSAM INPUT FILE  generated by DREAM.3D ***\n";
  masterStream << "***********************************************\n";
  masterStream << "\n";

  for(int32_t i = 0; i < numClusters; i++)
  {
    QString inputFilePath = outputPath + QDir::separator() + filePrefix + QString("_Cluster") + QString::number(i + 1) + ".ele";

    QFile inputFile(inputFilePath);

    if(!inputFile.open(QIODevice::OpenModeFlag::ReadOnly))
    {
      QString ss = QObject::tr("BSAM Input file could not be opened: %1").arg(inputFilePath);
      return false;
    }

    QTextStream inputStream(&inputFile);

    masterStream << "***********************************************\n";

    while(!inputStream.atEnd())
    {
      masterStream << inputStream.readLine();
    }

    masterStream << "\n";
  }

  return true;
}
