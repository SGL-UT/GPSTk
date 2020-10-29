//==============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

#include "TestUtil.hpp"
#include "ObsClockModel.hpp"

using namespace std;
using namespace gpstk;

//============================================================
// Declarations for test class
//============================================================

namespace gpstk
{
  class ObsClockModel_Child : public ObsClockModel
  {
    public:
      virtual void addEpoch(const ORDEpoch& re) {}
      virtual double getOffset(const gpstk::CommonTime& t) const {return(0.0);}
      virtual bool isOffsetValid(const gpstk::CommonTime& t) const {return false;}
  };
};


class ObsClockModel_T
{
    public: 
        ObsClockModel_T(){};
        ~ObsClockModel_T(){};
        int test_constructor();
        int test_constructor_defaults();
        int test_getSvMode();

};

//============================================================
// Definitions for test class
//============================================================


//----------------------------------------
// Constructor tests
//----------------------------------------

int ObsClockModel_T :: test_constructor( void )
{
    TestUtil test_util( "ObsClockModel", "constructor", __FILE__, __LINE__ );
    std::string test_desc = "ObsClockModel objects are created with default constructor";
    std::string test_fail = "class constructor failed";

    try
    {
      ObsClockModel_Child testObsClockModel;
        test_util.assert( true, "We have successfully constructed an ObsClockModel", __LINE__ );
    }
    catch(...)
    {
        test_util.assert( false, "constructor threw an exception", __LINE__ );
    }

    return( test_util.countFails() );
}


int ObsClockModel_T :: test_constructor_defaults( void )
{
    TestUtil test_util( "ObsClockModel", "constructor defaults", __FILE__, __LINE__ );
    std::string test_desc = "ObsClockModel constructor with default values";
    std::string test_fail = "class constructor failed";
    

    try
    {
        ObsClockModel_Child testObsClockModel;
        SatID testSatID( 1, SatelliteSystem::GPS );

        test_util.assert( testObsClockModel.getSigmaMultiplier() == 2.0 , "Default sigma value is not as expected", __LINE__ );
        test_util.assert( testObsClockModel.getElevationMask() == 0.0 , "Default elmask value is not as expected", __LINE__ );
        test_util.assert( testObsClockModel.getSvMode( testSatID ) == ObsClockModel::ALWAYS , "Default SvMode value is not as expected", __LINE__ );
        test_util.assert( testObsClockModel.getUseWonkyData() == false , "Default useWonkyData value is not as expected", __LINE__ );

        test_util.assert( true, "We have successfully tested default constructor values for ObsClockModel", __LINE__ );
    }
    catch(...)
    {
        test_util.assert( false, "constructor threw an exception while testing default data member values", __LINE__ );
    }

    return( test_util.countFails() );
}

//----------------------------------------
// Mode test
//----------------------------------------

int ObsClockModel_T :: test_getSvMode( void )
{
    TestUtil test_util( "ObsClockModel", "getSvMode", __FILE__, __LINE__ );
    std::string test_exception_msg = "test threw an unexpected exception";
    std::string test_fail_msg = "get SvMode value returned did not match set SvMode value expected";

    try
    {
        ObsClockModel_Child testObsClockModel;

        // create the SvMode that you will pass into setSvMode, then pass it in.
        ObsClockModel::SvMode expectedSvMode = ObsClockModel::ALWAYS;
        const ObsClockModel& dummyModel = testObsClockModel.setSvMode( expectedSvMode );

        // need at least one SatId in order to extract the SvMode
        SatID testSatID( 1, SatelliteSystem::GPS );
        ObsClockModel::SvMode returnedSvMode = testObsClockModel.getSvMode( testSatID );

        test_util.assert( returnedSvMode == expectedSvMode, test_fail_msg, __LINE__ );
    }
    catch(...)
    {
        test_util.assert( false, test_exception_msg, __LINE__ );
    }

    return( test_util.countFails() );
}



//============================================================
// main() for test application to be executed by CMake/CTest
//============================================================

int main( )
{
   int  errorTotal = 0;

   ObsClockModel_T  testClass;

   errorTotal += testClass.test_constructor();
   errorTotal += testClass.test_constructor_defaults();
   errorTotal += testClass.test_getSvMode();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return( errorTotal );
}

//============================================================
// The End
//============================================================
