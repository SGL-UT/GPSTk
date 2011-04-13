#pragma ident "$Id$"

/**
 * @file SolverGeneral.hpp
 * General Solver.
 */

#ifndef GPSTK_SOLVERGENERAL_HPP
#define GPSTK_SOLVERGENERAL_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2009
//
//============================================================================


#include "SolverBase.hpp"
#include "TypeID.hpp"
#include "ProcessingClass.hpp"
#include "EquationSystem.hpp"
#include "StochasticModel.hpp"
#include "SimpleKalmanFilter.hpp"


namespace gpstk
{

      /** @addtogroup GPSsolutions */
      /// @ingroup math


         /// Handy type definition
      typedef std::map<Variable, double> VariableDataMap;


      //@{

      /** This class is an Extended Kalman Filter (EKF) implementation that
       *  is run-time programmable, making it extremely flexible.
       *
       * SolverGeneral is programmed using class "EquationSystem", that defines
       * a set of rules to "tune" the solver to solve a specific problem. In
       * turn, "EquationSystem" relies in other classes like "Variable" and
       * "Equation", responsible of setting rules such as TypeID's, SourceID's,
       * stochastic models, etc.
       *
       * In this way, complex multi-station and/or hybrid GNSS-INS problems can
       * be tackled with relatively few code lines, encouraging code
       * reusability.
       *
       * A typical way to use this class follows, showing how to set up a
       * SolverGeneral object to perform "Precise Point Positioning" (PPP):
       *
       * @code
       *      // SETTING THE RULES: DEFINE VARIABLES
       *
       *      // Declare stochastic models to be used
       *   StochasticModel coordinatesModel;
       *   TropoRandomWalkModel tropoModel;
       *   PhaseAmbiguityModel ambiModel;
       *
       *      // These variables are, by default, SourceID-indexed
       *   Variable dx( TypeID::dx, &coordinatesModel, true, false, 100.0 );
       *
       *   Variable dy( TypeID::dy, &coordinatesModel );
       *   dy.setInitialVariance( 100.0 );     // Equivalent to 'dx' setup
       *
       *   Variable dz( TypeID::dz, &coordinatesModel );
       *   dz.setInitialVariance( 100.0 );
       *
       *   Variable cdt( TypeID::cdt );
       *   cdt.setDefaultForced(true);   // Force default coefficient (1.0)
       *
       *   Variable tropo( TypeID::wetMap, &tropoModel );
       *   tropo.setInitialVariance( 25.0 );
       *
       *
       *      // The following variable is, SourceID and SatID-indexed
       *   Variable ambi( TypeID::BLC, &ambiModel, true, true );
       *   ambi.setDefaultForced(true);   // Force default coefficient (1.0)
       *
       *      // This will be the independent term for code equation
       *   Variable prefitC( TypeID::prefitC );
       *
       *      // This will be the independent term for phase equation
       *   Variable prefitL( TypeID::prefitL );
       *
       *
       *      // SETTING THE RULES: DESCRIBE EQUATIONS
       *
       *      // Define Equation object for code equations, and add variables
       *   Equation equPC( prefitC );
       *
       *   equPC.addVariable(dx);
       *   equPC.addVariable(dy);
       *   equPC.addVariable(dz);
       *   equPC.addVariable(cdt);
       *   equPC.addVariable(tropo);
       *
       *      // Define Equation object for phase equations, and add variables
       *   Equation equLC( prefitL );
       *
       *   equLC.addVariable(dx);
       *   equLC.addVariable(dy);
       *   equLC.addVariable(dz);
       *   equLC.addVariable(cdt);
       *   equLC.addVariable(tropo);
       *   equLC.addVariable(ambi);       // This variable is for phase only
       *
       *      // Phase equations should have higher relative weight
       *   equLC.setWeight(10000.0);     // 100.0 * 100.0
       *
       *
       *      // SETTING THE RULES: SETUP EQUATION SYSTEM
       *
       *      // Create 'EquationSystem' object
       *   EquationSystem eqSystem;
       *
       *      // Add equation descriptions
       *   eqSystem.addEquation(equPC);
       *   eqSystem.addEquation(equLC);
       *
       *
       *      // SETUP "SolverGeneral" OBJECT
       *
       *      // Create 'SolverGeneral' object and add equation system
       *   SolverGeneral solver( eqSystem );
       *
       * @endcode
       *
       * The "SolverGeneral" object is then ready to be fed with data
       * encapsulated in an appropriate GDS. Take notice that for problems
       * involving multiple epochs and/or multiple stations the recommended
       * GDS is "gnssDataMap", which "SolverGeneral" fully supports.
       *
       * \warning Please be aware that this class requires a significant amount
       * of overhead. Therefore, if your priority is execution speed you should
       * either use the already provided 'purpose-specific' solvers (like
       * 'SolverPPP' or hand-code your own class. For new processing strategies
       * you should balance the importance of machine time (extra overhead)
       * versus researcher time (writing a new solver).
       *
       * \warning "SolverGeneral" is based on an Extended Kalman filter, and
       * Kalman filters are objets that store their internal state, so you MUST
       * NOT use the SAME object to process DIFFERENT data streams.
       *
       * @sa Variable.hpp, Equation.hpp, EquationSystem.hpp.
       *
       */
   class SolverGeneral : public SolverBase, public ProcessingClass
   {
   public:

         /** Explicit constructor.
          *
          * @param equation      Object describing the equations to be solved.
          */
      SolverGeneral( const Equation& equation ) : firstTime(true)
      { equSystem.addEquation(equation); };


         /** Explicit constructor.
          *
          * @param equationList  List of objects describing the equations
          *                      to be solved.
          */
      SolverGeneral( const std::list<Equation>& equationList );


         /** Explicit constructor.
          *
          * @param equationSys         Object describing an equation system to
          *                            be solved.
          */
      SolverGeneral( const EquationSystem& equationSys ) : firstTime(true)
      { equSystem = equationSys; };


         /// Get a copy of the equation system being solved.
      virtual EquationSystem getEquationSystem() const
      { return equSystem; };


         /** Set the equation system to be solved.
          *
          * @param equationSys         Object describing an equation system to
          *                            be solved.
          */
      virtual SolverGeneral& setEquationSystem(
                                             const EquationSystem& equationSys )
      { equSystem = equationSys; return (*this); };


         /** Add a new equation to be managed.
          *
          * @param equation   Equation object to be added.
          */
      virtual SolverGeneral& addEquation( const Equation& equation )
      { equSystem.addEquation(equation); return (*this); };


         /** Remove an Equation being managed. In this case the equation is
          *  identified by its independent term.
          *
          * @param indterm  Variable object of the equation independent term
          *                 (measurement type).
          *
          * \warning All Equations with the same independent term will be
          *          erased.
          */
      virtual SolverGeneral& removeEquation( const Variable& indterm )
      { equSystem.removeEquation(indterm); return (*this); };


         /** Remove all Equation objects currently defined.
          *
          * \warning This method will left this SolverGeneral method in an
          *          unstable state. You MUST add at least one equation
          *          definition for this object to work.
          */
      virtual SolverGeneral& clearEquations()
      { equSystem.clearEquations(); return (*this); };


         /// This method resets the filter, setting all variance values in
         /// covariance matrix to a very high level.
      virtual SolverGeneral& reset(void)
      { firstTime = true; return (*this); };


         /** Returns a reference to a gnnsSatTypeValue object after
          *  solving the previously defined equation system.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process( gnssSatTypeValue& gData )
         throw(ProcessingException);


         /** Returns a reference to a gnnsRinex object after solving
          *  the previously defined equation system.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process( gnssRinex& gData )
         throw(ProcessingException);


         /** Returns a reference to a gnssDataMap object after solving
          *  the previously defined equation system.
          *
          * @param gdsMap    Data object holding the data.
          */
      virtual gnssDataMap& Process( gnssDataMap& gdsMap )
         throw(ProcessingException);


         /** Returns the solution associated to a given Variable.
          *
          * @param variable    Variable object solution we are looking for.
          */
      virtual double getSolution( const Variable& variable ) const
         throw(InvalidRequest);


         /** Returns the solution associated to a given TypeID.
          *
          * @param type    TypeID of the solution we are looking for.
          *
          * \warning In the case the solution contains more than one variable
          * of this type, only the first one will be returned.
          */
      virtual double getSolution( const TypeID& type ) const
         throw(InvalidRequest);


         /** Returns the solution associated to a given TypeID and SourceID.
          *
          * @param type    TypeID of the solution we are looking for.
          * @param source  SourceID of the solution we are looking for.
          *
          * \warning In the case the solution contains more than one variable
          * of this type, only the first one will be returned.
          */
      virtual double getSolution( const TypeID& type,
                                  const SourceID& source ) const
         throw(InvalidRequest);


         /** Returns the solution associated to a given TypeID, SourceID and
          *  SatID.
          *
          * @param type    TypeID of the solution we are looking for.
          * @param source  SourceID of the solution we are looking for.
          * @param sat     SatID of the solution we are looking for.
          *
          * \warning In the case the solution contains more than one variable
          * of this type, only the first one will be returned.
          */
      virtual double getSolution( const TypeID& type,
                                  const SourceID& source,
                                  const SatID& sat ) const
         throw(InvalidRequest);


         /** Returns the variance associated to a given Variable.
          *
          * @param variable    Variable object variance we are looking for.
          */
      virtual double getVariance( const Variable& variable )
         throw(InvalidRequest);


         /** Returns the variance associated to a given TypeID.
          *
          * @param type    TypeID of the variance we are looking for.
          *
          * \warning In the case the solution contains more than one variable
          * of this type, only the first one will be returned.
          */
      virtual double getVariance( const TypeID& type )
         throw(InvalidRequest);


         /// Get the State Transition Matrix (phiMatrix)
      virtual Matrix<double> getPhiMatrix(void) const
      { return phiMatrix; };


         /// Get the Noise covariance matrix (QMatrix)
      virtual Matrix<double> getQMatrix(void) const
      { return qMatrix; };


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor.
      virtual ~SolverGeneral() {};


   protected:


         /// Equation system
      EquationSystem equSystem;


         /** Code to be executed before 'Compute()' method.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssDataMap& preCompute( gnssDataMap& gdsMap )
         throw(ProcessingException);


         /** Code to be executed after 'Compute()' method.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssDataMap& postCompute( gnssDataMap& gdsMap )
         throw(ProcessingException);


   private:


         /// State Transition Matrix (PhiMatrix)
      Matrix<double> phiMatrix;


         /// Noise covariance matrix (QMatrix)
      Matrix<double> qMatrix;


         /// Geometry matrix
      Matrix<double> hMatrix;


         /// Weights matrix
      Matrix<double> rMatrix;


         /// Measurements vector (Prefit-residuals)
      Vector<double> measVector;


         /// Map holding state information
      VariableDataMap stateMap;


         /// Map holding covariance information
      std::map<Variable, VariableDataMap > covarianceMap;


         /// General Kalman filter object
      SimpleKalmanFilter kFilter;


         /// Boolean indicating if this filter was run at least once
      bool firstTime;


         /// Initial index assigned to this class.
      static int classIndex;

         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


         // Do not allow the use of the default constructor.
      SolverGeneral();


         /** Compute the solution of the given equations set.
          *
          * @param prefitResiduals   Vector of prefit residuals
          * @param designMatrix      Design matrix for the equation system
          * @param weightMatrix      Matrix of weights
          *
          * \warning A typical Kalman filter works with the measurements noise
          * covariance matrix, instead of the matrix of weights. Beware of this
          * detail, because this method uses the later.
          *
          * @return
          *  0 if OK
          *  -1 if problems arose
          */
      virtual int Compute( const Vector<double>& prefitResiduals,
                           const Matrix<double>& designMatrix,
                           const Matrix<double>& weightMatrix )
         throw(InvalidSolver);


   }; // End of class 'SolverGeneral'

      //@}

}  // End of namespace gpstk
#endif   // GPSTK_SOLVERGENERAL_HPP
