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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009
//
//============================================================================


#include "TypeID.hpp"


namespace gpstk
{

   std::map< TypeID::ValueType, std::string > TypeID::tStrings;


   TypeID::Initializer TypeIDsingleton;

      // It should be initialize by false, NEVER CHANGE IT!!!
   bool TypeID::bUserTypeIDRegistered = false;

      // Map holding user defined TypeIDs by a string
   std::map<std::string,TypeID> TypeID::mapUserTypeID;


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
      tStrings[GRAPHIC1]   = "GRAPHIC1";
      tStrings[GRAPHIC2]   = "GRAPHIC2";
      tStrings[GRAPHIC5]   = "GRAPHIC5";
      tStrings[GRAPHIC6]   = "GRAPHIC6";
      tStrings[GRAPHIC7]   = "GRAPHIC7";
      tStrings[GRAPHIC8]   = "GRAPHIC8";
      tStrings[WL]         = "WL";
      tStrings[WL1]        = "WL1";
      tStrings[WL2]        = "WL2";
      tStrings[WL3]        = "WL3";
      tStrings[WL4]        = "WL4";
      tStrings[EWL]        = "EWL";
      tStrings[C1A]        = "C1A";
      tStrings[L1A]        = "L1A";
      tStrings[D1A]        = "D1A";
      tStrings[S1A]        = "S1A";
      tStrings[C1B]        = "C1B";
      tStrings[L1B]        = "L1B";
      tStrings[D1B]        = "D1B";
      tStrings[S1B]        = "S1B";
      tStrings[C1C]        = "C1C";
      tStrings[L1C]        = "L1C";
      tStrings[D1C]        = "D1C";
      tStrings[S1C]        = "S1C";
      tStrings[C1L]        = "C1L";
      tStrings[L1L]        = "L1L";
      tStrings[D1L]        = "D1L";
      tStrings[S1L]        = "S1L";
      tStrings[C1M]        = "C1M";
      tStrings[L1M]        = "L1M";
      tStrings[D1M]        = "D1M";
      tStrings[S1M]        = "S1M";
      tStrings[L1N]        = "L1N";
      tStrings[D1N]        = "D1N";
      tStrings[S1N]        = "S1N";
      tStrings[C1P]        = "C1P";
      tStrings[L1P]        = "L1P";
      tStrings[D1P]        = "D1P";
      tStrings[S1P]        = "S1P";
      tStrings[C1S]        = "C1S";
      tStrings[L1S]        = "L1S";
      tStrings[D1S]        = "D1S";
      tStrings[S1S]        = "S1S";
      tStrings[C1W]        = "C1W";
      tStrings[L1W]        = "L1W";
      tStrings[D1W]        = "D1W";
      tStrings[S1W]        = "S1W";
      tStrings[C1X]        = "C1X";
      tStrings[L1X]        = "L1X";
      tStrings[D1X]        = "D1X";
      tStrings[S1X]        = "S1X";
      tStrings[C1Y]        = "C1Y";
      tStrings[L1Y]        = "L1Y";
      tStrings[D1Y]        = "D1Y";
      tStrings[S1Y]        = "S1Y";
      tStrings[C1Z]        = "C1Z";
      tStrings[L1Z]        = "L1Z";
      tStrings[D1Z]        = "D1Z";
      tStrings[S1Z]        = "S1Z";
      tStrings[C2C]        = "C2C";
      tStrings[L2C]        = "L2C";
      tStrings[D2C]        = "D2C";
      tStrings[S2C]        = "S2C";
      tStrings[C2D]        = "C2D";
      tStrings[L2D]        = "L2D";
      tStrings[D2D]        = "D2D";
      tStrings[S2D]        = "S2D";
      tStrings[C2I]        = "C2I";
      tStrings[L2I]        = "L2I";
      tStrings[D2I]        = "D2I";
      tStrings[S2I]        = "S2I";
      tStrings[C2L]        = "C2L";
      tStrings[L2L]        = "L2L";
      tStrings[D2L]        = "D2L";
      tStrings[S2L]        = "S2L";
      tStrings[C2M]        = "C2M";
      tStrings[L2M]        = "L2M";
      tStrings[D2M]        = "D2M";
      tStrings[S2M]        = "S2M";
      tStrings[L2N]        = "L2N";
      tStrings[D2N]        = "D2N";
      tStrings[S2N]        = "S2N";
      tStrings[C2P]        = "C2P";
      tStrings[L2P]        = "L2P";
      tStrings[D2P]        = "D2P";
      tStrings[S2P]        = "S2P";
      tStrings[C2Q]        = "C2Q";
      tStrings[L2Q]        = "L2Q";
      tStrings[D2Q]        = "D2Q";
      tStrings[S2Q]        = "S2Q";
      tStrings[C2S]        = "C2S";
      tStrings[L2S]        = "L2S";
      tStrings[D2S]        = "D2S";
      tStrings[S2S]        = "S2S";
      tStrings[C2W]        = "C2W";
      tStrings[L2W]        = "L2W";
      tStrings[D2W]        = "D2W";
      tStrings[S2W]        = "S2W";
      tStrings[C2X]        = "C2X";
      tStrings[L2X]        = "L2X";
      tStrings[D2X]        = "D2X";
      tStrings[S2X]        = "S2X";
      tStrings[C2Y]        = "C2Y";
      tStrings[L2Y]        = "L2Y";
      tStrings[D2Y]        = "D2Y";
      tStrings[S2Y]        = "S2Y";
      tStrings[C2Y]        = "C2Y";
      tStrings[L2Y]        = "L2Y";
      tStrings[D2Y]        = "D2Y";
      tStrings[S2Y]        = "S2Y";
      tStrings[C5I]        = "C5I";
      tStrings[L5I]        = "L5I";
      tStrings[D5I]        = "D5I";
      tStrings[S5I]        = "S5I";
      tStrings[C5Q]        = "C5Q";
      tStrings[L5Q]        = "L5Q";
      tStrings[D5Q]        = "D5Q";
      tStrings[S5Q]        = "S5Q";
      tStrings[C5X]        = "C5X";
      tStrings[L5X]        = "L5X";
      tStrings[D5X]        = "D5X";
      tStrings[S5X]        = "S5X";
      tStrings[C6A]        = "C6A";
      tStrings[L6A]        = "L6A";
      tStrings[D6A]        = "D6A";
      tStrings[S6A]        = "S6A";
      tStrings[C6B]        = "C6B";
      tStrings[L6B]        = "L6B";
      tStrings[D6B]        = "D6B";
      tStrings[S6B]        = "S6B";
      tStrings[C6C]        = "C6C";
      tStrings[L6C]        = "L6C";
      tStrings[D6C]        = "D6C";
      tStrings[S6C]        = "S6C";
      tStrings[C6I]        = "C6I";
      tStrings[L6I]        = "L6I";
      tStrings[D6I]        = "D6I";
      tStrings[S6I]        = "S6I";
      tStrings[C6Q]        = "C6Q";
      tStrings[L6Q]        = "L6Q";
      tStrings[D6Q]        = "D6Q";
      tStrings[S6Q]        = "S6Q";
      tStrings[C6X]        = "C6X";
      tStrings[L6X]        = "L6X";
      tStrings[D6X]        = "D6X";
      tStrings[S6X]        = "S6X";
      tStrings[C6Z]        = "C6Z";
      tStrings[L6Z]        = "L6Z";
      tStrings[D6Z]        = "D6Z";
      tStrings[S6Z]        = "S6Z";
      tStrings[C7I]        = "C7I";
      tStrings[L7I]        = "L7I";
      tStrings[D7I]        = "D7I";
      tStrings[S7I]        = "S7I";
      tStrings[C7Q]        = "C7Q";
      tStrings[L7Q]        = "L7Q";
      tStrings[D7Q]        = "D7Q";
      tStrings[S7Q]        = "S7Q";
      tStrings[C7X]        = "C7X";
      tStrings[L7X]        = "L7X";
      tStrings[D7X]        = "D7X";
      tStrings[S7X]        = "S7X";
      tStrings[C8I]        = "C8I";
      tStrings[L8I]        = "L8I";
      tStrings[D8I]        = "D8I";
      tStrings[S8I]        = "S8I";
      tStrings[C8Q]        = "C8Q";
      tStrings[L8Q]        = "L8Q";
      tStrings[D8Q]        = "D8Q";
      tStrings[S8Q]        = "S8Q";
      tStrings[C8X]        = "C8X";
      tStrings[L8X]        = "L8X";
      tStrings[D8X]        = "D8X";
      tStrings[S8X]        = "S8X";
      tStrings[L1dot]      = "L1dot";
      tStrings[L1dot2]     = "L1dot2";
      tStrings[L2dot]      = "L2dot";
      tStrings[L2dot2]     = "L2dot2";
      tStrings[L5dot]      = "L5dot";
      tStrings[L5dot2]     = "L5dot2";
      tStrings[P1dot]      = "P1dot";
      tStrings[P1dot2]     = "P1dot2";
      tStrings[P2dot]      = "P2dot";
      tStrings[P2dot2]     = "P2dot2";
      tStrings[P5dot]      = "P5dot";
      tStrings[P5dot2]     = "P5dot2";
      tStrings[L6dot]      = "L6dot";
      tStrings[L6dot2]     = "L6dot2";
      tStrings[L7dot]      = "L7dot";
      tStrings[L7dot2]     = "L7dot2";
      tStrings[L8dot]      = "L8dot";
      tStrings[L8dot2]     = "L8dot2";
      tStrings[LCdot]      = "LCdot";
      tStrings[LCdot2]     = "LCdot2";
      tStrings[LIdot]      = "LIdot";
      tStrings[LIdot2]     = "LIdot2";
      tStrings[Ldeltadot]  = "Ldeltadot";
      tStrings[Ldeltadot2] = "Ldeltadot2";
      tStrings[rho]        = "rho";
      tStrings[rhodot]     = "rhodot";
      tStrings[rhodot2]    = "rhodot2";
      tStrings[dtSat]      = "dtSat";
      tStrings[dtSatdot]   = "dtSatdot";
      tStrings[dtSatdot2]  = "dtSatdot2";
      tStrings[rel]        = "rel";
      tStrings[gravDelay]  = "gravDelay";
      tStrings[tropo]      = "tropo";
      tStrings[dryTropo]   = "dryTropo";
      tStrings[dryMap]     = "dryTropoMap";
      tStrings[wetTropo]   = "wetTropo";
      tStrings[wetMap]     = "wetTropoMap";
      tStrings[tropoSlant] = "slantTropo";
      tStrings[iono]       = "verticalIono";
      tStrings[ionoTEC]    = "TotalElectronContent";
      tStrings[ionoMap]    = "ionoMap";
      tStrings[ionoL1]     = "slantIonoL1";
      tStrings[ionoL2]     = "slantIonoL2";
      tStrings[ionoL5]     = "slantIonoL5";
      tStrings[ionoL6]     = "slantIonoL6";
      tStrings[ionoL7]     = "slantIonoL7";
      tStrings[ionoL8]     = "slantIonoL8";
      tStrings[windUp]     = "windup";
      tStrings[satPCenter] = "satPhaseCenter";
      tStrings[satX]       = "satX";
      tStrings[satY]       = "satY";
      tStrings[satZ]       = "satZ";
      tStrings[satVX]      = "satVX";
      tStrings[satVY]      = "satVY";
      tStrings[satVZ]      = "satVZ";
      tStrings[satAX]      = "satAX";
      tStrings[satAY]      = "satAY";
      tStrings[satAZ]      = "satAZ";
      tStrings[satJ2kX]    = "satJ2kX";
      tStrings[satJ2kY]    = "satJ2kY";
      tStrings[satJ2kZ]    = "satJ2kZ";
      tStrings[satJ2kVX]   = "satJ2kVX";
      tStrings[satJ2kVY]   = "satJ2kVY";
      tStrings[satJ2kVZ]   = "satJ2kVZ";
      tStrings[satJ2kAX]   = "satJ2kAX";
      tStrings[satJ2kAY]   = "satJ2kAY";
      tStrings[satJ2kAZ]   = "satJ2kAZ";
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
      tStrings[BLC]        = "ambiguityLC";
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

      tStrings[prefitP1]   = "prefitResidualCodeP1";
      tStrings[prefitP2]   = "prefitResidualCodeP2";
      tStrings[prefitL1]   = "prefitResidualPhaseL1";
      tStrings[prefitL2]   = "prefitResidualPhaseL2";
      tStrings[postfitP1]  = "postfitResidualCodeP1";
      tStrings[postfitP2]  = "postfitResidualCodeP2";
      tStrings[postfitL1]  = "postfitResidualPhaseL1";
      tStrings[postfitL2]  = "postfitResidualPhaseL2";

      tStrings[prefitC5]   = "prefitResidualCodeC5";
      tStrings[prefitL5]   = "prefitResidualPhaseL5";
      tStrings[postfitC5]  = "postfitResidualCodeC5";
      tStrings[postfitL5]  = "postfitResidualPhaseL5";

      tStrings[prefitGRAPHIC1]  = "prefitResidualGRAPHIC1";
      tStrings[prefitGRAPHIC2]  = "prefitResidualGRAPHIC2";
      tStrings[postfitGRAPHIC1] = "postfitResidualGRAPHIC1";
      tStrings[postfitGRAPHIC2] = "postfitResidualGRAPHIC2";
      tStrings[prefitWL]   = "prefitResidualWL";
      tStrings[prefitWL2]  = "prefitResidualWL2";
      tStrings[prefitWL4]  = "prefitResidualWL4";
      tStrings[postfitWL]  = "postfitResidualWL";
      tStrings[postfitWL2] = "postfitResidualWL2";
      tStrings[postfitWL4] = "postfitResidualWL4";
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
      tStrings[codeBias]  = "codeBias";
      tStrings[recX]       = "RxPositionX";
      tStrings[recY]       = "RxPositionY";
      tStrings[recZ]       = "RxPositionZ";
      tStrings[recVX]      = "RxVelocityX";
      tStrings[recVY]      = "RxVelocityY";
      tStrings[recVZ]      = "RxVelocityZ";
      tStrings[recAX]      = "RxAccelerationX";
      tStrings[recAY]      = "RxAccelerationY";
      tStrings[recAZ]      = "RxAccelerationZ";
      tStrings[recLat]     = "RxLat";
      tStrings[recLon]     = "RxLon";
      tStrings[recH]       = "RxH";
      tStrings[recVLat]    = "RxVelocityLat";
      tStrings[recVLon]    = "RxVelocityLon";
      tStrings[recVH]      = "RxVelocityH";
      tStrings[recALat]    = "RxAccelerationLat";
      tStrings[recALon]    = "RxAccelerationLon";
      tStrings[recAH]      = "RxAccelerationH";
      tStrings[recJ2kX]       = "RxJ2kPositionX";
      tStrings[recJ2kY]       = "RxJ2kPositionY";
      tStrings[recJ2kZ]       = "RxJ2kPositionZ";
      tStrings[recJ2kVX]      = "RxJ2kVelocityX";
      tStrings[recJ2kVY]      = "RxJ2kVelocityY";
      tStrings[recJ2kVZ]      = "RxJ2kVelocityZ";
      tStrings[recJ2kAX]      = "RxJ2kAccelerationX";
      tStrings[recJ2kAY]      = "RxJ2kAccelerationY";
      tStrings[recJ2kAZ]      = "RxJ2kAccelerationZ";
      tStrings[sigma]      = "sigma";
      tStrings[iura]       = "iura";
      tStrings[dummy0]     = "dummy0";
      tStrings[dummy1]     = "dummy1";
      tStrings[dummy2]     = "dummy2";
      tStrings[dummy3]     = "dummy3";
      tStrings[dummy4]     = "dummy4";
      tStrings[dummy5]     = "dummy5";
      tStrings[dummy6]     = "dummy6";
      tStrings[dummy7]     = "dummy7";
      tStrings[dummy8]     = "dummy8";
      tStrings[dummy9]     = "dummy9";
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


      /// Static method to register new TypeID by a RegTypeID class
   void TypeID::regTypeIDs(RegTypeID* pReg, bool bAdd)
   {
      //
      if(!bAdd && bUserTypeIDRegistered)
      {
         unregAll();
      }
         
      pReg->regAll();

      bUserTypeIDRegistered = true;

   }

      // Return the new TypeID
   TypeID TypeID::regByName(std::string name,std::string desc)
   {

      std::map<std::string,TypeID>::iterator it = mapUserTypeID.find(name);

      if(it != mapUserTypeID.end())
      {
         return it->second;
      }
      else
      {
         TypeID newID = TypeID::newValueType(desc);

         mapUserTypeID.insert(std::pair<std::string,TypeID>(name, newID));

         return newID;
      }

   }  // End of 'TypeID::registerTypeID(std::string name,std::string desc)'



      // unregister a TypeID by it's name string
   void TypeID::unregByName(std::string name)
   {
      std::map<std::string,TypeID>::iterator it = mapUserTypeID.find(name);

      if(it!=mapUserTypeID.end())
      {
         TypeID delID = it->second;

         std::map<TypeID::ValueType,std::string>::iterator it2 = TypeID::tStrings.find(delID.type);
         if(it2!=TypeID::tStrings.end())
         {
            TypeID::tStrings.erase(it2);
         }

         mapUserTypeID.erase(it);
      }
      else
      {
         // the TypeID have not been registered
         // we do nothing
      }

   } // End of 'TypeID::unregisterTypeID(std::string name)'



      // unregister all TypeIDs registered by name string
   void TypeID::unregAll()
   {
      std::map<std::string,TypeID>::iterator it = mapUserTypeID.begin();

      for(it=mapUserTypeID.begin(); it!=mapUserTypeID.end(); it++)
      {
         TypeID delID = it->second;

         std::map<TypeID::ValueType,std::string>::iterator it2 = TypeID::tStrings.find(delID.type);
         if(it2!=TypeID::tStrings.end())
         {
            TypeID::tStrings.erase(it2);
         }

         mapUserTypeID.erase(it);
      }
      mapUserTypeID.clear();

      bUserTypeIDRegistered = false;

   }  // End of 'TypeID::unregisterAll()'

      // get the user registered TypeID by name string
   TypeID TypeID::byName(std::string name)
      throw(InvalidRequest)
   {
      // registerMyTypeID();

      std::map<std::string,TypeID>::iterator it = mapUserTypeID.find(name);
      if(it != mapUserTypeID.end())
      {
         return it->second;
      }
      else
      {
         InvalidRequest e("There are no registered TypeID name as '" 
            + name + "'.");
         GPSTK_THROW(e);
      }
   } // End of 'TypeID TypeID::byName(std::string name)'

} // End of namespace gpstk


