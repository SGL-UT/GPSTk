/* Module for generating IID samples that approximate those from a Gaussian (normal) random variable */
/* Johnathan York, UT Applied Research Laboratories, October 2006 */

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
