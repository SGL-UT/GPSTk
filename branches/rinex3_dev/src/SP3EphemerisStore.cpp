#pragma ident "$Id$"

/**
 * @file SP3EphemerisStore.cpp
 * Read & store SP3 formated ephemeris data
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

#include <vector>

#include "SP3EphemerisStore.hpp"
#include "Triple.hpp"
#include "SatID.hpp"
#include "SP3Data.hpp"
#include "SP3Header.hpp"
#include "Xvt.hpp"
#include "CommonTime.hpp"
#include "CivilTime.hpp"
#include "ECEF.hpp"

using namespace gpstk::StringUtils;

namespace gpstk
{
   // Load the given SP3 file
   void SP3EphemerisStore::loadFile(const std::string& filename)
      throw( FileMissingException )
   {
      try
      {

         SP3Stream strm(filename.c_str());
         if (!strm)
         {
            FileMissingException e("File " +filename+ " could not be opened.");
            GPSTK_THROW(e);
         }

         SP3Header header;
         strm >> header;

         addFile(filename, header);

            // If any file doesn't have the velocity data, clear the
            // the flag indicating that there is any velocity data
         if (!header.containsVelocity)
         {
            haveVelocity = false;
         }

         SP3Data rec;
         while(strm >> rec)
         {
               // Ephemeris and clock are valid, then add them
            addEphemeris(rec);

         }  // end of 'while(strm >> rec)'

      }
      catch (gpstk::Exception& e)
      {
         GPSTK_RETHROW(e);
      }

   }  // End of method 'SP3EphemerisStore::loadFile()'


   /// Insert a new SP3Data object into the store
   void SP3EphemerisStore::addEphemeris(const SP3Data& data)
      throw()
   {  
     CommonTime t = data.time;
     SatID sat = data.sat;
     if(sat.id <= 0) return;
     Xvt&  xvt = pe[sat][t];

     if (data.RecType == 'P')
     {
       xvt.x = ECEF(data.x[0], data.x[1], data.x[2]);
       xvt.dtime = data.clk;
       haveVelocity = false;
     }
     else if (data.RecType == 'V')
     {
       xvt.v = Triple(data.x[0],data.x[1],data.x[2]);
       xvt.ddtime = data.clk;
       haveVelocity = true;
     }

     if (t < initialTime) initialTime = t;
     else if (t > finalTime) finalTime = t;
   };

   /* A debugging function that outputs in human readable form,
    * all data stored in this object: dump the store to cout.
    *
    * @param[in] s the stream to receive the output; defaults to cout
    * @param detail determines how much detail to include in the output
    *   0 list of filenames with their start, stop times.
    *   1 list of filenames with their start, stop times,
    *     other header information and prns/accuracy.
    *   2 above, plus dump all the PVT data (use judiciously).
    */

   void SP3EphemerisStore::dump( ostream& s,
                                 short detail )
      const throw()
   {
      // for ref: typedef std::map<CommonTime, Xvt> SvEphMap;
      // for ref: typedef std::map<SatID, SvEphMap> EphMap;

      s << "Dump of SP3EphemerisStore, built from file(s) :" << std::endl;
      std::vector<std::string> fileNames = getFileNames();
      std::vector<std::string>::const_iterator f=fileNames.begin();
      for (f = fileNames.begin(); f != fileNames.end(); f++)
         s << "  " << *f << std::endl;

/*
  Add this back in when/if we add header info to the file store.
      while(fmi != fm.end()) {
         s << " File " << fmi->first << ", Times: " << fmi->second.time
            << " to " << (fmi->second.time+fmi->second.epochInterval*fmi->second.numberOfEpochs)
            << ", (" << fmi->second.numberOfEpochs
            << "  " << fmi->second.epochInterval << "sec intervals)." << std::endl;
         if(detail > 0) {
            s << "  Data used as input : " << fmi->second.dataUsed
               << "  Coordinate system : " << fmi->second.coordSystem << std::endl;
            s << "  Orbit estimate type : " << fmi->second.orbitType
               << "  Agency : " << fmi->second.agency << std::endl;
            s << "  List of satellite PRN/acc (" << fmi->second.svList.size()
               << " total) :\n";
            int i=0;
            std::map<short,short>::const_iterator it=fmi->second.svList.begin();
            while(it != fmi->second.svList.end()) {
               s << "  " << std::setw(2) << it->first << "/" << it->second;
               if(!(++i % 8)) s << std::endl;
               it++;
            }
            if(++i % 8) s << std::endl;
            s << "  Comments:\n";
            for(i=0; i<fmi->second.comments.size(); i++)
               s << "    " << fmi->second.comments[i] << std::endl;
            s << std::endl;
         }
         fmi++;
      }
*/
      if (detail >= 0)
      {
         const char *fmt="%4Y/%02m/%02d %2H:%02M:%02S (%P)";
         EphMap::const_iterator it;

         s << " Data stored for " << pe.size() << " satellites, over time span "
           << static_cast<CivilTime>(initialTime).printf(fmt) << " to "
           << static_cast<CivilTime>(finalTime).printf(fmt) << "." << endl;

         if (detail == 0) return;

         s << " Gap interval = " << gapInterval
            << "; checking gaps is " << (checkDataGap ? "on" : "off") << endl;
         s << " Max interval = " << maxInterval
            << "; checking interval is " << (checkInterval ? "on" : "off") << endl;
         s << " Interpolation order is " << interpOrder << endl;

         if (detail == 1) return;

         for (it=pe.begin(); it!=pe.end(); it++)
         {
            s << "  Satellite map for sat " << SP3SatID(it->first) << " : "
              << it->second.size() << " records.";
            if (detail == 1) { s << endl; continue; }
            s << "  Data:" << endl;
            SvEphMap::const_iterator jt;

            for(jt=it->second.begin(); jt!=it->second.end(); jt++)
            {
               s << " " << static_cast<CivilTime>(jt->first).printf(fmt) << " P "
                  << fixed << setprecision(6)
                  << setw(13) << jt->second.x[0] << " "
                  << setw(13) << jt->second.x[1] << " "
                  << setw(13) << jt->second.x[2] << " "
                  << setw(13) << jt->second.dtime;
               if (haveVelocity) s << " V "
                  << setw(13) << jt->second.v[0] << " "
                  << setw(13) << jt->second.v[1] << " "
                  << setw(13) << jt->second.v[2] << " "
                  << setw(13) << jt->second.ddtime;
               s << endl;
            }
         }

      }  // End of 'if (detail >= 0)...'

      s << "End of Dump of SP3EphemerisStore." << endl;

   }  // End of method 'SP3EphemerisStore::dump()'

   // units are km and microsec
   void SP3EphemerisStore::addPositionData( const CommonTime& t,
                                            const SatID& sat,
                                            const double& x,
                                            const double& y,
                                            const double& z,
                                            const double& c      )
      throw()
   {
      if(sat.id <= 0) return;
      Xvt& xvt = pe[sat][t];  // find or create pair
      xvt.x = ECEF(x,y,z);
      xvt.dtime = c;

      // update time limits
      if (t<initialTime)
         initialTime = t;
      else if (t>finalTime)
         finalTime = t;
   }

   // units are decimeters/sec and 1.e-4 microsec/sec
   void SP3EphemerisStore::addVelocityData( const CommonTime& t,
                                            const SatID& sat,
                                            const double& vx,
                                            const double& vy,
                                            const double& vz,
                                            const double& vc     )
      throw()
   {
      if(sat.id <= 0) return;
      Xvt& xvt = pe[sat][t];  // find or create pair
      xvt.v = Triple(vx,vy,vz);
      xvt.ddtime = vc;

      // update time limits
      if (t<initialTime)
         initialTime = t;
      else if (t>finalTime)
         finalTime = t;
   }

   void SP3EphemerisStore::addData( const CommonTime& t,
                                    const SatID& sat,
                                    const Xvt& pv        )
      throw()
   {
      if(sat.id <= 0) return;
      Xvt& xvt = pe[sat][t];  // find or create pair
      xvt = pv;

      // update time limits
      if (t<initialTime)
         initialTime = t;
      else if (t>finalTime)
         finalTime = t;
   }


}  // End of namespace gpstk
