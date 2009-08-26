#pragma ident "$Id$"

/**
 * @file TypeID.hpp
 * gpstk::TypeID - This class was written taking as inspiration ObsID. The
 * objective of this class is to create an index able to represent any type
 * of observation, correction, model parameter or other data value of interest
 * for GNSS data processing. This class is extensible in run-time, so the
 * programmer may add indexes on-demand.
 */

#ifndef GPSTK_TYPEID_HPP
#define GPSTK_TYPEID_HPP

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


#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>
#include "RinexObsHeader.hpp"


namespace gpstk
{

      /** This class creates an index able to represent any type of observation,
       *  correction, model parameter or other data value of interest for GNSS
       *  data processing.
       *
       * This class is extensible in run-time, so the programmer may add
       * indexes on-demand. For instance, in order to create a new TypeID
       * object referring INS-related data, and with "Inertial" as description
       * string, you may write the following:
       *
       * @code
       *    TypeID INS = TypeID::newValueType("Inertial");
       * @endcode
       *
       * Or using the constructor:
       *
       * @code
       *    TypeID INS(TypeID::newValueType("Inertial"));
       * @endcode
       *
       * From now on, you'll be able to use INS as TypeID when you need to
       * refer to inertial system data.
       *
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
         MWubbena,  ///< Melbourne-Wubbena combination
            // Model-related types
         rho,       ///< Geometric distance satellite-receiver
         rhodot,    ///< First derivative of geometric distance SV-RX
         rhodot2,   ///< Second derivative of geometric distance SV-RX
         dtSat,     ///< Satellite clock offset
         dtSatdot,  ///< Satellite clock offset drift
         dtSatdot2, ///< Satellite clock offset drift rate
         rel,       ///< Relativistic delay
         gravDelay, ///< Gravitational delay
         tropo,     ///< Vertical tropospheric delay, total
         dryTropo,  ///< Vertical tropospheric delay, dry component
         dryMap,    ///< Tropospheric mapping function, dry component
         wetTropo,  ///< Vertical tropospheric delay, wet component
         wetMap,    ///< Tropospheric mapping function, wet component
         tropoSlant, ///< Slant tropospheric delay, total
         iono,      ///< Vertical ionospheric delay
         ionoTEC,   ///< Total Electron Content (in TECU), 1TECU = 1e+16 electrons per m**2
         ionoMap,   ///< Ionospheric mapping function
         ionoL1,    ///< Slant ionospheric delay, frequency L1
         ionoL2,    ///< Slant ionospheric delay, frequency L2
         ionoL5,    ///< Slant ionospheric delay, frequency L5
         ionoL6,    ///< Slant ionospheric delay, frequency L6
         ionoL7,    ///< Slant ionospheric delay, frequency L7
         ionoL8,    ///< Slant ionospheric delay, frequency L8
         windUp,    ///< Wind-up effect (in radians)
         satPCenter,///< Satellite antenna phase center correction
         satX,      ///< Satellite position, X component
         satY,      ///< Satellite position, Y component
         satZ,      ///< Satellite position, Z component
         satVX,     ///< Satellite velocity, X component
         satVY,     ///< Satellite velocity, Y component
         satVZ,     ///< Satellite velocity, Z component
         satAX,     ///< Satellite acceleration, X component
         satAY,     ///< Satellite acceleration, Y component
         satAZ,     ///< Satellite acceleration, Z component
         elevation, ///< Satellite elevation
         azimuth,   ///< Satellite azimuth
            // Cycle slip flags
         CSL1,      ///< Cycle slip in L1
         CSL2,      ///< Cycle slip in L2
         CSL5,      ///< Cycle slip in L5
         CSL6,      ///< Cycle slip in L6
         CSL7,      ///< Cycle slip in L7
         CSL8,      ///< Cycle slip in L8
            // Satellite 'arcs'
         satArc,    ///< Satellite arc number
            // Phase-ambiguity types
         BL1,       ///< Phase ambiguity in L1
         BL2,       ///< Phase ambiguity in L2
         BL5,       ///< Phase ambiguity in L5
         BL6,       ///< Phase ambiguity in L6
         BL7,       ///< Phase ambiguity in L7
         BL8,       ///< Phase ambiguity in L8
         BLC,       ///< Phase ambiguity in LC
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
         prefitC,   ///< Prefit residual, code
         prefitL,   ///< Prefit residual, phase
         postfitC,  ///< Postfit residual, code
         postfitL,  ///< Postfit residual, phase
         dx,        ///< In the position domain: Position bias, X component; in the range domain: dx coefficient
         dy,        ///< In the position domain: Position bias, Y component; in the range domain: dy coefficient
         dz,        ///< In the position domain: Position bias, Z component; in the range domain: dz coefficient
         dLat,      ///< Position bias, Latitude component
         dLon,      ///< Position bias, Longitude component
         dH,        ///< Position bias, Height component
         cdt,       ///< In the position domain: Receiver clock offset, meters; in the range domain: cdt coefficient
         weight,    ///< Weight assigned to a given observation
            // Other types
         recX,      ///< Receiver position, X component
         recY,      ///< Receiver position, Y component
         recZ,      ///< Receiver position, Z component
         recVX,     ///< Receiver velocity, X component
         recVY,     ///< Receiver velocity, Y component
         recVZ,     ///< Receiver velocity, Z component
         recAX,     ///< Receiver acceleration, X component
         recAY,     ///< Receiver acceleration, Y component
         recAZ,     ///< Receiver acceleration, Z component
         recLat,    ///< Receiver position, Latitude component
         recLon,    ///< Receiver position, Longitude component
         recH,      ///< Receiver position, Height component
         recVLat,   ///< Receiver velocity, Latitude component
         recVLon,   ///< Receiver velocity, Longitude component
         recVH,     ///< Receiver velocity, Height component
         recALat,   ///< Receiver acceleration, Latitude component
         recALon,   ///< Receiver acceleration, Longitude component
         recAH,     ///< Receiver acceleration, Height component
         sigma,     ///< Standard deviation
         iura,      ///< Index User Range Accuracy
            // Handy dummy types for non-standard processing
         dummy0,    ///< Generic, undefined type #0
         dummy1,    ///< Generic, undefined type #1
         dummy2,    ///< Generic, undefined type #2
         dummy3,    ///< Generic, undefined type #3
         dummy4,    ///< Generic, undefined type #4
         dummy5,    ///< Generic, undefined type #5
         dummy6,    ///< Generic, undefined type #6
         dummy7,    ///< Generic, undefined type #7
         dummy8,    ///< Generic, undefined type #8
         dummy9,    ///< Generic, undefined type #9

         Last,      ///< used to extend this...
         Placeholder = Last+1000
      };


         /// empty constructor, creates an invalid object
      TypeID()
         : type(Unknown) {};


         /** Explicit constructor
          *
          * @param vt   ValueType for the new TypeID. If you want to use the
          *             next available ValueType, generate it using the
          *             'newValueType()' method, as indicated in the example in
          *             the documentation.
          */

      TypeID(ValueType vt)
         : type(vt) {};


         /// Equality requires all fields to be the same
      virtual bool operator==(const TypeID& right) const
      { return type==right.type; };


         /// This ordering is somewhat arbitrary but is required to be able
         /// to use an TypeID as an index to a std::map. If an application
         /// needs some other ordering, inherit and override this function.
      virtual bool operator<(const TypeID& right) const
      { return type < right.type; };


         /// Inequality operator
      bool operator!=(const TypeID& right) const
      { return !(operator==(right)); };


         /// Greater than operator
      bool operator>(const TypeID& right) const
      {  return (!operator<(right) && !operator==(right)); };


         /// Less than or equal operator
      bool operator<=(const TypeID& right) const
      { return (operator<(right) || operator==(right)); };


         /// Greater than or equal operator
      bool operator>=(const TypeID& right) const
      { return !(operator<(right)); };


         /// Assignment operator
      virtual TypeID operator=(const TypeID& right);


         /// Convenience output method
      virtual std::ostream& dump(std::ostream& s) const;


         /// Returns true if this is a valid TypeID. Basically just
         /// checks that the enum is defined
      virtual bool isValid() const;


         /// Destructor
      virtual ~TypeID() {};


         /** Static method to add new TypeID's
          * @param s      Identifying string for the new TypeID
          */
      static ValueType newValueType(const std::string& s);


         /// Type of the value
      ValueType type;


         /// Map holding type descriptions
      static std::map< ValueType, std::string > tStrings;


   public:
      class Initializer
      {
      public:
         Initializer();
      };

      static Initializer TypeIDsingleton;


   }; // End of class 'TypeID'



   namespace StringUtils
   {
         /// convert this object to a string representation
      std::string asString(const TypeID& p);
   }



      /// stream output for TypeID
   std::ostream& operator<<(std::ostream& s, const TypeID& p);



      /// Conversion from RinexObsType to TypeID
   TypeID::ValueType RinexType2TypeID(const RinexObsHeader::RinexObsType& rot);



}  // End of namespace gpstk

#endif   // GPSTK_TYPEID_HPP
