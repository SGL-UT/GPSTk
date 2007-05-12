
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
      tStrings[C1]      = "C1";
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
      tStrings[SSI1]    = "SSI1";
      tStrings[LLI1]    = "LLI1";
      tStrings[SSI2]    = "SSI2";
      tStrings[LLI2]    = "LLI2";
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
      tStrings[PC]      = "PC";
      tStrings[LC]      = "LC";
      tStrings[PI]      = "PI";
      tStrings[LI]      = "LI";
      tStrings[Pdelta]  = "Pdelta";
      tStrings[Ldelta]  = "Ldelta";
      tStrings[rho]     = "rho";
      tStrings[dtSat]   = "dtSat";
      tStrings[rel]     = "rel";
      tStrings[tropo]   = "tropo";
      tStrings[dryTropo] = "dryTropo";
      tStrings[wetTropo] = "wetTropo";
      tStrings[tropoSlant] = "slantTropo";
      tStrings[iono]    = "verticalIono";
      tStrings[ionoSlant] = "slantIono";
      tStrings[windUp]  = "windup";
      tStrings[satX]    = "satX";
      tStrings[satY]    = "satY";
      tStrings[satZ]    = "satZ";
      tStrings[elevation] = "elevation";
      tStrings[azimuth] = "azimuth";
      tStrings[BL1]     = "ambiguityL1";
      tStrings[BL2]     = "ambiguityL2";
      tStrings[BL5]     = "ambiguityL5";
      tStrings[BL6]     = "ambiguityL6";
      tStrings[BL7]     = "ambiguityL7";
      tStrings[BL8]     = "ambiguityL8";
      tStrings[mpC1]    = "multipathC1";
      tStrings[mpC2]    = "multipathC2";
      tStrings[mpC5]    = "multipathC5";
      tStrings[mpC6]    = "multipathC6";
      tStrings[mpC7]    = "multipathC7";
      tStrings[mpC8]    = "multipathC8";
      tStrings[mpL1]    = "multipathL1";
      tStrings[mpL2]    = "multipathL2";
      tStrings[mpL5]    = "multipathL5";
      tStrings[mpL6]    = "multipathL6";
      tStrings[mpL7]    = "multipathL7";
      tStrings[mpL8]    = "multipathL8";
      tStrings[instC1]  = "instrumentalC1";
      tStrings[instC2]  = "instrumentalC2";
      tStrings[instC5]  = "instrumentalC5";
      tStrings[instC6]  = "instrumentalC6";
      tStrings[instC7]  = "instrumentalC7";
      tStrings[instC8]  = "instrumentalC8";
      tStrings[instL1]  = "instrumentalL1";
      tStrings[instL2]  = "instrumentalL2";
      tStrings[instL5]  = "instrumentalL5";
      tStrings[instL6]  = "instrumentalL6";
      tStrings[instL7]  = "instrumentalL7";
      tStrings[instL8]  = "instrumentalL8";
      tStrings[prefitC] = "prefitResidualCode";
      tStrings[prefitL] = "prefitResidualPhase";
      tStrings[postfitC] = "posfitResidualCode";
      tStrings[postfitL] = "posfitResidualPhase";
      tStrings[rhoX]    = "unitVectorX";
      tStrings[rhoY]    = "unitVectorY";
      tStrings[rhoZ]    = "unitVectorZ";
      tStrings[dX]      = "dX";
      tStrings[dY]      = "dY";
      tStrings[dZ]      = "dZ";
      tStrings[dt]      = "RxClockOffset";
      tStrings[dLat]    = "dLat";
      tStrings[dLon]    = "dLon";
      tStrings[dH]      = "dH";
      tStrings[weight]  = "weight";
      tStrings[recX]    = "RxPositionX";
      tStrings[recY]    = "RxPositionY";
      tStrings[recZ]    = "RxPositionZ";
      tStrings[recLat]  = "RxLat";
      tStrings[recLon]  = "RxLon";
      tStrings[recH]    = "RxH";
      tStrings[sigma]   = "sigma";
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

}
