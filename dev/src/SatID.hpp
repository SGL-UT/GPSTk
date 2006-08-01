#pragma ident "$Id$"



#ifndef GPSTK_SATID_HPP
#define GPSTK_SATID_HPP

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






#include <iostream>
#include <iomanip>

/**
 * @file SatID.hpp
 * gpstk::SatID - navigation system-independent representation of a satellite.
 */

namespace gpstk
{
      /// Satellite identifier = satellite number (PRN, etc.) and system
   class SatID
   {
   public:
         /// Supported satellite systems
      enum System
      {
         systemGPS = 1,
         systemGlonass,
         systemTransit,
         systemGeosync,
         systemMixed
      };

      SatID() { id=-1; system=systemGPS; }
      SatID(int p, System s) { id=p; system=s; }
      void setfill(char c) { fillchar=c; }
      char getfill() { return fillchar; }
         // operator=, copy constructor and destructor built by compiler
         /// return the single-character system descriptor
      char systemCode() const
      {
         switch(system) {
            case systemGPS: return 'G';
            case systemMixed: return 'M';
            case systemGlonass: return 'R';
            case systemTransit: return 'T';
            case systemGeosync: return 'S';
         }
         return 0;
      };

         /// operator == for SatID
      bool operator==(const SatID& right) const
      { return ((system == right.system) && (id == right.id)); }

         /// operator != for SatID
      bool operator!=(const SatID& right) const
      { return ((system != right.system) || (id != right.id)); }

         /// order by system, then number
      bool operator<(const SatID& right) const
      {
         if (system==right.system)
            return (id<right.id);
         return (system<right.system);
      }

         // the following allow you to use, respectively,
         // std::string gpstk::StringUtils::asString<SatID>(const SatID p)
         // SatID gpstk::StringUtils::asData<SatID>(const std::string& s)

      int id;                ///< satellite identifier.
      System system;         ///< system this satellite is part of.
      static char fillchar;  ///< fill character used during stream output
   }; // class SatID

      /// stream output for SatID
   inline std::ostream& operator<<(std::ostream& s, const SatID& p)
   {
      switch(p.system)
      {
         case SatID::systemGPS:
            s << "G";
            break;
         case SatID::systemMixed:
            s << "G";
            break; // this is an error ... assume GPS
         case SatID::systemGlonass:
            s << "R";
            break;
         case SatID::systemTransit:
            s << "T";
            break;
         case SatID::systemGeosync:
            s << "S";
            break;
      }
      s << std::setw(2) << std::setfill(p.fillchar) << p.id << std::setfill(' ');
      return s;
   }

      /// stream input for SatID
   inline std::istream& operator>>(std::istream& s, SatID& p)
   {
      char c;
      s.unsetf(std::ios_base::skipws);
      s >> c;
      switch(c)
      {
         case '0': case '1': case '2': case '3':
         case '4': case '5': case '6':
         case '7': case '8': case '9':
            s.putback(c);
            p.system = SatID::systemGPS;
            break;
         case 'R': case 'r':
            p.system = SatID::systemGlonass;
            break;
         case 'T': case 't':
            p.system = SatID::systemTransit;
            break;
         case 'S': case 's':
            p.system = SatID::systemGeosync;
            break;
         case 'G': case 'g': case ' ':
         default: // error
            p.system = SatID::systemGPS;
            break;
      }
      s.setf(std::ios_base::skipws);
      s >> p.id;
      return s;
   }

} // namespace gpstk

#endif
