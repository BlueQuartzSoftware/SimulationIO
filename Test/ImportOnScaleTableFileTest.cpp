// -----------------------------------------------------------------------------
// Insert your license & copyright information here
// -----------------------------------------------------------------------------
#pragma once

#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Geometry/RectGridGeom.h"

#include "SimulationIO/SimulationIOFilters/ImportOnScaleTableFile.h"

#include "UnitTestSupport.hpp"

#include "SimulationIOTestFileLocations.h"

class ImportOnScaleTableFileTest
{

  const std::string k_TestFile = UnitTest::TestTempDir.toStdString() + "/OnScaleTestFile.flxtbl";
  const char k_NL = '\n';
  const float k_XIncrement = 0.001f;
  const float k_YIncrement = 0.002f;
  const float k_ZIncrement = 0.003f;

  const DataArrayPath k_VolumeDCName = {"OnScale Volume", "", ""};
  const QString k_CellAMName = {"Table Data"};
  const QString k_PhaseAMName = {"Material Info"};
  const QString k_MaterialArrayName = {"Names"};
  const QString k_FeatureIdName = {"FeatureIds"};
  const std::vector<std::string> k_Names = {"pzt4t11", "pzt4t12", "pzt4t13", "pzt4t14", "pzt4t15", "pzt4t16", "pzt4t17", "pzt4t18", "pzt4t19", "pzt4t20"};

  FloatArrayType::Pointer m_Xcrd = FloatArrayType::NullPointer();
  FloatArrayType::Pointer m_Ycrd = FloatArrayType::NullPointer();
  FloatArrayType::Pointer m_Zcrd = FloatArrayType::NullPointer();
  Int32ArrayType::Pointer m_Data = Int32ArrayType::NullPointer();

  int32_t m_XDim = 0;
  int32_t m_YDim = 0;
  int32_t m_ZDim = 0;

public:
  ImportOnScaleTableFileTest() = default;
  ~ImportOnScaleTableFileTest() = default;

  ImportOnScaleTableFileTest(const ImportOnScaleTableFileTest&) = delete;            // Copy Constructor
  ImportOnScaleTableFileTest(ImportOnScaleTableFileTest&&) = delete;                 // Move Constructor
  ImportOnScaleTableFileTest& operator=(const ImportOnScaleTableFileTest&) = delete; // Copy Assignment
  ImportOnScaleTableFileTest& operator=(ImportOnScaleTableFileTest&&) = delete;      // Move Assignment

  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    //  QFile::remove(k_TestFile);
#endif
  }

  // -----------------------------------------------------------------------------
  int32_t CreateInputTableFile()
  {
    m_Xcrd = FloatArrayType::NullPointer();
    m_Ycrd = FloatArrayType::NullPointer();
    m_Zcrd = FloatArrayType::NullPointer();
    m_Data = Int32ArrayType::NullPointer();

    std::ofstream outfile(k_TestFile, std::ios_base::out);
    if(!outfile.is_open())
    {
      return -1;
    }
    const int32_t entryCount = k_Names.size();
    int32_t currentEntryCount = 0;
    outfile << "hedr         0" << k_NL;
    outfile << "info         1" << k_NL;
    outfile << std::scientific;
    if(m_XDim > 1)
    {
      currentEntryCount = 0;
      outfile << "xcrd     " << m_XDim << k_NL;
      float value = 0.0;
      float maxValue = value + (m_XDim * k_XIncrement);
      int total = (maxValue - value) / k_XIncrement;
      m_Xcrd = FloatArrayType::CreateArray(total, QString("Xcrd"), true);
      int loopCount = 0;
      while(value < maxValue)
      {
        outfile << " " << value;
        m_Xcrd->setValue(loopCount, value);
        value = value + k_XIncrement;
        currentEntryCount++;
        if(currentEntryCount == entryCount)
        {
          outfile << k_NL;
          currentEntryCount = 0;
        }
        loopCount++;
      }
      if(currentEntryCount != 0)
      {
        outfile << k_NL;
      }
    }

    if(m_YDim > 1)
    {
      currentEntryCount = 0;
      outfile << "ycrd     " << m_YDim << k_NL;
      float value = 0.0;
      float maxValue = value + (m_YDim * k_YIncrement);
      int total = (maxValue - value) / k_YIncrement;
      m_Ycrd = FloatArrayType::CreateArray(total, QString("Ycrd"), true);
      int loopCount = 0;
      while(value < maxValue)
      {
        outfile << " " << value;
        m_Ycrd->setValue(loopCount, value);
        value = value + k_YIncrement;
        currentEntryCount++;
        if(currentEntryCount == entryCount)
        {
          outfile << k_NL;
          currentEntryCount = 0;
        }
        loopCount++;
      }
      if(currentEntryCount != 0)
      {
        outfile << k_NL;
      }
    }

    if(m_ZDim > 1)
    {
      currentEntryCount = 0;
      outfile << "zcrd     " << m_ZDim << k_NL;
      float value = 0.0;
      float maxValue = value + (m_ZDim * k_ZIncrement);
      int total = (maxValue - value) / k_ZIncrement;
      m_Zcrd = FloatArrayType::CreateArray(total, QString("Zcrd"), true);
      int loopCount = 0;
      while(value < maxValue)
      {
        outfile << " " << value;
        m_Zcrd->setValue(loopCount, value);
        value = value + k_ZIncrement;
        currentEntryCount++;
        if(currentEntryCount == entryCount)
        {
          outfile << k_NL;
          currentEntryCount = 0;
        }
        loopCount++;
      }
      if(currentEntryCount != 0)
      {
        outfile << k_NL;
      }
    }

    outfile << "name        " << k_Names.size() << k_NL;
    for(const auto& name : k_Names)
    {
      outfile << "" << name << k_NL;
    }

    int32_t totalMatr = std::abs((m_XDim - 1) * (m_YDim - 1) * (m_ZDim - 1));
    outfile << "matr        " << totalMatr << k_NL;
    currentEntryCount = 0;
    m_Data = Int32ArrayType::CreateArray(totalMatr, QString("Data"), true);

    for(int32_t i = 0; i < totalMatr; i++)
    {
      outfile << std::setw(10) << currentEntryCount;
      m_Data->setValue(i, currentEntryCount);
      currentEntryCount++;
      if(currentEntryCount == entryCount)
      {
        outfile << k_NL << " ";
        currentEntryCount = 0;
      }
    }

    return 0;
  }

  // -----------------------------------------------------------------------------
  void TestImport()
  {
    ImportOnScaleTableFile::Pointer filter = ImportOnScaleTableFile::New();
    DataContainerArray::Pointer dca = DataContainerArray::New();
    filter->setDataContainerArray(dca);
    filter->setVolumeDataContainerName(k_VolumeDCName);
    filter->setCellAttributeMatrixName(k_CellAMName);
    filter->setPhaseAttributeMatrixName(k_PhaseAMName);
    filter->setMaterialNameArrayName(k_MaterialArrayName);
    filter->setInputFile(QString::fromStdString(k_TestFile));
    filter->setOrigin({0.0f, 0.0f, 0.0f});
    filter->setSpacing({0.001f, 0.001f, 0.001f});
    filter->setFeatureIdsArrayName(k_FeatureIdName);

    filter->preflight();
    int32_t err = filter->getErrorCode();
    DREAM3D_REQUIRED(err, ==, 0)

    dca = DataContainerArray::New();
    filter->setDataContainerArray(dca);
    filter->execute();
    err = filter->getErrorCode();
    DREAM3D_REQUIRED(err, ==, 0)

    DataArrayPath phaseNamesPath = k_VolumeDCName;
    phaseNamesPath.setAttributeMatrixName(k_PhaseAMName);
    phaseNamesPath.setDataArrayName(k_MaterialArrayName);
    DataContainer::Pointer dc = dca->getDataContainer(k_VolumeDCName);
    DREAM3D_REQUIRE_VALID_POINTER(dc)
    RectGridGeom::Pointer geom = dc->getGeometryAs<RectGridGeom>();
    DREAM3D_REQUIRE_VALID_POINTER(geom)
    std::vector<size_t> dims = geom->getDimensions().toContainer<std::vector<size_t>>();
    if(m_XDim <= 0)
    {
      DREAM3D_REQUIRE_NULL_POINTER(m_Xcrd)
    }
    else
    {
      DREAM3D_REQUIRE_VALID_POINTER(m_Xcrd)
      DREAM3D_REQUIRE_EQUAL(m_Xcrd->getSize() - 1, dims[0])
    }
    if(m_YDim <= 0)
    {
      DREAM3D_REQUIRE_NULL_POINTER(m_Ycrd)
    }
    else
    {
      DREAM3D_REQUIRE_VALID_POINTER(m_Ycrd)
      DREAM3D_REQUIRE_EQUAL(m_Ycrd->getSize() - 1, dims[1])
    }
    if(m_ZDim <= 0)
    {
      DREAM3D_REQUIRE_NULL_POINTER(m_Zcrd)
    }
    else
    {
      DREAM3D_REQUIRE_VALID_POINTER(m_Zcrd)
      DREAM3D_REQUIRE_EQUAL(m_Zcrd->getSize() - 1, dims[2])
    }
    AttributeMatrix::Pointer phaseAM = dca->getAttributeMatrix(phaseNamesPath);
    DREAM3D_REQUIRE_VALID_POINTER(phaseAM)
    StringDataArray::Pointer materialNamesDA = phaseAM->getAttributeArrayAs<StringDataArray>(k_MaterialArrayName);
    DREAM3D_REQUIRE_VALID_POINTER(materialNamesDA)

    for(size_t i = 0; i < materialNamesDA->getSize(); i++)
    {
      if(materialNamesDA->getValue(i).toStdString() != k_Names[i])
      {
        DREAM3D_REQUIRE_EQUAL(0, 1)
      }
    }

    DataArrayPath featureIdsPath = k_VolumeDCName;
    featureIdsPath.setAttributeMatrixName(k_CellAMName);
    featureIdsPath.setDataArrayName(k_FeatureIdName);
    AttributeMatrix::Pointer cellAM = dca->getAttributeMatrix(featureIdsPath);
    DREAM3D_REQUIRE_VALID_POINTER(cellAM)
    Int32ArrayType::Pointer featureIdsDA = cellAM->getAttributeArrayAs<Int32ArrayType>(k_FeatureIdName);
    DREAM3D_REQUIRE_VALID_POINTER(featureIdsDA)
    DREAM3D_REQUIRE_EQUAL(featureIdsDA->getSize(), m_Data->getSize())
    for(size_t i = 0; i < featureIdsDA->getSize(); i++)
    {
      int32_t value_1 = featureIdsDA->getValue(i);
      int32_t value_2 = m_Data->getValue(i);
      DREAM3D_REQUIRE_EQUAL(value_1, value_2)
    }
  }

  // -----------------------------------------------------------------------------
  int TestImportOnScaleTableFile()
  {
    m_XDim = 20;
    m_YDim = 17;
    m_ZDim = 2;
    int32_t err = CreateInputTableFile();
    DREAM3D_REQUIRE_EQUAL(err, 0)
    TestImport();

    m_XDim = 10;
    m_YDim = 0;
    m_ZDim = 10;
    err = CreateInputTableFile();
    DREAM3D_REQUIRE_EQUAL(err, 0)
    TestImport();

    m_XDim = 0;
    m_YDim = 10;
    m_ZDim = 10;
    err = CreateInputTableFile();
    DREAM3D_REQUIRE_EQUAL(err, 0)
    TestImport();

    m_XDim = 10;
    m_YDim = 10;
    m_ZDim = 0;
    err = CreateInputTableFile();
    DREAM3D_REQUIRE_EQUAL(err, 0)
    TestImport();

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;
    std::cout << "############ Starting TestImportOnScaleTableFile  ##############" << std::endl;

    DREAM3D_REGISTER_TEST(TestImportOnScaleTableFile())
  }

private:
};
