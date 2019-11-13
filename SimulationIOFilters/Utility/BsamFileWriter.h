#pragma once

#include <cstdint>

#include <QtCore/QString>

namespace BsamFileWriter
{
bool write(const QString& outputPath, const QString& filePrefix, int32_t numClusters);
}
