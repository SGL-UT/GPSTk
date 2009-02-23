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






#include "FICStream.hpp"
#include "FICData.hpp"
#include "RinexNavData.hpp"
#include "RinexNavStream.hpp"
#include "FileFilterFrame.hpp"

#include "BasicFramework.hpp"

using namespace std;
using namespace gpstk;

struct EphDiffLessThan : 
   public std::binary_function<EngEphemeris, EngEphemeris, bool>
{
public:
   bool operator() (const EngEphemeris& l, const EngEphemeris& r) const
      {
         if (l.getPRNID() < r.getPRNID())
            return true;
         else if (l.getPRNID() > r.getPRNID())
            return false;
         else if (l.getFullWeek() < r.getFullWeek())
            return true;
         else if (l.getFullWeek() > r.getFullWeek())
            return false;
         else if (l.getIODC() < r.getIODC())
            return true;
         else if (l.getIODC() > r.getIODC())
            return false;
         else if (l.getASAlert(1) < r.getASAlert(1))
            return true;
         else if (l.getASAlert(1) > r.getASAlert(1))
            return false;
         else if (l.getASAlert(2) < r.getASAlert(2))
            return true;
         else if (l.getASAlert(2) > r.getASAlert(2))
            return false;
         else if (l.getASAlert(3) < r.getASAlert(3))
            return true;
         else if (l.getASAlert(3) > r.getASAlert(3))
            return false;

         return false;
      }
};

struct EphDiffEquals : 
   public std::binary_function<EngEphemeris, EngEphemeris, bool>
{
public:
   bool operator() (const EngEphemeris& l, const EngEphemeris& r) const
      {
         if ( (l.getPRNID() == r.getPRNID()) && 
              (l.getIODC() == r.getIODC()) )
            return true;
         return false;
      }
};

struct EphDiffFinder : 
   public std::unary_function<EngEphemeris, bool>
{
public:
   EphDiffFinder(const EngEphemeris& e)
         : PRN(e.getPRNID()), IODC(e.getIODC())
      {}

   bool operator() (const EngEphemeris& l) const
      {
         if ( (l.getPRNID() == PRN) && (l.getIODC() == IODC) )
            return true;
         return false;
      }

private:
   short PRN;
   double IODC;
};

class EphDiff : public BasicFramework
{
public:
   EphDiff(char* arg0);
   virtual bool initialize(int argc, char* argv[]) throw();

protected:
   virtual void process();

   void fillFIC(FileFilterFrame<FICStream, FICData>& ff,
                vector<EngEphemeris>& l);
   void fillRINEX(FileFilterFrame<RinexNavStream, RinexNavData>& ff,
                  vector<EngEphemeris>& l);

private:
   CommandOptionWithAnyArg ficFileOption;
   CommandOptionWithAnyArg rinexFileOption;

   string file1, file2;

   vector<EngEphemeris> file1list, file2list;
};

EphDiff::EphDiff(char* arg0)
      : BasicFramework(arg0, "Compares the contents of two files with ephemeris data - either file can be RINEX or FIC"),
        ficFileOption('f',
                      "fic",
                      "Name of an input FIC file.",
                      false),
        rinexFileOption('r',
                        "rinex",
                        "Name of an input RINEX NAV file", 
                        false)
{
}

bool EphDiff::initialize(int argc, char* argv[]) throw()
{
   if(!BasicFramework::initialize(argc, argv))
   {
      return false;
   }

      // check the command options for 2 input files
   int ficCount = ficFileOption.getCount();
   int rinexCount = rinexFileOption.getCount();

   if (  ((ficCount == 2) && (rinexCount != 0)) ||
         ((ficCount == 1) && (rinexCount != 1)) ||
         ((ficCount == 0) && (rinexCount != 2)) )
   {
      cout << "Exactly two input files must be specified on the command line" 
           << endl
           << "   ephdiff is ending..." << endl
           << endl;
      return false;
   }

      // open the files
   
      // in the case of 1 FIC and 1 rinex file, make sure the first file
      // is chosen correctly...
   if ( (ficCount == 1) && (rinexCount == 1) )
   {
      string ficname = ficFileOption.getValue()[0];
      string rinexname = rinexFileOption.getValue()[0];

      FileFilterFrame<FICStream, FICData> ficdata(ficname);
      FileFilterFrame<RinexNavStream, RinexNavData> rinexdata(rinexname);

      if (ficFileOption.getOrder() < rinexFileOption.getOrder())
      {
         file1 = ficname;
         file2 = rinexname;
         fillFIC(ficdata, file1list);
         fillRINEX(rinexdata, file2list);
      }
      else
      {
         file1 = rinexname;
         file2 = ficname;
         fillRINEX(rinexdata, file1list);
         fillFIC(ficdata, file2list);
      }

   }
   else if (ficCount == 2)
   {
      file1 = ficFileOption.getValue()[0];
      FileFilterFrame<FICStream, FICData> fic1(file1);
      file2 = ficFileOption.getValue()[1];
      FileFilterFrame<FICStream, FICData> fic2(file2);

      fillFIC(fic1, file1list);
      fillFIC(fic2, file2list);
   }
   else // if (rinexCount == 2)
   {
      file1 = rinexFileOption.getValue()[0];
      FileFilterFrame<RinexNavStream, RinexNavData> rn1(file1);
      file2 = rinexFileOption.getValue()[1];
      FileFilterFrame<RinexNavStream, RinexNavData> rn2(file2);

      fillRINEX(rn1, file1list);
      fillRINEX(rn2, file2list);
   }

   return true;
}

void EphDiff::process()
{
      // first sort and filter the lists
   stable_sort(file1list.begin(), file1list.end(), EphDiffLessThan());
   stable_sort(file2list.begin(), file2list.end(), EphDiffLessThan());

   vector<EngEphemeris>::iterator listitr;

   listitr = unique(file1list.begin(), file1list.end(), EphDiffEquals());
   file1list.erase(listitr, file1list.end());

   listitr = unique(file2list.begin(), file2list.end(), EphDiffEquals());
   file2list.erase(listitr, file2list.end());

      // for each element in the first list, try to find a match in the
      // second list.  if a match is found, compare the data. otherwise
      // add to the unmatchedData list.
   vector<EngEphemeris> unmatchedData;

   while (!file1list.empty())
   {
      listitr = find_if(file2list.begin(), 
                        file2list.end(), 
                        EphDiffFinder(file1list[0]));
                      
      if (listitr != file2list.end())
      {
            // compare the data
         
            // delete the data so we don't search for it again
         file2list.erase(listitr);
      }
      else
         unmatchedData.push_back(file1list[0]);

      file1list.erase(file1list.begin());
   }

      // that's all the processing... now just write the results
   cout << "Data in " << file1 << " not found in " << file2 << ": " << endl;

   if (!unmatchedData.empty())
   {
      listitr = unmatchedData.begin();
      while (listitr != unmatchedData.end())
      {
         (*listitr).dump(cout << endl);
         listitr++;
      }
   }
   else
      cout << "   All data in " << file1 << " was found in " << file2 << "."
           << endl;
   
   cout << endl;
   cout << "Data in " << file2 << " not found in " << file1 << ": " << endl;
   
   if (!file2list.empty())
   {
      listitr = file2list.begin();
      while (listitr != file2list.end())
      {
         (*listitr).dump(cout << endl);
         listitr++;
      }
   }
   else
      cout << "   All data in " << file2 << " was found in " << file1 << "."
           << endl;

}

void EphDiff::fillFIC(FileFilterFrame<FICStream, FICData>& ff,
                      vector<EngEphemeris>& l)
{
   list<FICData>& ficlist = ff.getData();
   
   list<FICData>::iterator itr = ficlist.begin();

   while (itr != ficlist.end())
   {
      if ((*itr).blockNum == 9)
         l.push_back(*itr);
      itr++;
   }
}

void EphDiff::fillRINEX(FileFilterFrame<RinexNavStream, RinexNavData>& ff,
                        vector<EngEphemeris>& l)
{
   list<RinexNavData>& ficlist = ff.getData();
   
   list<RinexNavData>::iterator itr = ficlist.begin();

   while (itr != ficlist.end())
   {
      l.push_back(EngEphemeris(*itr));
      itr++;
   }
}

int main(int argc, char* argv[])
{
   try
   {
      EphDiff ed(argv[0]);
      if (!ed.initialize(argc, argv))
         return 0;
      if (!ed.run())
         return 1;
      
      return 0;
   }
   catch(Exception& e)
   {
      cout << e << endl;
   }
   catch(exception& e)
   {
      cout << e.what() << endl;
   }
   catch(...)
   {
      cout << "unknown error" << endl;
   }
   return 1;
}
