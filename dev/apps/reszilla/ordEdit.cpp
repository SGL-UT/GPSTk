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

#include <EpochClockModel.hpp>
#include <vector>
#include <set>
#include "gps_constants.hpp"
#include "OrdApp.hpp"
#include "OrdApp.cpp"
#include "EphReader.hpp"
#include "BCEphemerisStore.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;

class OrdEdit : public OrdApp
{
public:
   OrdEdit() throw();
   
   bool initialize(int argc, char *argv[]) throw();

protected:
   virtual void process();

private:
   CommandOptionNoArg clkOpt, removeUnhlthyOpt, noClockOpt, noORDsOpt;
   CommandOptionWithNumberArg elvOpt, prnOpt, clkResOpt;
   CommandOptionWithAnyArg ephSourceOpt, startOpt, endOpt;
   
   double elMask, clkResidLimit;
   set<int> prnSet;
   vector<string> ephFilesVector;
   DayTime tStart, tEnd;
};


//-----------------------------------------------------------------------------
// The constructor basically just sets up all the command line options
//-----------------------------------------------------------------------------
OrdEdit::OrdEdit() throw()
   : OrdApp("ordEdit", "Edits an ord file based on various criteria."),
     elMask(0),clkResidLimit(0),
     removeUnhlthyOpt('u', "remove-unhealthy","Remove data for unhealthy SVs."
                  " Requires ephemeris source option."),
     ephSourceOpt('e',"be-file","Broadcast ephemeris source. Must be RINEX "
                  "nav file. In type 0 lines, health fields will be filled "
                  "in."),           
     elvOpt('m',"elev","Remove data for SVs below a given elevation mask."),
     clkOpt('k',"clock-est", "Remove ords that do not have corresponding "
            "clock estimates."),
     clkResOpt('s',"size","Remove clock residuals that are greater than "
               "this size (meters)."),
     prnOpt('p',"PRN","Add/Remove data from given PRN. Repeat option for multiple"
            " PRNs. Negative numbers remove, Postive numbers all, Zero removes all."),
     noClockOpt('c',"no-clock", "Remove all clock offset estimate warts. Give"
                " this option twice to remove all clock data. "),
     noORDsOpt('o',"no-ords","Remove all obs warts. Give this option twice to"
               " leave only warts. Give this option three times to remove all"
               "ORD data. "),
     startOpt('\0',"start","Throw out data before this time. Format as "
              "string: \"MO/DD/YYYY HH:MM:SS\" "),
     endOpt('\0',"end","Throw out data after this time. Format as string:"
            " \"MO/DD/YYYY HH:MM:SS\" ")
   
{}

//-----------------------------------------------------------------------------
bool OrdEdit::initialize(int argc, char *argv[]) throw()
{
   return OrdApp::initialize(argc,argv);
}

//-----------------------------------------------------------------------------
void OrdEdit::process()
{   
   //-- don't know SV health without broadcast ephemeris
   if (removeUnhlthyOpt.getCount() && !(ephSourceOpt.getCount()))
   {
      cout << "Need broadcast ephemeris source in order to remove data "
           << "from unhealthy SVs. Exiting..." << endl;
      exit(0);
   }
   //-- Get ephemeris data
   EphReader ephReader;
   ephReader.verboseLevel = verboseLevel;
   for (int i=0; i<ephSourceOpt.getCount(); i++)
      ephReader.read(ephSourceOpt.getValue()[i]);
   gpstk::EphemerisStore& eph = *ephReader.eph;  
   //-- Make sure that the eph data provided is broadcast eph 
   if (ephSourceOpt.getCount()&&(typeid(eph)!=typeid(BCEphemerisStore)))
   {
      cout << "You provided an eph source that was not broadcast ephemeris.\n"
              "(Precise ephemeris does not contain health info and can't be \n"
              " used with this program.) Exiting... \n";
      exit(0);
   }
   //-- get PRNs to be excluded
   for (int index = 0; index < prnOpt.getCount(); index++)
   {
      int prn = asInt(prnOpt.getValue()[index]);
      if (prn < 0)
         prnSet.insert(-prn);
      else if (prn > 0)
         prnSet.erase(prn);
      else
      {
         prnSet.clear();
         for (int i=1; i<=gpstk::MAX_PRN; i++)
            prnSet.insert(i);
      }
   }
   //-- get ephemeris sources, if given
   int numBEFiles = ephSourceOpt.getCount();
   for (int index = 0; index < numBEFiles; index++)
      ephFilesVector.push_back(asString(ephSourceOpt.getValue()[index]));
   //-- remove data below a given elevation mask?
   if (elvOpt.getCount())
      elMask = asDouble(elvOpt.getValue().front());
   //-- discard clock residuals that are too large?
   if (clkResOpt.getCount())
      clkResidLimit = asDouble(clkResOpt.getValue().front());
   //-- if a time span was specified, get it
   double ss;
   int mm,dd,yy,hh,minu; 
   if (startOpt.getCount())
   {
      sscanf(startOpt.getValue().front().c_str(),"%i/%i/%i %i:%i:%lf",
             &mm,&dd,&yy,&hh,&minu,&ss);
      tStart.setYMDHMS((short)yy,(short)mm,(short)dd,(short)hh,
                       (short)minu,(double)ss);
   }
   if (endOpt.getCount())
   {
      sscanf(endOpt.getValue().front().c_str(), "%i/%i/%i %i:%i:%lf",
             &mm,&dd,&yy,&hh,&minu,&ss);
      tEnd.setYMDHMS((short)yy,(short)mm,(short)dd,(short)hh,
                     (short)minu, (double)ss);
   }
   //-- too lazy?
   if (verboseLevel || debugLevel)
   {
      cout << "#   So, according to you, ordEdit should be... \n";
      if (clkOpt.getCount())
         cout << "# Removing ords that do not have corresponding "
              << "clock estimates.\n";
      else
         cout << "# Leaving in ords without corresponding clock "
              << "estimates.\n";
      if (removeUnhlthyOpt.getCount())
         cout << "# Removing unhealthy SVs.\n";
      else 
         cout << "# Not looking at SV health.\n";
      if (elMask)
         cout << "# Elevation mask set to " << elMask << " deg.\n";
      else
         cout << "# Keeping data for all SVs above the horizon. \n";
      if (startOpt.getCount())
         cout << "# Tossing data before " << tStart << endl;
      else
         cout << "# Start time is beginning of file. \n";
      if (endOpt.getCount())
         cout << "# Tossing data after " << tEnd << endl;
      else
         cout << "# End time is end of file. \n";
      if (prnSet.size())
      {
         cout << "# Ignoring PRNs: ";
         set<int>::const_iterator i;
         for (i = prnSet.begin(); i != prnSet.end(); i++)
            cout << *i << " ";
         cout << endl;
      }
      if (clkResidLimit)
         cout << "# Tossing clk resids > " << clkResidLimit << " m.\n";
      else
         cout << "# Keeping all clock residuals.\n";
      if (numBEFiles)
      {
         for (int index = 0; index < numBEFiles; index++)
            cout << "# Eph source: " <<  ephSourceOpt.getValue()[index] 
                 << endl;
      }
      if (noClockOpt.getCount() == 1)
         cout << "# Removing clock offset warts from ord file.\n";
      else if (noClockOpt.getCount() > 1)
         cout << "# Removing all clock data from ord file.\n";
      if (noORDsOpt.getCount() == 1)
         cout << "# Removing obs warts from ord file.\n";
      else if (noORDsOpt.getCount() == 2)
         cout << "# Leaving only warts in the data.\n";      
      else if (noORDsOpt.getCount() > 2)
         cout << "# Removing all ord data from file.\n";      
   }
   while (input)
   {
      ORDEpoch ordEpoch = read(input); 
      if (clkOpt.getCount() && !(ordEpoch.clockOffset.is_valid()))
         continue;
      else if (startOpt.getCount() && (ordEpoch.time < tStart))
         continue;
      else if (endOpt.getCount() && (ordEpoch.time > tEnd))
         continue;
      if (numBEFiles && removeUnhlthyOpt.getCount())
      {
         const BCEphemerisStore& bce = dynamic_cast<const BCEphemerisStore&>(eph);
         ORDEpoch::ORDMap::iterator iter = ordEpoch.ords.begin();
         while (iter!= ordEpoch.ords.end())
         {
            const SatID& satId = iter->first;
            ObsRngDev& ord = iter->second;
            iter++;
            const EngEphemeris& eph = bce.findEphemeris(satId, ordEpoch.time);
            ord.health =  eph.getHealth();
            if (ord.health.is_valid() && ord.health != 0)
               ordEpoch.removeORD(satId);
         }
      }
      if (elMask)
      {
         ORDEpoch::ORDMap::const_iterator iter = ordEpoch.ords.begin();
         while (iter!= ordEpoch.ords.end())
         {
            const SatID& satId = iter->first;
            const ObsRngDev& ord = iter->second;
            iter++;
            if ((ord.getElevation()< elMask))
                  ordEpoch.removeORD(satId);
         }
      }   

      if (noClockOpt.getCount() == 1)
      {
         // removing receiver clock offset estimate warts (type 70 lines)
         if (ordEpoch.clockOffset.is_valid() && ordEpoch.wonky)
            ordEpoch.clockOffset.set_valid(false);
      }
      else if (noClockOpt.getCount() > 1)
      {
         // removing all clock data (line types 50, 51, and 70)
         ordEpoch.clockOffset.set_valid(false);
      }
      if (noORDsOpt.getCount() == 1)
      {
         // removing obs warts (the type 20 lines)
         ORDEpoch::ORDMap::const_iterator iter = ordEpoch.ords.begin();
         while (iter!= ordEpoch.ords.end())
         {
            const SatID& satId = iter->first;
            const ObsRngDev& ord = iter->second;
            iter++;
            if (ord.wonky)
               ordEpoch.removeORD(satId);
         }
      }
      if (noORDsOpt.getCount() == 2)
      {
         // removing good obs (the type 0 lines)
         ORDEpoch::ORDMap::const_iterator iter = ordEpoch.ords.begin();
         while (iter!= ordEpoch.ords.end())
         {
            const SatID& satId = iter->first;
            const ObsRngDev& ord = iter->second;
            iter++;
            if (!ord.wonky)
               ordEpoch.removeORD(satId);
         }
      }
      else if (noORDsOpt.getCount() > 2)
      {
         // removing all ords
         ORDEpoch::ORDMap::const_iterator iter = ordEpoch.ords.begin();
         while (iter!= ordEpoch.ords.end())
         {
            const SatID& satId = iter->first;
            const ObsRngDev& ord = iter->second;
            iter++;
            ordEpoch.removeORD(satId);
         }         
      }
      if (prnSet.size())
      {
         set<int>::const_iterator i;
         for (i = prnSet.begin(); i != prnSet.end(); i++)
            ordEpoch.removeORD(SatID(*i,SatID::systemGPS));
      }
      if (clkResidLimit)
      {
         if (false)
            continue;
         /*********************************************
         hrrmmm. no clock residual because don't have 
         type 51 (linear clock estimate) generation
         anywhere yet. TBAdded
         **********************************************/
      }

      write(output, ordEpoch);
   }
   if (verboseLevel || debugLevel)
      cout << "#   Doneskies.\n";
}

//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   try
   {
      OrdEdit crap;
      if (!crap.initialize(argc, argv))
         exit(0);
      crap.run();
   }
   catch (gpstk::Exception &exc)
   { cout << exc << endl; }
   catch (std::exception &exc)
   { cerr << "Caught std::exception " << exc.what() << endl; }
   catch (...)
   { cerr << "Caught unknown exception" << endl; }
}

