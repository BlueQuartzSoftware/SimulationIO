// -----------------------------------------------------------------------------
// Insert your license & copyright information here
// -----------------------------------------------------------------------------

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"

#include "SIMPLib/Filtering/QMetaObjectUtilities.h"

#include "UnitTestSupport.hpp"

#include "SimulationIOTestFileLocations.h"

class Export3dSolidMeshTest
{

  public:
    Export3dSolidMeshTest() {}
    virtual ~Export3dSolidMeshTest() {}


  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
  #if REMOVE_TEST_FILES
    QFile::remove(UnitTest::Export3dSolidMeshTest::TestFile1);
    QFile::remove(UnitTest::Export3dSolidMeshTest::TestFile2);
  #endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the Export3dSolidMeshTest Filter from the FilterManager
    QString filtName = "Export3dSolidMesh";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if (nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The SimulationIO Requires the use of the " << filtName.toStdString() << " filter which is found in the SimulationIO Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestExport3dSolidMeshTest()
  {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   /* Please write Export3dSolidMeshTest test code here.
    *
    * Your IO test files are:
    * UnitTest::Export3dSolidMeshTest::TestFile1
    * UnitTest::Export3dSolidMeshTest::TestFile2
    *
    * SIMPLib provides some macros that will throw exceptions when a test fails
    * and thus report that during testing. These macros are located in the
    * SIMPLib/Utilities/UnitTestSupport.hpp file. Some examples are:
    *
    * SIMPLib_REQUIRE_EQUAL(foo, 0)
    * This means that if the variable foo is NOT equal to Zero then test will fail
    * and the current test will exit immediately. If there are more tests registered
    * with the SIMPLib_REGISTER_TEST() macro, the next test will execute. There are
    * lots of examples in the SIMPLib/Test folder to look at.
    */
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    int foo = -1;
    DREAM3D_REQUIRE_EQUAL(foo, 0)

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST( TestFilterAvailability() );

    DREAM3D_REGISTER_TEST( TestExport3dSolidMeshTest() )

    DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  }

  private:
    Export3dSolidMeshTest(const Export3dSolidMeshTest&); // Copy Constructor Not Implemented
    void operator=(const Export3dSolidMeshTest&); // Operator '=' Not Implemented


};

