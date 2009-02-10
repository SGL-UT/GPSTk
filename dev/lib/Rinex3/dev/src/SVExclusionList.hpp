#pragma ident "$Id: SVExclusionList.hpp 81 2006-08-10 16:45:12Z ehagen $"



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




/**
*
*
*
*Input File Format
*    The file may contain any number of lines of ASCII text.  Any line that 
*    does not begin with "EX" or "TS" will be ignored.  Lines beginning with
*    TS are assumed to contains a double quote delimited string that defines
*    the time format that will be used in following EX lines.  The default
*    time string is "%F %g" (GPS full week, GPS Second of Week).  See 
*    DayTime.hpp for more information on time specificaction strings 
*    (in particular the method DayTime.setToString()).
*
*    Each "EX" line must follow the format:
*
* EX pp,  startTime, endTime,  comment(optional)
* 
*where
*    pp  - SV PRNID
*    startTime - In format defined by latest TS command
*    endTime - In format defined by latest TS command
*    comment - An optional ASCII text comment to be stored with the exclusion
*
*    Read failues will silently discarded, however, the calling program 
*    may access a count of the number of read failures and a list of the 
*    failures.
*/
#ifndef GPSTK_SVEXCLUSIONLIST_HPP
#define GPSTK_SVEXCLUSIONLIST_HPP

//
#include <stdio.h>
#include <fstream>
#include <iostream>

// std library
#include <map>

// gpstk
#include "DayTime.hpp"
#include "Exception.hpp"
#include "gps_constants.hpp"

namespace gpstk
{
   class SVExclusion
   {
      public:
         SVExclusion( const gpstk::DayTime begin,
                      const gpstk::DayTime end, 
                      const int PRNID, 
                      const std::string commentArg );
         bool isApplicable( const int PRNID, const gpstk::DayTime dt ) const;
         std::string getComment() const;
         int getPRNID() const;
         gpstk::DayTime getBeginTime() const;
         gpstk::DayTime getEndTime() const;
         
      protected:
         int PRN_IDENTIFIER;
         gpstk::DayTime begExclude;
         gpstk::DayTime endExclude;
         std::string comment;
   };

      // For this first implementation, there will be a multimap 
      // organized by PRN.
      
   typedef std::multimap< int, SVExclusion >::const_iterator SVXListCI;
   typedef std::pair<SVXListCI,SVXListCI> SVXListPair;
      
   class SVExclusionList 
   {
      public:
         NEW_EXCEPTION_CLASS( NoSVExclusionFound , gpstk::Exception);
         NEW_EXCEPTION_CLASS( SVExclusionFileNotFound , gpstk::Exception);
         
         SVExclusionList( );
         SVExclusionList( std::string filename )
           throw(SVExclusionFileNotFound);
         void addFile( const std::string filename )
            throw(SVExclusionFileNotFound);
         bool isExcluded( const int PRN, const gpstk::DayTime dt ) const;
         void addExclusion( const SVExclusion );
         gpstk::DayTime getEarliestTime() const;
         gpstk::DayTime getLatestTime() const;
         int getNumberOfExclusions() const;
         const SVExclusion& getApplicableExclusion(
                            const int PRN, const gpstk::DayTime dt) 
                            const throw(NoSVExclusionFound);
         int numberOfReadFailures() const;
         void listOfReadFailures() const;       // List goes to cerr
         void listOfReadFailures( FILE* fpout ) const;
         void listOfReadFailures( std::ofstream fsout ) const;
         void dumpList( FILE* fp ) const;
         
      protected:
         gpstk::DayTime earliestTime;
         gpstk::DayTime latestTime;
         std::multimap< int, SVExclusion > exclusionMap;
         
         std::string timeSpecString;
         int readFailCount;
         std::list<std::string> readFailList;

         std::string buildFailString(const std::string s,
                     const int lineCount, const std::string filename );
   };
   
   inline gpstk::DayTime SVExclusionList::getEarliestTime() const
      { return(earliestTime); }
   inline gpstk::DayTime SVExclusionList::getLatestTime() const 
      { return(latestTime); }
   inline int SVExclusionList::getNumberOfExclusions() const
      { return(exclusionMap.size()); } 
   inline int SVExclusionList::numberOfReadFailures() const
      { return(readFailCount); }
      
   inline std::string SVExclusion::getComment() const {return(comment); }
   inline int SVExclusion::getPRNID() const { return(PRN_IDENTIFIER); }
   inline gpstk::DayTime SVExclusion::getBeginTime() const { return(begExclude); }
   inline gpstk::DayTime SVExclusion::getEndTime() const { return(endExclude); }
   
}   
#endif      
