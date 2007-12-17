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

#include "StringUtils.hpp"
#include "BinUtils.hpp"

#include "ATSData.hpp"
#include "ATSStream.hpp"

using namespace std;

using gpstk::BinUtils::computeCRC;
using gpstk::StringUtils::asString;
using gpstk::StringUtils::d2x;
using gpstk::StringUtils::int2x;


namespace gpstk
{
   // Set to zero for no debugging output
   // set to 1 to output text messages about decode/format/range errors
   // set to 2 to add a hex dump of those messages
   // set to 3+ to add the tossed bytes whether or not they are bad
   int ATSData::debugLevel = 0;

   // set true to print a hex dump of every message to cout
   bool ATSData::hexDump = false;
   const uint8_t ATSData::MaxNumChan = 48;
   const uint8_t ATSData::MaxNumSubChan = 12;

   //---------------------------------------------------------------------------
   void ATSData::reallyPutRecord(FFStream& ffs) const
      throw(std::exception, gpstk::StringUtils::StringException, 
            gpstk::FFStreamError)
   {
      if (typeid(*this) == typeid(ATSData))
      {
         gpstk::FFStreamError e("Directly writing an ATSData object to an"
                                " FFStream is not supported.");
         GPSTK_THROW(e);
      }

      ATSStream& stream = dynamic_cast<ATSStream&>(ffs);

      string str;
      stream << str;

      if (hexDump)
      {
         cout << endl;
         StringUtils::hexDumpData(cout, str);
      }
   } // ATSData::reallyPutRecord()

   template <class T>
   T decodeVar( std::string& str, std::string::size_type pos = std::string::npos)
   {
      T t;
      char *cp = reinterpret_cast<char*>( &t );
      
      if (pos == std::string::npos)
      {
         str.copy( cp, sizeof(T) );
         t = gpstk::BinUtils::intelToHost( t );
         str.erase( 0, sizeof(T) );
      }
      else
      {
         str.copy( cp, sizeof(T) , pos);
         t = gpstk::BinUtils::intelToHost( t );
      }
      return t;
   }

   //---------------------------------------------------------------------------
   void ATSData::reallyGetRecord(FFStream& ffs)
      throw(std::exception, gpstk::StringUtils::StringException, 
            gpstk::FFStreamError, gpstk::EndOfFile)
   {
      // Note that this will generate a bad_cast exception if it doesn't work.
      ATSStream& stream=dynamic_cast<ATSStream&>(ffs);

      char tmp;
      stream.getData(static_cast<char*>(&tmp), 1);
      if (!stream)
      {
         FFStreamError err("Error reading stream.");
         GPSTK_THROW(err);
      }
      numChan = tmp;

      if (numChan > MaxNumChan)
      {
         FFStreamError err("Channel count error: " +
                           StringUtils::asString((int)numChan)
                           + " > " + StringUtils::asString((int)MaxNumChan));
         GPSTK_THROW(err);
      }

      stream.getData(&tmp, 1);
      if (!stream)
      {
         FFStreamError err("Error reading stream.");
         GPSTK_THROW(err);
      }
      numSubChan = tmp;

      if (numSubChan > MaxNumSubChan)
      {
         FFStreamError err("Sub channel count error: " +
                           StringUtils::asString(numChan)
                           + " > " + StringUtils::asString(MaxNumSubChan));
         GPSTK_THROW(err);
      }

      if (debugLevel>2)
         cout << "numChan:"<< (int)numChan
              << ", numSubChan:" << (int)numSubChan << endl;
      size_t recSize = numChan * (9 + numSubChan * 65);

      char *buff = new char [recSize];
      stream.getData(buff, recSize);
      if (!stream)
      {
         FFStreamError err("Error reading stream.");
         GPSTK_THROW(err);
      }

      string str(buff, recSize);
      delete buff;
      stream.rawData = string(1,numChan) + string(1,numSubChan) + str;

      if (channels.size() != numChan)
         channels.resize(numChan);

      for (int i=0; i<numChan; i++)
      {
         ChannelBlock& cb = channels[i];
         int prn = decodeVar<uint8_t>(str);
         cb.svid = SatID(prn, SatID::systemGPS);
         cb.absTime = decodeVar<double>(str);
         if (cb.subChannels.size() != numSubChan)
            cb.subChannels.resize(numSubChan);
         for (int j=0; j<numSubChan; j++)
         {
            SubChannelBlock& scb = cb.subChannels[j];;
            scb.pseudorange = decodeVar<double>(str) + stream.rangeBias[i];
            scb.phase = decodeVar<double>(str);
            scb.rangeRate = decodeVar<double>(str);
            scb.cn0 = decodeVar<double>(str);
            scb.flags = decodeVar<uint8_t>(str);
            for (int k=0; k<4; k++)
               scb.navMSB[k] = decodeVar<uint32_t>(str);
            for (int k=0; k<4; k++)
               scb.navLSB[k] = decodeVar<uint32_t>(str);;
         }
      }

      if (debugLevel && stream.rdstate())
         ATSData::dump(cout);

      if (hexDump)
      {
         cout << "Record Number:" << stream.recordNumber << endl;
         StringUtils::hexDumpData(cout, stream.rawData);
      }
   } // ATSData::reallyGetRecord()

   //---------------------------------------------------------------------------
   void ATSData::dump(ostream& out, int detail) const
      throw()
   {
      ostringstream oss;
      if (detail)
         oss << getName() << " :"
             << " numChan:" << (int)numChan
             << " numSubChan:" << (int)numSubChan
             << endl;
      for (int i = 0; i < channels.size(); i++)
      {
         const ChannelBlock& cb = channels[i];
         if (detail)
            oss << getName() << " : prn:" << cb.svid
                << " absTime:" << setprecision(15) << cb.absTime << endl;
         for (int j=0; j<numSubChan; j++)
         {
            if (detail)
            {
               oss << getName() << setprecision(12)
                   << " : range:" << cb.subChannels[j].pseudorange
                   << " phase:" << cb.subChannels[j].phase
                   << " rangeRate:" << cb.subChannels[j].rangeRate
                   << setprecision(4)
                   << " cn0:" << cb.subChannels[j].cn0
                   << " flags:" << hex << (int)cb.subChannels[j].flags
                   << endl
                   << "   ";
               for (int k=0; k<4; k++)
               {
                  if (k>0)
                     oss << ", ";
                  oss << cb.subChannels[j].navMSB[k]
                      << " " << cb.subChannels[j].navLSB[k];
                  oss << dec << endl;
               }
            }
            else
            {
               short week = static_cast<short>(cb.absTime / DayTime::FULLWEEK);
               double sow = cb.absTime - week * DayTime::FULLWEEK;
               oss << left << setw(4) << week << right << setprecision(9)
                   << " " << setw(7) << sow
                   << "    " << setw(5) << cb.svid.id
                   << "  " << setprecision(14) << setw(18)
                   << cb.subChannels[j].pseudorange
                   << " " << setprecision(9) << setw(14) 
                   << cb.subChannels[j].rangeRate
                   << "  " << setprecision(14) << setw(18)
                   << cb.subChannels[j].phase
                   << setprecision(3)
                   << "  " << setw(4) << cb.subChannels[j].cn0
                   << "  " << hex << (int)cb.subChannels[j].flags << dec
                   << endl;
            }
         }
      }
      out << oss.str() << endl;
   }  // ATSData::dump()
   
} // namespace gpstk
