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

#include <cstring>

#include "StringUtils.hpp"
#include "BinUtils.hpp"

#include "MDPHeader.hpp"
#include "MDPStream.hpp"

using namespace std;

using gpstk::BinUtils::computeCRC;
using gpstk::StringUtils::asString;
using gpstk::StringUtils::d2x;
using gpstk::StringUtils::int2x;
using gpstk::BinUtils::netToHost;
using gpstk::BinUtils::hostToNet;
using gpstk::BinUtils::encodeVar;
using gpstk::BinUtils::decodeVar;


namespace gpstk
{
   const unsigned MDPHeader::myLength = 16;
   const unsigned MDPHeader::frameWord = 0x9c9c;

   // Set to zero for no debugging output
   // set to 1 to output text messages about decode/format/range errors
   // set to 2 to add a hex dump of those messages
   // set to 3+ to add the tossed bytes whether or not they are bad and informational
   //   messages about the state of the parsing
   int MDPHeader::debugLevel = 0;

   // set true to print a hex dump of every message to cout
   bool MDPHeader::hexDump = false;

   //---------------------------------------------------------------------------
   string MDPHeader::encode() const 
      throw()
   {
      short week=time.GPSfullweek();
      unsigned long sow100=static_cast<unsigned long>(
         0.5 + time.GPSsecond() * 100);
      if (sow100==60480000)
      {
         sow100=0;
         week++;
      }

      string str;
      str += encodeVar<uint16_t>(frameWord);
      str += encodeVar<uint16_t>(id);
      str += encodeVar<uint16_t>(length);
      str += encodeVar<uint16_t>(week);
      str += encodeVar<uint32_t>(sow100);
      str += encodeVar<uint16_t>(freshnessCount);
      str += encodeVar<uint16_t>(0); // placeholder for the CRC

      // No, the CRC is not computed here. That needs to be done after
      // the body of the message has been encoded.
      return str;
   }

      
   //---------------------------------------------------------------------------
   void MDPHeader::decode(string str)
      throw() 
   {
      if (str.length() != myLength)
         return;

      clearstate(lenbit);

      unsigned short fw    = decodeVar<uint16_t>(str);
      id                   = decodeVar<uint16_t>(str);
      length               = decodeVar<uint16_t>(str);
      unsigned short week  = decodeVar<uint16_t>(str);
      unsigned long sow100 = decodeVar<uint32_t>(str);
      freshnessCount       = decodeVar<uint16_t>(str);
      crc                  = decodeVar<uint16_t>(str);

      const unsigned long MaxSOW=604800;
      if (fw != frameWord ||
          sow100 > MaxSOW*100 || week>5000 ||
          length > 1024 ||
          id > 1024)
      {
         if (debugLevel>1)
         {
            cout << "Insane header:" << endl;
            dump(cout);
         }
         length=0;
         return;
      }

      if (sow100 == MaxSOW*100)
      {
         week += 1;
         sow100 = 0;
      }

      time.setGPSfullweek(week, double(sow100)/100);

      clearstate(fmtbit);

      // only clear the this bit if this object isn't a leaf data member
      if (id==0)
         clearstate(crcbit);
   }
      

   //---------------------------------------------------------------------------
   // Compute and set the CRC in an encoded representation of this
   // object
   void MDPHeader::setCRC(string& str) const
      throw(FFStreamError)
   {
      // Here we make sure that the length of the string matches
      // the length in the header.
      if(str.length() != length)
      {
         FFStreamError e("Message Length should be >= " + 
                         asString(length) + ".  Was: " +
                         asString(str.length()));
         GPSTK_THROW(e);
      }

      // clear out the spot for the crc
      str.replace(14, 2, 2, (char)0);
         
      // calculate the crc on the string
      crc = computeCRC((const unsigned char*)str.c_str(),
                       length, gpstk::BinUtils::CRCCCITT);

      // and place that value in the string
      unsigned short tmp = hostToNet(crc);
      str.replace(14, 2, (char*)&tmp, 2);
   } // MDPHeader::encode()
      

   //---------------------------------------------------------------------------
   // Compute the CRC of the string and set the crcbit appropriately.
   void MDPHeader::checkCRC(string str)
      throw()
   {
      // zero the CRC in the incoming string
      str.replace(14, 2, 2, (char)0);
         
      unsigned short ccrc1 = computeCRC((const unsigned char*)str.c_str(), 
                                        length, gpstk::BinUtils::CRCCCITT);
      if (ccrc1 == crc)
      {
         clearstate(crcbit);
         return;
      }

      // This crc will also be accepted until such time as it is no longer needed
      uint16_t ccrc2 = computeCRC((const unsigned char*)str.c_str(), 
                                  length, gpstk::BinUtils::CRC16);
      if (ccrc2 == crc)
      {
         clearstate(crcbit);
         return;
      }

      if (debugLevel)
         cout << "Bad CRC.  Received " << hex << crc
              << " computed " << ccrc1
              << " and " << ccrc2
              << ". Message ID=" << dec << id << endl;
   } // MDPHeader::checkCRC()


   //---------------------------------------------------------------------------
   void MDPHeader::reallyPutRecord(FFStream& ffs) const
      throw(std::exception, gpstk::StringUtils::StringException, 
            gpstk::FFStreamError)
   {
      if (typeid(*this) == typeid(MDPHeader))
      {
         gpstk::FFStreamError e("Directly writing an MDPHeader object to an"
                                " FFStream is not supported.");
         GPSTK_THROW(e);
      }

      MDPStream& stream = dynamic_cast<MDPStream&>(ffs);

      string body = encode();
      length = body.length() + MDPHeader::myLength;

      string str = MDPHeader::encode() + body;
      setCRC(str);
      
      stream << str;

      if (hexDump)
      {
         cout << endl;
         StringUtils::hexDumpData(cout, str);
      }
   } // MDPHeader::reallyPutRecord()


   //---------------------------------------------------------------------------
   void MDPHeader::readHeader(MDPStream& ffs)
      throw(FFStreamError, EndOfFile)
   {
      if (ffs.streamState == MDPStream::gotHeader)
         readBody(ffs);

      // first reset the data status
      char buff[MDPHeader::myLength];
      ffs.streamState = MDPStream::outOfSync;
    
      streampos p0 = ffs.tellg();
      uint16_t fw=0;
      const int maxTries=1024;
      uint16_t  b2[maxTries];
      int i;
      for (i=0; ffs && i<maxTries; i++)
      {
         fw = ffs.getData<uint16_t>();
         b2[i] = fw;
         fw = netToHost(fw);
         if (fw!=MDPHeader::frameWord)
            continue;
         std::memcpy(buff, &fw, sizeof(fw));
         break;
      }
      streampos p1 = ffs.tellg();

      if ((hexDump || debugLevel) && i>0)
      {
         string s((const char*)b2, i*2) ;
         cout << "Skipped " << s.size() << " bytes looking for frame word" << endl;
         if (debugLevel>1)
            StringUtils::hexDumpData(cout, s);
      }

      if (fw != MDPHeader::frameWord)
      {
         if (debugLevel>1)
            cout << "Failed to find frame word between " << hex << p0
                 << " to " << p1 << dec << endl;
         return;
      }

      p1-=2;
      if (debugLevel>2)
         cout << "Found frame word at " << hex << p1 << dec << endl;

      // then read in the rest of a header
      ffs.getData(buff+2, MDPHeader::myLength-2);
      if (ffs.fail())
         return;
         
      ffs.rawHeader = string(buff, MDPHeader::myLength);
      MDPHeader::decode(ffs.rawHeader);
      ffs.streamState = MDPStream::gotHeader;
      ffs.header = *this;
      if (debugLevel>2)
      {
         cout << "Got header at " << ffs.tellg()
              << " for id=" << id
              << " body, length=" << length << endl;
         StringUtils::hexDumpData(cout, ffs.rawHeader);
      }
   }


   //---------------------------------------------------------------------------
   string MDPHeader::readBody(MDPStream& ffs)
      throw(FFStreamError, EndOfFile)
   {
      if (ffs.streamState != MDPStream::gotHeader)
         return string();

      // Need to make sure we have a 'sane' length before we continue reading
      if (length <= MDPHeader::myLength)
      {
         if (debugLevel)
            cout << "Received a runt message at " << hex << ffs.tellg() 
                 << ", ignoring body" << dec << endl;
         ffs.streamState = MDPStream::gotBody;
         return string();
      }

      // Read in the body of the message
      const unsigned myLen = length - MDPHeader::myLength;
      char *buff = new char[myLen];
      if (debugLevel>2)
         cout << "Reading " << myLen 
              << " bytes for message id " << id
              << " body at offset " << hex <<  ffs.tellg() << dec << endl;
      ffs.getData(buff, myLen);
      if (ffs.gcount() == myLen)
         ffs.streamState = MDPStream::gotBody;
      string str(buff, ffs.gcount());
      delete buff;
      return str;
   }


   //---------------------------------------------------------------------------
   void MDPHeader::reallyGetRecord(FFStream& ffs)
      throw(std::exception, gpstk::StringUtils::StringException, 
            gpstk::FFStreamError, gpstk::EndOfFile)
   {
      // Note that this will generate a bad_cast exception if it doesn't work.
      MDPStream& stream=dynamic_cast<MDPStream&>(ffs);

      unsigned reqId = id;
      if (typeid(*this) == typeid(MDPHeader))
         reqId = 0;
      if (debugLevel>2)
         cout << "Reading at " << stream.tellg() 
              << " for id=" << reqId
              << " streamState=" << stream.streamState << endl;

      // first, make sure the data is flagged bad.
      clear(fmtbit | lenbit | crcbit);

      // If we already have a reasonable header for the desired message,
      // then we can skip reading a new header. If a header is the desired
      // message, then always read a new header.
      bool haveDesiredHeader = stream.header.id == reqId
         && reqId != 0
         && stream.streamState == MDPStream::gotHeader
         && stream.header.length > 0;
      if (haveDesiredHeader)
      {
         *this = stream.header;
         if (debugLevel>2)
         {
            cout << "Using header from previous reallyGetRecord:" << endl;
            stream.header.dump(cout);
         }
      }
      else
      {
         while (!haveDesiredHeader && stream)
         {
            readHeader(stream);
            if (stream.streamState != MDPStream::gotHeader)
               break;
            haveDesiredHeader = (stream.header.id == reqId || reqId == 0)
               && stream.header.length > 0;
         }
      }

      if (!haveDesiredHeader)
      {
         if (debugLevel)
            cout << "Failed to get header" << endl;
         return;
      }

      // We are done if this object is a MDPHeader
      if (reqId == 0)
      {
         setstate(crcbit|lenbit|fmtbit);
         return;
      }

      // read in the message body
      string body = readBody(stream);
      if (!stream || body.size() == 0)
         return;

      setstate(crcbit);
      checkCRC(stream.rawHeader+body);

      decode(body);

      if (debugLevel && (rdstate() || stream.rdstate()))
         MDPHeader::dump(cout);

      if (hexDump || (debugLevel>1 && rdstate()))
      {
         cout << "Record Number:" << stream.recordNumber << endl;
         StringUtils::hexDumpData(cout, stream.rawHeader+body);
      }
   } // MDPHeader::reallyGetRecord()


   //---------------------------------------------------------------------------
   void MDPHeader::dump(ostream& out) const
      throw()
   {
      ostringstream oss;
      oss << getName() << " :"
          << " ID:" << id
          << " Len:" << length
          << " Time:" << time.printf("%4Y/%03j/%02H:%02M:%05.2f")
          << " FC:" << hex << setfill('0') << setw(4) << freshnessCount
          << " crc:" << setw(4) << crc
          << " rdstate:" << rdstate();

      if (crcerr())
         oss << "-crc";
      if (fmterr())
         oss << "-fmt";
      if (lenerr())
         oss << "-len";
      if (parerr())
         oss << "-par";

      out << oss.str() << endl;
   }  // MDPHeader::dump()

} // namespace gpstk
