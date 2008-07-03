#pragma ident "$Id$"

/**
 * @file TypeID.cpp
 * gpstk::TypeID - Identifies types of values
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008
//
//============================================================================


#include "TypeID.hpp"


namespace gpstk
{

   std::map< TypeID::ValueType, std::string > TypeID::tStrings;


   TypeID::Initializer TypeIDsingleton;


      // Let's assign type descriptions
   TypeID::Initializer::Initializer()
   {
      tStrings[Unknown]    = "UnknownType";
      tStrings[C1]         = "C1";
      tStrings[C2]         = "C2";
      tStrings[P1]         = "P1";
      tStrings[P2]         = "P2";
      tStrings[L1]         = "L1";
      tStrings[L2]         = "L2";
      tStrings[D1]         = "D1";
      tStrings[D2]         = "D2";
      tStrings[S1]         = "S1";
      tStrings[S2]         = "S2";
      tStrings[T1]         = "T1";
      tStrings[T2]         = "T2";
      tStrings[SSI1]       = "SSI1";
      tStrings[LLI1]       = "LLI1";
      tStrings[SSI2]       = "SSI2";
      tStrings[LLI2]       = "LLI2";
      tStrings[C5]         = "C5";
      tStrings[L5]         = "L5";
      tStrings[D5]         = "D5";
      tStrings[S5]         = "S5";
      tStrings[SSI5]       = "SSI5";
      tStrings[LLI5]       = "LLI5";
      tStrings[C6]         = "C6";
      tStrings[L6]         = "L6";
      tStrings[D6]         = "D6";
      tStrings[S6]         = "S6";
      tStrings[SSI6]       = "SSI6";
      tStrings[LLI6]       = "LLI6";
      tStrings[C7]         = "C7";
      tStrings[L7]         = "L7";
      tStrings[D7]         = "D7";
      tStrings[S7]         = "S7";
      tStrings[SSI7]       = "SSI7";
      tStrings[LLI7]       = "LLI7";
      tStrings[C8]         = "C8";
      tStrings[L8]         = "L8";
      tStrings[D8]         = "D8";
      tStrings[S8]         = "S8";
      tStrings[SSI8]       = "SSI8";
      tStrings[LLI8]       = "LLI8";
      tStrings[PC]         = "PC";
      tStrings[LC]         = "LC";
      tStrings[PI]         = "PI";
      tStrings[LI]         = "LI";
      tStrings[Pdelta]     = "Pdelta";
      tStrings[Ldelta]     = "Ldelta";
      tStrings[MWubbena]   = "MWubbena";
      tStrings[rho]        = "rho";
      tStrings[dtSat]      = "dtSat";
      tStrings[rel]        = "rel";
      tStrings[gravDelay]  = "gravDelay";
      tStrings[tropo]      = "tropo";
      tStrings[dryTropo]   = "dryTropo";
      tStrings[dryMap]     = "dryTropoMap";
      tStrings[wetTropo]   = "wetTropo";
      tStrings[wetMap]     = "wetTropoMap";
      tStrings[tropoSlant] = "slantTropo";
      tStrings[iono]       = "verticalIono";
      tStrings[ionoMap]    = "ionoMap";
      tStrings[ionoSlant]  = "slantIono";
      tStrings[windUp]     = "windup";
      tStrings[satPCenter] = "satPhaseCenter";
      tStrings[satX]       = "satX";
      tStrings[satY]       = "satY";
      tStrings[satZ]       = "satZ";
      tStrings[elevation]  = "elevation";
      tStrings[azimuth]    = "azimuth";
      tStrings[CSL1]       = "CSL1";
      tStrings[CSL2]       = "CSL2";
      tStrings[CSL5]       = "CSL5";
      tStrings[CSL6]       = "CSL6";
      tStrings[CSL7]       = "CSL7";
      tStrings[CSL8]       = "CSL8";
      tStrings[satArc]     = "satArc";
      tStrings[BL1]        = "ambiguityL1";
      tStrings[BL2]        = "ambiguityL2";
      tStrings[BL5]        = "ambiguityL5";
      tStrings[BL6]        = "ambiguityL6";
      tStrings[BL7]        = "ambiguityL7";
      tStrings[BL8]        = "ambiguityL8";
      tStrings[mpC1]       = "multipathC1";
      tStrings[mpC2]       = "multipathC2";
      tStrings[mpC5]       = "multipathC5";
      tStrings[mpC6]       = "multipathC6";
      tStrings[mpC7]       = "multipathC7";
      tStrings[mpC8]       = "multipathC8";
      tStrings[mpL1]       = "multipathL1";
      tStrings[mpL2]       = "multipathL2";
      tStrings[mpL5]       = "multipathL5";
      tStrings[mpL6]       = "multipathL6";
      tStrings[mpL7]       = "multipathL7";
      tStrings[mpL8]       = "multipathL8";
      tStrings[instC1]     = "instrumentalC1";
      tStrings[instC2]     = "instrumentalC2";
      tStrings[instC5]     = "instrumentalC5";
      tStrings[instC6]     = "instrumentalC6";
      tStrings[instC7]     = "instrumentalC7";
      tStrings[instC8]     = "instrumentalC8";
      tStrings[instL1]     = "instrumentalL1";
      tStrings[instL2]     = "instrumentalL2";
      tStrings[instL5]     = "instrumentalL5";
      tStrings[instL6]     = "instrumentalL6";
      tStrings[instL7]     = "instrumentalL7";
      tStrings[instL8]     = "instrumentalL8";
      tStrings[prefitC]    = "prefitResidualCode";
      tStrings[prefitL]    = "prefitResidualPhase";
      tStrings[postfitC]   = "posfitResidualCode";
      tStrings[postfitL]   = "posfitResidualPhase";
      tStrings[dx]         = "dx";
      tStrings[dy]         = "dy";
      tStrings[dz]         = "dz";
      tStrings[cdt]        = "cdt";
      tStrings[dLat]       = "dLat";
      tStrings[dLon]       = "dLon";
      tStrings[dH]         = "dH";
      tStrings[weight]     = "weight";
      tStrings[recX]       = "RxPositionX";
      tStrings[recY]       = "RxPositionY";
      tStrings[recZ]       = "RxPositionZ";
      tStrings[recLat]     = "RxLat";
      tStrings[recLon]     = "RxLon";
      tStrings[recH]       = "RxH";
      tStrings[sigma]      = "sigma";
      tStrings[iura]       = "iura";
      tStrings[Last]       = "Last";
      tStrings[Placeholder]= "Placeholder";
   }


      // Assignment operator
   TypeID TypeID::operator=(const TypeID& right)
   {
      if ( this == &right ) return (*this);
      (*this).type = right.type;
      return *this;
   }


      // Convenience output method
   std::ostream& TypeID::dump(std::ostream& s) const
   {
      s << TypeID::tStrings[type];

      return s;
   } // TypeID::dump()


      // Returns true if this is a valid TypeID. Basically just
      // checks that the enum is defined
   bool TypeID::isValid() const
   {
      return !(type==Unknown);
   }


      /* Static method to add new TypeID's
       * @param string      Identifying string for the new TypeID
       */
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

   }  // End of namespace StringUtils


      // stream output for TypeID
   std::ostream& operator<<(std::ostream& s, const TypeID& p)
   {
      p.dump(s);
      return s;
   }


      // Conversion from RinexObsType to TypeID
   TypeID::ValueType RinexType2TypeID(const RinexObsHeader::RinexObsType& rot)
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


} // End of namespace gpstk
