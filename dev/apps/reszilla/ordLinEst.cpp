
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

#include "OrdApp.hpp"
#include "RobustLinearEstimator.hpp"

using namespace std;
using namespace gpstk;

class OrdLinEst : public OrdApp
{
public:
   OrdLinEst() throw();
   
   bool initialize(int argc, char *argv[]) throw();

protected:
   virtual void process();

};

//-----------------------------------------------------------------------------
// The constructor basically just sets up all the command line options
//-----------------------------------------------------------------------------
OrdLinEst::OrdLinEst() throw()
   : OrdApp("ordLinEst", "Computes a linear clock estimate. ")

{}

//-----------------------------------------------------------------------------
bool OrdLinEst::initialize(int argc, char *argv[]) throw()
{
   return OrdApp::initialize(argc,argv);
}


//-----------------------------------------------------------------------------
void OrdLinEst::process()
{   
   gpstk::ORDEpochMap oem;
   // read in data from the ord file to map of ORDEpochs
   while (input)
   {
      ORDEpoch ordEpoch = read(input); 
      oem[ordEpoch.time] = ordEpoch; 
   }
   
   RobustLinearEstimator rle;

   DoubleDoubleVec clocks;
   ORDEpochMap::const_iterator ei;
   int n=0;
   for (ei = oem.begin(); ei != oem.end(); ei++)
   {
      double mjd = ei->first.MJDdate();
      vdouble clk = ei->second.clockOffset;
      if (!clk.is_valid() || std::abs(clk) < 1e-6)
         continue; 
      std::pair<double, double> pr(mjd, clk);
      clocks.push_back(pr);
   }

   rle.process(clocks);
   
   bool gotEstimate = rle.a != 0;
   if (gotEstimate)
   {
      ORDEpochMap::iterator i;
      for (i=oem.begin(); i != oem.end(); i++)
      {
         const DayTime& t = i->first;
         ORDEpoch& ord = i->second;
         if (ord.clockOffset.is_valid())
            ord.clockResidual = ord.clockOffset - rle.eval(t.MJDdate());

         write(output, i->second);      
      }

      const int N=8;
      output << "# time              type      offset(m)   slope(m/day)    abdev(m)" << endl;
      output << setfill(' ');
      DayTime t0(oem.begin()->first);
      DayTime t1(oem.rbegin()->first);      
      for (int i=0; i<=N; i++)
      {
         DayTime t = t0 + i*(t1-t0)/N;
         output << t.printf(timeFormat) << " " << setw(4) << 51 //type
                << " " << setprecision(5) << setw(14)  << rle.eval(t.MJDdate())
                << " " << setprecision(5) << setw(14) << rle.b
                << " " << setprecision(3) << setw(11)  << rle.abdev
                << endl;
      }
   }   
   else
   {
      output << "# Unable to form linear estimate" << endl;
      ORDEpochMap::iterator i;
      for (i=oem.begin(); i != oem.end(); i++)
         write(output, i->second);      
   }
}

//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   try
   {
      OrdLinEst crap;
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

