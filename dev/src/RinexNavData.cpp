#pragma ident "$Id: //depot/sgl/gpstk/dev/src/RinexNavData.cpp#1 $"

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
 * @file RinexNavData.cpp
 * Encapsulates RINEX Navigation data
 */

#include "StringUtils.hpp"
#include "DayTime.hpp"
#include "RinexNavData.hpp"
#include "RinexNavStream.hpp"
#include "icd_200_constants.hpp"
 
namespace gpstk
{
   using namespace gpstk::StringUtils;
   using namespace std;

   RinexNavData::RinexNavData(const EngEphemeris& ee)
   {
      time = ee.getEpochTime();
      PRNID = ee.getPRNID();
      HOWtime = long(ee.getHOWTime(1));
      weeknum = ee.getFullWeek();
      codeflgs = ee.getCodeFlags();
      accuracy = ee.getAccuracy();
      health = ee.getHealth();
      L2Pdata = ee.getL2Pdata();
      IODC = ee.getIODC();
      IODE = ee.getIODE();

      Toc = ee.getToc();
      af0 = ee.getAf0();
      af1 = ee.getAf1();
      af2 = ee.getAf2();
      Tgd = ee.getTgd();

      Cuc = ee.getCuc();
      Cus = ee.getCus();
      Crc = ee.getCrc();
      Crs = ee.getCrs();
      Cic = ee.getCic();
      Cis = ee.getCis();

      Toe = ee.getToe();
      M0 = ee.getM0();
      dn = ee.getDn();
      ecc = ee.getEcc();
      Ahalf = ee.getAhalf();
      OMEGA0 = ee.getOmega0();
      i0 = ee.getI0();
      w = ee.getW();
      OMEGAdot = ee.getOmegaDot();
      idot = ee.getIDot();
      fitint = ee.getFitInterval();
   }

   void RinexNavData::reallyPutRecord(FFStream& ffs) const 
      throw(exception, FFStreamError, StringException)
   {
      RinexNavStream& strm = dynamic_cast<RinexNavStream&>(ffs);
      
      strm << putPRNEpoch() << endl;
      strm.lineNumber++;
      strm << putBroadcastOrbit1() << endl;
      strm.lineNumber++;
      strm << putBroadcastOrbit2() << endl;
      strm.lineNumber++;
      strm << putBroadcastOrbit3() << endl;
      strm.lineNumber++;
      strm << putBroadcastOrbit4() << endl;
      strm.lineNumber++;
      strm << putBroadcastOrbit5() << endl;
      strm.lineNumber++;
      strm << putBroadcastOrbit6() << endl;
      strm.lineNumber++;
      strm << putBroadcastOrbit7(strm.header.version) << endl;
      strm.lineNumber++;
   }

   void RinexNavData::reallyGetRecord(FFStream& ffs) 
      throw(exception, FFStreamError, StringException)
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

   void RinexNavData::dump(ostream& s) const
   {
      s << "PRN: " << setw(2) << PRNID
        << " TOE: " << time
        << " TOC: " << setw(4) << weeknum << " " 
        << fixed << setw(10) << setprecision(3) << Toc
        << " IODE: " << setw(4) << int(IODE)            // IODE should be int
        << " HOWtime: " << setw(6) << HOWtime           // HOW should be double
        << endl;
        //<< ios::hex << IODE << " HOWtime: " << HOWtime << endl; ?? IODE is double
   }

   RinexNavData::operator EngEphemeris() const throw()
   {
      EngEphemeris ee;

         // there's no TLM word in RinexNavData, so it's set to 0.
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

   list<double> RinexNavData::toList() const
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

   string RinexNavData::putPRNEpoch(void) const
      throw(StringException)
   {
      string line;
      line += rightJustify(asString(PRNID), 2);
      line += string(1, ' ');
         // year is padded with 0s but none of the rest are
      line += rightJustify(asString<short>(time.year()), 2, '0');
      line += string(1, ' ');
      line += rightJustify(asString<short>(time.month()), 2);
      line += string(1, ' ');
      line += rightJustify(asString<short>(time.day()), 2);
      line += string(1, ' ');
      line += rightJustify(asString<short>(time.hour()), 2);
      line += string(1, ' ');
      line += rightJustify(asString<short>(time.minute()), 2);
      line += rightJustify(asString(time.second(), 1), 5);
      line += string(1, ' ');
      line += doub2for(af0, 18, 2);
      line += string(1, ' ');
      line += doub2for(af1, 18, 2);
      line += string(1, ' ');
      line += doub2for(af2, 18, 2);
      return line;
   }

   string RinexNavData::putBroadcastOrbit1(void) const
      throw(StringException)
   {
      string line;
      line += string(3, ' ');
      line += string(1, ' ');
      line += doub2for(IODE, 18, 2);
      line += string(1, ' ');
      line += doub2for(Crs, 18, 2);
      line += string(1, ' ');
      line += doub2for(dn, 18, 2);
      line += string(1, ' ');
      line += doub2for(M0, 18, 2);
      return line;
   }

   string RinexNavData::putBroadcastOrbit2(void) const
      throw(StringException)
   {
      string line;
      line += string(3, ' ');
      line += string(1, ' ');
      line += doub2for(Cuc, 18, 2);
      line += string(1, ' ');
      line += doub2for(ecc, 18, 2);
      line += string(1, ' ');
      line += doub2for(Cus, 18, 2);
      line += string(1, ' ');
      line += doub2for(Ahalf, 18, 2);
      return line;
   }

   string RinexNavData::putBroadcastOrbit3(void) const
      throw(StringException)
   {
      string line;
      line += string(3, ' ');
      line += string(1, ' ');
      line += doub2for(Toe, 18, 2);
      line += string(1, ' ');
      line += doub2for(Cic, 18, 2);
      line += string(1, ' ');
      line += doub2for(OMEGA0, 18, 2);
      line += string(1, ' ');
      line += doub2for(Cis, 18, 2);
      return line;
   }

   string RinexNavData::putBroadcastOrbit4(void) const
      throw(StringException)
   {
      string line;
      line += string(3, ' ');
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

   string RinexNavData::putBroadcastOrbit5(void) const
      throw(StringException)
   {
         // Internally (RinexNavData and EngEphemeris), weeknum is the week of HOW
         // In Rinex *files*, weeknum is the week of TOE
      double wk=double(weeknum);
      if(HOWtime - Toe > DayTime::HALFWEEK)
         wk++;
      else if(HOWtime - Toe < -(DayTime::HALFWEEK))
         wk--;

      string line;
      line += string(3, ' ');
      line += string(1, ' ');
      line += doub2for(idot, 18, 2);
      line += string(1, ' ');
      line += doub2for((double)codeflgs, 18, 2);
      line += string(1, ' ');
      line += doub2for(wk, 18, 2);
      line += string(1, ' ');
      line += doub2for((double)L2Pdata, 18, 2);
      return line;
   }

   string RinexNavData::putBroadcastOrbit6(void) const
      throw(StringException)
   {
      string line;
      line += string(3, ' ');
      line += string(1, ' ');
      line += doub2for(accuracy, 18, 2);
      line += string(1, ' ');
      line += doub2for((double)health, 18, 2);
      line += string(1, ' ');
      line += doub2for(Tgd, 18, 2);
      line += string(1, ' ');
      line += doub2for(IODC, 18, 2);
      return line;
   }

   string RinexNavData::putBroadcastOrbit7(const double ver) const
      throw(StringException)
   {
      string line;
      line += string(3, ' ');
      line += string(1, ' ');
      line += doub2for(HOWtime, 18, 2);

      if (ver >= 2.1)
      {
         line += string(1, ' ');
         line += doub2for(fitint, 18, 2);         
      }
      return line;
   }

   void RinexNavData::getPRNEpoch(const string& currentLine)
      throw(StringException, FFStreamError)
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

         // Real Rinex has epochs 'yy mm dd hr 59 60.0' surprisingly often....
         double ds=0;
         if(sec >= 60.) { ds=sec; sec=0.0; }
         time = DayTime(yr,mo,day,hr,min,sec);
         if(ds != 0) time += ds;
         
         Toc = time.GPSsecond();
         af0 = gpstk::StringUtils::for2doub(currentLine.substr(22,19),19);
         af1 = gpstk::StringUtils::for2doub(currentLine.substr(41,19),19);
         af2 = gpstk::StringUtils::for2doub(currentLine.substr(60,19),19);
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void RinexNavData::getBroadcastOrbit1(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
         IODE = gpstk::StringUtils::for2doub(currentLine.substr(3,19),19);
         Crs = gpstk::StringUtils::for2doub(currentLine.substr(22,19),19);
         dn = gpstk::StringUtils::for2doub(currentLine.substr(41,19),19);
         M0 = gpstk::StringUtils::for2doub(currentLine.substr(60,19),19);
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void RinexNavData::getBroadcastOrbit2(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
         Cuc = gpstk::StringUtils::for2doub(currentLine.substr(3,19),19);
         ecc = gpstk::StringUtils::for2doub(currentLine.substr(22,19),19);
         Cus = gpstk::StringUtils::for2doub(currentLine.substr(41,19),19);
         Ahalf = gpstk::StringUtils::for2doub(currentLine.substr(60,19),19);
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void RinexNavData::getBroadcastOrbit3(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
         Toe = gpstk::StringUtils::for2doub(currentLine.substr(3,19),19);
         Cic = gpstk::StringUtils::for2doub(currentLine.substr(22,19),19);
         OMEGA0 = gpstk::StringUtils::for2doub(currentLine.substr(41,19),19);
         Cis = gpstk::StringUtils::for2doub(currentLine.substr(60,19),19);
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void RinexNavData::getBroadcastOrbit4(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
         i0 = gpstk::StringUtils::for2doub(currentLine.substr(3,19),19);
         Crc = gpstk::StringUtils::for2doub(currentLine.substr(22,19),19);
         w = gpstk::StringUtils::for2doub(currentLine.substr(41,19),19);
         OMEGAdot = gpstk::StringUtils::for2doub(currentLine.substr(60,19),19);
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void RinexNavData::getBroadcastOrbit5(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
         double codeL2, L2P, toe_wn;

         idot = gpstk::StringUtils::for2doub(currentLine.substr(3,19),19);
         codeL2 = gpstk::StringUtils::for2doub(currentLine.substr(22,19),19);
         toe_wn = gpstk::StringUtils::for2doub(currentLine.substr(41,19),19);
         L2P = gpstk::StringUtils::for2doub(currentLine.substr(60,19),19);

         codeflgs = (short) codeL2;
         L2Pdata = (short) L2P;
         weeknum = (short) toe_wn;
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void RinexNavData::getBroadcastOrbit6(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
         double SV_acc, SV_health;

         accuracy = gpstk::StringUtils::for2doub(currentLine.substr(3,19),19);
         SV_health = gpstk::StringUtils::for2doub(currentLine.substr(22,19),19);
         Tgd = gpstk::StringUtils::for2doub(currentLine.substr(41,19),19);
         IODC = gpstk::StringUtils::for2doub(currentLine.substr(60,19),19);

   
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
      throw(StringException, FFStreamError)
   {
      try
      {
         double HOW_sec;

         HOW_sec = gpstk::StringUtils::for2doub(currentLine.substr(3,19),19);
         fitint = gpstk::StringUtils::for2doub(currentLine.substr(22,19),19);
   
         HOWtime = (long) HOW_sec;

         // In Rinex *files*, weeknum is the week of TOE
         // Internally (RinexNavData and EngEphemeris), weeknum is the week of HOW
         if(HOWtime - Toe > DayTime::HALFWEEK)
            weeknum--;
         else if(HOWtime - Toe < -(DayTime::HALFWEEK))
            weeknum++;

         // Some Rinex files have HOW < 0
         while(HOWtime < 0) {
	   HOWtime += (long) DayTime::FULLWEEK;
            weeknum--;
         }
         
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }

}  // end of namespace
