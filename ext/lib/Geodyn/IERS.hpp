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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
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

/**
 * @file IERS.hpp
 * This class ease handling IERS earth orientation data.
 */

#ifndef GPSTK_IERS_HPP
#define GPSTK_IERS_HPP

#include <string>
#include "IERSConventions.hpp"
#include "PlanetEphemeris.hpp"
#include "MJD.hpp"

namespace gpstk
{
      /// @ingroup GeoDynamics 
      //@{

      /**
       * Class to easy handle IERS Earth Orientation Data globally.
       */
   class IERS
   {
   public:
         /// PI
      static const double PI;

         /// Arcseconds to radius convention 
      static const double ARCSEC2RAD;

      
         /// Return the current IERS version
      static std::string version(){return "IERS1996";}


      //
      // Methods to handle EOP data---------------------------------------------
      //

         /// 'finals.data' from http://maia.usno.navy.mil/
      static void loadIERSFile(const std::string& iersFile)
         throw(FileMissingException){ gpstk::LoadIERSFile(iersFile); } 

         /// ERP data file from IGS
      static void loadIGSFile(const std::string& igsFile)
         throw(FileMissingException){ gpstk::LoadIGSFile(igsFile); } 
 
         /// ERP data file from STK
      static void loadSTKFile(const std::string& stkFile)
         throw(FileMissingException){ gpstk::LoadSTKFile(stkFile); } 


         /// Request EOP Data
      static EOPDataStore::EOPData eopData(const double& mjdUTC)
         throw(InvalidRequest){return gpstk::EOPData( gpstk::MJD(mjdUTC) );}

      static EOPDataStore::EOPData eopData(const CommonTime& UTC)
         throw(InvalidRequest){return gpstk::EOPData(UTC);}
        
         /// Pole coordinate [arcseconds]
         /// @param  Modified Julidate in UTC
         /// @return Pole coordinate x in arcseconds
      static double xPole(const double& mjdUTC)
         throw (InvalidRequest){return gpstk::PolarMotionX( gpstk::MJD(mjdUTC) );}

      static double xPole(const CommonTime& UTC)
         throw (InvalidRequest){return gpstk::PolarMotionX(UTC);}
      
         /// Pole coordinate [arcseconds]
         /// @param  Modified Julidate in UTC
         /// @return Pole coordinate x in arcseconds
      static double yPole(const double& mjdUTC)
         throw (InvalidRequest){ return gpstk::PolarMotionY( gpstk::MJD(mjdUTC) );}

      static double yPole(const CommonTime& UTC)
         throw (InvalidRequest){ return gpstk::PolarMotionY(UTC);}

         /// UT1-UTC time difference [seconds]
         /// @param  Modified Julidate in UTC
         /// @return UT1-UTC time difference in seconds
      static double UT1mUTC(const double& mjdUTC)
         throw (InvalidRequest) { return gpstk::UT1mUTC( gpstk::MJD(mjdUTC) ); } 

      static double UT1mUTC(const CommonTime& UTC)
         throw (InvalidRequest) { return gpstk::UT1mUTC(UTC); } 
     
         /// Nutation dPsi [arcseconds]
         /// @param  Modified Julidate in UTC
         /// @return dPsi in arcseconds
      static double dPsi(const double& mjdUTC)
         throw (InvalidRequest){return gpstk::NutationDPsi( gpstk::MJD(mjdUTC) );}

      static double dPsi(const CommonTime& UTC)
         throw (InvalidRequest){return gpstk::NutationDPsi(UTC);}

         /// Nutation dEps [arcseconds]
         /// @param  Modified Julidate in UTC
         /// @return dEps in arcseconds
      static double dEps(const double& mjdUTC)
         throw (InvalidRequest){return gpstk::NutationDEps( gpstk::MJD(mjdUTC) );}

      static double dEps(const CommonTime& UTC)
         throw (InvalidRequest){return gpstk::NutationDEps(UTC);}

         /** Return the difference between TAI and UTC (known as leap seconds).
          * Values from the USNO website: ftp://maia.usno.navy.mil/ser7/leapsec.dat
          * As of July 19, 2002, no leap second in Dec 2002 so next opportunity for
          * adding a leap second is July 2003. Check IERS Bulletin C.
          * http://hpiers.obspm.fr/eoppc/bul/bulc/UTC-TAI.history
          *
          * @param mjd   Modified Julian Date in UTC
          * @return      number of leaps seconds.
         */
      static int TAImUTC(const double& mjdUTC)
         throw(InvalidRequest){return gpstk::TAImUTC( gpstk::MJD(mjdUTC) ); }

      static int TAImUTC(const CommonTime& UTC)
         throw(InvalidRequest){return gpstk::TAImUTC(UTC); }


      static double TTmTAI(){return gpstk::TTmTAI();}

      
      static double TAImGPST(){return gpstk::TAImGPST();}

      //
      // Methods to handle time system conversion-------------------------------
      //
      //
      //          -14s
      //    -----------------> BDT(Compass Time)
      //    |                                                                    
      //    |         +19s             +32.184s           +rel.effects       
      //   GPST ------------> TAI ----------------> TT -----------------> TDB   
      //                      T |                                         
      //           -(UT1-TAI) | |    -leap seconds                          
      //   UT1 ---------------| |--------------------> UTC                
      //    |                                                              
      //    |   earth rotation                                            
      //    ---------------------> GAST  
      //========================================================================

         /// Transform time between GPST and UTC
      static CommonTime GPST2UTC(const CommonTime& GPST){return gpstk::GPST2UTC(GPST);}
      static CommonTime UTC2GPST(const CommonTime& UTC){return gpstk::UTC2GPST(UTC);}
   
         /// Transform time between UT1 and UTC
      static CommonTime UT12UTC(const CommonTime& UT1){return gpstk::UT12UTC(UT1);}
      static CommonTime UTC2UT1(const CommonTime& UTC){return gpstk::UTC2UT1(UTC);}

      static CommonTime UT12UTC(const CommonTime& UT1,double ut1mutc)
      {return gpstk::UT12UTC(UT1,ut1mutc);}
      
      static CommonTime UTC2UT1(const CommonTime& UTC,double ut1mutc)
      {return gpstk::UTC2UT1(UTC,ut1mutc);}

         /// Transform time between TT and UTC
      static CommonTime TT2UTC(const CommonTime& TT){return gpstk::TT2UTC(TT);}
      static CommonTime UTC2TT(const CommonTime& UTC){return gpstk::UTC2TT(UTC);}

         /// Transform time between TAI and UTC
      static CommonTime TAI2UTC(const CommonTime& TAI){return gpstk::TAI2UTC(TAI);}
      static CommonTime UTC2TAI(const CommonTime& UTC){return gpstk::UTC2TAI(UTC);}

         /// Transform time between BDT(Compass Time) and UTC
      static CommonTime BDT2UTC(const CommonTime& BDT){return gpstk::BDT2UTC(BDT);}
      static CommonTime UTC2BDT(const CommonTime& UTC){return gpstk::UTC2BDT(UTC);}

         /// A rather general way to transfrom time
      static CommonTime ConvertTimeSystem(const CommonTime& time,TimeSystemEnum from,TimeSystemEnum to)
      {return gpstk::ConvertTimeSystem(time,from,to);}


         //
         // Methods to handle reference system conversion-------------------------------
         // 
         //Ritrf = POM * Theta * N * P * Ricrf
         //
      
         /// ECI to ECEF transform matrix, POM * Theta * NP 
      static Matrix<double> J2kToECEFMatrix(const CommonTime& UTC,const EOPDataStore::EOPData& ERP)
         throw(Exception) { return gpstk::J2kToECEFMatrix(UTC,ERP); }

         /// Convert position from J2000 to ECEF.
      static Vector<double> J2kPosToECEF(const CommonTime& UTC, const Vector<double>& j2kPos)
         throw(Exception) {return gpstk::J2kPosToECEF(UTC,j2kPos);}

         /// Convert position from ECEF to J2000.
      static Vector<double> ECEFPosToJ2k(const CommonTime& UTC, const Vector<double>& ecefPos)
         throw(Exception){return gpstk::ECEFPosToJ2k(UTC,ecefPos);}

         /// Convert position and velocity from J2000 to ECEF.
      static Vector<double> J2kPosVelToECEF(const CommonTime& UTC, const Vector<double>& j2kPosVel)
         throw(Exception){return gpstk::J2kPosVelToECEF(UTC,j2kPosVel);}


         /// Convert position and velocity from ECEF to J2000.
      static Vector<double> ECEFPosVelToJ2k(const CommonTime& UTC, const Vector<double>& ecefPosVel)
         throw(Exception){return gpstk::ECEFPosVelToJ2k(UTC,ecefPosVel);}

      
      //
      // Methods to handle planet ephemeris -------------------------------
      // 
      // DE405
      //

         /// Load the JPL ephemeris from a binary file
      static void loadBinaryEphemeris(const std::string ephFile)
         throw(Exception);
      
         /** Compute planet position and velocity in J2000
          *  
          * @param TT         Time(Modified Julian Date in TT<TAI+32.184>) of interest 
          * @param entity     The planet to be computed
          * @param center     relative to whick the result apply
          * @return           The position and velocity of the planet in m and m/s
          */
      static Vector<double> planetJ2kPosVel( const CommonTime&      TT, 
                                             PlanetEphemeris::Planet entity,
                                             PlanetEphemeris::Planet center 
                                                      = PlanetEphemeris::Earth);

         /// Return J2000 position of sun in m
      static Vector<double> sunJ2kPosition(const CommonTime& TT);
      static Vector<double> sunJ2kPosition(const CommonTime& time,TimeSystemEnum sys)
      { return sunJ2kPosition(ConvertTimeSystem(time,sys,TS_TT));}

         /// Return J2000 position of moon in m
      static Vector<double> moonJ2kPosition(const CommonTime& TT);
      static Vector<double> moonJ2kPosition(const CommonTime& time,TimeSystemEnum sys)
      { return moonJ2kPosition(ConvertTimeSystem(time,sys,TS_TT));}

         /// Return ECEF position of sun in m
      static Vector<double> sunECEFPosition(const CommonTime& TT);
      static Vector<double> sunECEFPosition(const CommonTime& time,TimeSystemEnum sys)
      { return sunECEFPosition(ConvertTimeSystem(time,sys,TS_TT));}

         /// Return ECEF position of moon in m
      static Vector<double> moonECEFPosition(const CommonTime& TT);
      static Vector<double> moonECEFPosition(const CommonTime& time,TimeSystemEnum sys)
      { return moonECEFPosition(ConvertTimeSystem(time,sys,TS_TT));}

   protected:
      IERS() {}
      ~IERS() {}
      
      static PlanetEphemeris jplEphemeris;

   }; // End of class 'IERS'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_IERS_HPP
