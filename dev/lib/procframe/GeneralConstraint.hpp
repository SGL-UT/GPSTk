#pragma ident "$Id$"

/**
 * @file GeneralConstraint.hpp
 * GeneralConstraint.
 */

#ifndef GPSTK_GENERALCONSTRAINT_HPP
#define GPSTK_GENERALCONSTRAINT_HPP

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

#include "Vector.hpp"
#include "Matrix.hpp"
#include "ConstraintSystem.hpp"
#include "SolverGeneral.hpp"

namespace gpstk
{

      /** This class working with 'SolverGeneral'.
       *
       */
   class GeneralConstraint
   {
   public:

         /// Common constructor
      GeneralConstraint(SolverGeneral& solverGeneral):solver(solverGeneral){}
         

         /// Feed the  constraint equations to the solver
      int constraint(gnssSatTypeValue& gData);
      

         /// Feed the  constraint equations to the solver
      int constraint(gnssRinex& gRin);
      

         /// Feed the  constraint equations to the solver
      int constraint(gnssDataMap& gdsMap);

         /// The method is used to update the solver state when the reference 
         /// satellite changed.
      virtual void updateRefSat(const SourceID& source, const SatID& sat)
      { /* Do nothing by d0efault */ }

         /// Default destructor
      virtual ~GeneralConstraint(){}

   protected:
      
         /// Override this metod to design your own constraint equations
      virtual void realConstraint(gnssDataMap& gdsMap){}

         /// Low level metod impose a ConstraintSystem object to the solver
      int constraintToSolver(ConstraintSystem& system, gnssDataMap& gdsMap);
      
         // Methods to parsing data from SolverGeneral

      Variable getVariable(const SourceID& source, const SatID& sat, const TypeID& type);

      VariableSet getVariables()
      { return solver.getEquationSystem().getCurrentUnknowns(); }

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

      SourceIDSet getCurrentSources()
      { return solver.getEquationSystem().getCurrentSources();}

      VariableSet getCurrentUnknowns()
      { return solver.getEquationSystem().getCurrentUnknowns();}

      SatIDSet getCurrentSats()
      { return solver.getEquationSystem().getCurrentSats();}


      Vector<double> getSolution(const VariableSet& varSet);


      Matrix<double> getCovariance(const VariableSet& varSet);


      GeneralConstraint& setSolution( const Variable& variable,
                                              const double& val )
      { solver.setSolution(variable,val); return (*this); }


      GeneralConstraint& setCovariance( const Variable& var1, 
                                                const Variable& var2,
                                                const double& cov)
      { solver.setCovariance(var1,var2,cov); return (*this); }

      
      GeneralConstraint& changeState( const VariableList& varList,
                                              const Matrix<double>& convertMat);

      int findIndexOfSat(const SatIDSet& satSet,const SatID& sat);

      void stackVariables(VariableList& varList,const VariableSet& varSet);

   protected:

         /// The partner solver it work with
      SolverGeneral& solver;

   }; // End of class 'GeneralConstraint'

}  // End of namespace gpstk


#endif   // GPSTK_SOLVERCONSTRAINT_HPP