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
      otDesc[otUnknown]  = "UnknownType";
      otDesc[otRange]    = "range";
      otDesc[otPhase]    = "phase";
      otDesc[otDoppler]  = "doppler";
      otDesc[otSNR]      = "snr";
      otDesc[otSSI]      = "ssi";
      otDesc[otLLI]      = "lli";
      otDesc[otTrackLen] = "tlen";

      cbDesc[cbUnknown] = "UnknownBand";
      cbDesc[cbL1]      = "L1";
      cbDesc[cbL2]      = "L2";
      cbDesc[cbL5]      = "L5";
      cbDesc[cbL1L2]    = "L1+L2";
      cbDesc[cbG1]      = "G1";
      cbDesc[cbG2]      = "G2";
      cbDesc[cbE5b]     = "E5b";
      cbDesc[cbE5ab]    = "L5a+b";
      cbDesc[cbE6]      = "E6";

      tcDesc[tcUnknown] = "UnknownCode";
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

      if (otDesc.size() != (int)otLast)
         std::cout << "Error in otDesc" << std::endl;
      if (cbDesc.size() != (int)cbLast)
         std::cout << "Error in cbDesc" << std::endl;
      if (tcDesc.size() != (int)tcLast)
         std::cout << "Error in tcDesc" << std::endl;
      
      rinex2ot['C'] = otRange;
      rinex2ot['L'] = otPhase;
      rinex2ot['D'] = otDoppler;
      rinex2ot['S'] = otSNR;

      rinex2cb['1'] = cbL1;
      rinex2cb['2'] = cbL1;
      rinex2cb['5'] = cbL5;
      rinex2cb['6'] = cbE6;
      rinex2cb['7'] = cbE5b;
      rinex2cb['8'] = cbE5ab;

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
   ObsID::ObsID(const std::string& id) throw(InvalidParameter)
   {
      int i = id.length() - 3;
      if ( i < 0 || i > 1)
      {
         InvalidParameter e("identifier must be 3 or 4 characters long");
         GPSTK_THROW(e);
      }

      type = rinex2ot[ id[i] ];
      band = rinex2cb[ id[i+1] ];
      code = rinex2tc[ id[i+2] ];

      /// This next block takes care of fixing up the codes that are reused
      /// betweern the various signals
      if (i==0 || id[0] == 'G') // GPS
      {
         if (id[i+2]=='X' && band==cbL5)
            code = tcIQ5;
      }
      if (id[0] == 'E') // Galileo
      {
         switch (code)
         {
            case tcCA: code = tcC; break;
            case tcI5: code = tcIE5; break;
            case tcQ5: code = tcQE5; break;
         }
         if (id[i+2] == 'X')
         {
            if (band == cbL1 || band == cbE6)
               code = tcBC;
            else if (band == cbL5 || band == cbE5b || band == cbE5ab)
               code = tcIQE5;
         }
      }
      else if (id[0] == 'R') // Glonass
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
   // Rinex 3 identifier is the same as for the ObsID constructor.  If there are spaces
   // the provided identifier, they are ignored
   ObsID ObsID::newID(const std::string& rinexID, const std::string& desc)
      throw(InvalidParameter)
   {
      using namespace std;
      ObsID oid(rinexID);
      // At least one parameter needs to be unknown 
      if (oid.type != otUnknown && oid.band != cbUnknown && oid.code != tcUnknown)
         GPSTK_THROW(InvalidParameter("Identifier " + rinexID + " is already defined"));

      int i = rinexID.length() - 3;
      if (oid.type == otUnknown && rinexID[i] != ' ')
      {
         oid.type = (ObservationType)otDesc.size();
         otDesc[oid.type] = desc;
         rinex2ot[rinexID[i]] = oid.type;
         ot2Rinex[oid.type] = rinexID[i];
      }
      if (oid.band == cbUnknown && rinexID[i+1] != ' ')
      {
         oid.band = (CarrierBand)cbDesc.size();
         cbDesc[oid.band] = desc;
         rinex2cb[rinexID[i+1]] = oid.band;
         cb2Rinex[oid.band] = rinexID[i+1];
      }
      if (oid.code == tcUnknown && rinexID[i+2] != ' ')
      {
         oid.code = (TrackingCode) tcDesc.size();
         tcDesc[oid.code];
         rinex2tc[rinexID[i+2]] = oid.code;
         tc2Rinex[oid.code] = rinexID[i+2];
      }
      return oid;
   }


   // Equality requires all fields to be the same
   bool ObsID::operator==(const ObsID& right) const
   { return type==right.type &&  band==right.band && code==right.code; }


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
