//============================================================================
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

#include "AshtechData.hpp"
#include "AshtechStream.hpp"

using namespace std;

namespace gpstk
 {
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    // This is the string that preceeds every message from the receiver.
    const string AshtechData::preamble("$PASHR,");

    // This is the string that is at the end of every message.
    const string AshtechData::trailer("\015\012");

    // Set to zero for no debugging output
    // set to 1 to output text messages about decode/format/range errors
    // set to 2 to add a hex dump of those messages
    // set to 3+ to add the tossed bytes whether or not they are bad
    int AshtechData::debugLevel = 0;

    // set true to print a hex dump of every message to cout
    bool AshtechData::hexDump = false;


    //---------------------------------------------------------------------------
    void AshtechData::reallyGetRecord(FFStream& ffs)
       throw(exception, FFStreamError, EndOfFile)
    {
       // Note that this will generate a bad_cast exception if it doesn't work.
       AshtechStream& stream=dynamic_cast<AshtechStream&>(ffs);

       // make sure the object is reset before starting the search
       clear(fmtbit | lenbit | crcbit);
       id.clear();

       readHeader(stream);
    } // AshtechData::reallyGetRecord()


    //---------------------------------------------------------------------------
    void AshtechData::readHeader(AshtechStream& stream)
       throw(FFStreamError, EndOfFile)
    {
       string& rawData = stream.rawData;
       size_t i;

       while (stream)
       {
          if (rawData.length() < preamble.length()+4)
          {
             char buff[512];
             stream.read(buff, sizeof(buff));
             rawData.append(buff, stream.gcount());
          }

          if (stream.header)
             i = rawData.find(preamble, preamble.length());
          else
             i = rawData.find(preamble);
          stream.header = false;

          if (i)
          {
             i = min (rawData.length(), i);
             if (debugLevel>2)
                cout << "Tossing " << i
                     << " bytes at offset: 0x" << hex << stream.getRawPos() << dec
                     << endl;
             if (hexDump)
                StringUtils::hexDumpData(cout, rawData.substr(0,i));
             rawData.erase(0, i);
          }
          else
          {
             id = rawData.substr(7,3);
             break;
          }
       }
       stream.header = true;
    }

    //---------------------------------------------------------------------------
    void AshtechData::readBody(AshtechStream& stream)
       throw(FFStreamError, EndOfFile)
    {
       string& rawData = stream.rawData;
       const static string term = trailer+preamble;
       size_t term_pos = rawData.find(term);

       while (stream)
       {
          term_pos = rawData.find(term);
          if (term_pos > 0 && term_pos < rawData.length())
             break;

          if (stream)
          {
             char cbuff[512];
             stream.read(cbuff, sizeof(cbuff));
             rawData.append(cbuff, stream.gcount());
          }
          else
             break;
       }

       term_pos += trailer.length();
       if (hexDump)
          StringUtils::hexDumpData(cout, rawData.substr(0,term_pos));

       decode(rawData.substr(0, term_pos));

       if (!good() && debugLevel>1)
          cout << "bad decode starting at at offset 0x"
               << hex << stream.getRawPos() << dec
               << endl;

       rawData.erase(0, term_pos);
       stream.header=false;
    }


    //---------------------------------------------------------------------------
    void AshtechData::dump(ostream& out) const throw()
    {
       ostringstream oss;
       oss << getName() << " : id:" << id
           << " checksum:" << hex << checksum
           << " rdstate:" << rdstate() << dec;
       if (crcerr())
          oss << "-crc";
       if (fmterr())
          oss << "-fmt";
       if (lenerr())
          oss << "-len";
       if (parerr())
          oss << "-par";

       out << oss.str() << endl;
    }  // AshtechData::dump()
} // namespace gpstk
