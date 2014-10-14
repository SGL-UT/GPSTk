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

#ifndef CONSTLINEARRECURRENTSEQUENCE_HPP
#define CONSTLINEARRECURRENTSEQUENCE_HPP

#include <bitset>

template < int Length, int Initial, int Polynomial >
class ConstLinearRecurrentSequence :  public std::bitset<Length>
{
public:
   ConstLinearRecurrentSequence(int start_index=0)
      : index(start_index % Length)
   {
      int reg=Initial;
      int outputmask=1;

      /* First compute outputmask by finding highest value bit in Polynomial */
      for(int temp=(Polynomial>>1); temp; temp>>=1)
         outputmask<<=1;

      /* Now compute the sequence */
      for(int i=0; i<Length; i++)
      {
         int accum=0;
         (*this)[i]=reg&1;
         for(int j=0;j<31;j++)
            if(Polynomial&(1<<j))  /* WARNING: Assumes int is 32 bits */ 
               accum^=(reg<<j);
         
         reg=(reg|(accum&outputmask))>>1;
      }
   }

   bool operator*() const {return (*this).test(index);}

   void operator++() { if((++index)==Length) index=0; } 
 
   int setIndex(int new_index) { index=new_index%Length; return getIndex();}
   int getIndex() const { return index; }

   bool isLastInSequence() const { return index==(Length-1); }

private:
   int index;
   std::bitset<Length> store;
};

#endif
