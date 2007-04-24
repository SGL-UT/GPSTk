#include "ConstLinearRecurrentSequence.hpp"



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
