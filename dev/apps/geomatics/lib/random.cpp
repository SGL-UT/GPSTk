#pragma ident "$Id: $"

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

/**
 * @file random.cpp
 * Simple random number generator.
 */

// -----------------------------------------------------------------------------
#include <iostream>
#include <iomanip>
#include <cmath>
#include <ctime>

#include "random.hpp"

using namespace std;

//double RandExpCor(double dt, double sigma, double T, double xlast);
//static int imod(int x, int y);

// -----------------------------------------------------------------------------
//int main(int argc, char **argv)
//{
	//time_t clk;
	//long i,N;
	//double x,y,z,dt=1.,sigma=5.0;

   //if(argc > 1) sigma=strtod(argv[1],NULL);
   //if(argc > 2) N=atol(argv[2]);
   //if(argc > 3) dt=strtod(argv[3],NULL);

	//time(&clk);
	//x = Rand(clk);
	//y = Rand(clk);
	//for(i=0; i<N; i++) {
		//// n = Rand(1)-0.5;
		//// n = RandNorm(5.0);
		//// n = RandomWalk(1.0, 5.0, n);
		//// n = RandExpCor(1.0, 5.0, 250., n);
		//y = RandomWalk(dt, sigma, y);
		//x = RandomWalk(dt, 1.0, x);
		//z = RandNorm(sigma);
		////printf("%4ld %10.3e\n",i+1,z);
      //cout << setw(4) << i+1
         //<< " " << fixed << setw(11) << setprecision(8) << x
         //<< " " << fixed << setw(11) << setprecision(8) << y
         //<< " " << fixed << setw(11) << setprecision(8) << z
         //<< endl;
		////printf("%4ld %11.8lf\n",i,n);
		//// j = 99.0*(n+16.0)/32.0;
		//// if(j>=0 && j<100) hist[j]++;
	//}
	//// for(i=0; i<100; i++) printf("%3hd %hd\n",i,hist[i]);
	//return 0;
//}

// -----------------------------------------------------------------------------
// Generate a random walk sequence, given sqrt variance sigma, time step dt
// and previous point xlast.
//
double RandomWalk(double dt, double sigma, double xlast)
{
	return xlast+RandNorm(sigma)*dt;
}

// -----------------------------------------------------------------------------
// Generate an exponentially correlated random sequence, given time step dt,
// sqrt variance sigma, time constant T and previous point xlast.
//
//double RandExpCor(double dt, double sigma, double T, double xlast)
//{
	//return exp(-dt/T)*xlast+RandNorm(sigma);
//}

// -----------------------------------------------------------------------------
// Generate random numbers uniformly distributed from 0.0 to 1.0.  Mbig and
// Mseed are large but arbitrary, but Mbig > Mseed.  The 55 is not arbitrary.
double Rand(long seed)
{
#define Mbig 1000000000.
#define Mseed 161803398.
#define imod(x,y) ((x)-((x)/(y))*(y))
	static short iff=0,inext,inextp;
	static double Ma[55];
	double mj,mk;
	short i,ii,k;
	if(!iff) {
      if(seed < 0) seed=-seed;
		mj = Mseed-seed;
		mj = fmod(mj,Mbig);
		Ma[54] = mj;
		mk = 1.0;
		for(i=0; i<55; i++) {
			ii = imod(21*(i+1),55);
			Ma[ii] = mk;
			mk = mj - mk;
			if(mk < 0.0) mk += Mbig;
			mj = Ma[ii];
		}
		for(k=0; k<4; k++) {
			for(i=0; i<55; i++) {
				Ma[i] -= Ma[imod(i+30,55)];
				if(Ma[i] < 0.0) Ma[i] += Mbig;
			}
		}
		inext = -1;
		inextp = 30;
		iff=1;
	}
	inext++; if(inext == 55) inext=0;
	inextp++; if(inextp == 55) inextp=0;
	mj = Ma[inext]-Ma[inextp];
	if(mj < 0.0) mj += Mbig;
	Ma[inext] = mj;
	return mj/Mbig;
#undef Mbig
#undef Mseed
#undef imod
}

// -----------------------------------------------------------------------------
// Generate normally distributed random numbers, zero mean and 
// sqrt of variance sigma.  Uses Box-Muller and Rand() above.
double RandNorm(double sigma)
{
#ifdef RAND_NORM_SAVE
	static short iset=0;
	static double saved;
	double r, v1, v2, fact;
	if(!iset) {
		do {
			v1 = 2.0*Rand(1)-1.0;
			v2 = 2.0*Rand(1)-1.0;
			r = v1*v1 + v2*v2;
		} while( r >= 1.0 || r == 0.0);
		fact = sigma*sqrt(-2.*log(r)/r);
		saved = v1*fact;
		iset = 1;
		return v2*fact;
	}
	iset = 0;
	return saved;
#else
	double r, v1, v2, fact;
	do {
		v1 = 2.0*Rand(1)-1.0;
		v2 = 2.0*Rand(1)-1.0;
		r = v1*v1 + v2*v2;
	} while( r >= 1.0 || r == 0.0);
	fact = sigma*sqrt(-2.*log(r)/r);
	return v2*fact;
#endif
}
// -----------------------------------------------------------------------------
// Return random integers between low and hi. If you want a different seed,
// call Rand(seed) before you call this.
// -----------------------------------------------------------------------------
int ARand(int low, int hi)
{
	double r=Rand(),d=(double)(hi-low);
	if(d < 0.0) d = -d;
	d = r*d;
	int i=(int)(d+0.5) + low;
	return i;
}

// -----------------------------------------------------------------------------
// Return random doubles between low and hi. If you want a different seed,
// call Rand(seed) before you call this.
// -----------------------------------------------------------------------------
double ARand(double low, double hi)
{
	double r=Rand(),d=(hi-low);
	if(d < 0.0) d = -d;
	d = r*d;
	return (low+d);
}

// -----------------------------------------------------------------------------
// integer mod function.  assume arguments positive.
//static int imod(int x, int y)
//{
	//if(x == 0 || y == 0) return 0;
	//return (x-(x/y)*y);
//}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
