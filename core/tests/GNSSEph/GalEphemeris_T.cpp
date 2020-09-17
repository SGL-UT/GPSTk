//==============================================================================
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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================
/*
*   The Galileo health status is determined from several pieces of data in the
*   message.  This is defined the Galileo Service Definition Document (SDD) in
*   section 2.3.1.4.  In order to test the various combinations, a 
*   RINEX 3 nav file was hand edited to cover the various combinations.  
*   A different SV was assigned to each combination in order to simplify the
*   test process.
*
*   Note that the full complement of possible combinations do not need to be 
*   tested.   SHS is the highest priority in the decision tree.   Unless
*   SHS==0, the other parameters do not matter.   Similarly, DVS is higher
*   prioity than SISA.   
*
*   The RINEX "health" word is actually bit-encoded and contains 
*   the DVS (1 bit) and the SHS (three bits).  This is described in the
*   RINEX 3.04 specification in Table A8.  The exact location of 
*   the three bits varies by signal type.  As a result, the reconstruted
*   "health" word is provided to aid confirmation.
*
*        Bit    8 7 6 5 4 3 2 1 0
*   Quantity    S S D S S D S S D
*               ----- ----- -----  
*     Signal      E5b   E5a   E1B
*
*                     Data Source                     "Health" word    Expected 
*   PRN ID  Signal    dec.   hex.  SISA   DVS    SHS     hex.   dec.     Result     Notes
*   ------  ------    -----------  ----   ---    ---   -------------   --------   -----
*      E01     E5b    516, 0x0204  !=255    0      0     0x000     0   Healthy    As broadcast
*      E02                         !=255    0      1     0x080   128   Unhealthy  SHS = Out of service
*      E03                         !=255    0      2     0x100   256   Marginal   SHS = Will be out of service
*      E04                         !=255    0      3     0x180   384   Unhealthy  SHS = Under test
*      E05                         !=255    1      0     0x040    64   Marginal   DVS = Working without guarantee
*      E06                            -1    0      0     0x000     0   Marginal   SISA = no accuracy predication available
*      E07     E1B    513, 0x0201  !=255    0      0     0x000     0   Healthy
*      E08            513, 0x0201  !=255    0      3     0x006     6   Unhealthy  
*      E09            513, 0x0201     -1    0      0     0x000     0   Marginal
*      E10     E5a    258, 0x0102  !=255    0      0     0x000     0   Healthy    High order bit in data source changes due to E5a   
*      E11            258, 0x0102  !=255    0      3     0x030    48   Unhealthy  
*      E12            258, 0x0102     -1    0      0     0x000     0   Marginal
*      E14   E1B+E5b  517, 0x0205  !=255    0      3     0x186   390   Unhealthy   As broadcast. Both E1B and E5b
*/
#include "OrbitEphStore.hpp"
#include "CivilTime.hpp"
#include "Exception.hpp"
#include "TimeString.hpp"
#include "TestUtil.hpp"

#include "OrbitEphStore.hpp"
#include "GalEphemeris.hpp"
#include "SatelliteSystem.hpp"
#include "Xvt.hpp"

// Rinex3 Nav Loading
#include "Rinex3NavStream.hpp"
#include "Rinex3NavHeader.hpp"
#include "Rinex3NavData.hpp"

using namespace std;

class GalEphemeris_T
{
public:
   
  gpstk::OrbitEphStore store; 

      /** This loads the test file into an OrbitEphStore. 
        * OrbitEphStore is tested elsewhere
        */
   unsigned loadRinexNavData()
   {
      TUDEF("GalEphemeris_T","Load Rinex Nav Data");

      std::string dataFilePath = gpstk::getPathData();
      std::string file_sep = "/";
      string fn = dataFilePath + file_sep + "test_input_rinex3_nav_gal.20n"; 
       
      gpstk::Rinex3NavStream strm;
      strm.open(fn.c_str(),ios::in);
      if (!strm.is_open())
      {
          stringstream ss;
          ss << "Failed to open " << fn << " for input.";
          TUFAIL(ss.str());
          TURETURN();
      }

      try
      {
         gpstk::Rinex3NavHeader Rhead;
         strm >> Rhead;
      }
      catch (gpstk::Exception e)
      {
         stringstream ss;
         ss << "Failed to read header from " << fn << ".";
         TUFAIL(ss.str());
         TURETURN();
      }

      gpstk::Rinex3NavData  Rdata;
      bool done = false;
      while (!done)
      {
         try
         {
            if (strm >> Rdata)
            {
               if (Rdata.satSys=="E")
               {
                  gpstk::GalEphemeris eph(Rdata);
                  store.addEphemeris(dynamic_cast<gpstk::OrbitEph*>(&eph));
               }
            }
            else
            {
               done = true;
            }
         }
         catch (gpstk::Exception e)
         {
            TUFAIL("Caught exception attempting to load test_input");
            done = true; 
         }
      }
      TURETURN();
   }

      /** This tests the known health status of selected 
        * SVs.
        */
   unsigned testHealthSettings()
   {
      TUDEF("GalEphemeris","Test health contents");

      const int prnId[] =
      {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14
      };
      const gpstk::Xvt::HealthStatus fullHealth[] =
      {
         gpstk::Xvt::Healthy,
         gpstk::Xvt::Unhealthy,
         gpstk::Xvt::Degraded,
         gpstk::Xvt::Unhealthy,
         gpstk::Xvt::Degraded,
         gpstk::Xvt::Degraded,
         gpstk::Xvt::Healthy,
         gpstk::Xvt::Unhealthy,
         gpstk::Xvt::Degraded,
         gpstk::Xvt::Healthy,
         gpstk::Xvt::Unhealthy,
         gpstk::Xvt::Degraded,
         gpstk::Xvt::Unhealthy
      };
      const gpstk::Xvt::HealthStatus binaryNealth[] =
      {
         gpstk::Xvt::Healthy,
         gpstk::Xvt::Unhealthy,
         gpstk::Xvt::Unhealthy,
         gpstk::Xvt::Unhealthy,
         gpstk::Xvt::Unhealthy,
         gpstk::Xvt::Unhealthy,
         gpstk::Xvt::Healthy,
         gpstk::Xvt::Unhealthy,
         gpstk::Xvt::Unhealthy,
         gpstk::Xvt::Healthy,
         gpstk::Xvt::Unhealthy,
         gpstk::Xvt::Unhealthy,
         gpstk::Xvt::Unhealthy
      };

      int CASE_COUNT = 13;

      gpstk::CommonTime ctTest = gpstk::CivilTime(2020, 5, 29, 0, 30, 0.0, gpstk::TimeSystem::GAL); 
      for (int i=0; i<CASE_COUNT; i++)
      {
         gpstk::SatID sid(prnId[i],gpstk::SatelliteSystem::Galileo); 
         gpstk::Xvt::HealthStatus health;
         TUCATCH(health = store.getSVHealth(sid, ctTest));
         TUASSERTE(gpstk::Xvt::HealthStatus,
                   binaryNealth[i], health);
         const gpstk::OrbitEph* eph = store.findOrbitEph(sid,ctTest);
         const gpstk::GalEphemeris* geph = dynamic_cast<const gpstk::GalEphemeris*>(eph);
         TUASSERTE(gpstk::Xvt::HealthStatus,
                   fullHealth[i], geph->health);
      }
      TURETURN();
   }
};

int main(int argc, char *argv[])
{
   unsigned total = 0;
   GalEphemeris_T testClass;
   total += testClass.loadRinexNavData();
   total += testClass.testHealthSettings();

   cout << "Total Failures for " << __FILE__ << ": " << total << endl;
   return total;
}
