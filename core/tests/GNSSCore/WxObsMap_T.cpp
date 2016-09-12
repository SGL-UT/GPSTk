//============================================================================
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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

#include "WxObsMap.hpp" 

#include "TestUtil.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

class WxObsMap_T
{
public: 
   WxObsMap_T(){ singlePrecisionError = 1E-7; }// Default Constructor, set the precision value for single precision. Note WxObsMap stores floating point values in single precision.
   ~WxObsMap_T() {} // Default Desructor

//================================================================
//	WxObsObservations

   int observationsTest(void)
   {
      gpstk::TestUtil testFramework("WxObsMap", "Default Constructor", __FILE__, __LINE__);
      std::string failMesg;

      gpstk::WxObservation Compare;

      failMesg = "Was the time value set correctly?";
      testFramework.assert(Compare.t == gpstk::CommonTime::END_OF_TIME, failMesg, __LINE__);

      failMesg = "Was the temperature source set correctly?";
      testFramework.assert(Compare.temperatureSource == gpstk::WxObservation::noWx, failMesg, __LINE__);

      failMesg = "Was the pressure source set correctly?";
      testFramework.assert(Compare.pressureSource == gpstk::WxObservation::noWx, failMesg, __LINE__);

      failMesg = "Was the humidity source set correctly?";
      testFramework.assert(Compare.humiditySource == gpstk::WxObservation::noWx, failMesg, __LINE__);

      gpstk::CommonTime cTime;
      cTime.set(500005,6,.7);

      gpstk::WxObservation Compare1(cTime, 100, .5f, .8f);

      testFramework.changeSourceMethod("Explicit Constructor");

      failMesg = "Was the time value set correctly?";
      testFramework.assert(Compare1.t == cTime, failMesg, __LINE__);

      failMesg = "Was the temperature value set correctly?";
      testFramework.assert(std::abs(Compare1.temperature - 100) < singlePrecisionError, failMesg, __LINE__);

      failMesg = "Was the pressure value set correctly?";
      testFramework.assert(std::abs(Compare1.pressure - 0.5) < singlePrecisionError, failMesg, __LINE__);

      failMesg = "Was the humidity value set correctly?";
      testFramework.assert(std::abs(Compare1.humidity - 0.8) < singlePrecisionError, failMesg, __LINE__);

      failMesg = "Was the temperature source set correctly?";
      testFramework.assert(Compare1.temperatureSource == gpstk::WxObservation::obsWx, failMesg, __LINE__);

      failMesg = "Was the temperature source set correctly?";
      testFramework.assert(Compare1.pressureSource == gpstk::WxObservation::obsWx, failMesg, __LINE__);

      failMesg = "Was the temperature source set correctly?";
      testFramework.assert(Compare1.humiditySource == gpstk::WxObservation::obsWx, failMesg, __LINE__);

      testFramework.changeSourceMethod("isAllValid");

      failMesg = "Does the isAllValid method function properly?";
      testFramework.assert(Compare1.isAllValid(), failMesg, __LINE__);

      testFramework.changeSourceMethod("<< Operator");

      std::string outputString, referenceString;
      std::stringstream outputStream, referenceStream;
      outputStream << Compare1;
      outputString = outputStream.str();
      referenceStream << cTime << ", t=" << 100 << ", p=" << 0.5 << ", rh=" << 0.8;
      referenceString =  referenceStream.str();

      failMesg = "Does the << operator function properly?";
      testFramework.assert(referenceString == outputString, failMesg, __LINE__);

      return testFramework.countFails();
   }
//================================================================



//================================================================
//	WxObsMap

//		Only tests can do here would verify that a map works
//		with CommonTime and WxObservation objects. Implicitly
//		tested in WxObsData, don't need explicit code for now

//================================================================



//================================================================
//	WxObsData

   int WxObsDataTest(void)
   {
      gpstk::TestUtil testFramework("WxObsMap", "Default Constructor", __FILE__, __LINE__);
      std::string failMesg;

      gpstk::WxObsData Compare;

      failMesg = "Was the default value stored?";
      testFramework.assert(Compare.firstTime == gpstk::CommonTime::END_OF_TIME, failMesg, __LINE__);

      failMesg = "Was the default value stored?";
      testFramework.assert(Compare.lastTime == gpstk::CommonTime::BEGINNING_OF_TIME, failMesg, __LINE__);

      testFramework.changeSourceMethod("insertObservation");

      gpstk::CommonTime cTime;
      cTime.set((double)1000);

      for (int i = 0; i < 10; i++)
      {
         cTime.set((double)1000+i);
         Compare.insertObservation(gpstk::WxObservation(cTime,
                                                        100+i, (50+i)*0.001, (80+i)*0.001));
      }

         //scale time counter back to access a stored observation
      cTime.set((double)1008);

      failMesg = "Was the WxObservation object stored in the map?";
      testFramework.assert(Compare.obs.count(cTime), failMesg, __LINE__);


      testFramework.changeSourceMethod("getMostRecent");

      gpstk::WxObservation obs1 = Compare.getMostRecent(cTime);	
      gpstk::WxObservation obs2(cTime, 108, 0.058f, 0.088f);

         //std::cout<<obs2<<std::endl<<obs1<<std::endl;
		
         //std::cout<<Compare.obs[cTime]<<std::endl;


      failMesg = "Did the getMostRecent method function correctly?";

         //should be able to do obs1 == obs2, but b/c theres no == operator...
      testFramework.assert(obs2.t == obs1.t && 
                           obs2.temperature == obs1.temperature &&
                           obs2.pressure == obs1.pressure &&
                           obs2.humidity == obs1.humidity, failMesg, __LINE__);

         //verifying objects exist before deleting them
      cTime.set((double) 1001);
      failMesg = "Is the WxObservation object stored?	";
      testFramework.assert(Compare.obs.count(cTime), failMesg, __LINE__);
      cTime.set((double) 1002);
      failMesg = "Is the WxObservation object stored?		";
      testFramework.assert(Compare.obs.count(cTime), failMesg, __LINE__);

      cTime.set((double) 1003);
         //should erase 1001 and 1002 entries
      Compare.flush(cTime);

      cTime.set((double) 1001);
      failMesg = "Was the WxObservation object erased?		";
      testFramework.assert(!Compare.obs.count(cTime), failMesg, __LINE__);
      cTime.set((double) 1002);
      failMesg = "Was the WxObservation object erased?		";
      testFramework.assert(!Compare.obs.count(cTime), failMesg, __LINE__);

      testFramework.changeSourceMethod("getWxObservation");

      failMesg = "[testing] WxObs.getWxObservation with a non-existent object, [expected] exception gpstk::Exception, [actual] threw no exception";
      try{ Compare.getWxObservation(cTime, 0, false); testFramework.assert(false, failMesg, __LINE__); }
      catch (gpstk::Exception e) { testFramework.assert(true, failMesg, __LINE__); }

         //return right object for explicit case
      cTime.set((double)1003);
      gpstk::WxObservation obs3 = Compare.getWxObservation(cTime, 0, false);
      gpstk::WxObservation obs4(cTime, 103, 0.053f, 0.083f);

      failMesg = "Did the getWxObservation return the correct object when called explicitly?	";
      testFramework.assert(obs3.t == obs3.t && 
                           obs3.temperature == obs4.temperature &&
                           obs3.pressure == obs4.pressure &&
                           obs3.humidity == obs4.humidity, failMesg, __LINE__);

         //return right object within range when theres only one
      cTime.set((double)1009);
      gpstk::WxObservation obs6(cTime, 109, 0.059f, 0.089f);
      cTime.set((double)1010);		
      gpstk::WxObservation obs5 = Compare.getWxObservation(cTime, 86401, false);

      failMesg = "Did the getWxObservation return the correct object when called in a range containing 1 object?";
      testFramework.assert(obs5.t == obs6.t && 
                           obs5.temperature == obs6.temperature &&
                           obs5.pressure == obs6.pressure &&
                           obs5.humidity == obs6.humidity, failMesg, __LINE__);

         //return right object within range when theres multiple
      cTime.set((double)1006);
      gpstk::WxObservation obs8(cTime, 106, 0.056f, 0.086f);	
      gpstk::WxObservation obs7 = Compare.getWxObservation(cTime, 86401, false);

      failMesg = "Did the getWxObservation return the correct object when called in a range containing 2 objects?";
      testFramework.assert(obs7.t == obs8.t && 
                           obs7.temperature == obs8.temperature &&
                           obs7.pressure == obs8.pressure &&
                           obs7.humidity == obs8.humidity, failMesg, __LINE__);

         //Linear Interpolation
      cTime.set((long)1007, (double)43200);
         //should have two values in range, where counter is 7 and 8
      gpstk::WxObservation obs9 = Compare.getWxObservation(cTime, 43200, true);
         //create object with what the interpolated values should be at the given time
      gpstk::WxObservation obs10(cTime, 107.5f, 0.0575f, 0.0875f);

      failMesg = "Did the getWxObservation perform linear interpolation succesfully?";
      testFramework.assert(std::abs(obs9.t - obs10.t) < singlePrecisionError && 
                           std::abs(obs9.temperature - obs10.temperature) < singlePrecisionError&&
                           std::abs(obs9.pressure - obs10.pressure) < singlePrecisionError &&
                           std::abs(obs9.humidity - obs10.humidity) < singlePrecisionError, failMesg, __LINE__);
		
      return testFramework.countFails();
   }

//================================================================	

private:
   double singlePrecisionError;

};


int main() //Main function to initialize and run all tests above
{
	WxObsMap_T testClass;
	int check = 0, errorCounter = 0;

	check = testClass.observationsTest();
	errorCounter += check;

	check = testClass.WxObsDataTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; 	
}
