#pragma once

#include <QtCore/QString>

#include "SIMPLib/Common/SIMPLArray.hpp"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/Geometry/ImageGeom.h"

namespace OnScaleTableFileWriter
{
bool write(const ImageGeom& imageGeom, const StringDataArray& phaseNames, const DataArray<int32_t>& featureIds, const QString& outputPath, const QString& filePrefix,
           const IntVec3Type& numKeypoints);
}
