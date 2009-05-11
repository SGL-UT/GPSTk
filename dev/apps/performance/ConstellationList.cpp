#pragma ident "$Id$"
/**
*  ConstellationList - Produce comma separated list of PRN ID values for
*  the SVs that are in the constellation.  In order to do this, the program
*  needs:
*    (1.)  An input file that defines the GPS constellation at a given date
*          (or dates)
*    (2.)  The year of interest (2-digit or 4-digit will do)
*    (3.)  The day of year of interest (Julian day, 0-365/366)
*  The user may also specify if they want the list in the "baseline 24", the
*  "excess" above the baseline, or both.
*
*  USAGE:
*  >ConstellationList -i<definitionFile> -y<year> -j<DOY> [-b] [-x] [-n] [-O]
*  -i : Constellation defintion file
*  -y : year (2-digit or 4-digit)
*  -j : day of year (0-365|366)
*  -b : List "baseline 24" PRN IDs
*  -x : List PRN IDs "in excess of" the baseline 24, but in-use on orbit
*  -n : List PRN IDs that are current not in baseline 24, either on-orbit 
*       "excess" or simply not in use at this time. 
*  -O : Assume input file is in form of USCG Ops Advisory.  By default a 
*       comma separated value file is assumed.
*
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
//  Copyright 2009, The University of Texas at Austin
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
// System
#include <stdio.h>

// Library
#include "BasicFramework.hpp"
#include "DayTime.hpp"
#include "icd_200_constants.hpp"

// Project
#include "ConstellationSet.hpp"

using namespace std;
using namespace gpstk;


class ConstellationList : public gpstk::BasicFramework
{
public:
   ConstellationList(const std::string& applName,
              const std::string& applDesc) throw();
   ~ConstellationList() {}
   virtual bool initialize(int argc, char *argv[]) throw();
   
protected:
   virtual void process();
   gpstk::CommandOptionWithAnyArg inputOption;
   gpstk::CommandOptionNoArg typeOption;
   gpstk::CommandOptionWithAnyArg yearOption;
   gpstk::CommandOptionWithAnyArg DOYOption;
   gpstk::CommandOptionNoArg base24Option;
   gpstk::CommandOptionNoArg excessOption;
   gpstk::CommandOptionNoArg notBase24Option;
   gpstk::CommandOptionNoArg SVNOption;

   FILE *logfp;
   ConstellationSet cs;
   
   bool outputPRN;
};

int main( int argc, char*argv[] )
{
   try
   {
      ConstellationList fc("ConstellationList", "List the satellites in or out of the Base 24.");
      if (!fc.initialize(argc, argv)) return(false);
      fc.run();
   }
   catch(gpstk::Exception& exc)
   {
      cout << exc << endl;
      return 1;
   }
   catch(...)
   {
      cout << "Caught an unnamed exception. Exiting." << endl;
      return 1;
   }
   return 0;
}

ConstellationList::ConstellationList(const std::string& applName, 
                       const std::string& applDesc) throw()
          :BasicFramework(applName, applDesc),
           inputOption('i', "input-file", "The name of the ConstallationDefinition file(s) to read.", true),
           typeOption('O', "OpsAd","Assume input file is Op Advisory format (CSV is default)",false),
           yearOption('y', "year", "Year of interest.", true),
           DOYOption('j',"day-of-year","Day of year.", true),
           SVNOption('s',"SVN Output","Output SVN in place of PRN (not valid for -O)",false),
           base24Option('b',"Base24", "List PRNs in Base 24 constellation", false),
           notBase24Option('n',"notBase24","List PRNs NOT used in Base 24 constellation",false),
           excessOption('x',"excessSVs","List PRNs in use, but in excess of the Base 24 constellation",false)
{
   inputOption.setMaxCount(10);
   yearOption.setMaxCount(1);
   DOYOption.setMaxCount(1);
}

bool ConstellationList::initialize(int argc, char *argv[])
   throw()
{
   if (!BasicFramework::initialize(argc, argv)) return false;

      // Load constellation defintions.   
   int totalCount = 0;
   
   vector<string> values;

   ConstellationSet::FileType ft = ConstellationSet::CSV;   
   if (typeOption.getCount()>0)
   {
      ft = ConstellationSet::OpAdvisory;
   } 

   values = inputOption.getValue();
   vector<string>::const_iterator vi;
   for (vi=values.begin();vi!=values.end();++vi)
   {
      string filename = *vi;
      int count = cs.loadFile( filename, ft );
      totalCount += count;
   }
   if (totalCount<1) 
   {
      cout << "Failure reading input file." << endl;
      return false;
   }
   
   outputPRN = true;
   if (ft==ConstellationSet::CSV && SVNOption.getCount()>0) outputPRN = false;
   return true;   
}

void ConstellationList::process()
{
      // Get day of interest from command line arguments
      // Arrange so this'll work with either 2-digit or 4-digit year numbers
   int year = StringUtils::asInt( yearOption.getValue().front() );
   if (year>=0 && year<=70) year += 2000;
   if (year<100) year += 1900;
   
   int DOY = StringUtils::asInt( DOYOption.getValue().front() );
   
   DayTime dt = DayTime( (short) year, (short) DOY, (DayTime::SEC_DAY / 2));

      // Try some samples
   ConstellationDefinition cd = cs.findCD( dt );

      // Test each PRN ID in order and
      // output a comma separated list of those match the selected criteriaz
   bool first = true;
   for (int PRNID=1; PRNID<=gpstk::MAX_PRN; ++PRNID)
   {
      SatID SV = SatID( PRNID, SatID::systemGPS );
      try
      {
         bool inBase24 = cd.inBase24(SV);
         if (notBase24Option.getCount()!=0)
         {
            if (!inBase24) 
            {
               if (!first) cout << ", ";
               //cout << PRNID;
               if (outputPRN) cout << PRNID;
                else cout << cd.getSVN(SV); 
               first = false;
            }
         }
         else
         {
            if ( ( inBase24 && base24Option.getCount()!=0) ||
                 (!inBase24 && excessOption.getCount()!=0) )
            {
               if (!first) cout << ", ";
               //cout << PRNID;
               if (outputPRN) cout << PRNID;
                else cout << cd.getSVN(SV); 
               first = false;
            }
         }
      }
      catch(ConstellationDefinition::NoSlotFoundForSV e)
      { //Do nothing but continue
        //Except in case -n, in which we want to add this PRN ID to the list
         if (notBase24Option.getCount()!=0)
         {
            if (!first) cout << ", ";
            //cout << PRNID;
            if (outputPRN) cout << PRNID;
             else cout << cd.getSVN(SV); 
            first = false;
         }
      }
   }
   cout << endl;
}
