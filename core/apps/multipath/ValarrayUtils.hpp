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
 * @file ValarrayUtils.cpp
 * Namespace of functions that work with valarrays.
 */

/**
 * @file ValarrayUtils.hpp
 * ValarrayUtils namespace and GPSTK valarray utility functions
 */

#ifndef GPSTK_VALARRAYUTILS_HPP
#define GPSTK_VALARRAYUTILS_HPP

#include <iostream>

#include <valarray>
#include <set>
#include <vector>

#include "Exception.hpp"

namespace gpstk
{
   /**
    * Provides basic operations available in matrix languages to
    * the valarray template.
    */
   namespace ValarrayUtils
   {
      /** @defgroup valarrayutilsgroup Valarray Manipulation Tools */
      //@{

      /// This is thrown instread of a std::exception when a
      /// gpstk::ValarrayUtils function fails.
      /// @ingroup exceptiongroup
      NEW_EXCEPTION_CLASS(ValarrayException, Exception);
      //@}

      template<class T> std::ostream& operator<<(std::ostream& ostr,
         const std::valarray<T>& x)
      {
         for (size_t i=0; i<x.size(); i++)
            ostr << x[i] << " ";
         return ostr;
         // return copy(&x[0], &x[x.size()], std::ostream_iterator<T>(ostr));
      }

      template<class T> std::ostream& operator<<(std::ostream& ostr,
         const std::set<T>& x)
      {
         for (typename std::set<T>::const_iterator i = x.begin(); i!=x.end(); i++)
            ostr << *i << " ";
         return ostr;
         // return copy(x.begin(), x.end(), std::ostream_iterator<T>(ostr));
      }

      template<class T> std::ostream& operator<<(std::ostream& ostr,
         const std::vector<T>& x)
      {
         for (typename std::vector<T>::const_iterator i = x.begin(); i!=x.end(); i++)
            ostr << *i << " ";
         return ostr;
         // return copy(x.begin(), x.end(), std::ostream_iterator<T>(ostr));
      }

      template<class T> std::set<T> unique(const std::valarray<T> & rhs)
      {
         size_t i;

         std::valarray<T> newArray;

         std::set<T> newSet;

         for (i=0; i<rhs.size(); i++)
         {
            //cout << rhs[i] << endl;
            newSet.insert(rhs[i]);
         }

         return newSet;
      }

   }                             // namespace ValarrayUtils

}                                // namespace gpstk
#endif                           // GPSTK_VALARRAYUTILS_HPP
