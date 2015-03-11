#include "AlmOrbit.hpp"
#include "Xvt.hpp"
#include "GPSWeekSecond.hpp"
#include "CommonTime.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <sstream>

class AlmOrbit_T : public gpstk::AlmOrbit
{
    public: 
	AlmOrbit_T(){ eps == 1E-12;}// Default Constructor, set the precision value
	~AlmOrbit_T() {} // Default Desructor

//=================================================================================
//	Test will check the initialization of AlmOrbit objects
//=================================================================================
	int initializationTest(void)
	{
		TestUtil testFramework("AlmOrbit", "Default Constructor", __FILE__, __LINE__);
		std::string testMesg;

		gpstk::AlmOrbit empty;

		//------------------------------------------------------------------------
		//Does the default constructor function correctly?
		//------------------------------------------------------------------------
		testMesg = "PRN not initialized to 0";
		testFramework.assert(AlmOrbit::PRN == 0, testMesg, __LINE__);
		testMesg = "Ecc not initialized to 0";
		testFramework.assert(fabs(AlmOrbit::ecc) < eps, testMesg, __LINE__);
		testMesg = "Offset not initialized to 0";
		testFramework.assert(fabs(AlmOrbit::i_offset) < eps, testMesg, __LINE__);
		testMesg = "OMEGAdot not initialized to 0";
		testFramework.assert(fabs(AlmOrbit::OMEGAdot) < eps, testMesg, __LINE__); 
		testMesg = "Ahalf not initialized to 0";
		testFramework.assert(fabs(AlmOrbit::Ahalf) < eps, testMesg, __LINE__);
		testMesg = "OMEGA0 not initialized to 0";
		testFramework.assert(fabs(AlmOrbit::OMEGA0) < eps, testMesg, __LINE__);
		testMesg = "W not initialized to 0";
		testFramework.assert(fabs(AlmOrbit::w) < eps, testMesg, __LINE__);
		testMesg = "M0 not initialized to 0";
		testFramework.assert(fabs(AlmOrbit::M0) < eps, testMesg, __LINE__);
		testMesg = "AF0 not initialized to 0";
		testFramework.assert(fabs(AlmOrbit::AF0) < eps, testMesg, __LINE__);
		testMesg = "AF1 not initialized to 0";
		testFramework.assert(fabs(AlmOrbit::AF1) < eps, testMesg, __LINE__);
		testMesg = "Toa not initialized to 0";
		testFramework.assert(AlmOrbit::Toa == 0, testMesg, __LINE__);
		testMesg = "Xmit_time not initialized to 0";
		testFramework.assert(AlmOrbit::xmit_time == 0, testMesg, __LINE__);
		testMesg = "Week not initialized to 0";
		testFramework.assert(AlmOrbit::week == 0, testMesg, __LINE__);
		testMesg = "SV_health not initialized to 0";
		testFramework.assert(AlmOrbit::SV_health == 0, testMesg, __LINE__);

//============================================================================

//		Should be tested by testing the inhereted members as comparison points
//		instead of using the get methods, but for the life of me I can't figure
//		out why AlmOrbit::week/Compare.PRN don't work

//============================================================================

		gpstk::AlmOrbit Compare(1, 0.00346661, 0.00388718, -8.01176E-09, 5153.58,
								-0.296182, -1.31888, 2.79387, 0.000148773,
								7.63976E-11, 466944, 250560, 797, 0);

		testFramework.changeSourceMethod("Explicit Constructor");
		//----------------------------------------------------------------
		//Does the explicit constructor function correctly?
		//----------------------------------------------------------------
		testMesg = "PRN value was not initialized correctly";
		testFramework.assert(Compare.getPRN() == 1, testMesg, __LINE__);
		testMesg = "Ecc value was not initialized correctly";
		testFramework.assert(fabs(Compare.getecc() - 0.00346661) < eps, testMesg, __LINE__);
		testMesg = "Offset value was not initialized correctly";
		testFramework.assert(fabs(Compare.geti_offset() - 0.00388718) < eps, testMesg, __LINE__);
		testMesg = "OMEGAdot value was not initialized correctly";
		testFramework.assert(fabs(Compare.getOMEGAdot() - -8.01176E-09) < eps, testMesg, __LINE__); 
		testMesg = "Ahalf value was not initialized correctly";
		testFramework.assert(fabs(Compare.getAhalf() - 5153.58) < eps, testMesg, __LINE__);
		testMesg = "OMEGA0 value was not initialized correctly";
		testFramework.assert(fabs(Compare.getOMEGA0() - -0.296182) < eps, testMesg, __LINE__);
		testMesg = "W value was not initialized correctly";
		testFramework.assert(fabs(Compare.getw() - -1.31888) < eps, testMesg, __LINE__);
		testMesg = "M0 value was not initialized correctly";
		testFramework.assert(fabs(Compare.getM0() - 2.79387) < eps, testMesg, __LINE__);
		testMesg = "AF0 value was not initialized correctly";
		testFramework.assert(fabs(Compare.getAF0() - 0.000148773) < eps, testMesg, __LINE__);
		testMesg = "AF1 value was not initialized correctly";
		testFramework.assert(fabs(Compare.getAF1() - 7.63976E-11) < eps, testMesg, __LINE__);
		testMesg = "Toa value was not initialized correctly";
		testFramework.assert(Compare.getToaSOW() == 466944, testMesg, __LINE__);
		testMesg = "Xmit_time value was not initialized correctly";
		testFramework.assert(Compare.getxmit_time() == 250560, testMesg, __LINE__);
		testMesg = "Week value was not initialized correctly";
		testFramework.assert(Compare.getToaWeek() == 797, testMesg, __LINE__);
		testMesg = "SV_health value was not initialized correctly";
		testFramework.assert(AlmOrbit::SV_health == 0, testMesg, __LINE__);

		return testFramework.countFails();
	
	}
//=================================================================================
//	Test will check the dump method for various verbosities
//=================================================================================
	int dumpTest(void)
	{
		TestUtil testFramework("AlmOrbit", "Dump", __FILE__, __LINE__);
		std::string testMesg;

		gpstk::AlmOrbit Compare(1, 0.00346661, 0.00388718, -8.01176E-09, 5153.58,
								-0.296182, -1.31888, 2.79387, 0.000148773,
								7.63976E-11, 466944, 250560, 797, 0);


		std::stringstream outputStream1, outputStream2, outputStream3;
		std::string outputString1, referenceString1, outputString2, referenceString2,
					outputString3, referenceString3;

		referenceString1 = "1, 466944, 797, 0, 1.4877e-04, 7.6398e-11, 3.4666e-03, "
							"-1.3189e+00, 5.1536e+03, 2.7939e+00, -2.9618e-01, "
							"-8.0118e-09, 3.8872e-03\n";
		Compare.dump(outputStream1, 0);
		outputString1 = outputStream1.str();

		//-----------------------------------------------------------------
		//Did the least verbose dump method function correctly?		
		//-----------------------------------------------------------------
		testMesg = "Least-verbose dump method did not function correctly";
		testFramework.assert(referenceString1 == outputString1, testMesg, __LINE__);


		referenceString2 = "PRN:1 Toa:466944 H:0 AFO:1.4877e-04 AF1:7.6398e-11 "
							"Ecc:3.4666e-03\n   w:-1.3189e+00 Ahalf:5.1536e+03 "
							"M0:2.7939e+00\n   OMEGA0:-2.9618e-01 "
							"OMEGAdot:-8.0118e-09 Ioff:3.8872e-03\n";
		Compare.dump(outputStream2, 1);
		outputString2 = outputStream2.str();

		//-----------------------------------------------------------------
		//Did the mid-level verbose dump method function correctly?
		//-----------------------------------------------------------------
		testMesg = "Medium-verbose dump method did not function correctly";
		testFramework.assert(referenceString2 == outputString2, testMesg, __LINE__);


		referenceString3 = "PRN:                   1\n"
						   "Toa:                   466944\n"
						   "xmit_time:             250560\n"
						   "week:                  797\n"
						   "SV_health:             0\n"
						   "AFO:                     1.4877e-04 sec\n"
						   "AF1:                     7.6398e-11 sec/sec\n"
						   "Sqrt A:                  5.1536e+03 sqrt meters\n"
						   "Eccentricity:            3.4666e-03\n"
						   "Arg of perigee:         -1.3189e+00 rad\n"
						   "Mean anomaly at epoch:   2.7939e+00 rad\n"
						   "Right ascension:        -2.9618e-01 rad         -8.0118e-09 rad/sec\n"
						   "Inclination offset:      3.8872e-03 rad    \n";
		Compare.dump(outputStream3, 2);
		outputString3 = outputStream3.str();

		//-----------------------------------------------------------------
		//Did the most verbose dump method function correctly?
		//-----------------------------------------------------------------
		testMesg = "High-verbose dump method did not function correctly";
		testFramework.assert(outputString3 == referenceString3, testMesg, __LINE__);

		return testFramework.countFails();
	
	}
//=================================================================================
//	Test will check the various operators
//=================================================================================
	int operatorTest(void)
	{
		TestUtil testFramework("AlmOrbit", "<< Operator", __FILE__, __LINE__);
		std::string testMesg;

		std::stringstream outputStream;
		std::string outputString, referenceString;

		gpstk::AlmOrbit Compare(1, 0.00346661, 0.00388718, -8.01176E-09, 5153.58,
								-0.296182, -1.31888, 2.79387, 0.000148773,
								7.63976E-11, 466944, 250560, 797, 0);

		referenceString = "PRN:1 Toa:466944 H:0 AFO:1.4877e-04 AF1:7.6398e-11 "
							"Ecc:3.4666e-03\n   w:-1.3189e+00 Ahalf:5.1536e+03 "
							"M0:2.7939e+00\n   OMEGA0:-2.9618e-01 "
							"OMEGAdot:-8.0118e-09 Ioff:3.8872e-03\n";

		outputStream << Compare;
		outputString = outputStream.str();

		
		//-----------------------------------------------------------------
		//Did the << operator function correctly?
		//-----------------------------------------------------------------
		testMesg = "The redirection operator << did not function correctly";
		testFramework.assert(outputString == referenceString, testMesg, __LINE__);

		return testFramework.countFails();
	
	}
//=================================================================================
//	Test will check the various get methods
//=================================================================================
	int getTest(void)
	{
		TestUtil testFramework("AlmOrbit", "get Methods", __FILE__, __LINE__);
		std::string testMesg;

		gpstk::AlmOrbit Compare(1, 0.00346661, 0.00388718, -8.01176E-09, 5153.58,
								-0.296182, -1.31888, 2.79387, 0.000148773,
								7.63976E-11, 466944, 250560, 797, 0);

		gpstk::GPSWeekSecond reference1(797,466944);
		gpstk::CommonTime cRef1 (reference1);

		//------------------------------------------------------------------
		//Did the getToaTime method function correctly?
		//------------------------------------------------------------------
		testMesg = "getToaTime method did not function correctly";
		testFramework.assert(cRef1 == Compare.getToaTime(), testMesg, __LINE__);

		gpstk::GPSWeekSecond reference2(797, 250560);
		gpstk::CommonTime cRef2 (reference2);
		//------------------------------------------------------------------
		//Did the getTransmitTime method function correctly?
		//------------------------------------------------------------------
		testMesg = "getTransmitTime method did not function correctly";
		testFramework.assert(cRef2 == Compare.getTransmitTime(), testMesg, __LINE__);

		//------------------------------------------------------------------
		//Did the getFullWeek method function correctly?
		//------------------------------------------------------------------
		testMesg = "getFullWeek method did not function correctly";
		testFramework.assert(Compare.getFullWeek() == 797, testMesg, __LINE__);

		//setting Toa to < -302400 & xmit_time to 0
		gpstk::AlmOrbit Compare1(1, 0.00346661, 0.00388718, -8.01176E-09, 5153.58,
								-0.296182, -1.31888, 2.79387, 0.000148773,
								7.63976E-11, -302401, 0, 797, 0);

		//------------------------------------------------------------------
		//Did the getFullWeek method round the week down?
		//------------------------------------------------------------------
		testMesg = "getFullWeek method did not round the week down";
		testFramework.assert(Compare1.getFullWeek() == 796, testMesg, __LINE__);

		//setting Toa to > 302400 & xmit_time to 0
		gpstk::AlmOrbit Compare2(1, 0.00346661, 0.00388718, -8.01176E-09, 5153.58,
								-0.296182, -1.31888, 2.79387, 0.000148773,
								7.63976E-11, 302401, 0, 797, 0);

		//------------------------------------------------------------------
		//Did the getFullWeek method round the week up?
		//------------------------------------------------------------------
		testMesg = "getFullWeek method did not round the week up";
		testFramework.assert(Compare2.getFullWeek() == 798, testMesg, __LINE__);

		return testFramework.countFails();
	}
//=================================================================================
//	Test will check the svXvt method
//=================================================================================
	int svXvtTest(void)
	{
		TestUtil testFramework("AlmOrbit", "svXvt", __FILE__, __LINE__);

		testFramework.assert(false, "svXvt IS UNVERIFIED! CALCULATIONS IN AlmOrbit.cpp NEED TO BE CHECKED!", __LINE__);

		return testFramework.countFails();
	}


	private:
	double eps;
};


int main() //Main function to initialize and run all tests above
{
	int check = 0, errorCounter = 0;
	AlmOrbit_T testClass;

	check = testClass.initializationTest();
	errorCounter += check;

	check = testClass.dumpTest();
	errorCounter += check;

	check = testClass.operatorTest();
	errorCounter += check;

	check = testClass.getTest();
	errorCounter += check;

	check = testClass.svXvtTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
