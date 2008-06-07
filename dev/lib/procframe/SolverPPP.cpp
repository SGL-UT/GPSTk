#pragma ident "$Id$"

/**
 * @file SolverPPP.cpp
 * Class to compute the PPP Solution.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008
//
//============================================================================


#include "SolverPPP.hpp"
#include "MatrixFunctors.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int SolverPPP::classIndex = 8100000;


      // Returns an index identifying this object.
   int SolverPPP::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string SolverPPP::getClassName() const
   { return "SolverPPP"; }


      // Constant stochastic model
   StochasticModel SolverPPP::constantModel;


      // White noise stochastic model
   WhiteNoiseModel SolverPPP::whitenoiseModel;


      // Random Walk stochastic model
   RandomWalkModel SolverPPP::rwalkModel(3e-8);


      // Phase biases stochastic model (constant + white noise)
   PhaseAmbiguityModel SolverPPP::biasModel;


      // Total number of satellites
   int SolverPPP::totalSVs(32);


      /// Initializing method.
   void SolverPPP::Init(void)
   {

         // Total number of unknowns is defined unknowns + total number of SVs
      numUnknowns = defaultEqDef.body.size() + totalSVs;

      Vector<double> initialState(numUnknowns, 0.0);
      Matrix<double> initialErrorCovariance(numUnknowns, numUnknowns, 0.0);
         // Fill the initialErrorCovariance matrix
         // First, the zenital wet tropospheric delay
      initialErrorCovariance(0,0) = 0.25;          // (0.5 m)**2
         // Second, the coordinates
      for (int i=1; i<4; i++)
      {
         initialErrorCovariance(i,i) = 10000.0;    // (100 m)**2
      }
         // Third, the receiver clock
      initialErrorCovariance(4,4) = 9.0e10;        // (300 km)**2
         // Finally, the phase biases
      for (int i=5; i<numUnknowns; i++)
      {
         initialErrorCovariance(i,i) = 4.0e16;     // (20000 km)**2
      }


      kFilter.Reset( initialState, initialErrorCovariance );


         // Pointer to default stochastic model for troposphere (random walk)
      pTropoStoModel = &rwalkModel;

         // Pointer to default coordinates stochastic model (constant)
      pCoordStoModel = &constantModel;

         // Pointer to default receiver clock stochastic model (white noise)
      pClockStoModel = &whitenoiseModel;

         // Pointer to stochastic model for phase biases
      pBiasStoModel  = &biasModel;

         // Set default factor that multiplies the phase weights
      weightFactor = 1000.0;

      solution.resize(numUnknowns);
   }


      /* Common constructor.
       *
       * @param useNEU   If true, will compute dLat, dLon, dH coordinates;
       *                 if false (the default), will compute dx, dy, dz.
       */
   SolverPPP::SolverPPP(bool useNEU)
   {
         // Set the equation system structure
      setNEU(useNEU);

         // Set the class index
      setIndex();

         // Call initializing method
      Init();
   }


      /* Compute the solution of the given equations set.
       *
       * @param prefitResiduals   Vector of prefit residuals
       * @param designMatrix      Design matrix for the equation system
       * @param weightVector      Vector of weights assigned to each
       *                          satellite.
       *
       * \warning A typical Kalman filter works with the measurements noise
       * covariance matrix, instead of the vector of weights. Beware of this
       * detail, because this method uses the later.
       *
       * @return
       *  0 if OK
       *  -1 if problems arose
       */
   int SolverPPP::Compute( const Vector<double>& prefitResiduals,
                           const Matrix<double>& designMatrix,
                           const Vector<double>& weightVector )
      throw(InvalidSolver)
   {
         // By default, results are invalid
      valid = false;

         // First, check that everyting has a proper size
      int wSize = static_cast<int>(weightVector.size());
      int pSize = static_cast<int>(prefitResiduals.size());
      if (!(wSize==pSize))
      {
         InvalidSolver e("prefitResiduals size does not match dimension \
of weightVector");
         GPSTK_THROW(e);
      }

      Matrix<double> wMatrix(wSize,wSize,0.0);  // Declare a weight matrix

         // Fill the weight matrix diagonal with the content of 
         // the weight vector
      for (int i=0; i<wSize; i++)
      {
         wMatrix(i,i) = weightVector(i);
      }

         // Call the more general SolverPPP::Compute() method
      return SolverPPP::Compute( prefitResiduals,
                                 designMatrix,
                                 wMatrix );
   }


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
   int SolverPPP::Compute( const Vector<double>& prefitResiduals,
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

      int phiRow = static_cast<int>(phiMatrix.rows());
      if (!(phiRow==numUnknowns))
      {
         InvalidSolver e("prefitResiduals size does not match dimension \
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
         InvalidSolver e("prefitResiduals size does not match dimension \
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

   }  // end SolverPPP::Compute()



      /* Returns a reference to a gnnsSatTypeValue object after 
       * solving the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& SolverPPP::Process(gnssSatTypeValue& gData)
      throw(InvalidSolver)
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


      /* Returns a reference to a gnnsRinex object after solving 
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssRinex& SolverPPP::Process(gnssRinex& gData)
      throw(InvalidSolver)
   {
         // Number of measurements is twice the number of visible satellites
      int numSV(gData.numSats());
      numMeas = 2 * numSV;

         // State Transition Matrix (PhiMatrix)
      phiMatrix.resize(numUnknowns, numUnknowns, 0.0);

         // Noise covariance matrix (QMatrix)
      qMatrix.resize(numUnknowns, numUnknowns, 0.0);

         // Geometry matrix
      hMatrix.resize(numMeas, numUnknowns, 0.0);

         // Weights matrix
      rMatrix.resize(numMeas, numMeas, 0.0);

         // Measurements vector (Prefit-residuals)
      measVector.resize(numMeas, 0.0);

         // Build the vector of measurements: Code + phase
      Vector<double> prefitC(gData.getVectorOfTypeID(defaultEqDef.header));
      Vector<double> prefitL(gData.getVectorOfTypeID(TypeID::prefitL));
      for (int i= 0; i<numSV; i++)
      {
         measVector(i) = prefitC(i);
         measVector(i+numSV) = prefitL(i);
      }


         // Generate the appropriate weights matrix
         // Try to extract weights from GDS
      satTypeValueMap dummy(gData.body.extractTypeID(TypeID::weight));
      int nW(dummy.numSats());   // Count the number of satellites with weights
      for (int i= 0; i<numSV; i++)
      {
         if (nW == numSV)   // Check if weights match
         {
            Vector<double>
               weightsVector(gData.getVectorOfTypeID(TypeID::weight));
            rMatrix(i,i) = weightsVector(i);
            rMatrix(i+numSV,i+numSV) = weightsVector(i)*weightFactor;
         }
         else  // If weights don't match, assign generic weights
         {     // Phases weights are bigger
            rMatrix(i,i) = 1.0;
            rMatrix(i+numSV,i+numSV) = 1.0*weightFactor;
         }
      }

         // Generate the corresponding geometry/design matrix
      Matrix<double> dMatrix(gData.body.getMatrixOfTypes(defaultEqDef.body));
         // First, fill the coefficients related to tropo, coord and clock
      for (int i= 0; i<numSV; i++)
      {
         for (int j= 0; j<(int)defaultEqDef.body.size(); j++)
         {
            hMatrix(i,j) = dMatrix(i,j);
            hMatrix(i+numSV,j) = dMatrix(i,j);
         }
      }
         // Then, fill the coefficients related to phase biases
         // Get a set with all satellites present in this GDS
      SatIDSet tempSat(gData.body.getSatID()); 
      SatIDSet::const_iterator itSat;
      int count(numSV);
      int nUnk((int)defaultEqDef.body.size()-1);
      for ( itSat = tempSat.begin(); itSat != tempSat.end(); ++itSat )
      {
         hMatrix( count , nUnk+(*itSat).id ) = 1.0;
         ++count;
      }


      SatID  dummySat;
      TypeID dummyType;
         // Now, let's fill the Phi and Q matrices
         // First, the troposphere
      pTropoStoModel->Prepare( dummyType,
                               dummySat,
                               gData );
      phiMatrix(0,0) = pTropoStoModel->getPhi();
      qMatrix(0,0)   = pTropoStoModel->getQ();
         // Second, the coordinates
      pCoordStoModel->Prepare( dummyType,
                               dummySat,
                               gData );
      for (int i=1; i<4; i++)
      {
         phiMatrix(i,i) = pCoordStoModel->getPhi();
         qMatrix(i,i)   = pCoordStoModel->getQ();
      }
         // Third, the receiver clock
      pClockStoModel->Prepare( dummyType,
                               dummySat,
                               gData );
      phiMatrix(4,4) = pClockStoModel->getPhi();
      qMatrix(4,4)   = pClockStoModel->getQ();
         // Finally, the phase biases
      for (int i=5; i<numUnknowns; i++)
      {
            // Create an appropriate satellite
         SatID sat( (i-4), SatID::systemGPS);
         pBiasStoModel->Prepare( TypeID::CSL1,
                                 sat,
                                 gData );
         phiMatrix(i,i) = pBiasStoModel->getPhi();
         qMatrix(i,i)   = pBiasStoModel->getQ();
      }


      try
      {
            // Call the Compute() method with the defined equation model.
            // This equation model MUST HAS BEEN previously set, usually when
            // creating the SolverPPP object with the appropriate
            // constructor.
         Compute(measVector, hMatrix, rMatrix);
      }
      catch(InvalidSolver& e)
      {
         GPSTK_RETHROW(e);
      }


         // Now we have to add the new values to the data structure
      Vector<double> postfitCode(numSV,0.0);
      Vector<double> postfitPhase(numSV,0.0);
      for (int i=0; i<numSV; i++)
      {
         postfitCode(i)  = postfitResiduals(i);
         postfitPhase(i) = postfitResiduals(i+numSV);
      }
      gData.insertTypeIDVector(TypeID::postfitC, postfitCode);
      gData.insertTypeIDVector(TypeID::postfitL, postfitPhase);

      return gData;

   }   // End SolverPPP::Process()


      /* Sets if a NEU system will be used.
       *
       * @param useNEU  Boolean value indicating if a NEU system will
       *                be used
       *
       */
   SolverPPP& SolverPPP::setNEU( bool useNEU )
   {

         // First, let's define a set with the typical code-based unknowns
      TypeIDSet tempSet;
         // Watch out here: 'tempSet' is a 'std::set', and all sets order their
         // elements. According to 'TypeID' class, this is the proper order:
      tempSet.insert(TypeID::wetMap);  // BEWARE: The first is wetMap!!!

      if (useNEU)
      {
         tempSet.insert(TypeID::dLat); // #2
         tempSet.insert(TypeID::dLon); // #3
         tempSet.insert(TypeID::dH);   // #4
      }
      else
      {
         tempSet.insert(TypeID::dx);   // #2
         tempSet.insert(TypeID::dy);   // #3
         tempSet.insert(TypeID::dz);   // #4
      }
      tempSet.insert(TypeID::cdt);     // #5

         // Now, we build the basic equation definition
      defaultEqDef.header = TypeID::prefitC;
      defaultEqDef.body = tempSet;

      return (*this);
   }


} // end namespace gpstk
