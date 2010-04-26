#pragma ident "$Id: $"

/**
 * @file IERS.cpp
 * This class ease handling IERS earth orientation data.
 */

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
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

#include "IERS.hpp"
#include "icd_200_constants.hpp"
#include <string>
#include <fstream>

namespace gpstk
{
      // Arcseconds to radius convention 
   const double IERS::ARCSEC2RAD = PI / 3600.0 / 180.0;
   
      // Object to hold EOP data
   EOPStore IERS::eopStore;
   

      // UT1-UTC time difference [s]
   double IERS::UT1mUTC(double mjdUTC)
      throw (InvalidRequest)
   {
      DayTime utc(mjdUTC);
      EarthOrientation eop = eopStore.getEOP(utc);
      return eop.UT1mUTC;
   }


      // Pole coordinate x [arcseconds]
   double IERS::xPole(double mjdUTC)
      throw (InvalidRequest)
   {
      DayTime utc(mjdUTC);
      EarthOrientation eop = eopStore.getEOP(utc);
      return eop.xp;
   }


      // Pole coordinate y [arcseconds]
   double IERS::yPole(double mjdUTC)
      throw (InvalidRequest)
   {
      DayTime utc(mjdUTC);
      EarthOrientation eop = eopStore.getEOP(utc);
      return eop.yp;
   }


      /* Add EOPs to the store via a flat IERS file. 
       *  get finals.data from http://maia.usno.navy.mil/
       *
       *  @param iersFile  Name of file to read, including path.
       */
   void IERS::loadIERSFile(std::string iersFile)
      throw(FileMissingException)
   {
      eopStore.clear();
      eopStore.addIERSFile(iersFile);

   } 
   

      /* Add EOPs to the store via a flat STK file. 
       *  EOP-v1.1.txt
       *  http://celestrak.com/SpaceData/EOP-format.asp
       *
       *  @param stkFile  Name of file to read, including path.
       */
   void IERS::loadSTKFile(std::string stkFile)
      throw(FileMissingException)
   {
      
      eopStore.clear();

      std::ifstream fstk(stkFile.c_str());
      
      
      int  numData = 0;
      bool bData = false;

      std::string buf;
      while(getline(fstk,buf))
      {   
        
         if(buf.substr(0,19) == "NUM_OBSERVED_POINTS")
         {
            numData = StringUtils::asInt(buf.substr(20));
            continue;
         }
         else if(buf.substr(0,14) == "BEGIN OBSERVED")
         {
            bData = true;
            continue;
         }
         else if(buf.substr(0,13) == "END PREDICTED")
         {
            bData = false;
            break;
         }
         if(!StringUtils::isDigitString(buf.substr(0,4)))
         {
            // for observed data and predicted data
            continue;
         }

         if(bData)
         {
            EarthOrientation eop;
            
            // # FORMAT(I4,I3,I3,I6,2F10.6,2F11.7,4F10.6,I4)
            int year = StringUtils::asInt(buf.substr(0,4));
            int month = StringUtils::asInt(buf.substr(4,3));
            int day = StringUtils::asInt(buf.substr(7,3));
            int mjd = StringUtils::asInt(buf.substr(10,6));

            eop.xp = StringUtils::asDouble(buf.substr(16,10));
            eop.yp = StringUtils::asDouble(buf.substr(26,10));
            eop.UT1mUTC = StringUtils::asDouble(buf.substr(36,11));
            
            eopStore.addEOP(mjd, eop);
         }
         
      }  // End of 'while'
      
      fstk.close();

   }  // End of method 'IERS::loadSTKFile'


   DayTime IERS::GPST2UTC(DayTime gpst)
   {
         // the input should be UTC
      int leapSec = TAImUTC(gpst.MJD());   
      DayTime utc = gpst;
      utc += (19.0 - double(leapSec));

      leapSec = TAImUTC(utc.MJD());
      utc = gpst;
      utc += (19.0 - double(leapSec));

      return utc;
   }


      /* Return the difference between TAI and UTC (known as leap seconds).
       * Values from the USNO website: ftp://maia.usno.navy.mil/ser7/leapsec.dat
       * As of July 19, 2002, no leap second in Dec 2002 so next opportunity for
       * adding a leap second is July 2003. Check IERS Bulletin C.
       * @param mjd Modified Julian Date
       * @return number of leaps seconds.
       */
   int IERS::TAImUTC(const double& mjdUTC)
   {
      const double mjd(mjdUTC);
      
      // ATTENTION:
      // the data should be updated when new leap seconds added.
      // check the sites:
      // http://hpiers.obspm.fr/eoppc/bul/bulc/UTC-TAI.history

      if(mjd < 41317.0)
      {
         InvalidRequest e("There are no leap second data on this epoch");
         GPSTK_THROW(e);
      }

      if ((mjd >=41317.0)&&(mjd < 41499.0)) return 10;
      if ((mjd >=41499.0)&&(mjd < 41683.0)) return 11;
      if ((mjd >=41683.0)&&(mjd < 42048.0)) return 12;
      if ((mjd >=42048.0)&&(mjd < 42413.0)) return 13;
      if ((mjd >=42413.0)&&(mjd < 42778.0)) return 14;
      if ((mjd >=42778.0)&&(mjd < 43144.0)) return 15;
      if ((mjd >=43144.0)&&(mjd < 43509.0)) return 16;
      if ((mjd >=43509.0)&&(mjd < 43874.0)) return 17;
      if ((mjd >=43874.0)&&(mjd < 44239.0)) return 18;
      if ((mjd >=44239.0)&&(mjd < 44786.0)) return 19;
      if ((mjd >=44786.0)&&(mjd < 45151.0)) return 20;
      if ((mjd >=45151.0)&&(mjd < 45516.0)) return 21;
      if ((mjd >=45516.0)&&(mjd < 46247.0)) return 22;
      if ((mjd >=46247.0)&&(mjd < 47161.0)) return 23;
      if ((mjd >=47161.0)&&(mjd < 47892.0)) return 24;
      if ((mjd >=47892.0)&&(mjd < 48257.0)) return 25;
      if ((mjd >=48257.0)&&(mjd < 48804.0)) return 26;
      if ((mjd >=48804.0)&&(mjd < 49169.0)) return 27;
      if ((mjd >=49169.0)&&(mjd < 49534.0)) return 28;
      if ((mjd >=49534.0)&&(mjd < 50083.0)) return 29;
      if ((mjd >=50083.0)&&(mjd < 50630.0)) return 30;
      if ((mjd >=50630.0)&&(mjd < 51179.0)) return 31;
      if ((mjd >=51179.0)&&(mjd < 53736.0)) return 32;
      if ((mjd >=53736.0)&&(mjd < 54832.0)) return 33;
      if  (mjd >=54832.0) return 34;                        // 2009-01-01

      // It should never go here and return 0 to avoid the warning
      return 0;

   } // IERS::TAImUTC()



   void IERS::test()
   {
      IERS::loadIERSFile("InputData\\IERS\\finals.data");

      double mjd = 48622.00;
      double ut1mutc = UT1mUTC(mjd);
      double xp = xPole(mjd);
      double yp = yPole(mjd);
      double taimutc = TAImUTC(mjd);

      std::cout<<ut1mutc<<std::endl
         <<xp<<std::endl
         <<yp<<std::endl
         <<taimutc<<std::endl;
   }

}  // End of namespace 'gpstk'
