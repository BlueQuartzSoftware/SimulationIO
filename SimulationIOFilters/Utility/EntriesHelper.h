#pragma once

#include <cstdint>
#include <functional>

#include <QtCore/QString>
#include <QtCore/QTextStream>

namespace EntriesHelper
{
void writeEntries(QTextStream& stream, std::function<QString(int32_t)> func, int32_t maxIndex, size_t maxEntriesPerLine, const QString& delimiter = " ", const QString& eol = "\n", size_t startingNumEntries = 0);
}
