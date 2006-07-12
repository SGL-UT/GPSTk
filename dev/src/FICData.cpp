#pragma ident "$Id: //depot/sgl/gpstk/dev/src/FICData.cpp#11 $"

/**
 * @file FICData.cpp
 * gpstk::FICData - container for the FIC file data.
 */

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
#include "FICData.hpp"
#include "FICStream.hpp"
#include "FICAStream.hpp"

#include <cmath>

#include "gpstkplatform.h"

using namespace gpstk::StringUtils;
using namespace std;
using namespace gpstk::BinUtils;

   // the "blk " literal is 4 chrs wide
const int FICBlkHdrSize = 4;

namespace gpstk
{
   const string FICData::blockString = "BLK ";

   void FICData::reallyPutRecord(FFStream& ffs) const
      throw(std::exception, gpstk::StringUtils::StringException, 
            gpstk::FFStreamError)
   {
      if (!isValid())
      {
         FFStreamError e("Tried to write an invalid FIC block");
         GPSTK_THROW(e);
      }
      
         // is this a FIC or FICA stream?
         // remember that a FICAStream derives from FICStream, so check
         // for FICA first
      if (dynamic_cast<FICAStream*>(&ffs))
      {
         FICAStream& strm = dynamic_cast<FICAStream&>(ffs);
         
         strm << '\n' << blockString;
         strm << setw(6) << blockNum 
              << setw(5) << f.size()
              << setw(5) << i.size()
              << setw(5) << c.size();
         unsigned long index;
         for (index = 0; index < f.size(); index++)
         {
            if ((index % 4) == 0)
               strm << '\n';
            double l10 = log10(fabs(f[index]));
            if ( ((l10 >= 100) || (l10 <= -100)) && (f[index] != 0))
               strm << doub2for(f[index], 20, 3);
            else
               strm << doub2for(f[index], 20, 2);
         }
         for (index = 0; index < i.size(); index++)
         {
            if ((index % 6) == 0)
               strm << '\n';
            strm << setw(12) << i[index];
         }
         for (index = 0; index < c.size(); index++)
         {
            if ((index % 8) == 0)
               strm << '\n';
            strm << setw(8) << c[index];
         }
      }
      else
      {
         if (dynamic_cast<FICStream*>(&ffs))
         {
            FICStream& strm = dynamic_cast<FICStream&>(ffs);
            
            strm << blockString;
            strm.writeData<int32_t>(hostToIntel(blockNum));
            strm.writeData<uint32_t>(hostToIntel(static_cast<uint32_t>(f.size())));
            strm.writeData<uint32_t>(hostToIntel(static_cast<uint32_t>(i.size())));
            strm.writeData<uint32_t>(hostToIntel(static_cast<uint32_t>(c.size())));
            unsigned long index;
            for (index = 0; index < f.size(); index++)
               strm.writeData<double>(hostToIntel(f[index]));
            for (index = 0; index < i.size(); index++)
               strm.writeData<int32_t>(hostToIntel(i[index]));
            for (index = 0; index < c.size(); index++)
               strm.writeData<char>(hostToIntel(c[index]));
         }
            // bad cast!!!
         else
         {
            FFStreamError e("FICData tried to read from a non-FIC file");
            GPSTK_THROW(e);
         }         
      }
   }

   bool FICData::isValid() const
   {
      switch(blockNum) {
         case 109:
            if((f.size() == 0) && (i.size() == 32) && (c.size() == 0))
               return true;
            break;
         case 9:
            if((f.size() == 60) && (i.size() == 0) && (c.size() == 0))
               return true;
            break;
         case 162:
            if((f.size() == 0) && (i.size() == 15) && (c.size() == 0))
               return true;
            break;
         case 62:
            if(( (f.size() == 20) || (f.size() == 32) || (f.size() == 14) || 
                 (f.size() == 23) || (f.size() == 47) || (f.size() == 29)   ) 
               && (i.size() == 6) && (c.size() == 0))
               return true;
            break;
         default:
            return false;
            break;
      }
      return false;
   }
    
   void FICData::dump(ostream& s) const 
   {
      int n;
      s << "FIC BlockNumber: " << blockNum << endl
        << " floats:   " << flush;
      for(n = 0; n< f.size(); n++)
         s << f[n] << " " << flush;
      s << endl << " integers: " << flush;
      for(n = 0; n< i.size(); n++)
         s << i[n] << " " << flush;
      s << endl << " chars:    " << flush;
      for(n = 0; n< c.size(); n++)
         s << c[n] << " " << flush;
      s << endl << endl;
   };

   void FICData::prettyDump(ostream& os) const
   {
      switch (blockNum)
      {
         case 109:
            prettyDump109(os);
            break;
         case 9:
            prettyDump9(os);
            break;
         case 62:
            prettyDump62(os);
            break;
         case 162:
            prettyDump162(os);
            break;
         default:
            break;
      }
   }

   void FICData::prettyDump9(ostream& os) const
   {
      short j;
      short ndx[3] = { 9, 25, 52 };
      short iod;
      short itemp;
      short epochWeek;
      double epochTime, xmitTime, diff;

      os << "**************************************";
      os << "**************************************\n";
      os << "   Block Number : ";
      os.width(3);
      os << blockNum << "\n";
      os << "Broadcast Ephemeris (Engineering Units)\n";
      os << "\n";
      os << "PRN : ";
      os.width(2);
      os << f[19] << "\n\n";

         // Check for week rollover between nav msg week # and
         // epoch time week number.
      epochTime = f[12];
      xmitTime = f[2];
      diff = -1 * (epochTime - xmitTime);
      if (diff > DayTime::HALFWEEK) epochWeek = (short) f[5] + 1;
      else epochWeek = (short) f[5];
      timeDisplay( os, "Clock Epoch:", epochWeek, f[12], 1 );

      epochTime = f[33];
      xmitTime = f[22];
      diff = -1 * (epochTime - xmitTime);
      if (diff > DayTime::HALFWEEK) epochWeek = (short) f[5] + 1;
      else epochWeek = (short) f[5];
      timeDisplay( os, "Eph Epoch:",   epochWeek, f[33], 0 );

      os << "Transmit Week:";
      os.width(4);
      os << f[5] << "\n";
      os.width(2);
      os << "Fit interval flag :  " << f[34] << "\n";

      os << "\n";
      os << "          SUBFRAME OVERHEAD\n\n";
      os << "               SOW    DOW:HH:MM:SS     IOD    ALERT   A-S\n";
      for (j=0;j<3;j++)
      {
         os.width(1);
         os << "SF" << (j+1) << " HOW:   ";
         os.width(7);
         os << f[2+j*20];

            // Convert SOW to D:H:M:S and output
         os << "  ";
         shortcut( os, f[2+j*20]);
         if (j==0)os << "   ";
         else os << "    ";
         os.setf(ios::uppercase);
            // Due to ancient use of FIC back
            // when IODC/IODE were AODC/AODE,
            // the IODC/IODE values are scaled by 2**10
            // in FIC and have to unscaled.
         iod = (short) (f[ndx[j]] / 2048);
         iod = iod;
         os << "0x";
         os.fill('0');
         if (j==0) os.width(3);           // IODC is longer
         else os.width(2);
         os << hex << iod << dec << "      ";
         os.fill(' ');
            // Word 4 (index 3) combines the "Alert" bit and the A-S flag
         itemp = (short) f[3+j*20];
         if (itemp & 0x0002) os << "1     ";   // "Alert" bit handling
         else os << "0     ";
         if (itemp & 0x0001) os << " on\n";      // A-S flag handling
         else os << "off\n";
      }
      os << "\n           CLOCK\n\n";
      os << "Bias T0:     ";
      os.setf(ios::scientific, ios::floatfield);
      os.setf(ios::right, ios::adjustfield);
      os.precision(8);
      os.width(16);
      os << f[15] << " sec\n";
      os << "Drift:       ";
      os.width(16);
      os << f[14] << " sec/sec\n";
      os << "Drift rate:  ";
      os.width(16);
      os << f[13] << " sec/(sec**2)\n";
      os << "Group delay: ";
      os.width(16);
      os << f[11] << " sec\n";
      os.setf(ios::fixed, ios::floatfield);
      os.precision(0);

      os << "\n           ORBIT PARAMETERS\n\n";
      os << "Semi-major axis:       ";
      os.setf(ios::scientific, ios::floatfield);
      os.setf(ios::right, ios::adjustfield);
      os.precision(8);
      os.width(16);
      os << f[32] << " m**.5\n";
      os << "Motion correction:     ";
      os.width(16);
      os << f[27] << " rad/sec\n";
      os << "Eccentricity:          ";
      os.width(16);
      os << f[30] << "\n";
      os << "Arg of perigee:        ";
      os.width(16);
      os << f[50] << " rad\n";
      os << "Mean anomaly at epoch: ";
      os.width(16);
      os << f[28] << " rad\n";
      os << "Right ascension:       ";
      os.width(16);
      os << f[46] << " rad    ";
      os.width(16);
      os << f[51] << " rad/sec\n";
      os << "Inclination:           ";
      os.width(16);
      os << f[48] << " rad    ";
      os.width(16);
      os << f[53] << " rad/sec\n";
      os.setf(ios::fixed, ios::floatfield);
      os.precision(0);

      os << "\n           HARMONIC CORRECTIONS\n\n";
      os << "Radial        Sine: ";
      os.setf(ios::scientific, ios::floatfield);
      os.setf(ios::right, ios::adjustfield);
      os.precision(8);
      os.width(16);
      os << f[26] << " m    Cosine: ";
      os.width(16);
      os << f[49] << " m\n";
      os << "Inclination   Sine: ";
      os.width(16);
      os << f[47] << " rad  Cosine: ";
      os.width(16);
      os << f[45] << " rad\n";
      os << "In-track      Sine: ";
      os.width(16);
      os << f[31] << " rad  Cosine: ";
      os.width(16);
      os << f[29] << " rad\n";
      os.setf(ios::fixed, ios::floatfield);
      os.precision(0);

      os << "\n           SV STATUS\n\n";
      os << "Health bits:   0x";
      os.fill('0');
      os.width(2);
      os << (short) f[8] << "      URA index: ";
      os.fill(' ');
      os.width(4);
      os << f[7] << "\n";
      os << "Code on L2:   ";
      switch ( (short) f[6] )
      {
         case 0:
            os << "reserved";
            break;

         case 1:
            os << " P only";
            break;

         case 2:
            os << " C/A only";
            break;

         case 3:
            os << " P & C/A";
            break;

         default:
            break;

      }
      os << "   L2 P Nav data:          ";
      if ( (short) f[10]!=0) os << "off";
      else os << "on";
      os << "\n";
   }

   void FICData::prettyDump109(ostream & os) const
   {
      os << "**************************************";
      os << "**************************************\n";
      os << "   Block Number : ";
      os.width(3);
      os << blockNum << "\n";
      os << "Broadcast Ephemeris as Transmitted\n";
      os << "\n";
      os << "PRN : ";
      os.width(2);
      os << i[1] << "\n";
      timeDisplay( os, "Transmit time:",
                   i[0], 
                   ((i[3] & 0x3FFFFFFFL) >> 13) * 6,
                   2 );
      os.setf(ios::uppercase);
      os << "\n";
      os << "Hexadecimal dump of words  1-10";
      short j;
      for (j=0;j<10;j++)
      {
         if (j==0 || j ==5 ) os << "\n";
         os << "    ";
         os.width(2);
         os << (j+1) << ":";
         os.width(8);
         os.fill('0');
         os << hex << i[j+2] << dec;
         os.fill(' ');
      }
      os << "\n\n";
      os << "Hexadecimal dump of words 11-20";
      for (j=10;j<20;j++)
      {
         if (j==10 || j ==15 ) os << "\n";
         os << "    ";
         os.width(2);
         os << (j+1) << ":";
         os.width(8);
         os.fill('0');
         os << hex << i[j+2] << dec;
         os.fill(' ');
      }
      os << "\n\n";
      os << "Hexadecimal dump of words 21-30";
      for (j=20;j<30;j++)
      {
         if (j==20 || j ==25 ) os << "\n";
         os << "    ";
         os.width(2);
         os << (j+1) << ":";
         os.width(8);
         os.fill('0');
         os << hex << i[j+2] << dec;
         os.fill(' ');
      }
      os << "\n\n";
   }

   void FICData::prettyDump62(ostream & os) const
   {
      short j,k;
      unsigned short btmp;
      unsigned long aword;
      short rotate;
      char  ochar;
      short almType;

      os << "**************************************";
      os << "**************************************\n";
      os << "   Block Number : ";
      os.width(3);
      os << blockNum << "\n";
      os << "Almanac(Engineering Units)\n";
      os << "\n";
      os << "SV ID : ";
      os.setf(ios::right, ios::adjustfield);
      os.width(2);
      os << i[3] << "\n";
      timeDisplay( os, "Transmit time", i[5], i[1], 1 );
      os << "Reference Wk  ";
      os.width(4);
      os << i[0] << "\n";
      almType = (short) f[6];
      if (almType>0&&almType<33)
      {
            // Output of SV almanac data
         timeDisplay( os, "Epoch time", (short) f[18], f[8], 0);
         os << "\n";
         os << "CLOCK\n";
         os.setf(ios::scientific, ios::floatfield);
         os.setf(ios::right, ios::adjustfield);
         os.precision(8);
         os << "   Bias:   ";
         os.width(16);
         os << f[16] << " sec\n";
         os << "   Drift:  ";
         os.width(16);
         os << f[17] << " sec/sec\n";

         os << "\nORBIT PARAMETERS\n";
         os << "   Semi-major axis:       ";
         os.width(16);
         os << f[12] << " m**.5\n";
         os << "   Eccentricity:          ";
         os.width(16);
         os << f[7] << "\n";
         os << "   Arg of perigee:        ";
         os.width(16);
         os << f[14] << " rad\n";
         os << "   Mean anamoly of epoch: ";
         os.width(16);
         os << f[15] << " rad\n";
         os << "   Right Ascension:       ";
         os.width(16);
         os << f[13] << " rad   ";
         os.width(16);
         os << f[10] << " rad/sec\n";
         os << "   Inclination:           ";
         os.width(16);
         os << f[9] << " rad (from 54 deg)\n";

         os.setf(ios::fixed, ios::floatfield);
         os.precision(0);
      }
      else if (almType>=57 && almType<=62 )
      {
            // Hexadecimal dump
         os << "\n";
         os << "Hexadecimal dump of non-parity bits of words 3-10\n";
//    os << "     **This feature under construction.**\n";
         os.setf(ios::uppercase);
         for (k=2;k<10;k++)
         {
            if (k==2 || k==6 ) os << "\n";
            os << "    ";
            os.width(2);
            os << (k+1) << ":";
            os.width(6);      // 'stead 6
            os.fill('0');
            aword = (unsigned long) f[k+5];
//       aword >>= 6;                     // The data had parity stripped
               // during subframe conversion.
            os << hex << aword << dec;
            os.fill(' ');
         }

      }
      else switch (almType)
      {
         case 51:       // Health 1
            os << "\n";
            os << "Page Type: SV Health Information\n";
            os << "\nAlmanac reference week (8 bit): ";
            os.precision(0);
            os << f[7] << "\n";
            os << "SV Health Information for PRN 1-24\n\n";
            os << "     --Health---        --Health---        --Health---        --Health---\n";
            os << "PRN  Hex  Binary   PRN  Hex  Binary   PRN  Hex  Binary   PRN  Hex  Binary\n";
            os.setf(ios::uppercase);
            for (k=1;k<=21;k+=4)
            {
               for (j=k;j<k+4;j++)
               {
                  if (j!=k) os << "    ";
                  else os << " ";
                  os.width(2);
                  os.fill('0');
                  os << j;
                  os.fill(' ');
                  os << "   ";
                  btmp = (unsigned short) f[j+7];
                  btmp &= 0x003F;
                  os.fill('0');
                  os.width(2);
                  os << hex << btmp << dec;
                  os.fill(' ');
                  os << "  ";
                  btmp = (unsigned short) f[j+7];
                  for (rotate=0;rotate<6;rotate++)
                  {
                     if (btmp & 0x20) os << "1";
                     else os << "0";
                     btmp <<= 1;
                  }
               }
               os << "\n";
            }
            break;

         case 52:
         case 53:
         case 54:
            os << "\n";
            if (almType==52) os << "Page Type: Subframe 4 Page 13, Reserved Bits\n";
            if (almType==53) os << "Page Type: Subframe 4 Page 14, Reserved Bits\n";
            if (almType==54) os << "Page Type: Subframe 4 Page 15, Reserved Bits\n";
               // Hexadecimal dump
            os << "Hexadecimal dump of non-parity bits of words 3-10\n";
            os << "     **This feature under construction.**\n";
            os << "     **This page is decoded incorrectly in the ";
            os << "subframe converter.**\n";
/*       os.setf(ios::uppercase);
         for (j=2;j<10;j++)
         {
                        if (j==2 || j==6 ) os << "\n";
                        os << "    ";
                        os.width(2);
                        os << (j+1) << ":";
                        os.width(6);
                        os.fill('0');
                        aword = i[j+2];
                        aword >>= 6;
                        os << hex << aword << dec;
                        os.fill(' ');
         }
*/
            break;

         case 55:
            os << "\n";
            os << "Page Type: Subframe 4 Page 17, Special Message\n";
               // Hexadecimal dump
               // Loop over words 3-10
            os << "\n Message : \"";
            for (k=1;k<23;k++)
            {
               aword = (unsigned long) f[k+6];
               ochar = (char) aword;
               os << ochar;
            }
            os << "\"\n";
            break;

         case 56:
            os << "\n";
            os << "Page Type: UTC and Ionospheric Information\n";
            os.setf(ios::scientific, ios::floatfield);
            os.setf(ios::right, ios::adjustfield);
            os.precision(8);
            os << "\n";
            os << "IONOSPHERIC PARAMETERS \n";
            os << "   Alpha 1 : ";
            os.width(16);
            os << f[7] << " sec          Beta 1 : ";
            os.width(16);
            os << f[11] << "sec\n";
            os << "   Alpha 2 : ";
            os.width(16);
            os << f[8] << " sec/rad      Beta 2 : ";
            os.width(16);
            os << f[12] << " sec/rad\n";
            os << "   Alpha 3 : ";
            os.width(16);
            os << f[9] << " sec/rad**2   Beta 3 : ";
            os.width(16);
            os << f[13] << " sec/rad**2\n";
            os << "   Alpha 4 : ";
            os.width(16);
            os << f[10] << " sec/rad**3   Beta 4 : ";
            os.width(16);
            os << f[14] << " sec/rad**3\n\n";

            os << "GPS-UTC CORRECTION PARAMETERS\n";
            os << "   Bias  (A0)                                ";
            os.width(16);
            os << f[15] << " sec\n";
            os << "   Drift (A1)                                ";
            os.width(16);
            os << f[16] << " sec/sec\n";
            os.setf(ios::fixed, ios::floatfield);
            os.precision(0);
            os << "   Reference time (tot)                      ";
            os.width(16);
            os << f[17] << " sec of week\n";
            os << "   Current leap second (dtls)                ";
            os.width(16);
            os << f[19] << " sec\n";
            os << "   Ref week of current leap second (WNt)     ";
            os.width(16);
            os << f[18] << " weeks\n";
            os << "   Week of next/last change (WNLSF)          ";
            os.width(16);
            os << f[20] << " weeks\n";
            os << "   Day number of future change (DN)          ";
            os.width(16);
            os << f[21] << " day of week\n";
            os << "   Scheduled future time increment due to\n";
            os << "    lead seconds (dtLSF)                     ";
            os.width(16);
            os << f[22] << " sec\n";
            break;

         case 63:       // Health 2
            os << "\n";
            os << "Page Type: SV Health and Configuration Information\n\n";
            os.precision(0);
            os << "SV Health Information for PRN 25-32\n";
            os << "-----Health-----   -----Health-----   -----Health-----   -----Health-----\n";
            os << "PRN  Hex  Binary   PRN  Hex  Binary   PRN  Hex  Binary   PRN  Hex  Binary\n";
            for (k=25;k<=29;k+=4)
            {
               for (j=k;j<k+4;j++)
               {
                  if (j!=k) os << "    ";
                  else os << " ";
                  os.width(2);
                  os.fill('0');
                  os << j;
                  os.fill(' ');
                  os << "   ";
                  btmp = (unsigned short) f[j+14];
                  btmp &= 0x003F;
                  os.fill('0');
                  os.width(2);
                  os << hex << btmp << dec;
                  os.fill(' ');
                  os << "  ";
                  btmp = (unsigned short) f[j+14];
                  for (rotate=0;rotate<6;rotate++)
                  {
                     if (btmp & 0x20) os << "1";
                     else os << "0";
                     btmp <<= 1;
                  }
               }
               os << "\n";
            }

            os << "\nSV Configuration for PRN 1-32 (bit coded)\n";
            os << "-----Config-----   -----Config-----   -----Config-----   -----Config-----\n";
            os << "PRN       Binary   PRN       Binary   PRN       Binary   PRN       Binary\n";
            os.fill('0');
            for (k=1;k<32;k+=4)
            {
               for (j=k;j<(k+4);j++)
               {
                  if (j==k) os << " ";
                  else os << "    ";
                  os.width(2);
                  os << j << "         ";
                  btmp = (unsigned short) f[j+6];
                  for (rotate=0;rotate<4;rotate++)
                  {
                     if (btmp & 0x08) os << "1";
                     else os << "0";
                     btmp <<= 1;
                  }
               }
               os << "\n";
            }
            os.fill(' ');
            break;

         default:
            break;
      }
      os << "\n\n";
   }

   void FICData::prettyDump162(ostream & os) const
   {
      os << "**************************************";
      os << "**************************************\n";
      os << "   Block Number : ";
      os.width(3);
      os << blockNum << "\n";
      os << "As Broadcast Almanac\n";
      os << "\n";
      os << "SV ID : ";
      os.width(2);
      os << i[0] << "\n";
      timeDisplay( os, "Transmit time", 
                   i[14], 
                   ((i[2] & 0x3FFFFFFFL) >> 13) * 6,
                   1 );
      
         // Added to display reference week in addition to transmit week.
      os << "Reference Wk  ";
      os.width(4);
      os << i[13];
      os << "\n";
      
      os.setf(ios::uppercase);
      os << "\n";
      os << "Hexadecimal dump of words in subframe";
      short j;
      for (j=0;j<10;j++)
      {
         if (j==0 || j ==5 ) os << "\n";
         os << "    ";
         os.width(2);
         os << (j+1) << ":";
         os.width(8);
         os.fill('0');
         os << hex << i[j+1] << dec;
         os.fill(' ');
      }
      os << "\n\n";
   }

      // shortcut is used in outBlock9 to convert the
      // HOW word time to D:H:M:S and print it out.
   void FICData::shortcut(ostream & os, const double HOW ) const
   {
      short DOW, hour, min, sec;
      long SOD, SOW;
      short SOH;

      SOW = (long) HOW;
      DOW = (short) (SOW / DayTime::SEC_DAY);
      SOD = SOW - DOW * long(DayTime::SEC_DAY);
      hour = (short) (SOD/3600);

      SOH = (short) (SOD - (hour*3600));
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
      os << ":";
      os.fill('0');
      os.width(2);
      os << hour << ":";
      os.width(2);
      os << min << ":";
      os.width(2);
      os << sec;
   }

      // timeDisplay method provides a standard means for displaying
      // the GPS time and d/m/y h:m:s.  The calling program provides
      // the opening text string for the line, the GPS week, the GPS SOW,
      // and a flag indicating if the header line is to be output.
      // (The latter is helpful when there are two or more times to be
      // output one above the other.)
   void FICData::timeDisplay( ostream & os, const char * legend,
                              const short week, const double SOW, 
                              const short headerFlag ) const
   {
      DayTime dt;
      short slen;
      short j;

      if (headerFlag)
         os << "              Week(10bt)     SOW     DOW   UTD     SOD   MM/DD/YYYY   HH:MM:SS\n";
      os << legend;
      slen = strlen(legend);
      for (j=1;j<(15-slen);j++) os << " ";

         // Convert to daytime struct from GPS wk,SOW to M/D/Y, H:M:S.
      dt.setGPSfullweek(week, SOW);

      os.width(4);
      os << dt.GPSfullweek() << "(";
      os.width(4);
      os << dt.GPS10bitweek() << ")  ";
      os.width(6);
      os << dt.GPSsecond() << "   ";

      switch (dt.GPSday())
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
      os << "   ";
      os.fill('0');
      os.width(3);
      os << dt.DOYday() << "   ";
      os.width(5);
      os << dt.DOYsecond() << "   ";
      os.width(2);
      os << dt.month() << "/";
      os.width(2);
      os << dt.day() << "/";
      os.width(4);
      os << dt.year() << "   ";
      os.width(2);
      os << dt.hour() << ":";
      os.width(2);
      os << dt.minute() << ":";
      os.width(2);
      os << dt.second() << "\n";
      os.fill(' ');
   }

   void FICData::reallyGetRecord(FFStream& ffs)
      throw(std::exception, gpstk::StringUtils::StringException, 
            gpstk::FFStreamError)
   {
         // is this a FIC or FICA stream?
      if (dynamic_cast<FICAStream*>(&ffs))
      {
         FICAStream& strm = dynamic_cast<FICAStream&>(ffs);

            // Make sure the header has been read.
         if (!strm.headerRead) 
            strm >> strm.header;
         
         f.clear();
         i.clear();
         c.clear();                  
         
         string line;
         strm.formattedGetLine(line, true);
         string blkHdr(line.substr(0,4));
         
         if ( (blkHdr.size() != FICBlkHdrSize) || 
              (blkHdr != blockString))
         {
            FFStreamError e("Bad block header, record=" + 
                            asString(strm.recordNumber) + 
                            " location=" + asString(strm.tellg()));
            e.addText(string("blkHdr=[")+string(blkHdr)+string("]"));
            GPSTK_THROW(e);
         }
            // get block number
         blockNum = asInt(line.substr(4, 6));
         
            // get num of f,i, and c to read then read them
         long numf = asInt(line.substr(10,5)), 
            numi = asInt(line.substr(15,5)),
            numc = asInt(line.substr(20,5));
         
         long n;
         
         for (n = 0; n < numf; n++)
         {
            if (n%4 == 0)
               strm.formattedGetLine(line);
            f.push_back(for2doub(line.substr((n%4)*20, 20), 20));
         }
         
         for (n = 0; n < numi; n++)
         {
            if (n%6 == 0)
               strm.formattedGetLine(line);
            i.push_back(asInt(line.substr((n%6)*12, 12)));
         }
         
         for (n = 0; n < numc; n++)
         {
            if (n%8 == 0)
               strm.formattedGetLine(line);
            c.push_back(line.substr((n%8)*8 + 7, 1)[0]);
         }
         
         if (!isValid())
         {
            FFStreamError e("Read an invalid FIC block");
            GPSTK_THROW(e);
         }
      }
      else
      {
         if (dynamic_cast<FICStream*>(&ffs))
         {
            FICStream& strm = dynamic_cast<FICStream&>(ffs);
            
               // Make sure the header has been read.
            if (!strm.headerRead) 
               strm >> strm.header;
            
            f.clear();
            i.clear();
            c.clear();
            
            char blkHdr[FICBlkHdrSize + 1];
            unsigned location = strm.tellg();

            strm.getData(blkHdr, FICBlkHdrSize);
            if ((string(blkHdr, FICBlkHdrSize) != blockString))
            {
               FFStreamError e("Bad block header, record="
                               + asString(strm.recordNumber)
                               + " location=" + asString(location));
               e.addText(string("blkHdr=[")+string(blkHdr)+string("]"));
               GPSTK_THROW(e);
            }
               // get block number
            blockNum = intelToHost(strm.getData<int32_t>());
            
               // get num of f,i, and c to read then read them
            int32_t numf = intelToHost(strm.getData<int32_t>());
            int32_t numi = intelToHost(strm.getData<int32_t>());
            int32_t numc = intelToHost(strm.getData<int32_t>());
            
            long n;
            
            for (n = 0; n < numf; n++)
            {
               double fl = intelToHost(strm.getData<double>());
               f.push_back(fl);
            }
            
            for (n = 0; n < numi; n++)
            {
               long lg = intelToHost(strm.getData<int32_t>());
               i.push_back(lg);
            }
            
            for (n = 0; n < numc; n++)
            {
               char ch = intelToHost(strm.getData<char>());
               c.push_back(ch);
            }
         }
            // bad cast!!!
         else
         {
            FFStreamError e("FICData tried to read from a non-FIC file");
            GPSTK_THROW(e);
         }         
      }
      if (!isValid())
      {
         FFStreamError e("Read an invalid FIC block");
         GPSTK_THROW(e);
      }
   }

   FICData::operator EngEphemeris() const
      throw(WrongBlockNumber)
   {
      if(blockNum==9)
      {
         gpstk::EngEphemeris eph;
         unsigned tlm = (unsigned)f[0];
         eph.setSF1( ((tlm >> 8) & 0x3fff), (long)f[2], (short)f[3], (short)f[5],
                     (short)f[6], (short)f[7], (short)f[8], (short)ldexp(f[9],-11),
                     (short)f[10], f[11], f[12], f[13], f[14], f[15], (short)f[18],
                     (short)f[19] );
         tlm = (unsigned)f[20];
         eph.setSF2( ((tlm >> 8) & 0x3fff),(long)f[22], (short)f[23],
                     (short)ldexp(f[25],-11), f[26], f[27], f[28], f[29], f[30],
                     f[31], f[32], f[33], (short)f[34] );
         tlm = (unsigned)f[40];
         eph.setSF3( ((tlm >> 8) & 0x3fff), (long)f[42], (short)f[43], f[45],
                     f[46], f[47], f[48], f[49], f[50], f[51], f[53] );
         return eph;
      }
      else if(blockNum == 109)
      {
         gpstk::EngEphemeris eph;
            // gotta transfer data in vector<long> to long[].
         long foo[30];
         int count = 0;
         for(int count = 2; count < i.size(); count++)
         {
            foo[count-2] = i[count];            
         }
         eph.addSubframe(foo,    i[0], i[1], 0);
         eph.addSubframe(foo+10, i[0], i[1], 0);
         eph.addSubframe(foo+20, i[0], i[1], 0);
         return eph;
      }
      else
      {
         WrongBlockNumber wbn( "Block number should be 9, was " + 
                               asString(blockNum) );
         GPSTK_THROW(wbn);
      }
   }


   FICData::operator AlmOrbit() const
      throw(WrongBlockNumber)
   {
      if(blockNum == 62)
      {
         AlmOrbit ao(i[3], f[7], f[9], f[10],
                     f[12], f[13], f[14], f[15],
                     f[16], f[17], long(f[8]), i[1],
                     i[0], short(f[11]));
         return ao;
      }
      else
      {
         WrongBlockNumber wbn( "Block number should be 62, was " + 
                               asString(blockNum) );
         GPSTK_THROW(wbn);
      }
   }

   std::string FICData::generateUniqueKey() const
      throw(WrongBlockNumber, WrongBlockFormat)
   {
      std::ostringstream out;
      DayTime transmitTime(0.L), howTime(0.L);
      WrongBlockNumber wbn( "Block number should be 9, 109, or 62, was "+
                            asString(blockNum) );

      switch (blockNum)
      {
         case 9:
               // the keys for EphData are PRN, week, IODC, AS and alert bits
            out << "EE" << ' '
                << f[19] << ' '
                << f[5] << ' '
                  // all bits are as counted from the MSB
                  // AODC is recorded, not IODC so divide by 2048
                << ldexp(f[9], -11) << ' '
                  // the AS and alert bits
                << f[3] << ' '
                << f[23] << ' '
                << f[43];
            break;

         case 109:
            out << "E" << ' '
                << i[1] << ' ' // prn
                << i[0] << ' ' // transmit week
                <<  (((i[4] & 0xC0)<<2) + ((i[9] & 0x3FC00000)>>22)) << ' '
                  // the AS and alert bits are in the HOW (word 2), bits 18 and 19
                << ((i[3] & 0x1800)>>11) << ' '
                << ((i[13] & 0x1800)>>11) << ' '
                << ((i[23] & 0x1800)>>11);
            break;

         case 62:
            transmitTime.setGPSfullweek(i[5], (double)i[1]);
            howTime.setGPSfullweek(i[5], f[2]);

               // we only have toa in this format message.. yay.
               // FIX magic number.. I'm a bit apprehensive about
               // using MAX_PRN here because I don't know how things
               // may change.  erf.
            if ((i[3] >= 1) && (i[3] <= 32))
            {
               out << "A" << ' '
                   << ((short)f[6]) << ' '               // page id
                   << transmitTime.GPSday() << ' '
                   << transmitTime.GPSfullweek() << ' '
                   << ((long)f[8]) << ' '                // toa
                   << howTime.DOYday() << ' '
                   << howTime.DOYyear();
            }
            else
            {
               WrongBlockFormat wbf("Format " + asString(i[4]) +
                                    " (page ID " + asString(i[3]) +
                                    ") cannot be used to generate a key.");
               GPSTK_THROW(wbf);
            }
            break;
         default:
            GPSTK_THROW(wbn);
      } // switch (blockNum)

      return out.str();

   }

} // namespace gpstk

