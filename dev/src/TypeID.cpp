
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
//  Dagoberto Salazar - gAGE. 2007
//
//============================================================================

#include "TypeID.hpp"


/**
 * @file TypeID.cpp
 * gpstk::TypeID - Identifies types of values
 */

namespace gpstk
{
   std::map< TypeID::ValueType, std::string > TypeID::tStrings;

   TypeID::Initializer TypeIDsingleton;

   TypeID::Initializer::Initializer()
   {
      tStrings[Unknown] = "UnknownType";
      tStrings[C1]      = "C/A";
      tStrings[C2]      = "C2";
      tStrings[P1]      = "P1";
      tStrings[P2]      = "P2";
      tStrings[L1]      = "L1";
      tStrings[L2]      = "L2";
      tStrings[D1]      = "D1";
      tStrings[D2]      = "D2";
      tStrings[S1]      = "S1";
      tStrings[S2]      = "S2";
      tStrings[T1]      = "T1";
      tStrings[T2]      = "T2";
      tStrings[SSI1]     = "SSI1";
      tStrings[LLI1]     = "LLI1";
      tStrings[SSI2]     = "SSI2";
      tStrings[LLI2]     = "LLI2";
      tStrings[C5]      = "C5";
      tStrings[L5]      = "L5";
      tStrings[D5]      = "D5";
      tStrings[S5]      = "S5";
      tStrings[SSI5]    = "SSI5";
      tStrings[LLI5]    = "LLI5";
      tStrings[C6]      = "C6";
      tStrings[L6]      = "L6";
      tStrings[D6]      = "D6";
      tStrings[S6]      = "S6";
      tStrings[SSI6]    = "SSI6";
      tStrings[LLI6]    = "LLI6";
      tStrings[C7]      = "C7";
      tStrings[L7]      = "L7";
      tStrings[D7]      = "D7";
      tStrings[S7]      = "S7";
      tStrings[SSI7]    = "SSI7";
      tStrings[LLI7]    = "LLI7";
      tStrings[C8]      = "C8";
      tStrings[L8]      = "L8";
      tStrings[D8]      = "D8";
      tStrings[S8]      = "S8";
      tStrings[SSI8]    = "SSI8";
      tStrings[LLI8]    = "LLI8";
      tStrings[LC]      = "LC";
      tStrings[PI]      = "PI";
      tStrings[LI]      = "LI";
      tStrings[Pdelta]  = "Pdelta";
      tStrings[Ldelta]  = "Ldelta";
      tStrings[rho]     = "rho";
      tStrings[dtSat]   = "dtSat";
      tStrings[rel]     = "rel";
      tStrings[tropo]   = "tropo";
      tStrings[dryTropo] = "dry tropo";
      tStrings[wetTropo] = "wet tropo";
      tStrings[tropoSlant] = "slant tropo";
      tStrings[iono]    = "vertical iono";
      tStrings[ionoSlant] = "slant iono";
      tStrings[inst]    = "instrumental";
      tStrings[MP]      = "multipath";
      tStrings[windUp]  = "windup";
      tStrings[B]       = "ambiguity";
      tStrings[elevation] = "elevation";
      tStrings[azimuth] = "azimuth";
      tStrings[PR]      = "prefit residual";
      tStrings[rhoX]    = "unit vector X";
      tStrings[rhoY]    = "unit vector Y";
      tStrings[rhoZ]    = "unit vector Z";
      tStrings[dX]    = "dX";
      tStrings[dY]    = "dY";
      tStrings[dZ]    = "dZ";
      tStrings[dt]    = "Rx clock offset";
      tStrings[dLat]    = "dLat";
      tStrings[dLon]    = "dLon";
      tStrings[dH]    = "dH";
      tStrings[weight]    = "weight";
      tStrings[sigma]    = "sigma";
   }


   std::ostream& TypeID::dump(std::ostream& s) const
   {
      s << TypeID::tStrings[type];

      return s;
   } // TypeID::dump()

   bool TypeID::isValid() const
   {
      // At some time this needs to be implemented.
      return !(type==Unknown);
   }

   TypeID::ValueType TypeID::newValueType(const std::string& s)
   {
      ValueType newId = 
         static_cast<ValueType>(TypeID::tStrings.rbegin()->first + 1);
      TypeID::tStrings[newId] = s;
      return newId;
   }


   namespace StringUtils
   {
      // convert this object to a string representation
      std::string asString(const TypeID& p)      
      {
         std::ostringstream oss;
         p.dump(oss);
         return oss.str();
      }
   }
   
   // stream output for TypeID
   std::ostream& operator<<(std::ostream& s, const TypeID& p)
   {
      p.dump(s);
      return s;
   }
/*
   // Conversion from RinexObsType to TypeID
   inline TypeID::ValueType RinexType2TypeID(const RinexObsHeader::RinexObsType& rot)
   {
        if (rot == RinexObsHeader::UN) return TypeID::Unknown;
        if (rot == RinexObsHeader::C1) return TypeID::C1;
        if (rot == RinexObsHeader::C2) return TypeID::C2;
        if (rot == RinexObsHeader::P1) return TypeID::P1;
        if (rot == RinexObsHeader::P2) return TypeID::P2;
        if (rot == RinexObsHeader::L1) return TypeID::L1;
        if (rot == RinexObsHeader::L2) return TypeID::L2;
        if (rot == RinexObsHeader::D1) return TypeID::D1;
        if (rot == RinexObsHeader::D2) return TypeID::D2;
        if (rot == RinexObsHeader::S1) return TypeID::S1;
        if (rot == RinexObsHeader::S2) return TypeID::S2;
        // v 2.11
        if (rot == RinexObsHeader::C5) return TypeID::C5;
        if (rot == RinexObsHeader::L5) return TypeID::L5;
        if (rot == RinexObsHeader::D5) return TypeID::D5;
        if (rot == RinexObsHeader::S5) return TypeID::S5;
        // Galileo-related
        if (rot == RinexObsHeader::C6) return TypeID::C6;
        if (rot == RinexObsHeader::L6) return TypeID::L6;
        if (rot == RinexObsHeader::D6) return TypeID::D6;
        if (rot == RinexObsHeader::S6) return TypeID::S6;
        if (rot == RinexObsHeader::C7) return TypeID::C7;
        if (rot == RinexObsHeader::L7) return TypeID::L7;
        if (rot == RinexObsHeader::D7) return TypeID::D7;
        if (rot == RinexObsHeader::S7) return TypeID::S7;
        if (rot == RinexObsHeader::C8) return TypeID::C8;
        if (rot == RinexObsHeader::L8) return TypeID::L8;
        if (rot == RinexObsHeader::D8) return TypeID::D8;
        if (rot == RinexObsHeader::S8) return TypeID::S8;

        // Just in case, but it should never get this far
        return TypeID::Unknown;
   }
*/

}
