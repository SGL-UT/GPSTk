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

#include <stdlib.h>
#include <math.h>

#include "normal.hpp"

const int log_two_of_sum_count=4;  // Higher number means better approximation
const double scaling_factor=sqrt(1<<log_two_of_sum_count)/((double)RAND_MAX*sqrt(1.0/12.0));
double
generate_normal_rv(void) {
  /* Generate a (mean=0, var=1) Gaussian RV approximation using central limit theorem */
  int i;
  int accum=0;

  for(i=0;i<(1<<log_two_of_sum_count);i++)
    accum+=rand()>>log_two_of_sum_count;

  return ((double)accum - (double)(RAND_MAX>>1))*scaling_factor;
}


#ifdef UNIT_TEST
#include <stdlib.h>
#include <stdio.h>

int
main(int argc, char *argv[]) {
 int i;
 double moment_1=0.0;
 double moment_2=0.0;

 for(i=0;i<100000;i++) {
   double x=generate_normal_rv();
   moment_1+=x;
   moment_2+=x*x;
 }

 printf("1st moment: %lf (should be near 0.0)\n",moment_1/100000.0);
 printf("2nd moment: %lf (should be near 1.0)\n",moment_2/100000.0);

 return 0;
}
#endif
