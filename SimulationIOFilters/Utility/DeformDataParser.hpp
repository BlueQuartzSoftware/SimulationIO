/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
 * All rights reserved.
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#pragma once

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

namespace SimulationIO
{

class DeformDataParser
{
public:
  using Self = DeformDataParser;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer()
  {
    return Pointer(static_cast<Self*>(nullptr));
  }
  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  virtual QString getNameOfClass() const
  {
    return QString("DeformDataParser");
  }
  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  static QString ClassName()
  {
    return QString("DeformDataParser");
  }

  virtual ~DeformDataParser() = default;

  void setColumnName(const QString& value)
  {
    m_ColumnName = value;
  }
  QString getColumnName()
  {
    return m_ColumnName;
  }

  void setColumnIndex(int32_t value)
  {
    m_ColumnIndex = value;
  }
  int32_t getColumnIndex()
  {
    return m_ColumnIndex;
  }

  virtual void setDataArray(IDataArray::Pointer value)
  {
    m_DataArray = value;
  }

  IDataArray::Pointer getDataArray()
  {
    return m_DataArray;
  }
  virtual IDataArray::Pointer initializeNewDataArray(size_t numTuples, const QString& name, bool allocate)
  {
    return IDataArray::NullPointer();
  }

  virtual void parse(const QByteArray& token, size_t index)
  {
  }

protected:
  DeformDataParser() = default;

private:
  IDataArray::Pointer m_DataArray;
  int32_t m_ColumnIndex = 0;
  QString m_ColumnName = QString("");

  DeformDataParser(const DeformDataParser&); // Copy Constructor Not Implemented
  void operator=(const DeformDataParser&);   // Move assignment Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class Int32Parser : public DeformDataParser
{
public:
  using Self = Int32Parser;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer()
  {
    return Pointer(static_cast<Self*>(nullptr));
  }
  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  virtual QString getNameOfClass() const override
  {
    return QString("Int32Parser");
  }
  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  static QString ClassName()
  {
    return QString("Int32Parser");
  }

  virtual ~Int32Parser() = default;

  static Pointer New(Int32ArrayType::Pointer ptr, const QString& name, int colIndex)
  {
    Pointer sharedPtr(new Int32Parser(ptr, name, colIndex));
    return sharedPtr;
  }

  void setDataArray(IDataArray::Pointer value) override
  {
    DeformDataParser::setDataArray(value);
    m_Ptr = std::dynamic_pointer_cast<Int32ArrayType>(value);
  }

  IDataArray::Pointer initializeNewDataArray(size_t numTuples, const QString& name, bool allocate) override
  {
    Int32ArrayType::Pointer array = Int32ArrayType::CreateArray(numTuples, name, allocate);
    if(allocate)
    {
      array->initializeWithZeros();
    }
    return array;
  }

  virtual void parse(const QByteArray& token, size_t index) override
  {
    bool ok = false;
    (*m_Ptr)[index] = token.toInt(&ok, 10);
  }

protected:
  Int32Parser(Int32ArrayType::Pointer ptr, const QString& name, int index)
  {
    setColumnName(name);
    setColumnIndex(index);
    setDataArray(ptr);
    m_Ptr = ptr;
  }

private:
  Int32ArrayType::Pointer m_Ptr;

  Int32Parser(const Int32Parser&);    // Copy Constructor Not Implemented
  void operator=(const Int32Parser&); // Move assignment Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class FloatParser : public DeformDataParser
{
public:
  using Self = FloatParser;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer()
  {
    return Pointer(static_cast<Self*>(nullptr));
  }
  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  virtual QString getNameOfClass() const override
  {
    return QString("FloatParser");
  }
  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  static QString ClassName()
  {
    return QString("FloatParser");
  }

  virtual ~FloatParser() = default;

  static Pointer New(FloatArrayType::Pointer ptr, const QString& name, int colIndex)
  {
    Pointer sharedPtr(new FloatParser(ptr, name, colIndex));
    return sharedPtr;
  }

  void setDataArray(IDataArray::Pointer value) override
  {
    DeformDataParser::setDataArray(value);
    m_Ptr = std::dynamic_pointer_cast<FloatArrayType>(value);
  }

  IDataArray::Pointer initializeNewDataArray(size_t numTuples, const QString& name, bool allocate) override
  {
    FloatArrayType::Pointer array = FloatArrayType::CreateArray(numTuples, name, allocate);
    if(allocate)
    {
      array->initializeWithZeros();
    }
    return array;
  }

  virtual void parse(const QByteArray& token, size_t index) override
  {
    bool ok = false;
    (*m_Ptr)[index] = token.toFloat(&ok);
  }

protected:
  FloatParser(FloatArrayType::Pointer ptr, const QString& name, int index)
  {
    setColumnName(name);
    setColumnIndex(index);
    setDataArray(ptr);
    m_Ptr = ptr;
  }

private:
  FloatArrayType::Pointer m_Ptr;

  FloatParser(const FloatParser&);    // Copy Constructor Not Implemented
  void operator=(const FloatParser&); // Move assignment Not ImplementedOperator '=' Not Implemented
};

} // namespace SimulationIO
