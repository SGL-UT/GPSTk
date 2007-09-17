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

/**
 * @file index.hpp
 * A template function to find the index of a particular element of std::vector
 */

#ifndef INDEX_ROUTINE_INCLUDE
#define INDEX_ROUTINE_INCLUDE

//------------------------------------------------------------------------------------
// find the index of first occurance of item t (of type T) in vector<T> v;
// i.e. j = index(v,t); implies v[j] == t. Return -1 if t is not found.
template<class T> int index(const std::vector<T> v, const T& t) 
{
   for(int i=0; i<v.size(); i++) {
      if(v[i] == t) return i;
   }
   return -1;
}

/*
// find the index of first occurance of item t (of type T) in vector<T> v;
// i.e. j = index(v,t); implies v[j] == t. Return -1 if t is not found.
// assume that the vector<T> is strictly increasing,
// that is that J > I strictly implies v[J] > v[I].
// let istart be a suggested starting point for the search;
// that is istart+(small) or istart-(small) may very well be the desired index
template<class T> int index_uniform(const std::vector<T> v, const T& t, int istart=0)
{
   if(istart < 0 || istart > v.size()) istart=0;
   int i=istart,k=0;
   while(i<v.size() && i>=0) {
      if(v[i] == t) {
         return i;
      }
      else if(v[i] < t) {
         if(k == -1) return -1;
         k = 1;                  // increasing
      }
      else {
         if(k == 1) return -1;
         k = -1;                 // decreasing
      }
      i += k;
   }
   return -1;
}
*/

#endif
