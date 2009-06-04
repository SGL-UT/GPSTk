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
 * @file NovatelData.cpp
 * gpstk::NovatelData - container for Novatel data, with methods to convert
 * binary data files to Rinex
 */

#include <cstring>
#include "BinUtils.hpp"
#include "StringUtils.hpp"
#include "DayTime.hpp"
#include "EngEphemeris.hpp"
#include "RinexObsHeader.hpp"
#include "icd_200_constants.hpp"
#include "NovatelData.hpp"

using namespace std;
using namespace gpstk::BinUtils;
using namespace gpstk::StringUtils;

static bool debug=false;

namespace gpstk
{

   // --------------------------------------------------------------------------------
   const double CFF=C_GPS_M/OSC_FREQ;
   const double wl1=CFF/L1_MULT;
   const double wl2=CFF/L2_MULT;
   const double PhaseRollover=8388608;

   // --------------------------------------------------------------------------------
   const string NovatelData::RecNames[] = {
         string("Unknown"),
         string("RGEB obs"),
         string("RGEC obs"),
         string("POSB pos"),
         string("REPB nav"),
         string("RCSB sts"),
         string("RANGE obs"),
         string("RANGECMP obs"),
         string("RAWEPHEM nav")
      };

   // --------------------------------------------------------------------------------
   bool NovatelData::isNav(void) const
   {
      switch(rectype) {
         case POSB:
         case RCSB:
            return false;
         case REPB:
         case RAWEPHEM:
            return true;
         case RGEB:
         case RGEC:
         case RANGE:
         case RANGECMP:
            return false;
         case Unknown:
         default:
            return false;
      }
   }

   // --------------------------------------------------------------------------------
   bool NovatelData::isObs(void) const
   {
      switch(rectype) {
         case POSB:
         case RCSB:
            return false;
         case REPB:
         case RAWEPHEM:
            return false;
         case RGEB:
         case RGEC:
         case RANGE:
         case RANGECMP:
            return true;
         case Unknown:
         default:
            return false;
      }
   }

   // --------------------------------------------------------------------------------
   bool NovatelData::isAux(void) const
   {
      switch(rectype) {
         case POSB:
         case RCSB:
            return true;
         case REPB:
         case RAWEPHEM:
            return false;
         case RGEB:
         case RGEC:
         case RANGE:
         case RANGECMP:
            return false;
         case Unknown:
         default:
            return false;
      }
   }

   // --------------------------------------------------------------------------------
      // True if this record belongs to OEM2 receivers
   bool NovatelData::isOEM2(void) const
   {
      switch(rectype) {
         case POSB:
         case RCSB:
         case REPB:
         case RGEB:
         case RGEC:
            return true;
         case RAWEPHEM:
         case RANGE:
         case RANGECMP:
         case Unknown:
         default:
            return false;
      }
   }

   // --------------------------------------------------------------------------------
      // True if this record belongs to OEM4 receivers
   bool NovatelData::isOEM4(void) const
   {
      switch(rectype) {
         case RAWEPHEM:
         case RANGE:
         case RANGECMP:
            return true;
         case POSB:
         case RCSB:
         case REPB:
         case RGEB:
         case RGEC:
         case Unknown:
         default:
            return false;
      }
   }

   // --------------------------------------------------------------------------------
   bool NovatelData::isValid(void) const
   {
      switch(rectype) {
         case POSB:
         case RCSB:
         case REPB:
         case RAWEPHEM:
         case RGEB:
         case RGEC:
         case RANGE:
         case RANGECMP:
            if(datasize == 0 || headersize==0) return false;
            return true;
         case Unknown:
         default:
            return false;
      }
   }

   // --------------------------------------------------------------------------------
   void NovatelData::dump(ostream& str) const
   {
      str << "Record type is " << rectype << endl;
   }

   // --------------------------------------------------------------------------------
   void NovatelData::reallyPutRecord(FFStream& s) const 
      throw(exception, StringUtils::StringException, FFStreamError)
   {
      FFStreamError e("Novatel::reallyPutRecord() is not implemented");
      GPSTK_THROW(e);
   }


   // --------------------------------------------------------------------------------
   void NovatelData::reallyGetRecord(FFStream& ffs)
      throw(exception, StringUtils::StringException, FFStreamError)
   {
   try {
      if(dynamic_cast<NovatelStream*>(&ffs)) {

         NovatelStream& strm = dynamic_cast<NovatelStream&>(ffs);

         unsigned char *p0 = &buffer[0];
         unsigned char *p1 = &buffer[1];
         unsigned char *p2 = &buffer[2];
         unsigned char *p3 = &buffer[3];
         unsigned char *p4 = &buffer[4];
         int i,j,k,failure;
         long filepos;

         if(debug) cout << "Entered NovatelData::reallyGetRecord()" << endl;
            // read loop
         do {

            // move data down by 1 byte
            *p0 = *p1;
            *p1 = *p2;

            // get another character
            try {
               strm.read((char *)p2, 1);
            }
            catch(exception& e) {
               if(debug) cout << "read 1 threw std exception: " << e.what() << endl;
               //FFStreamError fe(string("std exception: ")+e.what());
               //GPSTK_THROW(fe);
            }

            if(strm.bad()) {
               FFStreamError fe("Read error");
               GPSTK_THROW(fe);
            }
            if(strm.eof()) {
               if(debug) cout << "Reached EOF" << endl;
               break;
            }

            if(debug) cout << "got char 0x" << hex << uppercase << int(buffer[2])
               << dec << endl;

            // look for sync bytes
            if(*p0==0xAA && *p1==0x44 && *p2==0x11) {
               // -------------------------------------------------- OEM2
               if(debug) cout << "Found OEM2 sync" << endl;

                  // save position in case of failure
               filepos = strm.tellg();
               if(debug) cout << "File position " << filepos << endl;

                  // read 9 more characters into buffer, giving total of 12
               strm.read((char *)p3,9);
               if(strm.bad()) {
                  FFStreamError fe("Read error");
                  GPSTK_THROW(fe);
               }
               if(strm.eof()) {
                  if(debug) cout << "Reached EOF" << endl;
                  break;
               }

                  // read the record ID
                    if(*p4==0x20) rectype = RGEB;
               else if(*p4==0x21) rectype = RGEC;
               else if(*p4==0x01) rectype = POSB;
               else if(*p4==0x0E) rectype = REPB;
               else if(*p4==0x0D) rectype = RCSB;
               else               rectype = Unknown;
               recnum = int(*p4);
               intelToHost(recnum);

                  // read the rest of the record
               failure = 0;
               if(rectype != Unknown) {

                     // get the size of the record
                  std::memmove(&datasize, &(buffer[8]), 4);
                  intelToHost(datasize);
                  if (debug)
                     cout << "datasize:" << datasize << endl;

                     // read the rest of the record
                  if(datasize-12 >= 1024) {
                     //FFStreamError fe("Read error - buffer overflow");
                     //GPSTK_THROW(fe);
                     failure = 1;
                  }
                  else {
                     strm.read((char *)&buffer[12],datasize-12);
                     if(strm.bad()) {
                        FFStreamError fe("Read error");
                        GPSTK_THROW(fe);
                     }
                     if(strm.eof()) {
                        if(debug) cout << "Reached EOF" << endl;
                        break;
                     }
                     headersize = 3;             // just the sync bytes

                        // compute the checksum
                        // Ref OEM2 manual
                     unsigned char checksum = 0;
                     checksum ^= buffer[0];
                     checksum ^= buffer[1];
                     checksum ^= buffer[2];
                     for(i=4; i<datasize; i++) checksum ^= buffer[i];

                     if(checksum == buffer[3]) break;    // success
                     failure = 2;

                  }  // end if datasize fits into buffer
               }  // end if record type != unknown

               if (debug)
                  cout << "failure=" << failure
                       << ", rectype=" << rectype
                       << ", datasize=" << datasize << endl;

                  // failure - either type unknown, buffer overflow or failed checksum
               if(debug) {
                  cout << "Failure - ";
                  if(failure == 0) cout << "type unknown";
                  else if(failure == 1) cout << "buffer overflow";
                  else if(failure == 2) cout << "failed checksum";
                  cout << " for recnum " << recnum
                     << " with headersize " << headersize
                     << " and message size " << datasize << endl;
               }

               strm.seekg(filepos);          // rewind to just after the sync bytes
               datasize = headersize = 0;

            }  // end if OEM2 sync

            else if(*p0==0xAA && *p1==0x44 && *p2==0x12) {
               // -------------------------------------------------- OEM4
               // Ref OEM4 Manual pg 15

               if(debug) cout << "Found OEM4 sync" << endl;

                  // save position in case of failure
               filepos = strm.tellg();
               if(debug) cout << "File position " << filepos << endl;

                  // ---------------------------------------
                  // read header, 25 characters, into buffer
               strm.read((char *)p3,25);
               if(strm.bad()) {
                  FFStreamError fe("Read error");
                  GPSTK_THROW(fe);
               }
               if(strm.eof()) {
                  if(debug) cout << "Reached EOF" << endl;
                  break;
               }

                  // parse the header
                  // Ref OEM4 Manual pg 16
                  // (only need some of the data here - cast to Rinex functions
                  // will parse the whole thing)
               unsigned char headerLength;
               std::memmove(&headerLength, &(buffer[3]), 1);  intelToHost(headerLength);
               short messageID;
               std::memmove(&messageID, &(buffer[4]), 2);     intelToHost(messageID);
               //char messageType;
               //memmove(&messageType, &(buffer[6]), 1);   intelToHost(messageType);
               //char portAddress;
               //memmove(&portAddress, &(buffer[7]), 1);   intelToHost(portAddress);
               short messageLength;
               std::memmove(&messageLength, &(buffer[8]), 2); intelToHost(messageLength);
               //short sequence;
               //memmove(&sequence, &(buffer[10]), 2);     intelToHost(sequence);
               //char idleTime;
               //memmove(&idleTime, &(buffer[12]), 1);     intelToHost(idleTime);
               //char timeStatus;
               //memmove(&timeStatus, &(buffer[13]), 1);   intelToHost(timeStatus);
               //short week;
               //memmove(&week, &(buffer[14]), 2);         intelToHost(week);
               //long msecOfWeek;
               //memmove(&msecOfWeek, &(buffer[16]), 4);   intelToHost(msecOfWeek);
               //long rxStatus;
               //memmove(&rxStatus, &(buffer[20]), 4);     intelToHost(rxStatus);
               //short reserved;
               //memmove(&reserved, &(buffer[24]), 2);     intelToHost(reserved);
               //short rxSWVersion;
               //memmove(&rxSWVersion, &(buffer[26]), 2);  intelToHost(rxSWVersion);
               
               datasize = messageLength;
               headersize = int(headerLength);
               recnum = messageID;

               if(headersize != 28) {   // manual warns that changes may be made
                  Exception e("Header size : expected 28 but found "
                     + StringUtils::asString(headersize) + " for record ID "
                     + StringUtils::asString(recnum));
                  GPSTK_THROW(e);
               }

               if(debug) cout << "hL " << int(headerLength)
                     << " ID " << messageID
                     << " mL " << messageLength
                     //<< " seq " << sequence
                     //<< " week " << week
                     //<< " msow " << msecOfWeek
                     //<< " rxver " << rxSWVersion
                     << endl;

               if(     recnum ==  43) rectype = RANGE;
               else if(recnum == 140) rectype = RANGECMP;
               else if(recnum ==  41) rectype = RAWEPHEM;
               else                   rectype = Unknown;

               failure=0;
               if(rectype != Unknown) {

                     // ---------------------------------------
                     // read the data message, but don't overwrite the header
                     // first check against buffer overflow
                  if(datasize-28 >= 1024 || datasize-28 < 0) {
                     //FFStreamError fe("Read error - buffer overflow");
                     //GPSTK_THROW(fe);
                     failure = 1;
                  }
                  else {
                     strm.read((char *)&(buffer[28]),datasize);
                     if(strm.bad()) {
                        FFStreamError fe("Read error");
                        GPSTK_THROW(fe);
                     }
                     if(strm.eof()) {
                        datasize = 0;         // mark a bad record
                        if(debug) cout << "Reached EOF" << endl;
                        break;
                     }
                     if(debug) cout << "Successfully read message" << endl;

                        // ---------------------------------------
                        // validate with 32-bit CRC
                        // cf. Ref OEM4 manual pg 21.

                        // get the checksum at the end
                     unsigned int checksum =
                        intelToHost(strm.getData<unsigned int>());

                        // calculate the checksum of the header(even sync)+data
                     unsigned int check=0,ultemp1,ultemp2;
                     for(i=0; i<datasize+28; i++) {
                        ultemp1 = (check >> 8) & 0x00FFFFFFL;
                        j = ((int)check ^ buffer[i]) & 0xFF;
                        ultemp2 = j;
                        for(k=8; k>0; k--) {
                           if(ultemp2 & 1)
                              ultemp2 = (ultemp2 >> 1) ^ 0xEDB88320L;
                           else
                              ultemp2 >>= 1;
                        }
                        check = ultemp1 ^ ultemp2;
                     }

                     if(check == checksum) {
                        if(debug) cout << "checksum ok" << endl;
                        break;
                     }
                     else failure = 2;

                  }  // end if datasize-28 < buffersize
               }  // end if rectype != Unknown

                  // failure - either type unknown, buffer overflow or failed checksum
               if(debug) {
                  cout << "Failure - ";
                  if(failure == 0) cout << "type unknown";
                  else if(failure == 1) cout << "buffer overflow";
                  else if(failure == 2) cout << "failed checksum";
                  cout << " for recnum " << recnum
                     << " with headersize " << headersize
                     << " and message size " << datasize << endl;
               }
               strm.seekg(filepos);
               datasize = headersize = 0;               // marks an invalid object

            }  // end if OEM4 sync

            else {                                       // skip these bytes
                  // print only if sync is not underway
               if(debug && !(*p1==0xAA && *p2==0x44) && !(*p2==0xAA) )
                  cout << "Skip a byte " << hex << uppercase << setfill('0')
                       << setw(2) << int(*p0) << setfill(' ') << endl;
            }

         } while(1);   // end read loop
      }
      else {
         FFStreamError e("NovatelData tried to read from a non-Novatel file");
         GPSTK_THROW(e);
      }

      if(!isValid()) {
         FFStreamError e("Read an invalid Novatel record");
         GPSTK_THROW(e);
      }

   }
   catch(Exception e) {
      if(debug) cout << "reallyGetRecord caught GPSTK exception " << e << endl;
   }
   catch(exception e) {
      if(debug) cout << "reallyGetRecord caught std exception " << e.what() << endl;
   }
   catch(...) {
      if(debug) cout << "reallyGetRecord caught an unknown exception" << endl;
   }

   }  // end NovatelData::reallyGetRecord


   // --------------------------------------------------------------------------------
   NovatelData::operator RinexNavData()
      throw(Exception)
   {
      if(!isValid() || !isNav()) {
         Exception e("Invalid or non-Nav record");
         GPSTK_THROW(e);
      }

      int i,j,k;
      long templ;
      EngEphemeris eeph;

      if(rectype == RAWEPHEM) {                    // OEM4

            // parse header
            // Ref OEM4 Manual pg 16
         unsigned char headerLength;
         std::memmove(&headerLength,  &(buffer[3]), 1); intelToHost(headerLength);
         short messageID;
         std::memmove(&messageID,     &(buffer[4]), 2); intelToHost(messageID);
         char messageType;
         std::memmove(&messageType,   &(buffer[6]), 1); intelToHost(messageType);
         char portAddress;
         std::memmove(&portAddress,   &(buffer[7]), 1); intelToHost(portAddress);
         short messageLength;
         std::memmove(&messageLength, &(buffer[8]), 2); intelToHost(messageLength);
         short sequence;
         std::memmove(&sequence,     &(buffer[10]), 2); intelToHost(sequence);
         char idleTime;
         std::memmove(&idleTime,     &(buffer[12]), 1); intelToHost(idleTime);
         char timeStatus;
         std::memmove(&timeStatus,   &(buffer[13]), 1); intelToHost(timeStatus);
         short week;
         std::memmove(&week,         &(buffer[14]), 2); intelToHost(week);
         long msecOfWeek;
         std::memmove(&msecOfWeek,   &(buffer[16]), 4); intelToHost(msecOfWeek);
         long rxStatus;
         std::memmove(&rxStatus,     &(buffer[20]), 4); intelToHost(rxStatus);
         short reserved;
         std::memmove(&reserved,     &(buffer[24]), 2); intelToHost(reserved);
         short rxSWVersion;
         std::memmove(&rxSWVersion,  &(buffer[26]), 2); intelToHost(rxSWVersion);
               
            // parse data
            // Ref OEM4 Manual pg 206
         short prn,track=1;
         long gpsSOW;

            // get PRN and timetag 
         std::memmove(&templ, &(buffer[28]), 4);
         intelToHost(templ);
         prn = short(templ);
         std::memmove(&gpsWeek, &(buffer[32]), 4);      // long gpsWeek is member data
         intelToHost(gpsWeek);
         std::memmove(&gpsSOW, &(buffer[36]), 4);
         intelToHost(gpsSOW);
      
            // convert the 3 subframes and create EngEphemeris
         long subframe[10];
         for(j=0; j<3; j++) {
            k = 40 + j*30;

            if(debug) {
               cout << "Subframe " << setfill('0') << j+1;
               for(i=0; i<30; i++)
                  cout << " " << hex << uppercase << setw(2) << int(buffer[k+i]);
               cout << dec << setfill(' ') << endl;
            }

            for(i=0; i<10; i++) {
               subframe[i] = (buffer[k] << 22)+(buffer[k+1] << 14)+(buffer[k+2] << 6);
               k += 3;
            }
            if(!eeph.addSubframe(subframe,gpsWeek,prn,track)){
               if(debug) cout << "Failed to convert RAWEPH subframe " << j+1
                  << ", prn " << prn << " at time " << gpsWeek << " " << gpsSOW
                  << endl;
            }
         }
      }  // end RAWEPH record

      else if(rectype == REPB) {                   // OEM2

         long prn;
         short track=1;

         // get PRN
         std::memmove(&prn,&(buffer[12]), 4);
         intelToHost(prn);

         // be sure week is defined
         if(gpsWeek == -1) {
            DayTime sysTime;
            gpsWeek = long(sysTime.GPSfullweek());
         }

            // convert the 3 subframes and create EngEphemeris
         long subframe[10];
         for(j=0; j<3; j++) {
            k = 16 + j*30;

            if(debug) {
               cout << "Subframe " << setfill('0') << j+1;
               for(i=0; i<30; i++)
                  cout << " " << hex << uppercase << setw(2) << int(buffer[k+i]);
               cout << dec << setfill(' ') << endl;
            }

            for(i=0; i<10; i++) {
               subframe[i] = (buffer[k] << 22)+(buffer[k+1] << 14)+(buffer[k+2] << 6);
               k += 3;
            }
            if(!eeph.addSubframe(subframe,gpsWeek,short(prn),track)){
               if(debug) cout << "Failed to convert REPB subframe " << j+1
                     << ", prn " << prn << endl;
            }
         }
         
      }  // end REPB record

      // convert it to Rinex
      RinexNavData rnd(eeph);

      return rnd;

   }  // end NovatelData::operator RinexNavData()


   // --------------------------------------------------------------------------------
   NovatelData::operator RinexObsData()
      throw(Exception)
   {
      if(!isValid() || !isObs()) {
         Exception e("Invalid or non-Obs record");
         GPSTK_THROW(e);
      }

      int i,j;
      int16_t temps;
      int32_t nobs;        // number of observation records (may be 2/PRN: L1 and L2)
      SatID sat;
      RinexObsData rod;     // this will be returned
      RinexObsData::RinexDatum rd;
      RinexObsData::RinexSatMap::iterator satit;
      RinexObsData::RinexObsTypeMap::iterator obsit;

      if(     rectype == RGEB) {             // OEM2


      }  // end RGEB record

      else if(rectype == RGEC) {             // OEM2
            // Ref OEM2 Manual pg 97

         if(debug) {
            cout << "Header " << setfill('0') << hex << uppercase;
            for(i=0; i<24; i++) cout << " " << setw(2) << int(buffer[i]);
            cout << dec << setfill(' ') << endl;
         }

            // number of observation records to follow
         std::memmove(&temps, &(buffer[12]), 2);
         intelToHost(temps);
         nobs = int32_t(temps);

            // GPS week (long gpsWeek is member data)
         std::memmove(&temps, &(buffer[14]), 2);
         intelToHost(temps);

            // resolve the week number ambiguity
         if(gpsWeek == -1) {
            DayTime sysTime;
            gpsWeek = long(sysTime.GPSfullweek());
         }
         gpsWeek = long(temps) + 1024*(gpsWeek/1024);

            // seconds of week * 100
         int32_t gpsSOW;
         std::memmove(&gpsSOW, &(buffer[16]), 4);
         intelToHost(gpsSOW);

            // receiver status
         int32_t rxStatus;
         std::memmove(&rxStatus, &(buffer[20]), 4);
         
            // put timetag into rod
         if (debug)
            cout << "gpsWeek:" << gpsWeek << " sow:" << gpsSOW/100.0 << endl;
         rod.time = DayTime(gpsWeek,gpsSOW/100.);
         rod.epochFlag = 0;
         rod.clockOffset = 0.0;     // don't have it ?
         rod.numSvs = 0;

            // loop over observation records
         for(i=0; i<nobs; i++) {
            uint32_t data[5];
            for(j=0; j<5; j++)
               std::memmove(&data[j], &(buffer[24+i*20+j*4]), 4);

            int prn         =     int(data[0] & 0x0000003FL);

            double SNR      = double((data[0] & 0x000007C0L) >>  6);

            double locktime = double((data[0] & 0xFFFFF800L) >> 11);

            double Ph;
            if(data[1] & 0x80000000L)     // 2s complement
               Ph =  double((data[1] ^ 0x7FFFFFFFL) + 1);
            else
               Ph =  double(data[1]);

            double Doppler  = double((data[2] & 0xFFFFFFF0L) >> 4);
            if(data[2] & 0x80000000L)     // 2s complement
               Doppler = -double((((data[2] & 0xFFFFFFF0L) ^ 0xFFFFFFF0L) >> 4)+1);
   
            //                                               this is 0c++/4.3.2/tr1/exp_integral.tcc:xFFFFFFFF + 1
            double Pr       =  double(data[2] & 0x0000000FL) * 4294967296.
                             + double(data[3]);
            // could the pseudorange ever be negative?
            if(data[2] & 0x00000008L)     // 2s complement
               Pr = -double((data[2] & 0x0000000FL) ^ 0x0000000FL) * 4294967296.
                    - double(data[3]                ^ 0xFFFFFFFFL  + 1);

            double SdPh     =     int(data[4] & 0x0000000FL);

            double SdPr     = double((data[4] & 0x000000F0L) >>  4);

            long TrackStatus =  long((data[4] & 0xFFFFFF00L) >>  8);
            // the rest are reserved

            // swap bytes
            intelToHost(prn);
            intelToHost(SNR);
            intelToHost(locktime);
            intelToHost(Ph);
            intelToHost(Doppler);
            intelToHost(Pr);
            intelToHost(SdPr);
            intelToHost(SdPh);

            // convert to physical units
            SNR += 20.;             // dB-Hz, but 51 means >=51, and 20 means <=20.
            locktime /= 32.;        // sec
            Doppler /= 256.;        // Hz
            Pr /= 128.;             // m
            Ph /= 256.;             // cycles
            SdPr = (SdPr + 1.)/16.; // m
            SdPh = (SdPh + 1)/512.; // cycles

            // break out the TrackStatus
            // cf. Table 5-6, pg 95 of OEM2 manual
            int TrackState   = int( TrackStatus & 0x0000000FL);
            int Channel      = int((TrackStatus & 0x000001F0L) >>  4);
            bool PhaseLock   = bool(TrackStatus & 0x00000200L);
            bool ParityKnown = bool(TrackStatus & 0x00000400L);
            bool CodeLock    = bool(TrackStatus & 0x00000800L);
            int Frequency    = int((TrackStatus & 0x00100000L) >> 20); // 0:L1 1:L2
            // CodeType is 0: CA 1: P 2: Pcodeless
            int CodeType   = int((TrackStatus & 0x00600000L) >> 21);

            if(!PhaseLock || !CodeLock) continue;

            // correct the phase for rollovers
            // ref. OEM2 manual pg 97
            double ADRrolls = ((-Pr/(Frequency==0 ? wl1 : wl2))-Ph)/PhaseRollover;
            Ph += long(ADRrolls + (ADRrolls > 0 ? 0.5 : -0.5)) * PhaseRollover;

            //apparently the Novatel convert utility ignores this too
            //ignore if(!ParityKnown) Ph = 0.0;

            // fill RinexObsData rod
            sat = SatID(prn,SatID::systemGPS);
            satit = rod.obs.find(sat);          // find the sat
            if(satit == rod.obs.end()) {        // not there - add this sat
               RinexObsData::RinexObsTypeMap rotm;
               rod.obs[sat] = rotm;
               rod.numSvs++;
               satit = rod.obs.find(sat);       // now find it
            }

            // for convenience, reference the obs data map
            RinexObsData::RinexObsTypeMap& obs = satit->second;
            if(Frequency == 0) {       // frequency = L1
               rd.ssi = rd.lli = 0; rd.data = -Ph;
               obs[RinexObsHeader::L1] = rd;                         // L1

               rd.ssi = rd.lli = 0; rd.data = Pr;
               if(CodeType == 0) obs[RinexObsHeader::C1] = rd;       // C1
               else              obs[RinexObsHeader::P1] = rd;       // P1

               rd.ssi = rd.lli = 0; rd.data = -Doppler;
               obs[RinexObsHeader::D1] = rd;                         // D1

               rd.ssi = rd.lli = 0; rd.data = SNR;                   // S1
               obs[RinexObsHeader::S1] = rd;
            }
            else {
               rd.ssi = rd.lli = 0; rd.data = Ph;
               obs[RinexObsHeader::L2] = rd;                         // L2

               rd.ssi = rd.lli = 0; rd.data = Pr;
               obs[RinexObsHeader::P2] = rd;                         // P2

               rd.ssi = rd.lli = 0; rd.data = -Doppler;
               obs[RinexObsHeader::D2] = rd;                         // D2

               rd.ssi = rd.lli = 0; rd.data = SNR;
               obs[RinexObsHeader::S2] = rd;                         // S2
            }

         }

      }  // end RGEC record

      else {                                 // all OEM4 obs records

            // header
            // Ref OEM4 Manual pg 16
         uint8_t headerLength;
         std::memmove(&headerLength, &(buffer[3]), 1);  intelToHost(headerLength);
         int16_t messageID;
         std::memmove(&messageID, &(buffer[4]), 2);     intelToHost(messageID);
         int8_t messageType;
         std::memmove(&messageType, &(buffer[6]), 1);   intelToHost(messageType);
         int8_t portAddress;
         std::memmove(&portAddress, &(buffer[7]), 1);   intelToHost(portAddress);
         int16_t messageLength;
         std::memmove(&messageLength, &(buffer[8]), 2); intelToHost(messageLength);
         int16_t sequence;
         std::memmove(&sequence, &(buffer[10]), 2);     intelToHost(sequence);
         int8_t idleTime;
         std::memmove(&idleTime, &(buffer[12]), 1);     intelToHost(idleTime);
         int8_t timeStatus;
         std::memmove(&timeStatus, &(buffer[13]), 1);   intelToHost(timeStatus);
         int16_t week;
         std::memmove(&week, &(buffer[14]), 2);         intelToHost(week);
         int32_t msecOfWeek;
         std::memmove(&msecOfWeek, &(buffer[16]), 4);   intelToHost(msecOfWeek);
         int32_t rxStatus;
         std::memmove(&rxStatus, &(buffer[20]), 4);     intelToHost(rxStatus);
         int16_t reserved;
         std::memmove(&reserved, &(buffer[24]), 2);     intelToHost(reserved);
         int16_t rxSWVersion;
         std::memmove(&rxSWVersion, &(buffer[26]), 2);  intelToHost(rxSWVersion);
               
            // put timetag into rod
         rod.time = DayTime(week,double(msecOfWeek)/1000.);
         rod.epochFlag = 0;
         rod.clockOffset = 0.0;     // don't have it ?

         if(     rectype == RANGE) {
            // Ref OEM4 Manual pg 198-201

            nobs = 0;
            std::memmove(&nobs, &(buffer[28]), 4);
            intelToHost(nobs);

            rod.numSvs = 0;
            for(i=0; i<nobs; i++) {
               uint16_t prn,reserved;
               uint32_t TrackStatus;
               float PrStd,PhStd,Doppler,SNR,locktime;
               double Pr,Ph;

               std::memmove(&prn,         &(buffer[32+i*44]), 2);
               intelToHost(prn);
               std::memmove(&reserved,    &(buffer[34+i*44]), 2);
               intelToHost(reserved);
               std::memmove(&Pr,          &(buffer[36+i*44]), 8);
               intelToHost(Pr);
               std::memmove(&PrStd,       &(buffer[44+i*44]), 4);
               intelToHost(PrStd);
               std::memmove(&Ph,          &(buffer[48+i*44]), 8);
               intelToHost(Ph);
               std::memmove(&PhStd,       &(buffer[56+i*44]), 4);
               intelToHost(PhStd);
               std::memmove(&Doppler,     &(buffer[60+i*44]), 4);
               intelToHost(Doppler);
               std::memmove(&SNR,         &(buffer[64+i*44]), 4);
               intelToHost(SNR);
               std::memmove(&locktime,    &(buffer[68+i*44]), 4);
               intelToHost(locktime);
               std::memmove(&TrackStatus, &(buffer[72+i*44]), 4);
               intelToHost(TrackStatus);

               // break out the TrackStatus
               // cf. Table 56, pg 199 of OEM4 manual
               int TrackState = int( TrackStatus & 0x0000001FL);
               int Channel    = int((TrackStatus & 0x000003E0L) >>  5);
               bool PhaseLock = bool(TrackStatus & 0x00000400L);
               bool CodeLock  = bool(TrackStatus & 0x00001000L);
               int Frequency  = int((TrackStatus & 0x00600000L) >> 21); // 0:L1 1:L2
               // CodeType is 0CA 1P 2Pcodeless
               int CodeType   = int((TrackStatus & 0x03800000L) >> 23);
               bool HalfCycle = bool(TrackStatus & 0x10000000L);

               if(!PhaseLock || !CodeLock) continue;        // data is not reliable

               // fill RinexObsData rod
               sat = SatID(prn,SatID::systemGPS);
               satit = rod.obs.find(sat);          // find the sat
               if(satit == rod.obs.end()) {        // not there - add this sat
                  RinexObsData::RinexObsTypeMap rotm;
                  rod.obs[sat] = rotm;
                  rod.numSvs++;
                  satit = rod.obs.find(sat);       // now find it
               }

               // for convenience, reference the obs data map inside rod
               RinexObsData::RinexObsTypeMap& obs = satit->second;
               if(Frequency == 0) {       // frequency = L1
                  rd.ssi = rd.lli = 0; rd.data = -Ph;
                  obs[RinexObsHeader::L1] = rd;                      // L1

                  rd.ssi = rd.lli = 0; rd.data = Pr;
                  if(CodeType == 0) obs[RinexObsHeader::C1] = rd;    // C1
                  else              obs[RinexObsHeader::P1] = rd;    // P1

                  rd.ssi = rd.lli = 0; rd.data = Doppler;
                  obs[RinexObsHeader::D1] = rd;                      // D1

                  rd.ssi = rd.lli = 0; rd.data = SNR;
                  obs[RinexObsHeader::S1] = rd;                      // S1
               }
               else {
                  rd.ssi = rd.lli = 0; rd.data = -Ph;
                  obs[RinexObsHeader::L2] = rd;                      // L2

                  rd.ssi = rd.lli = 0; rd.data = Pr;
                  obs[RinexObsHeader::P2] = rd;                      // P2

                  rd.ssi = rd.lli = 0; rd.data = Doppler;
                  obs[RinexObsHeader::D2] = rd;                      // D2

                  rd.ssi = rd.lli = 0; rd.data = SNR;
                  obs[RinexObsHeader::S2] = rd;                      // S2
               }

            }

         }  // end RANGE record

         else if(rectype == RANGECMP) {
            // Ref OEM4 Manual pg 202-203

            nobs = 0;
            std::memmove(&nobs, &(buffer[28]), 4);
            intelToHost(nobs);

            rod.numSvs = 0;
            for(i=0; i<nobs; i++) {
               uint32_t data[6];
               for(j=0; j<6; j++)
                  std::memmove(&data[j], &(buffer[32+i*24+j*4]), 4);

               long TrackStatus =        data[0];
               // this is what is in the manual - its wrong
               //double Doppler =   double(data[1] & 0x0FFFFFFFL);
               // this is not documented in the manual...
               //double Doppler =   double(data[1] & 0x000FFFFFL);
               //if(data[1] & 0x0FF00000L == 0x0FF00000L) Doppler = -Doppler;
               // try this - cf the OEM2 manual and implementation above
               double Doppler  = double((data[1] & 0x0FFFFFFFL));
               if(data[1] & 0x08000000L)     // 2s complement
                  Doppler = -double(((data[1] & 0x0FFFFFFFL) ^ 0x0FFFFFFFL) + 1);
               double Pr =       double((data[1] & 0xF0000000L) >> 28)
                                + double(data[2]) * 16.;
               double Ph =        double(data[3]);
               int SdPrCode =        int(data[4] & 0x0000000FL);
               double SdPh =     double((data[4] & 0x000000F0L) >>  4);
               int prn =            int((data[4] & 0x0000FF00L) >>  8);
               double locktime = double((data[4] & 0xFFFF0000L) >> 16)
                               +  double(data[5] & 0x0000001FL);
               double SNR     =  double((data[5] & 0x000003E0L) >>  5);
               // the rest are reserved
 
               // swap bytes
               intelToHost(Doppler);
               intelToHost(Pr);
               intelToHost(Ph);
               intelToHost(SdPrCode);  // code - see pg 203 of OEM4 manual
               intelToHost(SdPh);
               intelToHost(prn);
               intelToHost(locktime);
               intelToHost(SNR);

               // convert to physical units
               Doppler /= 256.;        // Hz
               Pr /= 128.;             // m
               Ph /= 256.;             // cycles
               double SdPr;
               switch(SdPrCode) {      // this is just a code
                  // ref table on pg 203 of OEM4 manual
                  case  0: SdPr =   0.050; break; // m
                  case  1: SdPr =   0.075; break; // m
                  case  2: SdPr =   0.113; break; // m
                  case  3: SdPr =   0.169; break; // m
                  case  4: SdPr =   0.253; break; // m
                  case  5: SdPr =   0.380; break; // m
                  case  6: SdPr =   0.570; break; // m
                  case  7: SdPr =   0.854; break; // m
                  case  8: SdPr =   1.281; break; // m
                  case  9: SdPr =   2.375; break; // m
                  case 10: SdPr =   4.750; break; // m
                  case 11: SdPr =   9.500; break; // m
                  case 12: SdPr =  19.000; break; // m
                  case 13: SdPr =  38.000; break; // m
                  case 14: SdPr =  76.000; break; // m
                  case 15: SdPr = 152.000; break; // m
                  default: SdPr =    0.00; break;
               }
               SdPh = (SdPh + 1)/512.; // cycles
               locktime /= 32.;        // seconds
               SNR += 20.;             // dB-Hz
               // NB SNR 51 means >=51, and 20 means <=20.

               // break out the TrackStatus
               // cf. Table 56, pg 199 of OEM4 manual
               int TrackState = int( TrackStatus & 0x0000001FL);
               int Channel    = int((TrackStatus & 0x000003E0L) >>  5);
               bool PhaseLock = bool(TrackStatus & 0x00000400L);
               bool CodeLock  = bool(TrackStatus & 0x00001000L);
               int Frequency  = int((TrackStatus & 0x00600000L) >> 21); // 0:L1 1:L2
               // CodeType is 0CA 1P 2Pcodeless
               int CodeType   = int((TrackStatus & 0x03800000L) >> 23);
               bool HalfCycle = bool(TrackStatus & 0x10000000L);

               if(!PhaseLock || !CodeLock) continue;        // data is not reliable

               // correct the phase for rollovers
               // cf. OEM4 manual pg 203
               double ADRrolls = ((Pr/(Frequency==0 ? wl1 : wl2)) + Ph)/PhaseRollover;
               Ph -= long(ADRrolls + (ADRrolls > 0 ? 0.5 : -0.5)) * PhaseRollover;

               // consider debiasing the phase

               // use track status flags to set lli on the phase

               // what to do with HalfCycle?

               // fill RinexObsData rod
               sat = SatID(prn,SatID::systemGPS);
               satit = rod.obs.find(sat);          // find the sat
               if(satit == rod.obs.end()) {        // not there - add this sat
                  RinexObsData::RinexObsTypeMap rotm;
                  rod.obs[sat] = rotm;
                  rod.numSvs++;
                  satit = rod.obs.find(sat);       // now find it
               }

               // for convenience, reference the obs data map inside rod
               RinexObsData::RinexObsTypeMap& obs = satit->second;
               if(Frequency == 0) {       // frequency = L1
                  rd.ssi = rd.lli = 0; rd.data = -Ph;
                  obs[RinexObsHeader::L1] = rd;                      // L1

                  rd.ssi = rd.lli = 0; rd.data = Pr;
                  if(CodeType == 0) obs[RinexObsHeader::C1] = rd;    // C1
                  else              obs[RinexObsHeader::P1] = rd;    // P1

                  rd.ssi = rd.lli = 0; rd.data = Doppler;
                  obs[RinexObsHeader::D1] = rd;                      // D1

                  rd.ssi = rd.lli = 0; rd.data = SNR;
                  obs[RinexObsHeader::S1] = rd;                      // S1
               }
               else {
                  rd.ssi = rd.lli = 0; rd.data = -Ph;
                  obs[RinexObsHeader::L2] = rd;                      // L2

                  rd.ssi = rd.lli = 0; rd.data = Pr;
                  obs[RinexObsHeader::P2] = rd;                      // P2

                  rd.ssi = rd.lli = 0; rd.data = Doppler;
                  obs[RinexObsHeader::D2] = rd;                      // D2

                  rd.ssi = rd.lli = 0; rd.data = SNR;
                  obs[RinexObsHeader::S2] = rd;                      // S2
               }

            }  // end loop over obs

         }  // end RANGECMP record

      }  // end all OEM4 obs records

      return rod;

   }  // end NovatelData::operator RinexObsData()

}  // end namespace gpstk
