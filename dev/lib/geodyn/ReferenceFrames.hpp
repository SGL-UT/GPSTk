#pragma ident "$Id: $"

/**
 * @file ReferenceFrames.hpp
 * Class to do Reference frame transformation.
 */


#ifndef GPSTK_REFERENCE_FRAMES_HPP
#define GPSTK_REFERENCE_FRAMES_HPP


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



#include "Vector.hpp"
#include "Matrix.hpp"
#include "SolarSystem.hpp"
#include "UTCTime.hpp"

namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{

      /** Class to do Reference frame transformation.
       */
   class ReferenceFrames
   {
   public:

         /** Open the given binary file, 
          *  
          * @param filename  name of binary file to be read.
          * @return 0 success,
          *        -3 input stream is not open or not valid
          *        -4 header has not yet been read.
          * @throw if a gap in time is found between consecutive records.
          */
      static int setJPLEphFile(std::string filename) 
         throw(Exception)
      {
         return solarPlanets.initializeWithBinaryFile(filename);
      }

         /** Compute planet position in J2000
          *  
          * @param TT         Time(Modified Julian Date in TT<TAI+32.184>) of interest 
          * @param entity     The planet to be computed
          * @return           The position of the planet in km
          */
      static Vector<double> getJ2kPosition( const DayTime&      TT,
                                            SolarSystem::Planet entity = SolarSystem::Earth)
         throw(Exception);

         /** Compute planet velocity in J2000
          *  
          * @param TT         Time(Modified Julian Date in TT<TAI+32.184>) of interest 
          * @param entity     The planet to be computed
          * @return           The velocity of the planet in km/s
          */
      static Vector<double> getJ2kVelocity( const DayTime&      TT, 
                                            SolarSystem::Planet entity = SolarSystem::Earth)
         throw(Exception);



         /** Compute planet position and velocity in J2000
          *  
          * @param TT         Time(Modified Julian Date in TT<TAI+32.184>) of interest 
          * @param entity     The planet to be computed
          * @param center     relative to whick the result apply
          * @return           The position and velocity of the planet in km and km/s
          */
      static Vector<double> getJ2kPosVel( const DayTime&      TT, 
                                          SolarSystem::Planet entity,
                                          SolarSystem::Planet center = SolarSystem::Earth)
         throw(Exception);
      
         /** Compute planet position in ECEF
          *  
          * @param UTC        Time(Modified Julian Date in UTC) of interest 
          * @param entity     The planet to be computed
          * @param center     relative to whick the result apply
          * @return           The position of the planet in km 
          */
      static Vector<double> getECEFPosition(UTCTime             UTC, 
                                          SolarSystem::Planet entity,
                                          SolarSystem::Planet center = SolarSystem::Earth)
         throw(Exception);

      
         /** Compute planet velocity in ECEF
          *  
          * @param UTC        Time(Modified Julian Date in UTC) of interest 
          * @param entity     The planet to be computed
          * @param center     relative to whick the result apply
          * @return           The position of the planet in km/s
          */
      static Vector<double> getECEFVelocity(UTCTime             UTC, 
                                          SolarSystem::Planet entity,
                                          SolarSystem::Planet center = SolarSystem::Earth)
         throw(Exception);


          /** Compute planet position and velocity in ECEF
           *  
           * @param UTC        Time(Modified Julian Date in UTC) of interest 
           * @param entity     The planet to be computed
           * @param center     relative to whick the result apply
           * @return           The position and velocity of the planet in km and km/s
           */
      static Vector<double> getECEFPosVel(UTCTime             UTC, 
                                          SolarSystem::Planet entity,
                                          SolarSystem::Planet center = SolarSystem::Earth)
         throw(Exception);


         /// ECEF = POM * Theta * NP * J2k
      static void J2kToECEFMatrix(UTCTime         UTC, 
                                  Matrix<double>& POM,
                                  Matrix<double>& Theta, 
                                  Matrix<double>& NP)
         throw(Exception);


         /// Get ECI to ECF transform matrix, POM * Theta * NP 
      static Matrix<double> J2kToECEFMatrix(UTCTime UTC);
         
         /// NP TOD - TrueOfDate
      static Matrix<double> J2kToTODMatrix(UTCTime UTC);

         /// Convert position and velocity from J2000 to ECEF.
      static Vector<double> J2kPosVelToECEF(UTCTime UTC, Vector<double> j2kPosVel)
         throw(Exception);
         
         /// Convert position and velocity from ECEF to J2000.
      static Vector<double> ECEFPosVelToJ2k(UTCTime UTC, Vector<double> ecefPosVel)
         throw(Exception);


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
      static void doodsonArguments(DayTime UT1,DayTime TT,double BETA[6],double FNUT[5]); 


      /// Greenwich mean sidereal time by IAU 2000 model
      static double iauGmst00(DayTime UT1,DayTime TT);


      static Matrix<double> enuMatrix(double longitude,double latitude);

      static Vector<double> enuToAzElDt(Vector<double> enu);

      static void XYZ2BLH(double xyz[3],double blh[3]);
      static void BLH2XYZ(double blh[3],double xyz[3]);

      static void XYZ2ENU(double blh[3],double xyz[3],double enu[3]);
      static void ENU2XYZ(double blh[3],double enu[3],double xyz[3]);

         /// some test
      static void test();

         /// Rotate a matrix about the x-axis.
      static Matrix<double> Rx(const double& angle);

         /// Rotate a matrix about the y-axis.
      static Matrix<double> Ry(const double& angle);

         /// Rotate a matrix about the z-axis.
      static Matrix<double> Rz(const double& angle);

   protected:
        
         /// Default constructor
      ReferenceFrames(){}

         /// Default deconstructor
      ~ReferenceFrames(){}

         
         /// Precession matrix by IAU 1976 model
      static Matrix<double> iauPmat76(DayTime TT);
         
         /// Nutation angles by IAU 1980 model
      static void nutationAngles(DayTime TT, double& dpsi, double& deps);
         
         /// Mean obliquity of the ecliptic by IAU 1980 model
      static double meanObliquity(DayTime TT);
         
         /// Equation of the equinoxes by IAU 1994 model
      static double iauEqeq94(DayTime TT);
         
         /// Greenwich mean sidereal time by IAU 1982 model
      static double iauGmst82(DayTime UT1);

           
         /// Nutation matrix from nutation angles
      static Matrix<double> iauNmat(const double& epsa, 
                                    const double& dpsi, 
                                    const double& deps);

         /// earth rotation angle
      static double earthRotationAngle(DayTime UT1);


         /**Earth rotation angle first order rate.
          *  @param mjdTT         Modified Julian Date in TT
		    *  @return              d(GAST)/d(t) in [rad]
		    */
		static double earthRotationAngleRate1(const double& mjdTT);


		   /**Earth rotation angle second order rate .
		    * @param   Modified Julian Date in TT
          * @return  d(GAST)2/d(t)2 in [rad]
		    */
		static double earthRotationAngleRate2(const double& mjdTT);


		   /**Earth rotation angle third order rate.
		    * @param   Modified Julian Date in TT
          * @return  d(GAST)3/d(t)3 in [rad]
		    */
		static double earthRotationAngleRate3(const double& mjdTT);

  
         /// Normalize angle into the range -pi <= a < +pi.
      static double normalizeAngle(double a);

        
   private:

         /// Objects to handle the JPL Ephemeris
      static SolarSystem solarPlanets;

      // Constant Variables
      //-------------------------------------------------

         /// Reference epoch (J2000), Julian Date
      static const double DJ00;

         /// Conversion offset, Julian Date to Modified Julian Date.
      static const double JD_TO_MJD; 

         /// 2PI
      static const double D2PI;

         /// Days per Julian century 
      static const double DJC;

         /// Arcseconds to radians 
      static const double DAS2R;

         /// seconds to radians
      static const double DS2R;

         /// Arcseconds in a full circle 
      static const double TURNAS;

   }; // End of class 'ReferenceFrames'

      // @}

}  // End of namespace 'gpstk'




#endif  // REFERENCE_FRAMES_HPP
