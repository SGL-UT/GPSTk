#pragma ident "$Id$"

/**
 * @file SolverConstraint.hpp
 * Solver Constrain.
 */

#ifndef GPSTK_SOLVERCONSTRAINT_HPP
#define GPSTK_SOLVERCONSTRAINT_HPP

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

#include "ConstraintSystem.hpp"
#include "SolverGeneral.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"

namespace gpstk
{
   class SolverGeneral;

   class SolverConstraint
   {
   public:

         /// Common constructor
      SolverConstraint():solver(0){}

         /// Method to  do constraint processing
      void constraint(SolverGeneral* pSolver,gnssDataMap& gdsMap)
      { solver=pSolver; constraint(gdsMap); }

         /// Method to get constraint matrix to feed back to the kalman filter
      void constraintMatrix(Vector<double>& prefit,
                            Matrix<double>& design,
                            Matrix<double>& covariance)
      { constraintSystem.constraintMatrix(getVariables(),prefit,design,covariance); }


      bool isValid();
         
         /// Default destructor
      virtual ~SolverConstraint(){}
    
   protected:

         /// Abstract to be overrided
      virtual void constraint(gnssDataMap& gdsMap);

         // Methods to parsing data from SolverGeneral

      VariableSet getVariables();

      VariableSet getVariables(const SourceID& source);

      VariableSet getVariables(const SourceID& source,const TypeID& type);

      VariableSet getVariables(const SourceID& source,const TypeIDSet& typeSet);

      VariableSet getVariables(const SourceIDSet& sourceSet);

      VariableSet getVariables(const SourceIDSet& sourceSet,const TypeID& type);

      VariableSet getVariables(const SourceIDSet& sourceSet,const TypeIDSet& typeSet);

      VariableSet getVariables(const SatID& sat);

      VariableSet getVariables(const SatID& sat,const TypeID& type);

      VariableSet getVariables(const SatID& sat,const TypeIDSet& typeSet);

      VariableSet getVariables(const SourceID& source, const SatID& sat, const TypeID& type);

      Vector<double> getSolution(const VariableSet& varSet);

      Matrix<double> getCovariance(const VariableSet& varSet);

   protected:

      SolverGeneral* solver;

      ConstraintSystem constraintSystem;
   };

}  // End of namespace gpstk


#endif   // GPSTK_SOLVERCONSTRAINT_HPP