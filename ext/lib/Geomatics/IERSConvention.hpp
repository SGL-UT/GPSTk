//============================================================================
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

/// @file IERSConvention.hpp
/// Include file defining the IERSConvention class.

//------------------------------------------------------------------------------------
#ifndef CLASS_IERSCONVENTION_INCLUDE
#define CLASS_IERSCONVENTION_INCLUDE

#include <iostream>
#include <string>
#include "Exception.hpp"

//------------------------------------------------------------------------------------
namespace gpstk
{

   /// This class encapsulates the choice of IERS Convention, which applies directly
   /// to the operation of class EarthOrientation, and is used in class SolarSystem
   /// and in the functions defined in SolidEarthTides.cpp.
   /// The IERS convention determines the precise form of frame transformations
   /// between the conventional terrestrial frame and the conventional inertial frame,
   /// as well as the solid earth tides.
   /// References:
   /// IERS1996: IERS Technical Note 21, "IERS Conventions (1996),"
   ///   Dennis D. McCarthy, U.S. Naval Observatory, 1996.
   /// IERS2003: IERS Technical Note 32, "IERS Conventions (2003),"
   ///   Dennis D. McCarthy and Gerard Petit eds., U.S. Naval Observatory and
   ///   Bureau International des Poids et Mesures, 2004.
   /// IERS2010: IERS Technical Note 36, "IERS Conventions (2010),"
   ///   Gerard Petit and Brian Luzum eds., Bureau International des Poids et Mesures
   ///   and U.S. Naval Observatory, 2010.
   class IERSConvention
   {
   public:
      //------------------------------------------------------------------------------
      // class IERSConvention is really just a 'smart enum', that is an enum with
      // standard operators (c'tor, boolean logic, string I/O).

      /// List of IERS conventions that are implemented in this class.
      enum Convention
      {
         NONE = 0,      // 0 MUST be first
         IERS1996,
         IERS2003,
         IERS2010,
         count          // the number of conventions; this must be last
      };

      /// Constructor, including empty constructor
      IERSConvention(Convention conv = IERS1996) throw()
         { setConvention(conv); }

      /// constructor from int
      IERSConvention(int i) throw()
      {
         if(i <= 0 || i >= count)
            convention = NONE;
         else
            convention = static_cast<Convention>(i);
      }

      // copy constructor and operator= are implemented by compiler

      /// choose an IERS Convention
      void setConvention(const Convention& conv) throw()
      {
         if(conv < 0 || conv >= count) convention = NONE;
         else                          convention = conv;
      }

      /// get the IERS Convention
      Convention getConvention(void) const throw ()
         { return convention; }

      /// get a std::string for each convention
      /// @return descriptive std::string
      std::string asString(void) const throw()
         { return Strings[convention]; }

      /// define convention based on input string
      /// @param str input string, must match output string for given convention
      void fromString(const std::string str) throw()
      {
         convention = NONE;
         for(int i=0; i<count; i++) {
            if(Strings[i] == str) {
               convention = static_cast<Convention>(i);
               break;
            }
         }
      }

      /// boolean operator==
      bool operator==(const IERSConvention& right) const throw()
         { return convention == right.convention; }

      /// boolean operator< (used by STL to sort)
      bool operator<(const IERSConvention& right) const throw()
         { return convention < right.convention; }

      // the rest follow from Boolean algebra...
      /// boolean operator!=
      bool operator!=(const IERSConvention& right) const throw()
         { return !operator==(right); }

      /// boolean operator>=
      bool operator>=(const IERSConvention& right) const throw()
         { return !operator<(right); }

      /// boolean operator<=
      bool operator<=(const IERSConvention& right) const throw()
         { return (operator<(right) || operator==(right)); }

      /// boolean operator>
      bool operator>(const IERSConvention& right) const throw()
         { return (!operator<(right) && !operator==(right)); }

      //------------------------------------------------------------------------------
   private:
      // member data

      /// convention (= element of Convention enum) for this object
      Convention convention;

      /// set of string labels for elements of enum Convention
      static const std::string Strings[];

   }; // end class IERSConvention

   /// Write name (asString()) of a Convention to an output stream.
   /// @param os The output stream
   /// @param cv The Convention to be written
   /// @return reference to the output stream
   std::ostream& operator<<(std::ostream os, const IERSConvention::Convention& cv);

} // end namespace gpstk

#endif  // define CLASS_IERSCONVENTION_INCLUDE
