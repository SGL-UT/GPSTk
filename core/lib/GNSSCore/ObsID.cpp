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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/// @file ObsID.cpp
/// gpstk::ObsID - Identifies types of observations

#include <math.h>
#include "ObsID.hpp"

namespace gpstk
{
   // descriptions (strings) of each code, carrier and type
   std::map< TrackingCode,    std::string > ObsID::tcDesc;
   std::map< CarrierBand,     std::string > ObsID::cbDesc;
   std::map< gpstk::ObservationType, std::string > ObsID::otDesc;
   // mappings between code, carrier, type and characters
   std::map< char, gpstk::ObservationType> ObsID::char2ot;
   std::map< char, CarrierBand> ObsID::char2cb;
   std::map< char, TrackingCode> ObsID::char2tc;
   std::map< gpstk::ObservationType, char > ObsID::ot2char;
   std::map< CarrierBand, char > ObsID::cb2char;
   std::map< TrackingCode, char> ObsID::tc2char;

   // map of valid RINEX tracking codes, systems and frequency
   std::map<char, std::map<char, std::string> > ObsID::validRinexTrackingCodes;

   // string containing the system characters for all valid RINEX systems.
   std::string ObsID::validRinexSystems;

   // maps between 1-char and 3-char system id
   std::map<std::string, std::string> ObsID::map1to3sys;
   std::map<std::string, std::string> ObsID::map3to1sys;

   // string containing the frequency digits for all valid RINEX systems.
   std::string ObsID::validRinexFrequencies;

   // object that forces initialization of the maps
   ObsIDInitializer singleton;

   // Construct this object from the string specifier
   ObsID::ObsID(const std::string& strID, double version)
         : rinexVersion(version)
   {
      int i = strID.length() - 3;
      if ( i < 0 || i > 1)
      {
         InvalidParameter e("identifier must be 3 or 4 characters long");
         GPSTK_THROW(e);
      }

      char sys = i ? strID[0] : 'G';
      char ot = strID[i];
      char cb = strID[i+1];
      char tc = strID[i+2];
 
      if (!char2ot.count(ot) || !char2cb.count(cb) || !char2tc.count(tc))
         idCreator(strID.substr(i,3));

      type = char2ot[ ot ];
      band = char2cb[ cb ];
      code = char2tc[ tc ];

         // special handling for iono delay and channel num pseudo-observables.
      if (ot == 'X')
      {
         if ((cb != '1') || (tc != ' '))
         {
            InvalidParameter e("Invalid channel number pseudo-obs ID " + strID);
            GPSTK_THROW(e);
         }
         code = TrackingCode::Undefined;
      }
      if (ot == 'I')
      {
         if ((cb < '1') || (cb > '9') || (tc != ' '))
         {
            InvalidParameter e("Invalid ionospheric delay pseudo-obs ID " +
                               strID);
            GPSTK_THROW(e);
         }
         code = TrackingCode::Undefined;
      }

      /// This next block takes care of fixing up the codes that are reused
      /// between the various signals
      switch (sys)
      {
         case 'G':    // GPS
      {
         if (tc=='X' && band==CarrierBand::L5)
            code = TrackingCode::IQ5;
         if (band==CarrierBand::L1)
         {
            if (tc=='X') code = TrackingCode::G1X;
            if (tc=='S') code = TrackingCode::G1D;
            if (tc=='L') code = TrackingCode::G1P;
         }
         break;
      }
//
// Explanation of Galileo cases.
//
// Left three columns are from RINEX 3.04, Table 6.
// Next two columns show the desired ObsID code/band.
// Last three columns show the simple char2cb[ ] and char2tc[ ] conversions.
// The band conversions appear to be correct in all cases.   However, the
// code converstion marked "NO" are incorrect.  This is due to the overloading
// of several characters by the RINEX standard.  These need to be "fixed up"
// following the simple conversion.
//
//                     RINEX   Desired ObsID       Simple char2tc[ ] conversion
// Band  Channel/Code   code    band     code      band   code     code correct?
//   E1  A PRS           x1A    cbL1     tcA       cbL1   tcA
//       B I/NAV OS      x1B             tcB              tcB
//       C               x1C             tcC              tcCA      NO
//       B+C             x1X             tcBC             tcC2LM    NO
//       A+B+C           x1Z             tcABC            tcABC
//  E5a  I F/NAV OS      x5I    cbL5     tcIE5a    cbL5   tcI5      NO
//       Q               x5Q             tcQE5a           tcQ5      NO
//       I+Q             x5X             tcIQE5a          tcC2LM    NO
//  E5b  I I/NAV OS      x7I    cbE5b    tcIE5b    cbE5b  tcI5      NO
//       Q               x7Q             tcQE5b           tcQ5      NO
//       I+Q             x7X             tcIQE5b          tcC2LM    NO
//   E5  I               x8I    cbE5ab   tcIE5     cbE5ab tcI5      NO
//       Q               x8Q             tcQE5            tcQ5      NO
//       I+Q             x8X             tcIQE5           txC2LM    NO
//   E6  A PRS           x6A    cbE6     tcA       cbE6   tcA
//       B C/NAV CS      x6B             tcB              tcB
//       C               x6C             tcC              tcCA      NO
//       B+C             x6X             tcBC             tcC2LM    NO
//       A+B+C           x6Z             tcABC            tcABD
//
         case 'E':   // Galileo
      {
         if (band==CarrierBand::L1)
         {
            switch (code)
            {
               case TrackingCode::CA:   code = TrackingCode::C;  break;
               case TrackingCode::C2LM: code = TrackingCode::BC; break;
               default: break;
            }
         }
         if (band==CarrierBand::E6)
         {
            switch (code)
            {
               case TrackingCode::CA:
                  code = TrackingCode::C6;
                  break;
               case TrackingCode::C2LM:
                  code = TrackingCode::BC6;
                  break;
               case TrackingCode::A:
                  code = TrackingCode::A6;
                  break;
               case TrackingCode::B:
                  code = TrackingCode::B6;
                  break;
               case TrackingCode::C:
                  code = TrackingCode::C6;
                  break;
               case TrackingCode::BC:
                  code = TrackingCode::BC6;
                  break;
               case TrackingCode::ABC:
                  code = TrackingCode::ABC6;
                  break;
               default:
                  break;
            }
         }
         if (band==CarrierBand::L5)
         {
            switch (code)
            {
               case TrackingCode::I5:   code = TrackingCode::IE5a;  break;
               case TrackingCode::Q5:   code = TrackingCode::QE5a;  break;
               case TrackingCode::C2LM: code = TrackingCode::IQE5a; break;
               default: break;
            }
         }
         if (band==CarrierBand::E5b)
         {
            switch (code)
            {
               case TrackingCode::I5:   code = TrackingCode::IE5b;  break;
               case TrackingCode::Q5:   code = TrackingCode::QE5b;  break;
               case TrackingCode::C2LM: code = TrackingCode::IQE5b; break;
               default: break;
            }
         }
         if (band==CarrierBand::E5ab)
         {
            switch (code)
            {
               case TrackingCode::I5:   code = TrackingCode::IE5;  break;
               case TrackingCode::Q5:   code = TrackingCode::QE5;  break;
               case TrackingCode::C2LM: code = TrackingCode::IQE5; break;
               default: break;
            }
         }
         break;
      }
         case 'R': // Glonass
      {
         switch (band)
         {
            case CarrierBand::L1: band = CarrierBand::G1; break;
            case CarrierBand::L2: band = CarrierBand::G2; break;
            case CarrierBand::E6: band = CarrierBand::G2a; break;
            default: break;
         }
         switch (band)
         {
            case CarrierBand::G1:
            case CarrierBand::G2:
               switch (code)
               {
                  case TrackingCode::CA: code = TrackingCode::GCA; break;
                  case TrackingCode::P: code = TrackingCode::GP; break;
               }
               break;
            case CarrierBand::G1a:
               switch (code)
               {
                  case TrackingCode::A: code = TrackingCode::L1OCD; break;
                  case TrackingCode::B: code = TrackingCode::L1OCP; break;
                  case TrackingCode::IQR3: case TrackingCode::C2LM: code = TrackingCode::L1OC; break;
               }
               break;
            case CarrierBand::G2a:
               switch (code)
               {
                  case TrackingCode::A: code = TrackingCode::L2CSI; break;
                  case TrackingCode::B: code = TrackingCode::L2OCP; break;
                  case TrackingCode::IQR3: case TrackingCode::C2LM: code = TrackingCode::L2CSIOCp; break;
               }
               break;
            case CarrierBand::G3:
               switch (code)
               {
                  case TrackingCode::I5: code = TrackingCode::IR3; break;
                  case TrackingCode::Q5: code = TrackingCode::QR3; break;
                  case TrackingCode::C2LM: case TrackingCode::G1X: code = TrackingCode::IQR3; break;
               }
               break;
         }
         break;
      }
         case 'S':    // SBAS or Geosync
      {
         switch (code)
         {
            case TrackingCode::CA: code = TrackingCode::SCA; break;     // 'C'
            case TrackingCode::I5: code = TrackingCode::SI5; break;     // 'I'
            case TrackingCode::Q5: code = TrackingCode::SQ5; break;     // 'Q'
            case TrackingCode::C2LM: case TrackingCode::G1X: code = TrackingCode::SIQ5; break;  // 'X'
            default: break;
         }
         break;
      }
         case 'J':   // QZSS
      {
         if(band == CarrierBand::L1) switch (code)
         {
            case TrackingCode::CA: code = TrackingCode::JCA; break;     // 'C'
            case TrackingCode::C2M: case TrackingCode::G1D: code = TrackingCode::JD1; break;    // 'S'
            case TrackingCode::C2L: case TrackingCode::G1P: code = TrackingCode::JP1; break;    // 'L'
            case TrackingCode::C2LM: case TrackingCode::G1X: code = TrackingCode::JX1; break;   // 'X'
            case TrackingCode::ABC: code = TrackingCode::JZ1; break;    // 'Z'
            default: break;
         }
         if(band == CarrierBand::L2) switch (code)
         {
            case TrackingCode::C2M: case TrackingCode::G1D: code = TrackingCode::JM2; break;    // 'S'
            case TrackingCode::C2L: case TrackingCode::G1P: code = TrackingCode::JL2; break;    // 'L'
            case TrackingCode::C2LM: case TrackingCode::G1X: code = TrackingCode::JX2; break;   // 'X'
            default: break;
         }
         if(band == CarrierBand::L5) switch (code)
         {
            case TrackingCode::I5: code = TrackingCode::JI5; break;     // 'I'
            case TrackingCode::Q5: code = TrackingCode::JQ5; break;     // 'Q'
            case TrackingCode::C2LM: code = TrackingCode::JIQ5; break;  // 'X'
            case TrackingCode::D: code = TrackingCode::JI5S; break;
            case TrackingCode::P: code = TrackingCode::JQ5S; break;
            case TrackingCode::ABC: code = TrackingCode::JIQ5S; break;
            default: break;
         }
         if(band == CarrierBand::E6) switch (code)
         {
            case TrackingCode::C2M: case TrackingCode::G1D: code = TrackingCode::JI6; break;    // 'S'
            case TrackingCode::C2L: case TrackingCode::G1P: code = TrackingCode::JQ6; break;    // 'L'
            case TrackingCode::C2LM: case TrackingCode::G1X: code = TrackingCode::JIQ6; break;  // 'X'
            case TrackingCode::ABC: code = TrackingCode::JDE6; break;
            default: break;
         }
         break;
      }
         case 'C':   // BeiDou
      {
         if (fabs(rinexVersion - 3.02) < 0.005)
         {
               // RINEX 3.02
            if(band == CarrierBand::L1)
               band = CarrierBand::B1;
         }
         else
         {
               // RINEX 3.0[013]
            if (band == CarrierBand::L2)
               band = CarrierBand::B1;
         }
         if (band == CarrierBand::E6)
            band = CarrierBand::B3;
         else if (band == CarrierBand::E5b)
            band = CarrierBand::B2;

         switch (band)
         {
            case CarrierBand::B1: // B1-2
               switch (code)
               {
                  case TrackingCode::I5:     // 'I'
                     code = TrackingCode::CI1;
                     break;
                  case TrackingCode::Q5:     // 'Q'
                     code = TrackingCode::CQ1;
                     break;
                  case TrackingCode::C2LM:   // 'X'
                  case TrackingCode::G1X:
                     code = TrackingCode::CIQ1;
                     break;
                  default:
                     break;
               }
               break;
            case CarrierBand::L1: // B1
               switch (code)
               {
                  case TrackingCode::D:
                     code = TrackingCode::CCD1;
                     break;
                  case TrackingCode::P:
                     code = TrackingCode::CCP1;
                     break;
                  case TrackingCode::C2LM:
                     code = TrackingCode::CCDP1;
                     break;
                  case TrackingCode::A:
                     code = TrackingCode::CA1;
                     break;
                  case TrackingCode::N:
                     code = TrackingCode::CodelessC;
                     break;
               }
               break;
            case CarrierBand::L5: // B2a
               switch (code)
               {
                  case TrackingCode::D:
                     code = TrackingCode::CI2a;
                     break;
                  case TrackingCode::P:
                     code = TrackingCode::CQ2a;
                     break;
                  case TrackingCode::C2LM:
                     code = TrackingCode::CIQ2a;
                     break;
               }
               break;
            case CarrierBand::B2: // B2b
               switch (code)
               {
                  case TrackingCode::I5:
                     code = TrackingCode::CI7;
                     break;
                  case TrackingCode::Q5:
                     code = TrackingCode::CQ7;
                     break;
                  case TrackingCode::C2LM:
                     code = TrackingCode::CIQ7;
                     break;
                  case TrackingCode::D:
                     code = TrackingCode::CI2b;
                     break;
                  case TrackingCode::P:
                     code = TrackingCode::CQ2b;
                     break;
                  case TrackingCode::ABC:
                     code = TrackingCode::CIQ2b;
                     break;
               }
               break;
            case CarrierBand::E5ab:
               switch (code)
               {
                  case TrackingCode::D:
                     code = TrackingCode::CI2ab;
                     break;
                  case TrackingCode::P:
                     code = TrackingCode::CQ2ab;
                     break;
                  case TrackingCode::C2LM:
                     code = TrackingCode::CIQ2ab;
                     break;
               }
               break;
            case CarrierBand::B3: // B3
               switch (code)
               {
                  case TrackingCode::I5:     // 'I'
                     code = TrackingCode::CI6;
                     break;
                  case TrackingCode::Q5:     // 'Q'
                     code = TrackingCode::CQ6;
                     break;
                  case TrackingCode::C2LM:   // 'X'
                  case TrackingCode::G1X:
                     code = TrackingCode::CIQ6;
                     break;
                  case TrackingCode::A:
                     code = TrackingCode::CIQ3A;
                     break;
                  default:
                     break;
               }
               break;
            case CarrierBand::E5b: 
               switch (code)
               {
                  case TrackingCode::I5:     // 'I'
                     code = TrackingCode::CI7;
                     break;
                  case TrackingCode::Q5:     // 'Q'
                     code = TrackingCode::CQ7;
                     break;
                  case TrackingCode::C2LM:   // 'X'
                  case TrackingCode::G1X:
                     code = TrackingCode::CIQ7;
                     break;
                  default:
                     break;
               }
               break;
         } // switch (band)
         break;
      }
         case 'I':  // IRNSS
      {
         if(band == CarrierBand::L5)
         {
            switch (code)
            {
               case TrackingCode::CA:   code = TrackingCode::IC5; break;   // 'C'
               case TrackingCode::A:    code = TrackingCode::IA5; break;   // 'A'
               case TrackingCode::B:    code = TrackingCode::IB5; break;   // 'B'
               case TrackingCode::C2LM: case TrackingCode::G1X: code = TrackingCode::IX5; break;   // 'X'
            default: break;
            }
         }
         if(band == CarrierBand::I9)
         {
            switch (code)
            {
               case TrackingCode::CA:   code = TrackingCode::IC9; break;   // 'C'
               case TrackingCode::A:    code = TrackingCode::IA9; break;   // 'A'
               case TrackingCode::B:    code = TrackingCode::IB9; break;   // 'B'
               case TrackingCode::C2LM: case TrackingCode::G1X: code = TrackingCode::IX9; break;   // 'X'
            default: break;
            }
         }
         break;
      }
         default:
            break;
      } // end of checking which GNSS system this obs is for
   }


   // Convenience output method
   std::ostream& ObsID::dump(std::ostream& s) const
   {
      s << ObsID::cbDesc[band] << " "
        << ObsID::tcDesc[code] << " "
        << ObsID::otDesc[type];
      return s;
   } // ObsID::dump()

   // This is used to register a new ObsID & Rinex 3 identifier.  The syntax for the
   // Rinex 3 identifier is the same as for the ObsID constructor. 
   // If there are spaces in the provided identifier, they are ignored
   ObsID ObsID::newID(const std::string& strID, const std::string& desc)
   {
      if (char2ot.count(strID[0]) && 
          char2cb.count(strID[1]) && 
          char2tc.count(strID[2]))
          GPSTK_THROW(InvalidParameter("Identifier " + strID + " already defined."));

      return idCreator(strID, desc);
   }


   ObsID ObsID::idCreator(const std::string& strID, const std::string& desc)
   {
      char ot = strID[0];
      ObservationType type;
      if (!char2ot.count(ot))
      {
         type = (ObservationType)otDesc.size();
         otDesc[type] = desc;
         char2ot[ot] = type;
         ot2char[type] = ot;
      }
      else
         type = char2ot[ot];

      char cb = strID[1];
      CarrierBand band;
      if (!char2cb.count(cb))
      {
         band = (CarrierBand)cbDesc.size();
         cbDesc[band] = desc;
         char2cb[cb] = band;
         cb2char[band] = cb;
      }
      else
         band = char2cb[cb];

      char tc = strID[2];
      TrackingCode code;
      if (!char2tc.count(tc))
      {
         code = (TrackingCode) tcDesc.size();
         tcDesc[code] = desc;
         char2tc[tc] = code;
         tc2char[code] = tc;
      }
      else
         code = char2tc[tc];
 
      return ObsID(type, band, code);
   }


   // Equality requires all fields to be the same unless the field is unknown
   bool ObsID::operator==(const ObsID& right) const
   {
         // Version comparison is intentionally left out.
      bool ot = type == ObservationType::Any ||
         right.type == ObservationType::Any || type == right.type;
      bool cb = band == CarrierBand::Any || right.band == CarrierBand::Any ||
         band == right.band;
      bool tc = code == TrackingCode::Any || right.code == TrackingCode::Any ||
         code == right.code;
      return ot && cb && tc;
   }


   // This ordering is somewhat arbitrary but is required to be able
   // to use an ObsID as an index to a std::map. If an application needs
   // some other ordering, inherit and override this function.
   bool ObsID::operator<(const ObsID& right) const
   {
      if (band == right.band)
         if (code == right.code)
            return type < right.type;
         else
            return code < right.code;
      else
         return band < right.band;

      // This should never be reached...
      return false;
   }


   namespace StringUtils
   {
      // convert this object to a string representation
      std::string asString(const ObsID& p)
      {
         std::ostringstream oss;
         p.dump(oss);
         return oss.str();
      }
   }


   // stream output for ObsID
   std::ostream& operator<<(std::ostream& s, const ObsID& p)
   {
      p.dump(s);
      return s;
   }
}
