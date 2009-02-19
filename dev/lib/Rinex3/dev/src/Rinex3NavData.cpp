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


/**
 * @file Rinex3NavData.cpp
 * Encapsulates RINEX 3 Navigation data
 */

#include "StringUtils.hpp"
#include "CivilTime.hpp"
#include "GPSWeekSecond.hpp"
#include "Rinex3NavData.hpp"
#include "Rinex3NavStream.hpp"
#include "icd_200_constants.hpp"

namespace gpstk
{
   using namespace gpstk::StringUtils;
   using namespace std;

   Rinex3NavData::Rinex3NavData(const EngEphemeris& ee)
   {
      // universal epoch info

      satSys = ee.getSatSys();
      PRNID  = ee.getPRNID();
      time   = ee.getEpochTime();

      Toc     = ee.getToc();
      HOWtime = long(ee.getHOWTime(1));
      weeknum = ee.getFullWeek();

      accuracy = ee.getAccuracy();
      health   = ee.getHealth();

      // GPS or Galileo data

      af0 = ee.getAf0(); // GPS and Galileo only
      af1 = ee.getAf1(); // GPS and Galileo only
      af2 = ee.getAf2(); // GPS and Galileo only

      Crs = ee.getCrs(); // GPS and Galileo only
      dn  = ee.getDn();  // GPS and Galileo only
      M0  = ee.getM0();  // GPS and Galileo only

      Cuc   = ee.getCuc();   // GPS and Galileo only
      ecc   = ee.getEcc();   // GPS and Galileo only
      Cus   = ee.getCus();   // GPS and Galileo only
      Ahalf = ee.getAhalf(); // GPS and Galileo only

      Toe    = ee.getToe();    // GPS and Galileo only
      Cic    = ee.getCic();    // GPS and Galileo only
      OMEGA0 = ee.getOmega0(); // GPS and Galileo only
      Cis    = ee.getCis();    // GPS and Galileo only

      i0       = ee.getI0();       // GPS and Galileo only
      Crc      = ee.getCrc();      // GPS and Galileo only
      w        = ee.getW();        // GPS and Galileo only
      OMEGAdot = ee.getOmegaDot(); // GPS and Galileo only

      idot = ee.getIDot(); // GPS and Galileo only

      // GPS-only data

      IODE = ee.getIODE(); // GPS only

      codeflgs = ee.getCodeFlags(); // GPS only
      L2Pdata  = ee.getL2Pdata();   // GPS only

      Tgd  = ee.getTgd();  // GPS only
      IODC = ee.getIODC(); // GPS only

      fitint = ee.getFitInterval(); // GPS only

      // Galileo-only data

      IODnav = ee.getIODnav(); // Galileo only

      datasources = ee.getDatasources(); // Galileo only

      BGDa = ee.getBGDa(); // Galileo only
      BGDb = ee.getBGDb(); // Galileo only

      // GLONASS-only data

      TauN   = ee.getTauN();   // GLONASS only
      GammaN = ee.getGammaN(); // GLONASS only
      MFtime = ee.getMFtime(); // GLONASS only

      px = ee.getpx(); // GLONASS only
      vx = ee.getvx(); // GLONASS only
      ax = ee.getax(); // GLONASS only
      py = ee.getpy(); // GLONASS only
      vy = ee.getvy(); // GLONASS only
      ay = ee.getay(); // GLONASS only
      pz = ee.getpz(); // GLONASS only
      vz = ee.getvz(); // GLONASS only
      az = ee.getaz(); // GLONASS only

      freqNum   = ee.getFreqNum();   // GLONASS only
      ageOfInfo = ee.getAgeOfInfo(); // GLONASS only

   }

   void Rinex3NavData::reallyPutRecord(FFStream& ffs) const
      throw(exception, FFStreamError, StringException)
   {
      Rinex3NavStream& strm = dynamic_cast<Rinex3NavStream&>(ffs);

      strm << putPRNEpoch() << endl;
      strm.lineNumber++;
      strm << putBroadcastOrbit1() << endl;
      strm.lineNumber++;
      strm << putBroadcastOrbit2() << endl;
      strm.lineNumber++;
      strm << putBroadcastOrbit3() << endl;
      strm.lineNumber++;
      if ( satSys == "G" || satSys == "E" ) // GPS and Galileo have 7 B.O.'s
        {
          strm << putBroadcastOrbit4() << endl;
          strm.lineNumber++;
          strm << putBroadcastOrbit5() << endl;
          strm.lineNumber++;
          strm << putBroadcastOrbit6() << endl;
          strm.lineNumber++;
          strm << putBroadcastOrbit7(strm.header.version) << endl;
          strm.lineNumber++;
        }
   }

   void Rinex3NavData::reallyGetRecord(FFStream& ffs) 
      throw(exception, FFStreamError, StringException)
   {
      Rinex3NavStream& strm = dynamic_cast<Rinex3NavStream&>(ffs);

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

   void Rinex3NavData::dump(ostream& s) const
   {
      s << "SatSys: " << satSys
        << "PRN: " << setw(2) << PRNID
        << " TOE: " << time
        << " TOC: " << setw(4) << weeknum << " " 
        << fixed << setw(10) << setprecision(3) << Toc
        << " IODE: " << setw(4) << int(IODE)            // IODE should be int
        << " HOWtime: " << setw(6) << HOWtime           // HOW should be double
        << endl;
        //<< ios::hex << IODE << " HOWtime: " << HOWtime << endl; ?? IODE is double
   }

   Rinex3NavData::operator EngEphemeris() const throw()
   {
      EngEphemeris ee;

         // there's no TLM word in Rinex3NavData, so it's set to 0.
         // likewise, there's no AS alert or tracker.
         // Also, in Rinex, the accuracy is in meters, and setSF1 expects
         // the accuracy flag.  We'll give it zero and pass the accuracy
         // separately via the setAccuracy() method.
      ee.setSF1(0, HOWtime, 0, weeknum, codeflgs, 0, health,
                short(IODC), L2Pdata, Tgd, Toc, af2, af1, af0, 0, PRNID);
      ee.setSF2(0, HOWtime, 0, short(IODE), Crs, dn, M0, Cuc, ecc, Cus, Ahalf,
                Toe, (fitint > 4) ? 1 : 0);
      ee.setSF3(0, HOWtime, 0, Cic, OMEGA0, Cis, i0, Crc, w, OMEGAdot,
                idot);

      ee.setAccuracy(accuracy);

      return ee;
   }

   list<double> Rinex3NavData::toList() const
   {
      list<double> l;

      l.push_back(PRNID);
      l.push_back(HOWtime);
      l.push_back(weeknum);
      l.push_back(codeflgs);
      l.push_back(accuracy);
      l.push_back(health);
      l.push_back(L2Pdata);
      l.push_back(IODC);
      l.push_back(IODE);
      l.push_back(Toc);
      l.push_back(af0);
      l.push_back(af1);
      l.push_back(af2);
      l.push_back(Tgd);
      l.push_back(Cuc);
      l.push_back(Cus);
      l.push_back(Crc);
      l.push_back(Crs);
      l.push_back(Cic);
      l.push_back(Cis);
      l.push_back(Toe);
      l.push_back(M0);
      l.push_back(dn);
      l.push_back(ecc);
      l.push_back(Ahalf);
      l.push_back(OMEGA0);
      l.push_back(i0);
      l.push_back(w);
      l.push_back(OMEGAdot);
      l.push_back(idot);
      l.push_back(fitint);

      return l;
   }

   string Rinex3NavData::putPRNEpoch(void) const
      throw(StringException)
   {
      string line;
      CivilTime civtime = time;

      line += satSys;
      line += rightJustify(asString(PRNID), 2);
      line += string(1, ' ');
      line += rightJustify(asString<short>(civtime.year  ), 4);
      line += string(1, ' ');
      line += rightJustify(asString<short>(civtime.month ), 2);
      line += string(1, ' ');
      line += rightJustify(asString<short>(civtime.day   ), 2);
      line += string(1, ' ');
      line += rightJustify(asString<short>(civtime.hour  ), 2);
      line += string(1, ' ');
      line += rightJustify(asString<short>(civtime.minute), 2);
      line += string(1, ' ');
      line += rightJustify(asString<short>(civtime.second), 2);

      if ( satSys == "R" ) // GLONASS
        {
          line += string(1, ' ');
          line += doub2for(TauN          , 18, 2);
          line += string(1, ' ');
          line += doub2for(GammaN        , 18, 2);
          line += string(1, ' ');
          line += doub2for((double)MFtime, 18, 2);
        }
      else                 // GPS or Galileo
        {
          line += string(1, ' ');
          line += doub2for(af0, 18, 2);
          line += string(1, ' ');
          line += doub2for(af1, 18, 2);
          line += string(1, ' ');
          line += doub2for(af2, 18, 2);
        }

      return line;
   }

   string Rinex3NavData::putBroadcastOrbit1(void) const
      throw(StringException)
   {
      string line;

      line += string(4, ' ');
      if ( satSys == "R" )      // GLONASS
        {
          line += string(1, ' ');
          line += doub2for(px, 18, 2);
          line += string(1, ' ');
          line += doub2for(vx, 18, 2);
          line += string(1, ' ');
          line += doub2for(ax, 18, 2);
          line += string(1, ' ');
          line += doub2for((double)health, 18, 2);
        }
      else if ( satSys == "G" ) // GPS
        {
          line += string(1, ' ');
          line += doub2for(IODE, 18, 2);
          line += string(1, ' ');
          line += doub2for(Crs , 18, 2);
          line += string(1, ' ');
          line += doub2for(dn  , 18, 2);
          line += string(1, ' ');
          line += doub2for(M0  , 18, 2);
        }
      else if ( satSys == "E" ) // Galileo
        {
          line += string(1, ' ');
          line += doub2for(IODnav, 18, 2);
          line += string(1, ' ');
          line += doub2for(Crs   , 18, 2);
          line += string(1, ' ');
          line += doub2for(dn    , 18, 2);
          line += string(1, ' ');
          line += doub2for(M0    , 18, 2);
        }

      return line;
   }

   string Rinex3NavData::putBroadcastOrbit2(void) const
      throw(StringException)
   {
      string line;

      line += string(4, ' ');
      if ( satSys == "R" )
        {
          line += string(1, ' ');
          line += doub2for(py, 18, 2);
          line += string(1, ' ');
          line += doub2for(vy, 18, 2);
          line += string(1, ' ');
          line += doub2for(ay, 18, 2);
          line += string(1, ' ');
          line += doub2for((double)freqNum, 18, 2);
        }
      else
        {
          line += string(1, ' ');
          line += doub2for(Cuc  , 18, 2);
          line += string(1, ' ');
          line += doub2for(ecc  , 18, 2);
          line += string(1, ' ');
          line += doub2for(Cus  , 18, 2);
          line += string(1, ' ');
          line += doub2for(Ahalf, 18, 2);
        }

      return line;
   }

   string Rinex3NavData::putBroadcastOrbit3(void) const
      throw(StringException)
   {
      string line;

      line += string(4, ' ');
      if ( satSys == "R" )
        {
          line += string(1, ' ');
          line += doub2for(pz, 18, 2);
          line += string(1, ' ');
          line += doub2for(vz, 18, 2);
          line += string(1, ' ');
          line += doub2for(az, 18, 2);
          line += string(1, ' ');
          line += doub2for(ageOfInfo, 18, 2);
        }
      else
        {
          line += string(1, ' ');
          line += doub2for(Toe   , 18, 2);
          line += string(1, ' ');
          line += doub2for(Cic   , 18, 2);
          line += string(1, ' ');
          line += doub2for(OMEGA0, 18, 2);
          line += string(1, ' ');
          line += doub2for(Cis   , 18, 2);
        }

      return line;
   }

   string Rinex3NavData::putBroadcastOrbit4(void) const
      throw(StringException)
   {
      string line;

      line += string(4, ' ');
      line += string(1, ' ');
      line += doub2for(i0, 18, 2);
      line += string(1, ' ');
      line += doub2for(Crc, 18, 2);
      line += string(1, ' ');
      line += doub2for(w, 18, 2);
      line += string(1, ' ');
      line += doub2for(OMEGAdot, 18, 2);

      return line;
   }

   string Rinex3NavData::putBroadcastOrbit5(void) const
      throw(StringException)
   {
      // Internally (Rinex3NavData and EngEphemeris), weeknum is the week of HOW.
      // In RIENX 3 *files*, weeknum is the week of TOE.
      double wk = double(weeknum);
      if(HOWtime - Toe > HALFWEEK)
         wk++;
      else if(HOWtime - Toe < -(HALFWEEK))
         wk--;

      string line;

      line += string(4, ' ');
      line += string(1, ' ');
      line += doub2for(idot, 18, 2);
      if ( satSys == "G" )      // GPS
        {
          line += string(1, ' ');
          line += doub2for((double)codeflgs, 18, 2);
        }
      else if ( satSys == "E" ) // Galileo
        {
          line += string(1, ' ');
          line += doub2for((double)datasources, 18, 2);
        }
      line += string(1, ' ');
      line += doub2for(wk, 18, 2);
      if ( satSys == "G" )      // GPS
        {
          line += string(1, ' ');
          line += doub2for((double)L2Pdata, 18, 2);
        }

      return line;
   }

   string Rinex3NavData::putBroadcastOrbit6(void) const
      throw(StringException)
   {
      string line;

      line += string(4, ' ');
      line += string(1, ' ');
      line += doub2for(accuracy, 18, 2);
      line += string(1, ' ');
      line += doub2for((double)health, 18, 2);

      if ( satSys == "G" )      // GPS
        {
          line += string(1, ' ');
          line += doub2for(Tgd, 18, 2);
          line += string(1, ' ');
          line += doub2for(IODC, 18, 2);
        }
      else if ( satSys == "E" ) // Galileo
        {
          line += string(1, ' ');
          line += doub2for(BGDa, 18, 2);
          line += string(1, ' ');
          line += doub2for(BGDb, 18, 2);
        }

      return line;
   }

   string Rinex3NavData::putBroadcastOrbit7(const double ver) const
      throw(StringException)
   {
      string line;

      line += string(4, ' ');
      line += string(1, ' ');
      line += doub2for(HOWtime, 18, 2);
      if ( satSys == "G" )
        {
          line += string(1, ' ');
          line += doub2for(fitint, 18, 2);         
        }

      return line;
   }

   void Rinex3NavData::getPRNEpoch(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
         // check for spaces in the right spots...
         if (currentLine[3] != ' ')
            throw( FFStreamError("Badly formatted line") );
         for (int i = 8; i <= 23; i += 3)
            if (currentLine[i] != ' ')
               throw( FFStreamError("Badly formatted line") );

         satSys = currentLine.substr(0,1);

         PRNID = asInt(currentLine.substr(1,2));

         short yr  = asInt(currentLine.substr( 4,4));
         short mo  = asInt(currentLine.substr( 7,2));
         short day = asInt(currentLine.substr(10,2));
         short hr  = asInt(currentLine.substr(13,2));
         short min = asInt(currentLine.substr(16,2));
         short sec = asInt(currentLine.substr(19,2));

         // Real RINEX 2 had epochs 'yy mm dd hr 59 60.0' surprisingly often.
         // Keep this in place (as Int) to be cautious.
         short ds = 0;
         if (sec >= 60) { ds=sec; sec=0; }
         time = CivilTime(yr,mo,day,hr,min,(double)sec).convertToCommonTime();
         if (ds != 0) time += ds;

         Toc = ((GPSWeekSecond)time).sow;

         af0 = gpstk::StringUtils::for2doub(currentLine.substr(23,19));
         af1 = gpstk::StringUtils::for2doub(currentLine.substr(42,19));
         af2 = gpstk::StringUtils::for2doub(currentLine.substr(61,19));
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " + string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void Rinex3NavData::getBroadcastOrbit1(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
         IODE = gpstk::StringUtils::for2doub(currentLine.substr( 4,19));
         Crs  = gpstk::StringUtils::for2doub(currentLine.substr(23,19));
         dn   = gpstk::StringUtils::for2doub(currentLine.substr(42,19));
         M0   = gpstk::StringUtils::for2doub(currentLine.substr(61,19));
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " + string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void Rinex3NavData::getBroadcastOrbit2(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
         Cuc   = gpstk::StringUtils::for2doub(currentLine.substr( 4,19));
         ecc   = gpstk::StringUtils::for2doub(currentLine.substr(23,19));
         Cus   = gpstk::StringUtils::for2doub(currentLine.substr(42,19));
         Ahalf = gpstk::StringUtils::for2doub(currentLine.substr(61,19));
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " + string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void Rinex3NavData::getBroadcastOrbit3(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
         Toe    = gpstk::StringUtils::for2doub(currentLine.substr( 4,19));
         Cic    = gpstk::StringUtils::for2doub(currentLine.substr(23,19));
         OMEGA0 = gpstk::StringUtils::for2doub(currentLine.substr(42,19));
         Cis    = gpstk::StringUtils::for2doub(currentLine.substr(61,19));
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " + string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void Rinex3NavData::getBroadcastOrbit4(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
         i0       = gpstk::StringUtils::for2doub(currentLine.substr( 4,19));
         Crc      = gpstk::StringUtils::for2doub(currentLine.substr(23,19));
         w        = gpstk::StringUtils::for2doub(currentLine.substr(42,19));
         OMEGAdot = gpstk::StringUtils::for2doub(currentLine.substr(61,19));
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " + string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void Rinex3NavData::getBroadcastOrbit5(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
         double codeL2, L2P, toe_wn;

         idot   = gpstk::StringUtils::for2doub(currentLine.substr( 4,19));
         codeL2 = gpstk::StringUtils::for2doub(currentLine.substr(23,19));
         toe_wn = gpstk::StringUtils::for2doub(currentLine.substr(42,19));
         L2P    = gpstk::StringUtils::for2doub(currentLine.substr(61,19));

         codeflgs = (short)codeL2;
         L2Pdata  = (short)L2P;
         weeknum  = (short)toe_wn;
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " + string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void Rinex3NavData::getBroadcastOrbit6(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
         double SV_acc, SV_health;

         accuracy  = gpstk::StringUtils::for2doub(currentLine.substr( 4,19));
         SV_health = gpstk::StringUtils::for2doub(currentLine.substr(23,19));
         Tgd       = gpstk::StringUtils::for2doub(currentLine.substr(42,19));
         IODC      = gpstk::StringUtils::for2doub(currentLine.substr(61,19));

         health = (short)SV_health;
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " + string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void Rinex3NavData::getBroadcastOrbit7(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
         double HOW_sec;

         HOW_sec = gpstk::StringUtils::for2doub(currentLine.substr( 4,19));
         fitint  = gpstk::StringUtils::for2doub(currentLine.substr(23,19));

         HOWtime = (long)HOW_sec;

         // In RINEX *files*, weeknum is the week of TOE.
         // Internally (Rinex3NavData and EngEphemeris), weeknum is the week of HOW.
         if (HOWtime - Toe > HALFWEEK)
            weeknum--;
         else if (HOWtime - Toe < -HALFWEEK)
            weeknum++;

         // Some RINEX files have HOW < 0.
         while(HOWtime < 0)
           {
             HOWtime += (long)FULLWEEK;
             weeknum--;
           }

      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " + string(e.what()));
         GPSTK_THROW(err);
      }
   }

}  // end of namespace
