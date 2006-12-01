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

#include "OrdApp.hpp"

using namespace std;
using namespace gpstk;

class OrdClock : public OrdApp
{
public:
   OrdClock() throw();
   
   bool initialize(int argc, char *argv[]) throw();

protected:
   virtual void process();

private:
   CommandOptionNoArg useWartsOption, estimateOnlyOption, debiasOnlyOption;
};


//-----------------------------------------------------------------------------
// The constructor basically just sets up all the command line options
//-----------------------------------------------------------------------------
OrdClock::OrdClock() throw()
   : OrdApp("clkGen", "Generates clock estimates for each epoch of ords."),
     useWartsOption('w', "use-warts",
        "Use warts in the clock solution. The default is "
        "to not use warts (type=20)."),
     estimateOnlyOption('e', "estimate-only",
        "Only compute the receiver clock bias. Don't remove"
        " this bias from the ords. The default is to both estimate"
        " the bias and remove the it from the ords."),
     debiasOnlyOption('b', "debias-only",
        "Only remove the bias from the ords. "
        "The default is to both estimate the bias and remove the it from the ords.")
{}


//-----------------------------------------------------------------------------
bool OrdClock::initialize(int argc, char *argv[]) throw()
{
   return OrdApp::initialize(argc,argv);
}


//-----------------------------------------------------------------------------
void OrdClock::process()
{
   // Set up our clock model
   gpstk::EpochClockModel cm;
   cm.setSigmaMultiplier(1.5);
   cm.setElevationMask(15);
   cm.setSvMode(ObsClockModel::ALWAYS);

   if (useWartsOption.getCount())
      cm.setUseWonkyData(true);

   bool estimate=true;
   bool debias=true;
   if (estimateOnlyOption.getCount())
      debias = false;

   if (debiasOnlyOption.getCount())
      estimate = false;

   while (input)
   {
      ORDEpoch ordEpoch = read(input);

      if (estimate)
      {
         cm.addEpoch(ordEpoch);
         if (cm.isOffsetValid())
            ordEpoch.clockOffset = cm.getOffset();
      }

      if (debias && ordEpoch.clockOffset.is_valid())
         ordEpoch.removeOffset(ordEpoch.clockOffset);

      if (ordEpoch.clockOffset.is_valid())
      {
         double clk_mag = std::abs(ordEpoch.clockOffset);
         if (clk_mag > 1e6 || clk_mag < 1e-5)
            ordEpoch.wonky = true;
      }
      else
         ordEpoch.wonky = true;

      if (ordEpoch.wonky)
      {
         ORDEpoch::ORDMap::iterator i;
         for (i = ordEpoch.ords.begin(); i != ordEpoch.ords.end(); i++)
            i->second.wonky = true;
      }

      write(output, ordEpoch);
   }
}


//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   try
   {
      OrdClock crap;
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
