#pragma ident "$Id$"

/**
 * @file GaussianDistribution.cpp
 * This class implements the Gaussian (or normal) distribution.
 */


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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008
//
//============================================================================


#include "GaussianDistribution.hpp"


using namespace std;

namespace gpstk
{


      /* Default constructor. Sets a standard normal distribution:
       *  mean = 0.0, and standard deviation = 1.0.
       */
   GaussianDistribution::GaussianDistribution()
      : mean(0.0), sigma(1.0)
   {

      recompute();

   }  // End of constructor 'GaussianDistribution::GaussianDistribution()'



      /* Explicit constructor.
       *
       * @param mu      Mean
       * @param sig     Standard deviation
       *
       * \warning If (sig <= 0.0), it will be set to 1.0.
       */
   GaussianDistribution::GaussianDistribution( double mu,
                                               double sig )
      : mean(mu), sigma(sig)
   {

      recompute();

   }  // End of constructor 'GaussianDistribution::GaussianDistribution()'



      /* Computes the probability density function
       *
       * @param x    Value
       */
   double GaussianDistribution::pdf(double x)
   {

      return ( a * exp( b * (x - mean) * (x - mean) ) );

   }  // End of method 'GaussianDistribution::pdf()'



      /* Computes the cumulative distribution function
       *
       * @param x    Value
       */
   double GaussianDistribution::cdf(double x)
   {

      return ( 0.5 *
             ( 1.0
             + gpstk::erf( 0.70710678118654746 * (x - mean)/sigma ) ) );

   }  // End of method 'GaussianDistribution::cdf()'



      /* Sets the standard deviation
       *
       * @param sig     Standard deviation
       *
       * \warning If (sig <= 0.0), it will be set to 1.0.
       */
   GaussianDistribution& GaussianDistribution::setSigma(double sig)
   {

      if( sig <= 0.0 )
      {
         sig = 1.0;
      }

      sigma = sig;

      recompute();

      return (*this);

   }  // End of method 'GaussianDistribution::setSigma()'



      /* Sets all parameters in one pass.
       *
       * @param mu      Mean
       * @param sig     Standard deviation
       *
       * \warning If (sig <= 0.0), it will be set to 1.0.
       */
   GaussianDistribution& GaussianDistribution::setParameters( double mu,
                                                              double sig )
   {

      mean = mu;

         // Set sigma, check limits and recompute
      setSigma(sig);

      return (*this);

   }  // End of method 'GaussianDistribution::setParameters()'



      // Compute internal parameters
   void GaussianDistribution::recompute(void)
   {

         // If sigma is less or equal than zero, let's set it to 1.0
      if( sigma <= 0.0)
      {
         sigma = 1.0;
      }

      a = 0.3989422804014327 / sigma;
      b = -0.5 / (sigma*sigma);

      return;

   }  // End of method 'GaussianDistribution::recompute()'



}  // End of namespace gpstk
