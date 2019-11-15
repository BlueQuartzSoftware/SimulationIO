#include "OnScaleTableFileWriter.h"

void OnScaleTableFileWriter::writeCoords(QTextStream& stream, const QString& text, size_t maxIndex, float origin, float spacing)
{
  stream << QString("%1 %2\n").arg(text).arg(maxIndex);

  auto func = [origin, spacing](size_t i) { return QString::number(origin + (i * spacing), 'f', 3); };

  EntriesHelper::writeEntries(stream, func, maxIndex, 6);

  stream << "\n";
}
