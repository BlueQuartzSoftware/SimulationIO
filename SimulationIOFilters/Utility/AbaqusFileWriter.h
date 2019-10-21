#pragma once

#include <QtCore/QString>

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SIMPLib/Geometry/ImageGeom.h"

namespace AbaqusFileWriter
{
bool write(const ImageGeom& imageGeom, const DataArray<int32_t>& featureIds, const DataArray<int32_t>& cellPhases, const DataArray<float>& cellEulerAngles, const DynamicTableData& matConst,
           const QString& outputPath, const QString& filePrefix, const QString& jobName, int32_t numDepvar, int32_t numMatConst, int32_t numUserOutVar);
}
