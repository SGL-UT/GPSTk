#pragma ident "$Id$"
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

#ifndef GPSTK_TYPEID_HPP
#define GPSTK_TYPEID_HPP

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>
#include "RinexObsHeader.hpp"


/**
 * @file TypeID.hpp
 * gpstk::TypeID - This class was written taking as inspiration ObsID. The
   objective of this class is to create an index able to represent any type 
   of observation, correction, model parameter or other data value of interest 
   for GNSS data processing. This class is extensible in run-time, so the 
   programmer may add indexes on-demand.
 */

namespace gpstk
{

   /**
    * This class creates an index able to represent any type of observation,
    * correction, model parameter or other data value of interest for GNSS 
    * data processing. 
    */   

   class TypeID
   {
   public:
      /// The type of the data value.
      enum ValueType
      {
         Unknown,
         // Observation-related types
         C1,        ///< GPS civil code observation in L1 frequency
         C2,        ///< GPS civil code observation in L2 frequency
         P1,        ///< GPS precise code observation in L1 frequency
         P2,        ///< GPS precise code observation in L2 frequency
         L1,        ///< GPS phase observation in L1 frequency
         L2,        ///< GPS phase observation in L2 frequency
         D1,        ///< GPS doppler observation in L1 frequency
         D2,        ///< GPS doppler observation in L2 frequency
         S1,        ///< GPS signal strength observation in L1 frequency
         S2,        ///< GPS signal strength observation in L2 frequency
         T1,        ///< Transit integrated doppler observation in L1 frequency
         T2,        ///< Transit integrated doppler observation in L2 frequency
         SSI1,      ///< Signal strength indicator/index, L1 frequency
         LLI1,      ///< Loss of Lock Indicator/ lock count, L1 frequency
         SSI2,      ///< Signal strength indicator/index, L2 frequency
         LLI2,      ///< Loss of Lock Indicator/ lock count, L2 frequency
         // v 2.11
         C5,        ///< GPS L5C-code pseudorange
         L5,        ///< GPS phase observation in L5 frequency
         D5,        ///< GPS doppler observation in L5 frequency
         S5,        ///< GPS signal strength observation in L5 frequency
         SSI5,      ///< Signal strength indicator/index, L5 frequency
         LLI5,      ///< Loss of Lock Indicator/ lock count, L5 frequency
         // Galileo-related
         C6,        ///< Galileo E6-code pseudorange
         L6,        ///< Galileo phase observation in L6 frequency
         D6,        ///< Galileo doppler observation in L6 frequency
         S6,        ///< Galileo signal strength observation in L6 frequency
         SSI6,      ///< Signal strength indicator/index, L6 frequency
         LLI6,      ///< Loss of Lock Indicator/ lock count, L6 frequency
         C7,        ///< Galileo E5b-code pseudorange
         L7,        ///< Galileo phase observation in L5b frequency
         D7,        ///< Galileo doppler observation in L5b frequency
         S7,        ///< Galileo signal strength observation in L5b frequency
         SSI7,      ///< Signal strength indicator/index, L5b frequency
         LLI7,      ///< Loss of Lock Indicator/ lock count, L5b frequency
         C8,        ///< Galileo E5a+b-code pseudorange
         L8,        ///< Galileo phase observation in L5a+b frequency
         D8,        ///< Galileo doppler observation in L5a+b frequency
         S8,        ///< Galileo signal strength observation in L5a+b frequency
         SSI8,      ///< Signal strength indicator/index, L5a+b frequency
         LLI8,      ///< Loss of Lock Indicator/ lock count, L5a+b frequency
         // Combination-related types
         PC,        ///< Code-based ionosphere-free combination
         LC,        ///< Phase-based ionosphere-free combination
         PI,        ///< Code-based ionospheric combination
         LI,        ///< Phase-based ionospheric combination
         Pdelta,    ///< Narrow-lane combination
         Ldelta,    ///< Wide-lane combination
         // Model-related types
         rho,       ///< Geometric distance satellite-receiver
         dtSat,     ///< Satellite clock offset
         rel,       ///< Relativistic delay
         tropo,     ///< Vertical tropospheric delay, total
         dryTropo,  ///< Vertical tropospheric delay, dry component
         wetTropo,  ///< Vertical tropospheric delay, wet component
         tropoSlant, ///< Slant tropospheric delay, total
         iono,      ///< Vertical ionospheric delay
         ionoSlant, ///< Slant ionospheric delay
         windUp,    ///< Wind-up effect (in radians)
         satX,      ///< Satellite position, X component
         satY,      ///< Satellite position, Y component
         satZ,      ///< Satellite position, Z component
         elevation, ///< Satellite elevation
         azimuth,   ///< Satellite azimuth
         // Phase-ambiguity types
         BL1,       ///< Phase ambiguity in L1
         BL2,       ///< Phase ambiguity in L2
         BL5,       ///< Phase ambiguity in L5
         BL6,       ///< Phase ambiguity in L6
         BL7,       ///< Phase ambiguity in L7
         BL8,       ///< Phase ambiguity in L8
         // Multipath-related types
         mpC1,      ///< Multipath bias, C1
         mpC2,      ///< Multipath bias, C2
         mpC5,      ///< Multipath bias, C5
         mpC6,      ///< Multipath bias, C6
         mpC7,      ///< Multipath bias, C7
         mpC8,      ///< Multipath bias, C8
         mpL1,      ///< Multipath bias, L1
         mpL2,      ///< Multipath bias, L2
         mpL5,      ///< Multipath bias, L5
         mpL6,      ///< Multipath bias, L6
         mpL7,      ///< Multipath bias, L7
         mpL8,      ///< Multipath bias, L8
         // Instrumental delays types
         instC1,    ///< Instrumental delay, C1
         instC2,    ///< Instrumental delay, C2
         instC5,    ///< Instrumental delay, C5
         instC6,    ///< Instrumental delay, C6
         instC7,    ///< Instrumental delay, C7
         instC8,    ///< Instrumental delay, C8
         instL1,    ///< Instrumental delay, L1
         instL2,    ///< Instrumental delay, L2
         instL5,    ///< Instrumental delay, L5
         instL6,    ///< Instrumental delay, L6
         instL7,    ///< Instrumental delay, L7
         instL8,    ///< Instrumental delay, L8
         // Equation system-related types
         prefit,    ///< Prefit residual
         postfit,   ///< Postfit residual
         rhoX,      ///< Unit vector from satellite to receiver, X component
         rhoY,      ///< Unit vector from satellite to receiver, Y component
         rhoZ,      ///< Unit vector from satellite to receiver, Z component
         dX,        ///< Position bias, X component
         dY,        ///< Position bias, Y component
         dZ,        ///< Position bias, Z component
         dt,        ///< Receiver clock offset
         dLat,      ///< Position bias, Latitude component
         dLon,      ///< Position bias, Longitude component
         dH,        ///< Position bias, Height component
         weight,    ///< Weight assigned to a given observation
         // Other types
         sigma,     ///< Standard deviation
         Last,      ///< used to extend this...
         Placeholder = Last+1000
      };

      /// empty constructor, creates an invalid object
      TypeID()
         : type(Unknown) {};

      /// Explicit constructor
      TypeID(ValueType vt)
         : type(vt) {};

      /// Equality requires all fields to be the same
      virtual bool operator==(const TypeID& right) const
      { return type==right.type; }

      /// This ordering is somewhat arbitrary but is required to be able
      /// to use an TypeID as an index to a std::map. If an application needs
      /// some other ordering, inherit and override this function.
      virtual bool operator<(const TypeID& right) const
      { return type < right.type; }


      bool operator!=(const TypeID& right) const
      { return !(operator==(right)); }

      bool operator>(const TypeID& right) const
      {  return (!operator<(right) && !operator==(right)); }

      bool operator<=(const TypeID& right) const
      { return (operator<(right) || operator==(right)); }

      bool operator>=(const TypeID& right) const
      { return !(operator<(right)); }

      /// Assignment operator
      virtual TypeID operator=(const TypeID& right)
      {
        if ( this == &right ) return (*this);
        (*this).type = right.type;
        return *this;
      }

      /// Convenience output method
      virtual std::ostream& dump(std::ostream& s) const;

      /// Returns true if this is a valid TypeID. Basically just
      /// checks that the enum is defined
      virtual bool isValid() const;

      /// Destructor
      virtual ~TypeID() {}


      static ValueType newValueType(const std::string& s);

      // Field
      /// Type of the value
      ValueType  type;

      static std::map< ValueType, std::string > tStrings;

   public:
      class Initializer
      {
      public:
         Initializer();
      };

      static Initializer TypeIDsingleton;

   }; // class TypeID

   namespace StringUtils
   {
      /// convert this object to a string representation
      std::string asString(const TypeID& p);
   }
   
   /// stream output for TypeID
   std::ostream& operator<<(std::ostream& s, const TypeID& p);


   /// Conversion from RinexObsType to TypeID
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



} // namespace gpstk
#endif
