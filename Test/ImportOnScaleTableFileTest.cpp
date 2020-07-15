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
  int32_t CreateInputTableFile(int32_t xDim, int32_t yDim, int32_t zDim, const std::vector<std::string>& names)
  {

    std::ofstream outfile(k_TestFile, std::ios_base::out);
    if(!outfile.is_open())
    {
      return -1;
    }
    const int32_t entryCount = 8;
    int32_t currentEntryCount = 0;
    outfile << "hedr         0" << k_NL;
    outfile << "info         1" << k_NL;
    outfile << std::scientific;
    if(xDim > 1)
    {
      currentEntryCount = 0;
      outfile << "xcrd     " << xDim << k_NL << " ";
      float value = 0.0;
      float maxValue = value + (xDim * k_XIncrement);
      while(value < maxValue)
      {
        outfile << value << " ";
        value = value + k_XIncrement;
        currentEntryCount++;
        if(currentEntryCount == entryCount)
        {
          outfile << k_NL << " ";
          currentEntryCount = 0;
        }
      }
      if(currentEntryCount != 0)
      {
        outfile << k_NL;
      }
    }

    if(yDim > 1)
    {
      currentEntryCount = 0;
      outfile << "ycrd     " << yDim << k_NL << " ";
      float value = 0.0;
      float maxValue = value + (yDim * k_YIncrement);
      while(value < maxValue)
      {
        outfile << value << " ";
        value = value + k_YIncrement;
        currentEntryCount++;
        if(currentEntryCount == entryCount)
        {
          outfile << k_NL << " ";
          currentEntryCount = 0;
        }
      }
      if(currentEntryCount != 0)
      {
        outfile << k_NL;
      }
    }

    if(zDim > 1)
    {
      currentEntryCount = 0;
      outfile << "zcrd     " << zDim << k_NL << " ";
      float value = 0.0;
      float maxValue = value + (zDim * k_ZIncrement);
      while(value < maxValue)
      {
        outfile << value << " ";
        value = value + k_ZIncrement;
        currentEntryCount++;
        if(currentEntryCount == entryCount)
        {
          outfile << k_NL << " ";
          currentEntryCount = 0;
        }
      }
      if(currentEntryCount != 0)
      {
        outfile << k_NL;
      }
    }

    outfile << "name        " << names.size() << k_NL;
    for(const auto& name : names)
    {
      outfile << "" << name << k_NL;
    }

    int32_t totalMatr = std::abs((xDim - 1) * (yDim - 1) * (zDim - 1));
    outfile << "matr        " << totalMatr << k_NL;
    currentEntryCount = 0;

    for(int32_t i = 0; i < totalMatr; i++)
    {
      outfile << std::setw(10) << currentEntryCount;
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
  }

  // -----------------------------------------------------------------------------
  int TestImportOnScaleTableFile()
  {

    std::vector<std::string> names = {"pzt4t11", "pzt4t12", "pzt4t13", "pzt4t14", "pzt4t15", "pzt4t16", "pzt4t17", "pzt4t18", "pzt4t19", "pzt4t20"};
    int32_t err = CreateInputTableFile(20, 17, 2, names);
    DREAM3D_REQUIRE_EQUAL(err, 0)
    TestImport();

    err = CreateInputTableFile(10, 0, 10, names);
    DREAM3D_REQUIRE_EQUAL(err, 0)
    TestImport();

    err = CreateInputTableFile(0, 10, 10, names);
    DREAM3D_REQUIRE_EQUAL(err, 0)
    TestImport();

    err = CreateInputTableFile(10, 10, 0, names);
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
