#pragma ident "$Id: $"

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
 * @file PackedNavBits.cpp
 * Engineering units navigation message abstraction.
 */
#include <math.h>
#include <iostream>
#include <iomanip>

#include "PackedNavBits.hpp"
#include "GPSWeekSecond.hpp"
#include "CivilTime.hpp"
#include "YDSTime.hpp"
#include "icd_gps_constants.hpp"

namespace gpstk
{
   using namespace std;

   PackedNavBits::PackedNavBits()
                 :bits(900)
   {
      bits_used = 0;
   }

   void PackedNavBits::setSatID(const SatID satSysArg)
   {
      satSys = satSysArg;
      return;
   }

   void PackedNavBits::setObsID(const ObsID obsIDArg)
   {
      obsID = obsIDArg;
      return;
   }

   void PackedNavBits::setTime(const CommonTime TransmitTimeArg)
   {
      TransmitTime = TransmitTimeArg;
      return;
   }

   ObsID PackedNavBits::getobsID() const
   {
      return(obsID);
   }

   SatID PackedNavBits::getsatSys() const
   {
      return(satSys);
   }

   CommonTime PackedNavBits::getTransmitTime() const
   {
      return(TransmitTime);
   }

   size_t PackedNavBits::getNumBits() const
   {
      return(bits_used);
   }

   uint64_t PackedNavBits::asUint64_t(const int startBit, 
                                      const int numBits ) const
      throw(InvalidParameter)                                    
   {
      uint64_t temp = 0L;       // Set up a temporary variable with a known size
                                // It needs to be AT LEAST 33 bits.
      int stop = startBit + numBits;
      if (stop>bits.size())
      {
         InvalidParameter exc("Requested bits not present.");
         GPSTK_THROW(exc);
      }
      for (int i=startBit; i<stop; ++i)
      {
         temp <<= 1;
         if (bits[i]) temp++;
      }
      return( temp ); 
   }

   unsigned long PackedNavBits::asUnsignedLong(const int startBit, 
                                               const int numBits, 
                                               const int scale ) const
   {
      uint64_t temp = asUint64_t( startBit, numBits );
      unsigned long ulong = (unsigned long) temp;
      ulong *= scale; 
      return( ulong ); 
   }

   long PackedNavBits::asLong(const int startBit, const int numBits,   
                              const int scale) const
   {
      int64_t s = SignExtend( startBit, numBits);
      return( (long) (s * scale ) );
   }

   double PackedNavBits::asUnsignedDouble(const int startBit, const int numBits, 
                                          const int power2) const
   {
      uint64_t uint = asUint64_t( startBit, numBits );
      
         // Convert to double and scale
      double dval = (double) uint;
      dval *= pow(2, power2);
      return( dval );
   }

   double PackedNavBits::asSignedDouble(const int startBit, const int numBits,  
                                        const int power2 ) const
   {
      int64_t s = SignExtend( startBit, numBits);

         // Convert to double and scale
      double dval = (double) s;
      dval *= pow(2, power2);
      return( dval );
   }

   double PackedNavBits::asDoubleSemiCircles(const int startBits, const int numBits, 
                                             const int power2) const
   {
      double drad = asSignedDouble( startBits, numBits, power2);
      return (drad*PI);
   };

   std::string PackedNavBits::asString(const int startBit, const int numChars) const 
   {
      int CHAR_SIZE = 8;
      string out = " ";
      int currentStart = startBit;
      for (int i = 0; i < numChars; ++i)
      {
         uint64_t temp = asUint64_t(currentStart, CHAR_SIZE);
         char ch = (char) temp;
         out += ch;
         currentStart += CHAR_SIZE;
      }
      return(out);
   }

   void PackedNavBits::addUnsignedLong( const unsigned long value, 
                                        const int numBits,
                                        const int scale ) 
      throw(InvalidParameter)
   {
      uint64_t out = (uint64_t) value;
      out /= scale;

      uint64_t test = pow(2,numBits) - 1; 
      if ( out > test )
      {
         InvalidParameter exc("Scaled value too large for specifed bit length");
         GPSTK_THROW(exc);
      }
      addUint64_t( out, numBits ); 
   }  

   void PackedNavBits::addLong( const long value, const int numBits, const int scale ) 
      throw(InvalidParameter)
   {
      union
      {
         uint64_t u_out;
         int64_t out;
      };
      out = (int64_t) value;
      out /= scale;

      int64_t test = pow(2,numBits-1) - 1; 
      if ( ( out > test ) || ( out < -( test + 1 ) ) )
      {
         InvalidParameter exc("Scaled value too large for specifed bit length");
         GPSTK_THROW(exc);
      }
      addUint64_t( u_out, numBits ); 
   } 

   void PackedNavBits::addUnsignedDouble( const double value, const int numBits,
                                          const int power2 ) 
      throw(InvalidParameter)
   {
      uint64_t out = (uint64_t) ScaleValue(value, power2);
      uint64_t test = pow(2,numBits) - 1;
      if ( out > test )
      {
         InvalidParameter exc("Scaled value too large for specifed bit length");
         GPSTK_THROW(exc);
      }

      addUint64_t( out, numBits ); 
   } 

   void PackedNavBits::addSignedDouble( const double value, const int numBits,
                                        const int power2 ) 
      throw(InvalidParameter)
   {
      union
      {
         uint64_t u_out;
         int64_t out;
      };
      out = (int64_t) ScaleValue(value, power2);
      int64_t test = pow(2,numBits-1) - 1; 
      if ( ( out > test ) || ( out < -( test + 1 ) ) )
      {
         InvalidParameter exc("Scaled value too large for specifed bit length");
         GPSTK_THROW(exc);
      }
      addUint64_t( u_out, numBits ); 
   }

   void PackedNavBits::addDoubleSemiCircles( const double Radians, const int numBits, 
                                             const int power2)
      throw(InvalidParameter)
   {
      union
      {
         uint64_t u_out;
         int64_t out;
      };
      double temp = Radians/PI;
      out = (int64_t) ScaleValue(temp, power2);
      int64_t test = pow(2,numBits-1) - 1; 
      if ( ( out > test ) || ( out < -( test + 1 ) ) )
      {
         InvalidParameter exc("Scaled value too large for specifed bit length");
         GPSTK_THROW(exc);
      }
      addUint64_t( u_out, numBits );
   }

   void PackedNavBits::addString( const string String, const int numChars ) 
      throw(InvalidParameter)
   {
      int numPadBlanks = 0;
      int numToCopy = 0;
      if (numChars < String.length())
      {
         numPadBlanks = 0;
         numToCopy = numChars;
      }
      else if (numChars > String.length())
      {
         numToCopy = String.length();
         numPadBlanks = numChars - numToCopy;
      }
      else
      {
         numToCopy = numChars;
      }
      int i;
      for (i = 0; i < numToCopy; ++i)
      {
         char ch = String[i];
         bool valid = false;
         if ( ('A' <= ch && ch <= 'Z') || ('0' <= ch && ch <= ':') || (' ' == ch) ||
            ('"' == ch) || ('\'' == ch) || ('+' == ch) || ('-' <= ch && ch <= '/') ||
            (0xF8 == ch) ) valid = true;
         if (!valid)
         {
            InvalidParameter exc("Invalid character '<< ch <<' in text string. ");
            GPSTK_THROW(exc);
         }
         uint64_t out = (uint64_t) ch;
         addUint64_t(out, 8);
      }
      uint64_t space = 0x00000020;
      for (i = 0; i < numPadBlanks; ++i)
         addUint64_t(space, 8);
   }  

   void PackedNavBits::addUint64_t( const uint64_t value, const int numBits )
   {
      size_t ndx = bits_used;
      uint64_t mask = 0x0000000000000001L;
      mask <<= (numBits-1);
      for (int i=0; i<numBits; ++i)
      {
         bits[ndx] = false;
         if (value & mask) 
         {
             //set the bits to true
            bits[ndx] = true;
         }
         mask>>= 1;
         ndx++;
      }
      bits_used += numBits;
   }

   void PackedNavBits::trimsize()
   {
      bits.resize(bits_used);
   }

   int64_t PackedNavBits::SignExtend( const int startBit, const int numBits) const
   {
      union
      {
         uint64_t u;
         int64_t s;
      };
      u = asUint64_t( startBit, numBits);
      s <<= 64 - numBits; // Move sign bit to msb.
      s >>= 64- numBits;  // Shift result back to correct location sign bit extended.
      return (s);
   }

   double PackedNavBits::ScaleValue( const double value, const int power2) const
   {
      double temp = value;
      temp /= pow(2, power2);
      if (temp >= 0) temp += 0.5; // Takes care of rounding
      else temp -= 0.5;
      return ( temp );
   }

   static void timeDisplay( ostream & os, const CommonTime& t )
      {
         // Convert to CommonTime struct from GPS wk,SOW to M/D/Y, H:M:S.
         GPSWeekSecond dummyTime;
         dummyTime = GPSWeekSecond(t);
         os << dec;
         os << setw(4) << dummyTime.week << "(";
         os << setw(4) << (dummyTime.week & 0x03FF) << ")  ";
         os << setw(6) << setfill(' ') << dummyTime.sow << "   ";

         switch (dummyTime.getDayOfWeek())
         {
            case 0: os << "Sun-0"; break;
            case 1: os << "Mon-1"; break;
            case 2: os << "Tue-2"; break;
            case 3: os << "Wed-3"; break;
            case 4: os << "Thu-4"; break;
            case 5: os << "Fri-5"; break;
            case 6: os << "Sat-6"; break;
            default: break;
         }
         os << "   " << (static_cast<YDSTime>(t)).printf("%3j   %5.0s  ") 
            << (static_cast<CivilTime>(t)).printf("%02m/%02d/%04Y   %02H:%02M:%02S");
      }

   static void shortcut(ostream & os, const long HOW )
   {
      short DOW, hour, min, sec;
      long SOD, SOW;
      short SOH;
      
      SOW = static_cast<long>( HOW );
      DOW = static_cast<short>( SOW / SEC_PER_DAY );
      SOD = SOW - static_cast<long>( DOW * SEC_PER_DAY );
      hour = static_cast<short>( SOD/3600 );

      SOH = static_cast<short>( SOD - (hour*3600) );
      min = SOH/60;


      sec = SOH - min * 60;
      switch (DOW)
      {
         case 0: os << "Sun-0"; break;
         case 1: os << "Mon-1"; break;
         case 2: os << "Tue-2"; break;
         case 3: os << "Wed-3"; break;
         case 4: os << "Thu-4"; break;
         case 5: os << "Fri-5"; break;
         case 6: os << "Sat-6"; break;
         default: break;
      }

      os << ":" << setfill('0')
         << setw(2) << hour
         << ":" << setw(2) << min
         << ":" << setw(2) << sec
         << setfill(' ');
   }

   void PackedNavBits::dump(ostream& s) const
      throw()
   {
      ios::fmtflags oldFlags = s.flags();
   
      s.setf(ios::fixed, ios::floatfield);
      s.setf(ios::right, ios::adjustfield);
      s.setf(ios::uppercase);
      s.precision(0);
      s.fill(' ');
      
      s << "****************************************************************"
        << "************" << endl
        << "Packed Nav Bits" << endl
        << endl
        << "SatID: " << setw(4) << getsatSys() << endl
        << endl
        << "Carrier: " << ObsID::cbDesc[obsID.band] << "      "
        << "Code: " << ObsID::tcDesc[obsID.code] << endl << endl
        << "Number Of Bits: " << dec << getNumBits() << endl
        << endl;
  
      s << "              Week(10bt)     SOW     DOW   UTD     SOD"
        << "  MM/DD/YYYY   HH:MM:SS\n";
      s << "Clock Epoch:  ";

      timeDisplay(s, getTransmitTime());
      s << endl;     

      s << endl << "Packed Bits, Left Justified, 32 Bits Long:\n";
      int numBitInWord = 0;
      int word_count   = 0;
      uint32_t word    = 0;
      for(int i = 0; i < bits.size(); ++i)
      {
         word <<= 1;
         if (bits[i]) word++;
       
         numBitInWord++;
         if (numBitInWord >= 32)
         {
            s << "  0x" << setw(8) << setfill('0') << hex << word;
            numBitInWord = 0;
            word_count++;
            //Print four words per line 
            if (word_count %5 == 0) s << endl;        
         }
      }
      word <<= 32 - numBitInWord;
      if (numBitInWord > 0 ) s << "  0x" << setw(8) << setfill('0') << hex << word;
      s.setf(ios::fixed, ios::floatfield);
      s.precision(3);
   } // end of PackedNavBits::dump()
   
   ostream& operator<<(ostream& s, const PackedNavBits& eph)
   {
      eph.dump(s);
      return s;

   } // end of operator<<
   
} // namespace
