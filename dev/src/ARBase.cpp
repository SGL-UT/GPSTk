#pragma ident "$Id$"

/**
 * @file ARBase.cpp
 * 
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
//  Wei Yan - Chinese Academy of Sciences . 2011
//
//============================================================================

#include "ARBase.hpp"


namespace gpstk
{
      // This method to get the convert matrix from SD ambiguity to DD 
      // ambiguity
   Matrix<double> ARBase::sd2ddMatrix(const size_t& n, const size_t& i)
   {
      if( i >= n)
      {
         Exception e("The reference index CAN NOT greater than toltal number.");
         GPSTK_THROW(e);
      }

      Matrix<double> sdMat(n-1, n, 0.0);

      for(int j = 0; j< sdMat.rows(); j++)
      {
         sdMat(j,i) = -1.0;
         if(j<i) sdMat(j,j) = 1.0;  
         else    sdMat(j,j+1) = 1.0;    
      }

      return sdMat;

   }  // End of method 'ARBase::sd2ddMatrix()'
   
}   // End of namespace gpstk

