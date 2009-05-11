#pragma ident "$Id$"
#include "ConstLinearRecurrentSequence.hpp"

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

#ifdef SIMPLE_DEMO

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
