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

#ifdef SIMPLE_DEMO

#include "ConstLinearRecurrentSequence.hpp"

#include <stdio.h>
#include <bitset>

typedef ConstLinearRecurrentSequence<1023,0x3FF,(1<<10)|(1<<3),std::bitset<1023> > CA_G1_Sequence;

typedef ConstLinearRecurrentSequence<1023,0x3FF,(1<<10)|(1<<9)|(1<<8)|(1<<6)|(1<<3)|(1<<2),std::bitset<1023> > CA_G2_Sequence;


template <> CA_G1_Sequence::StoreType CA_G1_Sequence::store(0);
template <> CA_G2_Sequence::StoreType CA_G2_Sequence::store(0);



int
main(int argc, char *argv[])
{

  printf("C/A G2 sequence is:\n");
  for(CA_G2_Sequence::iterator i;!i.isLastInSequence();++i) {
   printf("%u",*i);
  }
  printf("\n");
}
#endif
