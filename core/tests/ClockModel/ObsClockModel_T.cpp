

#include "TestUtil.hpp"
#include "ObsClockModel.hpp"

using namespace std;
using namespace gpstk;

//------------------------------------------------------------
// Declarations for test class
//------------------------------------------------------------
class ObsClockModel_T
{
    public: 
        ObsClockModel_T(){}
        ~ObsClockModel_T(){}
         int test_constructor();
         int test_getSvStatus();
         int test_setSvStatus();
         int test_setSvMap();
         int test_getSvMode();
         int test_simpleOrdClock();
};
//------------------------------------------------------------
// Definitions for test class
//------------------------------------------------------------

int ObsClockModel_T :: test_constructor( void )
{
    TestUtil test_util( "ObsClockModel", "constructor", __FILE__, __LINE__ );
    std::string test_desc = "ObsClockModel objects are created with default constructor";
    std::string test_fail = "class constructor failed";

    // do stuff

    return( test_util.countFails() );
}


int ObsClockModel_T :: test_getSvStatus( void )
{
    TestUtil test_util( "ObsClockModel", "getSvStatus", __FILE__, __LINE__ );
    std::string test_desc = "ObsClockModel method getSvStatus is tested here";
    std::string test_fail = "test failed";

    // do stuff

    return( test_util.countFails() );
}


int ObsClockModel_T :: test_setSvStatus( void )
{
    TestUtil test_util( "ObsClockModel", "setSvStatus", __FILE__, __LINE__ );
    std::string test_desc = "ObsClockModel method setSvStatus is tested here";
    std::string test_fail = "test failed";

    // do stuff

    return( test_util.countFails() );
}


int ObsClockModel_T :: test_setSvMap( void )
{
    TestUtil test_util( "ObsClockModel", "setSvMap", __FILE__, __LINE__ );
    std::string test_desc = "ObsClockModel method setSvMap is tested here";
    std::string test_fail = "test failed";

    // do stuff

    return( test_util.countFails() );
}


int ObsClockModel_T :: test_getSvMode( void )
{
    TestUtil test_util( "ObsClockModel", "getSvMode", __FILE__, __LINE__ );
    std::string test_desc = "ObsClockModel method getSvMode is tested here";
    std::string test_fail = "test failed";

    // do stuff

    return( test_util.countFails() );
}


int ObsClockModel_T :: test_simpleOrdClock( void )
{
    TestUtil test_util( "ObsClockModel", "simpleOrdClock", __FILE__, __LINE__ );
    std::string test_desc = "ObsClockModel method simpleOrdClock is tested here";
    std::string test_fail = "test failed";

    // do stuff

    return( test_util.countFails() );
}




//------------------------------------------------------------
// main() for test application to be executed by CMake/CTest
//------------------------------------------------------------
int main( )
{
   int  errorTotal = 0;

   ObsClockModel_T  testClass;

   errorTotal += testClass.test_constructor();
   errorTotal += testClass.test_getSvStatus();
   errorTotal += testClass.test_setSvStatus();
   errorTotal += testClass.test_setSvMap();
   errorTotal += testClass.test_getSvMode();
   errorTotal += testClass.test_simpleOrdClock();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return( errorTotal );
}
