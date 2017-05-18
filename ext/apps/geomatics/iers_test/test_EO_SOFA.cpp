/// @file test.cpp Test IERS routines

// system includes
#include <string>
// gpstk
#include "Exception.hpp"
#include "GNSSconstants.hpp"
#include "Matrix.hpp"
// geomatics
#include "CommandLine.hpp"
#include "logstream.hpp"
#include "SolarSystem.hpp"
#include "EarthOrientation.hpp"

using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
static const string testVersion("1.0 3/12/12");

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
try {
   int i;
   EarthOrientation nGF96; nGF96.convention = IERSConvention::IERS1996;
   EarthOrientation nGF03; nGF03.convention = IERSConvention::IERS2003;
   EarthOrientation nGF10; nGF10.convention = IERSConvention::IERS2010;

   // sofa example section 5 of "SOFA Tools for Earth Attitude"

   // turn on debug7 so we get output of intermediate quantities from within class EO
   LOGlevel = ConfigureLOG::Level("DEBUG7");

   // define input
   //ttag.setYMDHMS(2007,4,5,12,0,0.0,TimeSystem::UTC);
   double mjd = 54195.5;      // UTC
   EphTime ttag;
   ttag.setMJD(static_cast<long double>(54195.5));
   ttag.setTimeSystem(TimeSystem::UTC);
   double UT1mUTC,xp,yp;
   UT1mUTC = -0.072073685;    // sec
   xp = 0.0349282;            // arcsec
   yp = 0.4833163;            // arcsec

   LOG(INFO) << "Test EarthOrientation";
   LOG(INFO) << "Epoch 2007/ 4/ 5 12: 0:0.000 UTC";
   LOG(INFO) << "Polar angles xp yp = " << fixed << setprecision(7)
               << xp << " " << yp << " arcsec";
   LOG(INFO) << "UT1-UTC = " << fixed << setprecision(9) << UT1mUTC;

   // units - don't do this, ARL:UT code takes them in arcsec
   //xp *= EarthOrientation::ARCSEC_TO_RAD;   // rad
   //yp *= EarthOrientation::ARCSEC_TO_RAD;   // rad

   // time tags
   double UT1=mjd+UT1mUTC/SEC_PER_DAY;
   double dt = TimeSystem::Correction(TimeSystem::UTC,TimeSystem::TAI,2007,4,5.5);
   double TAI=mjd+dt/SEC_PER_DAY;
   dt = TimeSystem::Correction(TimeSystem::UTC,TimeSystem::TT,2007,4,5.5);
   double TT =mjd+dt/SEC_PER_DAY;
   LOG(INFO) << fixed
      << setprecision(1) << "Times: UTC " << MJD_TO_JD << " + "
         << setprecision(15) << setw(21) << mjd << endl
      << setprecision(1) << "       TAI " << MJD_TO_JD << " + "
         << setprecision(15) << setw(21) << TAI << endl
      << setprecision(1) << "       TT  " << MJD_TO_JD << " + "
         << setprecision(15) << setw(21) << TT << endl
      << setprecision(1) << "       UT1 " << MJD_TO_JD << " + "
         << setprecision(15) << setw(21) << UT1;

   //TT.convertSystemTo(TimeSystem::TT);       // TT
   //TAI.convertSystemTo(TimeSystem::TAI);     // TAI
   //LOG(INFO) << fixed << setprecision(1)
   //            << "Times: UTC " << MJD_TO_JD << ttag.printf(" + %21.15Q\n")
   //            << "       TAI " << MJD_TO_JD << ttag.printf(" + %21.15Q\n")
   //            << "       TT  " << MJD_TO_JD << ttag.printf(" + %21.15Q\n")
   //            << "       UT1 " << MJD_TO_JD << ttag.printf(" + %21.15Q");
   //            ;

   Matrix<double> M;

   LOG(INFO) << "\nIERS 1996 / IAU 1976/1980 "
      << "------------------------------------------" << "\n\nNO Adjustments made";
   nGF96.xp = xp; nGF96.yp = yp; nGF96.UT1mUTC = UT1mUTC;
   M = nGF96.ECEFtoInertial(ttag);
   LOG(INFO) << "\ncelestial-to-terrestrial matrix:\n" << showpos << fixed
      << setprecision(15) << setw(18) << transpose(M);

   LOG(INFO) << "\nIERS 2003 / IAU 2000A, CIO "
      << "-----------------------------------------" << "\n\nNO Adjustments made";
   nGF03.xp = xp; nGF03.yp = yp; nGF03.UT1mUTC = UT1mUTC;
   M = nGF03.ECEFtoInertial(ttag);
   LOG(INFO) << "\ncelestial-to-terrestrial matrix:\n" << showpos << fixed
      << setprecision(15) << setw(18) << transpose(M);

   LOG(INFO) << "\nIERS 2010 / IAU 2000A, CIO "
      << "-----------------------------------------\n"
      << "\nUsing XY series\n"
      << "\nNO Adjustments made\n";
   nGF10.xp = xp; nGF10.yp = yp; nGF10.UT1mUTC = UT1mUTC;
   M = nGF10.ECEFtoInertial(ttag);
   LOG(INFO) << "\ncelestial-to-terrestrial matrix:\n" << showpos << fixed
      << setprecision(15) << setw(18) << transpose(M);

   return 0;
}
catch(Exception& e) { cerr << "Exception: " << e; }
catch (...) { cerr << "Unknown exception.  Abort." << endl; }
   return 1;
}   // end main()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
