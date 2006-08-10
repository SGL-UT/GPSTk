#pragma ident "$Id$"


//lgpl-license START
//lgpl-license END

#include <StringUtils.hpp>
#include <BinUtils.hpp>

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
   // set to 3+ to add the tossed bytes whether or not they are bad
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

      if (fw != frameWord)
         return;

      const unsigned long MaxSOW=604800;
      if (sow100 == MaxSOW*100)
      {
         week += 1;
         sow100 = 0;
      }
      if (sow100 > MaxSOW*100 || week>5000)
         return;

      time.setGPSfullweek(week, double(sow100)/100);

      // only clear the these bits if this object isn't a leaf data member
      if (id==0)
         clearstate(fmtbit & crcbit);
   }
      

   //---------------------------------------------------------------------------
   // Compute and set the CRC in an encoded representation of this
   // object
   void MDPHeader::setCRC(string& str) const
      throw()
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
      unsigned short lcrc = computeCRC((const unsigned char*)str.c_str(),
                                       length, gpstk::BinUtils::CRCCCITT);

      // and place that value in the string
      unsigned short tmp=lcrc;
      hostToNet(tmp);
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
   void MDPHeader::reallyGetRecord(FFStream& ffs)
      throw(std::exception, gpstk::StringUtils::StringException, 
            gpstk::FFStreamError, gpstk::EndOfFile)
   {
      // Note that this will generate a bad_cast exception if it don't work.
      MDPStream& stream=dynamic_cast<MDPStream&>(ffs);

      // first, make sure the data is flagged bad.
      clear(fmtbit | lenbit | crcbit);

      if (typeid(*this) == typeid(MDPHeader))
      {
         // first reset the data status
         setstate(fmtbit | lenbit);
         
         // If a header has been read in but the corresponding body hasn't been
         // read, read and toss the body
         if (stream.streamState == MDPStream::gotHeader)
         {
            unsigned bodyLen = length-myLength;
            char *trash = new char[bodyLen];
            if (debugLevel>2)
               cout << "Reading to toss " << bodyLen << endl;
            stream.getData(trash, bodyLen);
            string body(trash, stream.gcount());
            delete trash;

            if (stream.fail())
               return;

            stream.streamState = MDPStream::gotBody;
            if (hexDump || debugLevel>2)
            {
               cout << "Tossing Record Number:" << stream.recordNumber << endl;
               StringUtils::hexDumpData(cout, stream.rawHeader+body);
            }
         }

         char buff[myLength];
         if (stream.streamState == MDPStream::outOfSync ||
             stream.streamState == MDPStream::gotBody)
         {
            stream.streamState = MDPStream::outOfSync;

            if (debugLevel>2)
               cout << "Reading frame word" << endl;;
            uint16_t fw=0;
            for (int i=0; i<128; i++)
            {
               fw = stream.getData<uint16_t>();
               fw = netToHost(fw);
               memcpy(buff, &fw, sizeof(fw));
               if (fw==frameWord)
                  break;
            }

            if (fw!=frameWord)
            {
               FFStreamError err("Failed to find frame word.");
               GPSTK_THROW(err);
            }
            else
            {
               if (debugLevel>2)
                  cout << "Reading header" << endl;
               // then read in the rest of a header
               stream.getData(buff+2, myLength-2);
               if (stream.fail())
                  return;

               stream.rawHeader = string(buff, myLength);
               decode(stream.rawHeader);
               stream.streamState = MDPStream::gotHeader;
               stream.header = *this;
            }
         }
      }
      else
      {
         // This object is not a header
         // Before reading this body in, we need to make sure that
         // we have received a header with the right message ID
         while (stream.header.id != id)
            MDPHeader::reallyGetRecord(stream);

         // Now get the header values from the most recently
         // read header
         MDPHeader& myHeader = dynamic_cast<MDPHeader&>(*this);
         myHeader = stream.header;
         const unsigned myLen = length - MDPHeader::myLength;

         // Read in the body of the message
         char *buff = new char[myLen];
         if (debugLevel>2)
            cout << "Reading body " << myLen << endl;
         stream.getData(buff, myLen);
         string me(buff, stream.gcount());
         delete buff;
         if (stream.fail())
            return;

         stream.streamState = MDPStream::gotBody;

         setstate(crcbit);
         checkCRC(stream.rawHeader+me);

         decode(me);
         
         if (debugLevel && (rdstate() || stream.rdstate()))
            MDPHeader::dump(cout);

         if (hexDump || (debugLevel>1 && rdstate()))
         {
            cout << "Record Number:" << stream.recordNumber << endl;
            StringUtils::hexDumpData(cout, stream.rawHeader+me);
         }
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
