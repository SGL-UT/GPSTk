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

/// @file EarthOrientation.cpp
/// class EarthOrientation encapsulates the Earth orientation parameters or EOPs.
/// Also, this class includes several static functions that implement the models in
/// the IERS Conventions, many of which involve the EOPs.
/// Cf. classes EOPStore and SolarSystem.

//------------------------------------------------------------------------------------
// system includes
#include <fstream>
// GPSTk
#include "MiscMath.hpp"
#include "logstream.hpp"

#include "EarthOrientation.hpp"

//------------------------------------------------------------------------------------
using namespace std;

namespace gpstk
{
   //---------------------------------------------------------------------------------
   // constants
   //---------------------------------------------------------------------------------
   // JulianEpoch is the epoch for CoordTransTime
   //const double EarthOrientation::JulianEpoch=2451545.0;     // JD
   const double EarthOrientation::JulianEpoch=51544.5;         // MJD
   const int EarthOrientation::intJulianEpoch=51544;

   // pi
   const double EarthOrientation::TWOPI=6.283185307179586476925287;
   const double EarthOrientation::PI=(TWOPI/2.);
   const double EarthOrientation::HALFPI=(TWOPI/4.);

   // convert degrees to radians
   const double EarthOrientation::DEG_TO_RAD=0.0174532925199432957692369;

   // convert radians to degrees
   const double EarthOrientation::RAD_TO_DEG=57.29577951308232087679815;

   // convert arc seconds to radians
   const double EarthOrientation::ARCSEC_TO_RAD=4.848136811095359935899141e-6;

   // arcseconds in 360 degrees
   const double EarthOrientation::ARCSEC_PER_CIRCLE=1296000.0;

   //---------------------------------------------------------------------------------
   ostream& operator<<(ostream& os, const EarthOrientation& eo)
   {
      os << " " << fixed << setw(10) << setprecision(6) << eo.xp
         << " " << setw(10) << setprecision(6) << eo.yp
         << " " << setw(11) << setprecision(7) << eo.UT1mUTC
         << " " << eo.convention.asString();
      return os;
   }

   //---------------------------------------------------------------------------------
   // EOP interpolation and correction routines - internal use only

   /// Compute fundamental arguments at CoordTransTime T, which are
   /// GMST+pi, L, Lp, F, D, Omega. These are valid for interpolating EOPs in both
   /// IERS 2003 and 2010 conventions.
   /// param T CoordTransTime for epoch of interest
   /// param args array of 6 fundamental arguments at T, units radians
   static void computeFundamentalArgs(const double& T, double args[6]) throw();

   /// Corrections to EOPs xp,yp,UT1mUTC for diurnal and semi-diurnal variations
   /// due to ocean tides. Based on IERS routine ortho_eop.f from the USNO web site
   /// for the 2010 conventions (NB the 2003 version of ortho_eop is nominally
   /// different, however in fact the algorithm and constants are the same; the only
   /// difference is that the 2003 algorithm (FTN) is single precision).
   /// param mjd time (UTC) of interest
   /// param dxp correction to xp in arcseconds
   /// param dyp correction to yp in arcseconds
   /// param dUT correction to UT1mUTC in seconds
   static void correctEOPOceanTides(const double& mjd,
                                    double& dxp, double& dyp, double& dUT) throw();

   /// Corrections to Earth rotation due to zonal tides using USNO IERS2010 algorithm.
   /// param args array of 6 fundamental arguments at T, units radians
   ///  NB result of computeFundamentalArgs(); args[0] (=GMST+pi) is not used
   /// param dUT correction to UT1mUTC in seconds
   /// param dld correction to length of day in seconds/day
   /// param dom correction to yp in radians per second
   static void correctEarthRotationZonalTides(const double args[6],
                                    double& dUT, double& dld, double& dom) throw();

   /// Corrections to UT1 and length of day (LOD) due to zonal tides using USNO
   /// IERS2003 algorithm.
   /// param args array of 6 fundamental arguments at T, units radians
   ///  NB result of computeFundamentalArgs(); args[0] (=GMST+pi) is not used
   /// param dUT correction to UT1mUTC in seconds
   /// param dld correction to length of day in seconds/day
   /// param dom correction to yp in radians per second
   static void correctEarthRotationZonalTides2003(const double args[6],
                                    double& dUT, double& dld, double& dom) throw();

   /// Corrections to UT1 and length of day (LOD) due to subdiurnal librations
   /// using USNO IERS2010 algorithm.
   /// param args array of 6 fundamental arguments at T (CoordTransTime), in radians
   ///  NB result of computeFundamentalArgs()
   /// param dUT correction to UT1mUTC in seconds
   /// param dld correction to length of day in seconds/day
   static void correctEarthRotationLibrations(const double args[6],
                                              double& dUT, double& dld) throw();

   // Given parallel arrays of length four containing the values from EOPStore
   // for time (int MJD) and EOPs xp, yp, and UT1mUTC, where the time of interest
   // t lies within the values of the time array, interpolate and apply
   // corrections to determine the EOPs at t, using the algorithm prescribed
   // by the given IERS convention.
   // param t EphTime at which to compute EOPs
   // param time vector of length 4 of consecutive MJDs from EOPStore;
   //                              t must lie within this timespan.
   // param X vector of length 4 of consecutive xp from EOPStore.
   // param Y vector of length 4 of consecutive yp from EOPStore.
   // param dT vector of length 4 of consecutive UT1mUTC from EOPStore.
   // param conv the IERSConvention to be used.
   // return EarthOrientation EOPs at time t.
   void EarthOrientation::interpolateEOP(const EphTime& t,
                                         const vector<double>& time,
                                         const vector<double>& X,
                                         const vector<double>& Y,
                                         vector<double>& dT,
                                         const IERSConvention& in_conv)
      throw(InvalidRequest)
   {
      int i,j;
      double dxp,dyp,dUT,dlod,domega;
      double args[6];

      // set the convention for this object
      convention = in_conv;

      // first get MJD(UTC), for the Lagrange interpolation
      EphTime ttag(t);
      ttag.convertSystemTo(TimeSystem::UTC);
      double mjdUTC(ttag.dMJD());

      // now convert to TT, for the corrections algorithms
      ttag.convertSystemTo(TimeSystem::TT);
      double mjd(ttag.dMJD());
      double T = (mjd - 51544.5)/36525.0;

      // ----------------------------------------------------------------
      // step 1 : Lagrange interpolation of xp and yp
      double err;
      xp = LagrangeInterpolation(time,X,mjdUTC,err);            // arcsec
      yp = LagrangeInterpolation(time,Y,mjdUTC,err);            // arcsec
      //LOG(INFO) << " -> " << fixed << setprecision(10) << mjdUTC
      //   << " " << setprecision(15) << xp << " " << yp;

      // 1a. remove long period tides from UT1-UTC data -------------------
      // NB don't need ttag anymore but do need mjd and T.
      for(i=0; i<time.size(); i++) {
         ttag.setMJD(time[i]);
         ttag.setTimeSystem(TimeSystem::UTC);
         ttag.convertSystemTo(TimeSystem::TT);
         double Ttemp = (ttag.dMJD() - 51544.5)/36525.0;
         computeFundamentalArgs(Ttemp, args);
         if(convention == IERSConvention::IERS2010)
            correctEarthRotationZonalTides(args, dUT, dlod, domega);
         else
            correctEarthRotationZonalTides2003(args, dUT, dlod, domega);
         dT[i] -= dUT;
         //LOG(INFO) << " UT " <<fixed<< setprecision(10) << time[i] << " " << dT[i];
      }

      // 1b. interpolate UT1-UTC -------------------------------------------
      UT1mUTC = LagrangeInterpolation(time,dT,mjdUTC,err);      // seconds
      //LOG(INFO) << " -> " << fixed << setprecision(10) << mjdUTC
      //                    << " " << setprecision(15) << UT1mUTC;

      // ----------------------------------------------------------------
      // step 2 : Compute fundamental arguments for use in corrections
      computeFundamentalArgs(T, args);
      //LOG(INFO) << fixed
      //   << "T(" << setprecision(2) << mjd << ")=" << setprecision(15) << T
      //   << "\nGMST(" << setprecision(2) << mjd << ")="<<setprecision(15)<< args[0]
      //   << "\nL(" << setprecision(2) << mjd << ")=" << setprecision(15) << args[1]
      //   << "\nLp(" << setprecision(2) << mjd << ")=" << setprecision(15) << args[2]
      //   << "\nF(" << setprecision(2) << mjd << ")=" << setprecision(15) << args[3]
      //   << "\nD(" << setprecision(2) << mjd << ")=" << setprecision(15) << args[4]
      //   << "\nOm(" << setprecision(2) << mjd << ")=" << setprecision(15)<< args[5];

      // ----------------------------------------------------------------
      // step 3 : Compute corrections and apply to eop

      // 3a. restore long period tides to UT1-UTC --------------------------
      // Corrections to Earth rotation due to zonal tides using USNO algorithm
      // differences between 2003 and 2010 are very small, and only in the Zonal tides
      if(convention == IERSConvention::IERS2010)
         correctEarthRotationZonalTides(args, dUT, dlod, domega);
      else
         correctEarthRotationZonalTides2003(args, dUT, dlod, domega);

      UT1mUTC += dUT;
      //LOG(INFO) << " Zonal tides UT1 correction " <<fixed<<setprecision(15) << dUT;

      // step 3b. --------------------------------------
      // Corrections to EOP due to diurnal and semidiurnal effects of ocean tides
      // using USNO algorithm
      correctEOPOceanTides(mjd, dxp, dyp, dUT);
      xp += dxp;
      yp += dyp;
      UT1mUTC += dUT;
      //LOG(INFO) << " Tides corrections " << fixed << setprecision(15)
      //   << dxp << " " << dyp << " " << dUT;

      // step 3c. --------------------------------------
      // correct for librations - a few microarcseconds but high frequency
      //correctEarthRotationLibrations(args, dUT, dlod);
      //UT1mUTC += dUT;
      //LOG(INFO) << " Librations UT1 correction " << fixed<<setprecision(15) << dUT;

   }

   //---------------------------------------------------------------------------------
   void computeFundamentalArgs(const double& T, double args[6]) throw()
   {

      // compute arguments and their time derivatives GMST+pi, L, Lp, F, D, Omega
      // units are radians and radians/day
      // NB to compute corrections to length of day (LOD), remove '//LOD' everywhere
      //LOD double dargs[6];
      //LOD static const double AS_TO_RAD_PER_DAY=(ARCSEC_TO_RAD/36525.0);

      // GMST+pi
      // GMST in seconds of day. NB 3155760000.0 = 876600.0*3600.0
      double GMST = ::fmod(     67310.54841
                      + T*(3155760000.0 + 8640184.812866
                    + T*T*(0.093104 + T*(-6.2e-6))), 86400.0);      // seconds of day
      // convert to arcsec, then radians : 360*3600 arcsec / 24*3600 sec = 15 as/sec
      GMST *= 15.0 * EarthOrientation::ARCSEC_TO_RAD;
      args[0] = ::fmod(GMST + EarthOrientation::PI, EarthOrientation::TWOPI);
      //LOD dargs[0] = 15.0*(876600*3600 + 8640184.812866
      //LOD            + T*(2.*0.093104 - T*(3.*6.2e-6))) * AS_TO_RAD_PER_DAY;

      // fundamental arguments cf. IERS 2010 conventions TN36 5.7.2 eqn 5.43

      // L = mean anomaly of moon
      args[1] = EarthOrientation::L(T);
      //LOD dargs[1] = (1717915923.2178 + T*(2.*31.8792
      //LOD        + T*(3.*0.051635 + T*(-4.*0.00024470)))) * AS_TO_RAD_PER_DAY;

      // Lp = mean anomaly of sun
      args[2] = EarthOrientation::Lp(T);
      //LOD dargs[2] = (129596581.0481 + T*(-2.*0.5532
      //LOD    + T*(-3.*0.000136 + T*(-4.*0.00001149)))) * AS_TO_RAD_PER_DAY;

      // F = mean anomaly of moon minus Omega
      args[3] = EarthOrientation::F(T);
      //LOD dargs[3] = (1739527262.8478 + T*(-2.*12.7512
      //LOD    + T*(-3.*0.001037 + T*(4.*0.00000417)))) * AS_TO_RAD_PER_DAY;

      // D = mean elongation moon from sun
      args[4] = EarthOrientation::D(T);
      //LOD dargs[4] = (1602961601.2090 + T*(-2.*6.3706
      //LOD    + T*(+3.*0.006593 + T*(-4.*0.00003169)))) * AS_TO_RAD_PER_DAY;

      // Omega = mean longitude of ascending node of moon
      args[5] = EarthOrientation::Omega2003(T);
      //LOD dargs[5] = (-6962890.2665 + T*(2.*7.4722
      //LOD    + T*(3.*0.007702 + T*(-4.*0.00005939)))) * AS_TO_RAD_PER_DAY;

   }  // end computeFundamentalArgs()

   //---------------------------------------------------------------------------------
   void correctEOPOceanTides(const double& mjd, double& dxp, double& dyp, double& dUT)
      throw()
   {
      // compute time dependent part of second degree diurnal and semidiurnal
      // tidal potential from the dominant spectral lines of the
      // Cartwright-Tayler-Edden harmonic decomposition.

      // define the orthotide weight factors
      static const double fact[][2] = {
         { 0.0298, 0.0200 },
         { 0.1408, 0.0905 },
         { 0.0805, 0.0638 },
         { 0.6002, 0.3476 },
         { 0.3025, 0.1645 },
         { 0.1517, 0.0923 }
      };
      //for(int kk=0; kk<6; kk++)
      //LOG(INFO) << "SP(" << kk << ")=" << fixed << setprecision(5)
      //   << setw(9) << fact[kk][0] << setw(9) << fact[kk][1];

      // tidal potential model for 71 diurnal and semidiurnal lines
      typedef struct {
         int nj,mj;                 // nj is always 2!
         double hs, phase, freq;
      } Coeff;
      static const Coeff C[] = {
         { 2, 1,  -1.94,  9.0899831,  5.18688050 },
         { 2, 1,  -1.25,  8.8234208,  5.38346657 },
         { 2, 1,  -6.64, 12.1189598,  5.38439079 },
         { 2, 1,  -1.51,  1.4425700,  5.41398343 },
         { 2, 1,  -8.02,  4.7381090,  5.41490765 },
         { 2, 1,  -9.47,  4.4715466,  5.61149372 },
         { 2, 1, -50.20,  7.7670857,  5.61241794 },
         { 2, 1,  -1.80, -2.9093042,  5.64201057 },
         { 2, 1,  -9.54,  0.3862349,  5.64293479 },
         { 2, 1,   1.52, -3.1758666,  5.83859664 },
         { 2, 1, -49.45,  0.1196725,  5.83952086 },
         { 2, 1,-262.21,  3.4152116,  5.84044508 },
         { 2, 1,   1.70, 12.8946194,  5.84433381 },
         { 2, 1,   3.43,  5.5137686,  5.87485066 },
         { 2, 1,   1.94,  6.4441883,  6.03795537 },
         { 2, 1,   1.37, -4.2322016,  6.06754801 },
         { 2, 1,   7.41, -0.9366625,  6.06847223 },
         { 2, 1,  20.62,  8.5427453,  6.07236095 },
         { 2, 1,   4.14, 11.8382843,  6.07328517 },
         { 2, 1,   3.94,  1.1618945,  6.10287781 },
         { 2, 1,  -7.14,  5.9693878,  6.24878055 },
         { 2, 1,   1.37, -1.2032249,  6.26505830 },
         { 2, 1,-122.03,  2.0923141,  6.26598252 },
         { 2, 1,   1.02, -1.7847596,  6.28318449 },
         { 2, 1,   2.89,  8.0679449,  6.28318613 },
         { 2, 1,  -7.30,  0.8953321,  6.29946388 },
         { 2, 1, 368.78,  4.1908712,  6.30038810 },
         { 2, 1,  50.01,  7.4864102,  6.30131232 },
         { 2, 1,  -1.08, 10.7819493,  6.30223654 },
         { 2, 1,   2.93,  0.3137975,  6.31759007 },
         { 2, 1,   5.25,  6.2894282,  6.33479368 },
         { 2, 1,   3.95,  7.2198478,  6.49789839 },
         { 2, 1,  20.62, -0.1610030,  6.52841524 },
         { 2, 1,   4.09,  3.1345361,  6.52933946 },
         { 2, 1,   3.42,  2.8679737,  6.72592553 },
         { 2, 1,   1.69, -4.5128771,  6.75644239 },
         { 2, 1,  11.29,  4.9665307,  6.76033111 },
         { 2, 1,   7.23,  8.2620698,  6.76125533 },
         { 2, 1,   1.51, 11.5576089,  6.76217955 },
         { 2, 1,   2.16,  0.6146566,  6.98835826 },
         { 2, 1,   1.38,  3.9101957,  6.98928248 },
         { 2, 2,   1.80, 20.6617051, 11.45675174 },
         { 2, 2,   4.67, 13.2808543, 11.48726860 },
         { 2, 2,  16.01, 16.3098310, 11.68477889 },
         { 2, 2,  19.32,  8.9289802, 11.71529575 },
         { 2, 2,   1.30,  5.0519065, 11.73249771 },
         { 2, 2,  -1.02, 15.8350306, 11.89560406 },
         { 2, 2,  -4.51,  8.6624178, 11.91188181 },
         { 2, 2, 120.99, 11.9579569, 11.91280603 },
         { 2, 2,   1.13,  8.0808832, 11.93000800 },
         { 2, 2,  22.98,  4.5771061, 11.94332289 },
         { 2, 2,   1.06,  0.7000324, 11.96052486 },
         { 2, 2,  -1.90, 14.9869335, 12.11031632 },
         { 2, 2,  -2.18, 11.4831564, 12.12363121 },
         { 2, 2, -23.58,  4.3105437, 12.13990896 },
         { 2, 2, 631.92,  7.6060827, 12.14083318 },
         { 2, 2,   1.92,  3.7290090, 12.15803515 },
         { 2, 2,  -4.66, 10.6350594, 12.33834347 },
         { 2, 2, -17.86,  3.2542086, 12.36886033 },
         { 2, 2,   4.47, 12.7336164, 12.37274905 },
         { 2, 2,   1.97, 16.0291555, 12.37367327 },
         { 2, 2,  17.20, 10.1602590, 12.54916865 },
         { 2, 2, 294.00,  6.2831853, 12.56637061 },
         { 2, 2,  -2.46,  2.4061116, 12.58357258 },
         { 2, 2,  -1.02,  5.0862033, 12.59985198 },
         { 2, 2,  79.96,  8.3817423, 12.60077620 },
         { 2, 2,  23.83, 11.6772814, 12.60170041 },
         { 2, 2,   2.59, 14.9728205, 12.60262463 },
         { 2, 2,   4.47,  4.0298682, 12.82880334 },
         { 2, 2,   1.95,  7.3254073, 12.82972756 },
         { 2, 2,   1.17,  9.1574019, 13.06071921 }
      };
 
      static const double dt(2.0);
      static const double TWOPI(6.283185307179586476925287);
      int j,k,n,m;
      double a[2][3], b[2][3], dt60, pinm, alpha;
 
      // compute time dependent potential matrix
      for(k=0; k<3; ++k) {
         dt60 = (mjd - (k-1)*dt) - 37076.5;
         a[0][k] = a[1][k] = 0.0;
         b[0][k] = b[1][k] = 0.0;
         for(j=0; j<71; ++j) {
            n = C[j].nj;         // 2!
            m = C[j].mj;         // 1 or 2
            pinm = double((n+m)%2) * TWOPI / 4.0;
            alpha = ::fmod(C[j].phase - pinm, TWOPI)
                  + ::fmod(C[j].freq * dt60, TWOPI);
            a[m-1][k] += C[j].hs * ::cos(alpha);
            b[m-1][k] -= C[j].hs * ::sin(alpha);
         }
      }

      // orthogonalize response terms
      double ap,am,bp,bm,p[3][2],q[3][2];
      for(m=0; m<2; ++m) {
         ap = a[m][2] + a[m][0];
         am = a[m][2] - a[m][0];
         bp = b[m][2] + b[m][0];
         bm = b[m][2] - b[m][0];
         p[0][m] = fact[0][m] * a[m][1];
         p[1][m] = fact[1][m] * a[m][1] - fact[2][m]*ap;
         p[2][m] = fact[3][m] * a[m][1] - fact[4][m]*ap + fact[5][m]*bm;
         q[0][m] = fact[0][m] * b[m][1];
         q[1][m] = fact[1][m] * b[m][1] - fact[2][m]*bp;
         q[2][m] = fact[3][m] * b[m][1] - fact[4][m]*bp - fact[5][m]*am;
         a[m][0] = p[0][m];
         a[m][1] = p[1][m];
         a[m][2] = p[2][m];
         b[m][0] = q[0][m];
         b[m][1] = q[1][m];
         b[m][2] = q[2][m];
      }

      // fill partials vector
      double h[12];
      for(j=0,m=0; m<2; ++m) {
         for(k=0; k<3; ++k) {
            h[j]   = a[m][k];
            h[j+1] = b[m][k];
            j += 2;
         }
      }
      //for(k=0; k<12; k++)
      //LOG(INFO) << "H(" << k << ")=" << fixed << setprecision(15) << h[k];

      // orthoweights
      static const double orthowts[][3] = {
         { -6.77832, 14.86283, -1.76335 },
         {-14.86323, -6.77846,  1.03364 },
         {  0.47884,  1.45234, -0.27553 },
         { -1.45303,  0.47888,  0.34569 },
         {  0.16406, -0.42056, -0.12343 },
         {  0.42030,  0.16469, -0.10146 },
         {  0.09398, 15.30276, -0.47119 },
         { 25.73054, -4.30615,  1.28997 },
         { -4.77974,  0.07564, -0.19336 },
         {  0.28080,  2.28321,  0.02724 },
         {  1.94539, -0.45717,  0.08955 },
         { -0.73089, -1.62010,  0.04726 }
      };
      //for(k=0; k<12; k++)
      //LOG(INFO) << "ORTHOW(" << k << ")=" << fixed << setprecision(5)
      //   << setw(9) << orthowts[k][0]
      //   << setw(9) << orthowts[k][1]
      //   << setw(9) << orthowts[k][2];

      double eop[3];
      for(k=0; k<3; ++k) {
         eop[k] = 0.0;
         for(j=0; j<12; ++j)
            eop[k] += h[j] * orthowts[j][k];
      }
      
      // convert to arcsec and seconds
      dxp = eop[0] * 1.e-6;
      dyp = eop[1] * 1.e-6;
      dUT = eop[2] * 1.e-6;

   }

   //---------------------------------------------------------------------------------
   // Corrections to Earth rotation due to zonal tides using USNO algorithm
   // param args array of 6 fundamental arguments at T, units radians
   //  NB args[0] (=GMST+pi) is not used
   // param dUT correction to UT1mUTC in seconds
   // param dld correction to length of day in seconds/day
   // param dom correction to yp in radians per second
   void correctEarthRotationZonalTides(const double args[6],
                                       double& dUT, double& dld, double& dom)
      throw()
   {
      // constants
      static const double TWOPI(6.283185307179586476925287);

      // luni-solar coefficients in argument multiplying L Lp F D Omega, 62 terms
      // and coefficients of DUT sin, cos; DLOD cos, sin; DOMEGA cos, sin
      typedef struct {
         int nargs[5];
         double dutsin, dutcos, dldcos, dldsin, domcos, domsin;  // yes its sc cs cs
      } Coeff;

      static const Coeff C[] = {
         { { 1,  0,  2,  2,  2 }, -0.0235, 0.0000, 0.2617, 0.0000, -0.2209, 0.0000 },
         { { 2,  0,  2,  0,  1 }, -0.0404, 0.0000, 0.3706, 0.0000, -0.3128, 0.0000 },
         { { 2,  0,  2,  0,  2 }, -0.0987, 0.0000, 0.9041, 0.0000, -0.7630, 0.0000 },
         { { 0,  0,  2,  2,  1 }, -0.0508, 0.0000, 0.4499, 0.0000, -0.3797, 0.0000 },
         { { 0,  0,  2,  2,  2 }, -0.1231, 0.0000, 1.0904, 0.0000, -0.9203, 0.0000 },
         { { 1,  0,  2,  0,  0 }, -0.0385, 0.0000, 0.2659, 0.0000, -0.2244, 0.0000 },
         { { 1,  0,  2,  0,  1 }, -0.4108, 0.0000, 2.8298, 0.0000, -2.3884, 0.0000 },
         { { 1,  0,  2,  0,  2 }, -0.9926, 0.0000, 6.8291, 0.0000, -5.7637, 0.0000 },
         { { 3,  0,  0,  0,  0 }, -0.0179, 0.0000, 0.1222, 0.0000, -0.1031, 0.0000 },
         { {-1,  0,  2,  2,  1 }, -0.0818, 0.0000, 0.5384, 0.0000, -0.4544, 0.0000 },
         { {-1,  0,  2,  2,  2 }, -0.1974, 0.0000, 1.2978, 0.0000, -1.0953, 0.0000 },
         { { 1,  0,  0,  2,  0 }, -0.0761, 0.0000, 0.4976, 0.0000, -0.4200, 0.0000 },
         { { 2,  0,  2, -2,  2 },  0.0216, 0.0000,-0.1060, 0.0000,  0.0895, 0.0000 },
         { { 0,  1,  2,  0,  2 },  0.0254, 0.0000,-0.1211, 0.0000,  0.1022, 0.0000 },
         { { 0,  0,  2,  0,  0 }, -0.2989, 0.0000, 1.3804, 0.0000, -1.1650, 0.0000 },
         { { 0,  0,  2,  0,  1 }, -3.1873, 0.2010,14.6890, 0.9266,-12.3974,-0.7820 },
         { { 0,  0,  2,  0,  2 }, -7.8468, 0.5320,36.0910, 2.4469,-30.4606,-2.0652 },
         { { 2,  0,  0,  0, -1 },  0.0216, 0.0000,-0.0988, 0.0000,  0.0834, 0.0000 },
         { { 2,  0,  0,  0,  0 }, -0.3384, 0.0000, 1.5433, 0.0000, -1.3025, 0.0000 },
         { { 2,  0,  0,  0,  1 },  0.0179, 0.0000,-0.0813, 0.0000,  0.0686, 0.0000 },
         { { 0, -1,  2,  0,  2 }, -0.0244, 0.0000, 0.1082, 0.0000, -0.0913, 0.0000 },
         { { 0,  0,  0,  2, -1 },  0.0470, 0.0000,-0.2004, 0.0000,  0.1692, 0.0000 },
         { { 0,  0,  0,  2,  0 }, -0.7341, 0.0000, 3.1240, 0.0000, -2.6367, 0.0000 },
         { { 0,  0,  0,  2,  1 }, -0.0526, 0.0000, 0.2235, 0.0000, -0.1886, 0.0000 },
         { { 0, -1,  0,  2,  0 }, -0.0508, 0.0000, 0.2073, 0.0000, -0.1749, 0.0000 },
         { { 1,  0,  2, -2,  1 },  0.0498, 0.0000,-0.1312, 0.0000,  0.1107, 0.0000 },
         { { 1,  0,  2, -2,  2 },  0.1006, 0.0000,-0.2640, 0.0000,  0.2228, 0.0000 },
         { { 1,  1,  0,  0,  0 },  0.0395, 0.0000,-0.0968, 0.0000,  0.0817, 0.0000 },
         { {-1,  0,  2,  0,  0 },  0.0470, 0.0000,-0.1099, 0.0000,  0.0927, 0.0000 },
         { {-1,  0,  2,  0,  1 },  0.1767, 0.0000,-0.4115, 0.0000,  0.3473, 0.0000 },
         { {-1,  0,  2,  0,  2 },  0.4352, 0.0000,-1.0093, 0.0000,  0.8519, 0.0000 },
         { { 1,  0,  0,  0, -1 },  0.5339, 0.0000,-1.2224, 0.0000,  1.0317, 0.0000 },
         { { 1,  0,  0,  0,  0 }, -8.4046, 0.2500,19.1647, 0.5701,-16.1749,-0.4811 },
         { { 1,  0,  0,  0,  1 },  0.5443, 0.0000,-1.2360, 0.0000,  1.0432, 0.0000 },
         { { 0,  0,  0,  1,  0 },  0.0470, 0.0000,-0.1000, 0.0000,  0.0844, 0.0000 },
         { { 1, -1,  0,  0,  0 }, -0.0555, 0.0000, 0.1169, 0.0000, -0.0987, 0.0000 },
         { {-1,  0,  0,  2, -1 },  0.1175, 0.0000,-0.2332, 0.0000,  0.1968, 0.0000 },
         { {-1,  0,  0,  2,  0 }, -1.8236, 0.0000, 3.6018, 0.0000, -3.0399, 0.0000 },
         { {-1,  0,  0,  2,  1 },  0.1316, 0.0000,-0.2587, 0.0000,  0.2183, 0.0000 },
         { { 1,  0, -2,  2, -1 },  0.0179, 0.0000,-0.0344, 0.0000,  0.0290, 0.0000 },
         { {-1, -1,  0,  2,  0 }, -0.0855, 0.0000, 0.1542, 0.0000, -0.1302, 0.0000 },
         { { 0,  2,  2, -2,  2 }, -0.0573, 0.0000, 0.0395, 0.0000, -0.0333, 0.0000 },
         { { 0,  1,  2, -2,  1 },  0.0329, 0.0000,-0.0173, 0.0000,  0.0146, 0.0000 },
         { { 0,  1,  2, -2,  2 }, -1.8847, 0.0000, 0.9726, 0.0000, -0.8209, 0.0000 },
         { { 0,  0,  2, -2,  0 },  0.2510, 0.0000,-0.0910, 0.0000,  0.0768, 0.0000 },
         { { 0,  0,  2, -2,  1 },  1.1703, 0.0000,-0.4135, 0.0000,  0.3490, 0.0000 },
         { { 0,  0,  2, -2,  2 },-49.7174, 0.4330,17.1056, 0.1490,-14.4370,-0.1257 },
         { { 0,  2,  0,  0,  0 }, -0.1936, 0.0000, 0.0666, 0.0000, -0.0562, 0.0000 },
         { { 2,  0,  0, -2, -1 },  0.0489, 0.0000,-0.0154, 0.0000,  0.0130, 0.0000 },
         { { 2,  0,  0, -2,  0 }, -0.5471, 0.0000, 0.1670, 0.0000, -0.1409, 0.0000 },
         { { 2,  0,  0, -2,  1 },  0.0367, 0.0000,-0.0108, 0.0000,  0.0092, 0.0000 },
         { { 0, -1,  2, -2,  1 }, -0.0451, 0.0000, 0.0082, 0.0000, -0.0069, 0.0000 },
         { { 0,  1,  0,  0, -1 },  0.0921, 0.0000,-0.0167, 0.0000,  0.0141, 0.0000 },
         { { 0, -1,  2, -2,  2 },  0.8281, 0.0000,-0.1425, 0.0000,  0.1202, 0.0000 },
         { { 0,  1,  0,  0,  0 },-15.8887, 0.1530, 2.7332, 0.0267, -2.3068,-0.0222 },
         { { 0,  1,  0,  0,  1 }, -0.1382, 0.0000, 0.0225, 0.0000, -0.0190, 0.0000 },
         { { 1,  0,  0, -1,  0 },  0.0348, 0.0000,-0.0053, 0.0000,  0.0045, 0.0000 },
         { { 2,  0, -2,  0,  0 }, -0.1372, 0.0000,-0.0079, 0.0000,  0.0066, 0.0000 },
         { {-2,  0,  2,  0,  1 },  0.4211, 0.0000,-0.0203, 0.0000,  0.0171, 0.0000 },
         { {-1,  1,  0,  1,  0 }, -0.0404, 0.0000, 0.0008, 0.0000, -0.0007, 0.0000 },
         { { 0,  0,  0,  0,  2 },  7.8998, 0.0000, 0.1460, 0.0000, -0.1232, 0.0000 },
         { { 0,  0,  0,  0,  1 }, -1617.2681,0.0000,-14.9471,0.0000, 12.6153,0.0000}
      };

      // number of coefficients
      static const int N = int(sizeof(C) / sizeof(Coeff));

      dUT = dld = dom = 0.0;
      for(int i=0; i<N; i++) {
         double arg(0.0);
         for(int j=0; j<5; j++)
            arg += double(C[i].nargs[j]) * args[j+1];    // NB args[0] is not used
         arg = ::fmod(arg,TWOPI);
         double sarg = ::sin(arg);
         double carg = ::cos(arg);

         dUT += C[i].dutsin * sarg + C[i].dutcos * carg;
         dld += C[i].dldsin * sarg + C[i].dldcos * carg;
         dom += C[i].domsin * sarg + C[i].domcos * carg;
      }

      // change units to seconds, seconds/day, and radians/sec
      dUT *= 1.e-4;
      dld *= 1.e-5;
      dom *= 1.e-14;

   }  // end correctEarthRotationZonalTides()

   //---------------------------------------------------------------------------------
   // Corrections to Earth rotation due to zonal tides using USNO IERS2003 algorithm
   // param args array of 6 fundamental arguments at T, units radians
   //  NB args[0] (=GMST+pi) is not used
   // param dUT correction to UT1mUTC in seconds
   // param dld correction to length of day in seconds/day
   // param dom correction to yp in radians per second
   void correctEarthRotationZonalTides2003(const double args[6],
                                           double& dUT, double& dld, double& dom)
      throw()
   {
      // constants
      static const double TWOPI(6.283185307179586476925287);

      // luni-solar multipliers in argument L Lp F D Omega, 62 terms
      // and coefficients of DUT sin, cos; DLOD cos, sin; DOMEGA cos, sin
      typedef struct {
         int nargs[5];
         double dutsin, dutcos, dldcos, dldsin, domcos, domsin;  // yes its sc cs cs
      } Coeff;

      static const Coeff C[] = {
         { { 1, 0, 2, 2, 2 }, -0.02, 0.00,  0.26,  0.00, -0.22, 0.00 },
         { { 2, 0, 2, 0, 1 }, -0.04, 0.00,  0.38,  0.00, -0.32, 0.00 },
         { { 2, 0, 2, 0, 2 }, -0.10, 0.00,  0.91,  0.00, -0.76, 0.00 },
         { { 0, 0, 2, 2, 1 }, -0.05, 0.00,  0.45,  0.00, -0.38, 0.00 },
         { { 0, 0, 2, 2, 2 }, -0.12, 0.00,  1.09,  0.01, -0.92,-0.01 },
         { { 1, 0, 2, 0, 0 }, -0.04, 0.00,  0.27,  0.00, -0.22, 0.00 },
         { { 1, 0, 2, 0, 1 }, -0.41, 0.00,  2.84,  0.02, -2.40,-0.01 },
         { { 1, 0, 2, 0, 2 }, -1.00, 0.01,  6.85,  0.04, -5.78,-0.03 },
         { { 3, 0, 0, 0, 0 }, -0.02, 0.00,  0.12,  0.00, -0.11, 0.00 },
         { {-1, 0, 2, 2, 1 }, -0.08, 0.00,  0.54,  0.00, -0.46, 0.00 },
         { {-1, 0, 2, 2, 2 }, -0.20, 0.00,  1.30,  0.01, -1.10,-0.01 },
         { { 1, 0, 0, 2, 0 }, -0.08, 0.00,  0.50,  0.00, -0.42, 0.00 },
         { { 2, 0, 2,-2, 2 },  0.02, 0.00, -0.11,  0.00,  0.09, 0.00 },
         { { 0, 1, 2, 0, 2 },  0.03, 0.00, -0.12,  0.00,  0.10, 0.00 },
         { { 0, 0, 2, 0, 0 }, -0.30, 0.00,  1.39,  0.01, -1.17,-0.01 },
         { { 0, 0, 2, 0, 1 }, -3.22, 0.02, 14.86,  0.09,-12.54,-0.08 },
         { { 0, 0, 2, 0, 2 }, -7.79, 0.05, 35.84,  0.22,-30.25,-0.18 },
         { { 2, 0, 0, 0,-1 },  0.02, 0.00, -0.10,  0.00,  0.08, 0.00 },
         { { 2, 0, 0, 0, 0 }, -0.34, 0.00,  1.55,  0.01, -1.31,-0.01 },
         { { 2, 0, 0, 0, 1 },  0.02, 0.00, -0.08,  0.00,  0.07, 0.00 },
         { { 0,-1, 2, 0, 2 }, -0.02, 0.00,  0.11,  0.00, -0.09, 0.00 },
         { { 0, 0, 0, 2,-1 },  0.05, 0.00, -0.20,  0.00,  0.17, 0.00 },
         { { 0, 0, 0, 2, 0 }, -0.74, 0.00,  3.14,  0.02, -2.65,-0.02 },
         { { 0, 0, 0, 2, 1 }, -0.05, 0.00,  0.22,  0.00, -0.19, 0.00 },
         { { 0,-1, 0, 2, 0 }, -0.05, 0.00,  0.21,  0.00, -0.17, 0.00 },
         { { 1, 0, 2,-2, 1 },  0.05, 0.00, -0.13,  0.00,  0.11, 0.00 },
         { { 1, 0, 2,-2, 2 },  0.10, 0.00, -0.26,  0.00,  0.22, 0.00 },
         { { 1, 1, 0, 0, 0 },  0.04, 0.00, -0.10,  0.00,  0.08, 0.00 },
         { {-1, 0, 2, 0, 0 },  0.05, 0.00, -0.11,  0.00,  0.09, 0.00 },
         { {-1, 0, 2, 0, 1 },  0.18, 0.00, -0.41,  0.00,  0.35, 0.00 },
         { {-1, 0, 2, 0, 2 },  0.44, 0.00, -1.02, -0.01,  0.86, 0.01 },
         { { 1, 0, 0, 0,-1 },  0.54, 0.00, -1.23, -0.01,  1.04, 0.01 },
         { { 1, 0, 0, 0, 0 }, -8.33, 0.06, 18.99,  0.13,-16.03,-0.11 },
         { { 1, 0, 0, 0, 1 },  0.55, 0.00, -1.25, -0.01,  1.05, 0.01 },
         { { 0, 0, 0, 1, 0 },  0.05, 0.00, -0.11,  0.00,  0.09, 0.00 },
         { { 1,-1, 0, 0, 0 }, -0.06, 0.00,  0.12,  0.00, -0.10, 0.00 },
         { {-1, 0, 0, 2,-1 },  0.12, 0.00, -0.24,  0.00,  0.20, 0.00 },
         { {-1, 0, 0, 2, 0 }, -1.84, 0.01,  3.63,  0.02, -3.07,-0.02 },
         { {-1, 0, 0, 2, 1 },  0.13, 0.00, -0.26,  0.00,  0.22, 0.00 },
         { { 1, 0,-2, 2,-1 },  0.02, 0.00, -0.04,  0.00,  0.03, 0.00 },
         { {-1,-1 ,0, 2, 0 }, -0.09, 0.00,  0.16,  0.00, -0.13, 0.00 },
         { { 0, 2, 2,-2, 2 }, -0.06, 0.00,  0.04,  0.00, -0.03, 0.00 },
         { { 0, 1, 2,-2, 1 },  0.03, 0.00, -0.02,  0.00,  0.01, 0.00 },
         { { 0, 1, 2,-2, 2 }, -1.91, 0.02,  0.98,  0.01, -0.83,-0.01 },
         { { 0, 0, 2,-2, 0 },  0.26, 0.00, -0.09,  0.00,  0.08, 0.00 },
         { { 0, 0, 2,-2, 1 },  1.18,-0.01, -0.42,  0.00,  0.35, 0.00 },
         { { 0, 0, 2,-2, 2 },-49.06, 0.43, 16.88,  0.15,-14.25,-0.13 },
         { { 0, 2, 0, 0, 0 }, -0.20, 0.00,  0.07,  0.00, -0.06, 0.00 },
         { { 2, 0, 0,-2,-1 },  0.05, 0.00, -0.02,  0.00,  0.01, 0.00 },
         { { 2, 0, 0,-2, 0 }, -0.56, 0.01,  0.17,  0.00, -0.14, 0.00 },
         { { 2, 0, 0,-2, 1 },  0.04, 0.00, -0.01,  0.00,  0.01, 0.00 },
         { { 0,-1, 2,-2, 1 }, -0.05, 0.00,  0.01,  0.00, -0.01, 0.00 },
         { { 0, 1, 0, 0,-1 },  0.09, 0.00, -0.02,  0.00,  0.01, 0.00 },
         { { 0,-1, 2,-2, 2 },  0.82,-0.01, -0.14,  0.00,  0.12, 0.00 },
         { { 0, 1, 0, 0, 0 },-15.65, 0.15,  2.69,  0.03, -2.27,-0.02 },
         { { 0, 1, 0, 0, 1 }, -0.14, 0.00,  0.02,  0.00, -0.02, 0.00 },
         { { 1, 0, 0,-1, 0 },  0.03, 0.00,  0.00,  0.00,  0.00, 0.00 },
         { { 2, 0,-2, 0, 0 }, -0.14, 0.00, -0.02,  0.00,  0.02, 0.00 },
         { {-2, 0, 2, 0, 1 },  0.43,-0.01, -0.02,  0.00,  0.02, 0.00 },
         { {-1, 1, 0, 1, 0 }, -0.04, 0.00,  0.00,  0.00,  0.00, 0.00 },
         { { 0, 0, 0, 0, 2 },  8.20, 0.11,  0.15,  0.00, -0.13, 0.00 },
         { { 0, 0, 0, 0, 1 }, -1689.54,-25.04,-15.62,0.23,13.18,-0.20}
         };

      // number of coefficients (62)
      static const int N = int(sizeof(C) / sizeof(Coeff));

      dUT = dld = dom = 0.0;
      for(int i=0; i<N; i++) {
         double arg(0.0);
         for(int j=0; j<5; j++)
            arg += double(C[i].nargs[j]) * args[j+1];    // NB args[0] is not used
         arg = ::fmod(arg,TWOPI);
         double sarg = ::sin(arg);
         double carg = ::cos(arg);

         dUT += C[i].dutsin * sarg + C[i].dutcos * carg;
         dld += C[i].dldsin * sarg + C[i].dldcos * carg;
         dom += C[i].domsin * sarg + C[i].domcos * carg;
      }

      // change units to seconds, seconds/day, and radians/sec
      dUT *= 1.e-4;
      dld *= 1.e-5;
      dom *= 1.e-14;

   }  // end correctEarthRotationZonalTides2003()

   //---------------------------------------------------------------------------------
   // Corrections to UT1 and length of day (LOD) due to subdiurnal librations
   // using USNO IERS2010 algorithm.
   // param args array of 6 fundamental arguments at T (CoordTransTime), in radians
   // param dUT correction to UT1mUTC in seconds
   // param dld correction to length of day in seconds/day
   void correctEarthRotationLibrations(const double args[6], double& dUT, double& dld)
      throw()
   {
      //static const double TWOPI(6.283185307179586476925287);
      //static const double ARCSEC_TO_RAD=4.848136811095359935899141e-6;
      //static const double ARCSEC_PER_CIRCLE(1296000.0);

      // Coefficients of the quasi semidiurnal terms in dUT1, dLOD
      // IERS Conventions (2010), Table 5.1b
      typedef struct {
         int nargs[6];
         double period, dutsin, dutcos, dldsin, dldcos;     // NB period is not used
      } Coeff;

      static const Coeff C[] = {
         { { 2, -2,  0, -2,  0, -2 }, 0.5377239,  0.05, -0.03,  -0.3,  -0.6 },
         { { 2,  0,  0, -2, -2, -2 }, 0.5363232,  0.06, -0.03,  -0.4,  -0.7 },
         { { 2, -1,  0, -2,  0, -2 }, 0.5274312,  0.35, -0.20,  -2.4,  -4.1 },
         { { 2,  1,  0, -2, -2, -2 }, 0.5260835,  0.07, -0.04,  -0.5,  -0.8 },
         { { 2,  0,  0, -2,  0, -1 }, 0.5175645, -0.07,  0.04,   0.5,   0.8 },
         { { 2,  0,  0, -2,  0, -2 }, 0.5175251,  1.75, -1.01, -12.2, -21.3 },
         { { 2,  1,  0, -2,  0, -2 }, 0.5079842, -0.05,  0.03,   0.3,   0.6 },
         { { 2,  0, -1, -2,  2, -2 }, 0.5006854,  0.04, -0.03,  -0.3,  -0.6 },
         { { 2,  0,  0, -2,  2, -2 }, 0.5000000,  0.76, -0.44,  -5.5,  -9.6 },
         { { 2,  0,  0,  0,  0,  0 }, 0.4986348,  0.21, -0.12,  -1.5,  -2.6 },
         { { 2,  0,  0,  0,  0, -1 }, 0.4985982,  0.06, -0.04,  -0.4,  -0.8 }
      };

      // number of coefficients (11)
      static const int N = int(sizeof(C) / sizeof(Coeff));

      dUT = dld = 0.0;
      for(int i=0; i<N; ++i) {
         double arg = 0.0;
         for(int j=0; j<6; ++j)
            arg += C[i].nargs[j] * args[j];
         arg = ::fmod(arg, EarthOrientation::TWOPI);
         double sarg = ::sin(arg);
         double carg = ::cos(arg);

         dUT += C[i].dutsin * sarg + C[i].dutcos * carg;
         dld += C[i].dldsin * sarg + C[i].dldcos * carg;
      }

      // convert to seconds
      dUT *= 1.e-6;        // seconds
      dld *= 1.e-6;        // seconds per day

   }  // end correctEarthRotationLibrations()

   //---------------------------------------------------------------------------------
   // private member functions, used internally

   //---------------------------------------------------------------------------------
   // Compute the 'coordinate transformation time' which is the time since
   // epoch J2000 = January 1 2000 12h UT = 2451545.0JD, divided by 36525 days.
   // This quantity is used throughout the terrestrial / inertial coordinate
   // transformations.
   // Throws if the TimeSystem conversion fails (if t.system==TimeSystem::Unknown)
   double EarthOrientation::CoordTransTime(EphTime ttag)
      throw(Exception)
   {
      try {
         EphTime t(ttag);
         t.convertSystemTo(TimeSystem::TT);
         //return (t.dMJD()-JulianEpoch)/36525.0;

         // do in a way to maximize precision
         int in = int(t.dMJD()-0.5)-intJulianEpoch;
         double frac = 0.5+t.secOfDay()/86400.0;
         if(frac > 1.0) frac -= 1.0;
         return (double(in)/36525.0 + frac/36525.0);
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //---------------------------------------------------------------------------------
   // Return mean longitude of lunar ascending node, in radians,
   // given T, the CoordTransTime at the Epoch of interest. (Ref: F5 pg 23)
   // valid for IERS1996
   double EarthOrientation::Omega(double T)
      throw()
   {
      return ::fmod(  450160.398036
               + T*(-6962890.2665         // diff Omega2003 only in .2665 vs .5431
               + T*(       7.4722
               + T*(       0.007702
               + T*(      -0.00005939)))), ARCSEC_PER_CIRCLE) * ARCSEC_TO_RAD;
   }

   //---------------------------------------------------------------------------------
   // Return mean longitude of lunar ascending node, in radians,
   // given T, the CoordTransTime at the Epoch of interest.
   // valid for IERS 2003, 2010
   double EarthOrientation::Omega2003(double T)
      throw()
   {
      return ::fmod(  450160.398036       // 125.04455501 * 3600
               + T*(-6962890.5431           // .2665
               + T*(       7.4722
               + T*(       0.007702
               + T*(      -0.00005939)))), ARCSEC_PER_CIRCLE) * ARCSEC_TO_RAD;
   }

   //---------------------------------------------------------------------------------
   // Return mean longitude of the moon - Omega, in radians,
   // given T, the CoordTransTime at the Epoch of interest. (Ref: F3 pg 23)
   // valid for IERS1996, IERS2003, IERS2010
   double EarthOrientation::F(double T)
      throw()
   {
      return ::fmod(    335779.526232  // 93.27209062 * 3600
               + T*(1739527262.8478
               + T*(       -12.7512
               + T*(        -0.001037
               + T*(         0.00000417)))), ARCSEC_PER_CIRCLE) * ARCSEC_TO_RAD;
   }

   //---------------------------------------------------------------------------------
   // Return mean elongation of the moon from the sun, in radians,
   // given T, the CoordTransTime at the Epoch of interest. (Ref: F4 pg 23)
   // valid for IERS1996, IERS2003
   double EarthOrientation::D(double T)
      throw()
   {
      return ::fmod(    1072260.703692  // 297.85019547 * 3600
                + T*(1602961601.2090
                + T*(        -6.3706
                + T*(         0.006593
                + T*(        -0.00003169)))), ARCSEC_PER_CIRCLE) * ARCSEC_TO_RAD;
   }

   //---------------------------------------------------------------------------------
   // Return mean anomaly of the moon, in radians,
   // given T, the CoordTransTime at the Epoch of interest. (Ref: F1 pg 23)
   // valid for IERS1996, IERS2003
   double EarthOrientation::L(double T)
      throw()
   {
      return ::fmod(     485868.249036    // 134.96340251 * 3600
                + T*(1717915923.2178
                + T*(        31.8792
                + T*(         0.051635
                + T*(        -0.00024470)))), ARCSEC_PER_CIRCLE ) * ARCSEC_TO_RAD;
   }

   //---------------------------------------------------------------------------------
   // Return mean anomaly of the sun, in radians, given T, the CoordTransTime at the
   // time of interest. (Ref: F2 pg 23)
   // valid for IERS1996, IERS2003
   double EarthOrientation::Lp(double T)
      throw()
   {
      return ::fmod(1287104.793048     // 357.52910918 * 3600
                + T*(129596581.0481
                + T*(       -0.5532
                + T*(        0.000136     // NB this has a minus sign in interp.f
                + T*(       -0.00001149)))), ARCSEC_PER_CIRCLE) * ARCSEC_TO_RAD;
   }

   //------------------------------------------------------------------------------
   // Compute the mean longitude of Mercury, in radians, given T, the CoordTransTime
   // at the time of interest. Valid for IERS2003, IERS2010
   // param T coordinate transformation time.
   // return LMe in radians.
   double EarthOrientation::LMe(double T)
      throw()
   {
      double lme(::fmod(4.402608842 + 2608.7903141574 * T, TWOPI));
      return lme;
   }

   //------------------------------------------------------------------------------
   // Compute the mean longitude of Venus, in radians, given T, the CoordTransTime
   // at the time of interest. Valid for IERS2003, IERS2010
   // param T coordinate transformation time.
   // return LV in radians.
   double EarthOrientation::LV(double T)
      throw()
   {
      double lv(::fmod(3.176146697 + 1021.3285546211 * T, TWOPI));
      return lv;
   }

   //------------------------------------------------------------------------------
   // Compute the mean longitude of Earth, in radians, given T, the CoordTransTime
   // at the time of interest. Valid for IERS2003, IERS2010
   // param T coordinate transformation time.
   // return LE in radians.
   double EarthOrientation::LE(double T)
      throw()
   {
      double le(::fmod(1.753470314 + 628.3075849991 * T, TWOPI));
      return le;
   }

   //------------------------------------------------------------------------------
   // Compute the mean longitude of Mars, in radians, given T, the CoordTransTime
   // at the time of interest. Valid for IERS2003, IERS2010
   // param T coordinate transformation time.
   // return LMa in radians.
   double EarthOrientation::LMa(double T)
      throw()
   {
      double lma(::fmod(6.203480913 + 334.0612426700 * T, TWOPI));
      return lma;
   }

   //------------------------------------------------------------------------------
   // Compute the mean longitude of Jupiter, in radians, given T, the
   // CoordTransTime at the time of interest. Valid for IERS2003, IERS2010
   // param T coordinate transformation time.
   // return LJ in radians.
   double EarthOrientation::LJ(double T)
      throw()
   {
      double lj(::fmod(0.599546497 + 52.9690962641 * T, TWOPI));
      return lj;
   }

   //------------------------------------------------------------------------------
   // Compute the mean longitude of Saturn, in radians, given T, the
   // CoordTransTime at the time of interest. Valid for IERS2003, IERS2010
   // param T coordinate transformation time.
   // return LS in radians.
   double EarthOrientation::LS(double T)
      throw()
   {
      double ls(::fmod(0.874016757 + 21.3299104960 * T, TWOPI));
      return ls;
   }

   //------------------------------------------------------------------------------
   // Compute the mean longitude of Uranus, in radians, given T, the
   // CoordTransTime at the time of interest. Valid for IERS2003, IERS2010
   // param T coordinate transformation time.
   // return LU in radians.
   double EarthOrientation::LU(double T)
      throw()
   {
      double lu(::fmod(5.481293872 + 7.4781598567 * T, TWOPI));
      return lu;
   }

   //------------------------------------------------------------------------------
   // Compute the mean longitude of Neptune, in radians, given T, the
   // CoordTransTime at the time of interest. Valid for IERS2003, IERS2010
   // param T coordinate transformation time.
   // return LN in radians.
   double EarthOrientation::LN(double T)
      throw()
   {
      double ln(::fmod(5.311886287 + 3.8133035638 * T, TWOPI));
      return ln;
   }

   //------------------------------------------------------------------------------
   // Compute the general precession in longitude, in radians,
   // given T, the CoordTransTime at the time of interest.
   // Valid for IERS2003
   // param T coordinate transformation time.
   // return Pa in radians.
   double EarthOrientation::Pa(double T)
      throw()
   {
      double pa((0.024381750 + 0.00000538691 * T) * T);
      return pa;
   }

   //---------------------------------------------------------------------------------
   // Compute eps, the obliquity of the ecliptic, in radians,
   // given T, the CoordTransTime at the time of interest.
   // throw if convention is not defined
   double EarthOrientation::Obliquity(double T) throw(Exception)
   {
      if(convention == IERSConvention::IERS1996)
         return Obliquity1996(T);
      else if(convention == IERSConvention::IERS2003)
         return Obliquity1996(T);       // same as 96
      else if(convention == IERSConvention::IERS2010)
         return Obliquity2010(T);
      else {
         Exception e("IERS convention is not defined");
         GPSTK_THROW(e);
      }
   }

   //---------------------------------------------------------------------------------
   // Compute Greenwich Mean Sidereal Time, or the Greenwich hour angle of
   // the mean vernal equinox (radians), given the coordinate time of interest,
   // and this object's UT1-UTC (sec), which comes from the IERS bulletin.
   // param t EphTime epoch of the rotation (UTC).
   // param reduced, bool true when UT1mUTC is 'reduced', meaning assumes
   //                 'no tides', as is the case with the NGA EOPs (default=F).
   // throw if convention is not defined
   double EarthOrientation::GMST(const EphTime& t, bool reduced) throw(Exception)
   {
      try {
         if(convention == IERSConvention::IERS1996)
            return GMST1996(t,UT1mUTC,reduced);
         else if(convention == IERSConvention::IERS2003)
            return GMST2003(t,UT1mUTC);
         else if(convention == IERSConvention::IERS2010)
            return GMST2010(t,UT1mUTC);
         else {
            Exception e("IERS convention is not defined");
            GPSTK_THROW(e);
         }
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //---------------------------------------------------------------------------------
   // Compute Greenwich Apparent Sidereal Time, or the Greenwich hour angle of
   // the true vernal equinox (radians), given the coordinate time of interest,
   // and this object's UT1-UTC (sec), which comes from the IERS bulletin.
   // param t EphTime epoch of the rotation.
   // param reduced, bool true when UT1mUTC is 'reduced', meaning assumes
   //                 'no tides', as is the case with the NGA EOPs (default=F).
   // throw if convention is not defined
   double EarthOrientation::GAST(const EphTime& t, bool reduced) throw(Exception)
   {
      try {
         if(convention == IERSConvention::IERS1996)
            return GAST1996(t,UT1mUTC,reduced);
         else if(convention == IERSConvention::IERS2003)
            return GAST2003(t,UT1mUTC);
         else if(convention == IERSConvention::IERS2010)
            return GAST2010(t,UT1mUTC);
         else {
            Exception e("IERS convention is not defined");
            GPSTK_THROW(e);
         }
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //---------------------------------------------------------------------------------
   // Generate transformation matrix (3X3 rotation) due to the EOPs (polar motion
   // angles xp and yp (arcseconds)), as found in the IERS bulletin;
   // see class EarthOrientation.
   // param t EphTime epoch of the rotation.
   // throw if convention is not defined
   Matrix<double> EarthOrientation::PolarMotionMatrix(const EphTime& t)
      throw(Exception)
   {
      if(convention == IERSConvention::IERS1996)
         return PolarMotionMatrix1996(xp,yp);
      else if(convention == IERSConvention::IERS2003)
         return PolarMotionMatrix2003(t,xp,yp);
      else if(convention == IERSConvention::IERS2010)
         return PolarMotionMatrix2003(t,xp,yp);      // valid also for 2010
      else {
         Exception e("IERS convention is not defined");
         GPSTK_THROW(e);
      }
   }

   //---------------------------------------------------------------------------------
   // Compute the precession matrix, a 3x3 rotation matrix, given T,
   // the coordinate transformation time at the time of interest
   // throw if convention is not defined
   Matrix<double> EarthOrientation::PrecessionMatrix(const EphTime& t)
      throw(Exception)
   {
      double T=CoordTransTime(t);
      if(convention == IERSConvention::IERS1996)
         return PrecessionMatrix1996(T);
      else if(convention == IERSConvention::IERS2003)
         return PrecessionMatrix2003(T);
      else if(convention == IERSConvention::IERS2010)
         return PrecessionMatrix2010(T);
      else {
         Exception e("IERS convention is not defined");
         GPSTK_THROW(e);
      }
   }

   //------------------------------------------------------------------------------
   // Nutation of the obliquity (deps) and of the longitude (dpsi)
   // param T,    the coordinate transformation time at the time of interest
   // param deps, nutation of the obliquity (output) in radians
   // param dpsi, nutation of the longitude (output) in radians
   // throw if convention is not defined
   // NB this is never called internally
   //void EarthOrientation::NutationAngles(double T, double& deps, double& dpsi)
   //   throw(Exception)
   //{
   //   if(convention == IERSConvention::IERS1996) {
   //      double om;
   //      NutationAngles1996(T,deps,dpsi,om);
   //   }
   //   if(convention == IERSConvention::IERS2003)
   //      NutationAngles2003(T,deps,dpsi);
   //   if(convention == IERSConvention::IERS2010)
   //      NutationAngles2010(T,deps,dpsi);
   //   else {
   //      Exception e("IERS convention is not defined");
   //      GPSTK_THROW(e);
   //   }
   //}

   //---------------------------------------------------------------------------------
   // Compute the nutation matrix, given coordinate transformation time T
   Matrix<double> EarthOrientation::NutationMatrix(const EphTime& t) throw(Exception)
   {
      try {
         double T = CoordTransTime(t);
         if(convention == IERSConvention::IERS1996)
            return NutationMatrix1996(T);
         else if(convention == IERSConvention::IERS2003)
            return NutationMatrix2003(T);
         else if(convention == IERSConvention::IERS2010)
            return NutationMatrix2010(T);
         else {
            Exception e("IERS convention is not defined");
            GPSTK_THROW(e);
         }
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //------------------------------------------------------------------------------
   // Generate precise transformation matrix (3X3 rotation) for Earth motion due
   // to precession, nutation and frame bias, at the given time of interest.
   // param t EphTime epoch of the rotation.
   // return 3x3 rotation matrix
   // throw if the TimeSystem conversion fails (if t.system==TimeSystem::Unknown)
   // throw if convention is not defined
   Matrix<double> EarthOrientation::PreciseEarthRotation(const EphTime& t)
      throw(Exception)
   {
      if(convention == IERSConvention::IERS1996)
         return Matrix<double>(3,3);                        // TD
      else if(convention == IERSConvention::IERS2003)
         return PreciseEarthRotation2003(CoordTransTime(t));
      else if(convention == IERSConvention::IERS2010)
         return PreciseEarthRotation2010(CoordTransTime(t));
      else {
         Exception e("IERS convention is not defined");
         GPSTK_THROW(e);
      }
   }

   //---------------------------------------------------------------------------------
   // Generate the full transformation matrix (3x3 rotation) relating the ECEF
   // frame to the conventional inertial frame.
   // Input is the time of interest; use this object's EOPs - the polar motion angles
   // xp and yp (arcseconds), and UT1-UTC (seconds) (as found in the IERS bulletin).
   // throw if convention is not defined
   Matrix<double> EarthOrientation::ECEFtoInertial(const EphTime& t, bool reduced)
      throw(Exception)
   {
      try {
         if(convention == IERSConvention::IERS1996)
            return ECEFtoInertial1996(t,xp,yp,UT1mUTC,reduced);
         else if(convention == IERSConvention::IERS2003)
            return ECEFtoInertial2003(t,xp,yp,UT1mUTC);
         else if(convention == IERSConvention::IERS2010)
            return ECEFtoInertial2010(t,xp,yp,UT1mUTC);
         else {
            Exception e("IERS convention is not defined");
            GPSTK_THROW(e);
         }
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //------------------------------------------------------------------------------
   // Compute the transformation from ECEF to the J2000 dynamical (inertial)
   // frame. This differs from the ECEFtoInertial transformation only by the
   // frame bias matrix. Only available in IERS2010.

   // TD this is not right
   //Matrix<double> EarthOrientation::ECEFtoJ2000(const EphTime& t, bool reduced)
   //   throw(Exception)
   //{
   //   try {
   //      if(convention != IERSConvention::IERS2010) {
   //         Exception e("ECEFtoJ2000 implemented only for IERS2010");
   //         GPSTK_THROW(e);
   //      }

   //      // get the frame bias matrix
   //      //double T=0.0;        // T at J2000
   //      double gamb, phib, psib, eps;
   //      //FukushimaWilliams(T, gamb, phib, psib, eps);
   //      //eps = 0.0;
   //      //Matrix<double> FB = FukushimaWilliams(gamb, phib, psib, eps);
   //      gamb = 0.052928 * ARCSEC_TO_RAD;
   //      phib = 0.006891 * ARCSEC_TO_RAD;
   //      psib = 0.041775 * ARCSEC_TO_RAD;
   //      Matrix<double> FB = rotation(-psib,3)*rotation(phib,1)*rotation(gamb,3);

   //      // get the full ECEFtoInertial, and back out the frame bias
   //      Matrix<double> T2C = ECEFtoInertial2010(t, xp, yp, UT1mUTC);

   //      return (FB*T2C);
   //   }
   //   catch(Exception& e) { GPSTK_RETHROW(e); }
   //}

   //------------------------------------------------------------------------------
   // private functions
   //------------------------------------------------------------------------------

   //------------------------------------------------------------------------------
   // Compute the locator s which gives the position of the CIO on the equator of
   // the CIP, given the coordinate transformation time T. Consistent with IAU 2000A
   // (IERS2003) precession-nutation, and P03 precession (IERS2010).
   // and the coordinates X,Y of the CIP. Derived in part from SOFA routine s00.c
   // for IERS2003 and s06.c for IERS2010.
   // param T, the coordinate transformation time at the time of interest
   // param X, the X coordinate of the CIP (input)
   // param Y, the Y coordinate of the CIP (input)
   // param which, the IERS convention to be used (input)
   // return S, the parameter that positions the CIO on the CIP equator in radians.
   double EarthOrientation::S(double T, double& X, double& Y, IERSConvention which)
      throw()
   {
      int i,j;
      double s,st[6],arg;

      // Fundamental arguments: all in radians
      double farg[8];
      farg[0] = L(T);     // mean anomaly of the moon
      farg[1] = Lp(T);    // mean anomaly of the sun
      farg[2] = F(T);     // mean longitude of the moon minus Omega
      farg[3] = D(T);     // mean elongation of the moon from the sun
      // mean longitude of lunar ascending node
      if(which==IERSConvention::IERS2010)
         farg[4] = Omega2003(T);
      else
         farg[4] = Omega(T);
      farg[5] = LV(T);    // mean longitude of Venus
      farg[6] = LE(T);    // mean longitude of Earth
      farg[7] = Pa(T);    // general precession in longitude

      // Based on table 5.2c IERS Tech Note 32 Chap 5 (tab5.2c.txt obtained from IERS)
      // also used SOFA routine s00.c

      // coefficients of polynomial in T for IERS2003
      static const double polycoeff[6] =
         { 94.00e-6, 3808.35e-6, -119.94e-6, -72574.09e-6, 27.70e-6, 15.61e-6 };
      // same for 2010
      static const double polycoeff2010[6] =
         { 94.00e-6, 3808.65e-6, -122.68e-6, -72574.11e-6, 27.98e-6, 15.62e-6 };

      typedef struct {
         int coeff[8];                 // coefficients of l,lp,f,d,o,lv,le,pa
         double sincoeff,coscoeff;     // coefficients of sine and cosine
      } Coeffs;

      // constant terms (T^0)
      static const Coeffs C0[] = {
         // indexes 1-10
         {{ 0,  0,  0,  0,  1,  0,  0,  0}, -2640.73e-6,   0.39e-6 },
         {{ 0,  0,  0,  0,  2,  0,  0,  0},   -63.53e-6,   0.02e-6 },
         {{ 0,  0,  2, -2,  3,  0,  0,  0},   -11.75e-6,  -0.01e-6 },
         {{ 0,  0,  2, -2,  1,  0,  0,  0},   -11.21e-6,  -0.01e-6 },
         {{ 0,  0,  2, -2,  2,  0,  0,  0},     4.57e-6,   0.00e-6 },
         {{ 0,  0,  2,  0,  3,  0,  0,  0},    -2.02e-6,   0.00e-6 },
         {{ 0,  0,  2,  0,  1,  0,  0,  0},    -1.98e-6,   0.00e-6 },
         {{ 0,  0,  0,  0,  3,  0,  0,  0},     1.72e-6,   0.00e-6 },
         {{ 0,  1,  0,  0,  1,  0,  0,  0},     1.41e-6,   0.01e-6 },
         {{ 0,  1,  0,  0, -1,  0,  0,  0},     1.26e-6,   0.01e-6 },
         // indexes 11-20
         {{ 1,  0,  0,  0, -1,  0,  0,  0},     0.63e-6,   0.00e-6 },
         {{ 1,  0,  0,  0,  1,  0,  0,  0},     0.63e-6,   0.00e-6 },
         {{ 0,  1,  2, -2,  3,  0,  0,  0},    -0.46e-6,   0.00e-6 },
         {{ 0,  1,  2, -2,  1,  0,  0,  0},    -0.45e-6,   0.00e-6 },
         {{ 0,  0,  4, -4,  4,  0,  0,  0},    -0.36e-6,   0.00e-6 },
         {{ 0,  0,  1, -1,  1, -8, 12,  0},     0.24e-6,   0.12e-6 },
         {{ 0,  0,  2,  0,  0,  0,  0,  0},    -0.32e-6,   0.00e-6 },
         {{ 0,  0,  2,  0,  2,  0,  0,  0},    -0.28e-6,   0.00e-6 },
         {{ 1,  0,  2,  0,  3,  0,  0,  0},    -0.27e-6,   0.00e-6 },
         {{ 1,  0,  2,  0,  1,  0,  0,  0},    -0.26e-6,   0.00e-6 },
         // indexes 21-30
         {{ 0,  0,  2, -2,  0,  0,  0,  0},     0.21e-6,   0.00e-6 },
         {{ 0,  1, -2,  2, -3,  0,  0,  0},    -0.19e-6,   0.00e-6 },
         {{ 0,  1, -2,  2, -1,  0,  0,  0},    -0.18e-6,   0.00e-6 },
         {{ 0,  0,  0,  0,  0,  8,-13, -1},     0.10e-6,  -0.05e-6 },
         {{ 0,  0,  0,  2,  0,  0,  0,  0},    -0.15e-6,   0.00e-6 },
         {{ 2,  0, -2,  0, -1,  0,  0,  0},     0.14e-6,   0.00e-6 },
         {{ 0,  1,  2, -2,  2,  0,  0,  0},     0.14e-6,   0.00e-6 },
         {{ 1,  0,  0, -2,  1,  0,  0,  0},    -0.14e-6,   0.00e-6 },
         {{ 1,  0,  0, -2, -1,  0,  0,  0},    -0.14e-6,   0.00e-6 },
         {{ 0,  0,  4, -2,  4,  0,  0,  0},    -0.13e-6,   0.00e-6 },
         // indexes 31-33
         {{ 0,  0,  2, -2,  4,  0,  0,  0},     0.11e-6,   0.00e-6 },
         {{ 1,  0, -2,  0, -3,  0,  0,  0},    -0.11e-6,   0.00e-6 },
         {{ 1,  0, -2,  0, -1,  0,  0,  0},    -0.11e-6,   0.00e-6 }
      };

      // First order terms (T)
      // NB C1[1].sincoeff=1.71e-6 in 2003 becomes 1.73e-6 in 2010 (2nd row)
      static const double C1_1_sincoeff2010 = 1.73e-6;
      static const Coeffs C1[] = {
         {{ 0,  0,  0,  0,  2,  0,  0,  0},    -0.07e-6,   3.57e-6 },
         {{ 0,  0,  0,  0,  1,  0,  0,  0},     1.71e-6,  -0.03e-6 },
         {{ 0,  0,  2, -2,  3,  0,  0,  0},     0.00e-6,   0.48e-6 }
      };

      // Second order terms (T^2)
      // NB C2[0].sincoeff=743.53e-6 in 2003 becomes 743.52e-6 in 2010 (1st row)
      static const double C2_0_sincoeff = 743.52e-6;
      static const Coeffs C2[] = {
         // indexes 1-10
         {{ 0,  0,  0,  0,  1,  0,  0,  0},   743.53e-6,  -0.17e-6 },
         {{ 0,  0,  2, -2,  2,  0,  0,  0},    56.91e-6,   0.06e-6 },
         {{ 0,  0,  2,  0,  2,  0,  0,  0},     9.84e-6,  -0.01e-6 },
         {{ 0,  0,  0,  0,  2,  0,  0,  0},    -8.85e-6,   0.01e-6 },
         {{ 0,  1,  0,  0,  0,  0,  0,  0},    -6.38e-6,  -0.05e-6 },
         {{ 1,  0,  0,  0,  0,  0,  0,  0},    -3.07e-6,   0.00e-6 },
         {{ 0,  1,  2, -2,  2,  0,  0,  0},     2.23e-6,   0.00e-6 },
         {{ 0,  0,  2,  0,  1,  0,  0,  0},     1.67e-6,   0.00e-6 },
         {{ 1,  0,  2,  0,  2,  0,  0,  0},     1.30e-6,   0.00e-6 },
         {{ 0,  1, -2,  2, -2,  0,  0,  0},     0.93e-6,   0.00e-6 },
         // indexes 11-20
         {{ 1,  0,  0, -2,  0,  0,  0,  0},     0.68e-6,   0.00e-6 },
         {{ 0,  0,  2, -2,  1,  0,  0,  0},    -0.55e-6,   0.00e-6 },
         {{ 1,  0, -2,  0, -2,  0,  0,  0},     0.53e-6,   0.00e-6 },
         {{ 0,  0,  0,  2,  0,  0,  0,  0},    -0.27e-6,   0.00e-6 },
         {{ 1,  0,  0,  0,  1,  0,  0,  0},    -0.27e-6,   0.00e-6 },
         {{ 1,  0, -2, -2, -2,  0,  0,  0},    -0.26e-6,   0.00e-6 },
         {{ 1,  0,  0,  0, -1,  0,  0,  0},    -0.25e-6,   0.00e-6 },
         {{ 1,  0,  2,  0,  1,  0,  0,  0},     0.22e-6,   0.00e-6 },
         {{ 2,  0,  0, -2,  0,  0,  0,  0},    -0.21e-6,   0.00e-6 },
         {{ 2,  0, -2,  0, -1,  0,  0,  0},     0.20e-6,   0.00e-6 },
         // indexes 21-25
         {{ 0,  0,  2,  2,  2,  0,  0,  0},     0.17e-6,   0.00e-6 },
         {{ 2,  0,  2,  0,  2,  0,  0,  0},     0.13e-6,   0.00e-6 },
         {{ 2,  0,  0,  0,  0,  0,  0,  0},    -0.13e-6,   0.00e-6 },
         {{ 1,  0,  2, -2,  2,  0,  0,  0},    -0.12e-6,   0.00e-6 },
         {{ 0,  0,  2,  0,  0,  0,  0,  0},    -0.11e-6,   0.00e-6 }
      };

      // Third order terms (T^3)
      static const Coeffs C3[] = {
         {{ 0,  0,  0,  0,  1,  0,  0,  0},     0.30e-6, -23.51e-6 },
         {{ 0,  0,  2, -2,  2,  0,  0,  0},    -0.03e-6,  -1.39e-6 },
         {{ 0,  0,  2,  0,  2,  0,  0,  0},    -0.01e-6,  -0.24e-6 },
         {{ 0,  0,  0,  0,  2,  0,  0,  0},     0.00e-6,   0.22e-6 }
      };
      static const Coeffs C32010[] = {
         {{ 0,  0,  0,  0,  1,  0,  0,  0},     0.30e-6, -23.42e-6 },
         {{ 0,  0,  2, -2,  2,  0,  0,  0},    -0.03e-6,  -1.46e-6 },
         {{ 0,  0,  2,  0,  2,  0,  0,  0},    -0.01e-6,  -0.25e-6 },
         {{ 0,  0,  0,  0,  2,  0,  0,  0},     0.00e-6,   0.23e-6 }
      };

      // Fourth order terms (T^4)
      static const Coeffs C4[] = {
         {{ 0,  0,  0,  0,  1,  0,  0,  0},    -0.26e-6,  -0.01e-6 }
      };

      // number of terms in each order
      const int n0 = int(sizeof C0 / sizeof(Coeffs));
      const int n1 = int(sizeof C1 / sizeof(Coeffs));
      const int n2 = int(sizeof C2 / sizeof(Coeffs));
      const int n3 = int(sizeof C3 / sizeof(Coeffs));
      const int n4 = int(sizeof C4 / sizeof(Coeffs));

      // initialize with the polynomial coefficients
      if(which != IERSConvention::IERS2010)
         for(i=0; i<6; ++i) st[i] = polycoeff[i];
      else
         for(i=0; i<6; ++i) st[i] = polycoeff2010[i];

      // do the sums
      for(i=n0-1; i>=0; --i) {            // order 0
         arg = 0.0;
         for(j=0; j<8; ++j)
            arg += C0[i].coeff[j] * farg[j];
         st[0] += C0[i].sincoeff * ::sin(arg) + C0[i].coscoeff * ::cos(arg);
      }

      for(i=n1-1; i>=0; --i) {            // order 1
         arg = 0.0;
         for(j=0; j<8; ++j)
            arg += C1[i].coeff[j] * farg[j];
         if(which == IERSConvention::IERS2010 && i==1)
            st[1] += C1_1_sincoeff2010 * ::sin(arg) + C1[i].coscoeff * ::cos(arg);
         else
            st[1] += C1[i].sincoeff * ::sin(arg) + C1[i].coscoeff * ::cos(arg);
      }

      for(i=n2-1; i>=0; --i) {            // order 2
         arg = 0.0;
         for(j=0; j<8; ++j)
            arg += C2[i].coeff[j] * farg[j];
         if(which == IERSConvention::IERS2010 && i==0)
            st[2] += C2_0_sincoeff * ::sin(arg) + C2[i].coscoeff * ::cos(arg);
         else
            st[2] += C2[i].sincoeff * ::sin(arg) + C2[i].coscoeff * ::cos(arg);
      }

                                          // order 3
      if(which != IERSConvention::IERS2010)               // 2003
         for(i=n3-1; i>=0; --i) {
            arg = 0.0;
            for(j=0; j<8; ++j)
               arg += C3[i].coeff[j] * farg[j];
            st[3] += C3[i].sincoeff * ::sin(arg) + C3[i].coscoeff * ::cos(arg);
         }
      else                                               // 2010
         for(i=n3-1; i>=0; --i) {
            arg = 0.0;
            for(j=0; j<8; ++j)
               arg += C32010[i].coeff[j] * farg[j];
            st[3] += C32010[i].sincoeff * ::sin(arg)+C32010[i].coscoeff * ::cos(arg);
         }
   
      for(i=n4-1; i>=0; --i) {            // order 4
         arg = 0.0;
         for(j=0; j<8; ++j)
            arg += C4[i].coeff[j] * farg[j];
         st[4] += C4[i].sincoeff * ::sin(arg) + C4[i].coscoeff * ::cos(arg);
      }

      // combine all the terms
      s = st[0] + (st[1] + (st[2] + (st[3] + (st[4] + st[5]*T)*T)*T)*T)*T;
      s *= ARCSEC_TO_RAD;
      s -= X*Y/2.0;

      return s;
   }

   //------------------------------------------------------------------------------
   // The position of the Terrestrial Ephemeris Origin (TEO) on the equator of the
   // Celestial Intermediate Pole (CIP), as given by the quantity s'.
   // Also called the Terrestrial Intermediate Origin (TIO). Valid 2003 and 2010
   // Ref. IERS Tech Note 32 Chap 5 Eqn 12 and IERS Tech Note 36 Chap 5 Eqn 5.13
   // param T Coordinate transformation time T.
   // return angle 's prime' in radians
   double EarthOrientation::Sprime(double T)
      throw()
   {
      double sp = -47.0e-6 * T * ARCSEC_TO_RAD;
      return sp;
   }

   //------------------------------------------------------------------------------
   // Compute the coordinates X,Y of the Celestial Intermediate Origin (CIO) using
   // a series based on IAU 2006 precession and IAU 2000A nutation (IERS 2010).
   // The coordinates form a unit vector that points towards the CIO; they include
   // the effects of frame bias, precession and nutation. cf. sofa xy06
   // Reference IERS(2010) Section 5.5.4
   // param T, the coordinate transformation time at the time of interest
   // param X, x coordinate of CIO
   // param Y, y coordinate of CIO
   void EarthOrientation::XYCIO(double& T, double& X, double& Y)
      throw()
   {
      // include data arrays : defines MAXPT
      #include "IERS2010CIOSeriesData.hpp"

      int i,j;
      double t;

      // compute and store powers of T
      double powsT[MAXPT+1];
      t = 1.0;
      for(i=0; i<=MAXPT; i++) {
         powsT[i] = t;
         t *= T;
      }

      // fundamental arguments
      double fa[14];
      fa[0] = L(T);     // mean anomaly of the moon
      fa[1] = Lp(T);    // mean anomaly of the sun
      fa[2] = F(T);     // mean longitude of the moon - Omega
      fa[3] = D(T);     // mean elongation of the moon from the sun
      fa[4] = Omega2003(T); // mean longitude of lunar ascending node
      fa[5] = LMe(T);   // mean longitude Mercury
      fa[6] = LV(T);    // mean longitude of Venus
      fa[7] = LE(T);    // mean longitude of Earth
      fa[8] = LMa(T);   // mean longitude Mars
      fa[9] = LJ(T);    // mean longitude Jupiter
      fa[10] = LS(T);   // mean longitude Saturn
      fa[11] = LU(T);   // mean longitude Uranus
      fa[12] = LN(T);   // mean longitude Neptune
      fa[13] = Pa(T);   // general precession in longitude

      // intermediate totals
      double xypoly[2]={0.0,0.0},xylunarsolar[2]={0.0,0.0},xyplanet[2]={0.0,0.0};

      // polynomial
      for(i=0; i<2; i++) {
         for(j=MAXPT; j>=0; j--)
            xypoly[i] += XYcoeff[i][j] * powsT[j];
      }

      // nutation planetary terms
      int ilast(NAmp),jlast;
      double sc[2];
      for(int ifreq=NFAP-1; ifreq >= 0; ifreq--) {
         // build the argument
         double arg(0.0);
         for(i=0; i<14; i++)
            if(nFAplanetary[ifreq][i])                         // don't add zero
               arg += double(nFAplanetary[ifreq][i]) * fa[i];
            
         // store sin and cos
         sc[0] = ::sin(arg);
         sc[1] = ::cos(arg);

         // amplitudes
         jlast = iamp[ifreq+NFALS];
         for(i=ilast; i >= jlast; i--) {
            j = i - jlast;          // coeff number
            xyplanet[jaxy[j]] += amp[i-1] * sc[jasc[j]] * powsT[japt[j]];
         }
         ilast = jlast - 1;
      }

      // NB ilast is maintained through this point

      // nutation lunar solar terms
      for(int ifreq=NFALS-1; ifreq >= 0; ifreq--) {
         // build the argument
         double arg(0.0);
         for(i=0; i<5; i++)
            if(nFAlunarsolar[ifreq][i])                         // don't add zero
               arg += double(nFAlunarsolar[ifreq][i]) * fa[i];
            
         // store sin and cos
         sc[0] = ::sin(arg);
         sc[1] = ::cos(arg);

         // amplitudes
         jlast = iamp[ifreq];
         for(i=ilast; i >= jlast; i--) {
            j = i - jlast;          // coeff number
            xylunarsolar[jaxy[j]] += amp[i-1] * sc[jasc[j]] * powsT[japt[j]];
         }
         ilast = jlast - 1;
      }

      X = xypoly[0] + (xylunarsolar[0]+xyplanet[0])*1.e-6;
      X *= ARCSEC_TO_RAD;
      Y = xypoly[1] + (xylunarsolar[1]+xyplanet[1])*1.e-6;
      Y *= ARCSEC_TO_RAD;
   }

   //---------------------------------------------------------------------------------
   // Starting with 2003 (and valid for 2010) conventions a new method for computing
   // the transformation fron ITRS to GCRS is provided by the Celestial Ephemeris
   // Origin (CEO) which is based on the Earth Rotation Angle. cf. sofa ERA00.c
   // param t EphTime time of interest
   // param UT1mUTC offset UT1-UTC
   // return Earth rotation angle in radians
   // throw if the TimeSystem conversion fails (if t.system==TimeSystem::Unknown)
   double EarthOrientation::EarthRotationAngle(const EphTime& t,
                                               const double& UT1mUTC)
      throw(Exception)
   {
      try {
         EphTime tUT1(t);
         tUT1.convertSystemTo(TimeSystem::UTC);
         tUT1 += UT1mUTC;

         // TN36 eqn 5.15
         //double days = tUT1.dMJD() - JulianEpoch;      // days since JE
         int idays(int(tUT1.dMJD()-0.5)-intJulianEpoch); // days = idays+frac
         double frac(0.5+tUT1.secOfDay()/86400.0);       // fractional part of days
         if(frac > 1.0) frac -= 1.0;

         // mod the terms with 1 individually to avoid numerical error
         double term1 = frac + 0.7790572732640 + 0.00273781191135448*frac;
         if(term1 > 1.0) term1 -= 1.0;
         double term2 = ::fmod(0.00273781191135448*double(idays), 1.0);
         double term = ::fmod(term1+term2, 1.0);

         double era = TWOPI * term;
         if(era > TWOPI) era -= TWOPI;

         return era;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //------------------------------------------------------------------------------
   // Equation of the equinoxes complementary terms, IAU 2000 (IERS 2003)
   // Note that GAST = GMST + EquationOfEquinoxes2003
   // param t EphTime epoch of interest
   // return the ee in radians
   // throw if the TimeSystem conversion fails (if t.system==TimeSystem::Unknown)
   // Based on IERS function EECT2000.f; all planets but Venus dropped b/c their
   // contribution is zero.
   double EarthOrientation::EquationOfEquinoxes2003(EphTime t)
      throw(Exception)
   {
      // first define the coefficients

      // number of integer coefficients and fundamental arguments
      const int N(8);

      // NB drop the lma,lju,lsa,lur,lne terms - all zero!
      typedef struct {
         // coefficients of l,lp,f,d,o,lme,lv,le,lma,lju,lsa,lur,lne,pa in arg
         //    *these were dropped:     *         *   *   *   *   *
         int coeff[N];
         // coefficients of sin(arg) and cos(arg) in the series
         double sincoeff,coscoeff;
      } Coeffs;

         // T^0  ...not very efficient
      static const Coeffs Czero[] = {
         // l   lp  f   d   o   lv  le  pa     c(sin)     c(cos)
         // 1-10
         {{ 0,  0,  0,  0,  1,  0,  0,  0}, 2640.96e-6, -0.39e-6 },
         {{ 0,  0,  0,  0,  2,  0,  0,  0},   63.52e-6, -0.02e-6 },
         {{ 0,  0,  2, -2,  3,  0,  0,  0},   11.75e-6,  0.01e-6 },
         {{ 0,  0,  2, -2,  1,  0,  0,  0},   11.21e-6,  0.01e-6 },
         {{ 0,  0,  2, -2,  2,  0,  0,  0},   -4.55e-6,  0.00e-6 },
         {{ 0,  0,  2,  0,  3,  0,  0,  0},    2.02e-6,  0.00e-6 },
         {{ 0,  0,  2,  0,  1,  0,  0,  0},    1.98e-6,  0.00e-6 },
         {{ 0,  0,  0,  0,  3,  0,  0,  0},   -1.72e-6,  0.00e-6 },
         {{ 0,  1,  0,  0,  1,  0,  0,  0},   -1.41e-6, -0.01e-6 },
         {{ 0,  1,  0,  0, -1,  0,  0,  0},   -1.26e-6, -0.01e-6 },
         // 11-20
         {{ 1,  0,  0,  0, -1,  0,  0,  0},   -0.63e-6,  0.00e-6 },
         {{ 1,  0,  0,  0,  1,  0,  0,  0},   -0.63e-6,  0.00e-6 },
         {{ 0,  1,  2, -2,  3,  0,  0,  0},    0.46e-6,  0.00e-6 },
         {{ 0,  1,  2, -2,  1,  0,  0,  0},    0.45e-6,  0.00e-6 },
         {{ 0,  0,  4, -4,  4,  0,  0,  0},    0.36e-6,  0.00e-6 },
         {{ 0,  0,  1, -1,  1, -8, 12,  0},   -0.24e-6, -0.12e-6 },
         {{ 0,  0,  2,  0,  0,  0,  0,  0},    0.32e-6,  0.00e-6 },
         {{ 0,  0,  2,  0,  2,  0,  0,  0},    0.28e-6,  0.00e-6 },
         {{ 1,  0,  2,  0,  3,  0,  0,  0},    0.27e-6,  0.00e-6 },
         {{ 1,  0,  2,  0,  1,  0,  0,  0},    0.26e-6,  0.00e-6 },
         // 21-30
         {{ 0,  0,  2, -2,  0,  0,  0,  0},   -0.21e-6,  0.00e-6 },
         {{ 0,  1, -2,  2, -3,  0,  0,  0},    0.19e-6,  0.00e-6 },
         {{ 0,  1, -2,  2, -1,  0,  0,  0},    0.18e-6,  0.00e-6 },
         {{ 0,  0,  0,  0,  0,  8,-13, -1},   -0.10e-6,  0.05e-6 },
         {{ 0,  0,  0,  2,  0,  0,  0,  0},    0.15e-6,  0.00e-6 },
         {{ 2,  0, -2,  0, -1,  0,  0,  0},   -0.14e-6,  0.00e-6 },
         {{ 1,  0,  0, -2,  1,  0,  0,  0},    0.14e-6,  0.00e-6 },
         {{ 0,  1,  2, -2,  2,  0,  0,  0},   -0.14e-6,  0.00e-6 },
         {{ 1,  0,  0, -2, -1,  0,  0,  0},    0.14e-6,  0.00e-6 },
         {{ 0,  0,  4, -2,  4,  0,  0,  0},    0.13e-6,  0.00e-6 },
         // 31-33
         {{ 0,  0,  2, -2,  4,  0,  0,  0},   -0.11e-6,  0.00e-6 },
         {{ 1,  0, -2,  0, -3,  0,  0,  0},    0.11e-6,  0.00e-6 },
         {{ 1,  0, -2,  0, -1,  0,  0,  0},    0.11e-6,  0.00e-6 }
      };

      // T^1  - do this manually
      //static const Coeffs C1[] = {
      //   {{ 0,  0,  0,  0,  1,  0,  0,  0,  0 }, -0.87e-6, +0.00e-6 }
      //};

      // number of terms in each order
      const int n0 = int(sizeof Czero / sizeof(Coeffs));
      //const int n1 = int(sizeof C1 / sizeof(Coeffs));

      try {
         // coordinate transformation time
         double T(CoordTransTime(t));
         // fundamental arguments l   lp  f   d   o   lme lv  le  pa
         double farg[N];

         LOG(DEBUG7) << "\nT = " << fixed << setprecision(15) << T;
         farg[0] = L(T);     // mean anomaly of the moon
         LOG(DEBUG7) << "L(T) = " << fixed << setprecision(15) << farg[0];
         farg[1] = Lp(T);    // mean anomaly of the sun
         LOG(DEBUG7) << "Lp(T) = " << fixed << setprecision(15) << farg[1];
         farg[2] = F(T);     // mean longitude of the moon - Omega
         LOG(DEBUG7) << "F(T) = " << fixed << setprecision(15) << farg[2];
         farg[3] = D(T);     // mean elongation of the moon from the sun
         LOG(DEBUG7) << "D(T) = " << fixed << setprecision(15) << farg[3];
         farg[4] = Omega2003(T); // mean longitude of lunar ascending node
         LOG(DEBUG7) << "Omega2003(T) = " << fixed << setprecision(15) << farg[4];
         farg[5] = LV(T);    // mean longitude of Venus
         LOG(DEBUG7) << "LV(T) = " << fixed << setprecision(15) << farg[5];
         farg[6] = LE(T);    // mean longitude of Earth
         LOG(DEBUG7) << "LE(T) = " << fixed << setprecision(15) << farg[6];
         farg[7] = Pa(T);    // general precession in longitude
         LOG(DEBUG7) << "Pa(T) = " << fixed << setprecision(15) << farg[7];

         // do the sums
         double ee(0.0);
         for(int i=n0-1; i>=0; --i) {            // order 0
            double arg(0.0);
            for(int j=0; j<N; ++j)
               if(Czero[i].coeff[j])
                  arg += Czero[i].coeff[j] * farg[j];
            ee += Czero[i].sincoeff * ::sin(arg);
            if(Czero[i].coscoeff) ee += Czero[i].coscoeff * ::cos(arg);
         }

         // the T^1 term
         ee += -0.87e-6 * ::sin(farg[4]) * T;

         // convert to radians
         ee *= ARCSEC_TO_RAD;

         return ee;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //------------------------------------------------------------------------------
   // Zonal tide terms for corrections of UT1mUTC when that quantity does not
   // include tides (e.g. NGA EOP), ref. IERS 1996 Ch. 8, table 8.1 pg 74.
   // param T,    the coordinate transformation time at the time of interest
   // param UT1mUT1R, the correction to UT1mUTC (seconds)
   // param dlodR, the correction to the length of day (seconds)
   // param domegaR, the correction to the Earth rotation rate (radians/second)
   void EarthOrientation::UT1mUTCTidalCorrections(double T,
                                                  double& UT1mUT1R,
                                                  double& dlodR,
                                                  double& domegaR)
         throw()
   {
      // Define (all doubles) and all in radians
      double o(Omega(T)); // mean longitude of lunar ascending node
      double f(F(T));     // mean longitude of the moon - Omega
      double d(D(T));     // mean elongation of the moon from the sun
      double l(L(T));     // mean anomaly of the moon
      double lp(Lp(T));   // mean anomaly of the sun
   
      //-----------------------------------------------------------------------
      // include code that forms UT1mUT1R dlodR domegaR
      #include "IERS1996UT1mUTCData.hpp"
   }

   //---------------------------------------------------------------------------------
   // Compute eps, the obliquity of the ecliptic, in radians,
   // given T, the CoordTransTime at the time of interest. IAU76 IAU80 for IERS1996,03
   double EarthOrientation::Obliquity1996(double T)
      throw()
   {
      //double eps;
      //// seconds of arc
      //eps = T*(-46.8150 + T*(-0.00059 + T*0.001813));
      //eps /= 3600.0;                       // convert to degrees
      //// degrees
      //eps += 23.4392911111111111;          // = 84381.448/3600.0
      //eps *= DEG_TO_RAD;
      //return eps;
      return (   84381.448
             + T*( -46.8150
             + T*(  -0.00059
             + T*(   0.001813)))) * ARCSEC_TO_RAD;
   }

   //---------------------------------------------------------------------------------
   // Compute eps, the obliquity of the ecliptic, in radians,
   // given T, the CoordTransTime at the time of interest.
   double EarthOrientation::Obliquity2010(double T)
      throw()
   {
      //double eps;
      //// mean obliquity cf. sofa obl06.c
      //eps = (84381.406 + (-46.836769 + (-0.0001831 + (0.00200340 +
      //      (-0.000000576 - 0.0000000434*T)*T)*T)*T)*T)*ARCSEC_TO_RAD;
      //return eps;
      return (  84381.406
             + T*(-46.836769
             + T*( -0.0001831
             + T*(  0.00200340
             + T*( -0.000000576
             + T*( -0.0000000434)))))) * ARCSEC_TO_RAD;
   }

   //------------------------------------------------------------------------------
   // Compute Greenwich Mean Sidereal Time, or the Greenwich hour angle of
   // the mean vernal equinox (radians), given the UT1 time of interest,
   // and UT1-UTC (sec), from the IERS bulletin. For IERS1996. cf sofa gmst82.c
   // param t EphTime epoch of the rotation.
   // param UT1mUTC, UT1-UTC in seconds, as found in the IERS bulletin.
   // return GMST in radians
   double EarthOrientation::GMST1996(EphTime t, double UT1mUTC, bool reduced)
      throw(Exception)
   {
      try {
         // convert to UTC first
         t.convertSystemTo(TimeSystem::UTC);

         // if reduced, compute tidal terms
         if(reduced) {
            double dlodR,domegaR,UT1mUT1R;
            UT1mUTCTidalCorrections(CoordTransTime(t), UT1mUT1R, dlodR, domegaR);
            UT1mUTC = UT1mUT1R-UT1mUTC;
         }
         // convert to UT1
         t += UT1mUTC;

         // dont use CoordTransTime() b/c UT1 is needed here, not TT
         double T((t.dMJD()-JulianEpoch)/36525.0);

         // Compute GMST in radians
         double G = -19089.45159 // first term is 24110.54841-43200. seconds
                     + T*(8640184.812866 + T*(0.093104 - T * 6.2e-6));
         // convert seconds to days
         G /= 86400.0;

         // this should be the same ... seconds (24060s = 6h 41min)
         //G = 24110.54841 + (8640184.812866 + (0.093104 - 6.2e-6*T)*T)*T;  sec
         //G /= 86400.0; // instead, divide the numbers above manually
         //G = 0.279057273264 + 100.0021390378009*T        // seconds/86400 = days
         //                  + (0.093104 - 6.2e-6*T)*T*T/86400.0;
         //G += (UT1mUTC + t.secOfDay())/86400.0;                      // days

         // add fraction of day
         double frac(0.5+t.secOfDay()/86400);           // fraction of day
         if(frac > 1.0) frac -= 1.0;
         G += frac;

         // convert to radians
         G *= TWOPI;                                                // radians
         G = ::fmod(G,TWOPI);
         if(G < 0.0) G += TWOPI;

         return G;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //------------------------------------------------------------------------------
   // Compute Greenwich Mean Sidereal Time, or the Greenwich hour angle of
   // the mean vernal equinox (radians), given the coordinate time of interest,
   // and UT1-UTC (sec), which comes from the IERS bulletin. For IERS2003
   // param t EphTime epoch of the rotation.
   // param UT1mUTC, UT1-UTC in seconds, as found in the IERS bulletin.
   double EarthOrientation::GMST2003(EphTime t, double UT1mUTC)
      throw(Exception)
   {
      try {
         // TT days since epoch
         double T(CoordTransTime(t)), G;
         double era = EarthRotationAngle(t,UT1mUTC);
         G = ::fmod(era + (0.014506 + (4612.15739966 + (1.39667721 + (-0.00009344
                         + 0.00001882*T)*T)*T)*T)*ARCSEC_TO_RAD, TWOPI);

         return G;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //------------------------------------------------------------------------------
   // Compute Greenwich Mean Sidereal Time, or the Greenwich hour angle of
   // the mean vernal equinox (radians), given the coordinate time of interest,
   // and UT1-UTC (sec), which comes from the IERS bulletin. For IERS2010
   // param t EphTime epoch of the rotation.
   // param UT1mUTC, UT1-UTC in seconds, as found in the IERS bulletin.
   double EarthOrientation::GMST2010(EphTime t, double UT1mUTC)
      throw(Exception)
   {
      try {
         double era = EarthRotationAngle(t,UT1mUTC);     // radians

         // IERS2010 - cf sofa gmst06.c and TN36 eqn 5.32
         double T = CoordTransTime(t);
         //double G = ::fmod(era+(0.014506 + (4612.156534 + (1.3915817 + (-0.00000044
         //    + (-0.000029956 + (-0.0000000368)*T)*T)*T)*T)*T)*ARCSEC_TO_RAD, TWOPI);
         //return G;
         return (era + (0.014506 + T*(4612.156534     // NB era in radians already
                                 + T*(1.3915817
                                 + T*(-0.00000044
                                 + T*(-0.000029956
                                 + T*(-0.0000000368)))))) * ARCSEC_TO_RAD);
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //---------------------------------------------------------------------------------
   // Helper to compute the Greenwich hour angle of the true vernal equinox, or
   // Greenwich Apparent Sidereal Time (GAST) in radians, for IERS1996,
   // given the (UT) time of interest t, and, where T = CoordTransTime(t),
   // o  = Omega(T) = mean longitude of lunar ascending node, in radians,
   // eps = the obliquity of the ecliptic, in radians,
   // dpsi = nutation in longitude (counted in the ecliptic),
   //                in seconds of arc.
   double EarthOrientation::gast1996(EphTime t,
                               double om, double eps, double dpsi, double UT1mUTC)
      throw(Exception)
   {
      try {
         double G = GMST1996(t,UT1mUTC,false);

         // add equation of equinoxes: dpsi, eps and Omega terms
         double ee = dpsi * ::cos(eps)
               + (0.00264  * ::sin(om) + 0.000063 * ::sin(2.0*om)) * ARCSEC_TO_RAD;

         LOG(DEBUG7) << "\nequequinox = " << fixed << setprecision(15)
               << showpos << 1.e3*ee*RAD_TO_DEG << " / 1.e3";
         LOG(DEBUG7) << "\nGMST = " << fixed << setprecision(15)
               << showpos << G*RAD_TO_DEG;

         return (G+ee);
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //------------------------------------------------------------------------------
   // Compute Greenwich Apparent Sidereal Time, or the Greenwich hour angle of
   // the true vernal equinox (radians), given the coordinate time of interest,
   // and UT1-UTC (sec), which comes from the IERS bulletin.
   // param t EphTime epoch of the rotation.
   // param UT1mUTC, UT1-UTC in seconds, as found in the IERS bulletin.
   //
   // GAST = Greenwich hour angle of the true vernal equinox
   // GAST = GMST + dpsi*cos(eps) + 0.00264" * sin(Omega) +0.000063" * sin(2*Omega)
   //    (these terms account for the accumulated precession and nutation in
   //       right ascension and minimize any discontinuity in UT1)
   //
   // GMST = Greenwich hour angle of the mean vernal equinox
   //      = Greenwich Mean Sideral Time
   //      = GMST0 + r*[UTC + (UT1-UTC)]
   // r    = is the ratio of universal to sidereal time
   //      = 1.002737909350795 + 5.9006E-11*T' - 5.9e-15*T'^2
   // T'   = days'/36525
   // days'= number of days elapsed since the Julian Epoch t0 (J2000)
   //      = +/-(integer+0.5)
   //   and
   // (UT1-UTC) (seconds) is taken from the IERS bulletin 
   //
   // GMST0 = GMST at 0h UT1
   //      = 6h 41min (50.54841+8640184.812866*T'+0.093104*T'^2-6.2E-6*T'^3)sec
   //
   // see pg 21 of the Reference (IERS 1996).
   double EarthOrientation::GAST1996(EphTime t, double UT1mUTC, bool reduced)
      throw(Exception)
   {
      try {
         double T(CoordTransTime(t));
         double omega,deps,dpsi,G;
         double eps(Obliquity1996(T)),epsa;

         NutationAngles1996(T,deps,dpsi,omega);        // deps is not used...

         // if reduced (NGA), correct for tides
         double UT1mUT1R,dlodR,domegaR;
         if(reduced) {
            UT1mUTCTidalCorrections(T, UT1mUT1R, dlodR, domegaR);
            G = gast1996(t, omega, eps, dpsi, UT1mUT1R-UT1mUTC);
         }
         else
            G = gast1996(t, omega, eps, dpsi, UT1mUTC);

         return G;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //------------------------------------------------------------------------------
   // Compute Greenwich Apparent Sidereal Time, or the Greenwich hour angle of
   // the true vernal equinox (radians), given the coordinate time of interest,
   // and UT1-UTC (sec), which comes from the IERS bulletin.
   // param t EphTime epoch of the rotation.
   // param UT1mUTC, UT1-UTC in seconds, as found in the IERS bulletin.
   double EarthOrientation::GAST2003(EphTime t, double UT1mUTC)
      throw(Exception)
   {
      try {
         double omega,deps,dpsi,G;
         double T(CoordTransTime(t));

         // precession and obliquity corrections (rad/century)
         double dpsipr,depspr;
         PrecessionRateCorrections2003(T, dpsipr, depspr);     // dpsipr not used

         // compute mean obliquity from IERS Tech Note 32 Chapter 5, eqn 32.
         double eps(Obliquity1996(T));
         // mean obliquity consistent with IAU 2000 P-N models
         double epsa(eps + depspr);

         NutationAngles2003(T,deps,dpsi);

         // Equation of the equinoxes.
         double ee = EquationOfEquinoxes2003(t);
         LOG(DEBUG7) << "\nee = " << fixed << setprecision(15) << showpos
               << 1.e6*ee*RAD_TO_DEG << " / 1.e6";
         ee = dpsi * ::cos(epsa) + ee;

         G = GMST2003(t, UT1mUTC) + ee;

         return G;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //------------------------------------------------------------------------------
   // Compute Greenwich Apparent Sidereal Time, or the Greenwich hour angle of
   // the true vernal equinox (radians), given the coordinate time of interest,
   // and UT1-UTC (sec), which comes from the IERS bulletin.
   // param t EphTime epoch of the rotation.
   // param UT1mUTC, UT1-UTC in seconds, as found in the IERS bulletin.
   double EarthOrientation::GAST2010(EphTime t, double UT1mUTC)
      throw(Exception)
   {
      try {
         Matrix<double> NutPreBias = PreciseEarthRotation2010(CoordTransTime(t));

         // extract X and Y coords of the CIP from the matrix
         // cf. sofa bpn2xy.c
         double X(NutPreBias(2,0)), Y(NutPreBias(2,1));

         // get T and the CIO locator s
         double T(CoordTransTime(t));
         double s(S(T,X,Y,IERSConvention::IERS2010));

         // get ERA(UT1)
         double era = EarthRotationAngle(t,UT1mUTC);

         // equation of the origins. cf. sofa eors.c
         double eo,ax,xs,ys,zs,p,q;
         ax = X / (1.0 + NutPreBias(2,2));
         xs = 1.0 - ax * X;
         ys = -ax * Y;
         zs = -X;
         p = NutPreBias(0,0)*xs + NutPreBias(0,1)*ys + NutPreBias(0,2)*zs;
         q = NutPreBias(1,0)*xs + NutPreBias(1,1)*ys + NutPreBias(1,2)*zs;
         eo = ((p != 0) || (q != 0)) ? s - ::atan2(q,p) : s;

         //double G(era-eo);
         //G = ::fmod(G, TWOPI);
         //return G;
         return ::fmod(era-eo, TWOPI);
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //---------------------------------------------------------------------------------
   // Generate transformation matrix (3X3 rotation) due to the polar motion
   // angles xp and yp (arcseconds), as found in the IERS bulletin;
   // see class EarthOrientation.
   // param xp, Earth wobble in arcseconds, as found in the IERS bulletin.
   // param yp, Earth wobble in arcseconds, as found in the IERS bulletin.
   // return Matrix<double>(3,3) rotation matrix
   Matrix<double> EarthOrientation::PolarMotionMatrix1996(double xp, double yp)
      throw()
   {
      xp *= ARCSEC_TO_RAD;
      yp *= ARCSEC_TO_RAD;
      Matrix<double> R1,R2;
      R1 = rotation(-yp,1);
      R2 = rotation(-xp,2);
      return (R2*R1);
   }

   //---------------------------------------------------------------------------------
   // Generate transformation matrix (3X3 rotation) due to the polar motion
   // angles xp and yp (arcseconds), as found in the IERS bulletin;
   // The returned matrix R transforms the CIP into TRS:  V(TRS) = R * V(CIP).
   // Also see class EarthOrientation, sofa pom00. Also valid for IERS2010.
   // param t EphTime epoch of the rotation.
   // param xp, Earth wobble in arcseconds, as found in the IERS bulletin.
   // param yp, Earth wobble in arcseconds, as found in the IERS bulletin.
   // return Matrix<double>(3,3) rotation matrix CIP -> TRS
   Matrix<double> EarthOrientation::PolarMotionMatrix2003(EphTime t,
                                                         double xp, double yp)
      throw()
   {
      double sp(Sprime(t));      // parameter s' provides position of TEO on CIP
      LOG(DEBUG7) << "\nsp = " << fixed << setprecision(15) << setw(18) << sp
            << " with T = " << CoordTransTime(t);
      xp *= ARCSEC_TO_RAD;
      yp *= ARCSEC_TO_RAD;
      Matrix<double> R1,R2,R3;
      R3 = rotation(sp, 3);
      R2 = rotation(-xp, 2);
      R1 = rotation(-yp, 1);
      return (R1*R2*R3);
   }

   //------------------------------------------------------------------------------
   // Compute Fukushima-Williams angles for computing nutation, frame bias and
   // precession matrices in IERS2010; cf. FukushimaWilliams().
   // NB. fourth angle is Obliquity.
   void EarthOrientation::FukushimaWilliams(double T, double& gamb, double& phib,
                                                       double& psib, double& eps)
      throw()
   {
      // P03 bias+precession angles cf. sofa pfw06.c
      gamb = (-0.052928 + (10.556378 + (0.4932044 + (-0.00031238 +
             (-0.000002788 + 0.0000000260*T)*T)*T)*T)*T)*ARCSEC_TO_RAD;
      phib = (84381.412819 + (-46.811016 + (0.0511268 + (0.00053289 +
             (-0.000000440 - 0.0000000176*T)*T)*T)*T)*T)*ARCSEC_TO_RAD;
      psib = (-0.041775 + (5038.481484 + (1.5584175 + (-0.00018522 +
             (-0.000026452 - 0.0000000148*T)*T)*T)*T)*T)*ARCSEC_TO_RAD;

      // obliquity
      eps = Obliquity2010(T);
   }

   //---------------------------------------------------------------------------------
   // Generate any of B = frame bias matrix
   //                PB = Precession*Bias matrix
   //               NPB = Nutation*Precession*Bias matrix
   // for IERS 2010, given the four F-W angles with caviats, as follows.
   // Get   B by passing the full F-W angles at J2000 (T=0).
   // Get  PB by passing the full F-W angles at time time of interest.
   // Get NPB by passing the full F-W angles at time time of interest with
   //    nutation angle corrections (NutationAngles2010()). Specifically,
   //     FukushimaWilliams(T, gamb, phib, psib, eps);
   //     NutationAngles2010(T, deps, dpsi);
   //     NPB = FukushimaWilliams(gamb, phib, psib+dpsi, eps+deps);
   // Thus the precession matrix is computed as PB * transpose(B), and
   //      the nutation matrix is computes as N = NPB * transpose(PB).
   // param gamb F-W angle
   // param phib F-W angle
   // param psib F-W angle
   // param epsa F-W angle, the obliquity
   // return 3x3 rotation matrix
   Matrix<double> EarthOrientation::FukushimaWilliams(double gamb, double phib,
                                                       double psib, double epsa)
      throw()
   {
      Matrix<double> R;
      R = rotation(-epsa,1) *
          rotation(-psib,3) *
          rotation(phib,1) *
          rotation(gamb,3);
      return R;
   }

   //---------------------------------------------------------------------------------
   // Nutation of the obliquity (deps) and of the longitude (dpsi), IERS 1996
   // model (ref SOFA nut80.c - not found in Tech Note 21!).
   // param T,    the coordinate transformation time at the time of interest
   // param deps, nutation of the obliquity, in radians (output)
   // param dpsi, nutation of the longitude, in radians (output)
   // param om, longitude mean ascending node of lunar orbit, from mean equinox
   void EarthOrientation::NutationAngles1996(double T,
                                              double& deps, double& dpsi, double& om)
      throw()
   {
      // include coefficients array; defines coeff[Ncoeff]
      #include "IERS1996NutationData.hpp"

      // Define fundamental arguments in radians - these do not appear elsewhere (!?)
      // Mean longitude of Moon minus mean longitude of Moon's perigee
      double el = ::fmod((485866.733 + (715922.633 + (31.310 + 0.064 * T) * T) * T)
                  * ARCSEC_TO_RAD + ::fmod(1325.0 * T, 1.0)*TWOPI, TWOPI);

      // Mean longitude of Sun minus mean longitude of Sun's perigee
      double elp = ::fmod((1287099.804 + (1292581.224 + (-0.577 - 0.012 * T) * T) * T)
                     * ARCSEC_TO_RAD + ::fmod(99.0 * T, 1.0)*TWOPI, TWOPI);

      // Mean longitude of Moon minus mean longitude of Moon's node
      double f = ::fmod((335778.877 + (295263.137 + (-13.257 + 0.011 * T) * T) * T)
                  * ARCSEC_TO_RAD + ::fmod(1342.0 * T, 1.0)*TWOPI, TWOPI);

      // Mean elongation of Moon from Sun
      double d = ::fmod((1072261.307 + (1105601.328 + (-6.891 + 0.019 * T) * T) * T)
                  * ARCSEC_TO_RAD + ::fmod(1236.0 * T, 1.0)*TWOPI, TWOPI);

      // Longitude of the mean ascending node of the lunar orbit on the
      // ecliptic, measured from the mean equinox of date
      om = ::fmod((450160.280 + (-482890.539 + (7.455 + 0.008 * T) * T) * T)
                  * ARCSEC_TO_RAD + ::fmod(-5.0 * T, 1.0)*TWOPI, TWOPI);

      // sum the series
      double arg,scoeff,ccoeff;
      deps = dpsi = 0.0;
      for(int i=Ncoeff-1; i>=0; i--) {
         // form argument
         double arg = coeff[i].nl * el
                    + coeff[i].nlp * elp
                    + coeff[i].nf * f
                    + coeff[i].nd * d
                    + coeff[i].nom * om;

         // sine and cosine terms
         scoeff = coeff[i].sp + coeff[i].spt*T;
         ccoeff = coeff[i].ce + coeff[i].cet*T;
         if(scoeff != 0.0) dpsi += scoeff * ::sin(arg);
         if(ccoeff != 0.0) deps += ccoeff * ::cos(arg);
      }

      // convert from 0.1 milliarcseconds to radians
      deps *= ARCSEC_TO_RAD * 1.e-4;
      dpsi *= ARCSEC_TO_RAD * 1.e-4;

   }

   //---------------------------------------------------------------------------------
   // Nutation of the obliquity (deps) and of the longitude (dpsi), IERS 2003 or
   // IAU 2000A model (MHB2000 luni-solar and planetary nutation without free core n).
   // param T,    the coordinate transformation time at the time of interest
   // param deps, nutation of the obliquity, in radians (output)
   // param dpsi, nutation of the longitude, in radians (output)
   void EarthOrientation::NutationAngles2003(double T, double& deps, double& dpsi)
      throw()
   {
      // sin and cos coefficients have units 0.1 microarcsec = 1e-7as
      const double COEFF_TO_RAD(ARCSEC_TO_RAD*1.0e-7);

      // -----------------------------------------
      // include huge static arrays of coefficients
      #include "IERS2003NutationData.hpp"

      // -----------------------------------------
      // Lunar-Solar nutation
      // fundamental arguments, in radians
      double l(L(T));                 // mean anomaly of the moon

      double lp(::fmod(  1287104.79305    // mean anomaly of the sun MHB2000 value
                  + T*(129596581.0481
                  + T*(       -0.5532
                  + T*(        0.000136
                  + T*(       -0.00001149)))), ARCSEC_PER_CIRCLE) * ARCSEC_TO_RAD);

      double f(::fmod(    335779.526232 // mean longitude of moon minus Omega MHB2000
                 + T*(1739527262.8478
                 + T*(       -12.7512
                 + T*(        -0.001037
                 + T*(         0.00000417)))), ARCSEC_PER_CIRCLE) * ARCSEC_TO_RAD);

      double d(::fmod(   1072260.70369     // mean elongation moon from sun MHB2000
                 + T*(1602961601.2090
                 + T*(        -6.3706
                 + T*(         0.006593
                 + T*(        -0.00003169)))), ARCSEC_PER_CIRCLE) * ARCSEC_TO_RAD);

      double Om(Omega2003(T));        // mean longitude of lunar ascending node

      // initialize
      deps = dpsi = 0.0;
      int i;
      double arg,sina,cosa;

      // form the LS series
      for(i=NLS-1; i>=0; --i) {
         // argument
         arg = ::fmod(LSCoeff[i].nl * l +
                      LSCoeff[i].nlp * lp +
                      LSCoeff[i].nf * f +
                      LSCoeff[i].nd * d +
                      LSCoeff[i].nom * Om, TWOPI);
         sina = ::sin(arg);
         cosa = ::cos(arg);
         // term
         deps += (LSCoeff[i].ce + LSCoeff[i].cet * T) * cosa + LSCoeff[i].se * sina;
         dpsi += (LSCoeff[i].sp + LSCoeff[i].spt * T) * sina + LSCoeff[i].cp * cosa;
      }

      // -----------------------------------------
      // Planetary nutation
      // fundamental arguments, in radians.

      // NB MHB2000 values are very close to IERS2003; follow SOFA here TD ??
      // mean anomaly of the moon MHB2000 value
      l = ::fmod(2.35555598 + 8328.6914269554 * T, TWOPI);
      // mean longitude of the moon minus Omega MHB2000 value
      f = ::fmod(1.627905234 + 8433.466158131 * T, TWOPI);
      // mean elongation of the Moon from the Sun MHB2000 value
      d = ::fmod(5.198466741 + 7771.3771468121 * T, TWOPI);
      // mean longitude of lunar ascending node MHB2000 value
      Om = ::fmod(2.18243920 - 33.757045 * T, TWOPI);

      // mean longitude Mercury
      double lme(LMe(T));
      // mean longitude of Venus
      double lve(LV(T));
      // mean longitude of Earth
      double lea(LE(T));
      // mean longitude Mars
      double lma(LMa(T));
      // mean longitude Jupiter
      double lju(LJ(T));
      // mean longitude Saturn
      double lsa(LS(T));
      // mean longitude Uranus
      double lur(LU(T));
      // mean longitude Neptune
      double lne(::fmod(5.321159000 + 3.8127774000 * T, TWOPI));
      // general precession in longitude
      double pa(Pa(T));

      // form the planetary series
      for(i=NP-1; i>=0; --i) {
         // argument
         arg = ::fmod(PCoeff[i].nl * l +
                    PCoeff[i].nf * f +
                    PCoeff[i].nd * d +
                    PCoeff[i].nom * Om +
                    PCoeff[i].nme * lme +
                    PCoeff[i].nve * lve +
                    PCoeff[i].nea * lea +
                    PCoeff[i].nma * lma +
                    PCoeff[i].nju * lju +
                    PCoeff[i].nsa * lsa +
                    PCoeff[i].nur * lur +
                    PCoeff[i].nne * lne +
                    PCoeff[i].npa * pa, TWOPI);
         sina = ::sin(arg);
         cosa = ::cos(arg);
         // term
         deps += PCoeff[i].ce * cosa + PCoeff[i].se * sina;
         dpsi += PCoeff[i].sp * sina + PCoeff[i].cp * cosa;
      }

      // convert 0.1microarcsec to radians
      deps *= COEFF_TO_RAD;
      dpsi *= COEFF_TO_RAD;

      return;
   }

   //---------------------------------------------------------------------------------
   // Nutation of the obliquity (deps) and of the longitude (dpsi), IERS 2010 or
   // IAU 2000A model (MHB2000 luni-solar and planetary nutation without free core n)
   // with P03 adjustments. cf. sofa nut06a.c
   // param T,    the coordinate transformation time at the time of interest
   // param deps, nutation of the obliquity, in radians (output)
   // param dpsi, nutation of the longitude, in radians (output)
   void EarthOrientation::NutationAngles2010(double T, double& deps, double& dpsi)
      throw()
   {
      NutationAngles2003(T,deps,dpsi);
      double fj2(-2.7774e-6 * T);
      dpsi *= (1.0+0.4697e-6 + fj2);
      deps *= (1.0+fj2);
   }

   //---------------------------------------------------------------------------------
   // Compute the nutation matrix, given
   // eps,  the obliquity of the ecliptic, in radians,
   // dpsi, the nutation in longitude (counted in the ecliptic), in radians
   // deps, the nutation in obliquity, in radians.
   Matrix<double> EarthOrientation::NutationMatrix(double eps,
                                                   double dpsi, double deps)
      throw()
   {
      Matrix<double> R1 = rotation(eps, 1);
      Matrix<double> R2 = rotation(-dpsi, 3);
      Matrix<double> R3 = rotation(-(eps+deps), 1);
      return (R3*R2*R1);
   }

   //------------------------------------------------------------------------------
   // IERS1996 nutation matrix, a 3x3 rotation matrix, given
   // param T, the coordinate transformation time at the time of interest
   // return nutation matrix Matrix<double>(3,3)
   Matrix<double> EarthOrientation::NutationMatrix1996(double T)
      throw()
   {
      double eps(Obliquity1996(T)), deps, dpsi, om;

      NutationAngles1996(T,deps,dpsi,om);
      return NutationMatrix(eps,dpsi,deps);
   }

   //------------------------------------------------------------------------------
   // IERS2003 nutation matrix, a 3x3 rotation matrix
   // (including the frame bias matrix), given
   // param T, the coordinate transformation time at the time of interest
   // return nutation matrix Matrix<double>(3,3)
   Matrix<double> EarthOrientation::NutationMatrix2003(double T)
      throw()
   {
      double eps(Obliquity1996(T)), deps, dpsi;    // same as Obliquity2003
      NutationAngles2003(T,deps,dpsi);

      // Precession rate contributions with respect to IAU 2000
      // Precession and obliquity corrections (radians)
      double depspr = -0.02524 * ARCSEC_TO_RAD * T;
      eps += depspr;

      return NutationMatrix(eps,dpsi,deps);
   }

   //------------------------------------------------------------------------------
   // IERS2010 nutation matrix, a 3x3 rotation matrix, given
   // param T, the coordinate transformation time at the time of interest;
   // cf. FukushimaWilliams().
   // return nutation matrix Matrix<double>(3,3)
   Matrix<double> EarthOrientation::NutationMatrix2010(double T)
      throw()
   {
      double deps,dpsi,eps;

      //// get the F-W angles at epoch
      //double gamb,phib,psib;
      //FukushimaWilliams(T, gamb, phib, psib, eps);
      //
      //// get nutation angles
      //NutationAngles2010(T,deps,dpsi);
      //
      //// construct nutation x precession x frame bias matrix
      //// NB this is the same as PreciseEarthRotation2010(T)
      //Matrix<double> NPB = FukushimaWilliams(gamb,phib,psib+dpsi,eps+deps);
      //
      //// now get PB alone - see FukushimaWilliams()
      //Matrix<double> PB = FukushimaWilliams(gamb,phib,psib,eps);
      //
      //return (NPB * transpose(PB));

      // same result
      NutationAngles2010(T,deps,dpsi);
      return NutationMatrix(Obliquity2010(T),dpsi,deps);
   }

   //------------------------------------------------------------------------------
   // Compute the IERS1996 precession matrix, a 3x3 rotation matrix, given
   // param T, the coordinate transformation time at the time of interest
   // return precession matrix Matrix<double>(3,3)
   Matrix<double> EarthOrientation::PrecessionMatrix1996(double T)
      throw()
   {
         // IAU76 - ref McCarthy - seconds of arc
         // NB t0==0 in sofa prec76.c - TD why do they do these things?
      double TAR(T*ARCSEC_TO_RAD);     // convert to radians
      double zeta  = TAR*(2306.2181 + T*(0.30188 + T*0.017998));
      double theta = TAR*(2004.3109 - T*(0.42665 + T*0.041833));
      double z     = TAR*(2306.2181 + T*(1.09468 + T*0.018203));

      Matrix<double> R1 = rotation(-zeta, 3);
      Matrix<double> R2 = rotation(theta, 2);
      Matrix<double> R3 = rotation(-z, 3);
      Matrix<double> P = R3*R2*R1;

      return P;
   }

   //------------------------------------------------------------------------------
   // Compute the IERS2003 precession matrix, a 3x3 rotation matrix, given
   // param T, the coordinate transformation time at the time of interest
   // Includes the frame bias matrix. cf sofa bp00.c
   // return precession matrix Matrix<double>(3,3)
   Matrix<double> EarthOrientation::PrecessionMatrix2003(double T)
      throw()
   {
      // obliquity at the J2000.0 epoch
      static const double eps0(84381.448 * ARCSEC_TO_RAD);
      LOG(DEBUG7) << "\nobliquity at J2000 eps0 = " << fixed << setprecision(15)
         << showpos << eps0;

      // frame bias corrections in longitude and obliquity
      static const double psibias = -0.041775  * ARCSEC_TO_RAD;
      static const double epsbias = -0.0068192 * ARCSEC_TO_RAD;
      // ICRS right ascension of the J2000.0 equinox
      static const double raeps0 = -0.0146 * ARCSEC_TO_RAD;
      LOG(DEBUG7) << "frame bias psi = " << fixed << setprecision(15)
         << showpos << psibias;
      LOG(DEBUG7) << "frame bias eps = " << fixed << setprecision(15)
         << showpos << epsbias;
      LOG(DEBUG7) << "frame bias dra = " << fixed << setprecision(15)
         << showpos << raeps0;

      // precession angles
      double psia((5038.7784 + (-1.07259 + (-0.001147)*T)*T)*T*ARCSEC_TO_RAD);
      double epsa(      eps0 + ((0.05127 + (-0.007726)*T)*T)*T*ARCSEC_TO_RAD);
      double chia((  10.5526 + (-2.38064 + (-0.001125)*T)*T)*T*ARCSEC_TO_RAD);
      LOG(DEBUG7) << "\nprecession angle psi = " << fixed << setprecision(15)
         << showpos << psia;
      LOG(DEBUG7) << "precession angle eps = " << fixed << setprecision(15)
         << showpos << epsa;
      LOG(DEBUG7) << "precession angle chi = " << fixed << setprecision(15)
         << showpos << chia;

      // Precession rate contributions with respect to IAU 2000 p-n models
      // Precession and obliquity corrections (radians)  cf sofa pr00.c
      double dpsipr, depspr;
      PrecessionRateCorrections2003(T, dpsipr, depspr);

      // Apply precession corrections
      LOG(DEBUG7) << "precession rate = " << fixed << setprecision(15) << showpos
         << dpsipr << " " << depspr;
      psia += dpsipr;
      epsa += depspr;

      // Frame bias matrix
      Matrix<double> R1 = rotation(raeps0, 3);
      Matrix<double> R2 = rotation(psibias * ::sin(eps0), 2);
      Matrix<double> R3 = rotation(-epsbias, 1);
      Matrix<double> FrameBias(R3*R2*R1);
      LOG(DEBUG7) << "\nframe bias matrix:\n" << fixed << setprecision(15) << showpos
         << FrameBias;

      // Precession matrix
      R1 = rotation(eps0, 1);
      R2 = rotation(-psia, 3);
      R3 = rotation(-epsa, 1);
      Matrix<double> R4 = rotation(chia, 3);
      Matrix<double> Precess(R4*R3*R2*R1);
      LOG(DEBUG7) << "\nprecession matrix:\n" << fixed << setprecision(15) << showpos
         << Precess;

      LOG(DEBUG7) << "\nprecession*framebias matrix:\n" << fixed << setprecision(15)
         << showpos << (Precess*FrameBias);

      return (Precess*FrameBias);
   }

   //------------------------------------------------------------------------------
   // Compute the IERS2003 precession and obliquity rate corrections, IAU 2000
   // param T, the coordinate transformation time at the time of interest
   // return precession, obliquity corrections in radians
   void EarthOrientation::PrecessionRateCorrections2003(double T,
                                                         double& dpsi, double& deps)
      throw()
   {
      // Precession rate contributions with respect to IAU 2000
      // Precession and obliquity corrections (radians)
      dpsi = -0.29965 * ARCSEC_TO_RAD * T;
      deps = -0.02524 * ARCSEC_TO_RAD * T;
   }

   //------------------------------------------------------------------------------
   // IERS2010 frame bias matrix, a 3x3 rotation matrix; cf. FukushimaWilliams().
   // return frame bias matrix Matrix<double>(3,3)
   Matrix<double> EarthOrientation::BiasMatrix2010(void)
      throw()
   {
      // get F-W angles at J2000
      double gamb,phib,psib,epsa;
      FukushimaWilliams(0.0, gamb, phib, psib, epsa);

      // frame bias matrix
      return FukushimaWilliams(gamb, phib, psib, epsa);
   }

   //---------------------------------------------------------------------------------
   // Compute the IERS2010 precession matrix, a 3x3 rotation matrix, given
   // param T, the coordinate transformation time at the time of interest
   // Does not include the frame bias matrix. Cf. FukushimaWilliams().
   // return precession matrix Matrix<double>(3,3)
   Matrix<double> EarthOrientation::PrecessionMatrix2010(double T)
      throw()
   {
      // the F-W angles
      double gamb,phib,psib,epsa;

      // get frame bias matrix
      Matrix<double> B = BiasMatrix2010();

      // get F-W angles at epoch
      FukushimaWilliams(T, gamb, phib, psib, epsa);

      // precession x frame bias matrix
      Matrix<double> PB = FukushimaWilliams(gamb, phib, psib, epsa);

      return (PB * transpose(B));
   }

   //------------------------------------------------------------------------------
   // Generate precise transformation matrix (3X3 rotation) for Earth motion due to
   // precession, nutation and frame bias (NPB matrix), at the given time of interest,
   // for IERS 2003.
   // param T CoordTransTime of interest
   // return 3x3 rotation matrix
   Matrix<double> EarthOrientation::PreciseEarthRotation2003(double T)
      throw(Exception)
   {
      try {
         Matrix<double> N = NutationMatrix2003(T);
         Matrix<double> P = PrecessionMatrix2003(T);     // includes bias

         //Matrix<double> NPB(N*P);
         //LOG(DEBUG7) << "\nNPB matrix:\n" << fixed << setprecision(15) << setw(18)
         //      << showpos << NPB;

         return (N*P);
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //------------------------------------------------------------------------------
   // Generate precise transformation matrix (3X3 rotation) for Earth motion due to
   // precession, nutation and frame bias (NPB matrix), at the given time of interest,
   // for IERS 2010.
   // param T CoordTransTime of interest
   // return 3x3 rotation matrix
   Matrix<double> EarthOrientation::PreciseEarthRotation2010(double T)
      throw(Exception)
   {
      try {
         double deps,dpsi,epsa;

         // get the F-W angles
         double gamb,phib,psib;
         FukushimaWilliams(T, gamb, phib, psib, epsa);

         // get nutation angles
         NutationAngles2010(T,deps,dpsi);

         // construct nutation x precession x frame bias matrix
         return FukushimaWilliams(gamb,phib,psib+dpsi,epsa+deps);
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //---------------------------------------------------------------------------------
   // Generate the full transformation matrix (3x3 rotation) relating the ECEF
   // frame to the conventional inertial frame, using IERS 1996 conventions.
   // Input is the time of interest, the polar motion angles xp and yp (arcsecs),
   // and UT1-UTC (sec) (xp,yp and UT1-UTC are just as found in the IERS bulletin;
   // see class EarthOrientation).
   Matrix<double> EarthOrientation::ECEFtoInertial1996(EphTime t,
                       double xp, double yp, double UT1mUTC, bool reduced)
      throw(Exception)
   {
      try {
         Matrix<double> P,N,W,S;

         double T=CoordTransTime(t);

         // precession
         P = PrecessionMatrix1996(T);
         LOG(DEBUG7) << "\nprecession matrix:\n" << fixed << setprecision(15)
               << setw(18) << showpos << P;

         // nutation
         double eps,deps,dpsi,om;
         // mean obliquity radians
         eps = Obliquity1996(T);
         LOG(DEBUG7) << "\nmean obliquity " << fixed << setprecision(15)
            << showpos << eps;
         // nutation angles - om is used in gast
         NutationAngles1996(T,deps,dpsi,om);
         LOG(DEBUG7) << "\nnutation angles psi eps " << fixed << setprecision(15)
            << showpos << dpsi << " " << deps;
         // nutation matrix
         N = NutationMatrix(eps,dpsi,deps);
         LOG(DEBUG7) << "\nnutation matrix:\n" << fixed << setprecision(15)
               << setw(18) << showpos << N;

         LOG(DEBUG7) << "\nNPB matrix:\n" << fixed << setprecision(15)
               << setw(18) << showpos << N*P;

         // if reduced (NGA), correct UT1mUTC for tides
         double UT1mUT1R,dlodR,domegaR;
         if(reduced) {
            UT1mUTCTidalCorrections(T, UT1mUT1R, dlodR, domegaR);
            UT1mUTC = UT1mUT1R - UT1mUTC;
         }

         double g = gast1996(t, om, eps, dpsi, UT1mUTC);
         LOG(DEBUG7) << "\nGAST = " << fixed << setprecision(15)
               << showpos << g*RAD_TO_DEG;

         S = rotation(g,3);
         LOG(DEBUG7) << "\ncelestial-to-terrestrial matrix (no polar motion):\n"
               << fixed << setprecision(15) << setw(18) << showpos << S*N*P;

         // Polar Motion
         W = PolarMotionMatrix1996(xp, yp);
         LOG(DEBUG7) << "\npolar motion matrix:\n" << fixed << setprecision(15)
               << setw(18) << showpos << W;

         return transpose(W*S*N*P);
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //---------------------------------------------------------------------------------
   // Generate the full transformation matrix (3x3 rotation) relating the ECEF
   // frame to the conventional inertial frame, using IERS 2003 conventions.
   // Input is the time of interest, the polar motion angles xp and yp (arcsecs),
   // and UT1-UTC (sec) (xp,yp and UT1-UTC are just as found in the IERS bulletin;
   // see class EarthOrientation).
   Matrix<double> EarthOrientation::ECEFtoInertial2003(EphTime t,
                                                double xp, double yp, double UT1mUTC)
      throw(Exception)
   {
      try {
         Matrix<double> P,N,R,W;

         double T(CoordTransTime(t));

         if(LOGlevel >= DEBUG7) {
            double gmst = GMST2003(t, UT1mUTC);
            LOG(DEBUG7) << "\nGMST = " << fixed << setprecision(15)
                  << showpos << gmst*RAD_TO_DEG;

            double gast = GAST2003(t, UT1mUTC);
            LOG(DEBUG7) << "\nGAST = " << fixed << setprecision(15)
                  << showpos << gast*RAD_TO_DEG;
         }

         // nutation
         double deps, dpsi, dpsipr, depspr;
         NutationAngles2003(T,deps,dpsi);
         LOG(DEBUG7) << "\nnutation angles psi eps " << fixed << setprecision(15)
            << showpos << dpsi << " " << deps;

         // Precession rate contributions with respect to IAU 2000
         // Precession and obliquity corrections (radians)
         PrecessionRateCorrections2003(T, dpsipr, depspr);
         LOG(DEBUG7) << "\nprecession-rate " << fixed << setprecision(15)
            << showpos << dpsipr << " " << depspr;

         double eps(Obliquity1996(T));         // same as 2003
         LOG(DEBUG7) << "\nmean obliquity " << fixed << setprecision(15)
            << showpos << eps;
         eps += depspr;

         N = NutationMatrix(eps,dpsi,deps);
         LOG(DEBUG7) << "\nnutation matrix:\n" << fixed << setprecision(15)
               << setw(18) << showpos << N;

         // precession
         P = PrecessionMatrix2003(T);

         Matrix<double> NPB(N*P);
         LOG(DEBUG7) << "\nNPB matrix:\n" << fixed << setprecision(15) << setw(18)
               << showpos << NPB;

         // ERA replaces GAST in the Earth rotation matrix
         double era(EarthRotationAngle(t,UT1mUTC));
         LOG(DEBUG7) << "\nERA = " << fixed << setprecision(15) << showpos
                        << era*RAD_TO_DEG;
         R = rotation(era,3);

         //double gast = GAST2003(t, UT1mUTC);
         //R = rotation(gast,3);

         // polar motion
         W = PolarMotionMatrix2003(t, xp, yp);
         LOG(DEBUG7) << "\npolar motion matrix:\n" << fixed << setprecision(15)
               << setw(18) << showpos << W;

         return transpose(W*R*N*P);
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //---------------------------------------------------------------------------------
   // Generate the full transformation matrix (3x3 rotation) relating the ECEF
   // frame to the conventional inertial frame, using IERS 2010 conventions.
   // Input is the time of interest, the polar motion angles xp and yp (arcsecs),
   // and UT1-UTC (sec) (xp,yp and UT1-UTC are just as found in the IERS bulletin;
   // see class EarthOrientation).
   Matrix<double> EarthOrientation::ECEFtoInertial2010(EphTime t,
                       double xp, double yp, double UT1mUTC)
      throw(Exception)
   {
      try {
         double T(CoordTransTime(t));

         // get the CIO coordinates and s
         // note that X,Y could also be obtained as (2,0),(2,1) components
         // of FukushimaWilliams()
         double X,Y,s;
         XYCIO(T, X, Y);
         s = S(T,X,Y,IERSConvention::IERS2010);
         LOG(DEBUG7) << "X = " << fixed << setprecision(15) << showpos << X;
         LOG(DEBUG7) << "Y = " << fixed << setprecision(15) << showpos << Y;
         LOG(DEBUG7) << "s\" = " << fixed << setprecision(15) << s/ARCSEC_TO_RAD;

         // compute transformation GCRS-to-CIRS or inertial-to-intermediate-celestial
         // cf. sofa c2ixys
         double r2(X*X+Y*Y);                          // squared radius
         double e(r2 != 0.0 ? ::atan2(Y, X) : 0.0);   // spherical angles
         double d(::atan(::sqrt(r2/(1.0-r2))));       //
         Matrix<double> GCRStoCIRS;
         GCRStoCIRS = rotation(-(e+s),3) * rotation(d, 2) * rotation(e, 3);
         LOG(DEBUG7) << "\nNPB matrix:\n" << fixed << setprecision(15) << setw(18)
               << showpos << GCRStoCIRS;

         // note that we could have called PreciseEarthRotation2010() instead

         // get ERA at UT1
         double era = EarthRotationAngle(t,UT1mUTC);
         LOG(DEBUG7) << "\nERA = " << fixed << setprecision(15)
                     << showpos << era*RAD_TO_DEG;

         // compute transf. CIRS-to-TIRS or intermediate-celestial-to-terrestrial
         Matrix<double> CIRStoTIRS;
         CIRStoTIRS = rotation(era, 3);
         LOG(DEBUG7) << "\ncelestial-to-terrestrial matrix (no polar motion):\n"
               << fixed << setprecision(15) << setw(18) << showpos
               << CIRStoTIRS * GCRStoCIRS;

         // compute the polar motion matrix, TIRS-to-ITRS
         //double sprime(Sprime(T));
         Matrix<double> PolarMotion(PolarMotionMatrix2003(t, xp, yp)); // 2010 == 2003
         LOG(DEBUG7) << "\npolar motion matrix:\n" << fixed << setprecision(15)
               << setw(18) << showpos << PolarMotion;

         // combine to get GCRS-to-ITRS
         Matrix<double> GCRStoITRS;
         GCRStoITRS = PolarMotion * CIRStoTIRS * GCRStoCIRS;

         // invert to get ITRS-to-GCRS or ECEFtoInertial
         return (transpose(GCRStoITRS));
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

} // end namespace gpstk

/*=============================================================================
** SOME of these routines, as noted, are based on, but not simply copied from, SOFA;
** SOFA has the following license.
**
**  Copyright (C) 2012
**  Standards Of Fundamental Astronomy Board
**  of the International Astronomical Union.
**
**  =====================
**  SOFA Software License
**  =====================
**
**  NOTICE TO USER:
**
**  BY USING THIS SOFTWARE YOU ACCEPT THE FOLLOWING SIX TERMS AND
**  CONDITIONS WHICH APPLY TO ITS USE.
**
**  1. The Software is owned by the IAU SOFA Board ("SOFA").
**
**  2. Permission is granted to anyone to use the SOFA software for any
**     purpose, including commercial applications, free of charge and
**     without payment of royalties, subject to the conditions and
**     restrictions listed below.
**
**  3. You (the user) may copy and distribute SOFA source code to others,
**     and use and adapt its code and algorithms in your own software,
**     on a world-wide, royalty-free basis.  That portion of your
**     distribution that does not consist of intact and unchanged copies
**     of SOFA source code files is a "derived work" that must comply
**     with the following requirements:
**
**     a) Your work shall be marked or carry a statement that it
**        (i) uses routines and computations derived by you from
**        software provided by SOFA under license to you; and
**        (ii) does not itself constitute software provided by and/or
**        endorsed by SOFA.
**
**     b) The source code of your derived work must contain descriptions
**        of how the derived work is based upon, contains and/or differs
**        from the original SOFA software.
**
**     c) The names of all routines in your derived work shall not
**        include the prefix "iau" or "sofa" or trivial modifications
**        thereof such as changes of case.
**
**     d) The origin of the SOFA components of your derived work must
**        not be misrepresented;  you must not claim that you wrote the
**        original software, nor file a patent application for SOFA
**        software or algorithms embedded in the SOFA software.
**
**     e) These requirements must be reproduced intact in any source
**        distribution and shall apply to anyone to whom you have
**        granted a further right to modify the source code of your
**        derived work.
**
**     Note that, as originally distributed, the SOFA software is
**     intended to be a definitive implementation of the IAU standards,
**     and consequently third-party modifications are discouraged.  All
**     variations, no matter how minor, must be explicitly marked as
**     such, as explained above.
**
**  4. You shall not cause the SOFA software to be brought into
**     disrepute, either by misuse, or use for inappropriate tasks, or
**     by inappropriate modification.
**
**  5. The SOFA software is provided "as is" and SOFA makes no warranty
**     as to its use or performance.   SOFA does not and cannot warrant
**     the performance or results which the user may obtain by using the
**     SOFA software.  SOFA makes no warranties, express or implied, as
**     to non-infringement of third party rights, merchantability, or
**     fitness for any particular purpose.  In no event will SOFA be
**     liable to the user for any consequential, incidental, or special
**     damages, including any lost profits or lost savings, even if a
**     SOFA representative has been advised of such damages, or for any
**     claim by any third party.
**
**  6. The provision of any version of the SOFA software under the terms
**     and conditions specified herein does not imply that future
**     versions will also be made available under the same terms and
**     conditions.
*
**  In any published work or commercial product which uses the SOFA
**  software directly, acknowledgement (see www.iausofa.org) is
**  appreciated.
**
**  Correspondence concerning SOFA software should be addressed as
**  follows:
**
**      By email:  sofa@ukho.gov.uk
**      By post:   IAU SOFA Center
**                 HM Nautical Almanac Office
**                 UK Hydrographic Office
**                 Admiralty Way, Taunton
**                 Somerset, TA1 2DN
**                 United Kingdom
**
**=============================================================================*/
