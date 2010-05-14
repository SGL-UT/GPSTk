#pragma ident "$Id: $"

/**
* @file EarthSolidTide.cpp
* Class to do Earth Solid Tide correction
*/

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
//
//============================================================================


#include "EarthSolidTide.hpp"
#include "IERS.hpp"
#include "UTCTime.hpp"
#include "ReferenceFrames.hpp"
#include <complex>
#include "ASConstant.hpp"

namespace gpstk
{
   using namespace std;

   // For dC21 and dS21
   // The coefficients we choose are in-phase(ip) amplitudes and out-of-phase amplitudes of the
   // corrections for frequency dependence, and multipliers of the Delaunay variables
   // Refers to Table 6.3a in IERS2003 P64

   const double EarthSolidTide::Argu_C21[48][7]=
   {
      -0.1,    0,       2,    0,  2,  0,  2,
      -0.1,    0,       0,    0,  2,  2,  2,
      -0.1,    0,       1,    0,  2,  0,  1,
      -0.7,    0.1,     1,    0,  2,  0,  2,
      -0.1,    0,      -1,    0,  2,  2,  2,
      -1.3,    0.1,     0,    0,  2,  0,  1,
      -6.8,    0.6,     0,    0,  2,  0,  2,
       0.1,    0,       0,    0,  0,  2,  0,
       0.1,    0,       1,    0,  2, -2,  2,
       0.1,    0,      -1,    0,  2,  0,  1,
       0.4,    0,      -1,    0,  2,  0,  2,
       1.3,   -0.1,     1,    0,  0,  0,  0,
       0.3,    0,       1,    0,  0,  0,  1,
       0.3,    0,      -1,    0,  0,  2,  0,
       0.1,    0,      -1,    0,  0,  2,  1,
      -1.9,    0.1,     0,    1,  2, -2,  2,
       0.5,    0,       0,    0,  2, -2,  1,
      -43.4,    2.9,    0,    0,  2, -2,  2,
       0.6,    0,       0,   -1,  2, -2,  2,
       1.6,   -0.1,     0,    1,  0,  0,  0,
       0.1,    0,      -2,    0,  2,  0,  1,
       0.1,    0,       0,    0,  0,  0, -2,
      -8.8,    0.5,     0,    0,  0,  0, -1,
       470.9, -30.2,    0,    0,  0,  0,  0,
       68.1,  -4.6,     0,    0,  0,  0,  1,
      -1.6,    0.1,     0,    0,  0,  0,  2,
       0.1,    0,      -1,    0,  0,  1,  0,
      -0.1,    0,       0,   -1,  0,  0, -1,
      -20.6,  -0.3,     0,   -1,  0,  0,  0,
       0.3,    0,       0,    1, -2,  2, -2,
      -0.3,    0,       0,   -1,  0,  0,  1,
      -0.2,    0,      -2,    0,  0,  2,  0,
      -0.1,    0,      -2,    0,  0,  2,  1,
      -5.0,    0.3,     0,    0, -2,  2, -2,
       0.2,    0,       0,    0, -2,  2, -1,
      -0.2,    0,       0,   -1, -2,  2, -2,
      -0.5,    0,       1,    0,  0, -2,  0,
      -0.1,    0,       1,    0,  0, -2,  1,
       0.1,    0,      -1,    0,  0,  0, -1,
      -2.1,    0.1,    -1,    0,  0,  0,  0,
      -0.4,    0,      -1,    0,  0,  0,  1,
      -0.2,    0,       0,    0,  0, -2,  0,
      -0.1,    0,      -2,    0,  0,  0,  0,
      -0.6,    0,       0,    0, -2,  0, -2,
      -0.4,    0,       0,    0, -2,  0, -1,
      -0.1,    0,       0,    0, -2,  0,  0,
      -0.1,    0,      -1,    0, -2,  0, -2,
      -0.1,    0,      -1,    0, -2,  0, -1
   };

   // For dC22 and dS22
   // Refer to Table 6.3c in IERS2003
   // (0.30102 . i 0.00130).
   const double EarthSolidTide::Argu_C22[2][6] = 
   {
      -0.3, 1, 0, 2, 0, 2,
      -1.2, 0, 0, 2, 0, 2
   };
   
   // For dC20
   // Refer to Table 6.3b in IERS2003
   // The nominal value k20 for the zonal tides is taken as 0.30190
   const double EarthSolidTide::Argu_C20[21][7]=
   {
       16.6, -6.7,  0,  0,  0,  0,  1,
      -0.1,   0.1,  0,  0,  0,  0,  2,
      -1.2,   0.8,  0, -1,  0,  0,  0,
      -5.5,   4.3,  0,  0, -2,  2, -2,
       0.1,  -0.1,  0,  0, -2,  2, -1,
      -0.3,   0.2,  0, -1, -2,  2, -2,
      -0.3,   0.7,  1,  0,  0, -2,  0,
       0.1,  -0.2, -1,  0,  0,  0, -1,
      -1.2,   3.7, -1,  0,  0,  0,  0,
       0.1,  -0.2, -1,  0,  0,  0,  1,
       0.1,  -0.2,  1,  0, -2,  0, -2,
       0,     0.6,  0,  0,  0, -2,  0,
       0,     0.3, -2,  0,  0,  0,  0,
       0.6,   6.3,  0,  0, -2,  0, -2,
       0.2,   2.6,  0,  0, -2,  0, -1,
       0,     0.2,  0,  0, -2,  0,  0,
       0.1,   0.2,  1,  0, -2, -2, -2,
       0.4,   1.1, -1,  0, -2,  0, -2,
       0.2,   0.5, -1,  0, -2,  0, -1,
       0.1,   0.2,  0,  0, -2, -2, -2,
       0.1,   0.1, -2,  0, -2,  0, -2
   };

      /**
       * Solid tide to normalized earth potential coefficients
       *
       * @param mjdUtc  UTC in MJD
       * @param dC      correction to normalized coefficients dC
       * @param dS      correction to normalized coefficients dS
       */
   void EarthSolidTide::getSolidTide(double mjdUtc, double dC[], double dS[] )
   {
      UTCTime utc(mjdUtc);
          
      Matrix<double> E = ReferenceFrames::J2kToECEFMatrix(utc);
      
      Vector<double> moonReci = ReferenceFrames::getJ2kPosition(utc.asTDB(),SolarSystem::Moon)*1000.0;
      Vector<double> sunReci = ReferenceFrames::getJ2kPosition(utc.asTDB(),SolarSystem::Sun)*1000.0;

      Vector<double> moonR = E * moonReci;         // in ecef m
      Vector<double> sunR = E * sunReci;           // in ecef m
      
      Position moonP(moonR(0),moonR(1),moonR(2));
      Position sunP(sunR(0),sunR(1),sunR(2));
      
      double r_sun, phi_sun, lamda_sun;
      r_sun = norm(sunR);
      phi_sun = sunP.getGeocentricLatitude()*ASConstant::PI/180.0;
      lamda_sun = sunP.getLongitude()*ASConstant::PI/180.0;

      double r_lunar, phi_lunar, lamda_lunar;
      r_lunar = norm(moonR);
      phi_lunar = moonP.getGeocentricLatitude()*ASConstant::PI/180.0;
      lamda_lunar = moonP.getLongitude()*ASConstant::PI/180.0;


      // reference bern 5 TIDPT2.f
      /*
      PERTURBING ACCELERATION DUE TO TIDES CORRESPONDING TO IERS STANDARDS 2003.
      STEP 1 CORRECTIONS OF SOLID EARTH TIDES INCLUDED, 
      STEP 2 ONLY TERM DUE TO K1. SOLID EARTH POLE TIDES INCLUDED
      OCEAN TIDE TERMS UP TO N=M=4 INCLUDED
      */

      /*       IERS2003,  P60 
      Elastic Earth           Anelastic Earth
      n m    knm     k+nm    Reknm   Imknm    k+nm
      2 0 0.29525 .0.00087 0.30190 .0.00000 .0.00089
      2 1 0.29470 .0.00079 0.29830 .0.00144 .0.00080
      2 2 0.29801 .0.00057 0.30102 .0.00130 .0.00057
      3 0 0.093 ?ก่ ?ก่ ?ก่
      3 1 0.093 ?ก่ ?ก่ ?ก่
      3 2 0.093 ?ก่ ?ก่ ?ก่ 
      3 3 0.094 ?ก่ ?ก่ ?ก่
      */
      complex<double> k[10] =      // Anelastic Earth
      { 
         complex<double >(0.30190, 0.0),          // 20
         complex<double >(0.29830,-0.00144),      // 21
         complex<double >(0.30102,-0.00130),      // 22
         complex<double >(0.093, 0.0),            // 30
         complex<double >(0.093, 0.0),            // 31
         complex<double >(0.093, 0.0),            // 32
         complex<double >(0.094, 0.0),            // 33
         complex<double >(-0.00089, 0.0),         // k+ 20
         complex<double >(-0.00080, 0.0),         // k+ 21
         complex<double >(-0.00057, 0.0)          // k+ 22
      };

      complex<double> res[7];
      
      //----------------------------------------------------------------------
      // The first step of the computation ,refer to "IERS conventions 2003" P59
      // Each iteration for dC[n,m] and dS[n,m]
      for(int n=2;n<=3;n++)
      {
         for(int m=0;m<=n;m++)
         {
            int index = n * n - 2 * n + m;          //index in the returning value array
            
            double Nnm = normFactor( n, m );        //normalization coefficents of degree n and order m

            // Pnm: normalized Legendre polynomials of degress n and order m
            // 0 for sun and 1 for lunar each
            double sunPnm  = Nnm * legendrePoly( n, m, std::sin( phi_sun) );
            double moonPnm  = Nnm * legendrePoly( n, m, std::sin( phi_lunar) );
            
            double sunTemp = (ASConstant::GM_Sun/ASConstant::GM_Earth)*std::pow(ASConstant::R_Earth/r_sun,n+1) * sunPnm;
            double moonTemp = (ASConstant::GM_Moon/ASConstant::GM_Earth)*std::pow(ASConstant::R_Earth/r_lunar,n+1)*moonPnm;

            // Exp(-m*lamda*i) for sun and lunar each
            complex<double> c_sun   = complex<double>( std::cos( - m * lamda_sun ), std::sin( - m * lamda_sun ) );
            complex<double> c_lunar = complex<double>( std::cos( - m * lamda_lunar ), std::sin( - m * lamda_lunar ) );

            res[index] =  sunTemp * c_sun + moonTemp * c_lunar;

            dC[index]  =  (k[index]*res[index]).real()/(2.0*n+1.0);
            dS[index]  = -(k[index]*res[index]).imag()/(2.0*n+1.0);
            
         }  // 'for(int m=0;m<=n;m++)'

      }  // 'for(int n=2;n<=3;n++)'

      // The correction of dC[4,i] and dS[4,i](i=0,1,2) produced by degree 2 tide
      // The only difference from the above dC[2,i] and dS[2,i] is value of k replaced by k+
      for(int n = 0; n <= 2; n ++ )
      {
         int index   = 2 * 2 - 2 * 2 + n;                     // liuwk            
         complex<double> c_temp   = k[n+7 ] * res[ index ];   // liuwk
         dC[7+n] = c_temp.real() / 5.0;
         dS[7+n] =-c_temp.imag() / 5.0;
      }

      
      //-------------------------------------------------------------
      // The second step 

      //   COMPUTE DOODSON'S FUNDAMENTAL ARGUMENTS (BETA) 
      double BETA[6] = {0.0};
      double Dela[5] = {0.0};
      ReferenceFrames::doodsonArguments(utc.asUT1(),utc.asTT(),BETA,Dela);
      double GMST = ReferenceFrames::iauGmst00(utc.asUT1(), utc.asTT());
      

      for(int i=0;i<48;i++)
      {
         // Computation of thet_f
         double thet_f = (GMST+ASConstant::PI)-(Argu_C21[i][2]*Dela[0]+Argu_C21[i][3]*Dela[1]+Argu_C21[i][4]*Dela[2]
         + Argu_C21[i][5]*Dela[3]+Argu_C21[i][6]*Dela[4]);
         
         double t_s = std::sin(thet_f);
         double t_c = std::cos(thet_f);

         // Resulted from formula 5b in chapter 6.1
         dC[1] += ((Argu_C21[i][0]*t_s+Argu_C21[i][1]*t_c )*1e-12);
         dS[1] += ((Argu_C21[i][0]*t_c-Argu_C21[i][1]*t_s )*1e-12);
      }

      for(int i=0;i<2;i++)
      {
         // Input the computation of thet_f
         double thet_f = 2*(GMST+ASConstant::PI)-(Argu_C22[i][1]*Dela[0]+Argu_C22[i][2]*Dela[1]+Argu_C22[i][3]*Dela[2]
         + Argu_C22[i][4]*Dela[3]+Argu_C22[i][5]*Dela[4]);
         
         double t_s = std::sin(thet_f);
         double t_c = std::cos(thet_f);

         // Resulted from formula 5b in chapter 6.1
         // The corrections are only to the real part.
         dC[2] += ((Argu_C22[i][0]*t_c)*1e-12 );
         dS[2] += ((-Argu_C22[i][0]*t_s)*1e-12 );
      }
      

      for(int i=0;i<21;i++)
      {
         // Input the computation of thet_f
         double thet_f = -(Argu_C20[i][2]*Dela[0]+Argu_C20[i][3]*Dela[1]+Argu_C20[i][4]*Dela[2]
         + Argu_C20[i][5]*Dela[3]+Argu_C20[i][6]*Dela[4]);

         double t_s = std::sin(thet_f);
         double t_c = std::cos(thet_f);

         // Resulted from formula 5a in chapter 6.1
         // Modified, 05.12.2009
         //         dC[0] += ( ( Argu_C20[i][0] * t_c - Argu_C20[i][1] * t_s ) * 1e-12 );
         dC[0]+=((Argu_C20[i][0]*t_c+Argu_C20[i][1]*t_s)*1e-12);
      }

      //--------------------------------------------------------------------------------
      // the third step
      // 
      //C REMOVE PREMANENT TIDE FROM C02 (FOR JGM-3 NOT FOR GEM-T3)
      //C ---------------------------------------------------------
      //IF (IZTID.EQ.1) CPOT(4)=CPOT(4)+1.3914129D-8*K20
   
   }

   // Nnm IERS2003 P60
   double EarthSolidTide::normFactor(int n, int m) 
   {
      // The input should be n >= m >= 0

      double fac(1.0);
      for(int i = (n-m+1); i <= (n+m); i++)
      {
         fac = fac * double(i);
      }

      double delta  = (m == 0) ? 1.0 : 0.0;

      double num = (2.0 * n + 1.0) * (2.0 - delta);

      // We should make sure fac!=0, but it won't happen on the case,
      // so we just skip handling it
      double out = std::sqrt(num/fac);                  
      
      return out;

   }  // End of method 'EarthSolidTide::normFactor'
   
  
      //  Legendre polynomial
   double EarthSolidTide::legendrePoly(int n, int m, double u)
   {
      // reference:Satellite Orbits Montenbruck. P66
      if(0==n && 0==m)
      {
         return 1.0;
      }
      else if(m==n)
      {
         return (2.0*m-1.0)*std::sqrt(1.0-u*u)*legendrePoly(n-1,m-1,u);
      }
      else if(n==m+1)
      {
         return (2.0*m+1)*u*legendrePoly(m,m,u);
      }
      else
      {
         return ((2.0*n-1.0)*u*legendrePoly(n-1,m,u)-(n+m-1.0)*legendrePoly(n-2,m,u))/(n-m);
      }
      
   }  // End of method 'EarthSolidTide::legendrePoly()'


   void EarthSolidTide::test()
   {   
      cout<<"testing solid tide"<<endl;
      // debuging
      double mjdUtc = 2454531 + 0.49983796296296296 - 2400000.5;
      double dc[10]={0.0},ds[10]={0.0};
      getSolidTide(mjdUtc,dc,ds);

      int a = 0;


   }	// End of method 'EarthSolidTide::test()'



}	// End of namespace 'gpstk'




