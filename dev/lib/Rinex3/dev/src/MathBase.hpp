#pragma ident "$Id: MathBase.hpp 70 2006-08-01 18:36:21Z ehagen $"



/**
 * @file MathBase.hpp
 * Basic math #defines (ABS, SQRT, etc)
 */
 
#ifndef GPSTK_MATH_BASE_HPP
#define GPSTK_MATH_BASE_HPP

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

//@fix MSVC doesnt like std::sqrt or std::abs, and disabling
//extensions allows abs(double) to be used instead of fabs()
#ifdef _MSC_VER
#undef _MSC_EXTENSIONS
#endif
#include <cmath>
#ifdef _MSC_VER
#define _MSC_EXTENSIONS
#endif

namespace gpstk
{
// do Doxygen elsewhere
#ifdef _MSC_VER
#define ABS(x)  ::abs(x)
#define SQRT(x) ::sqrt(x)
#else
#define ABS(x)  std::abs(x)
#define SQRT(x) std::sqrt(x)
#endif

}  // namespace gpstk

#endif
