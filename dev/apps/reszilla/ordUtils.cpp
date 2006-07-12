#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/reszilla/ordUtils.cpp#1 $"

#include "util.hpp"

#include "ordUtils.hpp"

#include "LinearClockModel.cpp"

using namespace std;


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void compute_ords(ORDEpochMap& oem,
                  const RODEpochMap& rem,
                  const gpstk::RinexObsHeader& roh,
                  const gpstk::EphemerisStore& eph,
                  const gpstk::WxObsData& wod,
                  bool svTime,
                  const string& ordModeStr,
                  const std::string& clkModelStr)
{
   bool dualFreq=false;
   RinexObsType p1,p2;

   if      (ordModeStr=="p1p2" || ordModeStr=="c1p2") dualFreq=true;

   if      (ordModeStr=="p1p2") p1=P1, p2=P2;
   else if (ordModeStr=="c1p2") p1=C1, p2=P2;
   else if (ordModeStr=="c1")   p1=C1;
   else if (ordModeStr=="p1")   p1=P1;
   else if (ordModeStr=="p2")   p1=P2;
   else
   {
      cout << "Unknown ORD computation requested, mode=" << ordModeStr << endl;
      return;
   }

   if (verbosity)
      cout << "Computing observed range deviations." << endl;

   if (verbosity>1)
      if (svTime)
         cout << "Assuming data is tagged in SV time (time of emission)." << endl;
      else
         cout << "Assuming data is taged in Receiver time (gps time)." << endl;


   if (gpstk::RSS(roh.antennaPosition[0],
           roh.antennaPosition[1],
           roh.antennaPosition[2]) < 1)
   {
      cout << "Warning! The antenna appears to be within one meter of the" << endl
           << "center of the geoid. This program is not capable of" << endl
           << "accurately estimating the propigation of GNSS signals" << endl
           << "through solids such as a planetary crust or magma. Also," << endl
           << "if this location is correct, your antenna is probally" << endl
           << "no longer in the best of operating condition." << endl;
      return;
   }
   
   gpstk::ObsClockModel* cm;
   if (clkModelStr=="epoch")
   {
      if (verbosity)
         cout << "Using an epoch clock model." << endl;
      cm = new(gpstk::EpochClockModel);
   }
   else if (clkModelStr=="linear")
   {
      if (verbosity)
         cout << "Using a linear clock model." << endl;
      cm = new(gpstk::LinearClockModel);
   }
   else
   {
      cout << "Unknown clock model requestd, model=" << clkModelStr << endl;
      return;
   }

   if (verbosity>4)
      gpstk::ObsRngDev::debug = true;

   cm->setElevationMask(5);
   gpstk::GPSGeoid gm;
   gpstk::ECEF ap(roh.antennaPosition);
   gpstk::Geodetic geo(ap, &gm);
   gpstk::NBTropModel tm(geo.getAltitude(), geo.getLatitude(), roh.firstObs.DOYday());

   RODEpochMap::const_iterator i;
   for (i=rem.begin(); i!=rem.end(); i++)
   {
      const gpstk::DayTime& t = i->first;
      const gpstk::RinexObsData& rod = i->second;

      gpstk::ORDEpoch& oe = oem[t];
      oe.time = t;

      // Now set up our trop model for this epoch
      const gpstk::WxObservation wx = wod.getMostRecent(t);
      if (verbosity>3)
         cout << "wx: " << wx << endl;
      if (wx.isAllValid())
         tm.setWeather(wx.temperature, wx.pressure, wx.humidity);

      // Walk over all prns in this epoch
      gpstk::RinexObsData::RinexPrnMap::const_iterator rpi;
      for (rpi=rod.obs.begin(); rpi!=rod.obs.end(); rpi++)
      {
         short prn = rpi->first.prn;

         gpstk::RinexObsData::RinexObsTypeMap rotm = rpi->second;

         try
         {
            if (dualFreq)
            {
               gpstk::ObsRngDev ord(rotm[p1].data, rotm[p2].data,
                                    prn, t, ap, eph, gm, tm, svTime);
               oe.ords[prn] = ord;
            }
            else
            {
               gpstk::ObsRngDev ord(rotm[p1].data, 
                                    prn, t, ap, eph, gm, tm, svTime);
               oe.ords[prn] = ord;
            }
         }
         catch (gpstk::EphemerisStore::NoEphemerisFound& e)
         {
            if (verbosity>2)
               cout << e.getText() << endl;
         }
      }

      cm->addEpoch(oe);
      if (verbosity>3)
         cout << "clk: " << *cm << endl;
      oe.applyClockModel(*cm);
      if (verbosity>3)
         cout << "oe: " << oe;
   }

   delete cm;
} // end of compute_ords()


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void dumpStats(const ORDEpochMap& oem, 
               const string& ordMode, 
               const double sigmam)
{
   cout << endl
        << "ord        elev   stddev    mean    z   #obs  #del   max   strip" << endl
        << "---------- -----  -------  ----------  ------ ----  ------ ------" << endl;

   string desc = ordMode + " ord  ";

   for (ElevationRangeList::const_iterator i = elr.begin(); i != elr.end(); i++)
      computeStats(desc, oem, *i, sigmam);

   if (verbosity>1)
      cout << endl
           << "stddev, mean, max, and strip in meters" << endl
           << "z: 0 if mean < stddev/sqrt(n)" << endl;
}

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void computeStats(
   const string desc,
   const ORDEpochMap& oem,
   const ElevationRange er,
   const double sigmam)
{
   float minElevation = er.first;
   float maxElevation = er.second;

   gpstk::Stats<double> fp;
   ORDEpochMap::const_iterator ei;
   for (ei = oem.begin(); ei != oem.end(); ei++)
   {
      const gpstk::DayTime& t = ei->first;
      gpstk::ORDEpoch::ORDMap::const_iterator pi;
      for (pi = ei->second.ords.begin(); pi != ei->second.ords.end(); pi++)
      {
         const float el = pi->second.getElevation();
         const double ord = pi->second.getORD();
         if (el>minElevation && el<maxElevation && abs(ord) < 1e6)
            fp.Add(ord);
      }
   }

   double strip = sigmam * fp.StdDev();
   gpstk::Stats<double> good, bad;
   for (ei = oem.begin(); ei != oem.end(); ei++)
   {
      const gpstk::DayTime& t = ei->first;
      gpstk::ORDEpoch::ORDMap::const_iterator pi;
      for (pi = ei->second.ords.begin(); pi != ei->second.ords.end(); pi++)
      {
         const float el = pi->second.getElevation();
         const double ord = pi->second.getORD();
         if (el>minElevation && el<maxElevation)
         {
            if (abs(ord) < strip)
               good.Add(ord);
            else
               bad.Add(ord);
         }
      }
   }

   cout << left << setw(10) << desc << right
        << " "
        << setprecision(2) << setw(2) <<  minElevation
        << "-"
        << setw(2) << maxElevation
        << " ";

   char b1[200];
   char zero = good.Average() < good.StdDev()/sqrt((float)good.N())?'0':' ';
   double maxOrd = max(abs(good.Minimum()), abs(good.Maximum()));
   sprintf(b1, "%8.5f  %8.4f %c %7d %4d  %6.2f %6.2f",
           good.StdDev(), good.Average(), zero,
           good.N(), bad.N(), maxOrd, strip);
   
   cout << b1 << endl;
}

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void dump(std::ostream& s, const ORDEpochMap& oem)
{
   s.setf(ios::fixed, ios::floatfield);
   s << "# time              PRN type  elev      clk(m)"
     << "        ord(m)    iodc  health"
     << endl;

   ORDEpochMap::const_iterator ei;
   for (ei = oem.begin(); ei != oem.end(); ei++)
   {
      const gpstk::DayTime& t = ei->first;
      string time=t.printf(timeFormat);
      gpstk::ORDEpoch::ORDMap::const_iterator pi;
      const gpstk::ORDEpoch& e = ei->second;
      for (pi = e.ords.begin(); pi != e.ords.end(); pi++)
      {
         const short& prn = pi->first;
         const gpstk::ObsRngDev& ord = pi->second;
         
         s << left << setw(20) << time << right
           << setfill(' ')
           << " " << setw(2) << prn
           << " " << setw(4) << 0
           << " " << setprecision(1) << setw(5)  << ord.getElevation()
           << " " << setprecision(3) << setw(12)  << e.clockOffset

           << "  " << setw(14) << setprecision(5) << ord.getORD()
           << hex
           << " " << setw(4) << ord.getIODC()
           << " " << setw(7) << ord.getHealth()
           << dec
           << endl;
      }
   }
}  // end dump()
