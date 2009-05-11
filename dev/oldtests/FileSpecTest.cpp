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
//============================================================================

#include <iostream>
#include <cstdio>

#include "FileSpec.hpp"
#include "FileHunter.hpp"
#include "DayTime.hpp"

/**
 * @file FileSpecTest.cpp
 *
 */

using namespace std;

main()
{
   try
   {
      cout << "Please make sure you've run genFileSpecTestDirs.pl before beginning." << endl;
      cout << "push enter to continue" << endl;
      getchar();
      
      int i;
#ifndef _WIN32
      gpstk::FileHunter moo("fstest/ADMS%3n/RINEXOBS/S%2n%t%03jA.%02yO");
#else
      gpstk::FileHunter moo("fstest\\ADMS%3n\\RINEXOBS\\S%2n%t%03jA.%02yO");
#endif
      moo.dump(cout);

      gpstk::DayTime start = gpstk::DayTime(2001, 51, (double)0);
      gpstk::DayTime end = gpstk::DayTime(2001, 53, (double)0);
      cout << "All files should be between " << start << " and " << end << " ascending. " << endl;
      vector<string> vs = moo.find(start, 
                                   end,
                                   gpstk::FileSpec::ascending);
      for(i = 0; i < vs.size(); i++)
      {
         cout << "search1 " << vs[i] << endl;
      }

      cout << endl;
      cout << "All files should be between " << start << " and " << end << " descending. " << endl;
      vs = moo.find(start, 
                    end,
                    gpstk::FileSpec::descending);
      for(i = 0; i < vs.size(); i++)
      {
         cout << "2 " << vs[i] << endl;
      }

      cout << endl;
      cout << "All files ascending order" << endl;
      vs = moo.find(gpstk::DayTime::BEGINNING_OF_TIME,
                    gpstk::DayTime::END_OF_TIME,
                    gpstk::FileSpec::ascending);
      for(i = 0; i < vs.size(); i++)
      {
         cout << "search3 " << vs[i] << endl;
      }

      cout << endl;
      vector<string> stationFilter;
      stationFilter.push_back("85402");
      moo.setFilter(gpstk::FileSpec::station, stationFilter);
      cout << "All files from 85402 descending order" << endl;
      vs = moo.find(gpstk::DayTime::BEGINNING_OF_TIME,
                    gpstk::DayTime::END_OF_TIME,
                    gpstk::FileSpec::descending);
      for(i = 0; i < vs.size(); i++)
      {
         cout << "search4 " << vs[i] << endl;
      }

         // this tests the version filtering of file hunter
      cout << " version search shoudl return the highest number " << endl;
      moo.newHunt("%1v");
      moo.dump(cout);
      vs = moo.find();
      for(i = 0; i < vs.size(); i++)
      {
         cout << "version search " << vs[i] << endl;
      }
      cout << endl;
   }
   catch (gpstk::Exception& e)
   {
      cout << e << endl;
      return -1;
   }
   
      // this tests the FileSpec finctions for extracting and setting
      // file information

   cout << "testing FileSpec extraction functions" << endl;
   gpstk::FileSpec boo("ADMS%3n/RINEXOBS/S%2n%t%03jA.%02yO");
   boo.dump(cout);
   gpstk::DayTime now;
   gpstk::FileSpec::FSTStringMap fstsMap;
   fstsMap[gpstk::FileSpec::station] = string("85408");
   fstsMap[gpstk::FileSpec::selected] = string("1");
   string filename = boo.toString(now, fstsMap);
   cout << filename << endl;
   cout << "station " << boo.extractField(filename, gpstk::FileSpec::station) << endl;
   cout << "daytime " << boo.extractDayTime(filename) << endl;

   boo.newSpec("ADMS402/RINEXOBS/S%2n%t%03jA.%02yO");
   boo.dump(cout);

   return 0;

}
