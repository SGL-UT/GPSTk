#pragma ident "$Id$"

/**
 * @file SpecialFunctions.hpp
 * Contains handy special functions: Gamma, erf, erfc, etc.
 */

#ifndef SPECIALFUNCTIONS_HPP
#define SPECIALFUNCTIONS_HPP

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


#include <cmath>
#include "icd_200_constants.hpp"


namespace gpstk
{

      /** @addtogroup math */
      //@{


      /** Computes the Gamma function using a simple Lanczos approximation.
       *
       * This implementation typically gives 15 correct decimal places, and
       * it is adapted from free Python code found in:
       *
       * http://en.wikipedia.org/wiki/Lanczos_approximation
       *
       * \warning Be aware that Gamma function is not defined for 0, -1, -2,...
       */
   double gamma(const double val);


      /** Computes the natural logarithm of Gamma function
       *  using the Lanczos approximation.
       *
       * \warning This version does not work for values <= 0.0
       */
   double lgamma(double val);


      /// Lower incomplete gamma function.
   double lower_gamma(const double a, const double z);


      /// Upper incomplete gamma function.
   double upper_gamma(const double a, const double z);


      /// Lower incomplete regularized gamma function P(a,z).
   double gammaP(const double a, const double z);


      /// Upper incomplete regularized gamma function Q(a,z).
   double gammaQ(const double a, const double z);


      /** Error function.
       *
       * This is a C++ implementation of the free Python code found in:
       *
       *   http://code.activestate.com/recipes/576391/
       *
       * Such code was based in a C code base with OpenBSD license from:
       *
       * ====================================================
       * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
       *
       * Developed at SunPro, a Sun Microsystems, Inc. business.
       * Permission to use, copy, modify, and distribute this
       * software is freely granted, provided that this notice
       * is preserved.
       * ====================================================
       */
   double erf(const double x);


      /// Complementary error function.
   double erfc(const double x);


      //@}

}  // End of namespace gpstk
#endif   // SPECIALFUNCTIONS_HPP
