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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

/**
 * @file random.hpp
 * Simple random number generator.
 */

#ifndef GPSTK_GEOMATICS_RANDOM_INCLUDE
#define GPSTK_GEOMATICS_RANDOM_INCLUDE

/// Generate random numbers uniformly distributed from 0 to 1.
double Rand(long seed=0);

/// Generate normally distributed random numbers, zero mean and 
/// sqrt of variance sigma.  Uses Box-Muller and Rand().
double RandNorm(double sigma);

/// Return random integers between low and hi. Seed the generator by calling
/// Rand(seed) before this call.
int ARand(int low, int hi);

/// Return random doubles between low and hi. Seed the generator by calling
/// Rand(seed) before this call.
double ARand(double low, double hi);

/// Generate a random walk sequence, given sqrt variance sigma, time step dt
/// and previous point xlast.
double RandomWalk(double dt, double sigma, double xlast);

/// Generate exponentially correlated random numbers, given sqrt variance sigma,
/// time step dt, time constant T, and previous point xlast.
double RandExpCor(double dt, double sigma, double T, double xlast);

#endif
