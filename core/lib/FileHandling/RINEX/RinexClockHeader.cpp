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
 * @file RinexClockHeader.cpp
 * Encapsulate header of RINEX clock file, including I/O
 */

#include <list>
#include <string>
#include <cstdlib>
#include "RinexClockHeader.hpp"
#include "RinexClockStream.hpp"
#include "StringUtils.hpp"
#include "SatID.hpp"
#include "FFStream.hpp"
#include "FFStreamError.hpp"

#ifdef _WIN32
#if (_MSC_VER == 1700)
#define strtoll _strtoi64
#endif
#endif

using namespace std;

namespace gpstk
{
   using namespace StringUtils;

   const string RinexClockHeader::versionString     =  "RINEX VERSION / TYPE";
   const string RinexClockHeader::runByString       =  "PGM / RUN BY / DATE";
   const string RinexClockHeader::commentString     =  "COMMENT";
   const string RinexClockHeader::leapSecondsString =  "LEAP SECONDS";
   const string RinexClockHeader::dataTypesString   =  "# / TYPES OF DATA";
   const string RinexClockHeader::stationNameString =  "STATION NAME / NUM";
   const string RinexClockHeader::calibrationClkString = "STATION CLK REF";
   const string RinexClockHeader::acNameString      =   "ANALYSIS CENTER";
   const string RinexClockHeader::numRefClkString   =  "# OF CLK REF";
   const string RinexClockHeader::analysisClkRefString = "ANALYSIS CLK REF";
   const string RinexClockHeader::numStationsString = "# OF SOLN STA / TRF";
   const string RinexClockHeader::solnStaNameString =  "SOLN STA NAME / NUM";
   const string RinexClockHeader::numSatsString     =  "# OF SOLN SATS";
   const string RinexClockHeader::prnListString     =  "PRN LIST";
   const string RinexClockHeader::endOfHeader       =  "END OF HEADER";


   bool RinexClockHeader::isValid() const
   {
      if ( !(dataTypeList.size() >= 1) )
      {
         return false;
      }

      list<RinexClkType>::const_iterator itr;
      for (itr = dataTypeList.begin(); itr != dataTypeList.end(); itr++)
      {
         if (*itr == AR)
         {
            if ( !(valid & allValidAR) )
               return false;
         }
         else if (*itr == AS)
         {
            if ( !(valid & allValidAS) )
               return false;
         }
         else if (*itr == CR)
         {
            if ( !(valid & allValidCR) )
               return false;
         }
         else if (*itr == DR)
         {
            if ( !(valid & allValidDR) )
               return false;
         }
         else if (*itr == MS)
         {
            if ( !(valid & allValidMS) )
               return false;
         }
         else  // unknown type
            return false;
      }

      return true;

   }  // isValid


   void RinexClockHeader::clear()
   {
      version = 2.0;
      fileType.clear();
      fileProgram.clear();
      fileAgency.clear();
      date.clear();
      commentList.clear();
      leapSeconds = 0;
      numType = 0;
      dataTypeList.clear();
      stationName.clear();
      stationNumber.clear();
      stationClkRef.clear();
      ac.clear();
      acName.clear();
      refClkList.clear();
      numSta = 0;
      trf.clear();
      solnStaList.clear();
      numSats = 0;
      prnList.clear();
      valid = 0;
   }


   void RinexClockHeader::dump(ostream& s) const
   {
      s << "---------------------- REQUIRED ---------------------" << endl;
      s << "Rinex Version: " << fixed << setw(4) << setprecision(2) << version
        << ", File type: " << fileType << endl;
      s << "Program: " << fileProgram
        << ", Agency: " << fileAgency
        << ", Date: " << date << endl;

      s << "Clock data types: ";
      bool ar, as, cr, dr, ms;
      ar = as = cr = dr = ms = false;
      list<RinexClkType>::const_iterator dataTypeListItr;
      for(dataTypeListItr = dataTypeList.begin();
          dataTypeListItr != dataTypeList.end(); dataTypeListItr++)
      {
         s << leftJustify(dataTypeListItr->type, 2) <<  ' ';
         if      (*dataTypeListItr == AS) as = true;
         else if (*dataTypeListItr == AR) ar = true;
         else if (*dataTypeListItr == CR) cr = true;
         else if (*dataTypeListItr == DR) dr = true;
         else if (*dataTypeListItr == MS) ms = true;
      }
      s << endl;

      if ( cr || dr || (valid & stationNameValid) )
      {
         s << "Station/Reciever: " << stationName
           << ' ' << stationNumber << endl;
      }

      if ( cr || (valid & calibrationClkValid) )
      {
         s << "Station Clock Ref: " << stationClkRef << endl;
      }

      if ( ar || as || ms || (valid & acNameValid) )
      {
         s << "Analysis Center: " << ac
           << ' ' << acName << endl;
      }

      if ( ar || as || (valid & numRefClkValid) )
      {
         list<RefClkRecord>::const_iterator refClkListItr;
         for (refClkListItr = refClkList.begin();
              refClkListItr != refClkList.end(); refClkListItr++)
         {
            s << "Clock References from: " << refClkListItr->startEpoch
              << ", to: " << refClkListItr->stopEpoch
              << ", count: " << refClkListItr->numClkRef
              << endl;
            list<RefClk>::const_iterator clocksItr;
            for (clocksItr = refClkListItr->clocks.begin();
                 clocksItr != refClkListItr->clocks.end(); clocksItr++)
            {
               s << "     " << "name: " << clocksItr->name
                 << ", number: " << clocksItr->number
                 << ", constraint: " << clocksItr->clkConstraint
                 << endl;
            }
         }
      }

      if ( ar || as || (valid & numStationsValid) )
      {
         s << "# of Solution Stations: " << numSta
           << ", TRF: " << trf
           << endl;
      }

      if ( ar || as || (valid & solnStaNameValid) )
      {
         list<SolnSta>::const_iterator solnStaListItr;
         for (solnStaListItr = solnStaList.begin();
              solnStaListItr != solnStaList.end(); solnStaListItr++)
         {
            s << "Soln. station/reciever name: " << solnStaListItr->name
              << ", number: " << solnStaListItr->number
              << endl
              << "  pos: x:" << rightJustify(asString(solnStaListItr->posX), 11)
              << " y:" << rightJustify(asString(solnStaListItr->posY), 11)
              << " z:" << rightJustify(asString(solnStaListItr->posZ), 11)
              << endl;
         }
      }

      if ( as || (valid & numSatsValid) )
      {
         s << "Soln. PRN count: " << numSats << endl;
      }

      if ( as || (valid & prnListValid) )
      {
         s << "  ";
         list<SatID>::const_iterator prnListItr;
         for (prnListItr = prnList.begin();
              prnListItr != prnList.end(); prnListItr++)
         {
            s << ' ';
            string sat;
            switch(prnListItr->system)
            {
               case SatelliteSystem::GPS:     sat = "G"; break;
               case SatelliteSystem::Glonass: sat = "R"; break;
               default:                   sat = "?"; break;
            }
            sat += rightJustify(asString(prnListItr->id), 2, '0');
            s << sat;
         }
         s << endl;
      }

      s << "---------------------- OPTIONAL* --------------------" << endl;
      s << "*If data type is AS or AR some comments are required." << endl;

      if ( as || ar || (valid & commentValid) )
      {
         s << "Comment(s): " << endl;
         list<string>::const_iterator commentListItr;
         for (commentListItr = commentList.begin();
              commentListItr != commentList.end(); commentListItr++)
         {
            s << "   " << *commentListItr << endl;
         }
      }

      if ( valid & leapSecondsValid )
      {
         s << "Leap Seconds: " << leapSeconds << endl;
      }

      s << "-------------------- END OF HEADER ------------------" << endl;

   }  // dump


   void RinexClockHeader::reallyPutRecord(FFStream& ffs) const
   {
      RinexClockStream& strm = dynamic_cast<RinexClockStream&>(ffs);

      strm.header = *this;

      if ( !isValid() )
      {
         FFStreamError err("Incomplete or invalid header.");
         err.addText("Make sure you set all header valid bits for all "
                     "of the available data.");
         GPSTK_THROW(err);
      }

      if (valid & versionValid)
      {
         strm << right << setw(9) << setprecision(2) << fixed << version << left
              << setw(11) << ' '
              << setw(40) << fileType << versionString << endlpp;
      }
      if (valid & runByValid)
      {
         strm << setw(20) << fileProgram
              << setw(20) << fileAgency
              << setw(20) << date
              << runByString << endlpp;
      }
      if (valid & commentValid)
      {
         list<string>::const_iterator itr;
         for (itr = commentList.begin(); itr != commentList.end(); itr++)
         {
            strm << setw(60) << (*itr) << commentString << endlpp;
         }
      }
      if (valid & leapSecondsValid)
      {
         strm << right << setw(6) << leapSeconds << left
              << setw(54) << ' ' << leapSecondsString << endlpp;
      }
      if ( valid & dataTypesValid )
      {
         strm << right << setw(6) << numType;
         for (const auto& itr : dataTypeList)
         {
            strm << "    " << right << setw(2) << itr.type;
         }
         strm << left << setw(54 - ((dataTypeList.size())*6)) << ' '
              << dataTypesString << endlpp;
      }
      if ( valid & stationNameValid )
      {
         strm << setw(4) << stationName
              << ' ' << setw(20) << stationNumber
              << setw(35) << ' ' << stationNameString << endlpp;
      }
      if ( valid & calibrationClkValid )
      {
         strm << setw(60) << stationClkRef << calibrationClkString << endlpp;
      }
      if ( valid & acNameValid )
      {
         strm << setw(3) << ac << "  " << setw(55) << acName << acNameString
              << endlpp;
      }
      if ( valid & numStationsValid )
      {
         for (const auto& recItr : refClkList)
         {
            strm << right << setw(6) << recItr.numClkRef << left
                 << ' ' << writeTime(recItr.startEpoch)
                 << ' ' << writeTime(recItr.stopEpoch)
                 << numRefClkString << endlpp;

            for (const auto& clkItr : recItr.clocks)
            {
               strm << setw(4) << clkItr.name
                    << ' ' << setw(20) << clkItr.number
                    << setw(15) << ' ';
               if (clkItr.clkConstraint != 0)
               {
                  strm << clkItr.clkConstraint;
               }
               else
               {
                  strm << setw(19) << ' ';
               }
               strm << ' ' << analysisClkRefString << endlpp;
            }
         }
      }
      if ( valid & numStationsValid )
      {
         strm << right << setw(6) << numSta << left
              << "    " << setw(50) << trf
              << numStationsString << endlpp;
      }
      if ( valid & solnStaNameValid )
      {
         for (const auto& itr : solnStaList)
         {
            strm << setw(4) << itr.name
                 << ' ' << setw(20) << itr.number
                 << right << setw(11) << itr.posX << ' '
                 << right << setw(11) << itr.posY << ' '
                 << right << setw(11) << itr.posZ << left
                 << solnStaNameString << endlpp;
         }
      }
      if ( valid & numSatsValid )
      {
         strm << right << setw(6) << numSats << left << setw(54) << ' '
              << numSatsString << endlpp;
      }
      if ( valid & prnListValid )
      {
         int prnCount = 0;
         for (const auto& itr : prnList)
         {
            prnCount++;

            if (itr.system == SatelliteSystem::GPS)
               strm << "G";
            else if (itr.system == SatelliteSystem::Glonass)
               strm << "R";
            else
               strm << " ";
            strm << right << setw(2) << setfill('0') << itr.id << setfill(' ')
                 << ' ' << left;
            if ( (prnCount % 15) == 0 )
            {
               strm << prnListString << endlpp;
            }
         }

         if ( (prnCount % 15) != 0 )
         {
            strm << setw((15-(prnCount % 15)) * 4) << ' ' << prnListString
                 << endlpp;
         }
      }

         
      strm << setw(60) << ' ' << endOfHeader << endlpp;

   }  // reallyPutRecord


      // This function parses the entire header from the given stream
   void RinexClockHeader::reallyGetRecord(FFStream& ffs)
   {
      RinexClockStream& strm = dynamic_cast<RinexClockStream&>(ffs);

         // if already read, just return
      if (strm.headerRead == true)
         return;

         // Reading a new header, clear any preexisting data.
      clear();

      string line;

      while ( !(valid & endValid) )
      {
         strm.formattedGetLine(line);
         StringUtils::stripTrailing(line);

         if ( line.length() == 0 )
         {
            FFStreamError ffse("No data read!");
            GPSTK_THROW(ffse);
         }
         else if ( line.length() < 60 || line.length() > 80 )
         {
            FFStreamError ffse("Invalid line length");
            GPSTK_THROW(ffse);
         }

         try
         {
            ParseHeaderRecord(line);
         }
         catch(FFStreamError& ffse)
         {
            GPSTK_RETHROW(ffse);
         }
      }

         // If we get here, we should have reached the end of header line
      strm.header = *this;
      strm.headerRead = true;

   }  // reallyGetRecord


      // this function parses a single header record
   void RinexClockHeader::ParseHeaderRecord(const string& line)
   {
      string label(line, 60, 20);

         // RINEX VERSION / TYPE
      if (label == versionString)
      {
         version = asDouble(line.substr(0,9));

         fileType = strip(line.substr(20, 40));
         if ( fileType[0] != 'C' && fileType[0] != 'c' )
         {
               // invalid fileType - throw
            FFStreamError e("Incorrect file type: " + fileType);
            GPSTK_THROW(e);
         }

         valid |= versionValid;

      }
         // PGM / RUN BY / DATE
      else if (label == runByString)
      {
         fileProgram =  strip(line.substr( 0, 20));
         fileAgency  =  strip(line.substr(20, 20));
         date        =  strip(line.substr(40, 20));

         valid |= runByValid;

      }
         // COMMENT
      else if (label == commentString)
      {
         string s = line.substr(0, 60);
         commentList.push_back(s);

         valid |= commentValid;

      }
         // LEAP SECONDS
      else if (label == leapSecondsString)
      {
         leapSeconds = asInt(line.substr(0,6));

         valid |= leapSecondsValid;

      }
         // # / TYPES OF DATA
      else if (label == dataTypesString)
      {
         numType = asInt(line.substr(0,6));
         if ( numType < 0 || numType > 5 )
         {
               // invalid number of data types - throw
            FFStreamError e("Invalid number of data types: " +
                            asString(numType));
            GPSTK_THROW(e);
         }
         dataTypeList.clear();
         for(int i = 0; i < numType; i++)
         {
            string dtype = line.substr(i*6+10, 2);
            if      ( upperCase(dtype) == "AR" ) dataTypeList.push_back(AR);
            else if ( upperCase(dtype) == "AS" ) dataTypeList.push_back(AS);
            else if ( upperCase(dtype) == "CR" ) dataTypeList.push_back(CR);
            else if ( upperCase(dtype) == "DR" ) dataTypeList.push_back(DR);
            else if ( upperCase(dtype) == "MS" ) dataTypeList.push_back(MS);
            else
            { // unknown data type - throw
               FFStreamError e("Invalid data type: " + dtype);
               GPSTK_THROW(e);
            }
         }

         valid |= dataTypesValid;

      }
         // STATION NAME / NUM
      else if (label == stationNameString)
      {
         stationName = line.substr(0,4);
         stationNumber = strip(line.substr(4,20));

         valid |= stationNameValid;

      }
         // STATION CLK REF
      else if (label == calibrationClkString)
      {
         stationClkRef = strip( line.substr(0,60) );

         valid |= calibrationClkValid;

      }
         // ANALYSIS CENTER
      else if (label == acNameString)
      {
         ac = line.substr(0, 3);
         acName = strip(line.substr(5,55));

         valid |= acNameValid;

      }
         // # OF CLK REF
      else if (label == numRefClkString)
      {
         RefClkRecord record;
         record.numClkRef = asInt( line.substr(0,6) );
         if( asInt(line.substr(7,4)) )
         {
            record.startEpoch = parseTime(line.substr(7,26));
            if ( asInt(line.substr(34,26)) )
            {
               record.stopEpoch = parseTime(line.substr(34,26));
               if ( record.startEpoch > record.stopEpoch )
               {  // invalid start/stop epochs - throw
                  FFStreamError e("Invalid Start/Stop Epoch start: " +
                                  line.substr(7,26) + ", stop: " +
                                  line.substr(34,26));
                  GPSTK_THROW(e);
               }
            }
            else
            {  // startEpoch w/o stopEpoch - throw
               FFStreamError e("Invalid Start/Stop Epoch start: " +
                               line.substr(7,26) + ", stop: " +
                               line.substr(34,26));
               GPSTK_THROW(e);
            }
         }
         else
         {
            record.startEpoch = CommonTime::BEGINNING_OF_TIME;
            if ( asInt(line.substr(34,26)) )
            {  // stop epoch w/o start epoch
               FFStreamError e("Invalid Start/Stop Epoch start: " +
                               line.substr(7,26) + ", stop: " +
                               line.substr(34,26));
               GPSTK_THROW(e);
            }
            else
            {
               record.stopEpoch = CommonTime::BEGINNING_OF_TIME;
            }
         }
            // add the ref clk record to the list
         refClkList.push_back(record);

         valid |= numRefClkValid;

      }
         /// ANALYSIS CLK REF
      else if (label == analysisClkRefString)
      {
         if ( refClkList.empty() )
         {  // empty list - throw
            FFStreamError e("\"ANALYSIS CLK REF\" record without previous "
                            "\"# OF CLK REF\" record.");
            GPSTK_THROW(e);
         }

            // get the previous reference clock record
         std::list<RefClkRecord>::iterator itr = refClkList.end();
         --itr;

         if ( itr->numClkRef <= itr->clocks.size() )
         {  // Excessive # of clock references - throw
            FFStreamError e("\"ANALYSIS CLK REF\" entry exceeds "
                            "\"# of CLK REF\": " + asString(itr->numClkRef));
            GPSTK_THROW(e);
         }

         RefClk refclk;
         refclk.name = line.substr(0,4);
         refclk.number = strip(line.substr(5,20));
         refclk.clkConstraint = line.substr(40,19);
         itr->clocks.push_back(refclk);

      }
         /// # OF SOLN STA / TRF
      else if (label == numStationsString)
      {
         numSta = asInt( line.substr(0,6) );
         trf = strip(line.substr(10,50));

         valid |= numStationsValid;

      }
         /// SOLN STA NAME / NUM
      else if (label == solnStaNameString)
      {
         SolnSta solnSta;

         solnSta.name = line.substr(0,4);
         solnSta.number = strip(line.substr(5,20));
         solnSta.posX = strtoll(strip(line.substr(25,11)).c_str(), 0, 10);
         solnSta.posY = strtoll(strip(line.substr(37,11)).c_str(), 0, 10);
         solnSta.posZ = strtoll(strip(line.substr(49,11)).c_str(), 0, 10);

         solnStaList.push_back(solnSta);

         valid |= solnStaNameValid;

      }
         // # OF SOLN SATS
      else if (label == numSatsString)
      {
         numSats = asInt(line.substr(0,6));

         valid |= numSatsValid;

      }
         // PRN LIST
      else if (label == prnListString)
      {
         string s = line.substr(0,60);
         string word = stripFirstWord(s);

         while ( !word.empty() )
         {
            if ( word[0] == 'G' || word[0] == 'g' )
            {
               prnList.push_back(SatID(asInt(word.substr(1,2)),
                                       SatelliteSystem::GPS));
            }
            else if ( word[0] == 'R' || word[0] == 'r' )
            {
               prnList.push_back(SatID(asInt(word.substr(1,2)),
                                       SatelliteSystem::Glonass));
            }
            else
            {  // unknown satellite system - throw
               FFStreamError e("Invalid PRN: " + word);
               GPSTK_THROW(e);
            }

            word = stripFirstWord(s);
         }

         valid |= prnListValid;

      }
         // END OF HEADER
      else if (label == endOfHeader)
      {
         valid |= endValid;

      }
      else
      {  // invalid label - throw
         FFStreamError e("Invalid label: " + label);
         GPSTK_THROW(e);
      }

   }   // ParseHeaderRecord


}  // namespace
