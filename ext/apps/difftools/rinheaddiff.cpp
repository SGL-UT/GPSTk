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

#include <cerrno>
#include <cstdio>
#include "BasicFramework.hpp"
#include "Rinex3ClockHeader.hpp"
#include "Rinex3ClockStream.hpp"
#include "RinexMetHeader.hpp"
#include "RinexMetStream.hpp"
#include "Rinex3NavHeader.hpp"
#include "Rinex3NavStream.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsStream.hpp"

using namespace std;
using namespace gpstk;

class RinexHeaderDiff : public BasicFramework
{
public:

   enum FileType {
      Unknown,
      RinexClock,
      RinexMet,
      RinexNav,
      RinexObs
   };

   RinexHeaderDiff(const string& applName);

   static FileType identFile(const string& fname, FFData*& hdr);

protected:
   virtual void process();
   gpstk::CommandOptionWithAnyArg exclOption;
   gpstk::CommandOptionWithAnyArg inclOption;
   gpstk::CommandOptionRest inputFileOption;
   gpstk::CommandOptionNOf filesRestr;
   gpstk::CommandOptionMutex incexc;
};


RinexHeaderDiff::RinexHeaderDiff(const string& applName)
      : BasicFramework(applName, "Print the differences between the headers"
                       " of two RINEX files"),
        exclOption('x', "exclude", "RINEX header lines to exclude"),
        inclOption('i', "include", "RINEX header lines to compare"),
        inputFileOption("FILES", true),
        filesRestr(2) // must have 2 files
{
   inputFileOption.setMaxCount(2);
   filesRestr.addOption(&inputFileOption);
   incexc.addOption(&exclOption);
   incexc.addOption(&inclOption);
}


void RinexHeaderDiff::
process()
{
   FileType ft1, ft2;
   FFData *header1, *header2;
   string fn1(inputFileOption.getValue()[0]),
      fn2(inputFileOption.getValue()[1]);
   bool giveUp = false, including = false;
   std::vector<std::string> diffs, inclExclList;
      // Assume the headers are different until proved otherwise.
   exitCode = 1;

   if (inclOption.getCount())
   {
      inclExclList = inclOption.getValue();
      including = true;
   }
   else
   {
         // Make exclusion the default because if nothing is
         // specified, we'll be excluding nothing.
      inclExclList = exclOption.getValue();
   }

      // First test for readability. Use fopen because perror.
   FILE *accTest;
   if ((accTest = fopen(fn1.c_str(), "r")) == NULL)
   {
      string errmsg = argv0 + ": " + fn1;
      std::perror(errmsg.c_str());
      giveUp = true;
   }
   else
   {
      fclose(accTest);
   }
   if ((accTest = fopen(fn2.c_str(), "r")) == NULL)
   {
      string errmsg = argv0 + ": " + fn2;
      std::perror(errmsg.c_str());
      giveUp = true;
   }
   else
   {
      fclose(accTest);
   }
      // don't bother further processing if we can't open the files
   if (giveUp)
      return;
      // get the headers and file types
   ft1 = identFile(fn1, header1);
   ft2 = identFile(fn2, header2);
   if (ft1 == Unknown)
   {
      cerr << argv0 << ": " << fn1 << ": Unknown file format" << endl;
      giveUp = true;
   }
   if (ft2 == Unknown)
   {
      giveUp = true;
      cerr << argv0 << ": " << fn2 << ": Unknown file format" << endl;
   }
      // don't bother further processing if files are of an unknown format
   if (giveUp)
      return;
   if (ft1 == ft2)
   {
         // compare headers
      switch (ft1)
      {
         case RinexMet:
            if (dynamic_cast<RinexMetHeader*>(header1)->compare(
                   *(dynamic_cast<RinexMetHeader*>(header2)),
                   diffs, inclExclList, including))
            {
                  // compare success
               exitCode = 0;
            }
            break;
         case RinexNav:
            if (dynamic_cast<Rinex3NavHeader*>(header1)->compare(
                   *(dynamic_cast<Rinex3NavHeader*>(header2)),
                   diffs, inclExclList, including))
            {
                  // compare success
               exitCode = 0;
            }
            break;
         case RinexObs:
            if (dynamic_cast<Rinex3ObsHeader*>(header1)->compare(
                   *(dynamic_cast<Rinex3ObsHeader*>(header2)),
                   diffs, inclExclList, including))
            {
                  // compare success
               exitCode = 0;
            }
            break;
         default:
            cerr << "File format currently not supported" << endl;
            return;
      }
   }
   else
   {
      cerr << argv0 << ": Can't compare files of different formats" << endl;
      giveUp = true;
   }
   if (giveUp)
      return;
   if (exitCode)
   {
         // found some differences
      cout << "Headers are different:" << endl;
      for (unsigned i = 0; i < diffs.size(); i++)
         cout << diffs[i] << endl;
   }
}


RinexHeaderDiff::FileType RinexHeaderDiff::
identFile(const string& fname, FFData*& hdr)
{
   hdr = NULL;
   try
   {
      Rinex3ClockStream s(fname.c_str(), ios::in);
      Rinex3ClockHeader h;
      s >> h;
      if (s)
      {
         hdr = new Rinex3ClockHeader(h);
         return RinexClock;
      }
   }
   catch (...) {}
   try
   {
      RinexMetStream s(fname.c_str(), ios::in);
      RinexMetHeader h;
      s >> h;
      if (s)
      {
         hdr = new RinexMetHeader(h);
         return RinexMet;
      }
   }
   catch (...) {}
   try
   {
      Rinex3NavStream s(fname.c_str(), ios::in);
      Rinex3NavHeader h;
      s >> h;
      if (s)
      {
         hdr = new Rinex3NavHeader(h);
         return RinexNav;
      }
   }
   catch (...) {}
   try
   {
      Rinex3ObsStream s(fname.c_str(), ios::in);
      Rinex3ObsHeader h;
      s >> h;
      if (s)
      {
         hdr = new Rinex3ObsHeader(h);
         return RinexObs;
      }
   }
   catch (...) {}
   return Unknown;
}


int main(int argc, char* argv[])
{
   try
   {
      RinexHeaderDiff m(argv[0]);
      if (!m.initialize(argc, argv))
         return m.exitCode;
      if (!m.run())
         return m.exitCode;

      return m.exitCode;
   }
   catch(Exception& e)
   {
      cout << e << endl;
   }
   catch(std::exception& e)
   {
      cout << e.what() << endl;
   }
   catch(...)
   {
      cout << "unknown error" << endl;
   }
      // only reach this point if an exception was caught
   return BasicFramework::EXCEPTION_ERROR;
}
