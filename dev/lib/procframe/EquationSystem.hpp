#pragma ident "$Id$"

/**
 * @file EquationSystem.hpp
 * Class to define and handle complex equation systems for solvers.
 */

#ifndef GPSTK_EQUATIONSYSTEM_HPP
#define GPSTK_EQUATIONSYSTEM_HPP

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



#include "DataStructures.hpp"
#include "StochasticModel.hpp"
#include "Equation.hpp"



namespace gpstk
{

      /// Thrown when attempting to use an invalid EquationSystem
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(InvalidEquationSystem, gpstk::Exception);


      /** @addtogroup DataStructures */
      //@{


      /** This class defines and handles complex equation systems for solvers,
       *  generating the Vectors and Matrices they will need, including:
       *
       *    \li Measurements Vector (prefit residuals).
       *    \li Geometry matrix.
       *    \li Equation weights matrix.
       *    \li State Transition Matrix (PhiMatrix).
       *    \li Process noise covariance matrix (QMatrix).
       *
       * In addition, this class will provide important/interesting information
       * such as number of variables, satellites and data sources being
       * processed.
       *
       * In order to achieve this, the 'EquationSystem' class will start from a
       * list of 'Equation descriptions' and the currently available GNSS data.
       * From there, it will deduce the unknowns corresponding to each data
       * source as well as the specific equations.
       *
       * A typical way to use this class follows, showing how to set up an
       * object to perform "Precise Point Positioning" (PPP):
       *
       * @code
       *
       *      // DEFINE VARIABLES
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
       *      // DESCRIBE EQUATIONS
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
       *      // SETUP EQUATION SYSTEM
       *
       *      // Create 'EquationSystem' object
       *   EquationSystem eqSystem;
       *
       *      // Add equation descriptions
       *   eqSystem.addEquation(equPC);
       *   eqSystem.addEquation(equLC);
       *
       * @endcode
       *
       * In this way, rather complex processing strategies may be set up in a
       * handy and flexible way.
       *
       * \warning Please be aware that this class requires a significant amount
       * of overhead. Therefore, if your priority is execution speed you should
       * either use the already provided 'purpose-specific' solvers (like
       * 'SolverPPP' or hand-code your own class. For new processing strategies
       * you should balance the importance of machine time (extra overhead)
       * versus researcher time (writing a new solver).
       *
       * @sa Variable.hpp, Equation.hpp.
       *
       */
   class EquationSystem
   {
   public:

         /// Default constructor
      EquationSystem()
         : isPrepared(false)
      {};


         /** Add a new equation to be managed.
          *
          * @param equation   Equation object to be added.
          */
      virtual EquationSystem& addEquation( const Equation& equation );


         /** Remove an Equation being managed. In this case the equation is
          *  identified by its independent term.
          *
          * @param indterm  Variable object of the equation independent term
          *                 (measurement type).
          *
          * \warning All Equations with the same independent term will be
          *          erased.
          */
      virtual EquationSystem& removeEquation( const Variable& indterm );


         /// Remove all Equation objects from this EquationSystem.
      virtual EquationSystem& clearEquations();


         /** Prepare this object to carry out its work.
          *
          * @param gData   GNSS data structure (GDS).
          *
          */
      virtual EquationSystem& Prepare( gnssSatTypeValue& gData )
      { gnssRinex gds(gData); return ( Prepare(gds) ); };


         /** Prepare this object to carry out its work.
          *
          * @param gData   GNSS data structure (GDS).
          *
          */
      virtual EquationSystem& Prepare( gnssRinex& gData );


         /** Prepare this object to carry out its work.
          *
          * @param gdsMap     Map of GNSS data structures (GDS), indexed
          *                   by SourceID.
          *
          */
      virtual EquationSystem& Prepare( gnssDataMap& gdsMap );


         /** Return the TOTAL number of variables being processed.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystem exception.
          */
      virtual int getTotalNumVariables() const
         throw(InvalidEquationSystem);


         /** Return the set containing all variables being processed.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystem exception.
          */
      virtual VariableSet getVarUnknowns() const
         throw(InvalidEquationSystem);


         /** Return the CURRENT number of variables, given the current equation
          *  system definition and the GDS's involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystem exception.
          */
      virtual int getCurrentNumVariables() const
         throw(InvalidEquationSystem);


         /** Return the set containing variables being currently processed.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystem exception.
          */
      virtual VariableSet getCurrentUnknowns() const
         throw(InvalidEquationSystem);


         /** Return the CURRENT number of sources, given the current equation
          *  system definition and the GDS's involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystem exception.
          */
      virtual int getCurrentNumSources() const
         throw(InvalidEquationSystem);


         /** Return the set containing sources being currently processed.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystem exception.
          */
      virtual SourceIDSet getCurrentSources() const
         throw(InvalidEquationSystem);


         /** Return the CURRENT number of satellites, given the current equation
          *  system definition and the GDS's involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystem exception.
          */
      virtual int getCurrentNumSats() const
         throw(InvalidEquationSystem);


         /** Return the set containing satellites being currently processed.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystem exception.
          */
      virtual SatIDSet getCurrentSats() const
         throw(InvalidEquationSystem);


         /** Get prefit residuals GPSTk Vector, given the current equation
          *  system definition and the GDS' involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystem exception.
          */
      virtual Vector<double> getPrefitsVector() const
         throw(InvalidEquationSystem);


         /** Get geometry matrix, given the current equation system definition
          *  and the GDS' involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystem exception.
          */
      virtual Matrix<double> getGeometryMatrix() const
         throw(InvalidEquationSystem);


         /** Get weights matrix, given the current equation system definition
          *  and the GDS' involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystem exception.
          */
      virtual Matrix<double> getWeightsMatrix() const
         throw(InvalidEquationSystem);


         /** Get the State Transition Matrix (PhiMatrix), given the current
          *  equation system definition and the GDS' involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystem exception.
          */
      virtual Matrix<double> getPhiMatrix() const
         throw(InvalidEquationSystem);


         /** Get the Process Noise Covariance Matrix (QMatrix), given the
          *  current equation system definition and the GDS' involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquationSystem exception.
          */
      virtual Matrix<double> getQMatrix() const
         throw(InvalidEquationSystem);


         /// Get the number of equation descriptions being currently processed.
      virtual int getEquationDefinitionNumber() const
      { return equationDescriptionList.size(); };


         /// Get the list of current equations.
      virtual std::list<Equation> getCurrentEquationsList() const
      { return currentEquationsList; };


         /// Destructor
      virtual ~EquationSystem() {};


   private:


         /// List containing the DESCRIPTIONS of Equation objects.
      std::list<Equation> equationDescriptionList;

         /// List of current equations
      std::list<Equation> currentEquationsList;

         /// Global set of unknowns
      VariableSet varUnknowns;

         /// Current set of unknowns
      VariableSet currentUnknowns;

         /// Old set of unknowns
      VariableSet oldUnknowns;

         /// Whether or not this EquationSystem is ready to be used
      bool isPrepared;

         /// Set containing all sources being currently processed
      SourceIDSet currentSourceSet;

         /// Set containing satellites being currently processed
      SatIDSet currentSatSet;

         /// State Transition Matrix (PhiMatrix)
      Matrix<double> phiMatrix;

         /// Process noise covariance matrix (QMatrix)
      Matrix<double> qMatrix;

         /// Geometry matrix
      Matrix<double> hMatrix;

         /// Weights matrix
      Matrix<double> rMatrix;

         /// Measurements vector (Prefit-residuals)
      Vector<double> measVector;

         /// Get current sources (SourceID's) and satellites (SatID's)
      void prepareCurrentSourceSat( gnssDataMap& gdsMap );

         /// Prepare set of current unknowns and list of current equations
      VariableSet prepareCurrentUnknownsAndEquations( gnssDataMap& gdsMap );

         /// Compute phiMatrix and qMatrix
      void getPhiQ( const gnssDataMap& gdsMap );

         /// Compute prefit residuals vector
      void getPrefit( gnssDataMap& gdsMap );

         /// Compute hMatrix and rMatrix
      void getGeometryWeights( gnssDataMap& gdsMap );

         /// General white noise stochastic model
      static WhiteNoiseModel whiteNoiseModel;


   }; // End of class 'EquationSystem'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_EQUATIONSYSTEM_HPP
