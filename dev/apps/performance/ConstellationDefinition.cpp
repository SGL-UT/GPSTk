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
*   @file ConstellationDefinition.cpp
*  
*  July 2007
*  Applied Research Laboratories, The University of Texas at Austin
*/
   // Language Headers
#include <stdio.h>

   // Library Headers
#include "icd_200_constants.hpp"

   // Project Headers
#include "ConstellationDefinition.hpp"

using namespace std;
using namespace gpstk;

ConstellationDefinition::ConstellationDefinition( ):
   effectiveDate( DayTime::BEGINNING_OF_TIME )
   { }

bool ConstellationDefinition::inBase24( const SatID SV ) const
{
   SlotDef sd = findSlot( SV );
   if (sd.slot<=4) return(true);
   return(false);
}

SlotDef ConstellationDefinition::getSlotDef( const SatID SV ) const
{
   return( findSlot( SV ) );
}

std::string ConstellationDefinition::getSlotString( const SatID SV ) const
{
   SlotDef sd = findSlot( SV );
   return( sd.planeSlot );
}

char ConstellationDefinition::getPlaneOnly( const SatID SV ) const
{
   SlotDef sd = findSlot( SV );
   return( sd.plane );
}

int ConstellationDefinition::getSlotOnly( const SatID SV ) const
{
   SlotDef sd = findSlot( SV );
   return( sd.slot );
}

int ConstellationDefinition::getTotalNumInBase24( ) const
{
   int count = 0;
   std::map<SatID,SlotDef>::const_iterator ci;
   for (ci=SVsToSlots.begin();ci!=SVsToSlots.end();++ci)
   {
      const SlotDef& sd = ci->second;
      if (sd.slot>=1 && sd.slot<=4) count++;  // Slots 1-4 in each plane define
                                              // the baseline 24 SV constellation 
   }
   return(count);
}

void ConstellationDefinition::setPlaneSlot( const SatID SV, 
                                            const char plane,
                                            const int slot )
{
   char temp[4];
   sprintf(temp,"%1c%1d",plane,slot);
   string ps(temp);
   SlotDef sd( ps, plane, slot );
   setPlaneSlot( SV, sd );     
}

void ConstellationDefinition::setPlaneSlot( const SatID SV, const SlotDef sd )
{
   SlotsToSVs.insert( make_pair( sd, SV ) );
   SVsToSlots.insert( make_pair( SV, sd ) );
}

void ConstellationDefinition::setEffectiveTime( const gpstk::DayTime dt )
{
   effectiveDate = dt;
   effectiveDate.setSecOfDay( (DayTime::SEC_DAY/2) );
}

void ConstellationDefinition::setSVNforPRN( const SatID SV, const int SVN )
{
   PRNtoSVN.insert( make_pair( SV, SVN ) );
}

void ConstellationDefinition::clearDefinition( )
{
   effectiveDate = DayTime::BEGINNING_OF_TIME;
   SlotsToSVs.clear();
   SVsToSlots.clear();
}
     
void ConstellationDefinition::dump( FILE* fp ) const
{
   char planes[] = { 'A', 'B', 'C', 'D', 'E', 'F' };
   fprintf(fp, "Constellation Definition for %s\n",
               effectiveDate.printf("%02m/%02d/%02y, DOY %03j, GPS Wk %F, DOW %w").c_str());
   fprintf(fp, "Plane   1   2   3   4   Extra\n");
   for (int i=0;i<6;++i)
   {
      bool done = false;
      int slot = 0;
      fprintf(fp,"  %1c  ",planes[i]);
      //while (!done)
      while (slot<9)
      {
         slot++;
         char temp[4];
         sprintf(temp,"%1c%1d",planes[i],slot);
         SlotDef sd( temp, planes[i], slot );
         try
         {
            SatID SV = findSV( sd );
            fprintf(fp,"  %02d",SV.id);
         }
         catch (ConstellationDefinition::NoSVFoundInSlot e)
         {
            if (slot<=4) fprintf(fp,"  --");
             else done = true;
         }
      }
      fprintf(fp,"\n");
   }
}

int ConstellationDefinition::getSVN( const SatID SV ) const
{
   std::map<SatID,int>::const_iterator ci;
   ci = PRNtoSVN.find(SV);
   if (ci!=PRNtoSVN.end()) return(ci->second);
   return(0); 
}

SlotDef ConstellationDefinition::findSlot( SatID SV ) const
   throw(ConstellationDefinition::NoSlotFoundForSV)
{
   std::map<SatID,SlotDef>::const_iterator ci;
   ci = SVsToSlots.find(SV);
   if (ci!=SVsToSlots.end())
   {
      SlotDef sd = ci->second;
      return( sd  );
   }

      // Did not find an Slot containing the requested SV.
      // Throw an exception
   char text[100];
   sprintf(text,"No Slot found containing SV ID %02d",SV.id);
   std::string sout = text;
   ConstellationDefinition::NoSlotFoundForSV exc(sout);
   GPSTK_THROW(exc);
}

gpstk::SatID ConstellationDefinition::findSV( SlotDef sd ) const
   throw(ConstellationDefinition::NoSVFoundInSlot)
{
   std::map<SlotDef,SatID>::const_iterator ci;
   ci = SlotsToSVs.find(sd);
   if (ci!=SlotsToSVs.end())
   {
      SatID sidr = ci->second;
      return( sidr );
   }

      // Did not find any SV occupying the specified slot.
      // Throw an exception
   char text[100];
   sprintf(text,"No SV found in slot %s",sd.planeSlot.c_str());
   std::string sout = text;
   ConstellationDefinition::NoSVFoundInSlot exc(sout);
   GPSTK_THROW(exc);
}
