#pragma ident "$Id$"


#include <sstream>
#include <BinUtils.hpp>
#include <StringUtils.hpp>
#include <gps_constants.hpp>

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
   //---------------------------------------------------------------------------
   MDPNavSubframe::MDPNavSubframe() 
      throw()
      : subframe(11)
   {
      id = myId;
   }

  
   //---------------------------------------------------------------------------
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
      

   //---------------------------------------------------------------------------
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

   //---------------------------------------------------------------------------
   void MDPNavSubframe::dump(ostream& out) const
      throw()
   {
      ostringstream oss;
      MDPHeader::dump(oss);
      oss << getName() << 0 <<":"
          << " PRN:" << prn
          << " CC:" << static_cast<int>(carrier)
          << " RC:" << static_cast<int>(range)
          << " NC:" << static_cast<int>(nav)
          << endl;

      oss << setfill('0') << hex;
      for(int i = 1; i < subframe.size(); i++)
      {
         if ((i % 5) == 1)
            oss << getName() << i <<": ";
         oss << setw(8) << uppercase << subframe[i] << "  ";
         if ((i % 5) == 0)
            oss << endl;
      }
      out << oss.str() << flush;
   }
      
} // namespace gpstk
