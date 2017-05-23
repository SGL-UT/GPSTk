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

/// @file EOPStore.cpp
/// class gpstk::EOPStore encapsulates input, storage and retreval of
/// Earth Orientation Parameters (EOPs - cf. class EarthOrientation).

//------------------------------------------------------------------------------------
#include "EOPStore.hpp"
//#include "logstream.hpp"

//------------------------------------------------------------------------------------
using namespace std;

namespace gpstk
{
   // Add to the store directly
   void EOPStore::addEOP(int mjd, EarthOrientation& eop)
      throw()
   {
      mapMJD_EOP[mjd] = eop;

      if(begMJD == -1 || endMJD == -1) begMJD = endMJD = mjd;
      else if(mjd < begMJD)            begMJD = mjd;
      else if(mjd > endMJD)            endMJD = mjd;
   }

   //---------------------------------------------------------------------------------
   // Add to the store by computing using an EOPPrediction file.
   // @param MJD integer MJD(UTC) at which to add EOPs
   // @return non-0 if MJD is outside range
   int EOPStore::addEOP(int mjd, EOPPrediction& eopp) throw(Exception)
   {
      EarthOrientation eo;
      try {
         eo = eopp.computeEOP(mjd);
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }

      addEOP(mjd,eo);

      return 0;
   }

   //---------------------------------------------------------------------------------
   // Add EOPs to the store via an inpu file: either an EOPP file
   // or a flat file produced by USNO (see http://maia.usno.navy.mil/
   // and get either file 'finals.data' or finals2000A.data').
   // @param filename Name of file to read, including path.
   // @throw if the file is not found
   void EOPStore::addFile(const string& filename)
      throw(FileMissingException)
   {
      try {
         addEOPPFile(filename);
      }
      catch(FileMissingException& fme)
      {
         if(StringUtils::matches(fme.getText(),string("wrong format")).empty()) {
            GPSTK_RETHROW(fme);
         }

         // try other format
         try {
            addIERSFile(filename);
         }
         catch(FileMissingException& fme)
         {
            GPSTK_RETHROW(fme);
         }
      }
   }

   //---------------------------------------------------------------------------------
   // Add EOPs to the store via an EOPP file: read the EOPPrediction from the file
   // and then compute EOPs for all days within the valid range.
   // @param filename Name of file to read, including path.
   // @throw if the file is not found
   void EOPStore::addEOPPFile(const string& filename)
      throw(FileMissingException)
   {
      // read the file into an EOPPrediction
      EOPPrediction eopp;
      try {
         eopp.loadFile(filename);
      }
      catch(FileMissingException& fme)
      {
         GPSTK_RETHROW(fme);
      }

      // pull out the beginning of the valid time range
      int mjd;
      mjd = eopp.getValidTime();
      // add all 7 days
      for(int i=0; i<7; i++) {
         EarthOrientation eo;
         eo = eopp.computeEOP(mjd);
         addEOP(mjd,eo);
         mjd++;
      }
   }

   //---------------------------------------------------------------------------------
   // see http://maia.usno.navy.mil/readme.finals
   void EOPStore::addIERSFile(const string& filename)
      throw(FileMissingException)
   {
      bool ok;
      int n,mjd;
      double fracmjd;
      string line,word;

      ifstream inpf(filename.c_str());
      if(!inpf) {
         FileMissingException fme("Could not open IERS file " + filename);
         GPSTK_THROW(fme);
      }

      ok = true;
      while(!inpf.eof() && inpf.good()) {
         getline(inpf,line);
         StringUtils::stripTrailing(line,'\r');
         if(inpf.eof()) break;
            // line length is actually 187
         if(inpf.bad() || line.size() < 70) { ok = false; break; }
         EarthOrientation eo;
         mjd = StringUtils::asInt(line.substr(7,5));
         // Bulletin A
         eo.xp = StringUtils::asDouble(line.substr(18,9));       // arcseconds
         eo.yp = StringUtils::asDouble(line.substr(37,9));       // arcseconds
         eo.UT1mUTC = StringUtils::asDouble(line.substr(58,10)); // seconds
         // Bulletin B
         //eo.xp = StringUtils::asDouble(line.substr(134,10));       // arcseconds
         //eo.yp = StringUtils::asDouble(line.substr(144,10));       // arcseconds
         //eo.UT1mUTC = StringUtils::asDouble(line.substr(154,11));  // seconds

         addEOP(mjd,eo);
      };
      inpf.close();

      if(!ok) {
         FileMissingException fme("IERS File " + filename
                                   + " is corrupted or wrong format");
         GPSTK_THROW(fme);
      }
   }

   //---------------------------------------------------------------------------------
   // Edit the store by deleting all entries before(after) the given min(max) MJDs.
   // If mjdmin is later than mjdmax, the two times are switched.
   //  @param mjdmin integer MJD desired earliest store time.
   //  @param mjdmax integer MJD desired latest store time.
   void EOPStore::edit(int mjdmin, int mjdmax)
      throw()
   {
      if(mjdmin > mjdmax) {
         int m=mjdmin;
         mjdmin = mjdmax;
         mjdmax = m;
      }

      if(mjdmin > endMJD) return;
      if(mjdmax < begMJD) return;

      map<int,EarthOrientation>::iterator it;
      it = mapMJD_EOP.lower_bound(mjdmin);
      if(it != mapMJD_EOP.begin())
         mapMJD_EOP.erase(mapMJD_EOP.begin(), it);

      it = mapMJD_EOP.upper_bound(mjdmax);
      if(it != mapMJD_EOP.end())
         mapMJD_EOP.erase(it, mapMJD_EOP.end());

      it = mapMJD_EOP.begin();
      if(it == mapMJD_EOP.end())
         begMJD = -1;
      else 
         begMJD = it->first;

      it = mapMJD_EOP.end();
      if(--it == mapMJD_EOP.end())
         endMJD = -1;
      else 
         endMJD = it->first;
   }

   //---------------------------------------------------------------------------------
   // Dump the store to cout.
   // @param detail determines how much detail to include in the output
   //   0 start and stop times (MJD), and number of EOPs.
   //   1 list of all times and EOPs.
   void EOPStore::dump(short detail, ostream& os) const
      throw()
   {
      os << "EOPStore dump (" << mapMJD_EOP.size() << " entries):\n";
      os << " Time limits: [MJD " << begMJD << " - " << endMJD << "]";

      int yy,mm,dd;
      convertJDtoCalendar(static_cast<long>(begMJD+MJD_TO_JD),yy,mm,dd);
      os << " = [m/d/y " << mm << "/" << dd << "/" << yy;
      convertJDtoCalendar(static_cast<long>(endMJD+MJD_TO_JD),yy,mm,dd);
      os << " - " << mm << "/" << dd << "/" << yy << "]" << endl;

      if(detail > 0) {
         os << "   MJD      xp         yp        UT1-UTC  IERS\n";
         int lastmjd=-1;
         map<int,EarthOrientation>::const_iterator it;
         for(it=mapMJD_EOP.begin(); it != mapMJD_EOP.end(); it++) {
            if(lastmjd != -1 && it->first - lastmjd > 1)
               os << " ....." << endl;
            os << " " << it->first << " " << it->second
               << "     (" << setfill('0') << setw(3)
               << EOPPrediction::getSerialNumber(it->first) << setfill(' ') << ")"
               << endl;
            lastmjd = it->first;
         }
      }
   }

   //---------------------------------------------------------------------------------
   // Get the EOP at the given epoch. This involves interpolation and corrections
   // as prescribed by the appropriate IERS convention, using code in class
   // EarthOrientation. This routine pulls data from the map for 4 entries
   // surrounding the input time; this array of data is passed to class
   // EarthOrientation to perform the interpolation and corrections.
   // @param mjd MJD(UTC) time of interest
   // @param conv IERSConvention to be used.
   // @throw InvalidRequest if the integer MJD falls outside the store,
   //   or if the store contains fewer than 4 entries
   // @return EarthOrientation EOPs at mjd.
   EarthOrientation EOPStore::getEOP(const double& mjd, const IERSConvention& conv)
      throw(InvalidRequest)
   {
      if(mapMJD_EOP.size() < 4) {
         InvalidRequest ir("Store is too small for interpolation");
         GPSTK_THROW(ir);
      }

      // Stored data uses UTC times
      //if(t.getTimeSystem() == TimeSystem::Unknown) {
      //   InvalidRequest ir("Time system is unknown");
      //   GPSTK_THROW(ir);
      //}

      // get MJD(UTC)
      double mjdUTC(mjd);

      // find 4 points surrounding the time of interest ----------------
      map<int,EarthOrientation>::iterator lowit,hiit,it;
      it = lowit = mapMJD_EOP.find(int(mjdUTC));
      (hiit = it)++;
      if(lowit == mapMJD_EOP.end() || hiit == mapMJD_EOP.end()) {
         InvalidRequest ir("Requested time lies outside the store");
         GPSTK_THROW(ir);
      }
      if(mapMJD_EOP.size() < 4) {
         InvalidRequest ir("Store contains less than 4 entries");
         GPSTK_THROW(ir);
      }

      // low and hi must span 4 entries and bracket t
      (it = lowit)--;
      if(it == mapMJD_EOP.end()) {
         hiit++;
         hiit++;                          // L t . . H
      }
      else {
         lowit = it;
         (it = hiit)++;
         if(it == mapMJD_EOP.end())
            lowit--;                      // L . . t H
         else
            hiit = it;                    // L . t . H
      }

      // fill arrays for Lagrange interpolation -----------------------
      //LOG(INFO) << " LAGINT at " << fixed << setprecision(9) << mjdUTC << "(UTC)";
      vector<double> vtime,vX,vY,vdT;
      for(it = lowit; it != mapMJD_EOP.end(); ++it) {
         vtime.push_back(double(it->first));
         vX.push_back(it->second.xp);
         vY.push_back(it->second.yp);
         //LOG(INFO) << " xy " << fixed << setprecision(10) << double(it->first)
         //   << " " << it->second.xp << " " << it->second.yp;
         vdT.push_back(it->second.UT1mUTC);
         if(it == hiit) break;
      }

      // let EarthOrientation do the interpolation and correction -----
      EarthOrientation eo;
      EphTime ttag;
      ttag.setMJD(mjdUTC);
      ttag.setTimeSystem(TimeSystem::UTC);
      eo.interpolateEOP(ttag, vtime, vX, vY, vdT, conv);

      return eo;
   }

} // end namespace gpstk
