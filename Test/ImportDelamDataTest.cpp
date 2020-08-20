/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
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
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
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
 *
 * The code contained herein was partially funded by the followig contracts:
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <fstream>

#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Geometry/RectGridGeom.h"

#include "UnitTestSupport.hpp"

#include "SimulationIO/SimulationIOFilters/ImportDelamData.h"

#include "SimulationIOTestFileLocations.h"

class ImportDelamDataTest
{
  static inline const std::string k_CSDGMTestFile = UnitTest::TestTempDir.toStdString() + "/csdgm.txt";
  static inline const std::string k_BvidStdOutTestFile = UnitTest::TestTempDir.toStdString() + "/bvid_stdout.txt";
  static inline const std::string k_WrongCSDGMTestFile = UnitTest::TestTempDir.toStdString() + "/csdgm.exe";
  static inline const std::string k_WrongBvidStdOutTestFile = UnitTest::TestTempDir.toStdString() + "/bvid_stdout.rar";
  static inline constexpr char k_NL = '\n';

public:
  ImportDelamDataTest() = default;
  ~ImportDelamDataTest() = default;
  ImportDelamDataTest(const ImportDelamDataTest&) = delete;            // Copy Constructor
  ImportDelamDataTest(ImportDelamDataTest&&) = delete;                 // Move Constructor
  ImportDelamDataTest& operator=(const ImportDelamDataTest&) = delete; // Copy Assignment
  ImportDelamDataTest& operator=(ImportDelamDataTest&&) = delete;      // Move Assignment

  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the ImportDelamDataTest Filter from the FilterManager
    QString filtName = "ImportDelamData";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory)
    {
      std::stringstream ss;
      ss << "The ImportDelamDataTest Requires the use of the " << filtName.toStdString()
         << " filter which is found in the SimulationIO Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  // Bvid StdOut File Creation Methods
  // -----------------------------------------------------------------------------
  int32_t CreateBvidStdOutFile_0()
  {
    std::ofstream outfile(k_BvidStdOutTestFile, std::ios_base::out);
    if(!outfile.is_open())
    {
      return -1;
    }

    outfile << "Desired" << k_NL;

    return 0;
  }

  // -----------------------------------------------------------------------------
  int32_t CreateBvidStdOutFile_1()
  {
    std::ofstream outfile(k_BvidStdOutTestFile, std::ios_base::out);
    if(!outfile.is_open())
    {
      return -1;
    }

    outfile << "Desired 0/90ply Elx = 0.5 Current  0/90ply Elx" << k_NL;

    return 0;
  }

  // -----------------------------------------------------------------------------
  int32_t CreateBvidStdOutFile_2()
  {
    std::ofstream outfile(k_BvidStdOutTestFile, std::ios_base::out);
    if(!outfile.is_open())
    {
      return -1;
    }

    outfile << "Desired 0/90ply Elx = 0.5 Current  0/90ply Elx = 0.7071067811865476";

    return 0;
  }

  // -----------------------------------------------------------------------------
  int32_t CreateBvidStdOutFile_3()
  {
    std::ofstream outfile(k_BvidStdOutTestFile, std::ios_base::out);
    if(!outfile.is_open())
    {
      return -1;
    }

    outfile << "Desired 0/90ply Elx = 0.5 Current  0/90ply Elx = 0.7071067811865476" << k_NL;
    outfile << "Previous Lcx = 40.0 Updated";

    return 0;
  }

  // -----------------------------------------------------------------------------
  int32_t CreateBvidStdOutFile_4()
  {
    std::ofstream outfile(k_BvidStdOutTestFile, std::ios_base::out);
    if(!outfile.is_open())
    {
      return -1;
    }

    outfile << "Desired 0/90ply Elx = 0.5 Current  0/90ply Elx = 0.7071067811865476" << k_NL;
    outfile << "Previous Lcx = 40.0 Updated Lcx = 38.18376618407357";

    return 0;
  }

  // -----------------------------------------------------------------------------
  int32_t CreateBvidStdOutFile_5()
  {
    std::ofstream outfile(k_BvidStdOutTestFile, std::ios_base::out);
    if(!outfile.is_open())
    {
      return -1;
    }

    outfile << "Desired 0/90ply Elx = 0.5 Current  0/90ply Elx = 0.7071067811865476" << k_NL;
    outfile << "Previous Lcx = 40.0 Updated Lcx = 38.18376618407357" << k_NL;
    outfile << "Total CPT:";

    return 0;
  }

  // -----------------------------------------------------------------------------
  int32_t CreateBvidStdOutFile_6()
  {
    std::ofstream outfile(k_BvidStdOutTestFile, std::ios_base::out);
    if(!outfile.is_open())
    {
      return -1;
    }

    outfile << "Desired 0/90ply Elx = 0.5 Current  0/90ply Elx = 0.7071067811865476" << k_NL;
    outfile << "Previous Lcx = 40.0 Updated Lcx = 38.18376618407357" << k_NL;
    outfile << "Total CPT: 2.032";

    return 0;
  }

  // -----------------------------------------------------------------------------
  int32_t CreateBvidStdOutFile_7()
  {
    std::ofstream outfile(k_BvidStdOutTestFile, std::ios_base::out);
    if(!outfile.is_open())
    {
      return -1;
    }

    outfile << "Desired 0/90ply Elx = 0.5 Current  0/90ply Elx = 0.7071067811865476" << k_NL;
    outfile << "Previous Lcx = 40.0 Updated Lcx = 38.18376618407357" << k_NL;
    outfile << "Total CPT: 2.032" << k_NL;
    outfile << "Total number of plies";

    return 0;
  }

  // -----------------------------------------------------------------------------
  int32_t CreateBvidStdOutFile_8()
  {
    std::ofstream outfile(k_BvidStdOutTestFile, std::ios_base::out);
    if(!outfile.is_open())
    {
      return -1;
    }

    outfile << "Desired 0/90ply Elx = 0.5 Current  0/90ply Elx = 0.7071067811865476" << k_NL;
    outfile << "Previous Lcx = 40.0 Updated Lcx = 38.18376618407357" << k_NL;
    outfile << "Total CPT: 2.032" << k_NL;
    outfile << "Total number of plies = 16";

    return 0;
  }

  // -----------------------------------------------------------------------------
  int32_t CreateBvidStdOutFile_9()
  {
    std::ofstream outfile(k_BvidStdOutTestFile, std::ios_base::out);
    if(!outfile.is_open())
    {
      return -1;
    }

    outfile << "Desired 0/90ply Elx = 0.5 Current  0/90ply Elx = 0.7071067811865476" << k_NL;
    outfile << "Previous Lcx = 40.0 Updated Lcx = 38.18376618407357" << k_NL;
    outfile << "Total CPT: 2.032" << k_NL;
    outfile << "Total number of plies = 16" << k_NL;
    outfile << "Total number of CZM layers";

    return 0;
  }

  // -----------------------------------------------------------------------------
  int32_t CreateBvidStdOutFile_10()
  {
    std::ofstream outfile(k_BvidStdOutTestFile, std::ios_base::out);
    if(!outfile.is_open())
    {
      return -1;
    }

    outfile << "Desired 0/90ply Elx = 0.5 Current  0/90ply Elx = 0.7071067811865476" << k_NL;
    outfile << "Previous Lcx = 40.0 Updated Lcx = 38.18376618407357" << k_NL;
    outfile << "Total CPT: 2.032" << k_NL;
    outfile << "Total number of plies = 16" << k_NL;
    outfile << "Total number of CZM layers = 15";

    return 0;
  }

  // -----------------------------------------------------------------------------
  // CSDGM File Creation Methods
  // -----------------------------------------------------------------------------
  int32_t CreateCSDGMFile_0()
  {
    std::ofstream outfile(k_CSDGMTestFile, std::ios_base::out);
    if(!outfile.is_open())
    {
      return -1;
    }

    outfile << "0.1 0.1 0.1";

    return 0;
  }

  // -----------------------------------------------------------------------------
  int32_t CreateCSDGMFile_1()
  {
    std::ofstream outfile(k_CSDGMTestFile, std::ios_base::out);
    if(!outfile.is_open())
    {
      return -1;
    }

    outfile << "invalid 0.1 0.1 0.1";

    return 0;
  }

  // -----------------------------------------------------------------------------
  int32_t CreateCSDGMFile_2()
  {
    std::ofstream outfile(k_CSDGMTestFile, std::ios_base::out);
    if(!outfile.is_open())
    {
      return -1;
    }

    outfile << "0.1 invalid 0.1 0.1";

    return 0;
  }

  // -----------------------------------------------------------------------------
  int32_t CreateCSDGMFile_3()
  {
    std::ofstream outfile(k_CSDGMTestFile, std::ios_base::out);
    if(!outfile.is_open())
    {
      return -1;
    }

    outfile << "0.1 0.1 invalid 0.1";

    return 0;
  }

  // -----------------------------------------------------------------------------
  int32_t CreateCSDGMFile_4()
  {
    std::ofstream outfile(k_CSDGMTestFile, std::ios_base::out);
    if(!outfile.is_open())
    {
      return -1;
    }

    outfile << "0.1 0.1 0.1 invalid";

    return 0;
  }

  // -----------------------------------------------------------------------------
  int32_t CreateCSDGMFile_5()
  {
    std::ofstream outfile(k_CSDGMTestFile, std::ios_base::out);
    if(!outfile.is_open())
    {
      return -1;
    }

    outfile << "1000 1000 1000 0.1";

    return 0;
  }

  // -----------------------------------------------------------------------------
  int32_t CreateCSDGMFile_6()
  {
    std::ofstream outfile(k_CSDGMTestFile, std::ios_base::out);
    if(!outfile.is_open())
    {
      return -1;
    }

    // Indexes 0 - 9
    outfile << "-18.7383 -18.7383 0.063 1" << k_NL;
    outfile << "-18.5026 -18.7383 0.063 2" << k_NL;
    outfile << "-18.2669 -18.7383 0.063 3" << k_NL;
    outfile << "-18.0312 -18.7383 0.063 4" << k_NL;
    outfile << "-17.7955 -18.7383 0.063 5" << k_NL;
    outfile << "-17.5598 -18.7383 0.063 6" << k_NL;
    outfile << "-17.3241 -18.7383 0.063 7" << k_NL;
    outfile << "-17.0884 -18.7383 0.063 8" << k_NL;
    outfile << "-16.8527 -18.7383 0.063 9" << k_NL;
    outfile << "-16.617 -18.7383 0.063 10" << k_NL;

    // Indexes 256,868 - 256,877
    outfile << "4.3605 11.1959 0.635 11" << k_NL;
    outfile << "4.5962 11.1959 0.635 12" << k_NL;
    outfile << "4.8319 11.1959 0.635 13" << k_NL;
    outfile << "5.06761 11.1959 0.635 14" << k_NL;
    outfile << "5.30331 11.1959 0.635 15" << k_NL;
    outfile << "5.53901 11.1959 0.635 16" << k_NL;
    outfile << "5.77471 11.1959 0.635 17" << k_NL;
    outfile << "6.01041 11.1959 0.635 18" << k_NL;
    outfile << "6.24612 11.1959 0.635 19" << k_NL;
    outfile << "6.48182 11.1959 0.635 20" << k_NL;

    // Indexes 560,650 - 560,659
    outfile << "11.903 -5.06759 1.397 21" << k_NL;
    outfile << "12.1387 -5.06759 1.397 22" << k_NL;
    outfile << "12.3744 -5.06759 1.397 23" << k_NL;
    outfile << "12.6101 -5.06759 1.397 24" << k_NL;
    outfile << "12.8458 -5.06759 1.397 25" << k_NL;
    outfile << "13.0815 -5.06759 1.397 26" << k_NL;
    outfile << "13.3172 -5.06759 1.397 27" << k_NL;
    outfile << "13.5529 -5.06759 1.397 28" << k_NL;
    outfile << "13.7886 -5.06759 1.397 29" << k_NL;
    outfile << "14.0243 -5.06759 1.397 30" << k_NL;

    // Indexes 813,554 - 813,563
    outfile << "17.0884 19.2098 1.9685 31" << k_NL;
    outfile << "17.3241 19.2098 1.9685 32" << k_NL;
    outfile << "17.5598 19.2098 1.9685 33" << k_NL;
    outfile << "17.7955 19.2098 1.9685 34" << k_NL;
    outfile << "18.0312 19.2098 1.9685 35" << k_NL;
    outfile << "18.2669 19.2098 1.9685 36" << k_NL;
    outfile << "18.5026 19.2098 1.9685 37" << k_NL;
    outfile << "18.7383 19.2098 1.9685 38" << k_NL;
    outfile << "18.974 19.2098 1.9685 39" << k_NL;
    outfile << "19.2098 19.2098 1.9685 40" << k_NL;

    return 0;
  }

  // -----------------------------------------------------------------------------
  // Tests
  // -----------------------------------------------------------------------------
  int MissingOrIncorrectInputFilesTest()
  {
    ImportDelamData::Pointer filter = ImportDelamData::New();

    // Empty CSDGM file test
    filter->preflight();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -2000)

    // Wrong CSDGM file extension test
    filter->setCSDGMFile(QString::fromStdString(k_WrongCSDGMTestFile));
    filter->preflight();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -2001)

    // Empty Bvid StdOut file test
    filter->setCSDGMFile(QString::fromStdString(k_CSDGMTestFile));
    filter->preflight();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -2002)

    // Wrong Bvid StdOut file extension test
    filter->setBvidStdOutFile(QString::fromStdString(k_WrongBvidStdOutTestFile));
    filter->preflight();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -2003)

    filter->setBvidStdOutFile(QString::fromStdString(k_BvidStdOutTestFile));
    int32_t result = CreateBvidStdOutFile_0();
    DREAM3D_REQUIRED(result, >=, 0)
    filter->preflight();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -2010)

    result = CreateBvidStdOutFile_1();
    DREAM3D_REQUIRED(result, >=, 0)
    filter->preflight();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -2005)

    result = CreateBvidStdOutFile_2();
    DREAM3D_REQUIRED(result, >=, 0)
    filter->preflight();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -2011)

    result = CreateBvidStdOutFile_3();
    DREAM3D_REQUIRED(result, >=, 0)
    filter->preflight();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -2006)

    result = CreateBvidStdOutFile_4();
    DREAM3D_REQUIRED(result, >=, 0)
    filter->preflight();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -2012)

    result = CreateBvidStdOutFile_5();
    DREAM3D_REQUIRED(result, >=, 0)
    filter->preflight();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -2007)

    result = CreateBvidStdOutFile_6();
    DREAM3D_REQUIRED(result, >=, 0)
    filter->preflight();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -2013)

    result = CreateBvidStdOutFile_7();
    DREAM3D_REQUIRED(result, >=, 0)
    filter->preflight();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -2008)

    result = CreateBvidStdOutFile_8();
    DREAM3D_REQUIRED(result, >=, 0)
    filter->preflight();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -2014)

    result = CreateBvidStdOutFile_9();
    DREAM3D_REQUIRED(result, >=, 0)
    filter->preflight();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -2009)

    result = CreateBvidStdOutFile_10();
    DREAM3D_REQUIRED(result, >=, 0)
    result = CreateCSDGMFile_0();
    DREAM3D_REQUIRED(result, >=, 0)
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -2017)

    result = CreateCSDGMFile_1();
    DREAM3D_REQUIRED(result, >=, 0)
    filter->setDataContainerArray(DataContainerArray::New());
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -2018)

    result = CreateCSDGMFile_2();
    DREAM3D_REQUIRED(result, >=, 0)
    filter->setDataContainerArray(DataContainerArray::New());
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -2019)

    result = CreateCSDGMFile_3();
    DREAM3D_REQUIRED(result, >=, 0)
    filter->setDataContainerArray(DataContainerArray::New());
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -2020)

    result = CreateCSDGMFile_4();
    DREAM3D_REQUIRED(result, >=, 0)
    filter->setDataContainerArray(DataContainerArray::New());
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -2021)

    result = CreateCSDGMFile_5();
    DREAM3D_REQUIRED(result, >=, 0)
    filter->setDataContainerArray(DataContainerArray::New());
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), -2022)

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  int CorrectImportTest()
  {
    ImportDelamData::Pointer filter = ImportDelamData::New();
    filter->setCSDGMFile(QString::fromStdString(k_CSDGMTestFile));
    filter->setBvidStdOutFile(QString::fromStdString(k_BvidStdOutTestFile));
    filter->setInterfaceThickness(0.01);

    int32_t result = CreateBvidStdOutFile_10();
    DREAM3D_REQUIRED(result, >=, 0)
    result = CreateCSDGMFile_6();
    DREAM3D_REQUIRED(result, >=, 0)
    filter->execute();
    DREAM3D_REQUIRE_EQUAL(filter->getErrorCode(), 0)

    DataContainerArray::Pointer dca = filter->getDataContainerArray();
    DREAM3D_REQUIRE_VALID_POINTER(dca)
    DataContainer::Pointer dc = dca->getDataContainer(filter->getDataContainerPath());
    DREAM3D_REQUIRE_VALID_POINTER(dc)
    AttributeMatrix::Pointer am = dc->getAttributeMatrix(filter->getCellAttributeMatrixName());
    DREAM3D_REQUIRE_VALID_POINTER(am)
    FloatArrayType::Pointer da = am->getAttributeArrayAs<FloatArrayType>(filter->getDataArrayName());
    DREAM3D_REQUIRE_VALID_POINTER(da)

    DREAM3D_REQUIRE_EQUAL(da->getValue(0), 1.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(1), 2.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(2), 3.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(3), 4.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(4), 5.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(5), 6.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(6), 7.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(7), 8.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(8), 9.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(9), 10.0f)

    DREAM3D_REQUIRE_EQUAL(da->getValue(256868), 11.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(256869), 12.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(256870), 13.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(256871), 14.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(256872), 15.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(256873), 16.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(256874), 17.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(256875), 18.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(256876), 19.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(256877), 20.0f)

    DREAM3D_REQUIRE_EQUAL(da->getValue(560650), 21.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(560651), 22.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(560652), 23.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(560653), 24.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(560654), 25.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(560655), 26.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(560656), 27.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(560657), 28.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(560658), 29.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(560659), 30.0f)

    DREAM3D_REQUIRE_EQUAL(da->getValue(813554), 31.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(813555), 32.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(813556), 33.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(813557), 34.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(813558), 35.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(813559), 36.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(813560), 37.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(813561), 38.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(813562), 39.0f)
    DREAM3D_REQUIRE_EQUAL(da->getValue(813563), 40.0f)

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(MissingOrIncorrectInputFilesTest())
    DREAM3D_REGISTER_TEST(CorrectImportTest())
  }
};

