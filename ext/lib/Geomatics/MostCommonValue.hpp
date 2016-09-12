/// @file MostCommonValue.hpp
/// Simple class to find the most common value, within a given tolerance, in a given
///    set of values.

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

#ifndef GPSTK_MOST_COMMON_VALUE_INCLUDE
#define GPSTK_MOST_COMMON_VALUE_INCLUDE
//--------------------------------------------------------------------------------
/// Simple class to find the most common value, within a given tolerance, in a given
/// set of values. Used to find, e.g. the time interval of a RINEX obs file.
/// Given enough examples (provided to add(dt)), it should be clear what the nominal
/// or most common value is. Use a tolerance to differentiate dt's, default 0.001.
class MostCommonValue
{
   int N;                        ///< number of different values to be considered
   double tol;                   ///< tolerance in comparing input to values[]
   std::vector<int> counts;      ///< number of times this value has been seen
   std::vector<double> values;   ///< value being considered - parallel to counts
   #define DEFAULT_DIM 9
   #define DEFAULT_TOL 0.00095   // less than a millisecond

public:
   /// only ctor; argument is the maximum number of values to remember and count (9)
   MostCommonValue(int i=DEFAULT_DIM) : tol(DEFAULT_TOL) { reset(i); }

   /// reset
   void reset(int i=DEFAULT_DIM) {
      N = i;
      counts.clear();
      values.clear();
   }

   /// add a measured timestep to the analysis
   inline void add(double dt)
   {
      for(int j=0; j<N; j++) {
         // first time this value has been seen, and there is room to consider it
         if(j >= counts.size() ) {
            counts.push_back(1);
            values.push_back(dt);
            break;
         }
         // match j?
         if(::fabs(dt-values[j]) < tol) {
            counts[j]++;
            break;
         }
         // running out of room
         if(j == N-1) {
            // find the least common dt and replace it
            int jj,kk,nl;
            for(jj=1,kk=0,nl=counts[0]; jj<counts.size(); jj++) {
               if(counts[jj] <= nl) { kk = jj; nl = counts[jj]; }
            }
            counts[kk] = 1; values[kk] = dt;       // replace it
         }
      }
   }

   /// return the best estimate of timestep
   inline double bestDT(void)
   {
      if(counts.size() == 0) return 0.0;
      int i,j(0);
      for(i=1; i<counts.size(); i++) if(counts[i] > counts[j]) j=i;
      return values[j];
   }

   /// return count for best estimate
   inline int bestN(void) {
      if(counts.size() == 0) return 0;
      int i,j(0);
      for(i=1; i<counts.size(); i++) if(counts[i] > counts[j]) j=i;
      return counts[j];
   }

   /// access the tolerance used in comparing timesteps
   inline double getTol(void) { return tol; }

   /// set the tolerance used in comparing timesteps
   inline void setTol(double t) { tol = t; }

   /// get the table of all the values and the number of times seen
   void getResults(std::vector<int>& out_counts, std::vector<double>& out_values)
   {
      out_counts = counts;
      out_values = values;
   }

   /// dump the table of values and counts
   void dump(std::ostream& os, int prec=3)
   {
      os << " MostCommonValue::dump(); tolerance is "
         << std::fixed << std::setprecision(prec) << tol << std::endl;
      os << " Table of counts and values from class MostCommonValue:\n";
      int n(counts.size());
      if(values.size() < n) n = values.size();

      for(int j=0; j<counts.size(); j++)
         os << " " << std::setw(3) << counts[j]
            << " " << std::fixed << std::setprecision(prec) << values[j] << std::endl;
   }

   #undef DEFAULT_DIM
   #undef DEFAULT_TOL
}; // end class MostCommonValue

#endif // GPSTK_MOST_COMMON_VALUE_INCLUDE
