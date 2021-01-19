

#include <cstdint>

#include <QtCore/QFile>

#include "SIMPLib/Common/QtBackwardCompatibilityMacro.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/AbstractFilter.h"

namespace OnScaleTableFileUtils
{

constexpr int32_t k_InputFileNotSet = -42000;
constexpr int32_t k_InputFileDoesNotExist = -42001;
constexpr int32_t k_ErrorOpeningFile = -42002;
constexpr int32_t k_MaterialParseError = -42003;
constexpr int32_t k_CoordParseError = -42004;

// -----------------------------------------------------------------------------
int32_t parseValues(QFile& in, FloatArrayType& data)
{
  bool ok = false;
  size_t count = 0;
  while(count < data.size())
  {
    QString line = in.readLine().trimmed();
    QStringList tokens = line.split(" ", QSTRING_SKIP_EMPTY_PARTS);
    for(const auto& token : tokens)
    {
      float value = token.toFloat(&ok);
      if(ok)
      {
        data[count] = value;
        count++;
      }
      else
      {
        return k_CoordParseError;
      }
    }
  }
  return 0;
}

// -----------------------------------------------------------------------------
int32_t parseValues(AbstractFilter* filter, QFile& in, Int32ArrayType& data)
{
  bool ok = false;
  size_t count = 0;
  size_t increment = data.size() / 100;
  size_t prog = 0;
  int32_t progress = 0;
  while(count < data.size())
  {
    QString line = in.readLine().trimmed();
    QStringList tokens = line.split(" ", QSTRING_SKIP_EMPTY_PARTS);
    for(const auto& token : tokens)
    {
      int32_t value = token.toInt(&ok);
      if(ok)
      {
        data[count] = value;
        count++;
      }
      else
      {
        return k_MaterialParseError;
      }
      prog++;
      if(prog > increment)
      {
        prog = 0;
        progress++;
        QString ss = QString("Reading Material Data %1%").arg(progress);
        filter->notifyStatusMessage(ss);
      }
    }
  }
  return 0;
}

// -----------------------------------------------------------------------------
void parseValues(QFile& in, std::vector<QString>& data)
{
  size_t count = 0;

  while(count < data.size())
  {
    QString line = in.readLine().trimmed();
    QStringList tokens = line.split(" ", QSTRING_SKIP_EMPTY_PARTS);
    for(const auto& token : tokens)
    {
      data[count] = token;
      count++;
    }
  }
}

} // namespace OnScaleTableFileUtils
