#pragma ident "$Id$"


/** @file Performs a data availability analysis of the input data. In general,
    availability is determined by station and satellite position.
*/

//lgpl-license START
//lgpl-license END

#ifndef DATAAVAILABILITYANALYZER_HPP
#define DATAAVAILABILITYANALYZER_HPP

#include <fstream>
#include <list>

#include "BasicFramework.hpp"
#include "CommandOptionWithTimeArg.hpp"
#include "GPSGeoid.hpp"
#include "MiscMath.hpp"
#include "ObsRngDev.hpp"

#include "DataReader.hpp"


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
   std::ifstream input;
   std::ofstream output;
   std::string timeFormat;
   gpstk::CommandOptionWithAnyArg inputOpt, outputOpt, independantOpt,
      mscFileOpt, msidOpt, timeFmtOpt, ephFileOpt, maskAngleOpt, timeMaskOpt;

   gpstk::CommandOptionNoArg badHealthMaskOpt, smashAdjacentOpt;

   gpstk::CommandOptionWithNumberArg timeSpanOpt;
   gpstk::CommandOptionWithTimeArg startTimeOpt, stopTimeOpt;

   gpstk::DayTime startTime, stopTime;
   double timeSpan, timeMask;
   double epochRate;

   enum ObsItemEnum {oiUnknown, oiElevation, oiAzimuth, oiTime, oiPRN, oiCCID,
                     oiSNR, oiHealth, oiTrackCount, oiIOD};

   typedef std::map<ObsItemEnum, std::string> ObsItemName;
   typedef std::map<std::string, ObsItemEnum> ObsItemId;

   ObsItemName obsItemName;
   ObsItemId obsItemId;
   ObsItemEnum oiX;    

   bool badHealthMask, smashAdjacent;

   gpstk::DataReader ephData, obsData;

   float maskAngle;

   // This is used to keep track of SV info
   struct InView
   {
      InView():up(false),aboveMask(false),smashCount(0){};

      void update(
         short prn,
         const gpstk::DayTime& time,
         const gpstk::ECEF& rxpos,
         const gpstk::EphemerisStore& eph,
         gpstk::GeoidModel& gm,
         float maskAngle);

      short prn;
      gpstk::DayTime time;

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
      short iodc, health;

      // This following items are only useful when this class is used to record
      // information associated with missing data.

      // Set only when the smash function merges this record with others.
      // Indicates the number of records merged.
      unsigned smashCount;

      // The number of SVs in track at this point in time.
      short inTrack;

      // The SNR of the CA signal. Note that this will be the SNR of the 
      // most recently received observation when an outage is detected.
      float snr;

      // A function object to allow printing of a list of these with a 
      // for_each loop
      class dump
      {
      public:
         dump(std::ostream& s, const std::string fmt)
            : stream(s), timeFormat(fmt) {};
         std::ostream& stream;
         std::string timeFormat;
         bool operator()(const InView& iv);
      };
   };

   typedef std::list<InView> MissingList;
   MissingList missingList;

   // This combines adjecent items from the same SV
   MissingList smash(const MissingList& ml) const;
};
#endif
