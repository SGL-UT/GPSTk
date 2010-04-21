#pragma ident "$Id:$"

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
//  Copyright 2010, The University of Texas at Austin
//
//============================================================================

// An example of robust statistics found in lib/geomatics
// compute Robust statistics. Also demonstrate the use
// of random number generators.

#include <iostream>
#include <cstdlib>
#include <ctime>

#include "RobustStats.hpp"
#include "random.hpp"
#include "Stats.hpp"

using namespace std;
using namespace gpstk;
using namespace Robust;


int main(int argc, char* argv[])
{
   double *s = 0; // NULL isn't part of the C++ std.
   double mean = 10.0;
   double std_dev = 2.0;
   double bad_measurement = 10000;

   try {

      // Generate a set of random numbers that are normally distributed
      size_t N = 1000;
      s= new double[N];

      Stats<double> simpleStats;

      for (size_t i = 0; i<N; i++)
      {
         s[i] = RandNorm(std_dev)+mean;
	 // Note the 1.0 keeps the math floating point, 
         // otherwise the division is integer division.

         simpleStats.Add(s[i]);
      }

      // Show the sample mean and std deviation before adding perturbed samples.
      cout  << endl << "Before perturbation: sample mean is               " 
	    << simpleStats.Average() << ", " << endl;
      cout  << "                     sample standard deviation is " 
	    << simpleStats.StdDev() << endl << endl;

      // Now perturb the sample data set with a "bad" measurement.
      srand(time(0));
      size_t i = (rand()%N);
      s[i] = bad_measurement;
      cout << "Altering measurement " << i << " to take the value of " 
	   << bad_measurement << endl;

      // Show how the sample mean and std deviation are altered.
      Stats<double> secondStats;
      for (size_t i = 0; i<N; i++)
      {
         secondStats.Add(s[i]);
      }
					       
      // Show the sample mean and std deviation before adding perturbed samples.
      cout  << endl << "After perturbation:  sample mean is                " 
	    << secondStats.Average() << ", " << endl;
      cout  << "                     sample standard deviation is " 
	    << secondStats.StdDev() << endl << endl;

      double median,mad,mest,Q1,Q3;

      QSort(s,N);
      Robust::Quartiles(s,N,Q1,Q3);
      mad = Robust::MedianAbsoluteDeviation(s,N,median);

      cout << "Robust statistics:\n";
      cout << "                     number    = " << N << endl;
      cout << "                     quartiles = " << setw(11) << setprecision(8) << Q1
	   << " " << setw(11) << setprecision(8) << Q3 << endl;
      cout << "                     median    = " << setw(11) << setprecision(8) << median << endl;
      cout << "                     MAD       = " << setw(11) << setprecision(8) << mad << endl;


      // Show how the sample mean and std deviation are altered.
      Stats<double> thirdStats;
      for (size_t i = 0; i<N; i++)
      {
	if ((fabs(s[i] - median)/mad)<8)
         thirdStats.Add(s[i]);
      }
					       

      // Compute the mean and std deviation now given robust statistics.
      cout  << endl << "Using robust stats:  sample mean is                " 
	    << thirdStats.Average() << ", " << endl;
      cout  << "                     sample standard deviation is " 
	    << thirdStats.StdDev() << endl << endl;



      
   }
   catch (bad_alloc& b)
   {
      cerr << "Allocation error. Out of memory?" << endl;
      exit(0);
   } 

   // Delete the dynamic array allocation. 
   // (don't just rely on the operating system to do this).
   delete[] s;
   s = 0;

  exit(0);
}
