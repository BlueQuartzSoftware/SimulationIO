#include "EntriesHelper.h"

void EntriesHelper::writeEntries(QTextStream& stream, std::function<QString(int32_t)> func, int32_t maxIndex, size_t maxEntriesPerLine, const QString& delimiter, const QString& eol, size_t startingNumEntries)
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
        stream << eol;
        entriesPerLine = 0;
      }
    }
    stream << func(i);
    entriesPerLine++;
  }
}
