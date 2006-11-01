
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
#include "OrdApp.hpp"
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
   CommandOptionNoArg clkOpt, removeUnhlthyOpt;
   CommandOptionWithNumberArg elvOpt, typeOpt, prnOpt, clkResOpt;
   CommandOptionWithAnyArg ephSourceOpt, startOpt, endOpt;
   
   double elMask, clkResidLimit;
   vector<int> prnVector, typeVector;
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
     ephSourceOpt('b',"be file","Broadcast ephemeris source. Must be RINEX "
                  "nav file. In type 0 lines, health fields will be filled in."),           
     elvOpt('e',"elev","Remove data for SVs below a given elevation mask."),
     clkOpt('c',"clock-est", "Remove ords that do not have corresponding "
            "clock estimates."),
     typeOpt('y',"type","Remove lines for a specified type. Types are 0, "
             "20, 21, 50, and 51. Repeat option for removing more than one"
             " type."),
     clkResOpt('s',"size","Remove clock residuals that are greater than "
               "this size (meters)."),
     prnOpt('p',"PRN","Remove data from given PRN. Repeat option for multiple"
            " PRNs."),
     startOpt('\0',"start","Throw out data before this time. Format as string: \"MO/DD/YYYY HH:MM:SS\" "),
     endOpt('\0',"end","Throw out data after this time.    Format as string: \"MO/DD/YYYY HH:MM:SS\" ")
   
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
   //-- get PRNs to be excluded
   int numPRNs = prnOpt.getCount();
   for (int index = 0; index < numPRNs; index++)
      prnVector.push_back(asInt(prnOpt.getValue()[index]));
   //-- get line types to be excluded
   int numTypes = typeOpt.getCount();
   for (int index = 0; index < numTypes; index++)
      typeVector.push_back(asInt(typeOpt.getValue()[index]));
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
      sscanf(startOpt.getValue().front().c_str(), "%i/%i/%i %i:%i:%lf",&mm,&dd,&yy,&hh,&minu,&ss);
      tStart.setYMDHMS((short)yy, (short)mm, (short)dd, (short)hh, (short)minu, (double)ss);
   }
   if (endOpt.getCount())
   {
      sscanf(endOpt.getValue().front().c_str(), "%i/%i/%i %i:%i:%lf",&mm,&dd,&yy,&hh,&minu,&ss);
      tEnd.setYMDHMS((short)yy, (short)mm, (short)dd, (short)hh, (short)minu, (double)ss);
   }

   if (verboseLevel || debugLevel) // too lazy?
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
      if (numPRNs)
      {
         cout << "# Ignoring PRNs: ";
         for (int index = 0; index < numPRNs; index++)
            cout << prnVector[index] << " ";
         cout << endl;
      }
      if (numTypes)
      {
         cout << "# Ignoring line types: ";
         for (int index = 0; index < numTypes; index++)
            cout << typeVector[index] << " ";
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
   }
   
   // Get ephemeris data
   EphReader ephReader;
   ephReader.verboseLevel = verboseLevel;
   for (int i=0; i<ephSourceOpt.getCount(); i++)
      ephReader.read(ephSourceOpt.getValue()[i]);
   gpstk::EphemerisStore& eph = *ephReader.eph;

   while (input)
   {
      bool writeThisEpoch = true;
      ORDEpoch ordEpoch = read(input);
      
      if (clkOpt.getCount() && !(ordEpoch.clockOffset.get_value()))  // should be using is_valid() but it's not working right! TBFixed
         writeThisEpoch = false;
      else if (startOpt.getCount() && (ordEpoch.time < tStart))
         writeThisEpoch = false;
      else if (endOpt.getCount() && (ordEpoch.time > tEnd))
         writeThisEpoch = false;        
      
      if (numBEFiles)
      {
         ORDEpoch::ORDMap::const_iterator iter = ordEpoch.ords.begin();
         while (iter!= ordEpoch.ords.end())
         {
            const SatID& satId = iter->first;
            const ObsRngDev& ord = iter->second;
            //ord.health = eph.getSatHealth(satId, ordEpoch.time);   add health
            //ord.health = bce.getSatHealth(satId, ordEpoch.time);   ????????
         }
      }      
      if (removeUnhlthyOpt.getCount())
      {
         ORDEpoch::ORDMap::const_iterator iter = ordEpoch.ords.begin();
         while (iter!= ordEpoch.ords.end())
         {
            const SatID& satId = iter->first;
            const ObsRngDev& ord = iter->second;
            if (ord.health != 0 )
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
            if (ord.getElevation()< elMask)
               ordEpoch.removeORD(satId);
            iter++;
         }
      }     
      if (numTypes)
      {
      ; // TBD
      }
      if (numPRNs)
      {
         for (int index = 0; index < prnVector.size(); index++)
         {
            int prn = prnVector[index];
            vector<int>::iterator i;
            i = find(prnVector.begin(), prnVector.end(), prn);
            SatID thisSatID(prn,SatID::systemGPS);
            if (i!=prnVector.end())
               ordEpoch.removeORD(thisSatID);
         }
      }
      if (clkResidLimit)
      {
      ; /*********************************************
         hrmmmm. no clock residual because don't have 
         type 51 (linear clock estimate) coming out of 
         ordClock. TBFixed
       **********************************************/
      }

      if (writeThisEpoch)
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

