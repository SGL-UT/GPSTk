#pragma ident "$Id$"

/**
 * @file CodeKalmanSolver.hpp
 * Class to compute the code-based solution using a simple Kalman solver.
 */

#ifndef CODEKALMANSOLVER_HPP
#define CODEKALMANSOLVER_HPP

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


#include "SolverLMS.hpp"
#include "StochasticModel.hpp"
#include "SimpleKalmanFilter.hpp"


namespace gpstk
{

      /** @addtogroup GPSsolutions */
      /// @ingroup math

      //@{

      /** This class computes the code-based solution using a simple
       *  Kalman solver.
       *
       * This class may be used either in a Vector- and Matrix-oriented way,
       * or with GNSS data structure objects from "DataStructures" class.
       *
       * A typical way to use this class with GNSS data structures follows:
       *
       * @code
       *      // Data stream
       *   RinexObsStream rin("ebre0300.02o");
       *
       *      // More declarations here: Ionospheric and tropospheric models,
       *      // ephemeris, etc.
       *
       *      // Declare the modeler object, setting all the parameters in
       *      // one pass
       *   ModelObs model( ionoStore,
       *                   mopsTM,
       *                   bceStore,
       *                   TypeID::C1 );
       *
       *      // Set initial position (Bancroft method)
       *   model.Prepare();
       *
       *      // Declare a CodeKalmanSolver object
       *   CodeKalmanSolver kSolver;
       *
       *      // This object will compute the appropriate MOPS weights
       *   ComputeMOPSWeights mopsW(nominalPos, bceStore);
       *
       *   gnssRinex gRin;
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> model >> mopsW >> kSolver;
       *   }
       * @endcode
       *
       * The "CodeKalmanSolver" object will extract all the data it needs
       * from the GNSS data structure that is "gRin" and will try to solve the
       * code-based system of equations using a simple Kalman filter. It will
       * also insert back postfit residual data into "gRin" if it successfully
       * solves the equation system.
       *
       * This class may optionally use weights assigned to each satellite.
       * This can be achieved with objects from classes such as
       * "ComputeIURAWeights", "ComputeMOPSWeights", etc., but in any case
       * this is not mandatory.
       *
       * By default, it will build the geometry matrix from the values of
       * coefficients dx, dy, dz and cdt, and the independent vector will be
       * composed of the code prefit residuals (TypeID::prefitC) values.
       *
       * You may change the former by redefining the default equation
       * definition to be used. For instance:
       *
       * @code
       *      // Define our own set of unknowns
       *   TypeIDSet unknownsSet;
       *   unknownsSet.insert(TypeID::dLat);
       *   unknownsSet.insert(TypeID::dLon);
       *   unknownsSet.insert(TypeID::dH);
       *   unknownsSet.insert(TypeID::cdt);
       *
       *      // Create a new equation definition
       *      // newEq(independent value, set of unknowns)
       *   gnssEquationDefinition newEq(TypeID::prefitC, unknownsSet);
       *
       *      // Reconfigure solver
       *   kSolver.setDefaultEqDefinition(newEq);
       * @endcode
       *
       *
       * \warning Kalman filters are objets that store their internal
       * state, so you MUST NOT use the SAME object to process DIFFERENT data
       * streams.
       *
       * @sa SolverBase.hpp and SolverLMS for base classes.
       *
       */
   class CodeKalmanSolver : public SolverLMS
   {
   public:

         /// Default constructor.
      CodeKalmanSolver();


         /** Explicit constructor. Sets the default equation definition
          *  to be used when fed with GNSS data structures.
          *
          * @param eqDef     gnssEquationDefinition to be used
          */
      CodeKalmanSolver(const gnssEquationDefinition& eqDef);


         /** Compute the code-based Kalman solution of the given equations set.
          *
          * @param prefitResiduals   Vector of prefit residuals
          * @param designMatrix      Design matrix for the equation system
          * @param weightMatrix      Matrix of weights
          *
          * \warning A typical Kalman filter works with the measurements noise
          * covariance matrix, instead of the matrix of weights. Beware of this
          * detail, because this method uses the later.
          *
          * \warning If you use this method, be sure you previously set
          * phiMatrix and qMatrix using the appropriate methods.
          *
          * @return
          *  0 if OK
          *  -1 if problems arose
          */
      virtual int Compute( const Vector<double>& prefitResiduals,
                           const Matrix<double>& designMatrix,
                           const Matrix<double>& weightMatrix )
         throw(InvalidSolver);


         /** Compute the code-based Kalman solution of the given equations set.
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
      virtual int Compute( const Vector<double>& prefitResiduals,
                           const Matrix<double>& designMatrix,
                           const Vector<double>& weightVector )
         throw(InvalidSolver);


         /** Returns a reference to a gnnsSatTypeValue object after
          *  solving the previously defined equation system.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException);


         /** Returns a reference to a gnnsRinex object after solving
          *  the previously defined equation system.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException);


         /// Get coordinates stochastic model pointer
      StochasticModel* getCoordinatesModel() const
      { return pCoordStoModel; };


         /** Set coordinates stochastic model
          *
          * @param pModel      Pointer to StochasticModel associated with
          *                    coordinates.
          */
      CodeKalmanSolver& setCoordinatesModel(StochasticModel* pModel)
      { pCoordStoModel = pModel; return (*this); };


         /// Get receiver clock stochastic model pointer
      StochasticModel* getReceiverClockModel() const
      { return pClockStoModel; };


         /** Set receiver clock stochastic model
          *
          * @param pModel      Pointer to StochasticModel associated with
          *                    receiver clock.
          */
      CodeKalmanSolver& setReceiverClockModel(StochasticModel* pModel)
      { pClockStoModel = pModel; return (*this); };


         /// Get the State Transition Matrix (phiMatrix)
      Matrix<double> getPhiMatrix() const
      { return phiMatrix; };


         /** Set the State Transition Matrix (phiMatrix)
          *
          * @param pMatrix     State Transition matrix.
          *
          * \warning Process() methods set phiMatrix and qMatrix according to
          * the stochastic models already defined. Therefore, you must use
          * the Compute() methods directly if you use this method.
          *
          */
      CodeKalmanSolver& setPhiMatrix(const Matrix<double> & pMatrix)
      { phiMatrix = pMatrix; return (*this); };


         /// Get the Noise covariance matrix (QMatrix)
      Matrix<double> getQMatrix() const
      { return qMatrix; };


         /** Set the Noise Covariance Matrix (QMatrix)
          *
          * @param pMatrix     Noise Covariance matrix.
          *
          * \warning Process() methods set phiMatrix and qMatrix according to
          * the stochastic models already defined. Therefore, you must use
          * the Compute() methods directly if you use this method.
          *
          */
      CodeKalmanSolver& setQMatrix(const Matrix<double> & pMatrix)
      { qMatrix = pMatrix; return (*this); };


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor.
      virtual ~CodeKalmanSolver() {};


   private:


         /// Number of unknowns
      int numUnknowns;


         /// Number of measurements
      int numMeas;


         /// Pointer to stochastic model for coordinates
      StochasticModel* pCoordStoModel;


         /// Pointer to stochastic model for receiver clock
      StochasticModel* pClockStoModel;


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


         /// General Kalman filter object
      SimpleKalmanFilter kFilter;


         /// Initializing method.
      void Init(void);


         /// Constant stochastic model
      static StochasticModel constantModel;


         /// White noise stochastic model
      static WhiteNoiseModel whitenoiseModel;


         /// Initial index assigned to this class.
      static int classIndex;

         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


   }; // End of class 'CodeKalmanSolver'

      //@}

}  // End of namespace gpstk
#endif   // CODEKALMANSOLVER_HPP
