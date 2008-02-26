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
