#pragma ident "$Id: $"

/**
 * @file ReferenceFrames.cpp
 * Class to do Reference frame transformation.
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

#include "ReferenceFrames.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "StringUtils.hpp"
#include "IERS.hpp"
#include "ASConstant.hpp"


namespace gpstk
{
   using namespace std;
      
      // Objects to handle JPL ephemeris 405 
   SolarSystem ReferenceFrames::solarPlanets;

      // Reference epoch (J2000), Julian Date
   const double ReferenceFrames::DJ00 = 2451545.0;

      // Conversion offset, Julian Date to Modified Julian Date.
   const double ReferenceFrames::JD_TO_MJD = 2400000.5;

      // 2PI
   const double ReferenceFrames::D2PI = 6.283185307179586476925287;

      // Days per Julian century 
   const double ReferenceFrames::DJC = 36525.0;

      // Arcseconds to radians 
   const double ReferenceFrames::DAS2R = 4.848136811095359935899141e-6;
      
      // seconds to radians
   const double ReferenceFrames::DS2R = 7.272205216643039903848712e-5;

      // Arcseconds in a full circle 
   const double ReferenceFrames::TURNAS = 1296000.0;


      /* Compute planet position in J2000
       *  
       * @param TT         Time(Modified Julian Date in TT<TAI+32.184>) of interest 
       * @param entity     The planet to be computed
       * @return           The position of the planet in km
       */
   Vector<double> ReferenceFrames::getJ2kPosition( const DayTime&      TT,
                                                   SolarSystem::Planet entity)
      throw(Exception)
   {
      Vector<double> rvJ2k = getJ2kPosVel(TT,entity);

      Vector<double> rJ2k(3,0.0);
      for(int i=0; i<3; i++)
      {
         rJ2k[i] = rvJ2k[i];
      }

      return rJ2k;
   }

      /* Compute planet velocity in J2000
       *  
       * @param TT         Time(Modified Julian Date in TT<TAI+32.184>) of interest 
       * @param entity     The planet to be computed
       * @return           The velocity of the planet in km
       */
   Vector<double> ReferenceFrames::getJ2kVelocity( const DayTime&      TT, 
                                                   SolarSystem::Planet entity)
      throw(Exception)
   {
      Vector<double> rvJ2k = getJ2kPosVel(TT,entity);

      Vector<double> vJ2k(3,0.0);
      for(int i = 0; i < 3; i++) 
      {
         vJ2k[i] = rvJ2k[i+3];
      }

      return vJ2k;
   }

      /* Compute planet position and velocity
       *  
       * @param TT         Time(Modified Julian Date in TT<TAI+32.184>) of interest 
       * @param entity     The planet to be computed
       * @param center     relative to whick the result apply
       * @return           The position and velocity of the planet in km and km/s
       */
   Vector<double> ReferenceFrames::getJ2kPosVel( const DayTime&      TT, 
                                                 SolarSystem::Planet entity,
                                                 SolarSystem::Planet center)
      throw(Exception)
   {
      Vector<double> rvJ2k(6,0.0);

      try
      {
         double rvState[6] = {0.0};
         int ret = solarPlanets.computeState(JD_TO_MJD + TT.MJD(),
            entity,
            center,
            rvState);
         
            // change the unit to km/s from km/day
         rvState[3] /= 86400.0;
         rvState[4] /= 86400.0;
         rvState[5] /= 86400.0;

         if(ret == 0)
         {
            rvJ2k = rvState;
            return rvJ2k;
         }
         else
         {
            rvJ2k.resize(6,0.0);

            // failed to compute
            InvalidRequest e("Failed to compute, error code: "
               +StringUtils::asString(ret)+" with meaning\n"
               +"-1 and -2 given time is out of the file \n"
               +"-3 and -4 input stream is not open or not valid,"
               +" or EOF was found prematurely");

            GPSTK_THROW(e);
         }
      }
      catch(Exception& e)
      {
         GPSTK_RETHROW(e);
      }
      catch(exception& e)
      {
         Exception ee(e.what());
         GPSTK_THROW(ee);
      }
      catch(...)
      {
         Exception e("Unknown error!");
         GPSTK_THROW(e);
      }

      return rvJ2k;
      
   }  // End of method 'ReferenceFrames::getJ2kPosVel()'


      /* Compute planet position in ECEF
       *  
       * @param UTC        Time(Modified Julian Date in UTC) of interest 
       * @param entity     The planet to be computed
       * @param center     relative to whick the result apply
       * @return           The position of the planet in km and km/s
       */
   Vector<double> ReferenceFrames::getECEFPosition(UTCTime             UTC, 
                                                   SolarSystem::Planet entity,
                                                   SolarSystem::Planet center)
         throw(Exception)
   {
      Vector<double> ecefPosVel = getECEFPosVel(UTC, entity, center);

      Vector<double> ecefPos(3,0.0);
      ecefPos(0) = ecefPosVel(0);
      ecefPos(1) = ecefPosVel(1);
      ecefPos(2) = ecefPosVel(2);

      return ecefPos;
   }

      /* Compute planet velocity in ECEF
       *  
       * @param UTC        Time(Modified Julian Date in UTC) of interest 
       * @param entity     The planet to be computed
       * @param center     relative to whick the result apply
       * @return           The position of the planet in km and km/s
       */
   Vector<double> ReferenceFrames::getECEFVelocity(UTCTime             UTC, 
                                                   SolarSystem::Planet entity,
                                                   SolarSystem::Planet center)
         throw(Exception)
   {
      Vector<double> ecefPosVel = getECEFPosVel(UTC, entity, center);

      Vector<double> ecefVel(3,0.0);
      ecefVel(0) = ecefPosVel(3);
      ecefVel(1) = ecefPosVel(4);
      ecefVel(2) = ecefPosVel(5);

      return ecefVel;
   }

      /* Compute planet position and velocity in ECEF
       *  
       * @param UTC        Time(Modified Julian Date in UTC) of interest 
       * @param entity     The planet to be computed
       * @param center     relative to whick the result apply
       * @return           The position and velocity of the planet in km and km/s
       */
   Vector<double> ReferenceFrames::getECEFPosVel( UTCTime             UTC, 
                                                  SolarSystem::Planet entity,
                                                  SolarSystem::Planet center)
      throw(Exception)
   {
        Vector<double> j2kPosVel = getJ2kPosVel( UTC.asTT(), entity, center);
        Vector<double> ecefPosVel = J2kPosVelToECEF(UTC, j2kPosVel);

        return ecefPosVel;
   }

      // ECEF = W * S * NP * J2k
   void ReferenceFrames::J2kToECEFMatrix(UTCTime         UTC,
                                         Matrix<double>& POM,
                                         Matrix<double>& Theta, 
                                         Matrix<double>& NP)
      throw(Exception)
   {
      // Earth orientation data
      double xp = UTC.xPole() * DAS2R;
      double yp = UTC.yPole() * DAS2R;
     
      DayTime TT = UTC.asTT();
      DayTime UT1 = UTC.asUT1();
      

      // IAU 1976 precession matrix       
      Matrix<double> P = iauPmat76(TT);

      // Nutation correction wrt IAU 1976/1980 (mas->radians)
      const double DDP80 = 0.0; //-55.0655 * DAS2R/1000.0;
      const double DDE80 = 0.0; //-6.3580 * DAS2R/1000.0;

      // Nutation angle
      double DPSI = 0.0;
      double DEPS = 0.0;         
      nutationAngles(TT, DPSI, DEPS);

      DPSI += DDP80;
      DEPS += DDE80;

      // Mean obliquity
      double EPSA = meanObliquity(TT); 
      
      // IAU 1980 Nutation matrix
      Matrix<double> N = iauNmat(EPSA, DPSI , DEPS);

      // NP
      NP = N * P;

      // Euqation of the equinoxes, including nutation correction
      double EE = iauEqeq94(TT) + DDP80 * std::cos(EPSA);

      // Greenwich apparent sidereal time(IAU 1982/1994)
      double GST = normalizeAngle(iauGmst82(UT1) + EE);
      
      Theta =  Rz(GST);
     
      // Polar motion matrix
      POM = Ry(-xp) * Rx(-yp);
      
      // All Matrix are ready now

      return;
      
   }  // End of method 'ReferenceFrames::J2kToECEFMatrix()'


      // return POM * Theta * NP 
   Matrix<double> ReferenceFrames::J2kToECEFMatrix(UTCTime UTC)
   {
      Matrix<double> POM, Theta, NP;
      J2kToECEFMatrix(UTC,POM,Theta,NP);

      return (POM * Theta * NP);
   }
   
   /// NP TOD - TrueOfDate
   Matrix<double> ReferenceFrames::J2kToTODMatrix(UTCTime UTC)
   {
      Matrix<double> POM, Theta, NP;
      J2kToECEFMatrix(UTC,POM,Theta,NP);

      return NP;
   }


   Vector<double> ReferenceFrames::J2kPosVelToECEF(UTCTime UTC, Vector<double> j2kPosVel)
      throw(Exception)
   {
      
      Matrix<double> POM, Theta, NP;
      J2kToECEFMatrix(UTC,POM,Theta,NP);

      const double dera = earthRotationAngleRate1(UTC.mjdTT());
      
         // Derivative of Earth rotation 
      Matrix<double> S(3,3,0.0);
      S(0,1) = 1.0; S(1,0) = -1.0;      
      
      Matrix<double> dTheta = dera * S * Theta;
      
      Matrix<double> c2t = POM * Theta * NP;
      Matrix<double> dc2t = POM * dTheta * NP;

      Vector<double> j2kPos(3, 0.0), j2kVel(3, 0.0);
      for(int i=0; i<3; i++)
      {
         j2kPos(i) = j2kPosVel(i);
         j2kVel(i) = j2kPosVel(i+3);
      }

      Vector<double> ecefPos = c2t * j2kPos;
      Vector<double> ecefVel = c2t * j2kVel + dc2t * j2kPos;
      
      Vector<double> ecefPosVel(6,0.0);
      for(int i=0; i<3; i++)
      {
         ecefPosVel(i) = ecefPos(i);
         ecefPosVel(i+3) = ecefVel(i);
      }

      return ecefPosVel;

   }  // End of method 'ReferenceFrames::J2kPosVelToECEF()'


   Vector<double> ReferenceFrames::ECEFPosVelToJ2k(UTCTime UTC, Vector<double> ecefPosVel)
      throw(Exception)
   {
      Matrix<double> POM, Theta, NP;
      J2kToECEFMatrix(UTC,POM,Theta,NP);

      const double dera = earthRotationAngleRate1(UTC.mjdTT());

      // Derivative of Earth rotation 
      Matrix<double> S(3,3,0.0);
      S(0,1) = 1.0; S(1,0) = -1.0;      

      Matrix<double> dTheta = dera * S * Theta;

      Matrix<double> c2t = POM * Theta * NP;
      Matrix<double> dc2t = POM * dTheta * NP;
      
      Vector<double> ecefPos(3, 0.0), ecefVel(3, 0.0);
      for(int i=0; i<3; i++)
      {
         ecefPos(i) = ecefPosVel(i);
         ecefVel(i) = ecefPosVel(i+3);
      }

      Vector<double> j2kPos = transpose(c2t) * ecefPos;
      Vector<double> j2kVel = transpose(c2t) * ecefVel 
                             +transpose(dc2t)* ecefPos;

      Vector<double> j2kPosVel(6,0.0);
      for(int i=0; i<3; i++)
      {
         j2kPosVel(i) = j2kPos(i);
         j2kPosVel(i+3) = j2kVel(i);
      }

      return j2kPosVel;

   }  // End of method 'ReferenceFrames::ECEFPosVelToJ2k()'
 
      // Get earth rotation angle
   double ReferenceFrames::earthRotationAngle(DayTime UT1)
   {
      // IAU 2000 model
      double t = UT1.MJD() + (JD_TO_MJD - DJ00);
      double f = fmod(UT1.MJD(),1.0) + fmod(JD_TO_MJD, 1.0);
      
      double era = normalizeAngle(D2PI*(f+0.7790572732640+0.00273781191135448*t));
      
      return era;
   }

      /*Earth rotation angle first order rate.
       *  @param mjdTT         Modified Julian Date in TT
       *  @return              d(GAST)/d(t) in [rad]
       */
   double ReferenceFrames::earthRotationAngleRate1(const double& mjdTT)
   {
      double T = (mjdTT - (JD_TO_MJD - DJ00) )/36525.0;
      double dera = (1.002737909350795 + 5.9006e-11 * T - 5.9e-15 * T * T ) 
         * D2PI / 86400.0;

      return dera;
   }


      /*Earth rotation angle second order rate .
       * @param   Modified Julian Date in TT
       * @return  d(GAST)2/d(t)2 in [rad]
       */
   double ReferenceFrames::earthRotationAngleRate2(const double& mjdTT)
   {
      double T = ( mjdTT - (JD_TO_MJD - DJ00) ) / 36525.0;
      double dera = (5.9006e-11 - 5.9e-15 * T) * D2PI / 86400.0;

      return dera;
   }


      /*Earth rotation angle third order rate.
       * @param   Modified Julian Date in TT
       * @return  d(GAST)3/d(t)3 in [rad]
       */
   double ReferenceFrames::earthRotationAngleRate3(const double& mjdTT)
   {
      double T = ( mjdTT - (JD_TO_MJD - DJ00) ) / 36525.0;
      double dera = ( -5.9e-15 ) * D2PI / 86400.0;

      return dera;
   }

         /**
          * COMPUTE DOODSON'S FUNDAMENTAL ARGUMENTS (BETA)
          * AND FUNDAMENTAL ARGUMENTS FOR NUTATION (FNUT)
          * BETA=(B1,B2,B3,B4,B5,B6)
          * FNUT=(F1,F2,F3,F4,F5)
          * F1=MEAN ANOMALY (MOON)
          * F2=MEAN ANOMALY (SUN)
          * F3=F=MOON'S MEAN LONGITUDE-LONGITUDE OF LUNAR ASC. NODE
          * F4=D=MEAN ELONGATION OF MOON FROM SUN
          * F5=MEAN LONGITUDE OF LUNAR ASC. NODE
          *
          * B2=S=F3+F5
          * B3=H=S-F4=S-D
          * B4=P=S-F1
          * B5=NP=-F5
          * B6=PS=S-F4-F2
          * B1=THETA+PI-S
          *
          * @param  mjdUT1 TIME IN MJD
          * @param  mjdTT  TIME IN TT
          * @param  THETA  CORRESPONDING MEAN SID.TIME GREENWICH     
          * @param  BETA   DOODSON ARGUMENTS                         
          * @param  FNUT   FUNDAMENTAL ARGUMENTS FOR NUTATION        
          */
   void ReferenceFrames::doodsonArguments(DayTime UT1, 
                                          DayTime TT, 
                                          double BETA[6],
                                          double FNUT[5])
   {
      // GMST based IAU2000
      double THETA = iauGmst00(UT1,TT);

      // Fundamental Arguments (from IERS Conventions 2003) 
      //-----------------------------------------------------
      //Julian centuries since J2000 
      double  t = (TT.MJD() + 2400000.5 - 2451545.0) / 36525.0; 

      // Mean anomaly of the Moon.
      double temp = fmod(           485868.249036  +
         t * ( 1717915923.2178 +
         t * (         31.8792 +
         t * (          0.051635 +
         t * (        - 0.00024470 ) ) ) ), TURNAS ) * DAS2R;

      double F1 = normalizeAngle(temp);         // -pi,pi

      // Mean anomaly of the Sun
      temp = fmod(         1287104.793048 +
         t * ( 129596581.0481 +
         t * (       - 0.5532 +
         t * (         0.000136 +
         t * (       - 0.00001149 ) ) ) ), TURNAS ) * DAS2R;

      double F2 = normalizeAngle(temp);      // -pi,pi
      
      // Mean longitude of the Moon minus that of the ascending node.
      temp = fmod(           335779.526232 +
         t * ( 1739527262.8478 +
         t * (       - 12.7512 +
         t * (        - 0.001037 +
         t * (          0.00000417 ) ) ) ), TURNAS ) * DAS2R;
      double F3 = normalizeAngle(temp);         // -pi,pi
      
      // Mean elongation of the Moon from the Sun. 
      temp = fmod(          1072260.703692 +
         t * ( 1602961601.2090 +
         t * (        - 6.3706 +
         t * (          0.006593 +
         t * (        - 0.00003169 ) ) ) ), TURNAS ) * DAS2R;

      double F4 = normalizeAngle(temp);         // -pi,pi
      
      // Mean longitude of the ascending node of the Moon.
      temp = fmod(          450160.398036 +
         t * ( - 6962890.5431 +
         t * (         7.4722 +
         t * (         0.007702 +
         t * (       - 0.00005939 ) ) ) ), TURNAS ) * DAS2R;
      double F5 = normalizeAngle(temp);      // -pi,pi

      FNUT[0] = F1;
      FNUT[1] = F2;
      FNUT[2] = F3;
      FNUT[3] = F4;
      FNUT[4] = F5;


      double S = F3+F5;

      BETA[0] = THETA+ASConstant::PI-S;
      BETA[1] = F3+F5;
      BETA[2] = S-F4;
      BETA[3] = S-F1;
      BETA[4] = -F5;
      BETA[5] = S-F4-F2;

   }  // End of method 'ReferenceFrames::doodsonArguments()'
   

   void ReferenceFrames::test()
   {
      IERS::loadSTKFile("InputData/EOP-v1.1.txt");

      
      double rv_j2k[6]={-23830.593e3,-9747.074e3,-6779.829e3,
                        +1.561964e3,-1.754346e3,-3.068851e3};

      Vector<double> j2kPosVel(6,0.0);
      Vector<double> ecefPosVel(6,0.0);

      // 2007 07 01 54282  0.192316  0.407299 -0.1582305
      UTCTime utc(2007,07,01,00,0,0.0);
     
      j2kPosVel = rv_j2k;
      ecefPosVel = J2kPosVelToECEF(utc,j2kPosVel);

      for(int i=0;i<6;i++)
         cout<<setprecision(12)<<ecefPosVel(i)<<endl;

      int a =0;

      /*
      std::string fileHeader = "de405/header.405";
      std::vector<std::string> fileData;

      std::ifstream fin("de405.txt");

      std::string path,header,testpo;

      getline(fin,path);
      getline(fin,header);
      getline(fin,testpo);

      fileHeader = path + header;

      std::string buf;
      while(getline(fin,buf))
      {
         std::string file = path + buf;
         fileData.push_back(file);
      }

      fin.close();

      solarPlanets.readASCIIheader(fileHeader);
      cout<<solarPlanets.readASCIIdata(fileData)<<endl;

      cout<<solarPlanets.writeBinaryFile("jplde405")<<endl;
      */
      
      /*
      solarPlanets.initializeWithBinaryFile("jplde405");
      double rv[6] = {0.0};
      DayTime t(2010,1,1,0,0,0.0);
      int ret = solarPlanets.computeState(t.JD(),SolarSystem::Moon,SolarSystem::Earth,rv);
      rv[3]/=86400.0;
      rv[4]/=86400.0;
      rv[5]/=86400.0;
      cout<<ret<<endl;

      int a = 0;
      */
     
   }

      // Normalize angle into the range -pi <= a < +pi.
   double ReferenceFrames::normalizeAngle(double a)
   {
      double w = fmod(a, D2PI);
      if (fabs(w) >= (D2PI*0.5)) 
      {
         w-= ((a<0.0)?-D2PI:D2PI);
      }

      return w;
   }

      // Rotate an r-matrix about the x-axis.
   Matrix<double> ReferenceFrames::Rx(const double& angle)
   {
      const double s = std::sin(angle);
      const double c = std::cos(angle);

      const double a[9] = { 1, 0, 0, 0, c, s, 0,-s, c };

      Matrix<double> r(3,3,0.0);
      r = a;

      return r;
   }

      // Rotate an r-matrix about the y-axis.
   Matrix<double> ReferenceFrames::Ry(const double& angle)
   {
      const double s = std::sin(angle);
      const double c = std::cos(angle);

      const double a[9] = { c, 0,-s, 0, 1, 0, s, 0, c };

      Matrix<double> r(3,3,0.0);
      r = a;

      return r;
   }

      // Rotate an r-matrix about the z-axis.
   Matrix<double> ReferenceFrames::Rz(const double& angle)
   {
      const double s = std::sin(angle);
      const double c = std::cos(angle);

      const double a[9] = { c, s, 0,-s, c, 0, 0, 0, 1 };

      Matrix<double> r(3,3,0.0);
      r = a;

      return r;
   }

   Matrix<double> ReferenceFrames::iauPmat76(DayTime TT)
   {
      
      // Interval between fundamental epoch J2000.0 and start epoch (JC). 
      const double t0 = 0.0;

      // Interval over which precession required (JC). 
      const double t = (JD_TO_MJD - DJ00 + TT.MJD()) / DJC;

      // Euler angles. 
      const double tas2r = t * DAS2R;
      const double w = 2306.2181 + (1.39656 - 0.000139 * t0) * t0;

      double zeta = (w + ((0.30188 - 0.000344 * t0) + 0.017998 * t) * t) * tas2r;

      double z = (w + ((1.09468 + 0.000066 * t0) + 0.018203 * t) * t) * tas2r;

      double theta = ((2004.3109 + (-0.85330 - 0.000217 * t0) * t0)
         + ((-0.42665 - 0.000217 * t0) - 0.041833 * t) * t) * tas2r;

      return ( Rz(-z) * Ry(theta) * Rz(-zeta) );

   }  // End of method 'ReferenceFrames::iauPmat76()'
   

   void ReferenceFrames::nutationAngles(DayTime TT, double& dpsi, double& deps)
   {
      // Units of 0.1 milliarcsecond to radians 
      const double U2R = DAS2R / 1e4;


      // Table of multiples of arguments and coefficients 
      // ------------------------------------------------ 

      // The units for the sine and cosine coefficients are 0.1 mas and 
      // the same per Julian century 

      static const struct 
      {
         int nl,nlp,nf,nd,nom; // coefficients of l,l',F,D,Om 
         double sp,spt;        // longitude sine, 1 and t coefficients 
         double ce,cet;        // obliquity cosine, 1 and t coefficients 
      } x[] = {

         /* 1-10 */
         {  0,  0,  0,  0,  1, -171996.0, -174.2,  92025.0,    8.9 },
         {  0,  0,  0,  0,  2,    2062.0,    0.2,   -895.0,    0.5 },
         { -2,  0,  2,  0,  1,      46.0,    0.0,    -24.0,    0.0 },
         {  2,  0, -2,  0,  0,      11.0,    0.0,      0.0,    0.0 },
         { -2,  0,  2,  0,  2,      -3.0,    0.0,      1.0,    0.0 },
         {  1, -1,  0, -1,  0,      -3.0,    0.0,      0.0,    0.0 },
         {  0, -2,  2, -2,  1,      -2.0,    0.0,      1.0,    0.0 },
         {  2,  0, -2,  0,  1,       1.0,    0.0,      0.0,    0.0 },
         {  0,  0,  2, -2,  2,  -13187.0,   -1.6,   5736.0,   -3.1 },
         {  0,  1,  0,  0,  0,    1426.0,   -3.4,     54.0,   -0.1 },

         /* 11-20 */
         {  0,  1,  2, -2,  2,    -517.0,    1.2,    224.0,   -0.6 },
         {  0, -1,  2, -2,  2,     217.0,   -0.5,    -95.0,    0.3 },
         {  0,  0,  2, -2,  1,     129.0,    0.1,    -70.0,    0.0 },
         {  2,  0,  0, -2,  0,      48.0,    0.0,      1.0,    0.0 },
         {  0,  0,  2, -2,  0,     -22.0,    0.0,      0.0,    0.0 },
         {  0,  2,  0,  0,  0,      17.0,   -0.1,      0.0,    0.0 },
         {  0,  1,  0,  0,  1,     -15.0,    0.0,      9.0,    0.0 },
         {  0,  2,  2, -2,  2,     -16.0,    0.1,      7.0,    0.0 },
         {  0, -1,  0,  0,  1,     -12.0,    0.0,      6.0,    0.0 },
         { -2,  0,  0,  2,  1,      -6.0,    0.0,      3.0,    0.0 },

         /* 21-30 */
         {  0, -1,  2, -2,  1,      -5.0,    0.0,      3.0,    0.0 },
         {  2,  0,  0, -2,  1,       4.0,    0.0,     -2.0,    0.0 },
         {  0,  1,  2, -2,  1,       4.0,    0.0,     -2.0,    0.0 },
         {  1,  0,  0, -1,  0,      -4.0,    0.0,      0.0,    0.0 },
         {  2,  1,  0, -2,  0,       1.0,    0.0,      0.0,    0.0 },
         {  0,  0, -2,  2,  1,       1.0,    0.0,      0.0,    0.0 },
         {  0,  1, -2,  2,  0,      -1.0,    0.0,      0.0,    0.0 },
         {  0,  1,  0,  0,  2,       1.0,    0.0,      0.0,    0.0 },
         { -1,  0,  0,  1,  1,       1.0,    0.0,      0.0,    0.0 },
         {  0,  1,  2, -2,  0,      -1.0,    0.0,      0.0,    0.0 },

         /* 31-40 */
         {  0,  0,  2,  0,  2,   -2274.0,   -0.2,    977.0,   -0.5 },
         {  1,  0,  0,  0,  0,     712.0,    0.1,     -7.0,    0.0 },
         {  0,  0,  2,  0,  1,    -386.0,   -0.4,    200.0,    0.0 },
         {  1,  0,  2,  0,  2,    -301.0,    0.0,    129.0,   -0.1 },
         {  1,  0,  0, -2,  0,    -158.0,    0.0,     -1.0,    0.0 },
         { -1,  0,  2,  0,  2,     123.0,    0.0,    -53.0,    0.0 },
         {  0,  0,  0,  2,  0,      63.0,    0.0,     -2.0,    0.0 },
         {  1,  0,  0,  0,  1,      63.0,    0.1,    -33.0,    0.0 },
         { -1,  0,  0,  0,  1,     -58.0,   -0.1,     32.0,    0.0 },
         { -1,  0,  2,  2,  2,     -59.0,    0.0,     26.0,    0.0 },

         /* 41-50 */
         {  1,  0,  2,  0,  1,     -51.0,    0.0,     27.0,    0.0 },
         {  0,  0,  2,  2,  2,     -38.0,    0.0,     16.0,    0.0 },
         {  2,  0,  0,  0,  0,      29.0,    0.0,     -1.0,    0.0 },
         {  1,  0,  2, -2,  2,      29.0,    0.0,    -12.0,    0.0 },
         {  2,  0,  2,  0,  2,     -31.0,    0.0,     13.0,    0.0 },
         {  0,  0,  2,  0,  0,      26.0,    0.0,     -1.0,    0.0 },
         { -1,  0,  2,  0,  1,      21.0,    0.0,    -10.0,    0.0 },
         { -1,  0,  0,  2,  1,      16.0,    0.0,     -8.0,    0.0 },
         {  1,  0,  0, -2,  1,     -13.0,    0.0,      7.0,    0.0 },
         { -1,  0,  2,  2,  1,     -10.0,    0.0,      5.0,    0.0 },

         /* 51-60 */
         {  1,  1,  0, -2,  0,      -7.0,    0.0,      0.0,    0.0 },
         {  0,  1,  2,  0,  2,       7.0,    0.0,     -3.0,    0.0 },
         {  0, -1,  2,  0,  2,      -7.0,    0.0,      3.0,    0.0 },
         {  1,  0,  2,  2,  2,      -8.0,    0.0,      3.0,    0.0 },
         {  1,  0,  0,  2,  0,       6.0,    0.0,      0.0,    0.0 },
         {  2,  0,  2, -2,  2,       6.0,    0.0,     -3.0,    0.0 },
         {  0,  0,  0,  2,  1,      -6.0,    0.0,      3.0,    0.0 },
         {  0,  0,  2,  2,  1,      -7.0,    0.0,      3.0,    0.0 },
         {  1,  0,  2, -2,  1,       6.0,    0.0,     -3.0,    0.0 },
         {  0,  0,  0, -2,  1,      -5.0,    0.0,      3.0,    0.0 },

         /* 61-70 */
         {  1, -1,  0,  0,  0,       5.0,    0.0,      0.0,    0.0 },
         {  2,  0,  2,  0,  1,      -5.0,    0.0,      3.0,    0.0 },
         {  0,  1,  0, -2,  0,      -4.0,    0.0,      0.0,    0.0 },
         {  1,  0, -2,  0,  0,       4.0,    0.0,      0.0,    0.0 },
         {  0,  0,  0,  1,  0,      -4.0,    0.0,      0.0,    0.0 },
         {  1,  1,  0,  0,  0,      -3.0,    0.0,      0.0,    0.0 },
         {  1,  0,  2,  0,  0,       3.0,    0.0,      0.0,    0.0 },
         {  1, -1,  2,  0,  2,      -3.0,    0.0,      1.0,    0.0 },
         { -1, -1,  2,  2,  2,      -3.0,    0.0,      1.0,    0.0 },
         { -2,  0,  0,  0,  1,      -2.0,    0.0,      1.0,    0.0 },

         /* 71-80 */
         {  3,  0,  2,  0,  2,      -3.0,    0.0,      1.0,    0.0 },
         {  0, -1,  2,  2,  2,      -3.0,    0.0,      1.0,    0.0 },
         {  1,  1,  2,  0,  2,       2.0,    0.0,     -1.0,    0.0 },
         { -1,  0,  2, -2,  1,      -2.0,    0.0,      1.0,    0.0 },
         {  2,  0,  0,  0,  1,       2.0,    0.0,     -1.0,    0.0 },
         {  1,  0,  0,  0,  2,      -2.0,    0.0,      1.0,    0.0 },
         {  3,  0,  0,  0,  0,       2.0,    0.0,      0.0,    0.0 },
         {  0,  0,  2,  1,  2,       2.0,    0.0,     -1.0,    0.0 },
         { -1,  0,  0,  0,  2,       1.0,    0.0,     -1.0,    0.0 },
         {  1,  0,  0, -4,  0,      -1.0,    0.0,      0.0,    0.0 },

         /* 81-90 */
         { -2,  0,  2,  2,  2,       1.0,    0.0,     -1.0,    0.0 },
         { -1,  0,  2,  4,  2,      -2.0,    0.0,      1.0,    0.0 },
         {  2,  0,  0, -4,  0,      -1.0,    0.0,      0.0,    0.0 },
         {  1,  1,  2, -2,  2,       1.0,    0.0,     -1.0,    0.0 },
         {  1,  0,  2,  2,  1,      -1.0,    0.0,      1.0,    0.0 },
         { -2,  0,  2,  4,  2,      -1.0,    0.0,      1.0,    0.0 },
         { -1,  0,  4,  0,  2,       1.0,    0.0,      0.0,    0.0 },
         {  1, -1,  0, -2,  0,       1.0,    0.0,      0.0,    0.0 },
         {  2,  0,  2, -2,  1,       1.0,    0.0,     -1.0,    0.0 },
         {  2,  0,  2,  2,  2,      -1.0,    0.0,      0.0,    0.0 },

         /* 91-100 */
         {  1,  0,  0,  2,  1,      -1.0,    0.0,      0.0,    0.0 },
         {  0,  0,  4, -2,  2,       1.0,    0.0,      0.0,    0.0 },
         {  3,  0,  2, -2,  2,       1.0,    0.0,      0.0,    0.0 },
         {  1,  0,  2, -2,  0,      -1.0,    0.0,      0.0,    0.0 },
         {  0,  1,  2,  0,  1,       1.0,    0.0,      0.0,    0.0 },
         { -1, -1,  0,  2,  1,       1.0,    0.0,      0.0,    0.0 },
         {  0,  0, -2,  0,  1,      -1.0,    0.0,      0.0,    0.0 },
         {  0,  0,  2, -1,  2,      -1.0,    0.0,      0.0,    0.0 },
         {  0,  1,  0,  2,  0,      -1.0,    0.0,      0.0,    0.0 },
         {  1,  0, -2, -2,  0,      -1.0,    0.0,      0.0,    0.0 },

         /* 101-106 */
         {  0, -1,  2,  0,  1,      -1.0,    0.0,      0.0,    0.0 },
         {  1,  1,  0, -2,  1,      -1.0,    0.0,      0.0,    0.0 },
         {  1,  0, -2,  2,  0,      -1.0,    0.0,      0.0,    0.0 },
         {  2,  0,  0,  2,  0,       1.0,    0.0,      0.0,    0.0 },
         {  0,  0,  2,  4,  2,      -1.0,    0.0,      0.0,    0.0 },
         {  0,  1,  0,  1,  0,       1.0,    0.0,      0.0,    0.0 }
      };

      // Number of terms in the series 
      const int NT = (int) (sizeof x / sizeof x[0]);

     
      // Interval between fundamental epoch J2000.0 and given date (JC). 
      const double t = ((JD_TO_MJD - DJ00) + TT.MJD()) / DJC;

      // Fundamental arguments 
      // --------------------- 

      // Mean longitude of Moon minus mean longitude of Moon's perigee. 
      double el = normalizeAngle(
         (485866.733 + (715922.633 + (31.310 + 0.064 * t) * t) * t)
         * DAS2R + fmod(1325.0 * t, 1.0) * D2PI);

      // Mean longitude of Sun minus mean longitude of Sun's perigee. 
      double elp = normalizeAngle(
         (1287099.804 + (1292581.224 + (-0.577 - 0.012 * t) * t) * t)
         * DAS2R + fmod(99.0 * t, 1.0) * D2PI);

      // Mean longitude of Moon minus mean longitude of Moon's node. 
      double f = normalizeAngle(
         (335778.877 + (295263.137 + (-13.257 + 0.011 * t) * t) * t)
         * DAS2R + fmod(1342.0 * t, 1.0) * D2PI);

      // Mean elongation of Moon from Sun. 
      double d = normalizeAngle(
         (1072261.307 + (1105601.328 + (-6.891 + 0.019 * t) * t) * t)
         * DAS2R + fmod(1236.0 * t, 1.0) * D2PI);

      // Longitude of the mean ascending node of the lunar orbit on the 
      // ecliptic, measured from the mean equinox of date. 
      double om = normalizeAngle(
         (450160.280 + (-482890.539 + (7.455 + 0.008 * t) * t) * t)
         * DAS2R + fmod(-5.0 * t, 1.0) * D2PI);


      // Nutation series 
      // --------------- 

      // Initialize nutation components. 
      double dp = 0.0;
      double de = 0.0;

      // Sum the nutation terms, ending with the biggest. 
      for (int j = NT-1; j >= 0; j--) 
      {

         // Form argument for current term. 
         double arg = (double)x[j].nl  * el
            + (double)x[j].nlp * elp
            + (double)x[j].nf  * f
            + (double)x[j].nd  * d
            + (double)x[j].nom * om;

         // Accumulate current nutation term. 
         double s = x[j].sp + x[j].spt * t;
         double c = x[j].ce + x[j].cet * t;
         if (s != 0.0) dp += s * std::sin(arg);
         if (c != 0.0) de += c * std::cos(arg);
      }

      // Convert results from 0.1 mas units to radians. 
      dpsi = dp * U2R;
      deps = de * U2R;

   }  // End of 'ReferenceFrames::nutationAngles()'


   double ReferenceFrames::meanObliquity(DayTime TT)
   {
     
      // Interval between fundamental epoch J2000.0 and given date (JC)
      double t = ((JD_TO_MJD - DJ00) + TT.MJD()) / DJC;

      // Mean obliquity of date. 
      double eps0 = DAS2R * (84381.448  +
         (-46.8150   +
         (-0.00059   +
         ( 0.001813) * t) * t) * t);

      return eps0;
   }

   double ReferenceFrames::iauEqeq94(DayTime TT)
   {
      // Interval between fundamental epoch J2000.0 and given date (JC). 
      double t = ((JD_TO_MJD - DJ00) + TT.MJD()) / DJC;

      // Longitude of the mean ascending node of the lunar orbit on the 
      // ecliptic, measured from the mean equinox of date. 
      double om = normalizeAngle((450160.280 + (-482890.539
         + (7.455 + 0.008 * t) * t) * t) * DAS2R
         + fmod(-5.0 * t, 1.0) * D2PI);

      // Nutation components and mean obliquity. 
      double dpsi(0.0), deps(0.0);
      nutationAngles(TT, dpsi, deps);
      
      double eps0 = meanObliquity(TT);

      // Equation of the equinoxes. 
      double ee = dpsi * std::cos(eps0) 
         + DAS2R*(0.00264 * std::sin(om) + 0.000063 * std::sin(om + om));

      return ee;
   }

   double ReferenceFrames::iauGmst82(DayTime UT1)
   {
      // Coefficients of IAU 1982 GMST-UT1 model 
      const double A = 24110.54841  -  86400.0 / 2.0;
      const double B = 8640184.812866;
      const double C = 0.093104;
      const double D =  -6.2e-6;

      // Note: the first constant, A, has to be adjusted by 12 hours 
      // because the UT1 is supplied as a Julian date, which begins  
      // at noon.                                                    

      // Julian centuries since fundamental epoch. 
      double d2 = JD_TO_MJD;
      double d1 = UT1.MJD();
      double t = (d1 + (d2 - DJ00)) / DJC;

      // Fractional part of JD(UT1), in seconds. 
      double f = 86400.0 * (fmod(d1, 1.0) + fmod(d2, 1.0));

      // GMST at this UT1. 
      double gmst = normalizeAngle(
         DS2R * ((A + (B + (C + D * t) * t) * t) + f));

      return gmst;

   }  // End of method 'ReferenceFrames::iauGmst82()'

      // Greenwich mean sidereal time by IAU 2000 model
   double ReferenceFrames::iauGmst00(DayTime UT1,DayTime TT)
   {

      // TT Julian centuries since J2000.0. 
      double t = ((JD_TO_MJD - DJ00) + TT.MJD()) / DJC;

      /* Greenwich Mean Sidereal Time, IAU 2000. */
      double gmst = normalizeAngle(earthRotationAngle(UT1) +
         (     0.014506   +
         (  4612.15739966 +
         (     1.39667721 +
         (    -0.00009344 +
         (     0.00001882 )
         * t) * t) * t) * t) * DAS2R);

      return gmst;

   }  // End of method 'ReferenceFrames::iauGmst00()'

      // Nutation matrix from nutation angles
   Matrix<double> ReferenceFrames::iauNmat(const double& epsa,
                                           const double& dpsi, 
                                           const double& deps)
   {
      return ( Rx(-(epsa+deps)) * Rz(-dpsi) * Rx(epsa) );
   }


   Matrix<double> ReferenceFrames::enuMatrix(double longitude, double latitude)
   {
      const double sb = std::sin(latitude);
      const double cb = std::cos(latitude);
      const double sl = std::sin(longitude);
      const double cl = std::cos(longitude);

      double r[3][3]={{-sl,cl,0.0},{-sb*cl,-sb*sl, cb},{ cb*cl,cb*sl,sb}};
      
      Matrix<double> enuMat(3,3,0.0);
      enuMat = &r[0][0];

      return enuMat;

   }  // End of method 'ReferenceFrames::enuMatrix()'

   // return Azimuth Elevation slant
   Vector<double> ReferenceFrames::enuToAzElDt(Vector<double> enu)
   {
      gpstk::Vector<double> r(3,0.0);

      const double rho = std::sqrt(enu(0)*enu(0)+enu(1)*enu(1));

      // Angles
      double A = std::atan2(enu(0),enu(1));
      A = (A<0.0)? (A+ASConstant::TWO_PI) : A;
      double E = std::atan ( enu(2) / rho );

      r(0) = A;
      r(1) = E;
      r(2) = norm(enu);

      return r;
   }


   void ReferenceFrames::XYZ2BLH(double xyz[3],double blh[3])
   {
      const double f = ASConstant::f_Earth; //sqrt(0.00669437999013);
      const double R_equ = ASConstant::R_Earth; //Equator radius [m]
      const double e2 = f*(2.0-f);          // Square of eccentricity
      const double e = std::sqrt(e2);

      const double  eps     = 1.0e3*std::numeric_limits<double>::epsilon();;   // Convergence criterion 
      const double  epsRequ = eps*R_equ;


      const double  X = xyz[0];                   // Cartesian coordinates
      const double  Y = xyz[1];
      const double  Z = xyz[2];
      const double  rho2 = X*X + Y*Y;           // Square of distance from z-axis
      const double  rho = std::sqrt(rho2+Z*Z);

      // Check validity of input data
      if (rho==0.0) 
      {
         blh[0] = 0.0;
         blh[1] = 0.0;
         blh[2] = -R_equ;
         return;
      }

      // Iteration 
      double  dZ, dZ_new, SinPhi;
      double  ZdZ, Nh, N;

      dZ = e2*Z;
      while(1) 
      {
         ZdZ    =  Z + dZ;
         Nh     =  std::sqrt ( rho2 + ZdZ*ZdZ ); 
         SinPhi =  ZdZ / Nh;                    // Sine of geodetic latitude
         N      =  R_equ / std::sqrt(1.0-e2*SinPhi*SinPhi); 
         dZ_new =  N*e2*SinPhi;
         if ( std::fabs(dZ-dZ_new) < epsRequ ) break;
         dZ = dZ_new;
      }

      // latitude, Longitude, altitude
      blh[0] = std::atan2 ( ZdZ, std::sqrt(rho2) );
      blh[1] = std::atan2 ( Y, X );
      blh[2] = Nh - N;

   }  // End of method 'ReferenceFrames::XYZ2BLH()'

   void ReferenceFrames::BLH2XYZ(double blh[3],double xyz[3])
   {
      const double f = ASConstant::f_Earth; //sqrt(0.00669437999013);
      const double a = ASConstant::R_Earth; //Equator radius [m]
      const double e2 = f*(2.0-f);          // Square of eccentricity

      double N=a/(std::sqrt(1-e2*std::sin(blh[0])*std::sin(blh[0])));
      xyz[0]=(N+blh[2])*std::cos(blh[0])*std::cos(blh[1]);
      xyz[1]=(N+blh[2])*std::cos(blh[0])*std::sin(blh[1]);
      xyz[2]=(N*(1-e2)+blh[2])*std::sin(blh[0]);

   }  // End of method 'ReferenceFrames::BLH2XYZ()'


   void ReferenceFrames::XYZ2ENU(double blh[3],double xyz[3],double enu[3])
   {
      double xyz0[3]={0.0};
      BLH2XYZ(blh,xyz0);

      double dxyz[3]={0.0};
      dxyz[0] = xyz[0]-xyz0[0];
      dxyz[1] = xyz[1]-xyz0[1];
      dxyz[2] = xyz[2]-xyz0[2];

      const double sb = std::sin(blh[0]);
      const double cb = std::cos(blh[0]);
      const double sl = std::sin(blh[1]);
      const double cl = std::cos(blh[1]);

      double r[3][3]={{-sl,cl,0.0},{-sb*cl,-sb*sl, cb},{ cb*cl,cb*sl,sb}};
      
      enu[0] = r[0][0] * dxyz[0] + r[0][1] * dxyz[1] + r[0][2] * dxyz[2];
      enu[1] = r[1][0] * dxyz[0] + r[1][1] * dxyz[1] + r[1][2] * dxyz[2];
      enu[2] = r[2][0] * dxyz[0] + r[2][1] * dxyz[1] + r[2][2] * dxyz[2];

   }  // End of method 'ReferenceFrames::XYZ2ENU()'


}  // End of namespace 'gsptk'


