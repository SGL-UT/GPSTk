#pragma ident "$Id: ConstraintSystem.hpp 2607 2011-05-19 05:31:37Z yanweignss $"

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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Wei Yan - Chinese Academy of Sciences . 2011
//
//============================================================================


#include "Variable.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"


namespace gpstk
{
      /// Constraint header declaration
   struct constraintHeader
   {
      double prefit;
      double variance;     // the smaller the tighter constraint

      constraintHeader():prefit(0.0),variance(1e-12){}

      constraintHeader(double meas,double var=1e-12)
         : prefit(meas),variance(var){}
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

      /// Thrown when attempting to use an invalid ConstraintSystem
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(InvalidConstraintSystem, gpstk::Exception);

   class ConstraintSystem
   {
   public:
         /// Default constructor
      ConstraintSystem(){}


         /// Default deconstructor
      ~ConstraintSystem(){}


         /// Add a single constraint
      virtual ConstraintSystem& addConstraint(const Constraint& constraint)
      { constraintList.push_back(constraint); return (*this); }


         /// Remove a single constraint
      virtual ConstraintSystem& removeConstraint(const Constraint& constraint);

         
         /// Clear all of the constraints
      virtual ConstraintSystem& clearConstraint()
      { constraintList.clear(); return (*this); }


         /// Method to set multi-constraints
      virtual ConstraintSystem& setConstraint(const VariableSet& varSet, 
                                              const Vector<double>& prefit);

         /// Method to set multi-constraints
      virtual ConstraintSystem& setConstraint(const VariableSet& varSet,
                                              const Vector<double>& prefit,
                                              const Matrix<double>& design);

         /// Method to get the matrixes of the ConstraintSystem
      virtual ConstraintSystem& constraintMatrix(const VariableSet& allVar,
                                                 Vector<double>& prefit,
                                                 Matrix<double>& design,
                                                 Matrix<double>& covariance)
         throw(InvalidConstraintSystem);

   
         /// Return current constraints
      ConstraintList getCurrentConstraints()
      { return constraintList; }


         /// Return the number of constraints
      int numberOfConstraints()
      { return constraintList.size(); }


         /// Check is there any constraint
      bool hasConstraints()
      { return (constraintList.size()>0)?true:false; }


      virtual ConstraintList getConstraintList() const
      { return constraintList; };


      virtual ConstraintSystem& setConstraintList(
                                         const ConstraintList& equationList )
      { constraintList = equationList; return (*this); };


         /// Add a constraint list
      virtual ConstraintSystem& addConstraintList(
                                            const ConstraintList& equationList);

   protected:

         /// Object to hold all constraints
      ConstraintList constraintList;

   }; // End of class 'ConstraintSystem'


}  // End of namespace gpstk

#endif   // GPSTK_CONSTRAINTSYSTEM_HPP
