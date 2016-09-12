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
//  Wei Yan - Chinese Academy of Sciences . 2011
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
 * @file GeneralConstraint.hpp
 * GeneralConstraint.
 */

#ifndef GPSTK_GENERALCONSTRAINT_HPP
#define GPSTK_GENERALCONSTRAINT_HPP

#include <vector>
#include "Vector.hpp"
#include "Matrix.hpp"
#include "ConstraintSystem.hpp"
#include "SolverGeneral.hpp"
#include "GeneralEquations.hpp"

namespace gpstk
{
      /// Thrown when attempting to use an invalid Constraint
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(InvalidConstraint, gpstk::Exception);


      /** This class working with 'SolverGeneral'.
       *
       */
   class GeneralConstraint
   {
   public:

         /// Common constructor
      GeneralConstraint( SolverGeneral& solverGeneral )
         :solver(solverGeneral) {}
         

         /// Feed the  constraint equations to the solver
      virtual void process( gnssRinex& gRin,
                            GeneralEquations* gEquPtr = 0 );


         /// Feed the  constraint equations to the solver
      virtual void process( gnssDataMap& gdsMap,
                            GeneralEquations* gEquPtr = 0 );


         /// Default destructor
      virtual ~GeneralConstraint(){}

   protected:

         /// Feed the  constraint equations to the solver
      virtual void constraint( gnssRinex& gRin )
         throw(InvalidConstraint);


         /// Feed the  constraint equations to the solver
      virtual void constraint( gnssDataMap& gdsMap )
         throw(InvalidConstraint);
      

         /// Override this method to design your own constraint equations
      virtual void realConstraint(gnssDataMap& gdsMap){}


         /// The method is used to update the solver state when the reference 
         /// satellite changed.
      virtual void updateRefSat( const CommonTime& time, 
                                 const SatSourceMap& refsatSource,
                                 const SourceSatMap& sourceRefsat )
      { /* Do nothing by default */ }


         /// Low level method impose a ConstraintSystem object to the solver
      int constraintToSolver( ConstraintSystem& system, gnssDataMap& gdsMap );
      

      Matrix<double> convertMatrix(size_t n, size_t oi, size_t ni);


      Matrix<double> convertMatrix(size_t n, size_t oi, size_t ni,
                                   std::vector<int> iv);


         // Methods to parsing data from SolverGeneral

      Variable getVariable( const SourceID& source, 
                            const SatID& sat, 
                            const TypeID& type );


      VariableSet getVariables()
      { return solver.getEquationSystem().getCurrentUnknowns(); }


      VariableSet getVariables(const SourceID& source);


      VariableSet getVariables(const SourceID& source,const TypeID& type);


      VariableSet getVariables( const SourceID& source,
                                const TypeIDSet& typeSet );


      VariableSet getVariables(const SourceIDSet& sourceSet);


      VariableSet getVariables( const SourceIDSet& sourceSet,
                                const TypeID& type );


      VariableSet getVariables( const SourceIDSet& sourceSet,
                                const TypeIDSet& typeSet );


      VariableSet getVariables(const SatID& sat);


      VariableSet getVariables(const SatID& sat,const TypeID& type);


      VariableSet getVariables(const SatID& sat,const TypeIDSet& typeSet);


      VariableSet getVariables( const SourceID& source, 
                                const SatID& sat, 
                                const TypeID& type );
   
      VariableSet getVariables( const SourceID& source, 
                                const SatIDSet& satSet, 
                                 const TypeID& type );

         /// Get the current sources of the solver
      SourceIDSet getCurrentSources()
      { return solver.getEquationSystem().getCurrentSources();}


         /// Get the current variables of the solver
      VariableSet getCurrentUnknowns()
      { return solver.getEquationSystem().getCurrentUnknowns();}


         /// Get the current satellite of the solver
      SatIDSet getCurrentSats()
      { return solver.getEquationSystem().getCurrentSats();}


         /// Get solution of the interesting variables
      Vector<double> getSolution(const VariableSet& varSet);


         /// Get covariance of the interesting variables
      Matrix<double> getCovariance(const VariableSet& varSet);

         /// Method of updating the solution of the variable
      GeneralConstraint& setSolution( const Variable& variable,
                                      const double& val )
      { solver.setSolution(variable,val); return (*this); }


         /// Method of updating the covariance of the variable
      GeneralConstraint& setCovariance( const Variable& var1, 
                                        const Variable& var2,
                                        const double& cov )
      { solver.setCovariance(var1,var2,cov); return (*this); }


         /// Change the state of the filter
      GeneralConstraint& changeState( const VariableList& varList,
                                      const Matrix<double>& convertMat );


         /// Method to get the index of the interesting sat in the SatSet object
      int findIndexOfSat(const SatIDSet& satSet,const SatID& sat);

      
         /// Method to static VariableSet to a VariableList object
      void stackVariables(VariableList& varList,const VariableSet& varSet);


         /// Method to get the union of the input VariableSet objects.
      VariableSet unionVariables(const VariableSet& vs1,
                                 const VariableSet& vs2);


         /// Method to get the difference of the input VariableSet objects.
      VariableSet differenceVariables( const VariableSet& vs1,
                                       const VariableSet& vs2 );

         /// Method to get the intersection of the input VariableSet objects.
      VariableSet intersectionVariables( const VariableSet& vs1,
                                         const VariableSet& vs2 );
         
         /// Check if the satellite is a reference satellite.
      bool isRefSat(const SatID& sat);


   protected:


         /// The partner solver it work with
      SolverGeneral& solver;

         /// Object holding the map of reference satellite to source. 
      SatSourceMap refsatSourceMap;

         /// Object holding the map of source to reference satellite. 
      SourceSatMap sourceRefsatMap;


   }; // End of class 'GeneralConstraint'

}  // End of namespace gpstk

#endif   // GPSTK_GENERALCONSTRAINT_HPP
