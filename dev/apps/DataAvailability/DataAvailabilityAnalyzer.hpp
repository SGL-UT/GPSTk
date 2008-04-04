#pragma ident "$Id$"

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

/** @file Performs a data availability analysis of the input data. In general,
    availability is determined by station and satellite position.
*/

#ifndef DATAAVAILABILITYANALYZER_HPP
#define DATAAVAILABILITYANALYZER_HPP

#include <fstream>
#include <list>
#include <set>

#include "BasicFramework.hpp"
#include "CommandOptionWithTimeArg.hpp"
#include "GPSGeoid.hpp"
#include "MiscMath.hpp"
#include "ObsRngDev.hpp"
#include "SatID.hpp"
#include "XvtStore.hpp"

typedef gpstk::XvtStore<gpstk::SatID> EphemerisStore;
typedef std::set<gpstk::ObsID> ObsSet;

class DataAvailabilityAnalyzer : public gpstk::BasicFramework
{
public:
   DataAvailabilityAnalyzer(const std::string& applName) throw();
   bool initialize(int argc, char *argv[]) throw();
   
protected:
   virtual void spinUp();
   virtual void process();
   virtual void shutDown();

private:
   void processEpoch(
      const gpstk::Triple& antPos,
      const gpstk::ObsEpoch& oe,
      const gpstk::ObsEpoch& prev_oe);
      
   void outputSummary();
   
   std::ifstream input;
   std::ofstream output;
   std::string timeFormat;
   gpstk::CommandOptionWithAnyArg inputOpt, outputOpt, independantOpt,
      mscFileOpt, msidOpt, timeFmtOpt, ephFileOpt, maskAngleOpt, trackAngleOpt, timeMaskOpt,
      rxPosOpt, timeSpanOpt, ignorePrnOpt;

   gpstk::CommandOptionNoArg badHealthMaskOpt, smashAdjacentOpt;

   gpstk::CommandOptionWithTimeArg startTimeOpt, stopTimeOpt;

   gpstk::DayTime startTime, stopTime;
   double timeSpan, timeMask;
   double epochRate;
   gpstk::DayTime firstEpochTime, lastEpochTime;
   
   // these are counters used in the summary
   unsigned long epochCounter;
   unsigned long allMissingCounter;
   unsigned long anyMissingCounter;

   enum ObsItemEnum {oiUnknown, oiElevation, oiAzimuth, oiTime, oiPRN, oiCCID,
                     oiSNR, oiHealth, oiTrackCount};

   typedef std::map<ObsItemEnum, std::string> ObsItemName;
   typedef std::map<std::string, ObsItemEnum> ObsItemId;
   typedef std::map<gpstk::DayTime, int> SVsInView;

   ObsItemName obsItemName;
   ObsItemId obsItemId;
   ObsItemEnum oiX;    

   bool badHealthMask, smashAdjacent;

   EphemerisStore* eph;
   gpstk::GPSGeoid gm;
   gpstk::Triple antennaPos;
   bool haveAntennaPos;
   long msid;

   float maskAngle, trackAngle;

   std::set<int> ignorePrn;

public:
   // This is used to keep track of SV info for both what SVs are in view
   // and when there is an obs that is missing. 
   struct InView
   {
      InView() : up(false), aboveMask(false), smashCount(0), span(0){};

      void update(
         short prn,
         const gpstk::DayTime& time,
         const gpstk::ECEF& rxpos,
         const EphemerisStore& eph,
         gpstk::GeoidModel& gm,
         float maskAngle, 
         float trackAngle);

      short prn;
      gpstk::DayTime time;

      // This is a list of all obs that are changed in the current epoch. Lost
      // is defined as present in the previous epoch but not present in the 
      // current epoch. Gained is those obs that are new to this epoch. 
      // Obviously, these are only used when there are some observations 
      // available for an SV
      ObsSet obsLost;
      ObsSet obsGained;
      
      // Set true when this SV has an elevation greater than 0
      // If this is false, no other fields are valid.
      bool up;
      
      // true when the sv is rising
      bool rising;

      // First epoch when this SV had an elevation greater than 0 
      gpstk::DayTime firstEpoch;

      // Set true when this SV has risen above the 'mask angle'
      // It is not cleared when the SV goes back below the mask angle.
      bool aboveMask;

      // First epoch when this SV had an elevation greater than the
      // 'mask angle'. Not valid unles aboveMask is true.
      gpstk::DayTime firstEpochAboveMask;

      // Number of epochs received from this SV during this pass
      unsigned epochCount;
      float elevation,azimuth;
      short health;

      // This following items are only useful when this class is used to record
      // information associated with missing data.

      // Set only when the smash function merges this record with others.
      // Indicates the number of records merged.
      unsigned smashCount;
      double span;

      // The number of SVs in track at this point in time.
      short inTrack;

      // The number of SVs physically above the mask angle at this time
      short numAboveMaskAngle, numAboveTrackAngle;

      // The SNR of the CA signal. Note that this will be the SNR of the 
      // most recently received observation when an outage is detected.
      float snr;

      // A function object to allow printing of a list of these with a 
      // for_each loop
      class dumper
      {
      public:
         dumper(std::ostream& s, const std::string fmt)
            : stream(s), timeFormat(fmt) {};
         std::ostream& stream;
         std::string timeFormat;
         bool operator()(const InView& iv)
         { iv.dump(stream, timeFormat); return true; }
      };

      void dump(std::ostream& s, const std::string fmt="%Y %j %02H:%02M:%04.1f")
         const;
   };

   typedef std::list<InView> MissingList;
   MissingList missingList;

   // This combines adjecent items from the same SV
   MissingList processList(const MissingList& ml, 
                           const EphemerisStore& eph);
   
   std::map<int, InView> inView;                         

};


void dump(std::ostream& s, const ObsSet& obs, int detail=0);


inline std::ostream& operator<<(std::ostream& s,
                         const DataAvailabilityAnalyzer::InView& iv)
{ iv.dump(s); return s; }


inline std::ostream& operator<<(std::ostream& s, const ObsSet& obs)
{ dump(s, obs); return s; }


#endif
