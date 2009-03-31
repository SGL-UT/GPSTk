#pragma ident "$Id$"

/**
 * @file ObsID.cpp
 * gpstk::ObsID - Identifies types of observations
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



#include "ObsID.hpp"


namespace gpstk
{
   std::map< ObsID::TrackingCode,    std::string > ObsID::tcDesc;
   std::map< ObsID::CarrierBand,     std::string > ObsID::cbDesc;
   std::map< ObsID::ObservationType, std::string > ObsID::otDesc;
   std::map< char, ObsID::ObservationType> ObsID::rinex2ot;
   std::map< char, ObsID::CarrierBand> ObsID::rinex2cb;
   std::map< char, ObsID::TrackingCode> ObsID::rinex2tc;
   std::map< ObsID::ObservationType, char > ObsID::ot2Rinex;
   std::map< ObsID::CarrierBand, char > ObsID::cb2Rinex;
   std::map< ObsID::TrackingCode, char> ObsID::tc2Rinex;


   ObsID::Initializer singleton;

   ObsID::Initializer::Initializer()
   {
      otDesc[otUnknown]   = "UnknownType";
      otDesc[otAny]       = "AnyType";
      otDesc[otRange]     = "range";
      otDesc[otPhase]     = "phase";
      otDesc[otDoppler]   = "doppler";
      otDesc[otSNR]       = "snr";
      otDesc[otChannel]   = "channel";
      otDesc[otIono]      = "iono";
      otDesc[otSSI]       = "ssi";
      otDesc[otLLI]       = "lli";
      otDesc[otTrackLen]  = "tlen";
      otDesc[otUndefined] = "undefined";

      cbDesc[cbUnknown]   = "UnknownBand";
      cbDesc[cbAny]       = "AnyBand";
      cbDesc[cbZero]      = "";
      cbDesc[cbL1]        = "L1";
      cbDesc[cbL2]        = "L2";
      cbDesc[cbL5]        = "L5";
      cbDesc[cbL1L2]      = "L1+L2";
      cbDesc[cbG1]        = "G1";
      cbDesc[cbG2]        = "G2";
      cbDesc[cbE5b]       = "E5b";
      cbDesc[cbE5ab]      = "L5a+b";
      cbDesc[cbE6]        = "E6";
      cbDesc[cbUndefined] = "undefined";

      tcDesc[tcUnknown] = "UnknownCode";
      tcDesc[tcAny]     = "AnyCode";
      tcDesc[tcCA]      = "C/A";
      tcDesc[tcP]       = "P";
      tcDesc[tcY]       = "Y";
      tcDesc[tcW]       = "W";
      tcDesc[tcN]       = "N";
      tcDesc[tcD]       = "D";
      tcDesc[tcM]       = "M";
      tcDesc[tcC2M]     = "C2M";
      tcDesc[tcC2L]     = "C2L";
      tcDesc[tcC2LM]    = "C2L+M";
      tcDesc[tcI5]      = "I5";
      tcDesc[tcQ5]      = "Q5";
      tcDesc[tcIQ5]     = "I+Q5";
      tcDesc[tcGCA]     = "C/A";
      tcDesc[tcGP]      = "P";
      tcDesc[tcA]       = "A";
      tcDesc[tcB]       = "B";
      tcDesc[tcC]       = "C";
      tcDesc[tcBC]      = "B+C";
      tcDesc[tcABC]     = "A+B+C";
      tcDesc[tcIE5]     = "IE5";
      tcDesc[tcQE5]     = "QE5";
      tcDesc[tcIQE5]    = "I+QE5";
      tcDesc[tcUndefined] = "undefined";

      if (otDesc.size() != (int)otLast)
         std::cout << "Error in otDesc" << std::endl;
      if (cbDesc.size() != (int)cbLast)
         std::cout << "Error in cbDesc" << std::endl;
      if (tcDesc.size() != (int)tcLast)
         std::cout << "Error in tcDesc" << std::endl;

      // The following definitions really should only describe the items that are
      // in the Rinex 3 specification. If an application needs additional ObsID
      // types to be able to be translated to/from Rinex3, the additional types
      // must be added by the application.
      rinex2ot[' '] = otUnknown;
      rinex2ot['*'] = otAny;
      rinex2ot['C'] = otRange;
      rinex2ot['L'] = otPhase;
      rinex2ot['D'] = otDoppler;
      rinex2ot['S'] = otSNR;
      rinex2ot['-'] = otUndefined;

      rinex2cb[' '] = cbUnknown;
      rinex2cb['*'] = cbAny;
      rinex2cb['1'] = cbL1;
      rinex2cb['2'] = cbL2;
      rinex2cb['5'] = cbL5;
      rinex2cb['6'] = cbE6;
      rinex2cb['7'] = cbE5b;
      rinex2cb['8'] = cbE5ab;
      rinex2cb['-'] = cbUndefined;

      rinex2tc[' '] = tcUnknown;
      rinex2tc['*'] = tcAny;
      rinex2tc['C'] = tcCA;
      rinex2tc['P'] = tcP;  
      rinex2tc['W'] = tcW;
      rinex2tc['Y'] = tcY;
      rinex2tc['M'] = tcM;
      rinex2tc['N'] = tcN;
      rinex2tc['D'] = tcD;
      rinex2tc['S'] = tcC2M;
      rinex2tc['L'] = tcC2L;
      rinex2tc['X'] = tcC2LM;
      rinex2tc['I'] = tcI5;
      rinex2tc['Q'] = tcQ5;
      rinex2tc['A'] = tcA;
      rinex2tc['B'] = tcB;
      rinex2tc['Z'] = tcABC;
      rinex2tc['-'] = tcUndefined;

      for (int i=otUnknown; i<otLast; i++) ot2Rinex[(ObservationType)i] = ' ';
      for (int i=cbUnknown; i<cbLast; i++) cb2Rinex[(CarrierBand)i] = ' ';
      for (int i=tcUnknown; i<tcLast; i++) tc2Rinex[(TrackingCode)i] = ' ';

      // Here the above three maps are reversed to speed up the runtime
      for (std::map< char, ObservationType>::const_iterator i=rinex2ot.begin();
           i != rinex2ot.end(); i++)
         ot2Rinex[i->second] = i->first;

      for (std::map< char, CarrierBand>::const_iterator i=rinex2cb.begin();
           i != rinex2cb.end(); i++)
         cb2Rinex[i->second] = i->first;

      for (std::map< char, TrackingCode>::const_iterator i=rinex2tc.begin();
           i != rinex2tc.end(); i++)
         tc2Rinex[i->second] = i->first;

      // And add the couple 'special' cases
      tc2Rinex[tcC] = 'C';
      tc2Rinex[tcIE5] = 'I';
      tc2Rinex[tcQE5] = 'Q';
      tc2Rinex[tcIQE5] = 'X';
      tc2Rinex[tcIQ5] = 'X';
      tc2Rinex[tcBC] = 'X';
   }


   // Construct this object from the rinex3 specifier
   ObsID::ObsID(const std::string& rinexID) throw(InvalidParameter)
   {
      int i = rinexID.length() - 3;
      if ( i < 0 || i > 1)
      {
         InvalidParameter e("identifier must be 3 or 4 characters long");
         GPSTK_THROW(e);
      }

      char sys = i ? rinexID[0] : 'G';
      char ot = rinexID[i];
      char cb = rinexID[i+1];
      char tc = rinexID[i+2];
      
      if (!rinex2ot.count(ot) || !rinex2cb.count(cb) || !rinex2tc.count(tc))
         idCreator(rinexID.substr(i,3));

      type = rinex2ot[ ot ];
      band = rinex2cb[ cb ];
      code = rinex2tc[ tc ];

      /// This next block takes care of fixing up the codes that are reused
      /// betweern the various signals
      if (sys == 'G') // GPS
      {
         if (tc=='X' && band==cbL5)
            code = tcIQ5;
      }
      if (sys == 'E') // Galileo
      {
         switch (code)
         {
            case tcCA: code = tcC; break;
            case tcI5: code = tcIE5; break;
            case tcQ5: code = tcQE5; break;
         }
         if (tc == 'X')
         {
            if (band == cbL1 || band == cbE6)
               code = tcBC;
            else if (band == cbL5 || band == cbE5b || band == cbE5ab)
               code = tcIQE5;
         }
      }
      else if (sys == 'R') // Glonass
      {
         switch (code)
         {
            case tcCA: code = tcGCA; break;
            case tcP: code = tcGP; break;
         }
      }
   }


   // Convenience output method
   std::ostream& ObsID::dump(std::ostream& s) const
   {
      s << ObsID::cbDesc[band] << " "
        << ObsID::tcDesc[code] << " "
        << ObsID::otDesc[type];
      return s;
   } // ObsID::dump()



   // Represent this object using the Rinex3 notation
   std::string ObsID::asRinex3ID() const
   {
      char buff[4];

      buff[0] = ot2Rinex[type];
      buff[1] = cb2Rinex[band];
      buff[2] = tc2Rinex[code];
      buff[3] = 0;
      return std::string(buff);
   }


   // This is used to register a new ObsID & Rinex 3 identifier.  The syntax for the
   // Rinex 3 identifier is the same as for the ObsID constructor. Four character 
   // identifiers (like are allowed in the constructor) are not allowed. 
   // If there are spaces in the provided identifier, they are ignored
   ObsID ObsID::newID(const std::string& rinexID, const std::string& desc)
      throw(InvalidParameter)
   {
      if (rinex2ot.count(rinexID[0]) && 
          rinex2cb.count(rinexID[1]) && 
          rinex2tc.count(rinexID[2]))
          GPSTK_THROW(InvalidParameter("Identifier " + rinexID + " already defined."));

      return idCreator(rinexID, desc);
   }


   ObsID ObsID::idCreator(const std::string& rinexID, const std::string& desc)
   {
      char ot = rinexID[0];
      ObservationType type;
      if (!rinex2ot.count(ot))
      {
         type = (ObservationType)otDesc.size();
         otDesc[type] = desc;
         rinex2ot[ot] = type;
         ot2Rinex[type] = ot;
      }
      else
         type = rinex2ot[ot];

      char cb = rinexID[1];
      CarrierBand band;
      if (!rinex2cb.count(cb))
      {
         band = (CarrierBand)cbDesc.size();
         cbDesc[band] = desc;
         rinex2cb[cb] = band;
         cb2Rinex[band] = cb;
      }
      else
         band = rinex2cb[cb];

      char tc = rinexID[2];
      TrackingCode code;
      if (!rinex2tc.count(tc))
      {
         code = (TrackingCode) tcDesc.size();
         tcDesc[code] = desc;
         rinex2tc[tc] = code;
         tc2Rinex[code] = tc;
      }
      else
         code = rinex2tc[tc];
      
      return ObsID(type, band, code);
   }


   // Equality requires all fields to be the same unless the field is unknown
   bool ObsID::operator==(const ObsID& right) const
   {
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

      // convert this object to a string representation
      std::string asRinex3ID(const ObsID& p)
      {
         return p.asRinex3ID();
      }
   }


   // stream output for ObsID
   std::ostream& operator<<(std::ostream& s, const ObsID& p)
   {
      p.dump(s);
      return s;
   }

}
