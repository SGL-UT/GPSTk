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
      : subframe(11), cooked(false), inverted(false)
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
   void MDPNavSubframe::cookSubframe() throw()
   {
      if (cooked)
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

      cooked = true;
   }

   // Print as a string of 1/0
   string asBin(unsigned v, const unsigned n)
   {
      char *s = new char[n+1];
      for (int i=0; i<n; i++,v=v>>1)
         if (v&1)
            s[i] = '1';
         else
            s[i] = '0';
      s[n] = 0;
      string ns(s);
      delete[] s;
      return ns;
   }

   //--------------------------------------------------------------------------
   bool MDPNavSubframe::checkParity() const throw()
   {
      uint32_t preamble = subframe[1] >> 22;
      bool needsInversion = (preamble == 0x74);
      if (debugLevel>2)
         cout << "preamble:" << hex << preamble << dec
              << " cooked:" << cooked
              << " inverted:" << inverted
              << " needsInversion:" << needsInversion
              << " parities:"
              << endl;

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
         unsigned computedParity = EngNav::computeParity(curr, prev, cooked);
         if (debugLevel>3)
            cout << i << ":" << asBin(receivedParity,6)
                 << "-" << asBin(computedParity,6) << " ";
         if (i==5 && debugLevel>1)
            cout << endl;
         if (receivedParity != computedParity)
            goodParity = false;

         // This seems to be required for pre-cooked bits but I don't understand
         // why...
         if (i == 1 && receivedParity == (~computedParity & 0x3f))
            goodParity = true;
      }
      if (debugLevel>2)
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
          << " C:" << cooked
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


   //--------------------------------------------------------------------------
   void dump(std::ostream& out, const EphemerisPages& pages) throw()
   {
      for (int sf=1; sf<=3; sf++)
      {
         EphemerisPages::const_iterator i = pages.find(sf);
         if (i == pages.end())
            out << "Missing subframe " << sf << endl;
         else
            i->second.dump(out);
      }
   }


   //--------------------------------------------------------------------------
   void dump(std::ostream& out, const AlmanacPages& pages) throw()
   {
      for (int p=1; p<=25; p++)
      {
         for (int sf=4; sf<=5; sf++)
         {
            AlmanacPages::const_iterator i = pages.find(SubframePage(sf, p));
            if (i == pages.end())
               out << "Missing subframe " << sf << " page " << p << endl;
            else
               i->second.dump(out);

         }
      }
   }  
} // namespace gpstk
