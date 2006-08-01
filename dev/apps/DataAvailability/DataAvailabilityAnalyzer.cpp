#pragma ident "$Id$"

/** @file Performs a data availability analysis of the input data. In general,
    availability is determined by station and satellite position.

    This program refers to the items that can be specified as the (in)dependant
    variables in the analysis as ObservationIDs or oid. The complete list of
    these are:
    el      Elevation, degrees (0-90)
    az      Azimuth, degrees (0-360, 0=north)
    time    Time in mjd (or as specified)
    prn     SV ID (1-31)
    ccid    Three character Rinex 3.0 observation specification: tna
              t: observation type (C, L, D, S)
              n: carrier band (1, 2, 5)
              a: code tracked (C, P, W, Y, M, I, Q, ...)
    ch      Channel (1-99)
    snr     Signal to noise ratio (C/C0) in dB-Hz
    health  SV health bits
    tcnt    Continuous tracking count
    iod     Issue of data
    intrk   Number of SVs in track
    tama    Time above mask angle
*/

//lgpl-license START
//lgpl-license END

#include <map>

#include "StringUtils.hpp"

#include "DataAvailabilityAnalyzer.hpp"


using namespace std;
using namespace gpstk;


//------------------------------------------------------------------------------
// The constructor basically just sets up all the command line options
//------------------------------------------------------------------------------
DataAvailabilityAnalyzer::DataAvailabilityAnalyzer(const std::string& applName)
   throw()
   : timeFormat("%Y %j %02H:%02M:%04.1f"),
     BasicFramework(
        applName,
        "Performs a data availability analysis of the input data. In general,"
        "availability is determined by station and satellite position."),
     
     inputOpt('o', "obs", 
              "Where to get the data to analyze.", true),
     
     outputOpt('\0', "output",
               "Where to send the output. The default is stdout."),
     
     independantOpt('x', "indepndant",
                    "The independant variable in the analysis. The default is time."),
     
     ephFileOpt('e', "eph",  "Where to get the ephemeris data. Can be "
                   " rinex, fic, or sp3", true),
     
     mscFileOpt('c', "msc", "Station coordinate file"),
     
     msidOpt('m', "msid", "Station to process data for. Used to select "
                "a station position from the msc file."),
     
     timeFmtOpt('t', "time-format", "Daytime format specifier used for "
                   "times in the output. The default is \"" 
                   + timeFormat + "\"."),
     
     startTimeOpt('\0', "start-time", "%4Y/%03j/%02H:%02M:%05.2f",
                  "Ignore data before this time. (%4Y/%03j/%02H:%02M:%05.2f)"),
     
     stopTimeOpt('\0',  "stop-time", "%4Y/%03j/%02H:%02M:%05.2f",
                 "Ignore any data after this time"),
     
     timeSpanOpt('l', "time-span", "How much data to process, in seconds"),

     maskAngleOpt('a', "mask-angle",
                  "Ignore anomalies on SVs below this elevation. The default is 10 degrees."),
     
     timeMaskOpt('c', "time-mask",
                 "Ignore anomalies on SVs that haven't been above the mask angle for this number of seconds. The default is 0 seconds."),

     badHealthMaskOpt('b', "bad-health",
                      "Ignore anomalies associated with SVs that are marked unhealthy."),

     smashAdjacentOpt('s', "smash-adjacent",
                      "Don't adjacent lines from the same PRN."),

     maskAngle(10), badHealthMask(false), timeMask(0), smashAdjacent(false)
{
   // Set up a couple of helper arrays to map from enum <-> string
   obsItemName[oiUnknown] = "unk";
   obsItemName[oiElevation] = "el";
   obsItemName[oiAzimuth] = "az";
   obsItemName[oiTime] = "time";
   obsItemName[oiPRN] = "prn";
   obsItemName[oiCCID] = "ccid";
   obsItemName[oiSNR] = "snr";
   obsItemName[oiHealth] = "health";
   obsItemName[oiTrackCount] = "tcnt";
   obsItemName[oiIOD] = "iod";

   for (ObsItemName::const_iterator i=obsItemName.begin(); i!=obsItemName.end(); i++)
      obsItemId[i->second] = i->first;
}


//------------------------------------------------------------------------------
// Here the command line options parsed and used to configure the program
//------------------------------------------------------------------------------
bool DataAvailabilityAnalyzer::initialize(int argc, char *argv[]) throw()
{
   if (!BasicFramework::initialize(argc,argv)) return false;

   if (debugLevel)
      cout << "debugLevel: " << debugLevel << endl
           << "verboseLevel: " << verboseLevel << endl;

   if (outputOpt.getCount())
   {
      output.open(outputOpt.getValue()[0].c_str(), std::ios::out);
      if (debugLevel)
         cout << "Sending output to" 
              << outputOpt.getValue()[0]
              << endl;
   }
   else
   {
      if (debugLevel)
         cout << "Sending output to stdout" << endl;
      output.copyfmt(std::cout);
      output.clear(std::cout.rdstate());
      output.std::basic_ios<char>::rdbuf(std::cout.rdbuf());
   }

   if (timeFmtOpt.getCount())
      timeFormat = timeFmtOpt.getValue()[0];

   if (startTimeOpt.getCount())
      startTime = startTimeOpt.getTime()[0];
   else
      startTime = DayTime::BEGINNING_OF_TIME;

   if (stopTimeOpt.getCount())
      stopTime = stopTimeOpt.getTime()[0];
   else
      stopTime = DayTime::END_OF_TIME;

   if (timeSpanOpt.getCount())
      timeSpan = StringUtils::asDouble(timeSpanOpt.getValue()[0]);
   else
      timeSpan = 1e99;

   if (maskAngleOpt.getCount())
      maskAngle = StringUtils::asDouble(maskAngleOpt.getValue()[0]);

   if (timeMaskOpt.getCount())
      timeMask = StringUtils::asDouble(timeMaskOpt.getValue()[0]);

   if (badHealthMaskOpt.getCount())
      badHealthMask = true;

   if (smashAdjacentOpt.getCount())
      smashAdjacent = true;

   oiX = oiTime;
   if (independantOpt.getCount())
   {
      ObsItemId::const_iterator i;
      i = obsItemId.find(independantOpt.getValue()[0]);
      if (i == obsItemId.end())
      {
         cout << "Cound not find obs item. Valid items are:" << endl;
         for (i=obsItemId.begin(); i!=obsItemId.end(); i++)
            cout << i->first << " ";
         cout << endl;
         exit(-1);
      }
      else
      {
         oiX = i->second;
      }
   }

   if (verboseLevel)
   {
      cout << "Using " << obsItemName[oiX] << " as the independant variable." << endl;
      cout << "Using a mask angle of " << maskAngle << " degrees" << endl;
      if (badHealthMask)
         cout << "Ignore anomalies associated with SVs marked unhealthy." << endl;
      else
         cout << "Including anomalies associated with SVs marked unhealthy." << endl;
   }

   return true;
}



//------------------------------------------------------------------------------
// Load all the data to analyze.
//------------------------------------------------------------------------------
void DataAvailabilityAnalyzer::spinUp()
{      
   ephData.verbosity = verboseLevel;
   ephData.read(ephFileOpt);

   if (!ephData.haveEphData)
   {
      cout << "Didn't get any ephemeris data from the eph files. "
           << "Exiting." << endl;
      return;
   }

   obsData.verbosity = verboseLevel;
   obsData.read(inputOpt);
   if (obsData.haveEphData && verboseLevel)
      cout << "Got eph data from the obs files." << endl;
   if (obsData.haveObsData && verboseLevel)
      cout << "Got obs data from the obs files." << endl;

   if (msidOpt.getCount())
      obsData.msid = StringUtils::asUnsigned(msidOpt.getValue()[0]);

   if (msidOpt.getCount() && mscFileOpt.getCount())
      obsData.read_msc_file(mscFileOpt.getValue()[0]);

   if (RSS(obsData.roh.antennaPosition[0],
           obsData.roh.antennaPosition[1],
           obsData.roh.antennaPosition[2]) < 1)
      cout << "Warning! The antenna appears to be within one meter of the" << endl
           << "center of the geoid. Please go check it." << endl;

   // Time of the first & last epochs in the obs data
   const DayTime firstEpochTime = obsData.rem.begin()->first;
   const DayTime lastEpochTime  = obsData.rem.rbegin()->first;

   // Check these against the header values, if the header values are present
   // tbd...

   // Clean up the start/stop times
   startTime = firstEpochTime > startTime ? firstEpochTime : startTime;
   stopTime  = lastEpochTime  < stopTime  ? lastEpochTime  : stopTime;
   if (stopTime - startTime > timeSpan)
      stopTime = startTime + timeSpan;

   if (verboseLevel)
      cout << "Start time: " << startTime.printf(timeFormat) << endl
           << "Stop time:  " << stopTime.printf(timeFormat) << endl;
}


std::string secAsHMS(double seconds)
{
   std::ostringstream oss;
   oss << setfill('0');
   
   if (seconds<0)
      oss << "-";

   seconds = std::abs(seconds);
   long d=0,h=0,m=0,s=0;
   s = static_cast<long>(std::floor(seconds));
   seconds -= static_cast<double>(s);

   if (s > 86400) { d = s/86400; s %= 86400; }
   if (s > 3600)  { h = s/3600;  s %= 3600;  }
   if (s > 60)    { m = s/60;    s %= 60;    }

   if (d) oss << d << " d ";
   if (h) oss << setw(2) << h << ":";
   if (m) oss << setw(2) << m << ":";

   if (h || m)
      oss << setfill('0');
   else
      oss << setfill(' ');

   oss << setw(2) << s;

   if (seconds>=0.1)
      oss << "." << static_cast<int>(seconds*10);
   else
      oss << "  ";

   return oss.str();
}


void DataAvailabilityAnalyzer::InView::update(
   short prn,
   const DayTime& time,
   const ECEF& rxpos,
   const EphemerisStore& eph,
   GeoidModel& gm,
   float maskAngle)
{
   try
   {
      this->prn = prn;
      this->time = time;
      // We really don't care about the observed range deviation, the
      // ObsRngDev class is just a convient way to get the azimuth, 
      // elevation, health, iodc, 
      ObsRngDev ord(0, prn, time, rxpos, eph, gm);
      vfloat el=ord.getElevation();
      if (el.is_valid() && el > 0)
      {
         if (!up)
         {
            firstEpoch = time;
            up = true;
            aboveMask = false;
            epochCount = 0;
            snr = 0;
            inTrack = 0;
         }
         else
         {
            rising = el > elevation;
         }
         if (el > maskAngle && !aboveMask)
         {
            aboveMask = true;
            firstEpochAboveMask = time;
         }
         elevation = ord.getElevation();
         azimuth = ord.getAzimuth();
         iodc = ord.getIODC();
         health = ord.getHealth();
      }
      else
      {
         up = false;
         aboveMask = false;
      }
   }
   catch (EphemerisStore::NoEphemerisFound& e)
   {
   }
} // end of InView::update()


//------------------------------------------------------------------------------
// The main logic of the program.
//------------------------------------------------------------------------------
DataAvailabilityAnalyzer::MissingList DataAvailabilityAnalyzer::smash(
   const MissingList& ml) const
{
   MissingList sml;
   for (MissingList::const_iterator i = ml.begin(); i != ml.end(); i++)
   {
      if (i == ml.begin())
      {
         sml.push_back(*i);
         continue;
      }

      InView& prev = *sml.rbegin();
      const InView& curr = *i;
      if (curr.prn == prev.prn && smashAdjacent)
      {
         prev.smashCount++;
         prev.time = curr.time;
         prev.elevation = curr.elevation;
         prev.azimuth = curr.azimuth;
         prev.snr = curr.snr;
         prev.epochCount = curr.epochCount;
      }
      else
      {
         sml.push_back(*i);
      }
   }
   return sml;
}


//------------------------------------------------------------------------------
// The main logic of the program.
//------------------------------------------------------------------------------
void DataAvailabilityAnalyzer::process()
{      
   static GPSGeoid gm;
   EphemerisStore& eph = *ephData.eph;
   ECEF rxpos(obsData.roh.antennaPosition);
   map<int, InView> inView;

   // Compute an estimate of the data rate.
   epochRate=-1;
   unsigned dtCount=0;
   DayTime t0; // time of the previous epoch
   DataReader::RODEpochMap::const_iterator i;
   for (i=obsData.rem.begin(); i!=obsData.rem.end(); i++)
   {
      const DayTime& t = i->first;
      if (i != obsData.rem.begin())
      {
         if (epochRate<=0 || (std::abs(epochRate - (t-t0)) > 1e-4))
         {
            epochRate = t-t0;
            dtCount = 0;
         }
         else
         {
            dtCount++;
         }
      }
      if (dtCount>10)
         break;
      t0 = t;
   }

   if (verboseLevel)
      cout << "Data rate: " << epochRate << " sec/epoch" << endl;
   
   for (DayTime t = startTime; t < stopTime; t += epochRate)
   {
      for (int prn=1; prn<=32; prn++)
         inView[prn].update(prn, t, rxpos, eph, gm, maskAngle);

      if (verboseLevel>2)
      {
         cout << t.printf(timeFormat) << "  SVs in view: ";
         for (int prn=1; prn<=32; prn++)
            if (inView[prn].up) 
               cout << prn << "(" << inView[prn].elevation << ") ";
         cout << endl;
      }

      i = obsData.rem.find(t);

      // This is when there are no obs for the entire epoch
      if (i == obsData.rem.end())
      {
         InView iv;
         iv.prn = 0;
         iv.time = t;
         missingList.push_back(iv);
         continue;
      }
      
      const gpstk::RinexObsData& rod = i->second;

      for (int prn=1; prn<=32; prn++)
      {
         gpstk::RinexObsData::RinexPrnMap::const_iterator rpi;
         rpi = rod.obs.find(gpstk::RinexPrn(prn, systemGPS));
         InView& iv=inView[prn];
         iv.inTrack = rod.obs.size();

         if (rpi == rod.obs.end())
         {
            if (rod.obs.size()<12 && iv.elevation>maskAngle && 
                (!iv.health || !badHealthMask))
            {
               missingList.push_back(iv);
            }
         }
         else
         {
            if (!iv.up)
            {
               missingList.push_back(iv);
            }
            else
            {
               iv.epochCount++;
               const gpstk::RinexObsData::RinexObsTypeMap& rotm = rpi->second;
               gpstk::RinexObsData::RinexObsTypeMap::const_iterator q;
               q = rotm.find(gpstk::RinexObsHeader::S1);     
               if (q != rotm.end())
                  iv.snr = q->second.data;
            }
         }
      }
   }
}


void DataAvailabilityAnalyzer::shutDown()
{
   MissingList sml = smash(missingList);

   for_each(sml.begin(), sml.end(), InView::dump(cout, timeFormat));
}


bool DataAvailabilityAnalyzer::InView::dump::operator()(const InView& iv)
{
   double timeUp     = iv.time - iv.firstEpoch;
   double timeUpMask = iv.time - iv.firstEpochAboveMask;
   char dir = iv.rising ? '+' : '-';

   string ccid="all";
   
   cout << left << iv.time.printf(timeFormat)
        << "+" << setw(4) << iv.smashCount
        << " prn:";

   if (iv.prn>0)
   {
      cout << setw(3) << iv.prn
           << " ccid:" << ccid
           << fixed
           << " el:" << setprecision(2) << setw(5) << iv.elevation
           << dir
           << " az:" << setprecision(0) << setw(3) << iv.azimuth
           << " hlth:" << hex << setw(2) << iv.health << dec;
   
      if (iv.up)
         cout << " snr:" << setprecision(1) << setw(4) << iv.snr
              << " tama:" << right << setw(11) <<  secAsHMS(timeUpMask);

   }
   else
      cout << "all";

   cout << endl;

   return true;
}
