
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


// This is a function object that is intended to look for discontinuities
// in a series of pairs. 
struct BigRateOperator : 
   public std::binary_function<DoubleDouble, DoubleDouble, bool>
{
   BigRateOperator(double r)
      : maxRate(std::abs(r))
   {}

   bool operator() (const DoubleDouble& l, const DoubleDouble& r) const
   {
      double dt = l.first - r.first;
      double dy = l.second - r.second;
      double rate = 0;
      if (dt != 0.0) 
         rate = dy/dt;
      return abs(rate) > maxRate;
   }

   const double maxRate;
};


struct ClockSegment: public RobustLinearEstimator
{
   DayTime startTime, endTime;
};


struct ClockSegmentList : public list<ClockSegment>
{
   vdouble eval(const DayTime& t)
   {
      vdouble offset;
      for (const_iterator k=begin(); k != end(); k++)
      {
         const ClockSegment& cs = *k;
         if ((t - cs.startTime) > -0.01 && (cs.endTime - t) > -0.01)
         {
            double mjd = t.MJDdate();
            if (cs.valid)
               offset = cs.eval(mjd);
            break;
         }
      }
      return offset;
   }

   void dump(ostream& output, string timeFormat) const
   {
      output << "#  t0                   t1                   offset(m) slope(m/d)  abdev(m)" << endl;
      for (const_iterator k=begin(); k != end(); k++)
      {
         const ClockSegment& cs = *k;
         double t = cs.startTime.MJDdate();
         output << ">c " << cs.startTime.printf(timeFormat)
                << "  " << cs.endTime.printf(timeFormat)
                << fixed
                << " " << setprecision(2) << setw(10) << cs.eval(t)
                << " " << setprecision(3) << setw(10) << cs.b
                << " " << setprecision(3) << setw(9) << cs.abdev
                << endl;
      }
   }
};


class OrdLinEst : public OrdApp
{
public:
   OrdLinEst() throw();
   
   bool initialize(int argc, char *argv[]) throw();

protected:
   virtual void process();
   CommandOptionWithAnyArg maxRateOption;

};

//-----------------------------------------------------------------------------
// The constructor basically just sets up all the command line options
//-----------------------------------------------------------------------------
OrdLinEst::OrdLinEst() throw()
   : OrdApp("ordLinEst", "Computes a linear clock estimate. "),
     maxRateOption('m', "max-rate",
        "Rate used to detect a clock jump. default is 10,000 m/day")

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

   DoubleDoubleVec clocks;
   ORDEpochMap::const_iterator ei;
   for (ei = oem.begin(); ei != oem.end(); ei++)
   {
      double mjd = ei->first.MJDdate();
      vdouble clk = ei->second.clockOffset;
      if (!clk.is_valid() || std::abs(clk) < 1e-6)
         continue;
      clocks.push_back(DoubleDouble(mjd, clk));
   }

   // An emperically determied maximum clock drift rate, in meters per day
   double maxRate=10000;
   if (maxRateOption.getCount())
      maxRate = StringUtils::asDouble(maxRateOption.getValue().front());
   BigRateOperator bro(maxRate);

   DoubleDoubleVec::iterator i,j;
   ClockSegmentList csl;
   for (i=clocks.begin(); i != clocks.end(); i++)
   {
      j = adjacent_find( i, clocks.end(), bro);
      ClockSegment seg;
      seg.startTime = DayTime(i->first+1e-9);
      if (j != clocks.end())
         seg.endTime = DayTime(j->first+1e-9);
      else
         seg.endTime = DayTime(clocks.rbegin()->first+1e-9);
      seg.process(i, j);
      csl.push_back(seg);
      if (j == clocks.end())
         break;
      i = j;
   }

   csl.dump(output, timeFormat);

   ORDEpochMap::iterator l;
   for (l=oem.begin(); l != oem.end(); l++)
   {
      const DayTime& t = l->first;
      ORDEpoch& ord = l->second;
      vdouble offset = csl.eval(t);
      if (offset.is_valid() && ord.clockOffset.is_valid())
         ord.clockResidual = ord.clockOffset - offset;

      write(output, l->second);      
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

