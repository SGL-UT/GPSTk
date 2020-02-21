#pragma ident "$Id$"

/**
* @file Msise00Drag.cpp
* This class computes the NRLMSISE atmosphere model.
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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
//
//============================================================================


#include "Msise00Drag.hpp"
#include <string>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include "ReferenceFrames.hpp"
#include "IERS.hpp"
#include "ASConstant.hpp"
#include "YDSTime.hpp"

namespace gpstk
{

   // test the model
   void Msise00Drag::test()
   {
      struct nrlmsise_output output[17];
      struct nrlmsise_input input[17];
      struct nrlmsise_flags flags;
      struct ap_array aph;

      int i;
      int j;
      
      /* input values */
      for (i=0;i<7;i++)
         aph.a[i]=100;

      flags.switches[0]=0;
      for (i=1;i<24;i++)
         flags.switches[i]=1;
      for (i=0;i<17;i++) {
         input[i].doy=172;
         input[i].year=0; /* without effect */
         input[i].sec=29000;
         input[i].alt=400;
         input[i].g_lat=60;
         input[i].g_long=-70;
         input[i].lst=16;
         input[i].f107A=150;
         input[i].f107=150;
         input[i].ap=4;
      }
      input[1].doy=81;
      input[2].sec=75000;
      input[2].alt=1000;
      input[3].alt=100;
      input[10].alt=0;
      input[11].alt=10;
      input[12].alt=30;
      input[13].alt=50;
      input[14].alt=70;
      input[16].alt=100;
      input[4].g_lat=0;
      input[5].g_long=0;
      input[6].lst=4;
      input[7].f107A=70;
      input[8].f107=180;
      input[9].ap=40;
      input[15].ap_a=&aph;
      input[16].ap_a=&aph;
      /* evaluate 0 to 14 */
      for (i=0;i<15;i++)
         gtd7(&input[i], &flags, &output[i]);
      /* evaluate 15 and 16 */
      flags.switches[9]=-1;
      for (i=15;i<17;i++)
         gtd7(&input[i], &flags, &output[i]);
      /* output type 1 */
      for (i=0;i<17;i++) {
         printf("\n");
         for (j=0;j<9;j++)
            printf("%E ",output[i].d[j]);
         printf("%E ",output[i].t[0]);
         printf("%E \n",output[i].t[1]);
         /* DL omitted */
      }

      /* output type 2 */
      for (i=0;i<3;i++) 
      {
         printf("\n");
         printf("\nDAY   ");
         for (j=0;j<5;j++)
            printf("         %3i",input[i*5+j].doy);
         printf("\nUT    ");
         for (j=0;j<5;j++)
            printf("       %5.0f",input[i*5+j].sec);
         printf("\nALT   ");
         for (j=0;j<5;j++)
            printf("        %4.0f",input[i*5+j].alt);
         printf("\nLAT   ");
         for (j=0;j<5;j++)
            printf("         %3.0f",input[i*5+j].g_lat);
         printf("\nLONG  ");
         for (j=0;j<5;j++)
            printf("         %3.0f",input[i*5+j].g_long);
         printf("\nLST   ");
         for (j=0;j<5;j++)
            printf("       %5.0f",input[i*5+j].lst);
         printf("\nF107A ");
         for (j=0;j<5;j++)
            printf("         %3.0f",input[i*5+j].f107A);
         printf("\nF107  ");
         for (j=0;j<5;j++)
            printf("         %3.0f",input[i*5+j].f107);
         printf("\n\n");
         printf("\nTINF  ");
         for (j=0;j<5;j++)
            printf("     %7.2f",output[i*5+j].t[0]);
         printf("\nTG    ");
         for (j=0;j<5;j++)
            printf("     %7.2f",output[i*5+j].t[1]);
         printf("\nHE    ");
         for (j=0;j<5;j++)
            printf("   %1.3e",output[i*5+j].d[0]);
         printf("\nO     ");
         for (j=0;j<5;j++)
            printf("   %1.3e",output[i*5+j].d[1]);
         printf("\nN2    ");
         for (j=0;j<5;j++)
            printf("   %1.3e",output[i*5+j].d[2]);
         printf("\nO2    ");
         for (j=0;j<5;j++)
            printf("   %1.3e",output[i*5+j].d[3]);
         printf("\nAR    ");
         for (j=0;j<5;j++)
            printf("   %1.3e",output[i*5+j].d[4]);
         printf("\nH     ");
         for (j=0;j<5;j++)
            printf("   %1.3e",output[i*5+j].d[6]);
         printf("\nN     ");
         for (j=0;j<5;j++)
            printf("   %1.3e",output[i*5+j].d[7]);
         printf("\nANM 0 ");
         for (j=0;j<5;j++)
            printf("   %1.3e",output[i*5+j].d[8]);
         printf("\nRHO   ");
         for (j=0;j<5;j++)
            printf("   %1.3e",output[i*5+j].d[5]);
         printf("\n");
      }
      printf("\n");
      
      /*
      nrlmsise-test should generate the following output:

      6.665177E+05 1.138806E+08 1.998211E+07 4.022764E+05 3.557465E+03 4.074714E-15 3.475312E+04 4.095913E+06 2.667273E+04 1.250540E+03 1.241416E+03 

      3.407293E+06 1.586333E+08 1.391117E+07 3.262560E+05 1.559618E+03 5.001846E-15 4.854208E+04 4.380967E+06 6.956682E+03 1.166754E+03 1.161710E+03 

      1.123767E+05 6.934130E+04 4.247105E+01 1.322750E-01 2.618848E-05 2.756772E-18 2.016750E+04 5.741256E+03 2.374394E+04 1.239892E+03 1.239891E+03 

      5.411554E+07 1.918893E+11 6.115826E+12 1.225201E+12 6.023212E+10 3.584426E-10 1.059880E+07 2.615737E+05 2.819879E-42 1.027318E+03 2.068878E+02 

      1.851122E+06 1.476555E+08 1.579356E+07 2.633795E+05 1.588781E+03 4.809630E-15 5.816167E+04 5.478984E+06 1.264446E+03 1.212396E+03 1.208135E+03 

      8.673095E+05 1.278862E+08 1.822577E+07 2.922214E+05 2.402962E+03 4.355866E-15 3.686389E+04 3.897276E+06 2.667273E+04 1.220146E+03 1.212712E+03 

      5.776251E+05 6.979139E+07 1.236814E+07 2.492868E+05 1.405739E+03 2.470651E-15 5.291986E+04 1.069814E+06 2.667273E+04 1.116385E+03 1.112999E+03 

      3.740304E+05 4.782720E+07 5.240380E+06 1.759875E+05 5.501649E+02 1.571889E-15 8.896776E+04 1.979741E+06 9.121815E+03 1.031247E+03 1.024848E+03 

      6.748339E+05 1.245315E+08 2.369010E+07 4.911583E+05 4.578781E+03 4.564420E-15 3.244595E+04 5.370833E+06 2.667273E+04 1.306052E+03 1.293374E+03 

      5.528601E+05 1.198041E+08 3.495798E+07 9.339618E+05 1.096255E+04 4.974543E-15 2.686428E+04 4.889974E+06 2.805445E+04 1.361868E+03 1.347389E+03 

      1.375488E+14 0.000000E+00 2.049687E+19 5.498695E+18 2.451733E+17 1.261066E-03 0.000000E+00 0.000000E+00 0.000000E+00 1.027318E+03 2.814648E+02 

      4.427443E+13 0.000000E+00 6.597567E+18 1.769929E+18 7.891680E+16 4.059139E-04 0.000000E+00 0.000000E+00 0.000000E+00 1.027318E+03 2.274180E+02 

      2.127829E+12 0.000000E+00 3.170791E+17 8.506280E+16 3.792741E+15 1.950822E-05 0.000000E+00 0.000000E+00 0.000000E+00 1.027318E+03 2.374389E+02 

      1.412184E+11 0.000000E+00 2.104370E+16 5.645392E+15 2.517142E+14 1.294709E-06 0.000000E+00 0.000000E+00 0.000000E+00 1.027318E+03 2.795551E+02 

      1.254884E+10 0.000000E+00 1.874533E+15 4.923051E+14 2.239685E+13 1.147668E-07 0.000000E+00 0.000000E+00 0.000000E+00 1.027318E+03 2.190732E+02 

      5.196477E+05 1.274494E+08 4.850450E+07 1.720838E+06 2.354487E+04 5.881940E-15 2.500078E+04 6.279210E+06 2.667273E+04 1.426412E+03 1.408608E+03 

      4.260860E+07 1.241342E+11 4.929562E+12 1.048407E+12 4.993465E+10 2.914304E-10 8.831229E+06 2.252516E+05 2.415246E-42 1.027318E+03 1.934071E+02 


      DAY            172          81         172         172         172
      UT           29000       29000       75000       29000       29000
      ALT            400         400        1000         100         400
      LAT             60          60          60          60           0
      LONG           -70         -70         -70         -70         -70
      LST             16          16          16          16          16
      F107A          150         150         150         150         150
      F107           150         150         150         150         150


      TINF       1250.54     1166.75     1239.89     1027.32     1212.40
      TG         1241.42     1161.71     1239.89      206.89     1208.14
      HE       6.665e+05   3.407e+06   1.124e+05   5.412e+07   1.851e+06
      O        1.139e+08   1.586e+08   6.934e+04   1.919e+11   1.477e+08
      N2       1.998e+07   1.391e+07   4.247e+01   6.116e+12   1.579e+07
      O2       4.023e+05   3.263e+05   1.323e-01   1.225e+12   2.634e+05
      AR       3.557e+03   1.560e+03   2.619e-05   6.023e+10   1.589e+03
      H        3.475e+04   4.854e+04   2.017e+04   1.060e+07   5.816e+04
      N        4.096e+06   4.381e+06   5.741e+03   2.616e+05   5.479e+06
      ANM 0    2.667e+04   6.957e+03   2.374e+04   2.820e-42   1.264e+03
      RHO      4.075e-15   5.002e-15   2.757e-18   3.584e-10   4.810e-15


      DAY            172         172         172         172         172
      UT           29000       29000       29000       29000       29000
      ALT            400         400         400         400         400
      LAT             60          60          60          60          60
      LONG             0         -70         -70         -70         -70
      LST             16           4          16          16          16
      F107A          150         150          70         150         150
      F107           150         150         150         180         150


      TINF       1220.15     1116.39     1031.25     1306.05     1361.87
      TG         1212.71     1113.00     1024.85     1293.37     1347.39
      HE       8.673e+05   5.776e+05   3.740e+05   6.748e+05   5.529e+05
      O        1.279e+08   6.979e+07   4.783e+07   1.245e+08   1.198e+08
      N2       1.823e+07   1.237e+07   5.240e+06   2.369e+07   3.496e+07
      O2       2.922e+05   2.493e+05   1.760e+05   4.912e+05   9.340e+05
      AR       2.403e+03   1.406e+03   5.502e+02   4.579e+03   1.096e+04
      H        3.686e+04   5.292e+04   8.897e+04   3.245e+04   2.686e+04
      N        3.897e+06   1.070e+06   1.980e+06   5.371e+06   4.890e+06
      ANM 0    2.667e+04   2.667e+04   9.122e+03   2.667e+04   2.805e+04
      RHO      4.356e-15   2.471e-15   1.572e-15   4.564e-15   4.975e-15


      DAY            172         172         172         172         172
      UT           29000       29000       29000       29000       29000
      ALT              0          10          30          50          70
      LAT             60          60          60          60          60
      LONG           -70         -70         -70         -70         -70
      LST             16          16          16          16          16
      F107A          150         150         150         150         150
      F107           150         150         150         150         150


      TINF       1027.32     1027.32     1027.32     1027.32     1027.32
      TG          281.46      227.42      237.44      279.56      219.07
      HE       1.375e+14   4.427e+13   2.128e+12   1.412e+11   1.255e+10
      O        0.000e+00   0.000e+00   0.000e+00   0.000e+00   0.000e+00
      N2       2.050e+19   6.598e+18   3.171e+17   2.104e+16   1.875e+15
      O2       5.499e+18   1.770e+18   8.506e+16   5.645e+15   4.923e+14
      AR       2.452e+17   7.892e+16   3.793e+15   2.517e+14   2.240e+13
      H        0.000e+00   0.000e+00   0.000e+00   0.000e+00   0.000e+00
      N        0.000e+00   0.000e+00   0.000e+00   0.000e+00   0.000e+00
      ANM 0    0.000e+00   0.000e+00   0.000e+00   0.000e+00   0.000e+00
      RHO      1.261e-03   4.059e-04   1.951e-05   1.295e-06   1.148e-07


      Note: These values equal those of the official FORTRAN package with
      one notable exception: the FORTRAN version reports for "anomalous
      oxygen" in test-run 4 exactly 0.000E-00, while my C compiler
      generates code which calculates 2.820e-42. When only 16-bit wide
      double variables are used, this value reduces to 0.000E-00 as well.

      */
   }

   
      /* Abstract class requires the subclass to compute the atmospheric density.
       * @param utc epoch in UTC
       * @param rb  EarthRef object.
       * @param r   Position vector.
       * @param v   Velocity vector
       * @return Atmospheric density in kg/m^3
       */
   double Msise00Drag::computeDensity(UTCTime utc, 
                                      EarthBody& rb, 
                                      Vector<double> r, 
                                      Vector<double> v)
   {
      struct nrlmsise_output output;
      struct nrlmsise_input input; 
      struct nrlmsise_flags flags;

      //* Get the J2000 to TOD transformation
      Matrix<double> N = ReferenceFrames::J2kToTODMatrix(utc);

      //* Transform r from J2000 to TOD
      Vector<double> r_tod = N * r;


      Matrix<double> eci2ecef = ReferenceFrames::J2kToECEFMatrix(utc);

      Vector<double> r_ecef = eci2ecef * r;
      
      Position geoidPos(r_ecef(0),r_ecef(1),r_ecef(3),Position::Cartesian);
      double alt = geoidPos.getAltitude() / 1000.0;    //* [km]

      if (alt > 1000.0) 
      {
         string msg("Msise00Drag only valid from 0 to 1000 km");
         Exception e(msg);
         //GPSTK_THROW(e);
      }
      
      //double dist2sun = norm(ReferenceFrames::getJ2kPosition(utc.asTDB(), SolarSystem::Sun))*1000.0;
      //double f107_in = this.f107_opt*Math.pow(dist2sun/Constants.AU,2);
      //double f107_in = this.f107_opt*Math.pow(Constants.AU/dist2sun,2);
      double f107_in = this->f107_opt;
            
      /* input values */
      //for (i=0;i<7;i++)
      //   aph.a[i]=13.853964381;//100;

      flags.switches[0]=0;
      for (int i=1; i<24; i++)
      {
         flags.switches[i] = 1;
      }

      input.doy = static_cast<YDSTime>(utc).doy;
      input.year = 2004;         // without effect 
      input.sec = static_cast<YDSTime>(utc).sod;
      input.alt= alt;
      input.g_lat = geoidPos.getGeodeticLatitude();
      input.g_long = geoidPos.getLongitude();
      input.lst = input.sec/3600.0 + input.g_long/15.0;
      input.f107A =f107_in;
      input.f107 = f107_in;
      input.ap = this->ap_opt;    //14.924291;//13.853964381; //???

      if(alt > 500.0)
      {
         gtd7d(&input, &flags, &output);
      }else
      {
         gtd7(&input, &flags, &output);
      }

      return output.d[5]*1000.0; //[kg/m^3]  

   }  // End of method 'Msise00Drag::computeDensity()'


   // LOCAL FUNCTIONS
   //-----------------------------------------------------------------------

   void Msise00Drag::tselec(struct nrlmsise_flags *flags) 
   {
      for (int i=0; i<24; i++) 
      {
         if (i!=9) 
         {
            if (flags->switches[i]==1)
               flags->sw[i]=1;
            else
               flags->sw[i]=0;
            if (flags->switches[i]>0)
               flags->swc[i]=1;
            else
               flags->swc[i]=0;
         } 
         else
         {
            flags->sw[i]=flags->switches[i];
            flags->swc[i]=flags->switches[i];
         }
      }

   }  // 'tselec()'


   void Msise00Drag::glatf(double lat, double *gv, double *reff) 
   {
      double dgtr = 1.74533E-2;
      double c2;
      c2 = std::cos(2.0*dgtr*lat);
      *gv = 980.616 * (1.0 - 0.0026373 * c2);
      *reff = 2.0 * (*gv) / (3.085462E-6 + 2.27E-9 * c2) * 1.0E-5;

   }  // 'glatf()'


   double Msise00Drag::ccor(double alt, double r, double h1, double zh) 
   {
      double e;
      double ex;
      e = (alt - zh) / h1;
      if (e>70)
         return std::exp(0.0);
      if (e<-70)
         return std::exp(r);
      ex = std::exp(e);
      e = r / (1.0 + ex);
      return std::exp(e);

   }  // 'ccor()'


   double Msise00Drag::ccor2(double alt, double r, double h1, double zh, double h2)
   {
      double e1, e2;
      double ex1, ex2;
      double ccor2v;
      e1 = (alt - zh) / h1;
      e2 = (alt - zh) / h2;
      if ((e1 > 70) || (e2 > 70))
         return std::exp(0.0);
      if ((e1 < -70) && (e2 < -70))
         return std::exp(r);
      ex1 = std::exp(e1);
      ex2 = std::exp(e2);
      ccor2v = r / (1.0 + 0.5 * (ex1 + ex2));
      return std::exp(ccor2v);

   }  // 'ccor2()'


   double Msise00Drag::scalh(double alt, double xm, double temp)
   {
      double g;
      double rgas=831.4;
      g = gsurf / (std::pow((1.0 + alt/re),2.0));
      g = rgas * temp / (g * xm);
      return g;

   }  // 'scalh()'


   double Msise00Drag::dnet (double dd, double dm, double zhm, double xmm, double xm)
   {
      double a;
      double ylog;
      a  = zhm / (xmm-xm);
      if (!((dm>0) && (dd>0))) 
      {
         printf("dnet log error %e %e %e\n",dm,dd,xm);
         if ((dd==0) && (dm==0))
            dd=1;
         if (dm==0)
            return dd;
         if (dd==0)
            return dm;
      } 
      ylog = a * std::log(dm/dd);
      if (ylog<-10)
         return dd;
      if (ylog>10)
         return dm;
      a = dd*std::pow((1.0 + std::exp(ylog)),(1.0/a));
      return a;

   }  // 'dnet()'


   void Msise00Drag::splini (double *xa, double *ya, double *y2a, int n, double x, double *y)
   {
      double yi=0;
      int klo=0;
      int khi=1;
      double xx, h, a, b, a2, b2;
      while ((x>xa[klo]) && (khi<n))
      {
         xx=x;
         if (khi<(n-1)) 
         {
            if (x<xa[khi])
               xx=x;
            else 
               xx=xa[khi];
         }
         h = xa[khi] - xa[klo];
         a = (xa[khi] - xx)/h;
         b = (xx - xa[klo])/h;
         a2 = a*a;
         b2 = b*b;
         yi += ((1.0 - a2) * ya[klo] / 2.0 + b2 * ya[khi] / 2.0 + ((-(1.0+a2*a2)/4.0 + a2/2.0) * y2a[klo] + (b2*b2/4.0 - b2/2.0) * y2a[khi]) * h * h / 6.0) * h;
         klo++;
         khi++;
      }
      *y = yi;

   }  // 'splini()'


   void Msise00Drag::splint (double *xa, double *ya, double *y2a, int n, double x, double *y)
   {
      /*      CALCULATE CUBIC SPLINE INTERP VALUE
      *       ADAPTED FROM NUMERICAL RECIPES BY PRESS ET AL.
      *       XA,YA: ARRAYS OF TABULATED FUNCTION IN ASCENDING ORDER BY X
      *       Y2A: ARRAY OF SECOND DERIVATIVES
      *       N: SIZE OF ARRAYS XA,YA,Y2A
      *       X: ABSCISSA FOR INTERPOLATION
      *       Y: OUTPUT VALUE
      */
      int klo=0;
      int khi=n-1;
      int k;
      double h;
      double a, b, yi;
      while ((khi-klo)>1) 
      {
         k=(khi+klo)/2;
         if (xa[k]>x)
            khi=k;
         else
            klo=k;
      }
      h = xa[khi] - xa[klo];
      if (h==0.0)
         printf("bad XA input to splint");
      a = (xa[khi] - x)/h;
      b = (x - xa[klo])/h;
      yi = a * ya[klo] + b * ya[khi] + ((a*a*a - a) * y2a[klo] + (b*b*b - b) * y2a[khi]) * h * h/6.0;
      *y = yi;

   }  // 'splint()'


   void Msise00Drag::spline (double *x, double *y, int n, double yp1, double ypn, double *y2) 
   {
      /*       CALCULATE 2ND DERIVATIVES OF CUBIC SPLINE INTERP FUNCTION
      *       ADAPTED FROM NUMERICAL RECIPES BY PRESS ET AL
      *       X,Y: ARRAYS OF TABULATED FUNCTION IN ASCENDING ORDER BY X
      *       N: SIZE OF ARRAYS X,Y
      *       YP1,YPN: SPECIFIED DERIVATIVES AT X[0] AND X[N-1]; VALUES
      *                >= 1E30 SIGNAL SIGNAL SECOND DERIVATIVE ZERO
      *       Y2: OUTPUT ARRAY OF SECOND DERIVATIVES
      */
      double *u;
      double sig, p, qn, un;
      int i, k;
      u=(double*)malloc(sizeof(double)*n);
      if (u==NULL) 
      {
         printf("Out Of Memory in spline - ERROR");
         return;
      }
      if (yp1>0.99E30) 
      {
         y2[0]=0;
         u[0]=0;
      } else 
      {
         y2[0]=-0.5;
         u[0]=(3.0/(x[1]-x[0]))*((y[1]-y[0])/(x[1]-x[0])-yp1);
      }
      for (i=1;i<(n-1);i++) 
      {
         sig = (x[i]-x[i-1])/(x[i+1] - x[i-1]);
         p = sig * y2[i-1] + 2.0;
         y2[i] = (sig - 1.0) / p;
         u[i] = (6.0 * ((y[i+1] - y[i])/(x[i+1] - x[i]) -(y[i] - y[i-1]) / (x[i] - x[i-1]))/(x[i+1] - x[i-1]) - sig * u[i-1])/p;
      }
      if (ypn>0.99E30) 
      {
         qn = 0;
         un = 0;
      } 
      else 
      {
         qn = 0.5;
         un = (3.0 / (x[n-1] - x[n-2])) * (ypn - (y[n-1] - y[n-2])/(x[n-1] - x[n-2]));
      }
      y2[n-1] = (un - qn * u[n-2]) / (qn * y2[n-2] + 1.0);
      for (k=n-2;k>=0;k--)
         y2[k] = y2[k] * y2[k+1] + u[k];

      free(u);

   }  // 'spline()'


   double Msise00Drag::zeta(double zz, double zl) 
   {
      return ((zz-zl)*(re+zl)/(re+zz));
   }

   double Msise00Drag::densm (double alt, double d0, double xm, double *tz, int mn3, 
      double *zn3, double *tn3, double *tgn3, int mn2, double *zn2, double *tn2, double *tgn2) 
   {
      /*      Calculate Temperature and Density Profiles for lower atmos.  */
      double xs[10], ys[10], y2out[10];
      double rgas = 831.4;
      double z, z1, z2, t1, t2, zg, zgdif;
      double yd1, yd2;
      double x, y, yi;
      double expl, gamm, glb;
      double densm_tmp;
      int mn;
      int k;
      densm_tmp=d0;
      if (alt>zn2[0]) 
      {
         if (xm==0.0)
            return *tz;
         else
            return d0;
      }

      /* STRATOSPHERE/MESOSPHERE TEMPERATURE */
      if (alt>zn2[mn2-1])
         z=alt;
      else
         z=zn2[mn2-1];
      mn=mn2;
      z1=zn2[0];
      z2=zn2[mn-1];
      t1=tn2[0];
      t2=tn2[mn-1];
      zg = zeta(z, z1);
      zgdif = zeta(z2, z1);

      /* set up spline nodes */
      for (k=0;k<mn;k++) 
      {
         xs[k]=zeta(zn2[k],z1)/zgdif;
         ys[k]=1.0 / tn2[k];
      }
      yd1=-tgn2[0] / (t1*t1) * zgdif;
      yd2=-tgn2[1] / (t2*t2) * zgdif * (std::pow(((re+z2)/(re+z1)),2.0));

      /* calculate spline coefficients */
      spline (xs, ys, mn, yd1, yd2, y2out);
      x = zg/zgdif;
      splint (xs, ys, y2out, mn, x, &y);

      /* temperature at altitude */
      *tz = 1.0 / y;
      if (xm!=0.0) 
      {
         /* calaculate stratosphere / mesospehere density */
         glb = gsurf / (std::pow((1.0 + z1/re),2.0));
         gamm = xm * glb * zgdif / rgas;

         /* Integrate temperature profile */
         splini(xs, ys, y2out, mn, x, &yi);
         expl=gamm*yi;
         if (expl>50.0)
            expl=50.0;

         /* Density at altitude */
         densm_tmp = densm_tmp * (t1 / *tz) * std::exp(-expl);
      }

      if (alt>zn3[0]) 
      {
         if (xm==0.0)
            return *tz;
         else
            return densm_tmp;
      }

      /* troposhere / stratosphere temperature */
      z = alt;
      mn = mn3;
      z1=zn3[0];
      z2=zn3[mn-1];
      t1=tn3[0];
      t2=tn3[mn-1];
      zg=zeta(z,z1);
      zgdif=zeta(z2,z1);

      /* set up spline nodes */
      for (k=0;k<mn;k++) 
      {
         xs[k] = zeta(zn3[k],z1) / zgdif;
         ys[k] = 1.0 / tn3[k];
      }
      yd1=-tgn3[0] / (t1*t1) * zgdif;
      yd2=-tgn3[1] / (t2*t2) * zgdif * (std::pow(((re+z2)/(re+z1)),2.0));

      /* calculate spline coefficients */
      spline (xs, ys, mn, yd1, yd2, y2out);
      x = zg/zgdif;
      splint (xs, ys, y2out, mn, x, &y);

      /* temperature at altitude */
      *tz = 1.0 / y;
      if (xm!=0.0) 
      {
         /* calaculate tropospheric / stratosphere density */
         glb = gsurf / (std::pow((1.0 + z1/re),2.0));
         gamm = xm * glb * zgdif / rgas;

         /* Integrate temperature profile */
         splini(xs, ys, y2out, mn, x, &yi);
         expl=gamm*yi;
         if (expl>50.0)
            expl=50.0;

         /* Density at altitude */
         densm_tmp = densm_tmp * (t1 / *tz) * std::exp(-expl);
      }
      if (xm==0.0)
         return *tz;
      else
         return densm_tmp;

   }  // 'densm'


   double Msise00Drag::densu (double alt, double dlb, double tinf, double tlb, double xm, double alpha, double *tz, double zlb, double s2, int mn1, double *zn1, double *tn1, double *tgn1)
   {
      /*      Calculate Temperature and Density Profiles for MSIS models
      *      New lower thermo polynomial
      */
      double yd2, yd1, x, y;
      double rgas=831.4;
      double densu_temp=1.0;
      double za, z, zg2, tt, ta;
      double dta, z1, z2, t1, t2, zg, zgdif;
      int mn;
      int k;
      double glb;
      double expl;
      double yi;
      double densa;
      double gamma, gamm;
      double xs[5], ys[5], y2out[5];
      /* joining altitudes of Bates and spline */
      za=zn1[0];
      if (alt>za)
         z=alt;
      else
         z=za;

      /* geopotential altitude difference from ZLB */
      zg2 = zeta(z, zlb);

      /* Bates temperature */
      tt = tinf - (tinf - tlb) * std::exp(-s2*zg2);
      ta = tt;
      *tz = tt;
      densu_temp = *tz;

      if (alt<za) 
      {
         /* calculate temperature below ZA
         * temperature gradient at ZA from Bates profile */
         dta = (tinf - ta) * s2 * std::pow(((re+zlb)/(re+za)),2.0);
         tgn1[0]=dta;
         tn1[0]=ta;
         if (alt>zn1[mn1-1])
            z=alt;
         else
            z=zn1[mn1-1];
         mn=mn1;
         z1=zn1[0];
         z2=zn1[mn-1];
         t1=tn1[0];
         t2=tn1[mn-1];
         /* geopotental difference from z1 */
         zg = zeta (z, z1);
         zgdif = zeta(z2, z1);
         /* set up spline nodes */
         for (k=0;k<mn;k++) 
         {
            xs[k] = zeta(zn1[k], z1) / zgdif;
            ys[k] = 1.0 / tn1[k];
         }
         /* end node derivatives */
         yd1 = -tgn1[0] / (t1*t1) * zgdif;
         yd2 = -tgn1[1] / (t2*t2) * zgdif * std::pow(((re+z2)/(re+z1)),2.0);
         /* calculate spline coefficients */
         spline (xs, ys, mn, yd1, yd2, y2out);
         x = zg / zgdif;
         splint (xs, ys, y2out, mn, x, &y);
         /* temperature at altitude */
         *tz = 1.0 / y;
         densu_temp = *tz;
      }
      if (xm==0)
         return densu_temp;

      /* calculate density above za */
      glb = gsurf / std::pow((1.0 + zlb/re),2.0);
      gamma = xm * glb / (s2 * rgas * tinf);
      expl = std::exp(-s2 * gamma * zg2);
      if (expl>50.0)
         expl=50.0;
      if (tt<=0)
         expl=50.0;

      /* density at altitude */
      densa = dlb * std::pow((tlb/tt),((1.0+alpha+gamma))) * expl;
      densu_temp=densa;
      if (alt>=za)
         return densu_temp;

      /* calculate density below za */
      glb = gsurf / std::pow((1.0 + z1/re),2.0);
      gamm = xm * glb * zgdif / rgas;

      /* integrate spline temperatures */
      splini (xs, ys, y2out, mn, x, &yi);
      expl = gamm * yi;
      if (expl>50.0)
         expl=50.0;
      if (*tz<=0)
         expl=50.0;

      /* density at altitude */
      densu_temp = densu_temp * std::pow ((t1 / *tz),(1.0 + alpha)) * std::exp(-expl);
      return densu_temp;


   }  // 'densu()'


   double Msise00Drag::g0(double a, double *p) 
   {
      return (a - 4.0 + (p[25] - 1.0) * (a - 4.0 + (std::exp(-std::sqrt(p[24]*p[24]) * (a - 4.0)) - 1.0) / std::sqrt(p[24]*p[24])));
   }

   double Msise00Drag::sumex(double ex) 
   {
      return (1.0 + (1.0 - std::pow(ex,19.0)) / (1.0 - ex) * std::pow(ex,0.5));
   }

   double Msise00Drag::sg0(double ex, double *p, double *ap) 
   {
      return (g0(ap[1],p) + (g0(ap[2],p)*ex + g0(ap[3],p)*ex*ex + \
         g0(ap[4],p)*std::pow(ex,3.0)   + (g0(ap[5],p)*std::pow(ex,4.0) + \
         g0(ap[6],p)*std::pow(ex,12.0))*(1.0-std::pow(ex,8.0))/(1.0-ex)))/sumex(ex);
   }

   double Msise00Drag::globe7(double *p, struct nrlmsise_input *input, struct nrlmsise_flags *flags) 
   {
      /*       CALCULATE G(L) FUNCTION 
      *       Upper Thermosphere Parameters */
      double t[15];
      int i,j;
      int sw9=1;
      double apd;
      double xlong;
      double tloc;
      double c, s, c2, c4, s2;
      double sr = 7.2722E-5;
      double dgtr = 1.74533E-2;
      double dr = 1.72142E-2;
      double hr = 0.2618;
      double cd32, cd18, cd14, cd39;
      double p32, p18, p14, p39;
      double df;
      double f1, f2;
      double tinf;
      struct ap_array *ap;

      tloc=input->lst;
      for (j=0;j<14;j++)
         t[j]=0;
      if (flags->sw[9]>0)
         sw9=1;
      else if (flags->sw[9]<0)
         sw9=-1;
      xlong = input->g_long;

      /* calculate legendre polynomials */
      c = std::sin(input->g_lat * dgtr);
      s = std::cos(input->g_lat * dgtr);
      c2 = c*c;
      c4 = c2*c2;
      s2 = s*s;

      plg[0][1] = c;
      plg[0][2] = 0.5*(3.0*c2 -1.0);
      plg[0][3] = 0.5*(5.0*c*c2-3.0*c);
      plg[0][4] = (35.0*c4 - 30.0*c2 + 3.0)/8.0;
      plg[0][5] = (63.0*c2*c2*c - 70.0*c2*c + 15.0*c)/8.0;
      plg[0][6] = (11.0*c*plg[0][5] - 5.0*plg[0][4])/6.0;
      /*      plg[0][7] = (13.0*c*plg[0][6] - 6.0*plg[0][5])/7.0; */
      plg[1][1] = s;
      plg[1][2] = 3.0*c*s;
      plg[1][3] = 1.5*(5.0*c2-1.0)*s;
      plg[1][4] = 2.5*(7.0*c2*c-3.0*c)*s;
      plg[1][5] = 1.875*(21.0*c4 - 14.0*c2 +1.0)*s;
      plg[1][6] = (11.0*c*plg[1][5]-6.0*plg[1][4])/5.0;
      /*      plg[1][7] = (13.0*c*plg[1][6]-7.0*plg[1][5])/6.0; */
      /*      plg[1][8] = (15.0*c*plg[1][7]-8.0*plg[1][6])/7.0; */
      plg[2][2] = 3.0*s2;
      plg[2][3] = 15.0*s2*c;
      plg[2][4] = 7.5*(7.0*c2 -1.0)*s2;
      plg[2][5] = 3.0*c*plg[2][4]-2.0*plg[2][3];
      plg[2][6] =(11.0*c*plg[2][5]-7.0*plg[2][4])/4.0;
      plg[2][7] =(13.0*c*plg[2][6]-8.0*plg[2][5])/5.0;
      plg[3][3] = 15.0*s2*s;
      plg[3][4] = 105.0*s2*s*c; 
      plg[3][5] =(9.0*c*plg[3][4]-7.*plg[3][3])/2.0;
      plg[3][6] =(11.0*c*plg[3][5]-8.*plg[3][4])/3.0;

      if (!(((flags->sw[7]==0)&&(flags->sw[8]==0))&&(flags->sw[14]==0))) 
      {
         stloc = std::sin(hr*tloc);
         ctloc = std::cos(hr*tloc);
         s2tloc = std::sin(2.0*hr*tloc);
         c2tloc = std::cos(2.0*hr*tloc);
         s3tloc = std::sin(3.0*hr*tloc);
         c3tloc = std::cos(3.0*hr*tloc);
      }

      cd32 = std::cos(dr*(input->doy-p[31]));
      cd18 = std::cos(2.0*dr*(input->doy-p[17]));
      cd14 = std::cos(dr*(input->doy-p[13]));
      cd39 = std::cos(2.0*dr*(input->doy-p[38]));
      p32=p[31];
      p18=p[17];
      p14=p[13];
      p39=p[38];

      /* F10.7 EFFECT */
      df = input->f107 - input->f107A;
      dfa = input->f107A - 150.0;
      t[0] =  p[19]*df*(1.0+p[59]*dfa) + p[20]*df*df + p[21]*dfa + p[29]*std::pow(dfa,2.0);
      f1 = 1.0 + (p[47]*dfa +p[19]*df+p[20]*df*df)*flags->swc[1];
      f2 = 1.0 + (p[49]*dfa+p[19]*df+p[20]*df*df)*flags->swc[1];

      /*  TIME INDEPENDENT */
      t[1] = (p[1]*plg[0][2]+ p[2]*plg[0][4]+p[22]*plg[0][6]) + \
         (p[14]*plg[0][2])*dfa*flags->swc[1] +p[26]*plg[0][1];

      /*  SYMMETRICAL ANNUAL */
      t[2] = p[18]*cd32;

      /*  SYMMETRICAL SEMIANNUAL */
      t[3] = (p[15]+p[16]*plg[0][2])*cd18;

      /*  ASYMMETRICAL ANNUAL */
      t[4] =  f1*(p[9]*plg[0][1]+p[10]*plg[0][3])*cd14;

      /*  ASYMMETRICAL SEMIANNUAL */
      t[5] =    p[37]*plg[0][1]*cd39;

      /* DIURNAL */
      if (flags->sw[7]) 
      {
         double t71, t72;
         t71 = (p[11]*plg[1][2])*cd14*flags->swc[5];
         t72 = (p[12]*plg[1][2])*cd14*flags->swc[5];
         t[6] = f2*((p[3]*plg[1][1] + p[4]*plg[1][3] + p[27]*plg[1][5] + t71) * \
            ctloc + (p[6]*plg[1][1] + p[7]*plg[1][3] + p[28]*plg[1][5] \
            + t72)*stloc);
      }

      /* SEMIDIURNAL */
      if (flags->sw[8]) 
      {
         double t81, t82;
         t81 = (p[23]*plg[2][3]+p[35]*plg[2][5])*cd14*flags->swc[5];
         t82 = (p[33]*plg[2][3]+p[36]*plg[2][5])*cd14*flags->swc[5];
         t[7] = f2*((p[5]*plg[2][2]+ p[41]*plg[2][4] + t81)*c2tloc +(p[8]*plg[2][2] + p[42]*plg[2][4] + t82)*s2tloc);
      }

      /* TERDIURNAL */
      if (flags->sw[14]) 
      {
         t[13] = f2 * ((p[39]*plg[3][3]+(p[93]*plg[3][4]+p[46]*plg[3][6])*cd14*flags->swc[5])* s3tloc +(p[40]*plg[3][3]+(p[94]*plg[3][4]+p[48]*plg[3][6])*cd14*flags->swc[5])* c3tloc);
      }

      /* magnetic activity based on daily ap */
      if (flags->sw[9]==-1) 
      {
         ap = input->ap_a;
         if (p[51]!=0) 
         {
            double exp1;
            exp1 = std::exp(-10800.0*std::sqrt(p[51]*p[51])/(1.0+p[138]*(45.0-std::sqrt(input->g_lat*input->g_lat))));
            if (exp1>0.99999)
               exp1=0.99999;
            if (p[24]<1.0E-4)
               p[24]=1.0E-4;
            apt[0]=sg0(exp1,p,ap->a);
            /* apt[1]=sg2(exp1,p,ap->a);
            apt[2]=sg0(exp2,p,ap->a);
            apt[3]=sg2(exp2,p,ap->a);
            */
            if (flags->sw[9]) 
            {
               t[8] = apt[0]*(p[50]+p[96]*plg[0][2]+p[54]*plg[0][4]+ \
                  (p[125]*plg[0][1]+p[126]*plg[0][3]+p[127]*plg[0][5])*cd14*flags->swc[5]+ \
                  (p[128]*plg[1][1]+p[129]*plg[1][3]+p[130]*plg[1][5])*flags->swc[7]* \
                  std::cos(hr*(tloc-p[131])));
            }
         }
      } 
      else 
      {
         double p44, p45;
         apd=input->ap-4.0;
         p44=p[43];
         p45=p[44];
         if (p44<0)
            p44 = 1.0E-5;
         apdf = apd + (p45-1.0)*(apd + (std::exp(-p44 * apd) - 1.0)/p44);
         if (flags->sw[9]) 
         {
            t[8]=apdf*(p[32]+p[45]*plg[0][2]+p[34]*plg[0][4]+ \
               (p[100]*plg[0][1]+p[101]*plg[0][3]+p[102]*plg[0][5])*cd14*flags->swc[5]+
               (p[121]*plg[1][1]+p[122]*plg[1][3]+p[123]*plg[1][5])*flags->swc[7]*
               std::cos(hr*(tloc-p[124])));
         }
      }

      if ((flags->sw[10])&&(input->g_long>-1000.0))
      {

         /* longitudinal */
         if (flags->sw[11]) 
         {
            t[10] = (1.0 + p[80]*dfa*flags->swc[1])* \
               ((p[64]*plg[1][2]+p[65]*plg[1][4]+p[66]*plg[1][6]\
               +p[103]*plg[1][1]+p[104]*plg[1][3]+p[105]*plg[1][5]\
               +flags->swc[5]*(p[109]*plg[1][1]+p[110]*plg[1][3]+p[111]*plg[1][5])*cd14)* \
               std::cos(dgtr*input->g_long) \
               +(p[90]*plg[1][2]+p[91]*plg[1][4]+p[92]*plg[1][6]\
               +p[106]*plg[1][1]+p[107]*plg[1][3]+p[108]*plg[1][5]\
               +flags->swc[5]*(p[112]*plg[1][1]+p[113]*plg[1][3]+p[114]*plg[1][5])*cd14)* \
               std::sin(dgtr*input->g_long));
         }

         /* ut and mixed ut, longitude */
         if (flags->sw[12])
         {
            t[11]=(1.0+p[95]*plg[0][1])*(1.0+p[81]*dfa*flags->swc[1])*\
               (1.0+p[119]*plg[0][1]*flags->swc[5]*cd14)*\
               ((p[68]*plg[0][1]+p[69]*plg[0][3]+p[70]*plg[0][5])*\
               std::cos(sr*(input->sec-p[71])));
            t[11]+=flags->swc[11]*\
               (p[76]*plg[2][3]+p[77]*plg[2][5]+p[78]*plg[2][7])*\
               std::cos(sr*(input->sec-p[79])+2.0*dgtr*input->g_long)*(1.0+p[137]*dfa*flags->swc[1]);
         }

         /* ut, longitude magnetic activity */
         if (flags->sw[13])
         {
            if (flags->sw[9]==-1) 
            {
               if (p[51])
               {
                  t[12]=apt[0]*flags->swc[11]*(1.+p[132]*plg[0][1])*\
                     ((p[52]*plg[1][2]+p[98]*plg[1][4]+p[67]*plg[1][6])*\
                     std::cos(dgtr*(input->g_long-p[97])))\
                     +apt[0]*flags->swc[11]*flags->swc[5]*\
                     (p[133]*plg[1][1]+p[134]*plg[1][3]+p[135]*plg[1][5])*\
                     cd14*std::cos(dgtr*(input->g_long-p[136])) \
                     +apt[0]*flags->swc[12]* \
                     (p[55]*plg[0][1]+p[56]*plg[0][3]+p[57]*plg[0][5])*\
                     std::cos(sr*(input->sec-p[58]));
               }
            } 
            else 
            {
               t[12] = apdf*flags->swc[11]*(1.0+p[120]*plg[0][1])*\
                  ((p[60]*plg[1][2]+p[61]*plg[1][4]+p[62]*plg[1][6])*\
                  std::cos(dgtr*(input->g_long-p[63])))\
                  +apdf*flags->swc[11]*flags->swc[5]* \
                  (p[115]*plg[1][1]+p[116]*plg[1][3]+p[117]*plg[1][5])* \
                  cd14*std::cos(dgtr*(input->g_long-p[118])) \
                  + apdf*flags->swc[12]* \
                  (p[83]*plg[0][1]+p[84]*plg[0][3]+p[85]*plg[0][5])* \
                  std::cos(sr*(input->sec-p[75]));
            }         
         }
      }

      /* parms not used: 82, 89, 99, 139-149 */
      tinf = p[30];
      for (i=0;i<14;i++)
         tinf = tinf + std::abs(flags->sw[i+1])*t[i];

      return tinf;

   }  //'globe7()'


   double Msise00Drag::glob7s(double *p, struct nrlmsise_input *input, struct nrlmsise_flags *flags) 
   {
      /*    VERSION OF GLOBE FOR LOWER ATMOSPHERE 10/26/99 
      */
      double pset=2.0;
      double t[14];
      double tt;
      double cd32, cd18, cd14, cd39;
      double p32, p18, p14, p39;
      int i,j;
      double dr=1.72142E-2;
      double dgtr=1.74533E-2;
      /* confirm parameter set */
      if (p[99]==0)
         p[99]=pset;
      if (p[99]!=pset) 
      {
         printf("Wrong parameter set for glob7s\n");
         return -1;
      }
      for (j=0;j<14;j++)
         t[j]=0.0;
      cd32 = std::cos(dr*(input->doy-p[31]));
      cd18 = std::cos(2.0*dr*(input->doy-p[17]));
      cd14 = std::cos(dr*(input->doy-p[13]));
      cd39 = std::cos(2.0*dr*(input->doy-p[38]));
      p32=p[31];
      p18=p[17];
      p14=p[13];
      p39=p[38];

      /* F10.7 */
      t[0] = p[21]*dfa;

      /* time independent */
      t[1]=p[1]*plg[0][2] + p[2]*plg[0][4] + p[22]*plg[0][6] + p[26]*plg[0][1] + p[14]*plg[0][3] + p[59]*plg[0][5];

      /* SYMMETRICAL ANNUAL */
      t[2]=(p[18]+p[47]*plg[0][2]+p[29]*plg[0][4])*cd32;

      /* SYMMETRICAL SEMIANNUAL */
      t[3]=(p[15]+p[16]*plg[0][2]+p[30]*plg[0][4])*cd18;

      /* ASYMMETRICAL ANNUAL */
      t[4]=(p[9]*plg[0][1]+p[10]*plg[0][3]+p[20]*plg[0][5])*cd14;

      /* ASYMMETRICAL SEMIANNUAL */
      t[5]=(p[37]*plg[0][1])*cd39;

      /* DIURNAL */
      if (flags->sw[7])
      {
         double t71, t72;
         t71 = p[11]*plg[1][2]*cd14*flags->swc[5];
         t72 = p[12]*plg[1][2]*cd14*flags->swc[5];
         t[6] = ((p[3]*plg[1][1] + p[4]*plg[1][3] + t71) * ctloc + (p[6]*plg[1][1] + p[7]*plg[1][3] + t72) * stloc) ;
      }

      /* SEMIDIURNAL */
      if (flags->sw[8]) 
      {
         double t81, t82;
         t81 = (p[23]*plg[2][3]+p[35]*plg[2][5])*cd14*flags->swc[5];
         t82 = (p[33]*plg[2][3]+p[36]*plg[2][5])*cd14*flags->swc[5];
         t[7] = ((p[5]*plg[2][2] + p[41]*plg[2][4] + t81) * c2tloc + (p[8]*plg[2][2] + p[42]*plg[2][4] + t82) * s2tloc);
      }

      /* TERDIURNAL */
      if (flags->sw[14])
      {
         t[13] = p[39] * plg[3][3] * s3tloc + p[40] * plg[3][3] * c3tloc;
      }

      /* MAGNETIC ACTIVITY */
      if (flags->sw[9]) 
      {
         if (flags->sw[9]==1)
            t[8] = apdf * (p[32] + p[45] * plg[0][2] * flags->swc[2]);
         if (flags->sw[9]==-1)   
            t[8]=(p[50]*apt[0] + p[96]*plg[0][2] * apt[0]*flags->swc[2]);
      }

      /* LONGITUDINAL */
      if (!((flags->sw[10]==0) || (flags->sw[11]==0) || (input->g_long<=-1000.0)))
      {
         t[10] = (1.0 + plg[0][1]*(p[80]*flags->swc[5]*std::cos(dr*(input->doy-p[81]))\
            +p[85]*flags->swc[6]*std::cos(2.0*dr*(input->doy-p[86])))\
            +p[83]*flags->swc[3]*std::cos(dr*(input->doy-p[84]))\
            +p[87]*flags->swc[4]*std::cos(2.0*dr*(input->doy-p[88])))\
            *((p[64]*plg[1][2]+p[65]*plg[1][4]+p[66]*plg[1][6]\
            +p[74]*plg[1][1]+p[75]*plg[1][3]+p[76]*plg[1][5]\
            )*std::cos(dgtr*input->g_long)\
            +(p[90]*plg[1][2]+p[91]*plg[1][4]+p[92]*plg[1][6]\
            +p[77]*plg[1][1]+p[78]*plg[1][3]+p[79]*plg[1][5]\
            )*std::sin(dgtr*input->g_long));
      }
      tt=0;
      for (i=0;i<14;i++)
         tt+=std::abs(flags->sw[i+1])*t[i];
      return tt;

   }  // 'glob7s()'


   void Msise00Drag::gtd7(struct nrlmsise_input *input, struct nrlmsise_flags *flags, struct nrlmsise_output *output)
   {
      double xlat;
      double xmm;
      int mn3 = 5;
      double zn3[5]={32.5,20.0,15.0,10.0,0.0};
      int mn2 = 4;
      double zn2[4]={72.5,55.0,45.0,32.5};
      double altt;
      double zmix=62.5;
      double tmp;
      double dm28m;
      double tz;
      double dmc;
      double dmr;
      double dz28;
      struct nrlmsise_output soutput;
      int i;

      tselec(flags);

      /* Latitude variation of gravity (none for sw[2]=0) */
      xlat=input->g_lat;
      if (flags->sw[2]==0)
         xlat=45.0;
      glatf(xlat, &gsurf, &re);

      xmm = pdm[2][4];

      /* THERMOSPHERE / MESOSPHERE (above zn2[0]) */
      if (input->alt>zn2[0])
         altt=input->alt;
      else
         altt=zn2[0];

      tmp=input->alt;
      input->alt=altt;
      gts7(input, flags, &soutput);
      altt=input->alt;
      input->alt=tmp;
      if (flags->sw[0])   /* metric adjustment */
         dm28m=dm28*1.0E6;
      else
         dm28m=dm28;
      output->t[0]=soutput.t[0];
      output->t[1]=soutput.t[1];
      if (input->alt>=zn2[0]) 
      {
         for (i=0;i<9;i++)
            output->d[i]=soutput.d[i];
         return;
      }

      /*       LOWER MESOSPHERE/UPPER STRATOSPHERE (between zn3[0] and zn2[0])
      *         Temperature at nodes and gradients at end nodes
      *         Inverse temperature a linear function of spherical harmonics
      */
      meso_tgn2[0]=meso_tgn1[1];
      meso_tn2[0]=meso_tn1[4];
      meso_tn2[1]=pma[0][0]*pavgm[0]/(1.0-flags->sw[20]*glob7s(pma[0], input, flags));
      meso_tn2[2]=pma[1][0]*pavgm[1]/(1.0-flags->sw[20]*glob7s(pma[1], input, flags));
      meso_tn2[3]=pma[2][0]*pavgm[2]/(1.0-flags->sw[20]*flags->sw[22]*glob7s(pma[2], input, flags));
      meso_tgn2[1]=pavgm[8]*pma[9][0]*(1.0+flags->sw[20]*flags->sw[22]*glob7s(pma[9], input, flags))*meso_tn2[3]*meso_tn2[3]/(std::pow((pma[2][0]*pavgm[2]),2.0));
      meso_tn3[0]=meso_tn2[3];

      if (input->alt<zn3[0])
      {
         /*       LOWER STRATOSPHERE AND TROPOSPHERE (below zn3[0])
         *         Temperature at nodes and gradients at end nodes
         *         Inverse temperature a linear function of spherical harmonics
         */
         meso_tgn3[0]=meso_tgn2[1];
         meso_tn3[1]=pma[3][0]*pavgm[3]/(1.0-flags->sw[22]*glob7s(pma[3], input, flags));
         meso_tn3[2]=pma[4][0]*pavgm[4]/(1.0-flags->sw[22]*glob7s(pma[4], input, flags));
         meso_tn3[3]=pma[5][0]*pavgm[5]/(1.0-flags->sw[22]*glob7s(pma[5], input, flags));
         meso_tn3[4]=pma[6][0]*pavgm[6]/(1.0-flags->sw[22]*glob7s(pma[6], input, flags));
         meso_tgn3[1]=pma[7][0]*pavgm[7]*(1.0+flags->sw[22]*glob7s(pma[7], input, flags)) *meso_tn3[4]*meso_tn3[4]/(std::pow((pma[6][0]*pavgm[6]),2.0));
      }

      /* LINEAR TRANSITION TO FULL MIXING BELOW zn2[0] */

      dmc=0;
      if (input->alt>zmix)
         dmc = 1.0 - (zn2[0]-input->alt)/(zn2[0] - zmix);
      dz28=soutput.d[2];

      /**** N2 density ****/
      dmr=soutput.d[2] / dm28m - 1.0;
      output->d[2]=densm(input->alt,dm28m,xmm, &tz, mn3, zn3, meso_tn3, meso_tgn3, mn2, zn2, meso_tn2, meso_tgn2);
      output->d[2]=output->d[2] * (1.0 + dmr*dmc);

      /**** HE density ****/
      dmr = soutput.d[0] / (dz28 * pdm[0][1]) - 1.0;
      output->d[0] = output->d[2] * pdm[0][1] * (1.0 + dmr*dmc);

      /**** O density ****/
      output->d[1] = 0;
      output->d[8] = 0;

      /**** O2 density ****/
      dmr = soutput.d[3] / (dz28 * pdm[3][1]) - 1.0;
      output->d[3] = output->d[2] * pdm[3][1] * (1.0 + dmr*dmc);

      /**** AR density ***/
      dmr = soutput.d[4] / (dz28 * pdm[4][1]) - 1.0;
      output->d[4] = output->d[2] * pdm[4][1] * (1.0 + dmr*dmc);

      /**** Hydrogen density ****/
      output->d[6] = 0;

      /**** Atomic nitrogen density ****/
      output->d[7] = 0;

      /**** Total mass density */
      output->d[5] = 1.66E-24 * (4.0 * output->d[0] + 16.0 * output->d[1] + 28.0 * output->d[2] + 32.0 * output->d[3] + 40.0 * output->d[4] + output->d[6] + 14.0 * output->d[7]);

      if (flags->sw[0])
         output->d[5]=output->d[5]/1000;

      /**** temperature at altitude ****/
      dd = densm(input->alt, 1.0, 0, &tz, mn3, zn3, meso_tn3, meso_tgn3, mn2, zn2, meso_tn2, meso_tgn2);
      output->t[1]=tz;

   }  // 'gtd7()'


   void Msise00Drag::gtd7d(struct nrlmsise_input *input, struct nrlmsise_flags *flags, struct nrlmsise_output *output) 
   {
      gtd7(input, flags, output);
      output->d[5] = 1.66E-24 * (4.0 * output->d[0] + 16.0 * output->d[1] + 28.0 * output->d[2] + 32.0 * output->d[3] + 40.0 * output->d[4] + output->d[6] + 14.0 * output->d[7] + 16.0 * output->d[8]);
      if (flags->sw[0])
         output->d[5]=output->d[5]/1000;

   }  // 'gtd7d()'



   void Msise00Drag::ghp7(struct nrlmsise_input *input, struct nrlmsise_flags *flags, struct nrlmsise_output *output, double press) 
   {
      double bm = 1.3806E-19;
      double rgas = 831.4;
      double test = 0.00043;
      double ltest = 12;
      double pl, p;
      double zi = 0.0;
      double z;
      double cl, cl2;
      double ca, cd;
      double xn, xm, diff;
      double g, sh;
      int l;
      pl = std::log10(press);
      if (pl >= -5.0) 
      {
         if (pl>2.5)
            zi = 18.06 * (3.00 - pl);
         else if ((pl>0.075) && (pl<=2.5))
            zi = 14.98 * (3.08 - pl);
         else if ((pl>-1) && (pl<=0.075))
            zi = 17.80 * (2.72 - pl);
         else if ((pl>-2) && (pl<=-1))
            zi = 14.28 * (3.64 - pl);
         else if ((pl>-4) && (pl<=-2))
            zi = 12.72 * (4.32 -pl);
         else if (pl<=-4)
            zi = 25.3 * (0.11 - pl);
         cl = input->g_lat/90.0;
         cl2 = cl*cl;
         if (input->doy<182)
            cd = (1.0 - (double) input->doy) / 91.25;
         else 
            cd = ((double) input->doy) / 91.25 - 3.0;
         ca = 0;
         if ((pl > -1.11) && (pl<=-0.23))
            ca = 1.0;
         if (pl > -0.23)
            ca = (2.79 - pl) / (2.79 + 0.23);
         if ((pl <= -1.11) && (pl>-3))
            ca = (-2.93 - pl)/(-2.93 + 1.11);
         z = zi - 4.87 * cl * cd * ca - 1.64 * cl2 * ca + 0.31 * ca * cl;
      } else
         z = 22.0 * std::pow((pl + 4.0),2.0) + 110.0;

      /* iteration  loop */
      l = 0;
      do 
      {
         l++;
         input->alt = z;
         gtd7(input, flags, output);
         z = input->alt;
         xn = output->d[0] + output->d[1] + output->d[2] + output->d[3] + output->d[4] + output->d[6] + output->d[7];
         p = bm * xn * output->t[1];
         if (flags->sw[0])
            p = p*1.0E-6;
         diff = pl - std::log10(p);
         if (std::sqrt(diff*diff)<test)
            return;
         if (l==ltest) {
            printf("ERROR: ghp7 not converging for press %e, diff %e",press,diff);
            return;
         }
         xm = output->d[5] / xn / 1.66E-24;
         if (flags->sw[0])
            xm = xm * 1.0E3;
         g = gsurf / (std::pow((1.0 + z/re),2.0));
         sh = rgas * output->t[1] / (xm * g);

         /* new altitude estimate using scale height */
         if (l <  6)
            z = z - sh * diff * 2.302;
         else
            z = z - sh * diff;
      } while (1==1);

   }  // 'ghp7()'


   void Msise00Drag::gts7(struct nrlmsise_input *input, struct nrlmsise_flags *flags, struct nrlmsise_output *output) 
   {
      /*     Thermospheric portion of NRLMSISE-00
      *     See GTD7 for more extensive comments
      *     alt > 72.5 km! 
      */
      double za;
      int i, j;
      double ddum, z;
      double zn1[5] = {120.0, 110.0, 100.0, 90.0, 72.5};
      double tinf;
      int mn1 = 5;
      double g0;
      double tlb;
      double s, z0, t0, tr12;
      double db01, db04, db14, db16, db28, db32, db40, db48;
      double zh28, zh04, zh16, zh32, zh40, zh01, zh14;
      double zhm28, zhm04, zhm16, zhm32, zhm40, zhm01, zhm14;
      double xmd;
      double b28, b04, b16, b32, b40, b01, b14;
      double tz;
      double g28, g4, g16, g32, g40, g1, g14;
      double zhf, xmm;
      double zc04, zc16, zc32, zc40, zc01, zc14;
      double hc04, hc16, hc32, hc40, hc01, hc14;
      double hcc16, hcc32, hcc01, hcc14;
      double zcc16, zcc32, zcc01, zcc14;
      double rc16, rc32, rc01, rc14;
      double rl;
      double g16h, db16h, tho, zsht, zmho, zsho;
      double dgtr=1.74533E-2;
      double dr=1.72142E-2;
      double alpha[9]={-0.38, 0.0, 0.0, 0.0, 0.17, 0.0, -0.38, 0.0, 0.0};
      double altl[8]={200.0, 300.0, 160.0, 250.0, 240.0, 450.0, 320.0, 450.0};
      double dd;
      double hc216, hcc232;
      za = pdl[1][15];
      zn1[0] = za;
      for (j=0;j<9;j++) 
         output->d[j]=0;

      /* TINF VARIATIONS NOT IMPORTANT BELOW ZA OR ZN1(1) */
      if (input->alt>zn1[0])
         tinf = ptm[0]*pt[0] * \
         (1.0+flags->sw[16]*globe7(pt,input,flags));
      else
         tinf = ptm[0]*pt[0];
      output->t[0]=tinf;

      /*  GRADIENT VARIATIONS NOT IMPORTANT BELOW ZN1(5) */
      if (input->alt>zn1[4])
         g0 = ptm[3]*ps[0] * \
         (1.0+flags->sw[19]*globe7(ps,input,flags));
      else
         g0 = ptm[3]*ps[0];
      tlb = ptm[1] * (1.0 + flags->sw[17]*globe7(pd[3],input,flags))*pd[3][0];
      s = g0 / (tinf - tlb);

      /*      Lower thermosphere temp variations not significant for
      *       density above 300 km */
      if (input->alt<300.0) 
      {
         meso_tn1[1]=ptm[6]*ptl[0][0]/(1.0-flags->sw[18]*glob7s(ptl[0], input, flags));
         meso_tn1[2]=ptm[2]*ptl[1][0]/(1.0-flags->sw[18]*glob7s(ptl[1], input, flags));
         meso_tn1[3]=ptm[7]*ptl[2][0]/(1.0-flags->sw[18]*glob7s(ptl[2], input, flags));
         meso_tn1[4]=ptm[4]*ptl[3][0]/(1.0-flags->sw[18]*flags->sw[20]*glob7s(ptl[3], input, flags));
         meso_tgn1[1]=ptm[8]*pma[8][0]*(1.0+flags->sw[18]*flags->sw[20]*glob7s(pma[8], input, flags))*meso_tn1[4]*meso_tn1[4]/(std::pow((ptm[4]*ptl[3][0]),2.0));
      } else
      {
         meso_tn1[1]=ptm[6]*ptl[0][0];
         meso_tn1[2]=ptm[2]*ptl[1][0];
         meso_tn1[3]=ptm[7]*ptl[2][0];
         meso_tn1[4]=ptm[4]*ptl[3][0];
         meso_tgn1[1]=ptm[8]*pma[8][0]*meso_tn1[4]*meso_tn1[4]/(std::pow((ptm[4]*ptl[3][0]),2.0));
      }

      z0 = zn1[3];
      t0 = meso_tn1[3];
      tr12 = 1.0;

      /* N2 variation factor at Zlb */
      g28=flags->sw[21]*globe7(pd[2], input, flags);

      /* VARIATION OF TURBOPAUSE HEIGHT */
      zhf=pdl[1][24]*(1.0+flags->sw[5]*pdl[0][24]*std::sin(dgtr*input->g_lat)*std::cos(dr*(input->doy-pt[13])));
      output->t[0]=tinf;
      xmm = pdm[2][4];
      z = input->alt;


      /**** N2 DENSITY ****/

      /* Diffusive density at Zlb */
      db28 = pdm[2][0]*std::exp(g28)*pd[2][0];
      /* Diffusive density at Alt */
      output->d[2]=densu(z,db28,tinf,tlb,28.0,alpha[2],&output->t[1],ptm[5],s,mn1,zn1,meso_tn1,meso_tgn1);
      dd=output->d[2];
      /* Turbopause */
      zh28=pdm[2][2]*zhf;
      zhm28=pdm[2][3]*pdl[1][5]; 
      xmd=28.0-xmm;
      /* Mixed density at Zlb */
      b28=densu(zh28,db28,tinf,tlb,xmd,(alpha[2]-1.0),&tz,ptm[5],s,mn1, zn1,meso_tn1,meso_tgn1);
      if ((flags->sw[15])&&(z<=altl[2])) 
      {
         /*  Mixed density at Alt */
         dm28=densu(z,b28,tinf,tlb,xmm,alpha[2],&tz,ptm[5],s,mn1,zn1,meso_tn1,meso_tgn1);
         /*  Net density at Alt */
         output->d[2]=dnet(output->d[2],dm28,zhm28,xmm,28.0);
      }


      /**** HE DENSITY ****/

      /*   Density variation factor at Zlb */
      g4 = flags->sw[21]*globe7(pd[0], input, flags);
      /*  Diffusive density at Zlb */
      db04 = pdm[0][0]*std::exp(g4)*pd[0][0];
      /*  Diffusive density at Alt */
      output->d[0]=densu(z,db04,tinf,tlb, 4.,alpha[0],&output->t[1],ptm[5],s,mn1,zn1,meso_tn1,meso_tgn1);
      dd=output->d[0];
      if ((flags->sw[15]) && (z<altl[0]))
      {
         /*  Turbopause */
         zh04=pdm[0][2];
         /*  Mixed density at Zlb */
         b04=densu(zh04,db04,tinf,tlb,4.-xmm,alpha[0]-1.,&output->t[1],ptm[5],s,mn1,zn1,meso_tn1,meso_tgn1);
         /*  Mixed density at Alt */
         dm04=densu(z,b04,tinf,tlb,xmm,0.,&output->t[1],ptm[5],s,mn1,zn1,meso_tn1,meso_tgn1);
         zhm04=zhm28;
         /*  Net density at Alt */
         output->d[0]=dnet(output->d[0],dm04,zhm04,xmm,4.);
         /*  Correction to specified mixing ratio at ground */
         rl=std::log(b28*pdm[0][1]/b04);
         zc04=pdm[0][4]*pdl[1][0];
         hc04=pdm[0][5]*pdl[1][1];
         /*  Net density corrected at Alt */
         output->d[0]=output->d[0]*ccor(z,rl,hc04,zc04);
      }


      /**** O DENSITY ****/

      /*  Density variation factor at Zlb */
      g16= flags->sw[21]*globe7(pd[1],input,flags);
      /*  Diffusive density at Zlb */
      db16 =  pdm[1][0]*std::exp(g16)*pd[1][0];
      /*   Diffusive density at Alt */
      output->d[1]=densu(z,db16,tinf,tlb, 16.,alpha[1],&output->t[1],ptm[5],s,mn1, zn1,meso_tn1,meso_tgn1);
      dd=output->d[1];
      if ((flags->sw[15]) && (z<=altl[1])) 
      {
         /*   Turbopause */
         zh16=pdm[1][2];
         /*  Mixed density at Zlb */
         b16=densu(zh16,db16,tinf,tlb,16.0-xmm,(alpha[1]-1.0), &output->t[1],ptm[5],s,mn1,zn1,meso_tn1,meso_tgn1);
         /*  Mixed density at Alt */
         dm16=densu(z,b16,tinf,tlb,xmm,0.,&output->t[1],ptm[5],s,mn1,zn1,meso_tn1,meso_tgn1);
         zhm16=zhm28;
         /*  Net density at Alt */
         output->d[1]=dnet(output->d[1],dm16,zhm16,xmm,16.);
         rl=pdm[1][1]*pdl[1][16]*(1.0+flags->sw[1]*pdl[0][23]*(input->f107A-150.0));
         hc16=pdm[1][5]*pdl[1][3];
         zc16=pdm[1][4]*pdl[1][2];
         hc216=pdm[1][5]*pdl[1][4];
         output->d[1]=output->d[1]*ccor2(z,rl,hc16,zc16,hc216);
         /*   Chemistry correction */
         hcc16=pdm[1][7]*pdl[1][13];
         zcc16=pdm[1][6]*pdl[1][12];
         rc16=pdm[1][3]*pdl[1][14];
         /*  Net density corrected at Alt */
         output->d[1]=output->d[1]*ccor(z,rc16,hcc16,zcc16);
      }


      /**** O2 DENSITY ****/

      /*   Density variation factor at Zlb */
      g32= flags->sw[21]*globe7(pd[4], input, flags);
      /*  Diffusive density at Zlb */
      db32 = pdm[3][0]*std::exp(g32)*pd[4][0];
      /*   Diffusive density at Alt */
      output->d[3]=densu(z,db32,tinf,tlb, 32.,alpha[3],&output->t[1],ptm[5],s,mn1, zn1,meso_tn1,meso_tgn1);
      dd=output->d[3];
      if (flags->sw[15]) 
      {
         if (z<=altl[3]) 
         {
            /*   Turbopause */
            zh32=pdm[3][2];
            /*  Mixed density at Zlb */
            b32=densu(zh32,db32,tinf,tlb,32.-xmm,alpha[3]-1., &output->t[1],ptm[5],s,mn1,zn1,meso_tn1,meso_tgn1);
            /*  Mixed density at Alt */
            dm32=densu(z,b32,tinf,tlb,xmm,0.,&output->t[1],ptm[5],s,mn1,zn1,meso_tn1,meso_tgn1);
            zhm32=zhm28;
            /*  Net density at Alt */
            output->d[3]=dnet(output->d[3],dm32,zhm32,xmm,32.);
            /*   Correction to specified mixing ratio at ground */
            rl=std::log(b28*pdm[3][1]/b32);
            hc32=pdm[3][5]*pdl[1][7];
            zc32=pdm[3][4]*pdl[1][6];
            output->d[3]=output->d[3]*ccor(z,rl,hc32,zc32);
         }
         /*  Correction for general departure from diffusive equilibrium above Zlb */
         hcc32=pdm[3][7]*pdl[1][22];
         hcc232=pdm[3][7]*pdl[0][22];
         zcc32=pdm[3][6]*pdl[1][21];
         rc32=pdm[3][3]*pdl[1][23]*(1.+flags->sw[1]*pdl[0][23]*(input->f107A-150.));
         /*  Net density corrected at Alt */
         output->d[3]=output->d[3]*ccor2(z,rc32,hcc32,zcc32,hcc232);
      }


      /**** AR DENSITY ****/

      /*   Density variation factor at Zlb */
      g40= flags->sw[20]*globe7(pd[5],input,flags);
      /*  Diffusive density at Zlb */
      db40 = pdm[4][0]*std::exp(g40)*pd[5][0];
      /*   Diffusive density at Alt */
      output->d[4]=densu(z,db40,tinf,tlb, 40.,alpha[4],&output->t[1],ptm[5],s,mn1,zn1,meso_tn1,meso_tgn1);
      dd=output->d[4];
      if ((flags->sw[15]) && (z<=altl[4]))
      {
         /*   Turbopause */
         zh40=pdm[4][2];
         /*  Mixed density at Zlb */
         b40=densu(zh40,db40,tinf,tlb,40.-xmm,alpha[4]-1.,&output->t[1],ptm[5],s,mn1,zn1,meso_tn1,meso_tgn1);
         /*  Mixed density at Alt */
         dm40=densu(z,b40,tinf,tlb,xmm,0.,&output->t[1],ptm[5],s,mn1,zn1,meso_tn1,meso_tgn1);
         zhm40=zhm28;
         /*  Net density at Alt */
         output->d[4]=dnet(output->d[4],dm40,zhm40,xmm,40.);
         /*   Correction to specified mixing ratio at ground */
         rl=std::log(b28*pdm[4][1]/b40);
         hc40=pdm[4][5]*pdl[1][9];
         zc40=pdm[4][4]*pdl[1][8];
         /*  Net density corrected at Alt */
         output->d[4]=output->d[4]*ccor(z,rl,hc40,zc40);
      }


      /**** HYDROGEN DENSITY ****/

      /*   Density variation factor at Zlb */
      g1 = flags->sw[21]*globe7(pd[6], input, flags);
      /*  Diffusive density at Zlb */
      db01 = pdm[5][0]*std::exp(g1)*pd[6][0];
      /*   Diffusive density at Alt */
      output->d[6]=densu(z,db01,tinf,tlb,1.,alpha[6],&output->t[1],ptm[5],s,mn1,zn1,meso_tn1,meso_tgn1);
      dd=output->d[6];
      if ((flags->sw[15]) && (z<=altl[6])) 
      {
         /*   Turbopause */
         zh01=pdm[5][2];
         /*  Mixed density at Zlb */
         b01=densu(zh01,db01,tinf,tlb,1.-xmm,alpha[6]-1., &output->t[1],ptm[5],s,mn1,zn1,meso_tn1,meso_tgn1);
         /*  Mixed density at Alt */
         dm01=densu(z,b01,tinf,tlb,xmm,0.,&output->t[1],ptm[5],s,mn1,zn1,meso_tn1,meso_tgn1);
         zhm01=zhm28;
         /*  Net density at Alt */
         output->d[6]=dnet(output->d[6],dm01,zhm01,xmm,1.);
         /*   Correction to specified mixing ratio at ground */
         rl=std::log(b28*pdm[5][1]*std::sqrt(pdl[1][17]*pdl[1][17])/b01);
         hc01=pdm[5][5]*pdl[1][11];
         zc01=pdm[5][4]*pdl[1][10];
         output->d[6]=output->d[6]*ccor(z,rl,hc01,zc01);
         /*   Chemistry correction */
         hcc01=pdm[5][7]*pdl[1][19];
         zcc01=pdm[5][6]*pdl[1][18];
         rc01=pdm[5][3]*pdl[1][20];
         /*  Net density corrected at Alt */
         output->d[6]=output->d[6]*ccor(z,rc01,hcc01,zcc01);
      }


      /**** ATOMIC NITROGEN DENSITY ****/

      /*   Density variation factor at Zlb */
      g14 = flags->sw[21]*globe7(pd[7],input,flags);
      /*  Diffusive density at Zlb */
      db14 = pdm[6][0]*std::exp(g14)*pd[7][0];
      /*   Diffusive density at Alt */
      output->d[7]=densu(z,db14,tinf,tlb,14.,alpha[7],&output->t[1],ptm[5],s,mn1,zn1,meso_tn1,meso_tgn1);
      dd=output->d[7];
      if ((flags->sw[15]) && (z<=altl[7])) 
      {
         /*   Turbopause */
         zh14=pdm[6][2];
         /*  Mixed density at Zlb */
         b14=densu(zh14,db14,tinf,tlb,14.-xmm,alpha[7]-1., &output->t[1],ptm[5],s,mn1,zn1,meso_tn1,meso_tgn1);
         /*  Mixed density at Alt */
         dm14=densu(z,b14,tinf,tlb,xmm,0.,&output->t[1],ptm[5],s,mn1,zn1,meso_tn1,meso_tgn1);
         zhm14=zhm28;
         /*  Net density at Alt */
         output->d[7]=dnet(output->d[7],dm14,zhm14,xmm,14.);
         /*   Correction to specified mixing ratio at ground */
         rl=std::log(b28*pdm[6][1]*std::sqrt(pdl[0][2]*pdl[0][2])/b14);
         hc14=pdm[6][5]*pdl[0][1];
         zc14=pdm[6][4]*pdl[0][0];
         output->d[7]=output->d[7]*ccor(z,rl,hc14,zc14);
         /*   Chemistry correction */
         hcc14=pdm[6][7]*pdl[0][4];
         zcc14=pdm[6][6]*pdl[0][3];
         rc14=pdm[6][3]*pdl[0][5];
         /*  Net density corrected at Alt */
         output->d[7]=output->d[7]*ccor(z,rc14,hcc14,zcc14);
      }


      /**** Anomalous OXYGEN DENSITY ****/

      g16h = flags->sw[21]*globe7(pd[8],input,flags);
      db16h = pdm[7][0]*std::exp(g16h)*pd[8][0];
      tho = pdm[7][9]*pdl[0][6];
      dd=densu(z,db16h,tho,tho,16.,alpha[8],&output->t[1],ptm[5],s,mn1, zn1,meso_tn1,meso_tgn1);
      zsht=pdm[7][5];
      zmho=pdm[7][4];
      zsho=scalh(zmho,16.0,tho);
      output->d[8]=dd*std::exp(-zsht/zsho*(std::exp(-(z-zmho)/zsht)-1.));


      /* total mass density */
      output->d[5] = 1.66E-24*(4.0*output->d[0]+16.0*output->d[1]+28.0*output->d[2]+32.0*output->d[3]+40.0*output->d[4]+ output->d[6]+14.0*output->d[7]);
      db48=1.66E-24*(4.0*db04+16.0*db16+28.0*db28+32.0*db32+40.0*db40+db01+14.0*db14);



      /* temperature */
      z = std::sqrt(input->alt*input->alt);
      ddum = densu(z,1.0, tinf, tlb, 0.0, 0.0, &output->t[1], ptm[5], s, mn1, zn1, meso_tn1, meso_tgn1);
      if (flags->sw[0]) 
      {
         for(i=0;i<9;i++)
            output->d[i]=output->d[i]*1.0E6;
         output->d[5]=output->d[5]/1000;
      }

   }  //'gts7'




   //////////////////////////////////////////////////////////////////////////

   /* -------------------------------------------------------------------- */
   /* ---------  N R L M S I S E - 0 0    M O D E L    2 0 0 1  ---------- */
   /* -------------------------------------------------------------------- */

   /* This file is part of the NRLMSISE-00  C source code package - release
    * 20041227
    *
    * The NRLMSISE-00 model was developed by Mike Picone, Alan Hedin, and
    * Doug Drob. They also wrote a NRLMSISE-00 distribution package in 
    * FORTRAN which is available at
    * http://uap-www.nrl.navy.mil/models_web/msis/msis_home.htm
    *
    * Dominik Brodowski implemented and maintains this C version. You can
    * reach him at mail@brodo.de. See the file "DOCUMENTATION" for details,
    * and check http://www.brodo.de/english/pub/nrlmsise/index.html for
    * updated releases of this package.
    */

   /* TEMPERATURE */
   double Msise00Drag::pt[150] = {
       9.86573E-01, 1.62228E-02, 1.55270E-02,-1.04323E-01,-3.75801E-03,
      -1.18538E-03,-1.24043E-01, 4.56820E-03, 8.76018E-03,-1.36235E-01,
      -3.52427E-02, 8.84181E-03,-5.92127E-03,-8.61650E+00, 0.00000E+00,
       1.28492E-02, 0.00000E+00, 1.30096E+02, 1.04567E-02, 1.65686E-03,
      -5.53887E-06, 2.97810E-03, 0.00000E+00, 5.13122E-03, 8.66784E-02,
       1.58727E-01, 0.00000E+00, 0.00000E+00, 0.00000E+00,-7.27026E-06,
       0.00000E+00, 6.74494E+00, 4.93933E-03, 2.21656E-03, 2.50802E-03,
       0.00000E+00, 0.00000E+00,-2.08841E-02,-1.79873E+00, 1.45103E-03,
       2.81769E-04,-1.44703E-03,-5.16394E-05, 8.47001E-02, 1.70147E-01,
       5.72562E-03, 5.07493E-05, 4.36148E-03, 1.17863E-04, 4.74364E-03,
       6.61278E-03, 4.34292E-05, 1.44373E-03, 2.41470E-05, 2.84426E-03,
       8.56560E-04, 2.04028E-03, 0.00000E+00,-3.15994E+03,-2.46423E-03,
       1.13843E-03, 4.20512E-04, 0.00000E+00,-9.77214E+01, 6.77794E-03,
       5.27499E-03, 1.14936E-03, 0.00000E+00,-6.61311E-03,-1.84255E-02,
      -1.96259E-02, 2.98618E+04, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       6.44574E+02, 8.84668E-04, 5.05066E-04, 0.00000E+00, 4.02881E+03,
      -1.89503E-03, 0.00000E+00, 0.00000E+00, 8.21407E-04, 2.06780E-03,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
      -1.20410E-02,-3.63963E-03, 9.92070E-05,-1.15284E-04,-6.33059E-05,
      -6.05545E-01, 8.34218E-03,-9.13036E+01, 3.71042E-04, 0.00000E+00,
       4.19000E-04, 2.70928E-03, 3.31507E-03,-4.44508E-03,-4.96334E-03,
      -1.60449E-03, 3.95119E-03, 2.48924E-03, 5.09815E-04, 4.05302E-03,
       2.24076E-03, 0.00000E+00, 6.84256E-03, 4.66354E-04, 0.00000E+00,
      -3.68328E-04, 0.00000E+00, 0.00000E+00,-1.46870E+02, 0.00000E+00,
       0.00000E+00, 1.09501E-03, 4.65156E-04, 5.62583E-04, 3.21596E+00,
       6.43168E-04, 3.14860E-03, 3.40738E-03, 1.78481E-03, 9.62532E-04,
       5.58171E-04, 3.43731E+00,-2.33195E-01, 5.10289E-04, 0.00000E+00,
       0.00000E+00,-9.25347E+04, 0.00000E+00,-1.99639E-03, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00
   };

   double Msise00Drag::pd[9][150] = {
   /*  HE DENSITY */ {
       1.09979E+00,-4.88060E-02,-1.97501E-01,-9.10280E-02,-6.96558E-03,
       2.42136E-02, 3.91333E-01,-7.20068E-03,-3.22718E-02, 1.41508E+00,
       1.68194E-01, 1.85282E-02, 1.09384E-01,-7.24282E+00, 0.00000E+00,
       2.96377E-01,-4.97210E-02, 1.04114E+02,-8.61108E-02,-7.29177E-04,
       1.48998E-06, 1.08629E-03, 0.00000E+00, 0.00000E+00, 8.31090E-02,
       1.12818E-01,-5.75005E-02,-1.29919E-02,-1.78849E-02,-2.86343E-06,
       0.00000E+00,-1.51187E+02,-6.65902E-03, 0.00000E+00,-2.02069E-03,
       0.00000E+00, 0.00000E+00, 4.32264E-02,-2.80444E+01,-3.26789E-03,
       2.47461E-03, 0.00000E+00, 0.00000E+00, 9.82100E-02, 1.22714E-01,
      -3.96450E-02, 0.00000E+00,-2.76489E-03, 0.00000E+00, 1.87723E-03,
      -8.09813E-03, 4.34428E-05,-7.70932E-03, 0.00000E+00,-2.28894E-03,
      -5.69070E-03,-5.22193E-03, 6.00692E-03,-7.80434E+03,-3.48336E-03,
      -6.38362E-03,-1.82190E-03, 0.00000E+00,-7.58976E+01,-2.17875E-02,
      -1.72524E-02,-9.06287E-03, 0.00000E+00, 2.44725E-02, 8.66040E-02,
       1.05712E-01, 3.02543E+04, 0.00000E+00, 0.00000E+00, 0.00000E+00,
      -6.01364E+03,-5.64668E-03,-2.54157E-03, 0.00000E+00, 3.15611E+02,
      -5.69158E-03, 0.00000E+00, 0.00000E+00,-4.47216E-03,-4.49523E-03,
       4.64428E-03, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       4.51236E-02, 2.46520E-02, 6.17794E-03, 0.00000E+00, 0.00000E+00,
      -3.62944E-01,-4.80022E-02,-7.57230E+01,-1.99656E-03, 0.00000E+00,
      -5.18780E-03,-1.73990E-02,-9.03485E-03, 7.48465E-03, 1.53267E-02,
       1.06296E-02, 1.18655E-02, 2.55569E-03, 1.69020E-03, 3.51936E-02,
      -1.81242E-02, 0.00000E+00,-1.00529E-01,-5.10574E-03, 0.00000E+00,
       2.10228E-03, 0.00000E+00, 0.00000E+00,-1.73255E+02, 5.07833E-01,
      -2.41408E-01, 8.75414E-03, 2.77527E-03,-8.90353E-05,-5.25148E+00,
      -5.83899E-03,-2.09122E-02,-9.63530E-03, 9.77164E-03, 4.07051E-03,
       2.53555E-04,-5.52875E+00,-3.55993E-01,-2.49231E-03, 0.00000E+00,
       0.00000E+00, 2.86026E+01, 0.00000E+00, 3.42722E-04, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00
   }, /* O DENSITY */ {
       1.02315E+00,-1.59710E-01,-1.06630E-01,-1.77074E-02,-4.42726E-03,
       3.44803E-02, 4.45613E-02,-3.33751E-02,-5.73598E-02, 3.50360E-01,
       6.33053E-02, 2.16221E-02, 5.42577E-02,-5.74193E+00, 0.00000E+00,
       1.90891E-01,-1.39194E-02, 1.01102E+02, 8.16363E-02, 1.33717E-04,
       6.54403E-06, 3.10295E-03, 0.00000E+00, 0.00000E+00, 5.38205E-02,
       1.23910E-01,-1.39831E-02, 0.00000E+00, 0.00000E+00,-3.95915E-06,
       0.00000E+00,-7.14651E-01,-5.01027E-03, 0.00000E+00,-3.24756E-03,
       0.00000E+00, 0.00000E+00, 4.42173E-02,-1.31598E+01,-3.15626E-03,
       1.24574E-03,-1.47626E-03,-1.55461E-03, 6.40682E-02, 1.34898E-01,
      -2.42415E-02, 0.00000E+00, 0.00000E+00, 0.00000E+00, 6.13666E-04,
      -5.40373E-03, 2.61635E-05,-3.33012E-03, 0.00000E+00,-3.08101E-03,
      -2.42679E-03,-3.36086E-03, 0.00000E+00,-1.18979E+03,-5.04738E-02,
      -2.61547E-03,-1.03132E-03, 1.91583E-04,-8.38132E+01,-1.40517E-02,
      -1.14167E-02,-4.08012E-03, 1.73522E-04,-1.39644E-02,-6.64128E-02,
      -6.85152E-02,-1.34414E+04, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       6.07916E+02,-4.12220E-03,-2.20996E-03, 0.00000E+00, 1.70277E+03,
      -4.63015E-03, 0.00000E+00, 0.00000E+00,-2.25360E-03,-2.96204E-03,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       3.92786E-02, 1.31186E-02,-1.78086E-03, 0.00000E+00, 0.00000E+00,
      -3.90083E-01,-2.84741E-02,-7.78400E+01,-1.02601E-03, 0.00000E+00,
      -7.26485E-04,-5.42181E-03,-5.59305E-03, 1.22825E-02, 1.23868E-02,
       6.68835E-03,-1.03303E-02,-9.51903E-03, 2.70021E-04,-2.57084E-02,
      -1.32430E-02, 0.00000E+00,-3.81000E-02,-3.16810E-03, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00,-9.05762E-04,-2.14590E-03,-1.17824E-03, 3.66732E+00,
      -3.79729E-04,-6.13966E-03,-5.09082E-03,-1.96332E-03,-3.08280E-03,
      -9.75222E-04, 4.03315E+00,-2.52710E-01, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00
   }, /* N2 DENSITY */ {
       1.16112E+00, 0.00000E+00, 0.00000E+00, 3.33725E-02, 0.00000E+00,
       3.48637E-02,-5.44368E-03, 0.00000E+00,-6.73940E-02, 1.74754E-01,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 1.74712E+02, 0.00000E+00,
       1.26733E-01, 0.00000E+00, 1.03154E+02, 5.52075E-02, 0.00000E+00,
       0.00000E+00, 8.13525E-04, 0.00000E+00, 0.00000E+00, 8.66784E-02,
       1.58727E-01, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00,-2.50482E+01, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-2.48894E-03,
       6.16053E-04,-5.79716E-04, 2.95482E-03, 8.47001E-02, 1.70147E-01,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 2.47425E-05, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00
   }, /* TLB */ {
       9.44846E-01, 0.00000E+00, 0.00000E+00,-3.08617E-02, 0.00000E+00,
      -2.44019E-02, 6.48607E-03, 0.00000E+00, 3.08181E-02, 4.59392E-02,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 1.74712E+02, 0.00000E+00,
       2.13260E-02, 0.00000E+00,-3.56958E+02, 0.00000E+00, 1.82278E-04,
       0.00000E+00, 3.07472E-04, 0.00000E+00, 0.00000E+00, 8.66784E-02,
       1.58727E-01, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 3.83054E-03, 0.00000E+00, 0.00000E+00,
      -1.93065E-03,-1.45090E-03, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00,-1.23493E-03, 1.36736E-03, 8.47001E-02, 1.70147E-01,
       3.71469E-03, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       5.10250E-03, 2.47425E-05, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 3.68756E-03, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00
   }, /* O2 DENSITY */ {
       1.35580E+00, 1.44816E-01, 0.00000E+00, 6.07767E-02, 0.00000E+00,
       2.94777E-02, 7.46900E-02, 0.00000E+00,-9.23822E-02, 8.57342E-02,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 2.38636E+01, 0.00000E+00,
       7.71653E-02, 0.00000E+00, 8.18751E+01, 1.87736E-02, 0.00000E+00,
       0.00000E+00, 1.49667E-02, 0.00000E+00, 0.00000E+00, 8.66784E-02,
       1.58727E-01, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00,-3.67874E+02, 5.48158E-03, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 8.47001E-02, 1.70147E-01,
       1.22631E-02, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       8.17187E-03, 3.71617E-05, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-2.10826E-03,
      -3.13640E-03, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
      -7.35742E-02,-5.00266E-02, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 1.94965E-02, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00
   }, /* AR DENSITY */ {
       1.04761E+00, 2.00165E-01, 2.37697E-01, 3.68552E-02, 0.00000E+00,
       3.57202E-02,-2.14075E-01, 0.00000E+00,-1.08018E-01,-3.73981E-01,
       0.00000E+00, 3.10022E-02,-1.16305E-03,-2.07596E+01, 0.00000E+00,
       8.64502E-02, 0.00000E+00, 9.74908E+01, 5.16707E-02, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 8.66784E-02,
       1.58727E-01, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 3.46193E+02, 1.34297E-02, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-3.48509E-03,
      -1.54689E-04, 0.00000E+00, 0.00000E+00, 8.47001E-02, 1.70147E-01,
       1.47753E-02, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       1.89320E-02, 3.68181E-05, 1.32570E-02, 0.00000E+00, 0.00000E+00,
       3.59719E-03, 7.44328E-03,-1.00023E-03,-6.50528E+03, 0.00000E+00,
       1.03485E-02,-1.00983E-03,-4.06916E-03,-6.60864E+01,-1.71533E-02,
       1.10605E-02, 1.20300E-02,-5.20034E-03, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
      -2.62769E+03, 7.13755E-03, 4.17999E-03, 0.00000E+00, 1.25910E+04,
       0.00000E+00, 0.00000E+00, 0.00000E+00,-2.23595E-03, 4.60217E-03,
       5.71794E-03, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
      -3.18353E-02,-2.35526E-02,-1.36189E-02, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 2.03522E-02,-6.67837E+01,-1.09724E-03, 0.00000E+00,
      -1.38821E-02, 1.60468E-02, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 1.51574E-02,
      -5.44470E-04, 0.00000E+00, 7.28224E-02, 6.59413E-02, 0.00000E+00,
      -5.15692E-03, 0.00000E+00, 0.00000E+00,-3.70367E+03, 0.00000E+00,
       0.00000E+00, 1.36131E-02, 5.38153E-03, 0.00000E+00, 4.76285E+00,
      -1.75677E-02, 2.26301E-02, 0.00000E+00, 1.76631E-02, 4.77162E-03,
       0.00000E+00, 5.39354E+00, 0.00000E+00,-7.51710E-03, 0.00000E+00,
       0.00000E+00,-8.82736E+01, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00
   }, /* H DENSITY */ {
       1.26376E+00,-2.14304E-01,-1.49984E-01, 2.30404E-01, 2.98237E-02,
       2.68673E-02, 2.96228E-01, 2.21900E-02,-2.07655E-02, 4.52506E-01,
       1.20105E-01, 3.24420E-02, 4.24816E-02,-9.14313E+00, 0.00000E+00,
       2.47178E-02,-2.88229E-02, 8.12805E+01, 5.10380E-02,-5.80611E-03,
       2.51236E-05,-1.24083E-02, 0.00000E+00, 0.00000E+00, 8.66784E-02,
       1.58727E-01,-3.48190E-02, 0.00000E+00, 0.00000E+00, 2.89885E-05,
       0.00000E+00, 1.53595E+02,-1.68604E-02, 0.00000E+00, 1.01015E-02,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 2.84552E-04,
      -1.22181E-03, 0.00000E+00, 0.00000E+00, 8.47001E-02, 1.70147E-01,
      -1.04927E-02, 0.00000E+00, 0.00000E+00, 0.00000E+00,-5.91313E-03,
      -2.30501E-02, 3.14758E-05, 0.00000E+00, 0.00000E+00, 1.26956E-02,
       8.35489E-03, 3.10513E-04, 0.00000E+00, 3.42119E+03,-2.45017E-03,
      -4.27154E-04, 5.45152E-04, 1.89896E-03, 2.89121E+01,-6.49973E-03,
      -1.93855E-02,-1.48492E-02, 0.00000E+00,-5.10576E-02, 7.87306E-02,
       9.51981E-02,-1.49422E+04, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       2.65503E+02, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 6.37110E-03, 3.24789E-04,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       6.14274E-02, 1.00376E-02,-8.41083E-04, 0.00000E+00, 0.00000E+00,
       0.00000E+00,-1.27099E-02, 0.00000E+00, 0.00000E+00, 0.00000E+00,
      -3.94077E-03,-1.28601E-02,-7.97616E-03, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00,-6.71465E-03,-1.69799E-03, 1.93772E-03, 3.81140E+00,
      -7.79290E-03,-1.82589E-02,-1.25860E-02,-1.04311E-02,-3.02465E-03,
       2.43063E-03, 3.63237E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00
   }, /* N DENSITY */ {
       7.09557E+01,-3.26740E-01, 0.00000E+00,-5.16829E-01,-1.71664E-03,
       9.09310E-02,-6.71500E-01,-1.47771E-01,-9.27471E-02,-2.30862E-01,
      -1.56410E-01, 1.34455E-02,-1.19717E-01, 2.52151E+00, 0.00000E+00,
      -2.41582E-01, 5.92939E-02, 4.39756E+00, 9.15280E-02, 4.41292E-03,
       0.00000E+00, 8.66807E-03, 0.00000E+00, 0.00000E+00, 8.66784E-02,
       1.58727E-01, 9.74701E-02, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 6.70217E+01,-1.31660E-03, 0.00000E+00,-1.65317E-02,
       0.00000E+00, 0.00000E+00, 8.50247E-02, 2.77428E+01, 4.98658E-03,
       6.15115E-03, 9.50156E-03,-2.12723E-02, 8.47001E-02, 1.70147E-01,
      -2.38645E-02, 0.00000E+00, 0.00000E+00, 0.00000E+00, 1.37380E-03,
      -8.41918E-03, 2.80145E-05, 7.12383E-03, 0.00000E+00,-1.66209E-02,
       1.03533E-04,-1.68898E-02, 0.00000E+00, 3.64526E+03, 0.00000E+00,
       6.54077E-03, 3.69130E-04, 9.94419E-04, 8.42803E+01,-1.16124E-02,
      -7.74414E-03,-1.68844E-03, 1.42809E-03,-1.92955E-03, 1.17225E-01,
      -2.41512E-02, 1.50521E+04, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       1.60261E+03, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00,-3.54403E-04,-1.87270E-02,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       2.76439E-02, 6.43207E-03,-3.54300E-02, 0.00000E+00, 0.00000E+00,
       0.00000E+00,-2.80221E-02, 8.11228E+01,-6.75255E-04, 0.00000E+00,
      -1.05162E-02,-3.48292E-03,-6.97321E-03, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00,-1.45546E-03,-1.31970E-02,-3.57751E-03,-1.09021E+00,
      -1.50181E-02,-7.12841E-03,-6.64590E-03,-3.52610E-03,-1.87773E-02,
      -2.22432E-03,-3.93895E-01, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00 
   }, /* HOT O DENSITY */ {
       6.04050E-02, 1.57034E+00, 2.99387E-02, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-1.51018E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00,-8.61650E+00, 1.26454E-02,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 5.50878E-03, 0.00000E+00, 0.00000E+00, 8.66784E-02,
       1.58727E-01, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 6.23881E-02, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 8.47001E-02, 1.70147E-01,
      -9.45934E-02, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00
   }};
   /* S PARAM  */
   double Msise00Drag::ps[150] = {
       9.56827E-01, 6.20637E-02, 3.18433E-02, 0.00000E+00, 0.00000E+00,
       3.94900E-02, 0.00000E+00, 0.00000E+00,-9.24882E-03,-7.94023E-03,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 1.74712E+02, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 2.74677E-03, 0.00000E+00, 1.54951E-02, 8.66784E-02,
       1.58727E-01, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00,-6.99007E-04, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 1.24362E-02,-5.28756E-03, 8.47001E-02, 1.70147E-01,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 2.47425E-05, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00
   };

   /* TURBO */
   double Msise00Drag::pdl[2][25] = {
      { 1.09930E+00, 3.90631E+00, 3.07165E+00, 9.86161E-01, 1.63536E+01,
       4.63830E+00, 1.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 1.28840E+00, 3.10302E-02, 1.18339E-01 }, 
      { 1.00000E+00, 7.00000E-01, 1.15020E+00, 3.44689E+00, 1.28840E+00,
       1.00000E+00, 1.08738E+00, 1.22947E+00, 1.10016E+00, 7.34129E-01,
       1.15241E+00, 2.22784E+00, 7.95046E-01, 4.01612E+00, 4.47749E+00,
       1.23435E+02,-7.60535E-02, 1.68986E-06, 7.44294E-01, 1.03604E+00,
       1.72783E+02, 1.15020E+00, 3.44689E+00,-7.46230E-01, 9.49154E-01 }
   };
   /* LOWER BOUNDARY */
   double Msise00Drag::ptm[10]/* ptm[50]*/ = {
       1.04130E+03, 3.86000E+02, 1.95000E+02, 1.66728E+01, 2.13000E+02,
       1.20000E+02, 2.40000E+02, 1.87000E+02,-2.00000E+00, 0.00000E+00
   };
   double Msise00Drag::pdm[8][10] = {
   {    2.45600E+07, 6.71072E-06, 1.00000E+02, 0.00000E+00, 1.10000E+02,
       1.00000E+01, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00 },\
   {    8.59400E+10, 1.00000E+00, 1.05000E+02,-8.00000E+00, 1.10000E+02,
       1.00000E+01, 9.00000E+01, 2.00000E+00, 0.00000E+00, 0.00000E+00 },\
   {    2.81000E+11, 0.00000E+00, 1.05000E+02, 2.80000E+01, 2.89500E+01,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00 },
   {    3.30000E+10, 2.68270E-01, 1.05000E+02, 1.00000E+00, 1.10000E+02,
       1.00000E+01, 1.10000E+02,-1.00000E+01, 0.00000E+00, 0.00000E+00 },
   {    1.33000E+09, 1.19615E-02, 1.05000E+02, 0.00000E+00, 1.10000E+02,
       1.00000E+01, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00 },
   {    1.76100E+05, 1.00000E+00, 9.50000E+01,-8.00000E+00, 1.10000E+02,
       1.00000E+01, 9.00000E+01, 2.00000E+00, 0.00000E+00, 0.00000E+00, },
   {    1.00000E+07, 1.00000E+00, 1.05000E+02,-8.00000E+00, 1.10000E+02,
       1.00000E+01, 9.00000E+01, 2.00000E+00, 0.00000E+00, 0.00000E+00 },
   {    1.00000E+06, 1.00000E+00, 1.05000E+02,-8.00000E+00, 5.50000E+02,
       7.60000E+01, 9.00000E+01, 2.00000E+00, 0.00000E+00, 4.00000E+03 }};


   double Msise00Drag::ptl[4][100] = {
   /* TN1(2) */ {
       1.00858E+00, 4.56011E-02,-2.22972E-02,-5.44388E-02, 5.23136E-04,
      -1.88849E-02, 5.23707E-02,-9.43646E-03, 6.31707E-03,-7.80460E-02,
      -4.88430E-02, 0.00000E+00, 0.00000E+00,-7.60250E+00, 0.00000E+00,
      -1.44635E-02,-1.76843E-02,-1.21517E+02, 2.85647E-02, 0.00000E+00,
       0.00000E+00, 6.31792E-04, 0.00000E+00, 5.77197E-03, 8.66784E-02,
       1.58727E-01, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00,-8.90272E+03, 3.30611E-03, 3.02172E-03, 0.00000E+00,
      -2.13673E-03,-3.20910E-04, 0.00000E+00, 0.00000E+00, 2.76034E-03,
       2.82487E-03,-2.97592E-04,-4.21534E-03, 8.47001E-02, 1.70147E-01,
       8.96456E-03, 0.00000E+00,-1.08596E-02, 0.00000E+00, 0.00000E+00,
       5.57917E-03, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 9.65405E-03, 0.00000E+00, 0.00000E+00, 2.00000E+00
   }, /*  TN1(3) */ {
       9.39664E-01, 8.56514E-02,-6.79989E-03, 2.65929E-02,-4.74283E-03,
       1.21855E-02,-2.14905E-02, 6.49651E-03,-2.05477E-02,-4.24952E-02,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 1.19148E+01, 0.00000E+00,
       1.18777E-02,-7.28230E-02,-8.15965E+01, 1.73887E-02, 0.00000E+00,
       0.00000E+00, 0.00000E+00,-1.44691E-02, 2.80259E-04, 8.66784E-02,
       1.58727E-01, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 2.16584E+02, 3.18713E-03, 7.37479E-03, 0.00000E+00,
      -2.55018E-03,-3.92806E-03, 0.00000E+00, 0.00000E+00,-2.89757E-03,
      -1.33549E-03, 1.02661E-03, 3.53775E-04, 8.47001E-02, 1.70147E-01,
      -9.17497E-03, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       3.56082E-03, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00,-1.00902E-02, 0.00000E+00, 0.00000E+00, 2.00000E+00
   }, /* TN1(4) */ {
       9.85982E-01,-4.55435E-02, 1.21106E-02, 2.04127E-02,-2.40836E-03,
       1.11383E-02,-4.51926E-02, 1.35074E-02,-6.54139E-03, 1.15275E-01,
       1.28247E-01, 0.00000E+00, 0.00000E+00,-5.30705E+00, 0.00000E+00,
      -3.79332E-02,-6.24741E-02, 7.71062E-01, 2.96315E-02, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 6.81051E-03,-4.34767E-03, 8.66784E-02,
       1.58727E-01, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 1.07003E+01,-2.76907E-03, 4.32474E-04, 0.00000E+00,
       1.31497E-03,-6.47517E-04, 0.00000E+00,-2.20621E+01,-1.10804E-03,
      -8.09338E-04, 4.18184E-04, 4.29650E-03, 8.47001E-02, 1.70147E-01,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
      -4.04337E-03, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-9.52550E-04,
       8.56253E-04, 4.33114E-04, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 1.21223E-03,
       2.38694E-04, 9.15245E-04, 1.28385E-03, 8.67668E-04,-5.61425E-06,
       1.04445E+00, 3.41112E+01, 0.00000E+00,-8.40704E-01,-2.39639E+02,
       7.06668E-01,-2.05873E+01,-3.63696E-01, 2.39245E+01, 0.00000E+00,
      -1.06657E-03,-7.67292E-04, 1.54534E-04, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 2.00000E+00
   }, /* TN1(5) TN2(1) */ {
       1.00320E+00, 3.83501E-02,-2.38983E-03, 2.83950E-03, 4.20956E-03,
       5.86619E-04, 2.19054E-02,-1.00946E-02,-3.50259E-03, 4.17392E-02,
      -8.44404E-03, 0.00000E+00, 0.00000E+00, 4.96949E+00, 0.00000E+00,
      -7.06478E-03,-1.46494E-02, 3.13258E+01,-1.86493E-03, 0.00000E+00,
      -1.67499E-02, 0.00000E+00, 0.00000E+00, 5.12686E-04, 8.66784E-02,
       1.58727E-01,-4.64167E-03, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       4.37353E-03,-1.99069E+02, 0.00000E+00,-5.34884E-03, 0.00000E+00,
       1.62458E-03, 2.93016E-03, 2.67926E-03, 5.90449E+02, 0.00000E+00,
       0.00000E+00,-1.17266E-03,-3.58890E-04, 8.47001E-02, 1.70147E-01,
       0.00000E+00, 0.00000E+00, 1.38673E-02, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 1.60571E-03,
       6.28078E-04, 5.05469E-05, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-1.57829E-03,
      -4.00855E-04, 5.04077E-05,-1.39001E-03,-2.33406E-03,-4.81197E-04,
       1.46758E+00, 6.20332E+00, 0.00000E+00, 3.66476E-01,-6.19760E+01,
       3.09198E-01,-1.98999E+01, 0.00000E+00,-3.29933E+02, 0.00000E+00,
      -1.10080E-03,-9.39310E-05, 1.39638E-04, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 2.00000E+00
   } };

   double Msise00Drag::pma[10][100] = {
   /* TN2(2) */ {
       9.81637E-01,-1.41317E-03, 3.87323E-02, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-3.58707E-02,
      -8.63658E-03, 0.00000E+00, 0.00000E+00,-2.02226E+00, 0.00000E+00,
      -8.69424E-03,-1.91397E-02, 8.76779E+01, 4.52188E-03, 0.00000E+00,
       2.23760E-02, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00,-7.07572E-03, 0.00000E+00, 0.00000E+00, 0.00000E+00,
      -4.11210E-03, 3.50060E+01, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00,-8.36657E-03, 1.61347E+01, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00,-1.45130E-02, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 1.24152E-03,
       6.43365E-04, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 1.33255E-03,
       2.42657E-03, 1.60666E-03,-1.85728E-03,-1.46874E-03,-4.79163E-06,
       1.22464E+00, 3.53510E+01, 0.00000E+00, 4.49223E-01,-4.77466E+01,
       4.70681E-01, 8.41861E+00,-2.88198E-01, 1.67854E+02, 0.00000E+00,
       7.11493E-04, 6.05601E-04, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 2.00000E+00
   }, /* TN2(3) */ {
       1.00422E+00,-7.11212E-03, 5.24480E-03, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-5.28914E-02,
      -2.41301E-02, 0.00000E+00, 0.00000E+00,-2.12219E+01,-1.03830E-02,
      -3.28077E-03, 1.65727E-02, 1.68564E+00,-6.68154E-03, 0.00000E+00,
       1.45155E-02, 0.00000E+00, 8.42365E-03, 0.00000E+00, 0.00000E+00,
       0.00000E+00,-4.34645E-03, 0.00000E+00, 0.00000E+00, 2.16780E-02,
       0.00000E+00,-1.38459E+02, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 7.04573E-03,-4.73204E+01, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 1.08767E-02, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-8.08279E-03,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 5.21769E-04,
      -2.27387E-04, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 3.26769E-03,
       3.16901E-03, 4.60316E-04,-1.01431E-04, 1.02131E-03, 9.96601E-04,
       1.25707E+00, 2.50114E+01, 0.00000E+00, 4.24472E-01,-2.77655E+01,
       3.44625E-01, 2.75412E+01, 0.00000E+00, 7.94251E+02, 0.00000E+00,
       2.45835E-03, 1.38871E-03, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 2.00000E+00
   }, /* TN2(4) TN3(1) */ {
       1.01890E+00,-2.46603E-02, 1.00078E-02, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-6.70977E-02,
      -4.02286E-02, 0.00000E+00, 0.00000E+00,-2.29466E+01,-7.47019E-03,
       2.26580E-03, 2.63931E-02, 3.72625E+01,-6.39041E-03, 0.00000E+00,
       9.58383E-03, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00,-1.85291E-03, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 1.39717E+02, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 9.19771E-03,-3.69121E+02, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00,-1.57067E-02, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-7.07265E-03,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-2.92953E-03,
      -2.77739E-03,-4.40092E-04, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 2.47280E-03,
       2.95035E-04,-1.81246E-03, 2.81945E-03, 4.27296E-03, 9.78863E-04,
       1.40545E+00,-6.19173E+00, 0.00000E+00, 0.00000E+00,-7.93632E+01,
       4.44643E-01,-4.03085E+02, 0.00000E+00, 1.15603E+01, 0.00000E+00,
       2.25068E-03, 8.48557E-04,-2.98493E-04, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 2.00000E+00
   }, /* TN3(2) */ {
       9.75801E-01, 3.80680E-02,-3.05198E-02, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 3.85575E-02,
       5.04057E-02, 0.00000E+00, 0.00000E+00,-1.76046E+02, 1.44594E-02,
      -1.48297E-03,-3.68560E-03, 3.02185E+01,-3.23338E-03, 0.00000E+00,
       1.53569E-02, 0.00000E+00,-1.15558E-02, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 4.89620E-03, 0.00000E+00, 0.00000E+00,-1.00616E-02,
      -8.21324E-03,-1.57757E+02, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 6.63564E-03, 4.58410E+01, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00,-2.51280E-02, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 9.91215E-03,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-8.73148E-04,
      -1.29648E-03,-7.32026E-05, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-4.68110E-03,
      -4.66003E-03,-1.31567E-03,-7.39390E-04, 6.32499E-04,-4.65588E-04,
      -1.29785E+00,-1.57139E+02, 0.00000E+00, 2.58350E-01,-3.69453E+01,
       4.10672E-01, 9.78196E+00,-1.52064E-01,-3.85084E+03, 0.00000E+00,
      -8.52706E-04,-1.40945E-03,-7.26786E-04, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 2.00000E+00
   }, /* TN3(3) */ {
       9.60722E-01, 7.03757E-02,-3.00266E-02, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 2.22671E-02,
       4.10423E-02, 0.00000E+00, 0.00000E+00,-1.63070E+02, 1.06073E-02,
       5.40747E-04, 7.79481E-03, 1.44908E+02, 1.51484E-04, 0.00000E+00,
       1.97547E-02, 0.00000E+00,-1.41844E-02, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 5.77884E-03, 0.00000E+00, 0.00000E+00, 9.74319E-03,
       0.00000E+00,-2.88015E+03, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00,-4.44902E-03,-2.92760E+01, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 2.34419E-02, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 5.36685E-03,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-4.65325E-04,
      -5.50628E-04, 3.31465E-04, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-2.06179E-03,
      -3.08575E-03,-7.93589E-04,-1.08629E-04, 5.95511E-04,-9.05050E-04,
       1.18997E+00, 4.15924E+01, 0.00000E+00,-4.72064E-01,-9.47150E+02,
       3.98723E-01, 1.98304E+01, 0.00000E+00, 3.73219E+03, 0.00000E+00,
      -1.50040E-03,-1.14933E-03,-1.56769E-04, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 2.00000E+00
   }, /* TN3(4) */ {
       1.03123E+00,-7.05124E-02, 8.71615E-03, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-3.82621E-02,
      -9.80975E-03, 0.00000E+00, 0.00000E+00, 2.89286E+01, 9.57341E-03,
       0.00000E+00, 0.00000E+00, 8.66153E+01, 7.91938E-04, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 4.68917E-03, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 7.86638E-03, 0.00000E+00, 0.00000E+00, 9.90827E-03,
       0.00000E+00, 6.55573E+01, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00,-4.00200E+01, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 7.07457E-03, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 5.72268E-03,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-2.04970E-04,
       1.21560E-03,-8.05579E-06, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-2.49941E-03,
      -4.57256E-04,-1.59311E-04, 2.96481E-04,-1.77318E-03,-6.37918E-04,
       1.02395E+00, 1.28172E+01, 0.00000E+00, 1.49903E-01,-2.63818E+01,
       0.00000E+00, 4.70628E+01,-2.22139E-01, 4.82292E-02, 0.00000E+00,
      -8.67075E-04,-5.86479E-04, 5.32462E-04, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 2.00000E+00
   }, /* TN3(5) SURFACE TEMP TSL */ {
       1.00828E+00,-9.10404E-02,-2.26549E-02, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-2.32420E-02,
      -9.08925E-03, 0.00000E+00, 0.00000E+00, 3.36105E+01, 0.00000E+00,
       0.00000E+00, 0.00000E+00,-1.24957E+01,-5.87939E-03, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 2.79765E+01, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 2.01237E+03, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00,-1.75553E-02, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 3.29699E-03,
       1.26659E-03, 2.68402E-04, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 1.17894E-03,
       1.48746E-03, 1.06478E-04, 1.34743E-04,-2.20939E-03,-6.23523E-04,
       6.36539E-01, 1.13621E+01, 0.00000E+00,-3.93777E-01, 2.38687E+03,
       0.00000E+00, 6.61865E+02,-1.21434E-01, 9.27608E+00, 0.00000E+00,
       1.68478E-04, 1.24892E-03, 1.71345E-03, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 2.00000E+00
   }, /* TGN3(2) SURFACE GRAD TSLG */ {
       1.57293E+00,-6.78400E-01, 6.47500E-01, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-7.62974E-02,
      -3.60423E-01, 0.00000E+00, 0.00000E+00, 1.28358E+02, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 4.68038E+01, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00,-1.67898E-01, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 2.90994E+04, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 3.15706E+01, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 2.00000E+00
   }, /* TGN2(1) TGN1(2) */ {
       8.60028E-01, 3.77052E-01, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-1.17570E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 7.77757E-03, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 1.01024E+02, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 6.54251E+02, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,-1.56959E-02,
       1.91001E-02, 3.15971E-02, 1.00982E-02,-6.71565E-03, 2.57693E-03,
       1.38692E+00, 2.82132E-01, 0.00000E+00, 0.00000E+00, 3.81511E+02,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 2.00000E+00
   }, /* TGN3(1) TGN2(2) */ {
       1.06029E+00,-5.25231E-02, 3.73034E-01, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 3.31072E-02,
      -3.88409E-01, 0.00000E+00, 0.00000E+00,-1.65295E+02,-2.13801E-01,
      -4.38916E-02,-3.22716E-01,-8.82393E+01, 1.18458E-01, 0.00000E+00,
      -4.35863E-01, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00,-1.19782E-01, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 2.62229E+01, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00,-5.37443E+01, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00,-4.55788E-01, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 3.84009E-02,
       3.96733E-02, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 5.05494E-02,
       7.39617E-02, 1.92200E-02,-8.46151E-03,-1.34244E-02, 1.96338E-02,
       1.50421E+00, 1.88368E+01, 0.00000E+00, 0.00000E+00,-5.13114E+01,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       5.11923E-02, 3.61225E-02, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 2.00000E+00
   } };
    
   /* SEMIANNUAL MULT SAM */
   double Msise00Drag::sam[100] = {
       1.00000E+00, 1.00000E+00, 1.00000E+00, 1.00000E+00, 1.00000E+00,
       1.00000E+00, 1.00000E+00, 1.00000E+00, 1.00000E+00, 1.00000E+00,
       1.00000E+00, 1.00000E+00, 1.00000E+00, 1.00000E+00, 1.00000E+00,
       1.00000E+00, 1.00000E+00, 1.00000E+00, 1.00000E+00, 1.00000E+00,
       1.00000E+00, 1.00000E+00, 1.00000E+00, 1.00000E+00, 1.00000E+00,
       1.00000E+00, 1.00000E+00, 1.00000E+00, 1.00000E+00, 1.00000E+00,
       1.00000E+00, 1.00000E+00, 1.00000E+00, 1.00000E+00, 1.00000E+00,
       1.00000E+00, 1.00000E+00, 1.00000E+00, 1.00000E+00, 1.00000E+00,
       1.00000E+00, 1.00000E+00, 1.00000E+00, 1.00000E+00, 1.00000E+00,
       1.00000E+00, 1.00000E+00, 1.00000E+00, 1.00000E+00, 1.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00,
       0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00, 0.00000E+00
   };
    
    
   /* MIDDLE ATMOSPHERE AVERAGES */
   double Msise00Drag::pavgm[10] = {
       2.61000E+02, 2.64000E+02, 2.29000E+02, 2.17000E+02, 2.17000E+02,
       2.23000E+02, 2.86760E+02,-2.93940E+00, 2.50000E+00, 0.00000E+00 };


   //////////////////////////////////////////////////////////////////////////

   /* PARMB */
   double Msise00Drag::gsurf = 0.0;
   double Msise00Drag::re = 0.0;

   /* GTS3C */
   double Msise00Drag::dd=0.0;

    /* DMIX */
   double Msise00Drag::dm04=0.0;
   double Msise00Drag::dm16=0.0;
   double Msise00Drag::dm28=0.0;
   double Msise00Drag::dm32=0.0;
   double Msise00Drag::dm40=0.0;
   double Msise00Drag::dm01=0.0;
   double Msise00Drag::dm14=0.0;

    /* MESO7 */
   double Msise00Drag::meso_tn1[5]={0.0};
   double Msise00Drag::meso_tn2[4]={0.0};
   double Msise00Drag::meso_tn3[5]={0.0};
   double Msise00Drag::meso_tgn1[2]={0.0};
   double Msise00Drag::meso_tgn2[2]={0.0};
   double Msise00Drag::meso_tgn3[2]={0.0};

    /* LPOLY */
   double Msise00Drag::dfa=0.0;
   double Msise00Drag::plg[4][9]={{0.0}};
   double Msise00Drag::ctloc=0.0;
   double Msise00Drag::stloc=0.0;
   double Msise00Drag::c2tloc=0.0;
   double Msise00Drag::s2tloc=0.0;
   double Msise00Drag::s3tloc=0.0;
   double Msise00Drag::c3tloc=0.0;
   double Msise00Drag::apdf=0.0;
   double Msise00Drag::apt[4]={0.0};


}  // End of namespace 'gpstk'




