#pragma ident "$Id$"


#include <sstream>
#include <BinUtils.hpp>
#include <StringUtils.hpp>
#include <gps_constants.hpp>
#include <EngNav.hpp>

#include "miscenum.hpp"
#include "MDPNavSubframe.hpp"
#include "MDPStream.hpp"

using gpstk::StringUtils::asString;
using gpstk::BinUtils::netToHost;
using gpstk::BinUtils::hostToNet;
using gpstk::BinUtils::encodeVar;
using gpstk::BinUtils::decodeVar;
using namespace std;

namespace gpstk
{
   //--------------------------------------------------------------------------
   MDPNavSubframe::MDPNavSubframe() 
      throw()
      : subframe(11), knownUpright(false), inverted(false)
   {
      id = myId;
   }

  
   //--------------------------------------------------------------------------
   string MDPNavSubframe::encode() const 
      throw()
   {
      string str;
      str += encodeVar<int8_t>(prn);
      str += encodeVar<int8_t>(carrier);
      str += encodeVar<int8_t>(range);
      str += encodeVar<int8_t>(nav);
         
      for(int i=1; i<11; i++)
         str += encodeVar<uint32_t>(subframe[i]);

      return str;
   } // MDPNavSubframe::encode()
      

   //--------------------------------------------------------------------------
   void MDPNavSubframe::decode(string str) 
      throw()
   {
      if (str.length() != myLength)
         return;

      clearstate(lenbit);

      prn     = static_cast<int>        (decodeVar<int8_t>(str));
      carrier = static_cast<CarrierCode>(decodeVar<int8_t>(str));
      range   = static_cast<RangeCode>  (decodeVar<int8_t>(str));
      nav     = static_cast<NavCode>    (decodeVar<int8_t>(str));

      for (int i=1; i<11; i++)
         subframe[i] = decodeVar<uint32_t>(str);
      
      if (prn > gpstk::MAX_PRN ||
          carrier >= ccMax ||
          range >= rcMax ||
          nav >= ncMax)
         return;

      clearstate(fmtbit);
   } // MDPNavSubframe::decode()


   //---------------------------------------------------------------------------
   // This cracks the sow from the handover word
   unsigned long MDPNavSubframe::getHOWTime() const throw()
   {
      uint32_t  itemp,iret;
      itemp = subframe[2];
      itemp >>= 13;
      itemp &= 0x0001FFFFL;
      iret = itemp;
      iret = iret * 6;
      
      return iret;
   }

   //---------------------------------------------------------------------------
   // This cracks the subframe ID from the handover word
   unsigned int MDPNavSubframe::getSFID() const throw()
   {
      uint32_t  itemp;
      short iret;
      itemp = subframe[2];
      itemp >>= 8;
      itemp &= 0x00000007L;
      iret = ( short) itemp;
      return iret;
   }
   
   //---------------------------------------------------------------------------
   // This cracks the SV (page) ID from an almanac subframe
   unsigned int MDPNavSubframe::getSVID() const throw()
   {
      if (getSFID() <4)
         return 0;
      else
         return (subframe[3] >> 22) & 0x3F;
   }

   /// Get bit 30 from the given subframe word
   inline uint32_t getd30(uint32_t sfword)
   {
      return (sfword & 0x01);
   }
   
   /// Get bit 29 from the given subframe word
   inline uint32_t getd29(uint32_t sfword)
   {
      return ((sfword & 0x02) >> 1);
   }


   //--------------------------------------------------------------------------
   void MDPNavSubframe::setUpright() throw()
   {
      if (knownUpright)
         return;

      uint32_t preamble = subframe[1] >> 22;
      if (preamble == 0x74)
      {
         for (int i = 1; i<=10; i++)
            subframe[i] = ~subframe[i] & 0x3fffffff;
         inverted = true;
      }

      preamble = subframe[1] >> 22;
      if (preamble != 0x8b)
         return;

      // note that this routine assumes that D30 from the previous subframe
      // is zero. That is why we start with the second word in the array
      for (int i=2; i<=10; i++)
         if (getd30(subframe[i-1]))
            subframe[i] = (~subframe[i] & 0x3fffffc0) | (subframe[i] & 0x3f);

      knownUpright=true;
   }


   //--------------------------------------------------------------------------
   bool MDPNavSubframe::checkParity() const throw()
   {
      uint32_t preamble = subframe[1] >> 22;
      if (debugLevel>1)
         cout << "preamble:" << hex << preamble << dec
              << " knownUpright:" << knownUpright
              << " inverted:" << inverted
              << endl;

      bool needsInversion = (preamble == 0x74);

      if (debugLevel>1)
         cout << "needsInversion:" << needsInversion << endl
              << "parities:";

      bool goodParity = true;
      for (int i=1; i<=10; i++)
      {
         uint32_t prev = i==1 ? 0 : subframe[i-1];
         uint32_t curr = subframe[i];
         if (needsInversion)
         {
            if (i>1)
               prev = ~prev & 0x3fffffff;
            curr = ~curr & 0x3fffffff;
         }
         bool D30 = getd30(prev);
         unsigned receivedParity = curr & 0x3f;
         unsigned computedParity = EngNav::computeParity(curr, prev, knownUpright);
         if (debugLevel>1)
            cout << i << ":" << receivedParity
              << "-" << computedParity << " ";
         if (receivedParity != computedParity)
            goodParity = false;
      }
      if (debugLevel>1)
         cout << endl;
      return goodParity;
   }


   //--------------------------------------------------------------------------
   void MDPNavSubframe::dump(ostream& out) const
      throw()
   {
      ostringstream oss;
      MDPHeader::dump(oss);

      short page = getSFID() > 3 ? ((getHOWTime()-6) / 30) % 25 + 1 : 0;

      oss << getName() << 0 <<":"
          << " PRN:" << prn
          << " CC:" << static_cast<int>(carrier)
          << " RC:" << static_cast<int>(range)
          << " NC:" << static_cast<int>(nav)
          << " SF:" << getSFID()
          << " PG:" << page
          << " I:" << inverted
          << " U:" << knownUpright
          << endl;

      oss << setfill('0') << hex;
      for(int i = 1; i < subframe.size(); i++)
      {
         if ((i % 5) == 1)
            oss << getName() << i << ": ";
         oss << setw(8) << uppercase << subframe[i] << "  ";
         if ((i % 5) == 0)
            oss << endl;
      }
      out << oss.str() << flush;
   }
      
} // namespace gpstk
