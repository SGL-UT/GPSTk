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
*   @file ConstellationSet.hpp
*
*   Applied Research Laboratories, The University of Texas at Austin
*   July 2007
*
*   Each ConstellationSet object holds information 
*/
#ifndef CONSTELLATION_SET_HPP
#define CONSTELLATION_SET_HPP

   // Language Headers
#include <stdio.h>
#include <map>
#include <iostream>
#include <iomanip>


   // Library Headers
#include "ConstellationDefinition.hpp"

   // Project Headers

namespace gpstk
{
   
class ConstellationSet
{
   public:
      enum FileType
      {
         ARLFileType = 1,
         OpAdvisory,
         CSV,
         other
      };
      NEW_EXCEPTION_CLASS(NoConstellationFound, Exception);
      NEW_EXCEPTION_CLASS(InvalidDateString, Exception);
      NEW_EXCEPTION_CLASS(MismatchSize, Exception);

      ConstellationSet( );
      ConstellationSet( const std::string filename, FileType ftype );
      ~ConstellationSet() {}
      gpstk::DayTime getEarliestDate() const
         throw(NoConstellationFound);
      gpstk::DayTime getLatestDate() const
         throw(NoConstellationFound);
      int getTotalNum( ) const;
      ConstellationDefinition findCD( const gpstk::DayTime dt ) const 
         throw(NoConstellationFound);
      
      int loadFile( const std::string filename, FileType ftype ); 
      void clearSet( );
      
      void dump( FILE * logfp) const;
      
   protected:
      typedef std::map<gpstk::DayTime,ConstellationDefinition>::const_iterator CI;
      std::map<gpstk::DayTime,ConstellationDefinition> cdMap;

      gpstk::DayTime parseDate(std::string date)
        throw(InvalidDateString);
      std::list<std::string> parseNums(std::string data, std::string searching);
      int loadFileARL( const std::string filename );
      int loadFileOpAdvisory( const std::string filename )
         throw(InvalidDateString, MismatchSize);
      int loadFileCSV( const std::string filename );
};

inline int ConstellationSet::getTotalNum( ) const 
           { return(cdMap.size()); }
}
#endif
