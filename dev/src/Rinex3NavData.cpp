#pragma ident "$Id$"

/**
 * @file Rinex3NavData.cpp
 * Encapsulates RINEX 3 Navigation data.
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

#include "CivilTime.hpp"
#include "TimeString.hpp"
//#include "GPSWeekSecond.hpp"
#include "GNSSconstants.hpp"

#include "Rinex3NavData.hpp"
#include "OrbElemRinex.hpp"


namespace gpstk
{
   using namespace StringUtils;
   using namespace std;


      // This routine uses EngEphemeris, so is for GPS data only.
      // The comments about GPS v. Galileo next to each elements are just notes
      // from sorting out the ICDs in the RINEX 3 documentation. Please leave
      // them there until we add a routine for handling GalRecord or similar.
   Rinex3NavData::Rinex3NavData(const EngEphemeris& ee) // GPS only
   {
      // epoch info

      satSys = ee.getSatSys();
      PRNID  = ee.getPRNID();
      sat    = RinexSatID(PRNID,SatID::systemGPS);
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

//      IODnav = ee.getIODnav(); // Galileo only

//      datasources = ee.getDatasources(); // Galileo only

//      BGDa = ee.getBGDa(); // Galileo only
//      BGDb = ee.getBGDb(); // Galileo only

   }  // End of 'Rinex3NavData::Rinex3NavData(const EngEphemeris& ee)'

   Rinex3NavData::Rinex3NavData(const OrbElem* oeb) // GPS only
   {
      // epoch info

     
      // This throws a bad_cast exception if oe isn't really a OrbElemRinex
      const OrbElemRinex* oe = dynamic_cast<const OrbElemRinex*>(oeb);    
      // Use oer to initialize *this

      PRNID  = oe->satID.id;
      sat    = RinexSatID(PRNID,SatID::systemGPS);
      time   = oe->beginValid;
      satSys = "G";

      Toc     = (static_cast<GPSWeekSecond>(oe->ctToc)).sow;
      HOWtime = long(oe->HOWtime);
      weeknum = (static_cast<GPSWeekSecond>(oe->transmitTime)).week;

      accuracy = oe->accuracyValue;
      health   = oe->health;

      // GPS or Galileo data

      af0 = oe->af0; // GPS and Galileo only
      af1 = oe->af1; // GPS and Galileo only
      af2 = oe->af2; // GPS and Galileo only

      Crs = oe->Crs; // GPS and Galileo only
      dn  = oe->dn;  // GPS and Galileo only
      M0  = oe->M0;  // GPS and Galileo only

      Cuc   = oe->Cuc;   // GPS and Galileo only
      ecc   = oe->ecc;   // GPS and Galileo only
      Cus   = oe->Cus;   // GPS and Galileo only
      Ahalf = SQRT(oe->A); // GPS and Galileo only

      Toe    = (static_cast<GPSWeekSecond>(oe->ctToe)).sow;    // GPS and Galileo only
      Cic    = oe->Cic;    // GPS and Galileo only
      OMEGA0 = oe->OMEGA0; // GPS and Galileo only
      Cis    = oe->Cis;    // GPS and Galileo only

      i0       = oe->i0;       // GPS and Galileo only
      Crc      = oe->Crc;      // GPS and Galileo only
      w        = oe->w;        // GPS and Galileo only
      OMEGAdot = oe->OMEGAdot; // GPS and Galileo only

      idot = oe->idot; // GPS and Galileo only

      // GPS-only data

     // IODE = oe->getIODE(); // GPS only

      codeflgs = oe->codeflags; // GPS only
      L2Pdata  = oe->L2Pdata;   // GPS only

      Tgd  = oe->Tgd;  // GPS only
      IODC = oe->IODC; // GPS only

      fitint = oe->fitDuration; // GPS only

      // Galileo-only data

//      IODnav = ee.getIODnav(); // Galileo only

//      datasources = ee.getDatasources(); // Galileo only

//      BGDa = ee.getBGDa(); // Galileo only
//      BGDb = ee.getBGDb(); // Galileo only

  }  // End of 'Rinex3NavData::Rinex3NavData(const OrbElemRinex* oe)'



      // This constructor initializes R3NavData with Galileo data.
      // Refer to previous constructor, Rinex3NavData(const EngEphemeris& ee),
      // for more information on which elements are GPS and/or Galileo only.
   Rinex3NavData::Rinex3NavData(const GalEphemeris& ge)
   {
      // epoch info

      satSys = ge.getSatSys();
      PRNID  = ge.getPRNID();
      sat    = RinexSatID(PRNID,SatID::systemGalileo);
      time   = ge.getEpochTime();

      Toc     = ge.getToc();
      HOWtime = long(ge.getHOWTime(1));
      weeknum = ge.getFullWeek();

      accuracy = ge.getAccuracy();
      health   = ge.getHealth();

      // GPS or Galileo data

      af0 = ge.getAf0(); // GPS and Galileo only
      af1 = ge.getAf1(); // GPS and Galileo only
      af2 = ge.getAf2(); // GPS and Galileo only

      Crs = ge.getCrs(); // GPS and Galileo only
      dn  = ge.getDn();  // GPS and Galileo only
      M0  = ge.getM0();  // GPS and Galileo only

      Cuc   = ge.getCuc();   // GPS and Galileo only
      ecc   = ge.getEcc();   // GPS and Galileo only
      Cus   = ge.getCus();   // GPS and Galileo only
      Ahalf = ge.getAhalf(); // GPS and Galileo only

      Toe    = ge.getToe();    // GPS and Galileo only
      Cic    = ge.getCic();    // GPS and Galileo only
      OMEGA0 = ge.getOmega0(); // GPS and Galileo only
      Cis    = ge.getCis();    // GPS and Galileo only

      i0       = ge.getI0();       // GPS and Galileo only
      Crc      = ge.getCrc();      // GPS and Galileo only
      w        = ge.getW();        // GPS and Galileo only
      OMEGAdot = ge.getOmegaDot(); // GPS and Galileo only

      idot = ge.getIDot(); // GPS and Galileo only

      // Galileo-only data

      IODnav = ge.getIODnav(); // Galileo only

      datasources = ge.getDatasources(); // Galileo only

      BGDa = ge.getBGDa(); // Galileo only
      BGDb = ge.getBGDb(); // Galileo only

   }  // End of 'Rinex3NavData::Rinex3NavData(const GalEphemeris& ge)'


      // This constructor initializes R3NavData with Glonass data.
   Rinex3NavData::Rinex3NavData(const GloEphemeris& gloe)
   {

         // Epoch info
      satSys = gloe.getSatSys();
      PRNID  = gloe.getPRNID();
      sat    = RinexSatID(PRNID,SatID::systemGlonass);
      time   = gloe.getEpochTime();

         // GLONASS parameters
      TauN = gloe.getTauN();
      GammaN = gloe.getGammaN();
      MFtime = gloe.getMFtime();
      health = gloe.getHealth();
      freqNum = gloe.getfreqNum();
      ageOfInfo = gloe.getAgeOfInfo();

      Triple x( gloe.x );
      px = x[0];
      py = x[1];
      pz = x[2];

      Triple v( gloe.v );
      vx = v[0];
      vy = v[1];
      vz = v[2];

      Triple a( gloe.getAcc() );
      ax = a[0];
      ay = a[1];
      az = a[2];

   }  // End of 'Rinex3NavData::Rinex3NavData(const GloEphemeris& ge)'


      /* This function retrieves a RINEX 3 NAV record from the given
       *  FFStream.
       *  If an error is encountered in reading from the stream, the stream
       *  is returned to its original position and its fail-bit is set.
       *  @throws StringException when a StringUtils function fails.
       *  @throws FFStreamError when exceptions(failbit) is set and a read
       *          or formatting error occurs. This also resets the stream
       *          to its pre-read position.
       */
   void Rinex3NavData::reallyGetRecord(FFStream& ffs)
      throw(exception, FFStreamError, StringException)
   {

      try {
         Rinex3NavStream& strm = dynamic_cast<Rinex3NavStream&>(ffs);

         // If the header hasn't been read, read it...
         if(!strm.headerRead) {
            try {
               strm >> strm.header;
            }
            catch(exception& e) {
               FFStreamError fse(string("std::exception reading header ") + e.what());
               GPSTK_THROW(fse);
            }
            catch(FFStreamError& fse) { GPSTK_RETHROW(fse); }
         }

         // get the first line, the epoch line
         getPRNEpoch(strm);

         // get 3 data records
         for(int i=1; i<=3; i++) getRecord(i, strm);

         // SBAS and GLO only have 3 records
         if(satSys == "S" || satSys == "R") return;

         // COMPASS satSys == "C" TBD

         // GPS and GAL have 7 records, get 4-7
         if(satSys == "G" || satSys == "E")
            for(int i=4; i<=7; i++) getRecord(i, strm);
      }
      catch(exception& e) {
         FFStreamError fse(string("std::exception: ") + e.what());
         GPSTK_THROW(fse);
      }
      catch(FFStreamError& fse) { GPSTK_RETHROW(fse); }
      catch(StringException& se) { GPSTK_RETHROW(se); }

   }  // End of method 'Rinex3NavData::reallyGetRecord(FFStream& ffs)'


      // Outputs the record to the FFStream \a s.
   void Rinex3NavData::reallyPutRecord(FFStream& ffs) const
      throw(exception, FFStreamError, StringException)
   {

      try {
         Rinex3NavStream& strm = dynamic_cast<Rinex3NavStream&>(ffs);

         putPRNEpoch(strm);

         // put 3 data records
         for(int i=1; i<=3; i++) putRecord(i, strm);

         // SBAS and GLO only have 3 records
         if(satSys == "S" || satSys == "R") return;

         // COMPASS satSys == "C" TBD

         // GPS and GAL have 7 records, put 4-7
         if(satSys == "G" || satSys == "E")
            for(int i=4; i<=7; i++) putRecord(i, strm);
      }
      catch(exception& e) {
         FFStreamError fse(string("std::exception: ") + e.what());
         GPSTK_THROW(fse);
      }
      catch(FFStreamError& fse) { GPSTK_RETHROW(fse); }
      catch(StringException& se) { GPSTK_RETHROW(se); }

   }  // End of method 'Rinex3NavData::reallyPutRecord(FFStream& ffs)'


      // A debug output function.
      // Prints the PRN id and the IODC for this record.
   void Rinex3NavData::dump(ostream& s) const
   {

      if(satSys == "G")          // GPS
         s << "Sat: " << satSys << setfill('0') << setw(2)
           << PRNID << setfill(' ')
           << " TOE: " << setw(4) << weeknum
           << " " << fixed << setw(10) << setprecision(3) << Toe
           << " TOC: " << printTime(time,"%4F %10.3g")
           << " codeflags: " << setw(3) << codeflgs
           << " L2Pflag: " << setw(3) << L2Pdata
           << " IODC: " << setw(4) << int(IODC)
           << " IODE: " << setw(4) << int(IODE)      // IODE should be int
           << " HOWtime: " << setw(6) << HOWtime     // HOW should be double
           << " FitInt: " << setw(6) << fitint
           << endl;
      else if(satSys == "R")     // GLONASS
         s << "Sat: " << satSys << setfill('0') << setw(2)
           << PRNID << setfill(' ')
           << " freq: " << setw(2) << freqNum
           << " hlth: " << setw(2) << health
           << " " << printTime(time,"%4Y %02m %02d %02H %02M %06.3f")
           << " MFtime: " << setw(6) << MFtime
           << " TauN: " << scientific << setw(19) << setprecision(12) << TauN
           << " GammaN: " << setw(19) << GammaN
           << " AOI: " << fixed << setprecision(2) << setw(4) << ageOfInfo
           << endl;
      else if(satSys == "S")     // Geosync (SBAS)
         s << "Sat: " << satSys << setfill('0') << setw(2)
            << PRNID << setfill(' ')
           << " URAm: " << setw(2) << freqNum
           << " hlth: " << setw(2) << health
           << " " << printTime(time,"%4Y %02m %02d %02H %02M %06.3f")
           << " MFtime: " << setw(6) << MFtime
           << " aGf0: " << scientific << setw(19) << setprecision(12) << TauN
           << " aGf1: " << setw(19) << GammaN
           << " IODN " << fixed << setprecision(2) << setw(4) << ageOfInfo
           << endl;
      //else if(satSys == "E")   // Galileo
      //else if(satSys == "C")   // Compass
      else
         s << "Sat: "<< satSys << setfill('0') << setw(2)
           << PRNID << setfill(' ')
           << " (unknown system: " << satSys << ")" << endl;

   }  // End of method 'Rinex3NavData::dump(ostream& s)'


// This is the old converter from Rinex3NavData to EngEphemeris. The new one is below
      // Converts this Rinex3NavData to an EngEphemeris object.
/*    Rinex3NavData::operator EngEphemeris() const throw()
 *  {
 *
 *     EngEphemeris ee;
 *
 *     // There's no TLM word in Rinex3NavData, so it's set to 0.
 *     // Likewise, there's no AS alert or tracker.
 *    // Also, in RINEX, the accuracy is in meters, and setSF1 expects
 *     // the accuracy flag.  We'll give it zero and pass the accuracy
 *    // separately via the setAccuracy() method.
 *
 *     ee.setSF1(0, HOWtime, 1, weeknum, codeflgs, 0, health,
 *               short(IODC), L2Pdata, Tgd, Toc, af2, af1, af0, 0, PRNID);
 *
 *     ee.setSF2(0, HOWtime, 1, short(IODE), Crs, dn, M0, Cuc, ecc, Cus, Ahalf,
 *               Toc, (fitint > 4) ? 1 : 0);
 *     ee.setSF3(0, HOWtime, 1, Cic, OMEGA0, Cis, i0, Crc, w, OMEGAdot,
 *               idot);
 *
 *     ee.setAccuracy(accuracy);
 *
 *     return ee;
 *
 *  }  // End of 'Rinex3NavData::operator EngEphemeris()'
 */

      // Converts this Rinex3NavData to an EngEphemeris object.
   Rinex3NavData::operator EngEphemeris() const throw()
   {

      EngEphemeris ee;

      // There's no TLM word in Rinex3NavData, so it's set to 0.
      // Likewise, there's no AS alert or tracker.
      // Also, in RINEX, the accuracy is in meters, and setSF1 expects
      // the accuracy flag.  We'll give it zero and pass the accuracy
      // separately via the setAccuracy() method.

      ee.tlm_message[0] = 0;           
      ee.tlm_message[1] = 0;
      ee.tlm_message[2] = 0;
      ee.HOWtime[0] = HOWtime;  // RINEX does not actually specify 
      ee.HOWtime[1] = HOWtime;  // how the transmit time is derived.  Therefore,
      ee.HOWtime[2] = HOWtime;  // These values may be misleading.  
      ee.ASalert[0] = 1;               //AS and alert flags set to 1 (default)
      ee.ASalert[1] = 1;
      ee.ASalert[2] = 1;

      ee.weeknum    = weeknum;
      ee.codeflags  = codeflgs;
      ee.health     = health;  
      ee.IODC       = short(IODC);
      ee.L2Pdata    = L2Pdata;
      ee.Tgd        = Tgd;
      ee.tracker    = 0;
      ee.PRNID      = PRNID;
      ee.satSys     = satSys;
      bool healthy = false;
      if (health == 0) healthy = true;
      short accFlag = 0; //will be set later. BrcClockCorrection takes a flag, while EngEphemeris takes a double.
      double toc    = Toc;

      double timeDiff =toc - ee.HOWtime[0];
      short epochWeek = ee.weeknum;
      if (timeDiff < -HALFWEEK) epochWeek++;
      else if (timeDiff > HALFWEEK) epochWeek--;

      CommonTime tocCT = GPSWeekSecond(epochWeek, Toc, TimeSystem::GPS);

         // The observation ID has a type of navigation, but the
         // carrier and code types are undefined.  They could be
         // L1/L2 C/A, P, Y,.....
      ObsID obsID(ObsID::otNavMsg, ObsID::cbUndefined, ObsID::tcUndefined);
      ee.bcClock.loadData( satSys, obsID, PRNID, tocCT,
                        accFlag, healthy, af0, af1, af2);

      ee.IODE    = short(IODE);      
      ee.fitint  = (fitint > 4) ? 1 : 0;
      double toe = Toe; //?????
      
      //Needed for modernized nav quatities
      double A = Ahalf * Ahalf;
      double dndot = 0.0;
      double Adot = 0.0;

      short fitHours = getLegacyFitInterval(ee.IODC, ee.fitint);
      long beginFitSOW = Toe - (fitHours/2)*3600.0;
      long endFitSOW = Toe + (fitHours/2)*3600.0;
      short beginFitWk = ee.weeknum;
      short endFitWk = ee.weeknum;

      if (beginFitSOW < 0)
      {
         beginFitSOW += FULLWEEK;
         beginFitWk--;
      }
      CommonTime beginFit = GPSWeekSecond(beginFitWk, beginFitSOW, TimeSystem::GPS);
      if (endFitSOW >= FULLWEEK)
      {
         endFitSOW += FULLWEEK;
         endFitWk++;
      }

      CommonTime endFit = GPSWeekSecond(endFitWk, endFitSOW, TimeSystem::GPS);
      CommonTime toeCT = GPSWeekSecond(epochWeek, Toe, TimeSystem::GPS);

      ee.orbit.loadData( satSys, obsID, PRNID, beginFit, endFit, toeCT,
                      accFlag, healthy, Cuc, Cus, Crc, Crs, Cic, Cis, 
                      M0, dn, dndot, ecc, A, Ahalf, Adot, OMEGA0, i0, 
                      w, OMEGAdot, idot);

      ee.haveSubframe[0] = true;    //need to be true to perform certain EngEphemeris functions
      ee.haveSubframe[1] = true;    // examples: ee.dump(), ee.setAccuracy()
      ee.haveSubframe[2] = true;

      ee.setAccuracy(accuracy);

      return ee;

   }  // End of 'Rinex3NavData::operator EngEphemeris()'


      // Converts this Rinex3NavData to a GalEphemeris object.
   Rinex3NavData::operator GalEphemeris() const throw()
   {

      GalEphemeris ge;

      // There's no TLM word in Rinex3NavData, so it's set to 0.
      // Likewise, there's no AS alert or tracker.
      // Also, in RINEX, the accuracy is in meters, and setSF1 expects
      // the accuracy flag.  We'll give it zero and pass the accuracy
      // separately via the setAccuracy() method.

      ge.setSF1(0, HOWtime, 0, weeknum, datasources, 0, health,
                BGDb, L2Pdata, BGDa, Toc, af2, af1, af0, 0, PRNID);
      ge.setSF2(0, HOWtime, 0, short(IODnav), Crs, dn, M0, Cuc, ecc, Cus, Ahalf,
                Toc, 0);
      ge.setSF3(0, HOWtime, 0, Cic, OMEGA0, Cis, i0, Crc, w, OMEGAdot,
                idot);

      ge.setAccuracy(accuracy);

      return ge;

   }  // End of 'Rinex3NavData::operator GalEphemeris()'


      // Converts this Rinex3NavData to a GloEphemeris object.
   Rinex3NavData::operator GloEphemeris() const throw()
   {

      GloEphemeris gloe;

      gloe.setRecord( satSys,
                      PRNID,
                      time,
                      Triple(px, py, pz),
                      Triple(vx, vy, vz),
                      Triple(ax, ay, az),
                      TauN,
                      GammaN,
                      MFtime,
                      health,
                      freqNum,
                      ageOfInfo );

      return gloe;

   }  // End of 'Rinex3NavData::operator GloEphemeris()'


      // Converts the (non-CommonTime) data to an easy list
      // for comparison operators.
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
      l.push_back(Toc);
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

   }  // End of method 'Rinex3NavData::toList()'


      /* Generates the PRN/epoch line and outputs it to strm
       *  @param strm RINEX Nav stream
       */
   void Rinex3NavData::putPRNEpoch(Rinex3NavStream& strm) const
      throw(StringException)
   {
      string line;
      CivilTime civtime(time);

      if(strm.header.version >= 3) {                                 // version 3
         line = sat.toString();
         line += " ";
         line += rightJustify(asString<short>(civtime.year), 4);
         line += " ";
         line += rightJustify(asString<short>(civtime.month), 2, '0');
         line += " ";
         line += rightJustify(asString<short>(civtime.day), 2, '0');
         line += " ";
         line += rightJustify(asString<short>(civtime.hour), 2, '0');
         line += " ";
         line += rightJustify(asString<short>(civtime.minute), 2, '0');
         line += " ";
         line += rightJustify(asString<short>(civtime.second), 2, '0');
      }
      else {                                                         // version 2
         line = rightJustify(asString<short>(PRNID), 2);
         line += " ";
         line += rightJustify(asString<short>(civtime.year), 2, '0');
         line += " ";
         line += rightJustify(asString<short>(civtime.month), 2);
         line += " ";
         line += rightJustify(asString<short>(civtime.day), 2);
         line += " ";
         line += rightJustify(asString<short>(civtime.hour), 2);
         line += " ";
         line += rightJustify(asString<short>(civtime.minute), 2);
         line += " ";
         line += rightJustify(asString(civtime.second,1), 4);
      }

      if(satSys == "R" || satSys == "S") {
         line += doubleToScientific(TauN,19,12,2);
         line += doubleToScientific(GammaN,19,12,2);
         line += doubleToScientific((double)MFtime,19,12,2);
      }
      else if(satSys == "G" || satSys == "E") {
         line += doubleToScientific(af0,19,12,2);
         line += doubleToScientific(af1,19,12,2);
         line += doubleToScientific(af2,19,12,2);
      }
      //else if(satSys == "C")

      strm << stripTrailing(line) << endl;
      strm.lineNumber++;

   }  // End of 'Rinex3NavData::putPRNEpoch(Rinex3NavStream& strm)'


      /* Construct and write the nth record after the epoch record
       *  @param int n                 Record number (1-7), for nth record
       *                               after the epoch line.
       *  @param Rinex3NavStream strm  Stream to read from.
       */
   void Rinex3NavData::putRecord(const int& nline, Rinex3NavStream& strm) const
      throw(StringException, FFStreamError)
   {

      if(nline < 1 || nline > 7) {
         FFStreamError fse(string("Invalid line number ") + asString(nline));
         GPSTK_THROW(fse);
      }

      try {
         string line;

         if(strm.header.version < 3) line += string(3, ' ');
         else                        line += string(4, ' ');

         if(nline == 1) {
            if(satSys == "R" || satSys == "S") {     // GLO and GEO
               line += doubleToScientific(px,19,12,2);
               line += doubleToScientific(vx,19,12,2);
               line += doubleToScientific(ax,19,12,2);
               line += doubleToScientific((double)health,19,12,2);
            }
            else if(satSys == "G") {                  // GPS
               line += doubleToScientific(IODE,19,12,2);
               line += doubleToScientific(Crs,19,12,2);
               line += doubleToScientific(dn,19,12,2);
               line += doubleToScientific(M0,19,12,2);
            }
            else if(satSys == "E") {                  // GAL
               line += doubleToScientific(IODnav,19,12,2);
               line += doubleToScientific(Crs,19,12,2);
               line += doubleToScientific(dn,19,12,2);
               line += doubleToScientific(M0,19,12,2);
            }
         }

         else if(nline == 2) {
            if(satSys == "R" || satSys == "S") {      // GLO and GEO
               line += doubleToScientific(py,19,12,2);
               line += doubleToScientific(vy,19,12,2);
               line += doubleToScientific(ay,19,12,2);
               if(satSys == "R")
                  line += doubleToScientific((double)freqNum,19,12,2);
               else
                  line += doubleToScientific(accCode,19,12,2);
            }
            else {
               line += doubleToScientific(Cuc,19,12,2);
               line += doubleToScientific(ecc,19,12,2);
               line += doubleToScientific(Cus,19,12,2);
               line += doubleToScientific(Ahalf,19,12,2);
            }
         }

         else if(nline == 3) {
            if(satSys == "R" || satSys == "S") {
               line += doubleToScientific(pz,19,12,2);
               line += doubleToScientific(vz,19,12,2);
               line += doubleToScientific(az,19,12,2);
               if(satSys == "R")
                  line += doubleToScientific(ageOfInfo,19,12,2);
               else                    // GEO
                  line += doubleToScientific(IODN,19,12,2);
            }
            else {
               line += doubleToScientific(Toe,19,12,2);
               line += doubleToScientific(Cic,19,12,2);
               line += doubleToScientific(OMEGA0,19,12,2);
               line += doubleToScientific(Cis,19,12,2);
            }
         }

         else if(nline == 4) {
            line += doubleToScientific(i0,19,12,2);
            line += doubleToScientific(Crc,19,12,2);
            line += doubleToScientific(w,19,12,2);
            line += doubleToScientific(OMEGAdot,19,12,2);
         }

         else if(nline == 5) {
            // Internally (Rinex3NavData and EngEphemeris), weeknum=week of HOW
            // In RINEX 3 *files*, weeknum is the week of TOE.
            double wk = double(weeknum);
            if(HOWtime - Toe > HALFWEEK)
               wk++;
            else if(HOWtime - Toe < -(HALFWEEK))
               wk--;

            line += doubleToScientific(idot,19,12,2);
            if(satSys == "G") {                         // GPS
               line += doubleToScientific((double)codeflgs,19,12,2);
            }
            else if(satSys == "E") {                    // GAL
               line += doubleToScientific((double)datasources,19,12,2);
            }

            line += doubleToScientific(wk,19,12,2);
            if(satSys == "G") {                         // GPS
               line += doubleToScientific((double)L2Pdata,19,12,2);
            }
            else if(satSys == "E") {                     // GAL
               line += doubleToScientific((double) 0,19,12,2);
            }
         }

         else if(nline == 6) {
            line += doubleToScientific(accuracy,19,12,2);
            line += doubleToScientific((double)health,19,12,2);

            if(satSys == "G") {                        // GPS
               line += doubleToScientific(Tgd,19,12,2);
               line += doubleToScientific(IODC,19,12,2);
            }
            else if(satSys == "E") {                   // GAL
               line += doubleToScientific(BGDa,19,12,2);
               line += doubleToScientific(BGDb,19,12,2);
            }
         }

         else if(nline == 7) {
            line += doubleToScientific(HOWtime,19,12,2);

            if(satSys == "G") {
               line += doubleToScientific(fitint,19,12,2);
            }
            else if(satSys == "E") {
               line += doubleToScientific(0.0,19,12,2);
            }
         }

         strm << stripTrailing(line) << endl;
         strm.lineNumber++;
      }
      catch (std::exception &e) {
         FFStreamError err("std::exception: " + string(e.what()));
         GPSTK_THROW(err);
      }

   }  // End of method 'Rinex3NavData::putRecord(const int& nline,...'


   void Rinex3NavData::getPRNEpoch(Rinex3NavStream& strm)
      throw(StringException, FFStreamError)
   {
      try {
         int i;
         short yr,mo,day,hr,min;
         double dsec;

         string line;
         while(line.empty())        // ignore blank lines in place of epoch lines
            strm.formattedGetLine(line, true);

         if(strm.header.version >= 3) {
            // check for spaces in the right spots...
            if(line[3] != ' ')
               throw(FFStreamError("Badly formatted epoch line"));
            for(i = 8; i <= 20; i += 3)
               if(line[i] != ' ')
                  throw(FFStreamError("Badly formatted epoch line"));

            satSys = line.substr(0,1);
            PRNID = asInt(line.substr(1,2));
            sat.fromString(line.substr(0,3));

            yr  = asInt(line.substr( 4,4));
            mo  = asInt(line.substr( 9,2));
            day = asInt(line.substr(12,2));
            hr  = asInt(line.substr(15,2));
            min = asInt(line.substr(18,2));
            dsec = asDouble(line.substr(21,2));
         }
         else {                  // RINEX 2
            for(i=2; i <= 17; i+=3)
               if(line[i] != ' ') {
                  throw(FFStreamError("Badly formatted epoch line"));
               }

            satSys = string(1,strm.header.fileSys[0]);
            PRNID = asInt(line.substr(0,2));
            sat.fromString(satSys + line.substr(0,2));

            yr  = asInt(line.substr( 2,3));
            if(yr < 80) yr += 100;     // rollover is at 1980
            yr += 1900;
            mo  = asInt(line.substr( 5,3));
            day = asInt(line.substr( 8,3));
            hr  = asInt(line.substr(11,3));
            min = asInt(line.substr(14,3));
            dsec = asDouble(line.substr(17,5));
         }

         // Fix RINEX epochs of the form 'yy mm dd hr 59 60.0'
         short ds = 0;
         if(dsec >= 60.) { ds = dsec; dsec = 0; }
         time = CivilTime(yr,mo,day,hr,min,dsec).convertToCommonTime();
         if(ds != 0) time += ds;

         // do not specify the time system here - do it when cast into e.g. EngEphem.
         time.setTimeSystem(TimeSystem::Any);

         // TOC is the epoch time
         GPSWeekSecond gws(time);
         Toc = gws.sow;

         if(strm.header.version < 3) {
            if(satSys == "G") {
               af0 = StringUtils::for2doub(line.substr(22,19));
               af1 = StringUtils::for2doub(line.substr(41,19));
               af2 = StringUtils::for2doub(line.substr(60,19));
            }
            else if(satSys == "R" || satSys == "S") {
               TauN   =      StringUtils::for2doub(line.substr(22,19));
               GammaN =      StringUtils::for2doub(line.substr(41,19));
               MFtime = (int)StringUtils::for2doub(line.substr(60,19));
            }
         }
         else if(satSys == "G" || satSys == "E") {
            af0 = StringUtils::for2doub(line.substr(23,19));
            af1 = StringUtils::for2doub(line.substr(42,19));
            af2 = StringUtils::for2doub(line.substr(61,19));
         }
         else if(satSys == "R" || satSys == "S") {
            TauN   =      StringUtils::for2doub(line.substr(23,19));
            GammaN =      StringUtils::for2doub(line.substr(42,19));
            MFtime = (int)StringUtils::for2doub(line.substr(61,19));
         }
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " + string(e.what()));
         GPSTK_THROW(err);
      }
   }


   void Rinex3NavData::getRecord(const int& nline, Rinex3NavStream& strm)
      throw(StringException, FFStreamError)
   {
      if(nline < 1 || nline > 7) {
         FFStreamError fse(string("Invalid line number ") + asString(nline));
         GPSTK_THROW(fse);
      }

      try {
         int n(strm.header.version < 3 ? 3 : 4);
         string line;
         strm.formattedGetLine(line);

         if(nline == 1) {
            if(satSys == "G") {
               IODE = StringUtils::for2doub(line.substr(n,19)); n+=19;
               Crs  = StringUtils::for2doub(line.substr(n,19)); n+=19;
               dn   = StringUtils::for2doub(line.substr(n,19)); n+=19;
               M0   = StringUtils::for2doub(line.substr(n,19));
            }
            else if(satSys == "E") {
               IODnav = StringUtils::for2doub(line.substr(n,19)); n+=19;
               Crs    = StringUtils::for2doub(line.substr(n,19)); n+=19;
               dn     = StringUtils::for2doub(line.substr(n,19)); n+=19;
               M0     = StringUtils::for2doub(line.substr(n,19));
            }
            else if(satSys == "R" || satSys == "S") {
               px     =        StringUtils::for2doub(line.substr(n,19)); n+=19;
               vx     =        StringUtils::for2doub(line.substr(n,19)); n+=19;
               ax     =        StringUtils::for2doub(line.substr(n,19)); n+=19;
               health = (short)StringUtils::for2doub(line.substr(n,19));
            }
         }

         else if(nline == 2) {
            if(satSys == "G" || satSys == "E") {
               Cuc   = StringUtils::for2doub(line.substr(n,19)); n+=19;
               ecc   = StringUtils::for2doub(line.substr(n,19)); n+=19;
               Cus   = StringUtils::for2doub(line.substr(n,19)); n+=19;
               Ahalf = StringUtils::for2doub(line.substr(n,19));
            }
            else if(satSys == "R" || satSys == "S") {
               py      =        StringUtils::for2doub(line.substr(n,19)); n+=19;
               vy      =        StringUtils::for2doub(line.substr(n,19)); n+=19;
               ay      =        StringUtils::for2doub(line.substr(n,19)); n+=19;
               if(satSys == "R")
                  freqNum = (short)StringUtils::for2doub(line.substr(n,19));
               else                       // GEO
                  accCode = StringUtils::for2doub(line.substr(n,19));
            }
         }

         else if(nline == 3) {
            if(satSys == "G" || satSys == "E") {
               Toe    = StringUtils::for2doub(line.substr(n,19)); n+=19;
               Cic    = StringUtils::for2doub(line.substr(n,19)); n+=19;
               OMEGA0 = StringUtils::for2doub(line.substr(n,19)); n+=19;
               Cis    = StringUtils::for2doub(line.substr(n,19));
            }
            else if(satSys == "R" || satSys == "S") {
               pz        = StringUtils::for2doub(line.substr(n,19)); n+=19;
               vz        = StringUtils::for2doub(line.substr(n,19)); n+=19;
               az        = StringUtils::for2doub(line.substr(n,19)); n+=19;
               if(satSys == "R")
                  ageOfInfo = StringUtils::for2doub(line.substr(n,19));
               else                       // GEO
                  IODN = StringUtils::for2doub(line.substr(n,19));
            }
         }

         else if(nline == 4) {
            i0       = StringUtils::for2doub(line.substr(n,19)); n+=19;
            Crc      = StringUtils::for2doub(line.substr(n,19)); n+=19;
            w        = StringUtils::for2doub(line.substr(n,19)); n+=19;
            OMEGAdot = StringUtils::for2doub(line.substr(n,19));
         }

         else if(nline == 5) {
            if(satSys == "G") {
               idot     =        StringUtils::for2doub(line.substr(n,19)); n+=19;
               codeflgs = (short)StringUtils::for2doub(line.substr(n,19)); n+=19;
               weeknum  = (short)StringUtils::for2doub(line.substr(n,19)); n+=19;
               L2Pdata  = (short)StringUtils::for2doub(line.substr(n,19));
            }
            else if(satSys == "E") {
               idot        =       StringUtils::for2doub(line.substr(n,19)); n+=19;
               datasources =(short)StringUtils::for2doub(line.substr(n,19)); n+=19;
               weeknum     =(short)StringUtils::for2doub(line.substr(n,19)); n+=19;
            }
         }

         else if(nline == 6) {
            if(satSys == "G") {
               accuracy =       StringUtils::for2doub(line.substr(n,19)); n+=19;
               health   = short(StringUtils::for2doub(line.substr(n,19))); n+=19;
               Tgd      =       StringUtils::for2doub(line.substr(n,19)); n+=19;
               IODC     =       StringUtils::for2doub(line.substr(n,19));
            }
            else if(satSys == "E") {
               accuracy =       StringUtils::for2doub(line.substr(n,19)); n+=19;
               health   = short(StringUtils::for2doub(line.substr(n,19))); n+=19;
               BGDa     =       StringUtils::for2doub(line.substr(n,19)); n+=19;
               BGDb     =       StringUtils::for2doub(line.substr(n,19));
            }
         }

         else if(nline == 7) {
            HOWtime = long(StringUtils::for2doub(line.substr(n,19))); n+=19;
            fitint  =      StringUtils::for2doub(line.substr(n,19));
   
            // In RINEX *files*, weeknum is the week of TOE.
            // Internally (Rinex3NavData and EngEphemeris), weeknum is the week of HOW
            if(HOWtime - Toe > HALFWEEK)
               weeknum--;
            else if(HOWtime - Toe < -HALFWEEK)
               weeknum++;
   
            // Some RINEX files have HOW < 0.
            while(HOWtime < 0) {
               HOWtime += (long)FULLWEEK;
               weeknum--;
            }
         }
      }
      catch (std::exception &e) {
         FFStreamError err("std::exception: " + string(e.what()));
         GPSTK_THROW(err);
      }

   }  // end getRecord()


}  // End of namespace gpstk
