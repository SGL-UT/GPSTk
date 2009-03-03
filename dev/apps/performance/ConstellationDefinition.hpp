#pragma ident "$Id:$"
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
/**
*   @file ConstellationDefinition.hpp
*
*   Applied Research Laboratories, The University of Texas at Austin
*   July 2007
*
*   Each ConstellationDefinition object holds information on the 
*   state of the GPS Constellation for a given date.
*   The relationship of PRN IDs to slots (and excess slots) is available
*   through this class.
*
*     Planes : A, B, C, D, E, F
*     Slots  : 1-4 are the primary as defined in SPS PS
*              5-n are extra
*
*   MEMBER METHODS
*     DayTime getDate() - Return date associated with this method.
*                              (Time of day will be set to 1200Z to avoid
*                               ambiguity.)
*     bool inBase24( SatID SV ) -
*     string getSlotDef( SatID SV ) -
*     string getPlaneOnly( SatID SV ) - 
*     string getSlotOnly( SatID SV ) 
*     bool setPlaneSlot( SatID, char plane, int slot )
*     bool setPlaneSlot( SatID, SlotDef sd )
*     void clearDefinition( )
*     int getTotalNumsSVs() 
*     int getTotalNumsInBase24()
*     void dump( FILE * ) 
*     void dump( stream.????)
*
*/
#ifndef CONSTELLATION_DEFINITION_HPP
#define CONSTELLATION_DEFINITION_HPP

   // Language Headers
#include <stdio.h>
#include <map>
#include <iostream>
#include <iomanip>

   // Library Headers
#include "Exception.hpp"
#include "DayTime.hpp"
#include "SatID.hpp"

   // Project Headers

namespace gpstk
{
class SlotDef
{
   public:
      std::string planeSlot;  ///< 2-char plane and slot identifier
      char plane;             ///< 1-char plane ID (A-F)
      int slot;               ///< Slot within plane
      
      SlotDef() : planeSlot(std::string("UN")),plane(' '),slot(0) {}
      SlotDef(std::string ps, char c, int i ) :
         planeSlot(ps),plane(c),slot(i) {}
         
      bool operator==(const SlotDef& right) const
      { return ((plane==right.plane) && (slot==right.slot)); }

      bool operator!=(const SlotDef& right) const
      { return !(operator==(right)); }

      /// operator < for SlotDef : order by plane, then slot
      bool operator<(const SlotDef& right) const
      {
         if (plane==right.plane) return (slot<right.slot);
         return (plane<right.plane);
      }

      bool operator>(const SlotDef& right) const
      {  return (!operator<(right) && !operator==(right)); }

      bool operator<=(const SlotDef& right) const
      { return (operator<(right) || operator==(right)); }

      bool operator>=(const SlotDef& right) const
      { return !(operator<(right)); }
};
   
class ConstellationDefinition
{
   public:
      NEW_EXCEPTION_CLASS(NoSVFoundInSlot, Exception );
      NEW_EXCEPTION_CLASS(NoSlotFoundForSV, Exception );
      
      ConstellationDefinition( );
      ~ConstellationDefinition() {}
      gpstk::DayTime getDate() const;
      bool inBase24( const SatID SV ) const;
      SlotDef getSlotDef( const SatID SV ) const;
      std::string getSlotString( const SatID SV ) const;
      char getPlaneOnly( const SatID SV ) const;
      int getSlotOnly( const SatID SV ) const;
      int getTotalNumSVs( ) const;
      int getTotalNumInBase24( ) const;
      int getSVN( const SatID SV ) const;
      
      void setPlaneSlot( const SatID SV, const char plane, const int slot ); 
      void setPlaneSlot( const SatID SV, const SlotDef sd );
      void setSVNforPRN( const SatID SV, const int SVN );
      void setEffectiveTime( const gpstk::DayTime dt );
      void clearDefinition( );
      
      void dump( FILE * ) const;
      //void dump( stream.????) const;

   protected:
      gpstk::DayTime effectiveDate;
      std::map<SlotDef,SatID> SlotsToSVs;
      std::map<SatID,SlotDef> SVsToSlots;
      std::map<SatID,int> PRNtoSVN;       // Only used it input type is CSV

      SlotDef findSlot( SatID SV ) const 
         throw(NoSlotFoundForSV);
      gpstk::SatID findSV( SlotDef sd ) const 
         throw(NoSVFoundInSlot);      
};

inline int ConstellationDefinition::getTotalNumSVs( ) const 
           { return(SlotsToSVs.size()); }
           
inline gpstk::DayTime ConstellationDefinition::getDate( ) const
           { return( effectiveDate ); }
}
#endif
