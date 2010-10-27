#pragma ident "$Id$"

/**
 * @file SolverGeneral.hpp
 * General Solver.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2009
//
//============================================================================


#include "SolverGeneral.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int SolverGeneral::classIndex = 9600000;


      // Returns an index identifying this object.
   int SolverGeneral::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string SolverGeneral::getClassName() const
   { return "SolverGeneral"; }



      /* Explicit constructor.
       *
       * @param equationList  List of objects describing the equations
       *                      to be solved.
       */
   SolverGeneral::SolverGeneral( const std::list<Equation>& equationList )
      : firstTime(true)
   {

         // Visit each "Equation" in 'equationList' and add them to 'equSystem'
      for( std::list<Equation>::const_iterator itEq = equationList.begin();
           itEq != equationList.end();
           ++itEq )
      {
         equSystem.addEquation( (*itEq) );
      }

   }  // End of constructor 'SolverGeneral::SolverGeneral()'


      /* Returns a reference to a gnnsSatTypeValue object after
       * solving the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& SolverGeneral::Process(gnssSatTypeValue& gData)
      throw(ProcessingException)
   {

      try
      {

            // Build a gnssRinex object and fill it with data
         gnssRinex g1;
         g1.header = gData.header;
         g1.body = gData.body;

            // Call the Process() method with the appropriate input object
         Process(g1);

            // Update the original gnssSatTypeValue object with the results
         gData.body = g1.body;

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverGeneral::Process()'



      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssRinex& SolverGeneral::Process(gnssRinex& gData)
      throw(ProcessingException)
   {

      try
      {

            // Build a gnssDataMap object and fill it with data
         gnssDataMap gdsMap;
         SourceID source( gData.header.source );
         gdsMap.addGnssRinex( gData );

            // Call the Process() method with the appropriate input object,
            // and update the original gnssRinex object with the results
         Process(gdsMap);
         gData = gdsMap.getGnssRinex( source );

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverGeneral::Process()'



      /* Returns a reference to a gnssDataMap object after solving
       *  the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssDataMap& SolverGeneral::Process( gnssDataMap& gdsMap )
      throw(ProcessingException)
   {

      try
      {

            // Prepare everything before computing
         preCompute(gdsMap);


            // Call the Compute() method with the defined equation model.
            // This equation model MUST HAS BEEN previously set, usually when
            // creating the SolverPPP object with the appropriate
            // constructor.
         Compute( measVector,
                  hMatrix,
                  rMatrix );


            // Store data after computing
         postCompute(gdsMap);

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

      return gdsMap;

   }  // End of method 'SolverGeneral::Prepare()'



      /* Code to be executed before 'Compute()' method.
       *
       * @param gData    Data object holding the data.
       */
   gnssDataMap& SolverGeneral::preCompute( gnssDataMap& gdsMap )
      throw(ProcessingException)
   {

      try
      {

            // Prepare the equation system with current data
         equSystem.Prepare(gdsMap);

            // Get matrices and vectors out of equation system
            // Measurements vector (Prefit-residuals)
         measVector = equSystem.getPrefitsVector();

            // Geometry matrix
         hMatrix = equSystem.getGeometryMatrix();

            // Weights matrix
         rMatrix = equSystem.getWeightsMatrix();

            // State Transition Matrix (PhiMatrix)
         phiMatrix = equSystem.getPhiMatrix();

            // Noise covariance matrix (QMatrix)
         qMatrix = equSystem.getQMatrix();


            // Get the number of unknowns being processed
         int numUnknowns( equSystem.getTotalNumVariables() );

            // Get the set with unknowns being processed
         VariableSet unkSet( equSystem.getVarUnknowns() );

            // Feed the filter with the correct state and covariance matrix
         if(firstTime)
         {

            Vector<double> initialState(numUnknowns, 0.0);
            Matrix<double> initialErrorCovariance( numUnknowns,
                                                   numUnknowns,
                                                   0.0 );

               // Fill the initialErrorCovariance matrix

            int i(0);      // Set an index

            for( VariableSet::const_iterator itVar = unkSet.begin();
                 itVar != unkSet.end();
                 ++itVar )
            {

               initialErrorCovariance(i,i) = (*itVar).getInitialVariance();
               ++i;
            }


               // Reset Kalman filter
            kFilter.Reset( initialState, initialErrorCovariance );

               // No longer first time
            firstTime = false;

         }
         else
         {
               // Adapt the size to the current number of unknowns
            Vector<double> currentState(numUnknowns, 0.0);
            Matrix<double> currentErrorCov(numUnknowns, numUnknowns, 0.0);


               // Fill the state vector

            int i(0);      // Set an index

            for( VariableSet::const_iterator itVar = unkSet.begin();
                 itVar != unkSet.end();
                 ++itVar )
            {

               currentState(i) = stateMap[ (*itVar) ];
               ++i;
            }


               // Fill the covariance matrix

               // We need a copy of 'unkSet'
            VariableSet tempSet( unkSet );

            i = 0;         // Reset 'i' index

            for( VariableSet::const_iterator itVar1 = unkSet.begin();
                 itVar1 != unkSet.end();
                 ++itVar1 )
            {

                  // Fill the diagonal element
               currentErrorCov(i, i) = covarianceMap[ (*itVar1) ][ (*itVar1) ];

               int j(i+1);      // Set 'j' index

                  // Remove current Variable from 'tempSet'
               tempSet.erase( (*itVar1) );

               for( VariableSet::const_iterator itVar2 = tempSet.begin();
                    itVar2 != tempSet.end();
                    ++itVar2 )
               {

                     // Check if '(*itVar2)' belongs to 'covarianceMap'
                  if( covarianceMap.find( (*itVar2) ) != covarianceMap.end() )
                  {
                        // If it belongs, get element from 'covarianceMap'
                     currentErrorCov(i, j) =
                        currentErrorCov(j, i) =
                           covarianceMap[ (*itVar1) ][ (*itVar2) ];
                  }
                  else
                  {
                        // If it doesn't belong, ask for default covariance
                     currentErrorCov(i, j) =
                        currentErrorCov(j, i) = (*itVar2).getInitialVariance();
                  }

                  ++j;
               }

               ++i;

            }  // End of for( VariableSet::const_iterator itVar1 = unkSet...'


               // Reset Kalman filter to current state and covariance matrix
            kFilter.Reset( currentState, currentErrorCov );

         }  // End of 'if(firstTime)'


      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

      return gdsMap;

   }  // End of method 'SolverGeneral::preCompute()'



      // Compute the solution of the given equations set.
      //
      // @param prefitResiduals   Vector of prefit residuals
      // @param designMatrix      Design matrix for equation system
      // @param weightMatrix      Matrix of weights
      //
      // \warning A typical Kalman filter works with the measurements noise
      // covariance matrix, instead of the matrix of weights. Beware of this
      // detail, because this method uses the later.
      //
      // @return
      //  0 if OK
      //  -1 if problems arose
      //
   int SolverGeneral::Compute( const Vector<double>& prefitResiduals,
                               const Matrix<double>& designMatrix,
                               const Matrix<double>& weightMatrix )
      throw(InvalidSolver)
   {

         // By default, results are invalid
      valid = false;

      if (!(weightMatrix.isSquare()))
      {
         InvalidSolver e("Weight matrix is not square");
         GPSTK_THROW(e);
      }

      int wRow = static_cast<int>(weightMatrix.rows());
      int pRow = static_cast<int>(prefitResiduals.size());
      if (!(wRow==pRow))
      {
         InvalidSolver e("prefitResiduals size does not match dimension of \
weightMatrix");
         GPSTK_THROW(e);
      }

      int gRow = static_cast<int>(designMatrix.rows());
      if (!(gRow==pRow))
      {
         InvalidSolver e("prefitResiduals size does not match dimension \
of designMatrix");
         GPSTK_THROW(e);
      }

      if (!(phiMatrix.isSquare()))
      {
         InvalidSolver e("phiMatrix is not square");
         GPSTK_THROW(e);
      }

         // Get the number of unknowns being processed
      int numUnknowns( equSystem.getTotalNumVariables() );

      int phiRow = static_cast<int>(phiMatrix.rows());
      if (!(phiRow==numUnknowns))
      {
         InvalidSolver e("Number of unknowns does not match dimension \
of phiMatrix");
         GPSTK_THROW(e);
      }

      if (!(qMatrix.isSquare()))
      {
         InvalidSolver e("qMatrix is not square");
         GPSTK_THROW(e);
      }

      int qRow = static_cast<int>(qMatrix.rows());
      if (!(qRow==numUnknowns))
      {
         InvalidSolver e("Number of unknowns does not match dimension \
of qMatrix");
         GPSTK_THROW(e);
      }

         // After checking sizes, let's invert the matrix of weights in order
         // to get the measurements noise covariance matrix, which is what we
         // use in the "SimpleKalmanFilter" class
      Matrix<double> measNoiseMatrix;

      try
      {
         measNoiseMatrix = inverseChol(weightMatrix);
      }
      catch(...)
      {
         InvalidSolver e("Correct(): Unable to compute measurements noise \
covariance matrix.");
         GPSTK_THROW(e);
      }


      try
      {

            // Call the Kalman filter object.
         kFilter.Compute( phiMatrix,
                          qMatrix,
                          prefitResiduals,
                          designMatrix,
                          measNoiseMatrix );

      }
      catch(InvalidSolver& e)
      {
         GPSTK_RETHROW(e);
      }

         // Store the solution
      solution = kFilter.xhat;

         // Store the covariance matrix of the solution
      covMatrix = kFilter.P;

         // Compute the postfit residuals Vector
      postfitResiduals = prefitResiduals - (designMatrix * solution);

         // If everything is fine so far, then the results should be valid
      valid = true;

      return 0;

   }  // End of method 'SolverGeneral::Compute()'



      /* Code to be executed after 'Compute()' method.
       *
       * @param gData    Data object holding the data.
       */
   gnssDataMap& SolverGeneral::postCompute( gnssDataMap& gdsMap )
      throw(ProcessingException)
   {

      try
      {

            // Clean up values in 'stateMap' and 'covarianceMap'
         stateMap.clear();
         covarianceMap.clear();


            // Get the set with unknowns being processed
         VariableSet unkSet( equSystem.getVarUnknowns() );


            // Store values of current state

         int i(0);      // Set an index

         for( VariableSet::const_iterator itVar = unkSet.begin();
              itVar != unkSet.end();
              ++itVar )
         {

            stateMap[ (*itVar) ] = solution(i);
            ++i;
         }


            // Store values of covariance matrix

            // We need a copy of 'unkSet'
         VariableSet tempSet( unkSet );

         i = 0;         // Reset 'i' index

         for( VariableSet::const_iterator itVar1 = unkSet.begin();
              itVar1 != unkSet.end();
              ++itVar1 )
         {

               // Fill the diagonal element
            covarianceMap[ (*itVar1) ][ (*itVar1) ] = covMatrix(i, i);

            int j(i+1);      // Set 'j' index

               // Remove current Variable from 'tempSet'
            tempSet.erase( (*itVar1) );

            for( VariableSet::const_iterator itVar2 = tempSet.begin();
                 itVar2 != tempSet.end();
                 ++itVar2 )
            {

               covarianceMap[ (*itVar1) ][ (*itVar2) ] = covMatrix(i, j);

               ++j;
            }

            ++i;

         }  // End of for( VariableSet::const_iterator itVar1 = unkSet...'


            // Store the postfit residuals in the GNSS Data Structure

            // We need the list of equations being processed
         std::list<Equation> equList( equSystem.getCurrentEquationsList() );

         i = 0;         // Reset 'i' index

            // Visit each equation in "equList"
         for( std::list<Equation>::const_iterator itEq = equList.begin();
              itEq != equList.end();
              ++itEq )
         {

               // Get the TypeID of the residuals. The original type by default.
            TypeID residualType( (*itEq).header.indTerm.getType() );

               // Prefits are treated different
            if( residualType == TypeID::prefitC )
            {
               residualType = TypeID::postfitC;
            }
            else
            {
               if( residualType == TypeID::prefitL )
               {
                  residualType = TypeID::postfitL;
               }
            }

            gdsMap.insertValue( (*itEq).header.equationSource,
                                (*itEq).header.equationSat,
                                residualType,
                                postfitResiduals(i) );

            ++i;  // Increment counter

         }  // End of 'for( std::list<Equation>::const_iterator itEq = ...'

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

      return gdsMap;

   }  // End of method 'SolverGeneral::postCompute()'



      /* Returns the solution associated to a given Variable.
       *
       * @param variable    Variable object solution we are looking for.
       */
   double SolverGeneral::getSolution( const Variable& variable ) const
      throw(InvalidRequest)
   {

         // Look the variable inside the state map
      VariableDataMap::const_iterator it( stateMap.find( variable ) );

         // Check if the provided Variable exists in the solution. If not,
         // an InvalidSolver exception will be issued.
      if( it == stateMap.end() )
      {
         InvalidRequest e("Variable not found in solution vector.");
         GPSTK_THROW(e);
      }

         // Return value
      return (*it).second;

   }  // End of method 'SolverGeneral::getSolution()'



      /* Returns the solution associated to a given TypeID.
       *
       * @param type    TypeID of the solution we are looking for.
       *
       * \warning In the case the solution contains more than one variable
       * of this type, only the first one will be returned.
       */
   double SolverGeneral::getSolution( const TypeID& type ) const
      throw(InvalidRequest)
   {

         // Declare an iterator for 'stateMap' and go to the first element
      VariableDataMap::const_iterator it = stateMap.begin();

         // Look for a variable with the same type
      while( (*it).first.getType() != type &&
             it != stateMap.end() )
      {
         ++it;
      }

         // If the same type is not found, throw an exception
      if( it == stateMap.end() )
      {
         InvalidRequest e("Type not found in solution vector.");
         GPSTK_THROW(e);
      }

         // Else, return the corresponding value
      return (*it).second;

   }  // End of method 'SolverGeneral::getSolution()'



      /* Returns the solution associated to a given TypeID and SourceID.
       *
       * @param type    TypeID of the solution we are looking for.
       * @param source  SourceID of the solution we are looking for.
       *
       * \warning In the case the solution contains more than one variable
       * of this type, only the first one will be returned.
       */
   double SolverGeneral::getSolution( const TypeID& type,
                                      const SourceID& source ) const
      throw(InvalidRequest)
   {

         // Declare an iterator for 'stateMap' and go to the first element
      VariableDataMap::const_iterator it = stateMap.begin();

         // Look for a variable with the same type and source
      while( !( (*it).first.getType()   == type &&
                (*it).first.getSource() == source ) &&
             it != stateMap.end() )
      {
         ++it;
      }

         // If it is not found, throw an exception
      if( it == stateMap.end() )
      {
         InvalidRequest e("Type and source not found in solution vector.");
         GPSTK_THROW(e);
      }

         // Else, return the corresponding value
      return (*it).second;

   }  // End of method 'SolverGeneral::getSolution()'



      /* Returns the solution associated to a given TypeID, SourceID and
       * SatID.
       *
       * @param type    TypeID of the solution we are looking for.
       * @param source  SourceID of the solution we are looking for.
       * @param sat     SatID of the solution we are looking for.
       *
       * \warning In the case the solution contains more than one variable
       * of this type, only the first one will be returned.
       */
   double SolverGeneral::getSolution( const TypeID& type,
                                      const SourceID& source,
                                      const SatID& sat ) const
      throw(InvalidRequest)
   {

         // Declare an iterator for 'stateMap' and go to the first element
      VariableDataMap::const_iterator it = stateMap.begin();

         // Look for a variable with the same type, source and satellite
      while( !( (*it).first.getType()      == type    &&
                (*it).first.getSource()    == source  &&
                (*it).first.getSatellite() == sat        ) &&
             it != stateMap.end() )
      {
         ++it;
      }

         // If it is not found, throw an exception
      if( it == stateMap.end() )
      {
         InvalidRequest e("Type, source and SV not found in solution vector.");
         GPSTK_THROW(e);
      }

         // Else, return the corresponding value
      return (*it).second;

   }  // End of method 'SolverGeneral::getSolution()'



      /* Returns the variance associated to a given Variable.
       *
       * @param variable    Variable object variance we are looking for.
       */
   double SolverGeneral::getVariance(const Variable& variable)
      throw(InvalidRequest)
   {

         // Check if the provided Variable exists in the solution. If not,
         // an InvalidSolver exception will be issued.
      if( stateMap.find( variable ) == stateMap.end() )
      {
         InvalidRequest e("Variable not found in covariance matrix.");
         GPSTK_THROW(e);
      }

         // Return value
      return covarianceMap[ variable ][ variable ];

   }  // End of method 'SolverGeneral::getVariance()'



      /* Returns the variance associated to a given TypeID.
       *
       * @param type    TypeID of the variance we are looking for.
       *
       * \warning In the case the solution contains more than one variable
       * of this type, only the first one will be returned.
       */
   double SolverGeneral::getVariance(const TypeID& type)
      throw(InvalidRequest)
   {

         // Declare an iterator for 'covarianceMap' and go to the first element
      std::map<Variable, VariableDataMap >::const_iterator it
                                                      = covarianceMap.begin();

         // Look for a variable with the same type
      while( (*it).first.getType() != type &&
             it != covarianceMap.end() )
      {
         ++it;
      }

         // If the same type is not found, throw an exception
      if( it == covarianceMap.end() )
      {
         InvalidRequest e("Type not found in covariance matrix.");
         GPSTK_THROW(e);
      }

         // Else, return the corresponding value
      return covarianceMap[ (*it).first ][ (*it).first ];

   }  // End of method 'SolverGeneral::getVariance()'



}  // End of namespace gpstk
