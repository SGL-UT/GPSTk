#pragma ident "$Id$"

/**
 * @file ConstraintSystem.hpp
 * .
 */

#ifndef GPSTK_CONSTRAINTSYSTEM_HPP
#define GPSTK_CONSTRAINTSYSTEM_HPP

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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010, 2011
//
//============================================================================

#include "SolverGeneral.hpp"     // for 'VariableDataMap'
#include "Vector.hpp"
#include "Matrix.hpp"


namespace gpstk
{
      /// Constraint header declaration
   struct constraintHeader
   {
      double prefit;
      double variance;     // the smaller the tighter constraint

      constraintHeader():prefit(0),variance(100){}
   };

      /// Constraint structure declaration
   struct Constraint : gnssData<constraintHeader,VariableDataMap>
   {
         /// Default constructor.
      Constraint() {};

         /// Common constructor.
      Constraint( const constraintHeader& h,
         const VariableDataMap& b )
      {
         header = h;
         body   = b;
      }

         /// Destructor.
      virtual ~Constraint() {}
   };
      
      /// Handy type definition
   typedef std::list<Constraint> ConstraintList;

   class ConstraintSystem
   {
   public:
         /// Default constructor
      ConstraintSystem(){}

         /// Default deconstructor
      ~ConstraintSystem(){}

      virtual ConstraintSystem& addConstraint(const Constraint& constraint);

      virtual ConstraintSystem& removeConstraint( const Constraint& constraint );

      virtual ConstraintSystem& clearConstraint();

      virtual ConstraintSystem& setConstraint(const VariableSet& varSet,
                                              const Vector<double>& prefit,
                                              const Matrix<double>& design);

      virtual ConstraintSystem& constraintMatrix(const VariableSet& allVar,
                                                 Vector<double>& prefit,
                                                 Matrix<double>& design,
                                                 Matrix<double>& covariance);

   protected:
         /// Object to hold all constraints
      ConstraintList constraintList;

   }; // End of class 'ConstraintSystem'


}  // End of namespace gpstk

#endif   // GPSTK_CONSTRAINTSYSTEM_HPP