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

#include "Geodetic.hpp"
#include "GPSGeoid.hpp"
#include "StringUtils.hpp"

#include "ScreenProc.hpp"
#include "RinexConverters.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;

extern "C" void wench(int sig);

/*           1         2         3         4         5         6         7
   01234567890123456789012345678901234567890123456789012345678901234567890123456789
  |--------------------------------------------------------------------------------
 0|hostname:port                                        02:43:12  7/7/06 GPS
 1|                   
 2|PVT: 02:43:23.5   Offset: 234,456.1 ns  Drift: 13.235 ns/d
 3|Lon: -179.122345   Lat: 30.12345 N   Ht: 230.0 m      Rate: 1.0 s
 4| Vx:  0.0 cm/s      Vy:  0.0 cm/s    Vz:  0.00 cm/s    FOM: 12.2
 5| Trx: 28 C   ExtFreq: Unlocked    StartTime: 12:34 6/2/06   SSW: 311
 6|Tant: 39 C  Selftest: 0x000001     TestTime: 02:30 7/7/06
 7|
 8|Obs Rate: 1.5 s
 9|                   C1    P1      C2    P2      lock             res   
10|Ch Prn   Az  El    SNR   SNR     SNR   SNR     count  iodc  h   (m)
11|-- ---  ---  ---  ----  ------  ----  ------  ------  ----  -  -----
12| 1   8  133  22+  41.2  33.8 Y  38.1  39.2 Y  432000   2ba  0  
13| 2  31
14| 3  23
15| 4  27
16| 5  28
17| 6   3
18| 7  11
19| 8  13
20| 9   7
21|10  19
22|11  --
23|12  --
  |-------------------------------------------------------------------------------
*/
int pvtRow=2;
int tCol=5, offCol=26, driftCol=47;
int lonCol=5, latCol=23, altCol=40, prateCol=59;

int stsRow=5;
int trxCol=6, freqCol=22, stsTimeCol=44, stsSSWCol=65;

int chanRow=11;
int prnCol=3, azCol=8, elCol=13, c1snrCol=18, p1snrCol=24, c2snrCol=32;
int p2snrCol=38, resCol=63, lockCol=46, iodcCol=54, healthCol=60, orateCol=10;

bool MDPScreenProcessor::gotWench;

MDPScreenProcessor::MDPScreenProcessor(gpstk::MDPStream& in, std::ofstream& out):
   MDPProcessor(in, out),
   updateRate(0.5), obsRate(-1), pvtRate(-1)
{
   signal(SIGWINCH, wench);

   // Set up curses
   win = initscr();
   cbreak();
   nodelay(win, true);
   noecho();
   nonl();
   intrflush(win, true);
   keypad(win, true);
   prev_curs = ::curs_set(0);   // we want an invisible cursor. 

   gotWench=false;
   followEOF=true;

   host=in.filename;

   drawBase();

   obsOut = true;
   pvtOut = true;
   navOut = true;
   tstOut = true;
}


MDPScreenProcessor::~MDPScreenProcessor()
{
   curs_set(prev_curs);
   endwin();
}


//-----------------------------------------------------------------------------
void MDPScreenProcessor::process(const MDPObsEpoch& oe)
{
   int chan=oe.channel;

   if (chan>12)
      return;

   // Figure out whether the SV is rising or setting
   if ((currentObs[chan]).elevation > oe.elevation)
      elDir[chan]=-1;
   else if ((currentObs[chan]).elevation < oe.elevation)
      elDir[chan]=+1;
   else
      elDir[chan]=0;

   // Determine the obs output rate
   double dt = oe.time - currentObs[chan].time;
   if (currentObs[chan].prn > 0 && std::abs(dt-obsRate) > 1e-3)
      obsRate=dt;

   currentObs[chan] = oe;
   
   // Set channels inactive if we haven't seen data from them recently
   if (obsRate>0)
      for (int i=1; i<=12; i++)
         if (currentObs[i].prn > 0 && oe.time - currentObs[i].time > obsRate*2)
            currentObs[i].prn = 0;

   drawChan(chan);
   redraw();
}

//-----------------------------------------------------------------------------
void MDPScreenProcessor::process(const MDPPVTSolution& pvt)
{
   if (host=="")
   {
      host=in.filename;
      drawBase();
   }

   double dt = pvt.time - currentPvt.time;
   if (std::abs(dt-pvtRate) > 1e-3)
      pvtRate=dt;
   currentPvt = pvt;
   drawPVT();
   redraw();
}


void MDPScreenProcessor::process(const gpstk::MDPNavSubframe& sf)
{
   short sfid = sf.getSFID();
 
   NavIndex ni(RangeCarrierPair(sf.range, sf.carrier), sf.prn);
   prev[ni] = curr[ni];
   curr[ni] = sf;

   long sfa[10];
   sf.fillArray(sfa);
   if (gpstk::EngNav::subframeParity(sfa))
   {
      if (sfid > 3)
         return;
      
      EphemerisPages& ephPages = ephPageStore[ni];
      ephPages[sfid] = sf;
      EngEphemeris engEph;
      
      if (makeEngEphemeris(engEph, ephPages))
         ephStore[ni] = engEph;
   }
   else
   {
      parErrCnt[ni]++;
   }
}

void MDPScreenProcessor::process(const gpstk::MDPSelftestStatus& sts)
{
   currentSts = sts;
   drawSTS();
   redraw();
}

// Yes, one would think that sun would have a working curses but NO!!
// They require a non-const string to be passed to mvwaddstr()
// grrr.
void writeAt(WINDOW* win, int row, int col, const string s)
{
   char *str = const_cast<char*>(s.c_str());
   mvwaddstr(win, row, col, str);
}

void MDPScreenProcessor::redraw()
{
   gpstk::DayTime now;
   if (now - lastUpdateTime > updateRate)
   {
      string time=currentPvt.time.printf(" %02H:%02M:%02S %2m/%d/%02y");
      writeAt(win, 0, COLS-time.length()-5, time.c_str());
      lastUpdateTime = now;

      if (gotWench)
      {
         endwin();
         refresh();
         char buff[30];
         sprintf(buff, "%2d x %2d (wench)", LINES, COLS);
         writeAt(win, 0, COLS/2-15, buff);
         gotWench=false;
         clearok(win,true);
         drawBase();
      }
   }

   wrefresh(win);
}

void MDPScreenProcessor::drawSTS()
{
   string firstTime=currentSts.firstPVTTime.printf("%02H:%02M %m/%d/%2Y  ");
   writeAt(win, stsRow, stsTimeCol, firstTime.c_str());

   string testTime=currentSts.selfTestTime.printf("%02H:%02M %m/%d/%2Y  ");
   writeAt(win, stsRow+1, stsTimeCol, testTime.c_str());

   if (currentSts.extFreqStatus)
      writeAt(win, stsRow, freqCol, "Locked  ");
   else
      writeAt(win, stsRow, freqCol, "UnLocked");

   string sts=leftJustify(int2x(currentSts.status), 8);
   writeAt(win, stsRow+1, freqCol, sts.c_str());

   string trx=leftJustify(asString(currentSts.receiverTemp, 0), 2) + "C";
   writeAt(win, stsRow, trxCol, trx.c_str());
   
   string tant=leftJustify(asString(currentSts.antennaTemp, 0), 2) + "C";
   writeAt(win, stsRow+1, trxCol, tant.c_str());

   string ssw=leftJustify(int2x(currentSts.saasmStatusWord), 3);
   writeAt(win, stsRow, stsSSWCol, ssw.c_str());
}


void MDPScreenProcessor::drawPVT()
{
   string s=rightJustify(asString(pvtRate,1), 3) + " s";
   writeAt(win, pvtRow+1 , prateCol, s.c_str());

   string time=currentPvt.time.printf("%02H:%02M:%04.1f");
   writeAt(win, pvtRow, tCol, time.c_str());
   string off=rightJustify(asString(currentPvt.dtime*1e9, 1), 9) + " ns";
   writeAt(win, pvtRow, offCol, off.c_str());

   gpstk::GPSGeoid gm;
   gpstk::Geodetic llh(currentPvt.x, &gm);

   string lat, lon, alt;
   if (llh[0] > 0)
      lat=leftJustify(asString(llh[0],5)+" N", 12);
   else
      lat=leftJustify(asString(std::abs(llh[0]),5)+" S", 12);
   if (llh[1] < 180)
      lon=leftJustify(asString(llh[1],5)+" E", 12);
   else
      lon=leftJustify(asString(360.0-llh[1],5)+" W", 12);
   alt=leftJustify(asString(llh[2],3) + " m", 12);
   writeAt(win, pvtRow+1, latCol, (const char *)lat.c_str());
   writeAt(win, pvtRow+1, lonCol, lon.c_str());
   writeAt(win, pvtRow+1, altCol, alt.c_str());

   string drift=rightJustify(asString(currentPvt.ddtime*1e9*86400, 2), 9) + " ns/d";
   writeAt(win, pvtRow, driftCol, drift.c_str());
   string vx, vy, vz;
   vx=leftJustify(asString(currentPvt.v[0] * 100, 2)+" cm/s", 11);
   vy=leftJustify(asString(currentPvt.v[1] * 100, 2)+" cm/s", 11);
   vz=leftJustify(asString(currentPvt.v[2] * 100, 2)+" cm/s", 11);
   writeAt(win, pvtRow+2, lonCol, vx.c_str());
   writeAt(win, pvtRow+2, latCol, vy.c_str());
   writeAt(win, pvtRow+2, altCol, vz.c_str());

   string fom = leftJustify(asString((int)currentPvt.fom), 3);
   writeAt(win, pvtRow+2, prateCol, fom.c_str());
   fom = leftJustify(asString((int)currentPvt.pvtMode), 2);
   writeAt(win, pvtRow+2, prateCol+4, fom.c_str());
   fom = leftJustify(asString((int)currentPvt.corrections), 2);
   writeAt(win, pvtRow+2, prateCol+6, fom.c_str());
}


void MDPScreenProcessor::drawChan(int chan)
{
   if (chan>12 || chan < 1)
      return;

   for (int i=1; i<=12; i++)
      if (currentObs[i].prn == 0)
      {
         writeAt(win, chanRow+i, prnCol, " --");
         wclrtoeol(win);
      }

   int row = chanRow + chan;
   const MDPObsEpoch& obs=currentObs[chan];
   if (obs.prn == 0)
      return;

   string orate = leftJustify(asString(obsRate,1)+" s", 7);
   writeAt(win, chanRow-3 , orateCol, orate.c_str());

   string prn=rightJustify(asString((int)obs.prn), 3);
   string az=rightJustify(asString(obs.azimuth, 0), 3);
   string el=rightJustify(asString(obs.elevation, 0), 2);
   string health = rightJustify(int2x(obs.status), 2);

   if (elDir[chan] > 0)
      el=el+"+";
   else if (elDir[chan] < 0)
      el=el+"-";

   writeAt(win, row, prnCol, prn.c_str());
   writeAt(win, row, azCol, az.c_str());
   writeAt(win, row, elCol, el.c_str());
   writeAt(win, row, healthCol, health.c_str());

   // RangeCode:  rcUnknown, rcCA, rcPcode, rcYcode, rcCodeless, rcL2CM, rcL2CL, rcMcode1, rcMcode2
   // NavCode:  ncUnknown, ncICD_200_2, ncICD_700_M, ncICD_705_L5, ncICD_200_4
   if (obs.haveObservation(ccL1, rcCA))
   {
      MDPObsEpoch::Observation o = obs.getObservation(ccL1, rcCA);
      string snr = rightJustify(asString(o.snr, 1), 4);
      string lockCount = rightJustify(asString(o.lockCount), 6);
      writeAt(win, row, c1snrCol, snr.c_str());
      writeAt(win, row, lockCol, lockCount.c_str());
   }

   if (obs.haveObservation(ccL2,rcCM))
   {
      MDPObsEpoch::Observation o = obs.getObservation(ccL2, rcCM);
      string snr = rightJustify(asString(o.snr, 1), 4);
      string lockCount = rightJustify(asString(o.lockCount), 6);
      writeAt(win, row, c2snrCol, snr.c_str());
   }

   if (obs.haveObservation(ccL1, rcYcode))
   {
      MDPObsEpoch::Observation o=obs.getObservation(ccL1, rcYcode);
      string snr = rightJustify(asString(o.snr, 1), 4) + " Y";
      writeAt(win, row, p1snrCol, snr.c_str());
   }
   else if (obs.haveObservation(ccL1, rcPcode))
   {
      MDPObsEpoch::Observation o=obs.getObservation(ccL1, rcPcode);
      string snr = rightJustify(asString(o.snr, 1), 4) + " P";
      writeAt(win, row, p1snrCol, snr.c_str());
   }
   else if (obs.haveObservation(ccL1, rcCodeless))
   {
      MDPObsEpoch::Observation o=obs.getObservation(ccL1, rcCodeless);
      string snr = rightJustify(asString(o.snr, 1), 4) + " Z";
      writeAt(win, row, p1snrCol, snr.c_str());
   }

   if (obs.haveObservation(ccL2, rcYcode))
   {
      MDPObsEpoch::Observation o=obs.getObservation(ccL2, rcYcode);
      string snr = rightJustify(asString(o.snr, 1), 4) + " Y";
      writeAt(win, row, p2snrCol, snr.c_str());
   }
   else if (obs.haveObservation(ccL2, rcPcode))
   {
      MDPObsEpoch::Observation o=obs.getObservation(ccL2, rcPcode);
      string snr = rightJustify(asString(o.snr, 1), 4) + " P";
      writeAt(win, row, p2snrCol, snr.c_str());
   }
   else if (obs.haveObservation(ccL2, rcCodeless))
   {
      MDPObsEpoch::Observation o=obs.getObservation(ccL2, rcCodeless);
      string snr = rightJustify(asString(o.snr, 1), 4) + " Z";
      writeAt(win, row, p2snrCol, snr.c_str());
   }

   EphStore::const_iterator es_itr;
   for (es_itr=ephStore.begin(); es_itr != ephStore.end(); es_itr++)
      if (es_itr->first.second == obs.prn)
         break;

   if (es_itr == ephStore.end())
      return;

   const NavIndex& ni = es_itr->first;
   const gpstk::EngEphemeris& eph=es_itr->second;
   const gpstk::RangeCode rc = ni.first.first;
   const gpstk::CarrierCode cc = ni.first.second;

   string iodc=rightJustify(int2x(eph.getIODC()), 4);
   writeAt(win, row, iodcCol, iodc);
}


void MDPScreenProcessor::drawBase()
{
   wclear(win);
   char buff[80];
   sprintf(buff, "%2d x %2d", LINES, COLS);
   writeAt(win, 0, COLS/2-4, buff);

   writeAt(win, 0, 0, host.c_str());
   writeAt(win, 0, COLS-3, "GPS");

   writeAt(win, pvtRow,   0, "PVT:              Offset:");
   writeAt(win, pvtRow+1, 0, "Lon:              Lat:              Ht:              Rate:");
   writeAt(win, pvtRow,   0, "PVT:              Offset:               Drift:");
   writeAt(win, pvtRow+2, 0, " Vx:               Vy:              Vz:              FOM:");

   writeAt(win, stsRow,   0, " Trx:        ExtFreq:            StartTime:                  SSW:");
   writeAt(win, stsRow+1, 0, "Tant:       Selftest:             TestTime:              ");

   writeAt(win, chanRow-3, 0, "Obs Rate:");
   writeAt(win, chanRow-2, 0, "                   C1    P1      C2    P2      lock           ");
   writeAt(win, chanRow-1, 0, "Ch Prn   Az  El    SNR   SNR     SNR   SNR     count  iodc   h");
   writeAt(win, chanRow,   0, "-- ---  ---  --   ----  ------  ----  ------  ------  ----  --");
   for (int i=1; i<=12; i++)
   {
      string str=rightJustify(asString(i),2);
      writeAt(win, chanRow+i, 0, str.c_str());
      writeAt(win, chanRow+i, prnCol, " --");
      wclrtoeol(win);
   }
   redraw();
}


void wench(int sig)
{
   MDPScreenProcessor::gotWench=true;
}
