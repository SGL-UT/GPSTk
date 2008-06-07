#pragma ident "$Id$"

/**
 * @file CodeKalmanSolver.cpp
 * Class to compute the code-based solution using a simple Kalman solver.
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


#include "CodeKalmanSolver.hpp"
#include "MatrixFunctors.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int CodeKalmanSolver::classIndex = 8000000;


      // Returns an index identifying this object.
   int CodeKalmanSolver::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string CodeKalmanSolver::getClassName() const
   { return "CodeKalmanSolver"; }


      // Constant stochastic model
   StochasticModel CodeKalmanSolver::constantModel;


      // White noise stochastic model
   WhiteNoiseModel CodeKalmanSolver::whitenoiseModel;


      // Initializing method.
   void CodeKalmanSolver::Init()
   {
         // Set the class index
      setIndex();

      numUnknowns = defaultEqDef.body.size();

      Vector<double> initialState(numUnknowns, 0.0);
      Matrix<double> initialErrorCovariance(numUnknowns, numUnknowns, 0.0);
         // Fill the initialErrorCovariance matrix
         // First, the coordinates
      for (int i=0; i<3; i++)
      {
         initialErrorCovariance(i,i) = 100.0;
      }
         // Now, the receiver clock
      initialErrorCovariance(3,3) = 9.0e10;

      kFilter.Reset( initialState, initialErrorCovariance );

         // Pointer to default coordinates stochastic model (constant)
      pCoordStoModel = &constantModel;


         // Pointer to default receiver clock stochastic model (white noise)
      pClockStoModel = &whitenoiseModel;


      solution.resize(numUnknowns);
   }


      // Default constructor.
   CodeKalmanSolver::CodeKalmanSolver()
   {
         // First, let's define a set with the typical code-based unknowns
      TypeIDSet tempSet;
      tempSet.insert(TypeID::dx);
      tempSet.insert(TypeID::dy);
      tempSet.insert(TypeID::dz);
      tempSet.insert(TypeID::cdt);

         // Now, we build the default definition for a common GNSS 
         // code-based equation
      defaultEqDef.header = TypeID::prefitC;
      defaultEqDef.body = tempSet;

         // Call the initializing method
      Init();

   }


      /* Explicit constructor. Sets the default equation definition 
       * to be used when fed with GNSS data structures.
       *
       * @param eqDef     gnssEquationDefinition to be used
       */
   CodeKalmanSolver::CodeKalmanSolver(const gnssEquationDefinition& eqDef)
   {
      setDefaultEqDefinition(eqDef);
      Init();
   }


      /* Compute the code-based Kalman solution of the given equations set.
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
       * \warning If you use this method, be sure you previously set
       * phiMatrix and qMatrix using the appropriate methods.
       *
       * @return
       *  0 if OK
       *  -1 if problems arose
       */
   int CodeKalmanSolver::Compute( const Vector<double>& prefitResiduals,
                                  const Matrix<double>& designMatrix,
                                  const Vector<double>& weightVector )
      throw(InvalidSolver)
   {
         // By default, results are invalid
      valid = false;

         // Check that everyting has a proper size
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

         // Call the more general CodeKalmanSolver::Compute() method
      return CodeKalmanSolver::Compute( prefitResiduals,
                                        designMatrix,
                                        wMatrix );
   }


      // Compute the code-based Kalman solution of the given equations set.
      //
      // @param prefitResiduals   Vector of prefit residuals
      // @param designMatrix      Design matrix for equation system
      // @param weightMatrix      Matrix of weights
      //
      // \warning A typical Kalman filter works with the measurements noise
      // covariance matrix, instead of the matrix of weights. Beware of this
      // detail, because this method uses the later.
      //
      // \warning If you use this method, be sure you previously set
      // phiMatrix and qMatrix using the appropriate methods.
      //
      // @return
      //  0 if OK
      //  -1 if problems arose
      //
   int CodeKalmanSolver::Compute( const Vector<double>& prefitResiduals,
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
      postfitResiduals = prefitResiduals - designMatrix * solution;

         // If everything is fine so far, then the results should be valid
      valid = true;

      return 0;

   }  // end CodeKalmanSolver::Compute()



      /* Returns a reference to a gnnsSatTypeValue object after 
       * solving the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& CodeKalmanSolver::Process(gnssSatTypeValue& gData)
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
   gnssRinex& CodeKalmanSolver::Process(gnssRinex& gData)
      throw(InvalidSolver)
   {
         // Number of measurements equals the number of visible satellites
      numMeas = gData.numSats();

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

         // Build the vector of measurements
      measVector = gData.getVectorOfTypeID(defaultEqDef.header);

         // Generate the appropriate weights matrix
      Vector<double> weightsVector(gData.getVectorOfTypeID(TypeID::weight));
      for (int i= 0; i<numMeas; i++)
      {
         rMatrix(i,i) = weightsVector(i);
      }

         // Generate the corresponding geometry/design matrix
      hMatrix = gData.body.getMatrixOfTypes((*this).defaultEqDef.body);

      SatID  dummySat;
      TypeID dummyType;
         // Now, let's fill the Phi and Q matrices
         // First, the coordinates
      pCoordStoModel->Prepare(dummyType, dummySat, gData);
      for (int i=0; i<3; i++)
      {
         phiMatrix(i,i) = pCoordStoModel->getPhi();
         qMatrix(i,i)   = pCoordStoModel->getQ();
      }

         // Now, the receiver clock
      pClockStoModel->Prepare(dummyType, dummySat, gData);
      phiMatrix(3,3) = pClockStoModel->getPhi();
      qMatrix(3,3)   = pClockStoModel->getQ();

      try
      {
            // Call the Compute() method with the defined equation model.
            // This equation model MUST HAS BEEN previously set, usually when
            // creating the CodeKalmanSolver object with the appropriate
            // constructor.
         Compute(measVector, hMatrix, rMatrix);
      }
      catch(InvalidSolver& e)
      {
         GPSTK_RETHROW(e);
      }


         // Now we have to add the new values to the data structure
      if ( defaultEqDef.header == TypeID::prefitC )
      {
         gData.insertTypeIDVector(TypeID::postfitC, postfitResiduals);
      }

      return gData;

   }   // End CodeKalmanSolver::Process()


} // end namespace gpstk
