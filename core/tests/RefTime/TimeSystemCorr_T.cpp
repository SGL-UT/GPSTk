#include "CivilTime.hpp"
#include "CommonTime.hpp"
#include "TimeSystemCorr.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <sstream>
#include <cmath>

class TimeSystemCorr_T
{
public: 
   TimeSystemCorr_T(){ eps = 1E-12; }// Default Constructor, set the precision value
   ~TimeSystemCorr_T() {} // Default Desructor

//==========================================================================
// initializationTest ensures the constructors set the values properly
// get methods are tested implicitly
//==========================================================================
   int initializationTest(void)
   {
      gpstk::TestUtil testFramework("TimeSystemCorr", "Constructor", __FILE__, __LINE__);
      std::string testMesg = "Failed to create default object";

      gpstk::TimeSystemCorrection cUnknown;
      testFramework.assert(cUnknown.type==gpstk::TimeSystemCorrection::Unknown, testMesg, __LINE__);

      testMesg = "Default nitialization failed.";
      gpstk::TimeSystemCorrection cIRGP("IRGP");
      testFramework.assert(cIRGP.type==gpstk::TimeSystemCorrection::IRGP, testMesg, __LINE__);
      testFramework.assert(cIRGP.frTS==gpstk::TimeSystem::IRN, testMesg, __LINE__);
      testFramework.assert(cIRGP.toTS==gpstk::TimeSystem::GPS, testMesg, __LINE__);
      testFramework.assert(cIRGP.A0==0.0, testMesg, __LINE__);
      testFramework.assert(cIRGP.A1==0.0, testMesg, __LINE__);
      testFramework.assert(cIRGP.refWeek==0, testMesg, __LINE__);
      testFramework.assert(cIRGP.refSOW==0, testMesg, __LINE__);
      testFramework.assert(cIRGP.refYr==0, testMesg, __LINE__);
      testFramework.assert(cIRGP.refMon==0, testMesg, __LINE__);
      testFramework.assert(cIRGP.refDay==0, testMesg, __LINE__);
      testFramework.assert(cIRGP.geoUTCid==0, testMesg, __LINE__);

      return testFramework.countFails();
   }

      // operator==() and operator()< only exist in order to support 
      // use of TimeSystemCorr objects as map keys.  In this role, only the
      // CorrType is checked.   Therefore, it is sufficient
      // to see that the ordering is maintained without change. 
   int operatorTest(void)
   {
      gpstk::TestUtil testFramework("TimeSystemCorr", "operator<", __FILE__, __LINE__);
      std::string testMesg;

         // Build a complete set of correction objects
      //std::cout << "Constructing cUnknown" << std::endl;
      gpstk::TimeSystemCorrection cUnknown;
      gpstk::TimeSystemCorrection cGPUT("GPUT");
      gpstk::TimeSystemCorrection cGAUT("GAUT");
      gpstk::TimeSystemCorrection cSBUT("SBUT");
      gpstk::TimeSystemCorrection cGLUT("GLUT");
      gpstk::TimeSystemCorrection cGPGA("GPGA");
      gpstk::TimeSystemCorrection cGLGP("GLGP");
      gpstk::TimeSystemCorrection cQZGP("QZGP");
      gpstk::TimeSystemCorrection cQZUT("QZUT");
      gpstk::TimeSystemCorrection cBDUT("BDUT");
      gpstk::TimeSystemCorrection cBDGP("BDGP");
      gpstk::TimeSystemCorrection cIRUT("IRUT");
      gpstk::TimeSystemCorrection cIRGP("IRGP");

      testMesg = "Unequal objects returned True for operator==()";
      std::cout << "operator==() tests" << std::endl;     
      testFramework.assert( !(cUnknown==cGPUT), testMesg, __LINE__);
      testFramework.assert( !(cGPUT==cGAUT), testMesg, __LINE__);
      testFramework.assert( !(cGAUT==cSBUT), testMesg, __LINE__);
      testFramework.assert( !(cSBUT==cGLUT), testMesg, __LINE__);
      testFramework.assert( !(cGLUT==cGPGA), testMesg, __LINE__);
      testFramework.assert( !(cGPGA==cGLGP), testMesg, __LINE__);
      testFramework.assert( !(cGLGP==cQZGP), testMesg, __LINE__);
      testFramework.assert( !(cQZGP==cQZUT), testMesg, __LINE__);
      testFramework.assert( !(cQZUT==cBDUT), testMesg, __LINE__);
      testFramework.assert( !(cBDUT==cBDGP), testMesg, __LINE__);
      testFramework.assert( !(cBDGP==cIRUT), testMesg, __LINE__);
      testFramework.assert( !(cIRUT==cIRGP), testMesg, __LINE__);

      testMesg = "Greater-than object was not marked as greater by the < operator";
      std::cout << "operator<() tests" << std::endl;     
      testFramework.assert(cUnknown < cGPUT, testMesg, __LINE__);
      testFramework.assert(cGPUT < cGAUT, testMesg, __LINE__);
      testFramework.assert(cGAUT < cSBUT, testMesg, __LINE__);
      testFramework.assert(cSBUT < cGLUT, testMesg, __LINE__);
      testFramework.assert(cGLUT < cGPGA, testMesg, __LINE__);
      testFramework.assert(cGPGA < cGLGP, testMesg, __LINE__);
      testFramework.assert(cGLGP < cQZGP, testMesg, __LINE__);
      testFramework.assert(cQZGP < cQZUT, testMesg, __LINE__);
      testFramework.assert(cQZUT < cBDUT, testMesg, __LINE__);
      testFramework.assert(cBDUT < cBDGP, testMesg, __LINE__);
      testFramework.assert(cBDGP < cIRUT, testMesg, __LINE__);
      testFramework.assert(cIRUT < cIRGP, testMesg, __LINE__);

      return testFramework.countFails();
   }

/*
         Unknown=0,
         GPUT,    ///< GPS  to UTC using A0, A1
         GAUT,    ///< GAL  to UTC using A0, A1
         SBUT,    ///< SBAS to UTC using A0, A1, incl. provider and UTC ID
         GLUT,    ///< GLO  to UTC using A0 = -TauC , A1 = 0
         GPGA,    ///< GPS  to GAL using A0 = A0G   , A1 = A1G
         GLGP,    ///< GLO  to GPS using A0 = -TauGPS, A1 = 0
         QZGP,    ///< QZS  to GPS using A0, A1
         QZUT,    ///< QZS  to UTC using A0, A1
         BDUT,    ///< BDT  to UTC using A0, A1
         BDGP,    ///< BDT  to GPS using A0, A1  // not in RINEX
         IRUT,    ///< IRN  to UTC using A0, A1
         IRGP     ///< IRN  to GPS using A0, A1 
*/
   int correctionTest(void)
   {
      gpstk::TestUtil testFramework("TimeSystemCorr", "Correction", __FILE__, __LINE__);
      std::string testMesg = "Incorrect time system correction value ";

         // Define a representative A0 and A1.  Define an epoch time and a reasonable refernce time.
         // Use these values throughout the set of tests. 
         // Test at least one conversion for each conversion type.
      A0 = 1.0e-7;
      A1 = 1.0e-12;
      deltaT = -10000;    // 10,000 sec is a nice round number for prpogation
      tscRefTime = gpstk::CivilTime(2016, 1, 3, 0, 0, 0.0); 

      gpstk::CommonTime timeOfInterest = tscRefTime + deltaT;
      double forwardResult = A0 + A1 * deltaT; 
      forwardResult = forwardResult * -1.0;    // This is a CORRECTION, not an error
      double corrVal = 0.0; 

      gpstk::TimeSystemCorrection tscTest = buildObject("GPUT");
      timeOfInterest.setTimeSystem(gpstk::TimeSystem::GPS);
      corrVal = tscTest.Correction(timeOfInterest);
      testFramework.assert( std::abs(corrVal-forwardResult) < eps, testMesg, __LINE__);
      timeOfInterest.setTimeSystem(gpstk::TimeSystem::UTC);
      corrVal = tscTest.Correction(timeOfInterest);
      testFramework.assert( std::abs(corrVal - (-forwardResult)) < eps, testMesg, __LINE__);

      tscTest = buildObject("GAUT");
      timeOfInterest.setTimeSystem(gpstk::TimeSystem::GAL);
      corrVal = tscTest.Correction(timeOfInterest);
      testFramework.assert( std::abs(corrVal - forwardResult) < eps, testMesg, __LINE__);
      timeOfInterest.setTimeSystem(gpstk::TimeSystem::UTC);
      corrVal = tscTest.Correction(timeOfInterest);
      testFramework.assert( std::abs(corrVal-(-forwardResult)) < eps, testMesg, __LINE__);

      tscTest = buildObject("QZUT");
      timeOfInterest.setTimeSystem(gpstk::TimeSystem::QZS);
      corrVal = tscTest.Correction(timeOfInterest);
      testFramework.assert( std::abs(corrVal - forwardResult) < eps, testMesg, __LINE__);
      timeOfInterest.setTimeSystem(gpstk::TimeSystem::UTC);
      corrVal = tscTest.Correction(timeOfInterest);
      testFramework.assert( std::abs(corrVal-(-forwardResult)) < eps, testMesg, __LINE__);

      tscTest = buildObject("BDUT");
      timeOfInterest.setTimeSystem(gpstk::TimeSystem::BDT);
      corrVal = tscTest.Correction(timeOfInterest);
      testFramework.assert( std::abs(corrVal - forwardResult) < eps, testMesg, __LINE__);
      timeOfInterest.setTimeSystem(gpstk::TimeSystem::UTC);
      corrVal = tscTest.Correction(timeOfInterest);
      testFramework.assert( std::abs(corrVal-(-forwardResult)) < eps, testMesg, __LINE__);

      tscTest = buildObject("IRUT");
      timeOfInterest.setTimeSystem(gpstk::TimeSystem::IRN);
      corrVal = tscTest.Correction(timeOfInterest);
      testFramework.assert( std::abs(corrVal - forwardResult) < eps, testMesg, __LINE__);
      timeOfInterest.setTimeSystem(gpstk::TimeSystem::UTC);
      corrVal = tscTest.Correction(timeOfInterest);
      testFramework.assert( std::abs(corrVal-(-forwardResult)) < eps, testMesg, __LINE__);

         // GLONASS is a little different in that the A1 term is not used
      A1 = 0;
      forwardResult = A0 + A1 * deltaT; 
      forwardResult = forwardResult * -1.0;    // This is a CORRECTION, not an error
      tscTest = buildObject("GLUT");
      timeOfInterest.setTimeSystem(gpstk::TimeSystem::GLO);
      corrVal = tscTest.Correction(timeOfInterest);
      testFramework.assert( std::abs(corrVal - forwardResult) < eps, testMesg, __LINE__);
      timeOfInterest.setTimeSystem(gpstk::TimeSystem::UTC);
      corrVal = tscTest.Correction(timeOfInterest);
      testFramework.assert( std::abs(corrVal-(-forwardResult)) < eps, testMesg, __LINE__);

      return testFramework.countFails();
   }

   gpstk::TimeSystemCorrection buildObject(std::string str)
   {
      gpstk::TimeSystemCorrection tsc(str);
      tsc.refWeek = static_cast<gpstk::GPSWeekSecond>(tscRefTime).week;
      tsc.refSOW = static_cast<gpstk::GPSWeekSecond>(tscRefTime).sow;
      tsc.A0 = A0;
      tsc.A1 = A1; 

      return tsc;
   }

private:
   double eps;

   double A0;
   double A1;
   double deltaT;
   gpstk::CommonTime tscRefTime; 

};


int main() //Main function to initialize and run all tests above
{
   TimeSystemCorr_T testClass;
   int check, errorCounter = 0;

   check = testClass.initializationTest();
   errorCounter += check;

   check = testClass.operatorTest();
   errorCounter += check;

   check = testClass.correctionTest();
   errorCounter += check;

   std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

   return errorCounter; //Return the total number of errors
}
