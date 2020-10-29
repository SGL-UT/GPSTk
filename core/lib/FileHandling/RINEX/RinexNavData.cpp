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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 * @file RinexNavData.cpp
 * Encapsulates RINEX Navigation data
 */

#include "StringUtils.hpp"

#include "CommonTime.hpp"
#include "CivilTime.hpp"
#include "GPSWeekSecond.hpp"

#include "RinexNavData.hpp"
#include "RinexNavStream.hpp"
#include "GNSSconstants.hpp"
#include "TimeString.hpp"

namespace gpstk
{
   using namespace gpstk::StringUtils;
   using namespace std;

   RinexNavData::RinexNavData()
         : time(gpstk::CommonTime::BEGINNING_OF_TIME), PRNID(-1),
           sf1XmitTime(0), toeWeek(0), codeflgs(0), accuracy(0),
           health(0), L2Pdata(0), IODC(0), IODE(0), af0(0), af1(0), af2(0),
           Tgd(0), Cuc(0), Cus(0), Crc(0), Crs(0), Cic(0), Cis(0), Toe(0),
           M0(0), dn(0), ecc(0), Ahalf(0), OMEGA0(0), i0(0), w(0), OMEGAdot(0),
           idot(0), fitint(4)
   {
      time.setTimeSystem(gpstk::TimeSystem::GPS);
   }

   RinexNavData::RinexNavData(const EngEphemeris& ee)
         : time(ee.getEpochTime()), PRNID(ee.getPRNID()), sf1XmitTime(0),
           toeWeek(0), codeflgs(ee.getCodeFlags()), accuracy(ee.getAccuracy()),
           health(ee.getHealth()), L2Pdata(ee.getL2Pdata()), IODC(ee.getIODC()),
           IODE(ee.getIODE()), af0(ee.getAf0()), af1(ee.getAf1()),
           af2(ee.getAf2()), Tgd(ee.getTgd()), Cuc(ee.getCuc()),
           Cus(ee.getCus()), Crc(ee.getCrc()), Crs(ee.getCrs()),
           Cic(ee.getCic()), Cis(ee.getCis()), Toe(ee.getToe()), M0(ee.getM0()),
           dn(ee.getDn()), ecc(ee.getEcc()), Ahalf(ee.getAhalf()),
           OMEGA0(ee.getOmega0()), i0(ee.getI0()), w(ee.getW()),
           OMEGAdot(ee.getOmegaDot()), idot(ee.getIDot()),
           fitint(ee.getFitInterval())
   {
      setXmitTime(ee.getFullWeek(), ee.getHOWTime(1));
   }

   void RinexNavData::reallyPutRecord(FFStream& ffs) const
   {
      RinexNavStream& strm = dynamic_cast<RinexNavStream&>(ffs);

      strm << setw(2) << right << PRNID
           << printTime(time, " %02y %2m %2d %2H %2M%5.1f")
           << af0 << af1 << af2 << endlpp
           << "   " << IODE << Crs << dn << M0 << endlpp
           << "   " << Cuc << ecc << Cus << Ahalf << endlpp
           << "   " << Toe << Cic << OMEGA0 << Cis << endlpp
           << "   " << i0 << Crc << w << OMEGAdot << endlpp
           << "   " << idot << RNDouble(codeflgs) << RNDouble(toeWeek)
           << RNDouble(L2Pdata) << endlpp
           << "   " << accuracy << RNDouble(health) << Tgd << IODC << endlpp
           << "   " << RNDouble(sf1XmitTime);
      if (strm.header.version >= 2.1)
         strm << fitint;
      strm << endlpp;
   }

   void RinexNavData::reallyGetRecord(FFStream& ffs)
   {
      RinexNavStream& strm = dynamic_cast<RinexNavStream&>(ffs);

         // If the header hasn't been read, read it...
      if(!strm.headerRead)
         strm >> strm.header;

      string line;

      strm.formattedGetLine(line, true);
      getPRNEpoch(line);

      strm.formattedGetLine(line);
      getBroadcastOrbit1(line);

      strm.formattedGetLine(line);
      getBroadcastOrbit2(line);

      strm.formattedGetLine(line);
      getBroadcastOrbit3(line);

      strm.formattedGetLine(line);
      getBroadcastOrbit4(line);

      strm.formattedGetLine(line);
      getBroadcastOrbit5(line);

      strm.formattedGetLine(line);
      getBroadcastOrbit6(line);

      strm.formattedGetLine(line);
      getBroadcastOrbit7(line);
   }

   std::string RinexNavData::stableText() const
   {
      ostringstream s;
      s << "PRN: " << setw(2) << PRNID
        << " TOE: " << printTime(getToeTime(), "%02m/%02d/%04Y %02H:%02M:%02S")
        << " TOC: " << printTime(time, "%4F %10.3g")
        << " IODE: " << setw(4) << int(IODE)            // IODE should be int
        << " HOWtime: " << setw(6) << getHOWWS().sow;   // HOW should be double
      return s.str();
   }

   void RinexNavData::dump(ostream& s) const
   {
      s << "PRN: " << setw(2) << PRNID
        << " TOE: " << printTime(getToeTime(), "%02m/%02d/%04Y %02H:%02M:%02S")
        << " TOC: " << printTime(time, "%02m/%02d/%04Y %02H:%02M:%02S")
        << " IODE: " << setw(4) << int(IODE)            // IODE should be int
        << " HOWtime: " << setw(6) << getHOWWS().sow    // HOW should be double
        << endl;
   }

   RinexNavData::operator EngEphemeris() const throw()
   {
      EngEphemeris ee;

         // there's no TLM word in RinexNavData, so it's set to 0.
         // likewise, there's no AS alert or tracker.
         // Also, in Rinex, the accuracy is in meters, and setSF1 expects
         // the accuracy flag.  We'll give it zero and pass the accuracy
         // separately via the setAccuracy() method.
      CommonTime how1(getHOWTime()), how2(how1+6), how3(how2+6);
      GPSWeekSecond ws1(how1), ws2(how2), ws3(how3);
      ee.setSF1(0, ws1.sow, 0, ws1.week, codeflgs, 0, health,
                short(IODC), L2Pdata, Tgd, getTocWS().sow, af2, af1, af0, 0,
                PRNID);
      ee.setSF2(0, ws2.sow, 0, short(IODE), Crs, dn, M0, Cuc, ecc, Cus, Ahalf,
                Toe, (fitint > 4) ? 1 : 0);
      ee.setSF3(0, ws3.sow, 0, Cic, OMEGA0, Cis, i0, Crc, w, OMEGAdot,
                idot);
      ee.setFIC(false);
      ee.setAccuracy(accuracy);

      return ee;
   }

      // Convert this RinexNavData to a GPSEphemeris object.
      // for backward compatibility only - use Rinex3NavData
   RinexNavData::operator GPSEphemeris() const
   {
      GPSEphemeris gpse;
      try
      {
            // Overhead
         gpse.satID = SatID(PRNID, SatelliteSystem::GPS);
         gpse.ctToe = time;

            // clock model
         gpse.af0 = af0;
         gpse.af1 = af1;
         gpse.af2 = af2;
   
            // Major orbit parameters
         gpse.M0 = M0;
         gpse.dn = dn;
         gpse.ecc = ecc;
         gpse.A = Ahalf * Ahalf;
         gpse.OMEGA0 = OMEGA0;
         gpse.i0 = i0;
         gpse.w = w;
         gpse.OMEGAdot = OMEGAdot;
         gpse.idot = idot;
            // modern nav msg
         gpse.dndot = 0.;
         gpse.Adot = 0.;
   
            // Harmonic perturbations
         gpse.Cuc = Cuc;
         gpse.Cus = Cus;
         gpse.Crc = Crc;
         gpse.Crs = Crs;
         gpse.Cic = Cic;
         gpse.Cis = Cis;
   
         gpse.dataLoadedFlag = true;

         gpse.ctToc = time;
         gpse.ctToc.setTimeSystem(TimeSystem::GPS);

            // now load the GPS-specific parts
         gpse.IODC = IODC;
         gpse.IODE = IODE;
         gpse.health = health;
         gpse.accuracyFlag = accuracy;
         gpse.Tgd = Tgd;

         gpse.HOWtime = getHOWWS().sow;
         gpse.transmitTime = getXmitTime();
         gpse.transmitTime.setTimeSystem(TimeSystem::GPS);

         gpse.codeflags = codeflgs;
         gpse.L2Pdata = L2Pdata;

            /// @note IODC must be set first...
         gpse.fitint = fitint;
         gpse.setFitIntervalFlag(int(fitint)); 
         gpse.adjustValidity();
      }
      catch(Exception& e)
      {
         GPSTK_RETHROW(e);
      }
      return gpse;
   }

   list<double> RinexNavData::toList() const
   {
      list<double> l;
      GPSWeekSecond howws(getHOWWS());

      l.push_back(PRNID);
      l.push_back(howws.sow);
      l.push_back(howws.week);
      l.push_back(codeflgs);
      l.push_back(accuracy.val);
      l.push_back(health);
      l.push_back(L2Pdata);
      l.push_back(IODC.val);
      l.push_back(IODE.val);
      l.push_back(getTocWS().sow);
      l.push_back(af0.val);
      l.push_back(af1.val);
      l.push_back(af2.val);
      l.push_back(Tgd.val);
      l.push_back(Cuc.val);
      l.push_back(Cus.val);
      l.push_back(Crc.val);
      l.push_back(Crs.val);
      l.push_back(Cic.val);
      l.push_back(Cis.val);
      l.push_back(Toe.val);
      l.push_back(M0.val);
      l.push_back(dn.val);
      l.push_back(ecc.val);
      l.push_back(Ahalf.val);
      l.push_back(OMEGA0.val);
      l.push_back(i0.val);
      l.push_back(w.val);
      l.push_back(OMEGAdot.val);
      l.push_back(idot.val);
      l.push_back(fitint.val);

      return l;
   }

   void RinexNavData::getPRNEpoch(const string& currentLine)
   {
      try
      {
            // check for spaces in the right spots...
         for (int i = 2; i <= 17; i += 3)
            if (currentLine[i] != ' ')
               throw(FFStreamError("Badly formatted line"));

         PRNID = asInt(currentLine.substr(0,2));

         short yr = asInt(currentLine.substr(2,3));
         short mo = asInt(currentLine.substr(5,3));
         short day = asInt(currentLine.substr(8,3));
         short hr = asInt(currentLine.substr(11,3));
         short min = asInt(currentLine.substr(14,3));
         double sec = asDouble(currentLine.substr(17,5));

            // years 80-99 represent 1980-1999
         const int rolloverYear = 80;
         if (yr < rolloverYear)
            yr += 100;
         yr += 1900;

            // Real Rinex has epochs 'yy mm dd hr 59 60.0'
            // surprisingly often....
         double ds=0;
         if(sec >= 60.)
         {
            ds=sec;
            sec=0.0;
         }
         time = CivilTime(yr,mo,day,hr,min,sec,gpstk::TimeSystem::GPS).convertToCommonTime();
         if(ds != 0) time += ds;

         af0 = currentLine.substr(22,19);
         af1 = currentLine.substr(41,19);
         af2 = currentLine.substr(60,19);
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void RinexNavData::getBroadcastOrbit1(const string& currentLine)
   {
      try
      {
         IODE = currentLine.substr(3,19);
         Crs = currentLine.substr(22,19);
         dn = currentLine.substr(41,19);
         M0 = currentLine.substr(60,19);
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void RinexNavData::getBroadcastOrbit2(const string& currentLine)
   {
      try
      {
         Cuc = currentLine.substr(3,19);
         ecc = currentLine.substr(22,19);
         Cus = currentLine.substr(41,19);
         Ahalf = currentLine.substr(60,19);
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void RinexNavData::getBroadcastOrbit3(const string& currentLine)
   {
      try
      {
         Toe = currentLine.substr(3,19);
         Cic = currentLine.substr(22,19);
         OMEGA0 = currentLine.substr(41,19);
         Cis = currentLine.substr(60,19);
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void RinexNavData::getBroadcastOrbit4(const string& currentLine)
   {
      try
      {
         i0 = currentLine.substr(3,19);
         Crc = currentLine.substr(22,19);
         w = currentLine.substr(41,19);
         OMEGAdot = currentLine.substr(60,19);
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void RinexNavData::getBroadcastOrbit5(const string& currentLine)
   {
      try
      {
         RNDouble codeL2(0), L2P(0), toe_wn(0);

         idot = currentLine.substr(3,19);
         codeL2 = currentLine.substr(22,19);
         toe_wn = currentLine.substr(41,19);
         L2P = currentLine.substr(60,19);

         codeflgs = (short) codeL2;
         L2Pdata = (short) L2P;
         toeWeek = (short) toe_wn;
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void RinexNavData::getBroadcastOrbit6(const string& currentLine)
   {
      try
      {
         RNDouble SV_health(0);

         accuracy = currentLine.substr(3,19);
         SV_health = currentLine.substr(22,19);
         Tgd = currentLine.substr(41,19);
         IODC = currentLine.substr(60,19);


         health = (short) SV_health;
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void RinexNavData::getBroadcastOrbit7(const string& currentLine)
   {
      try
      {
         RNDouble HOW_sec(0);

         HOW_sec = currentLine.substr(3,19);
            // leave it alone so round-trips are possible
            // (even though we're storing a double as a long, which
            //could lead to failures in round-trip testing, though if
            //that happens your transmit time is messed).
            //setXmitTime(HOW_sec);
         sf1XmitTime = HOW_sec;
         fitint = currentLine.substr(22,19);
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }


   GPSWeekSecond RinexNavData::getXmitWS() const
   {
      GPSWeekSecond rv;
         // sf1XmitTime may not actually be a proper subframe 1
         // transmit time.  It may be a HOW time or something like
         // that.
      if (sf1XmitTime < 0)
      {
            // If the transmit time is negative, assume that it
            // corresponds to the Toe week, according to the footnote
            // attached to Table A4 in the 2.11 standard.
         long properXmit = fixSF1xmitSOW(sf1XmitTime+FULLWEEK);
         rv = GPSWeekSecond(toeWeek-1, properXmit, TimeSystem::GPS);
      }
      else
      {
            // If the transmit time is >= 0, make sure that we have
            // the right week using a trusty old half-week test.
         long properXmit = fixSF1xmitSOW(sf1XmitTime);
         double diff = Toe - properXmit;
         if (diff < -HALFWEEK)
            rv = GPSWeekSecond(toeWeek-1, properXmit, TimeSystem::GPS);
         else if (diff > HALFWEEK)
            rv = GPSWeekSecond(toeWeek+1, properXmit, TimeSystem::GPS);
         else
            rv = GPSWeekSecond(toeWeek, properXmit, TimeSystem::GPS);
      }
      return rv;
   }


   RinexNavData& RinexNavData::setXmitWeek(unsigned short fullweek)
   {
      if (sf1XmitTime < 0)
      {
            // If the transmit time is negative, assume that the
            // transmit week corresponds to the Toe week, according to
            // the footnote attached to Table A4 in the 2.11 standard.
         toeWeek = fullweek;
      }
      else
      {
            // If the transmit time is >= 0, make sure that we have
            // the right week using a trusty old half-week test.
         double diff = Toe - sf1XmitTime;
         if (diff < -HALFWEEK)
         {
            toeWeek = fullweek+1;
               // adjust  transmit time to be relative to the week.
               // week is in broadcast orbit 5
               // transmission time is in broadcast orbit 7
               //   see footnote in RINEX 2.11 document
            sf1XmitTime -= FULLWEEK;
         }
         else if (diff > HALFWEEK)
         {
            toeWeek = fullweek-1;
               // see comments above
            sf1XmitTime += FULLWEEK;
         }
         else
            toeWeek = fullweek;
      }
      return *this;
   }


   RinexNavData& RinexNavData::setXmitTime(unsigned short fullweek,
                                           unsigned long sow)
   {
      setXmitTime(sow);
      setXmitWeek(fullweek);
      return *this;
   }


}  // end of namespace
