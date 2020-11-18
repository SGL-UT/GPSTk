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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 * @file RinexObsID.cpp
 * gpstk::RinexObsID - Identifies types of observations
 */

#include "RinexObsID.hpp"
#include "RinexSatID.hpp"
#include "StringUtils.hpp"
#include "Rinex3ObsHeader.hpp"

namespace gpstk
{

   // string containing the system characters for all valid RINEX systems.
   std::string RinexObsID::validRinexSystems("GRESCJI");

   // maps between 1-char and 3-char system id
   std::map<std::string, std::string> RinexObsID::map1to3sys {
      { "G", "GPS" },
      { "R", "GLO" },
      { "E", "GAL" },
      { "S", "GEO" },
      { "C", "BDS" },
      { "J", "QZS" },
      { "I", "IRN" }
   };

   std::map<std::string, std::string> RinexObsID::map3to1sys {
      { "GPS", "G" },
      { "GLO", "R" },
      { "GAL", "E" },
      { "GEO", "S" },
      { "BDS", "C" },
      { "QZS", "J" },
      { "IRN", "I" }
   };

   // string containing the frequency digits for all valid RINEX systems.
   std::string RinexObsID::validRinexFrequencies("123456789");

   /// Construct this object from the string specifier

   RinexObsID::RinexObsID(const RinexObsType& rot)
         : ObsID()
   {
         // Note that the choice of tracking code for L1, L2, S1, S2
         // are arbitrary since they are ambiguous in the rinex 2
         // specifications
         // L1 -> L1P; P1 -> C1P; C1 -> C1C; S1 -> S1P; D1 -> D1P
      if (rot == RinexObsHeader::L1)
      {
         type=ObservationType::Phase;
         band=CarrierBand::L1;
         code=TrackingCode::P;
      }
      else if (rot == RinexObsHeader::P1)
      {
         type=ObservationType::Range;
         band=CarrierBand::L1;
         code=TrackingCode::P;
      }
      else if (rot == RinexObsHeader::C1)
      {
         type=ObservationType::Range;
         band=CarrierBand::L1;
         code=TrackingCode::CA;
      }
      else if (rot == RinexObsHeader::S1)
      {
         type=ObservationType::SNR;
         band=CarrierBand::L1;
         code=TrackingCode::P;
      }
      else if (rot == RinexObsHeader::D1)
      {
         type=ObservationType::Doppler;
         band=CarrierBand::L1;
         code=TrackingCode::P;
      }
         // L2 -> L2P; P2 -> C2P; C2 -> C2X; S2 -> S2P; D2 -> D2P
      else if (rot == RinexObsHeader::L2)
      {
         type=ObservationType::Phase;
         band=CarrierBand::L2;
         code=TrackingCode::P;
      }
      else if (rot == RinexObsHeader::P2)
      {
         type=ObservationType::Range;
         band=CarrierBand::L2;
         code=TrackingCode::P;
      }
      else if (rot == RinexObsHeader::C2)
      {
         type=ObservationType::Range;
         band=CarrierBand::L2;
         code=TrackingCode::L2CML;
      }
      else if (rot == RinexObsHeader::S2)
      {
         type=ObservationType::SNR;
         band=CarrierBand::L2;
         code=TrackingCode::P;
      }
      else if (rot == RinexObsHeader::D2)
      {
         type=ObservationType::Doppler;
         band=CarrierBand::L2;
         code=TrackingCode::P;
      }
   }

   // Represent this object using the Rinex3 notation
   std::string RinexObsID::asString(double version) const
   {
      char buff[4];

      buff[0] = ot2char[type];
      buff[1] = cb2char[band];
      buff[2] = tc2char[code];
      if ((fabs(version - 3.02) < 0.005) && (band == CarrierBand::B1) &&
          ((code == TrackingCode::B1I) || (code == TrackingCode::B1Q) ||
           (code == TrackingCode::B1IQ)))
      {
            // kludge for RINEX 3.02 BDS codes
         buff[1] = '1';
      }
         // special cases.
      if (type == ObservationType::Iono)
      {
         buff[2] = ' ';
      }
      else if (type == ObservationType::Channel)
      {
         buff[1] = '1';
         buff[2] = ' ';
      }
      buff[3] = 0;
      return std::string(buff);
   }

   namespace StringUtils
   {
      // convert this object to a string representation
      std::string asString(const RinexObsID& p)
      {
         return p.asString();
      }
   }

   // RINEX 3.03 document, section 5.1
   // GPS
   //       L1   C,S,L,X,P,W,Y,M,N(but not C1N)       G 1 CSLXPWYMN (but not C1N)
   //       L2   C,D,S,L,X,P,W,Y,M,N(but not C2N)     G 2 CDSLXPWYMN (but not C1N)
   //       L5   I,Q,X                                G 5 IQX
   // GLO
   //       G1   C,P                                  R 1 CP
   //       G2   C,P                                  R 2 CP
   // GAL
   //       E1   A,B,C,X,Z                            E 1 ABCXZ
   //       E5a  I,Q,X                                E 5 IQX
   //       E5b  I,Q,X                                E 7 IQX
   //       E5ab I,Q,X                                E 8 IQX
   //       E6   A,B,C,X,Z                            E 6 ABCXZ
   // SBAS
   //       L1   C                                    S 1 C
   //       L5   I,Q,X                                S 5 IQX
   // BDS
   //       B1   I,Q,X                                C 1 IQX
   //       B2   I,Q,X                                C 7 IQX
   //       B3   I,Q,X                                C 6 IQX
   // QZSS
   //       L1   C,S,L,X,Z                            J 1 CSLXZ
   //       L2   S,L,X                                J 2 SLX
   //       L5   I,Q,X                                J 5 IQX
   //       L6   S,L,X                                J 6 SLX
   // IRNSS
   //       L5   A,B,C,X                              I 5 ABCX
   //       L9   A,B,C,X                              I 9 ABCX

   // Determine if the given ObsID is valid. If the input string is 3
   // characters long, the system is assumed to be GPS. If this string is 4
   // characters long, the first character is the system designator as
   // described in the Rinex 3 specification.
   bool isValidRinexObsID(const std::string& strID)
   {
      int i(static_cast<int>(strID.length())-3);
      if(i < 0 || i > 1)
      {
         return false;
      }

      char sys;
      std::string id;

      if(i == 1)
      {
         sys = strID[0];
         id = strID.substr(1);
         return isValidRinexObsID(id,sys);
      }

      // test all RINEX systems
      std::string syss(RinexObsID::validRinexSystems);
      for(size_t j=0; j<syss.size(); j++)
      {
         if(isValidRinexObsID(strID,syss[j]))
         {
            return true;
         }
      }

      return false;
   }

   // Determine if the given ObsID is valid, for the given system
   bool isValidRinexObsID(const std::string& strID, const char sys)
   {
      if(strID.length() != 3)
      {
         return false;
      }
      char ot(strID[0]);
      char cb(strID[1]);
      char tc(strID[2]);
      const std::string &codes(RinexObsID::validRinexTrackingCodes[sys][cb]);
      if(ot == ' ' || ot == '-')
      {
         return false;
      }
      if (RinexObsID::char2ot.find(ot) == RinexObsID::char2ot.end())
      {
         return false;
      }
      if(codes.find(std::string(1,tc)) == std::string::npos)
      {
         return false;
      }
         // special cases for iono and channel num
      if (ot == 'I' && ((tc != ' ') || (cb < '1') || (cb > '9')))
      {
         return false;
      }
      if (ot == 'X' && ((tc != ' ') || (cb != '1')))
      {
         return false;
      }
      if (((codes == "* ") || (codes == " *")) && (ot == 'I'))
      {
            // channel num must always be "band" 1, but if the system
            // doesn't have any actual data on "1" band, we don't want
            // to accidentally say that we can get iono delay data for
            // a band that isn't valid for the system.
         return false;
      }
      if(sys == 'G' && ot == 'C' && tc == 'N')           // the one exception
      {
         return false;
      }

      return true;
   }

   std::ostream& RinexObsID::dumpCheck(std::ostream& s)
   {
      try {
         const std::string types("CLDS");
         std::map<char,std::string>::const_iterator it;

         for(size_t i=0; i<RinexObsID::validRinexSystems.size(); i++) {
            char csys = RinexObsID::validRinexSystems[i];
            std::string sys = RinexObsID::validRinexSystems.substr(i,1);
            RinexSatID sat(sys);
            std::string system(sat.systemString());

            s << "System " << sys << " = " << system << ", frequencies ";
            for(it = RinexObsID::validRinexTrackingCodes[sys[0]].begin();
               it != RinexObsID::validRinexTrackingCodes[sys[0]].end(); ++it)
               s << it->first;
            s << std::endl;

            for(it = RinexObsID::validRinexTrackingCodes[sys[0]].begin();
               it != RinexObsID::validRinexTrackingCodes[sys[0]].end(); ++it)
            {
               s << "   " << system << "(" << sys << "), freq " << it->first
                  << ", codes '" << it->second << "'" << std::endl;
               std::string codes(it->second), str;
               for(size_t j=0; j<codes.size(); ++j) {
                  std::ostringstream oss1;
                  for(size_t k=0; k<types.size(); ++k) {
                     str = std::string(1,types[k]) + std::string(1,it->first)
                           + std::string(1,codes[j]);
                     std::ostringstream oss;
                     if(!isValidRinexObsID(str,csys))
                        oss << str << " " << "-INVALID-";
                     else
                     {
                        RinexObsID robsid(sys+str,
                                          Rinex3ObsBase::currentVersion);
                        oss << str << " " << robsid;
                     }
                     oss1 << " " << StringUtils::leftJustify(oss.str(),34);
                  }
                  s << StringUtils::stripTrailing(oss1.str()) << std::endl;
               }
            }
         }
      }
      catch(Exception& e) {
         s << "Exception: " << e.what() << std::endl;
         GPSTK_RETHROW(e);
      }

      return s;
   }

   bool RinexObsID ::
   equalIndex(const RinexObsID& right)
      const
   {
      if (type != right.type)
         return false;
      if (type == ObservationType::Iono)
      {
            // only check band for ionospheric delay.
         return band == right.band;
      }
      if (type == ObservationType::Channel)
      {
            // There's only one channel type pseudo-observable
         return true;
      }
         // use the default for everything else
      return operator==(right);
   }

      // This is used to register a new RinexObsID & Rinex 3
      // identifier.  The syntax for the Rinex 3 identifier is the
      // same as for the RinexObsID constructor.  If there are spaces
      // in the provided identifier, they are ignored
   RinexObsID RinexObsID ::
   newID(const std::string& strID, const std::string& desc)
   {
      if (char2ot.count(strID[0]) && 
          char2cb.count(strID[1]) && 
          char2tc.count(strID[2]))
      {
         GPSTK_THROW(InvalidParameter("Identifier " + strID +
                                      " already defined."));
      }

      return idCreator(strID, desc);
   }


   RinexObsID RinexObsID ::
   idCreator(const std::string& strID, const std::string& desc)
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
 
      return RinexObsID(type, band, code);
   }
}  // end namespace
