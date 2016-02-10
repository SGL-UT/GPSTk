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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008
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

/**
 * @file StudentDistribution.hpp
 * This class implements the t-Student distribution.
 */

#ifndef STUDENTDISTRIBUTION_HPP
#define STUDENTDISTRIBUTION_HPP

#include "BaseDistribution.hpp"


namespace gpstk
{

      /// @ingroup math 
      //@{

      /** This class implements the t-Student distribution.
       *
       * A typical way to use this class follows:
       *
       * @code
       *
       *      // Declare a 'StudentDistribution' object with
       *      // 1 degree of freedom (default)
       *   StudentDistribution stuObj;
       *
       *   double x(5.7);
       *
       *   cout << stuObj.pdf(x) << " | "
       *        << stuObj.cdf(x) << " | "
       *        << stuObj.Q(x) << endl;
       *
       *      // Now, the same but with four degrees of freedom
       *   cout << stuObj.pdf(x, 4) << " | "
       *        << stuObj.cdf(x, 4) << " | "
       *        << stuObj.Q(x, 4) << endl;
       *
       * @endcode
       *
       * @sa SpecialFunctions.hpp for useful functions, and
       *     GaussianDistribution.hpp for a normal distribution.
       *
       */
   class StudentDistribution : public BaseDistribution
   {
   public:


         /// Default constructor. Sets the number of degrees of freedom to 1.
      StudentDistribution() : ndf(1) {};


         /** Explicit constructor.
          *
          * @param n       Degrees of freedom
          *
          * \warning "n" must be > 0, otherwise n = |n|.
          */
      StudentDistribution( int n )
      { setNDF(n); };


         /** Computes the probability density function
          *
          * @param x    Value
          */
      virtual double pdf(double x);


         /** Computes the probability density function
          *
          * @param x       Value
          * @param n       Degrees of freedom
          *
          * \warning "n" must be > 0, otherwise n = |n|.
          */
      virtual double pdf(double x, int n)
      { setNDF(n); return pdf(x); };


         /** Computes the cumulative distribution function
          *
          * @param x       Value
          */
      virtual double cdf(double x);


         /** Computes the cumulative distribution function
          *
          * @param x       Value
          * @param n       Degrees of freedom
          *
          * \warning "n" must be > 0, otherwise n = |n|.
          */
      virtual double cdf(double x, int n)
      { setNDF(n); return cdf(x); };


         /** Computes the upper tail of the t-student probability
          *  function Q(x, ndf).
          *
          * @param x       Value
          */
      virtual double Q(double x)
      { return ( 1.0 - cdf(x) ); };


         /** Computes the upper tail of the t-student probability
          *  function Q(x, n).
          *
          * @param x       Value
          * @param n       Degrees of freedom
          *
          * \warning "n" must be > 0, otherwise n = |n|.
          */
      virtual double Q(double x, int n)
      { return ( 1.0 - cdf(x,n) ); };


         /// Get number of degrees of freedom
      virtual double getNDF(void) const
      { return ndf; };


         /** Set the number of degrees of freedom.
          *
          * @param n       Degrees of freedom
          *
          * \warning "n" must be > 0, otherwise n = |n|.
          */
      virtual StudentDistribution& setNDF(int n)
         throw(InvalidParameter);


   private:


         /// Number of degrees of freedom
      int ndf;


   };  // End of class "StudentDistribution"

      //@}

}  // End of namespace gpstk
#endif   // STUDENTDISTRIBUTION_HPP
