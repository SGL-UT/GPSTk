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

#include "ObsID.hpp"

namespace gpstk
{
   // descriptions (strings) of each code, carrier and type
   std::map< ObsID::TrackingCode,    std::string > ObsID::tcDesc;
   std::map< ObsID::CarrierBand,     std::string > ObsID::cbDesc;
   std::map< ObsID::ObservationType, std::string > ObsID::otDesc;
   // mappings between code, carrier, type and characters
   std::map< char, ObsID::ObservationType> ObsID::char2ot;
   std::map< char, ObsID::CarrierBand> ObsID::char2cb;
   std::map< char, ObsID::TrackingCode> ObsID::char2tc;
   std::map< ObsID::ObservationType, char > ObsID::ot2char;
   std::map< ObsID::CarrierBand, char > ObsID::cb2char;
   std::map< ObsID::TrackingCode, char> ObsID::tc2char;

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
         code = tcUndefined;
      }
      if (ot == 'I')
      {
         if ((cb < '1') || (cb > '9') || (tc != ' '))
         {
            InvalidParameter e("Invalid ionospheric delay pseudo-obs ID " +
                               strID);
            GPSTK_THROW(e);
         }
         code = tcUndefined;
      }

      /// This next block takes care of fixing up the codes that are reused
      /// between the various signals
      switch (sys)
      {
         case 'G':    // GPS
      {
         if (tc=='X' && band==cbL5)
            code = tcIQ5;
         if (band==cbL1)
         {
            if (tc=='X') code = tcG1X;
            if (tc=='S') code = tcG1D;
            if (tc=='L') code = tcG1P;
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
         if (band==cbL1 || band==cbE6)
         {
            switch (code)
            {
               case tcCA:   code = tcC;  break;
               case tcC2LM: code = tcBC; break;
               default: break;
            }
         }
         if (band==cbL5)
         {
            switch (code)
            {
               case tcI5:   code = tcIE5a;  break;
               case tcQ5:   code = tcQE5a;  break;
               case tcC2LM: code = tcIQE5a; break;
               default: break;
            }
         }
         if (band==cbE5b)
         {
            switch (code)
            {
               case tcI5:   code = tcIE5b;  break;
               case tcQ5:   code = tcQE5b;  break;
               case tcC2LM: code = tcIQE5b; break;
               default: break;
            }
         }
         if (band==cbE5ab)
         {
            switch (code)
            {
               case tcI5:   code = tcIE5;  break;
               case tcQ5:   code = tcQE5;  break;
               case tcC2LM: code = tcIQE5; break;
               default: break;
            }
         }
         break;
      }
         case 'R': // Glonass
      {
         switch (code)
         {
            case tcCA: code = tcGCA; break;
            case tcP: code = tcGP; break;
            case tcI5: code = tcIR3; break;
            case tcQ5: code = tcQR3; break;
            case tcC2LM: case tcG1X: code = tcIQR3; break;
            default: break;
         }
         switch (band)
         {
            case cbL1: band = cbG1; break;
            case cbL2: band = cbG2; break;
            default: break;
         }
         break;
      }
         case 'S':    // SBAS or Geosync
      {
         switch (code)
         {
            case tcCA: code = tcSCA; break;     // 'C'
            case tcI5: code = tcSI5; break;     // 'I'
            case tcQ5: code = tcSQ5; break;     // 'Q'
            case tcC2LM: case tcG1X: code = tcSIQ5; break;  // 'X'
            default: break;
         }
         break;
      }
         case 'J':   // QZSS
      {
         if(band == cbL1) switch (code)
         {
            case tcCA: code = tcJCA; break;     // 'C'
            case tcC2M: case tcG1D: code = tcJD1; break;    // 'S'
            case tcC2L: case tcG1P: code = tcJP1; break;    // 'L'
            case tcC2LM: case tcG1X: code = tcJX1; break;   // 'X'
            case tcABC: code = tcJZ1; break;    // 'Z'
            default: break;
         }
         if(band == cbL2) switch (code)
         {
            case tcC2M: case tcG1D: code = tcJM2; break;    // 'S'
            case tcC2L: case tcG1P: code = tcJL2; break;    // 'L'
            case tcC2LM: case tcG1X: code = tcJX2; break;   // 'X'
            default: break;
         }
         if(band == cbL5) switch (code)
         {
            case tcI5: code = tcJI5; break;     // 'I'
            case tcQ5: code = tcJQ5; break;     // 'Q'
            case tcC2LM: code = tcJIQ5; break;  // 'X'
            default: break;
         }
         if(band == cbE6) switch (code)
         {
            case tcC2M: case tcG1D: code = tcJI6; break;    // 'S'
            case tcC2L: case tcG1P: code = tcJQ6; break;    // 'L'
            case tcC2LM: case tcG1X: code = tcJIQ6; break;  // 'X'
            default: break;
         }
         break;
      }
         case 'C':   // BeiDou
      {
         if(band == cbL1) band = cbB1;          // RINEX 3.02
         if(band == cbL2) band = cbB1;          // RINEX 3.0[013]
         if(band == cbE6) band = cbB3;

         if(band == cbB1) {
            switch (code)
            {
               case tcI5: code = tcCI1; break;     // 'I'
               case tcQ5: code = tcCQ1; break;     // 'Q'
               case tcC2LM: case tcG1X: code = tcCIQ1; break;  // 'X'
               default: break;
            }
         }
         if(band == cbB3) switch (code)
         {
            case tcI5: code = tcCI7; break;     // 'I'
            case tcQ5: code = tcCQ7; break;     // 'Q'
            case tcC2LM: case tcG1X: code = tcCIQ7; break;  // 'X'
            default: break;
         }
         if(band == cbE5b) {
            switch (code)
            {
               case tcI5: code = tcCI6; break;     // 'I'
               case tcQ5: code = tcCQ6; break;     // 'Q'
               case tcC2LM: case tcG1X: code = tcCIQ6; break;  // 'X'
            default: break;
            }
         }
         break;
      }
         case 'I':  // IRNSS
      {
         if(band == cbL5)
         {
            switch (code)
            {
               case tcCA:   code = tcIA5; break;   // 'A'
               case tcA:    code = tcIB5; break;   // 'B'
               case tcB:    code = tcIC5; break;   // 'B'
               case tcC2LM: case tcG1X: code = tcIX5; break;   // 'X'
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
      bool ot = type == otAny || right.type == otAny || type == right.type;
      bool cb = band == cbAny || right.band == cbAny || band == right.band;
      bool tc = code == tcAny || right.code == tcAny || code == right.code;
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


   std::string ObsID ::
   asString(ObservationType e) throw()
   {
      switch (e)
      {
         case otUnknown:   return "Unknown";
         case otAny:       return "Any";
         case otRange:     return "Range";
         case otPhase:     return "Phase";
         case otDoppler:   return "Doppler";
         case otSNR:       return "SNR";
         case otChannel:   return "Channel";
         case otDemodStat: return "DemodStat";
         case otIono:      return "Iono";
         case otSSI:       return "SSI";
         case otLLI:       return "LLI";
         case otTrackLen:  return "TrackLen";
         case otNavMsg:    return "NavMsg";
         case otRngStdDev: return "RngStdDev";
         case otPhsStdDev: return "PhsStdDev";
         case otFreqIndx:  return "FreqIndx";
         case otUndefined: return "Undefined";
         default:          return "???";
      }
   }


   ObsID::ObservationType ObsID ::
   asObservationType(const std::string& s) throw()
   {
      if (s == "Unknown")
         return otUnknown;
      if (s == "Any")
         return otAny;
      if (s == "Range")
         return otRange;
      if (s == "Phase")
         return otPhase;
      if (s == "Doppler")
         return otDoppler;
      if (s == "SNR")
         return otSNR;
      if (s == "Channel")
         return otChannel;
      if (s == "DemodStat")
         return otDemodStat;
      if (s == "Iono")
         return otIono;
      if (s == "SSI")
         return otSSI;
      if (s == "LLI")
         return otLLI;
      if (s == "TrackLen")
         return otTrackLen;
      if (s == "NavMsg")
         return otNavMsg;
      if (s == "RngStdDev")
         return otRngStdDev;
      if (s == "PhsStdDev")
         return otPhsStdDev;
      if (s == "FreqIndx")
         return otFreqIndx;
      if (s == "Undefined")
         return otUndefined;
      return otUnknown;
   }


   std::string ObsID ::
   asString(CarrierBand e) throw()
   {
      switch (e)
      {
         case cbZero:      return "Zero";
         case cbL1L2:      return "L1L2";
         case cbL1:        return "L1";
         case cbL2:        return "L2";
         case cbL5:        return "L5";
         case cbG1:        return "G1";
         case cbG1a:       return "G1a";
         case cbG2a:       return "G2a";
         case cbG2:        return "G2";
         case cbG3:        return "G3";
         case cbE6:        return "E6";
         case cbE5b:       return "E5b";
         case cbE5ab:      return "E5ab";
         case cbB1:        return "B1";
         case cbB3:        return "B3";
         case cbB2:        return "B2";
         case cbI9:        return "I9";
         case cbUnknown:   return "Unknown";
         case cbAny:       return "Any";
         case cbUndefined: return "Undefined";
         case cbLast:      return "Last";
         default:          return "???";
      }
   }


   ObsID::CarrierBand ObsID ::
   asCarrierBand(const std::string& s) throw()
   {
      if (s == "Zero")
         return cbZero;
      if (s == "L1L2")
         return cbL1L2;
      if (s == "L1")
         return cbL1;
      if (s == "L2")
         return cbL2;
      if (s == "L5")
         return cbL5;
      if (s == "G1")
         return cbG1;
      if (s == "G1a")
         return cbG1a;
      if (s == "G2a")
         return cbG2a;
      if (s == "G2")
         return cbG2;
      if (s == "G3")
         return cbG3;
      if (s == "E6")
         return cbE6;
      if (s == "E5b")
         return cbE5b;
      if (s == "E5ab")
         return cbE5ab;
      if (s == "B1")
         return cbB1;
      if (s == "B3")
         return cbB3;
      if (s == "B2")
         return cbB2;
      if (s == "I9")
         return cbI9;
      if (s == "Unknown")
         return cbUnknown;
      if (s == "Any")
         return cbAny;
      if (s == "Undefined")
         return cbUndefined;
      if (s == "Last")
         return cbLast;
      return cbUnknown;
   }


   std::string ObsID ::
   asString(TrackingCode e) throw()
   {
      switch (e)
      {
         case tcCA:        return "CA";
         case tcN:         return "N";
         case tcI5:        return "I5";
         case tcG1D:       return "G1D";
         case tcG1X:       return "G1X";
         case tcG1P:       return "G1P";
         case tcC2LM:      return "C2LM";
         case tcC2L:       return "C2L";
         case tcC2M:       return "C2M";
         case tcIQ5:       return "IQ5";
         case tcM:         return "M";
         case tcP:         return "P";
         case tcQ5:        return "Q5";
         case tcD:         return "D";
         case tcY:         return "Y";
         case tcW:         return "W";
         case tcL1OCD:     return "L1OCD";
         case tcL1OCP:     return "L1OCP";
         case tcL1OC:      return "L1OC";
         case tcL2CSIOCp:  return "L2CSIOCp";
         case tcL2CSI:     return "L2CSI";
         case tcL20CP:     return "L20CP";
         case tcIR3:       return "IR3";
         case tcIQR3:      return "IQR3";
         case tcQR3:       return "QR3";
         case tcGP:        return "GP";
         case tcGCA:       return "GCA";
         case tcA:         return "A";
         case tcABC:       return "ABC";
         case tcB:         return "B";
         case tcBC:        return "BC";
         case tcC:         return "C";
         case tcIE5:       return "IE5";
         case tcIQE5:      return "IQE5";
         case tcQE5:       return "QE5";
         case tcIE5a:      return "IE5a";
         case tcIQE5a:     return "IQE5a";
         case tcQE5a:      return "QE5a";
         case tcIE5b:      return "IE5b";
         case tcIQE5b:     return "IQE5b";
         case tcQE5b:      return "QE5b";
         case tcA6:        return "A6";
         case tcABC6:      return "ABC6";
         case tcB6:        return "B6";
         case tcBC6:       return "BC6";
         case tcC6:        return "C6";
         case tcSCA:       return "SCA";
         case tcSI5:       return "SI5";
         case tcSIQ5:      return "SIQ5";
         case tcSQ5:       return "SQ5";
         case tcJCA:       return "JCA";
         case tcJD1:       return "JD1";
         case tcJX1:       return "JX1";
         case tcJP1:       return "JP1";
         case tcJZ1:       return "JZ1";
         case tcJL2:       return "JL2";
         case tcJM2:       return "JM2";
         case tcJX2:       return "JX2";
         case tcJI5:       return "JI5";
         case tcJIQ5:      return "JIQ5";
         case tcJQ5:       return "JQ5";
         case tcJI5S:      return "JI5S";
         case tcJIQ5S:     return "JIQ5S";
         case tcJQ5S:      return "JQ5S";
         case tcJI6:       return "JI6";
         case tcJIQ6:      return "JIQ6";
         case tcJQ6:       return "JQ6";
         case tcJDE6:      return "JDE6";
         case tcJD6:       return "JD6";
         case tcJE6:       return "JE6";
         case tcCIQ1:      return "CIQ1";
         case tcCA1:       return "CA1";
         case tcCCD1:      return "CCD1";
         case tcCCDP1:     return "CCDP1";
         case tcCCP1:      return "CCP1";
         case tcCI1:       return "CI1";
         case tcCQ1:       return "CQ1";
         case tcCI2ab:     return "CI2ab";
         case tcCIQ2ab:    return "CIQ2ab";
         case tcCQ2ab:     return "CQ2ab";
         case tcCIQ7:      return "CIQ7";
         case tcCI2a:      return "CI2a";
         case tcCIQ2a:     return "CIQ2a";
         case tcCQ2a:      return "CQ2a";
         case tcCI2b:      return "CI2b";
         case tcCIQ2b:     return "CIQ2b";
         case tcCQ2b:      return "CQ2b";
         case tcCI7:       return "CI7";
         case tcCQ7:       return "CQ7";
         case tcCIQ6:      return "CIQ6";
         case tcCI6:       return "CI6";
         case tcCQ6:       return "CQ6";
         case tcCodelessC: return "CodelessC";
         case tcCIQ3A:     return "CIQ3A";
         case tcIB5:       return "IB5";
         case tcIX5:       return "IX5";
         case tcIC5:       return "IC5";
         case tcIB9:       return "IB9";
         case tcIX9:       return "IX9";
         case tcIC9:       return "IC9";
         case tcIA5:       return "IA5";
         case tcIA9:       return "IA9";
         case tcUnknown:   return "Unknown";
         case tcAny:       return "Any";
         case tcUndefined: return "Undefined";
         case tcLast:      return "Last";
         default:          return "???";
      }
   }


   ObsID::TrackingCode ObsID ::
   asTrackingCode(const std::string& s) throw()
   {
      if (s == "CA")
         return tcCA;
      if (s == "N")
         return tcN;
      if (s == "I5")
         return tcI5;
      if (s == "G1D")
         return tcG1D;
      if (s == "G1X")
         return tcG1X;
      if (s == "G1P")
         return tcG1P;
      if (s == "C2LM")
         return tcC2LM;
      if (s == "C2L")
         return tcC2L;
      if (s == "C2M")
         return tcC2M;
      if (s == "IQ5")
         return tcIQ5;
      if (s == "M")
         return tcM;
      if (s == "P")
         return tcP;
      if (s == "Q5")
         return tcQ5;
      if (s == "D")
         return tcD;
      if (s == "Y")
         return tcY;
      if (s == "W")
         return tcW;
      if (s == "L1OCD")
         return tcL1OCD;
      if (s == "L1OCP")
         return tcL1OCP;
      if (s == "L1OC")
         return tcL1OC;
      if (s == "L2CSIOCp")
         return tcL2CSIOCp;
      if (s == "L2CSI")
         return tcL2CSI;
      if (s == "L20CP")
         return tcL20CP;
      if (s == "IR3")
         return tcIR3;
      if (s == "IQR3")
         return tcIQR3;
      if (s == "QR3")
         return tcQR3;
      if (s == "GP")
         return tcGP;
      if (s == "GCA")
         return tcGCA;
      if (s == "A")
         return tcA;
      if (s == "ABC")
         return tcABC;
      if (s == "B")
         return tcB;
      if (s == "BC")
         return tcBC;
      if (s == "C")
         return tcC;
      if (s == "IE5")
         return tcIE5;
      if (s == "IQE5")
         return tcIQE5;
      if (s == "QE5")
         return tcQE5;
      if (s == "IE5a")
         return tcIE5a;
      if (s == "IQE5a")
         return tcIQE5a;
      if (s == "QE5a")
         return tcQE5a;
      if (s == "IE5b")
         return tcIE5b;
      if (s == "IQE5b")
         return tcIQE5b;
      if (s == "QE5b")
         return tcQE5b;
      if (s == "A6")
         return tcA6;
      if (s == "ABC6")
         return tcABC6;
      if (s == "B6")
         return tcB6;
      if (s == "BC6")
         return tcBC6;
      if (s == "C6")
         return tcC6;
      if (s == "SCA")
         return tcSCA;
      if (s == "SI5")
         return tcSI5;
      if (s == "SIQ5")
         return tcSIQ5;
      if (s == "SQ5")
         return tcSQ5;
      if (s == "JCA")
         return tcJCA;
      if (s == "JD1")
         return tcJD1;
      if (s == "JX1")
         return tcJX1;
      if (s == "JP1")
         return tcJP1;
      if (s == "JZ1")
         return tcJZ1;
      if (s == "JL2")
         return tcJL2;
      if (s == "JM2")
         return tcJM2;
      if (s == "JX2")
         return tcJX2;
      if (s == "JI5")
         return tcJI5;
      if (s == "JIQ5")
         return tcJIQ5;
      if (s == "JQ5")
         return tcJQ5;
      if (s == "JI5S")
         return tcJI5S;
      if (s == "JIQ5S")
         return tcJIQ5S;
      if (s == "JQ5S")
         return tcJQ5S;
      if (s == "JI6")
         return tcJI6;
      if (s == "JIQ6")
         return tcJIQ6;
      if (s == "JQ6")
         return tcJQ6;
      if (s == "JDE6")
         return tcJDE6;
      if (s == "JD6")
         return tcJD6;
      if (s == "JE6")
         return tcJE6;
      if (s == "CIQ1")
         return tcCIQ1;
      if (s == "CA1")
         return tcCA1;
      if (s == "CCD1")
         return tcCCD1;
      if (s == "CCDP1")
         return tcCCDP1;
      if (s == "CCP1")
         return tcCCP1;
      if (s == "CI1")
         return tcCI1;
      if (s == "CQ1")
         return tcCQ1;
      if (s == "CI2ab")
         return tcCI2ab;
      if (s == "CIQ2ab")
         return tcCIQ2ab;
      if (s == "CQ2ab")
         return tcCQ2ab;
      if (s == "CIQ7")
         return tcCIQ7;
      if (s == "CI2a")
         return tcCI2a;
      if (s == "CIQ2a")
         return tcCIQ2a;
      if (s == "CQ2a")
         return tcCQ2a;
      if (s == "CI2b")
         return tcCI2b;
      if (s == "CIQ2b")
         return tcCIQ2b;
      if (s == "CQ2b")
         return tcCQ2b;
      if (s == "CI7")
         return tcCI7;
      if (s == "CQ7")
         return tcCQ7;
      if (s == "CIQ6")
         return tcCIQ6;
      if (s == "CI6")
         return tcCI6;
      if (s == "CQ6")
         return tcCQ6;
      if (s == "CodelessC")
         return tcCodelessC;
      if (s == "CIQ3A")
         return tcCIQ3A;
      if (s == "IB5")
         return tcIB5;
      if (s == "IX5")
         return tcIX5;
      if (s == "IC5")
         return tcIC5;
      if (s == "IB9")
         return tcIB9;
      if (s == "IX9")
         return tcIX9;
      if (s == "IC9")
         return tcIC9;
      if (s == "IA5")
         return tcIA5;
      if (s == "IA9")
         return tcIA9;
      if (s == "Unknown")
         return tcUnknown;
      if (s == "Any")
         return tcAny;
      if (s == "Undefined")
         return tcUndefined;
      if (s == "Last")
         return tcLast;
      return tcUnknown;
   }
}
