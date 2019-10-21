#include "OnScaleTableFileWriter.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

namespace
{
void writeEntries(QTextStream& stream, std::function<QString(int32_t)> func, int32_t maxIndex, size_t maxEntriesPerLine, const QString& delimiter = " ", size_t startingNumEntries = 0)
{
  size_t entriesPerLine = startingNumEntries;
  for(int32_t i = 0; i < maxIndex; i++)
  {
    if(entriesPerLine != 0)
    {
      if((entriesPerLine % maxEntriesPerLine) != 0)
      {
        stream << delimiter;
      }
      else
      {
        stream << '\n';
        entriesPerLine = 0;
      }
    }
    stream << func(i);
    entriesPerLine++;
  }
}

void writeCoords(QTextStream& stream, const QString& text, int32_t maxIndex, float origin, float spacing)
{
  stream << QString("%1 %2\n").arg(text).arg(maxIndex);

  auto func = [origin, spacing](int32_t i) { return QString::number(origin + (i * spacing), 'e', 3); };

  writeEntries(stream, func, maxIndex, 6);

  stream << "\n";
}

} // namespace

bool OnScaleTableFileWriter::write(const ImageGeom& imageGeom, const StringDataArray& phaseNames, const DataArray<int32_t>& featureIds, const QString& outputPath, const QString& filePrefix,
                                   const IntVec3Type& numKeypoints)
{
  SizeVec3Type dims = imageGeom.getDimensions();
  FloatVec3Type spacing = imageGeom.getSpacing();
  FloatVec3Type origin = imageGeom.getOrigin();

  int32_t numTuples = static_cast<int32_t>(phaseNames.getNumberOfTuples());

  // Find total number of grain ids

  auto maxElement = std::max_element(featureIds.begin(), featureIds.end());

  if(maxElement == featureIds.end())
  {
    return false;
  }

  int32_t maxGrainId = *maxElement;

  int32_t totalPoints = imageGeom.getNumberOfElements();

  QString masterFilePath = outputPath + QDir::separator() + filePrefix + ".flxtbl";

  QFile masterFile(masterFilePath);

  if(!masterFile.open(QIODevice::OpenModeFlag::WriteOnly))
  {
    QString ss = QObject::tr("Error writing PZFLEX input file '%1'").arg(masterFilePath);
    return false;
  }

  QTextStream masterStream(&masterFile);

  masterStream.setRealNumberNotation(QTextStream::RealNumberNotation::FixedNotation);
  masterStream.setRealNumberPrecision(3);

  masterStream << "hedr 0\n";
  masterStream << "info 1\n";

  int32_t ne_x = dims[0];
  int32_t ne_y = dims[1];
  int32_t ne_z = dims[2];

  int32_t nnode_x = ne_x + 1;
  int32_t nnode_y = ne_y + 1;
  int32_t nnode_z = ne_z + 1;

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
    for(int32_t i = 1; i <= maxGrainId; i++)
    {
      masterStream << phaseNames.getValue(i) << " ";
    }
  }
  else
  {
    for(int32_t i = 1; i <= numTuples - 1; i++)
    {
      masterStream << phaseNames.getValue(i) << " ";
    }
    for(int32_t i = numTuples; i <= maxGrainId; ++i)
    {
      masterStream << QString("Phase_%1 ").arg(i);
    }
  }

  masterStream << "\n";
  masterStream << QString("matr %1\n").arg(ne_x * ne_y * ne_z);

  auto func = [&featureIds](int32_t i) { return QString::number(featureIds.at(i), 'f', 3); };

  writeEntries(masterStream, func, totalPoints, 40);

  return true;
}
