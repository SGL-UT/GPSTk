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
 * @file Triple.cpp
 * Three element double vectors, for use with geodetic coordinates
 */

#include "geometry.hpp"
#include "Triple.hpp"
#include <cmath>

namespace gpstk
{
   using namespace std;

   Triple :: Triple()
         : theArray(3)
   {
   }

   Triple :: Triple(const Triple& right)
      : theArray(right.theArray)
   {
   }

   Triple :: Triple(double a, 
                    double b,
                    double c)
      : theArray(3)
   {
      theArray[0] = a;
      theArray[1] = b;
      theArray[2] = c;
   }

   Triple& Triple :: operator=(const Triple& right)
   {
      theArray = right.theArray;
      return *this;
   }

   Triple& Triple :: operator=(const valarray<double>& right)
      throw(GeometryException)
   {
      if (right.size() != 3)
      {
         GPSTK_THROW(GeometryException("Incorrect vector size"));
      }

      theArray = right;
      return *this;
   }

      // returns the dot product of the two vectors
   double Triple :: dot(const Triple& right) const
      throw()
   {
      Triple z;
      z = (this->theArray)*(right.theArray);
      double a = z.theArray.sum();
      return a;
   }


      // retuns v1 x v2 , vector cross product
   Triple Triple :: cross(const Triple& right) const
      throw()
   {
      Triple cp;
      cp[0] = (*this)[1] * right[2] - (*this)[2] * right[1];
      cp[1] = (*this)[2] * right[0] - (*this)[0] * right[2];
      cp[2] = (*this)[0] * right[1] - (*this)[1] * right[0];
      return cp;
   }


   double Triple :: mag() const throw()
   {
      return ::sqrt(dot(*this));
   }

   Triple Triple::unitVector() const
   {
      double mag = sqrt(dot(*this));
      Triple retArg;
      retArg[0] = (*this)[0] / mag;
      retArg[1] = (*this)[1] / mag;
      retArg[2] = (*this)[2] / mag;
      return(retArg);
   }

      // function that returns the cosine of angle between this and right
   double Triple :: cosVector(const Triple& right) const
      throw()
   {
      double rx, ry, cosvects;
   
      rx = dot(*this);
      ry = right.dot(right);
      cosvects = dot(right) / ::sqrt(rx * ry);

      /* this if checks for and corrects round off error */
      if (fabs(cosvects) > 1.0e0)
      {
         cosvects = fabs(cosvects) / cosvects;
      }

      return cosvects;
   }


      // Computes the slant range between two vectors
   double Triple :: slantRange(const Triple& right) const
      throw()
   {
      Triple z;
      z = right.theArray - this->theArray;
      double r = z.mag();
      return r;
   }


      // Finds the elevation angle of the second point with respect to
      // the first point
   double Triple :: elvAngle(const Triple& right) const
      throw()
   {
      Triple z;
      z = right.theArray - this->theArray;
      double c = z.cosVector(*this);
      return 90.0 - ::acos(c) * RAD_TO_DEG;
   }


      //  Calculates a satellites azimuth from a station
   double Triple :: azAngle(const Triple& right) const
      throw(GeometryException)
   {
      double xy, xyz, cosl, sinl, sint, xn1, xn2, xn3, xe1, xe2;
      double z1, z2, z3, p1, p2, test, alpha;

      xy = (*this)[0] * (*this)[0] + (*this)[1] * (*this)[1] ;
      xyz = xy + (*this)[2] * (*this)[2] ;
      xy = ::sqrt(xy);
      xyz = ::sqrt(xyz);

      cosl = (*this)[0] / xy;
      sinl = (*this)[1] / xy;
      sint = (*this)[2] / xyz;

      xn1 = -sint * cosl;
      xn2 = -sint * sinl;
      xn3 = xy/xyz;

      xe1 = -sinl;
      xe2 = cosl;

      z1 = right[0] - (*this)[0];
      z2 = right[1] - (*this)[1];
      z3 = right[2] - (*this)[2];

      p1 = (xn1 * z1) + (xn2 * z2) + (xn3 * z3) ;
      p2 = (xe1 * z1) + (xe2 * z2) ;

      test = fabs(p1) + fabs(p2);

      if (test < 1.0e-16)
      {
         GPSTK_THROW(GeometryException("azAngle(), failed p1+p2 test."));
      }

      alpha = 90 - ::atan2(p1, p2) * RAD_TO_DEG;
      if (alpha < 0)
      {
         return alpha + 360;
      }
      else 
      {
         return alpha;
      }
   }

   Triple Triple :: operator-(const Triple& right) const
   { 
      Triple tmp;
      tmp.theArray = this->theArray - right.theArray;
      return tmp;
   }

   Triple Triple :: operator+(const Triple& right) const
   { 
      Triple tmp; 
      tmp.theArray = this->theArray + right.theArray; 
      return tmp;
   }

   std::ostream& operator<<(std::ostream& s, 
                            const gpstk::Triple& v)
   {
      if (v.size() > 0)
      {  
         s << "(" << v[0];
         for (int i = 1; i < v.size(); i++)
         {
            s << ", " << v[i];
         }
         s << ")";
      }
      
      return s;   
   }

} // namespace gpstk

