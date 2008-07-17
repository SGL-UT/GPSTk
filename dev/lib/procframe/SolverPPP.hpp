#pragma ident "$Id$"

/**
 * @file SolverPPP.hpp
 * Class to compute the PPP Solution.
 */

#ifndef SOLVERPPP_HPP
#define SOLVERPPP_HPP

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


namespace gpstk
{

      /** @addtogroup GPSsolutions */
      /// @ingroup math

      //@{

      /** This class computes the Precise Point Positioning (PPP) solution
       *  using a Kalman solver that combines ionosphere-free code and phase
       *  measurements.
       *
       * This class may be used either in a Vector- and Matrix-oriented way,
       * or with GNSS data structure objects from "DataStructures" class (much
       * more simple to use it this way).
       *
       * A typical way to use this class with GNSS data structures follows:
       *
       * @code
       *      // INITIALIZATION PART
       *
       *      // EBRE station nominal position
       *   Position nominalPos(4833520.192, 41537.1043, 4147461.560);
       *   RinexObsStream rin("ebre0300.02o");  // Data stream
       *
       *      // Load all the SP3 ephemerides files
       *   SP3EphemerisStore SP3EphList;
       *   SP3EphList.loadFile("igs11512.sp3");
       *   SP3EphList.loadFile("igs11513.sp3");
       *   SP3EphList.loadFile("igs11514.sp3");
       *
       *   NeillTropModel neillTM( nominalPos.getAltitude(),
       *                           nominalPos.getGeodeticLatitude(), 30);
       *
       *      // Objects to compute the tropospheric data
       *   BasicModel basicM(nominalPos, SP3EphList);
       *   ComputeTropModel computeTropo(neillTM);
       *
       *      // More declarations here: ComputeMOPSWeights, SimpleFilter,
       *      // LICSDetector, MWCSDetector, SolidTides, OceanLoading, 
       *      // PoleTides, CorrectObservables, ComputeWindUp, ComputeLinear,
       *      // LinearCombinations, etc.
       *
       *      // Declare a SolverPPP object
       *   SolverPPP pppSolver;
       *
       *     // PROCESSING PART
       *
       *   gnssRinex gRin;
       *
       *   while(rin >> gRin)
       *   {
       *      try
       *      {
       *         gRin  >> basicM
       *               >> correctObs
       *               >> compWindup
       *               >> computeTropo
       *               >> linear
       *               >> pcFilter
       *               >> markCSLI
       *               >> markCSMW
       *               >> mopsWeights
       *               >> pppSolver;
       *      }
       *      catch(...)
       *      {
       *         cerr << "Unknown exception at epoch: " << time << endl;
       *         continue;
       *      }
       *
       *         // Print results
       *      cout << time.DOYsecond()      << "  "; // Output field #1
       *      cout << pppSolver.solution[1] << "  "; // dx: Output field #2
       *      cout << pppSolver.solution[2] << "  "; // dy: Output field #3
       *      cout << pppSolver.solution[3] << "  "; // dz: Output field #4
       *      cout << pppSolver.solution[0] << "  "; // wetMap: Output field #5
       *      cout << endl;
       *   }
       * @endcode
       *
       * The "SolverPPP" object will extract all the data it needs from the
       * GNSS data structure that is "gRin" and will try to solve the PPP
       * system of equations using a Kalman filter. It will also insert back
       * postfit residual data (both code and phase) into "gRin" if it
       * successfully solves the equation system.
       *
       * By default, it will build the geometry matrix from the values of
       * coefficients wetMap, dx, dy, dz and cdt, IN THAT ORDER. Please note
       * that the first field of the solution will be the estimation of the
       * zenital wet tropospheric component (or at least, the part that wasn't
       * modeled by the tropospheric model used).
       *
       * You may configure the solver to work with a NEU system in the class
       * constructor or using the "setNEU()" method.
       *
       * In any case, the "SolverPPP" object will also automatically add and
       * estimate the ionosphere-free phase ambiguities. The independent vector
       * is composed of the code and phase prefit residuals.
       *
       * This class expects some weights assigned to each satellite. That can
       * be achieved with objects from classes such as "ComputeIURAWeights",
       * "ComputeMOPSWeights", etc.
       *
       * If these weights are not assigned, then the "SolverPPP" object will
       * set a value of "1.0" to code measurements, and "weightFactor" to phase
       * measurements. The default value of "weightFactor" is "10000.0". This
       * implies that code sigma is 1 m, and phase sigma is 1 cm.
       *
       * By default, the stochastic models used for each type of variable are:
       *
       *    \li Coordinates are modeled as constants (StochasticModel).
       *    \li Zenital wet tropospheric component is modeled as a random walk
       *        (RandomWalkModel), with a qPrime value of 3e-8 m*m/s.
       *    \li Receiver clock is modeled as white noise (WhiteNoiseModel).
       *    \li Phase biases are modeled as white noise when cycle slips
       *        happen, and as constants between cycle slips
       *        (PhaseAmbiguityModel).
       *
       * You may change this assignment with methods "setCoordinatesModel()",
       * "setTroposphereModel()" and "setReceiverClockModel()". However, you
       * are not allowed to change the phase biases stochastic model.
       *
       * \warning "SolverPPP" is based on a Kalman filter, and Kalman filters
       * are objets that store their internal state, so you MUST NOT use the
       * SAME object to process DIFFERENT data streams.
       *
       * @sa SolverBase.hpp, SolverLMS and CodeKalmanSolver for base classes.
       *
       */
   class SolverPPP : public CodeKalmanSolver
   {
   public:

         /** Common constructor.
          *
          * @param useNEU   If true, will compute dLat, dLon, dH coordinates;
          *                 if false (the default), will compute dx, dy, dz.
          */
      SolverPPP(bool useNEU = false);


         /** Compute the PPP Solution of the given equations set.
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


         /** Compute the PPP Solution of the given equations set.
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
         throw(InvalidSolver);


         /** Returns a reference to a gnnsRinex object after solving
          *  the previously defined equation system.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(InvalidSolver);


         /** Sets if a NEU system will be used.
          *
          * @param useNEU  Boolean value indicating if a NEU system will
          *                be used
          *
          */
      SolverPPP& setNEU( bool useNEU );


         /** Get the weight factor multiplying the phase measurements sigmas.
          * This factor is the code_sigma/phase_sigma ratio.
          */
      double getWeightFactor() const
      { return std::sqrt(weightFactor); };


         /** Set the weight factor multiplying the phase measurement sigma
          *
          * @param factor      Factor multiplying the phase measurement sigma
          *
          * \warning This factor should be the code_sigma/phase_sigma ratio.
          * For instance, if we assign a code sigma of 1 m and a phase sigma
          * of 10 cm, the ratio is 100, and so should be "factor".
          */
      SolverPPP& setWeightFactor(double factor)
      { weightFactor = (factor*factor); return (*this); };


         /// Get coordinates stochastic model pointer
      StochasticModel* getCoordinatesModel() const
      { return pCoordStoModel; };


         /** Set coordinates stochastic model
          *
          * @param pModel      Pointer to StochasticModel associated with
          *                    coordinates.
          */
      SolverPPP& setCoordinatesModel(StochasticModel* pModel)
      { pCoordStoModel = pModel; return (*this); };


         /// Get wet troposphere stochastic model pointer
      StochasticModel* getTroposphereModel() const
      { return pTropoStoModel; };


         /** Set wet troposphere stochastic model
          *
          * @param pModel      Pointer to StochasticModel associated with
          *                    wet troposphere.
          */
      SolverPPP& setTroposphereModel(StochasticModel* pModel)
      { pTropoStoModel = pModel; return (*this); };


         /// Get receiver clock stochastic model pointer
      StochasticModel* getReceiverClockModel() const
      { return pClockStoModel; };


         /** Set receiver clock stochastic model
          *
          * @param pModel      Pointer to StochasticModel associated with
          *                    receiver clock.
          */
      SolverPPP& setReceiverClockModel(StochasticModel* pModel)
      { pClockStoModel = pModel; return (*this); };


         /// Get phase biases stochastic model pointer
      StochasticModel* getPhaseBiasesModel() const
      { return pBiasStoModel; };


         /** Set phase biases stochastic model.
          *
          * @param pModel      Pointer to StochasticModel associated with
          *                    receiver clock.
          *
          * \warning This method should be used with caution, because model
          * must be of PhaseAmbiguityModel class in order to make sense.
          */
      SolverPPP& setPhaseBiasesModel(StochasticModel* pModel)
      { pBiasStoModel = pModel; return (*this); };


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
      SolverPPP& setPhiMatrix(const Matrix<double> & pMatrix)
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
      SolverPPP& setQMatrix(const Matrix<double> & pMatrix)
      { qMatrix = pMatrix; return (*this); };


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor.
      virtual ~SolverPPP() {};


   private:


         /// Number of unknowns
      int numUnknowns;


         /// Number of measurements
      int numMeas;


         /// Weight factor for phase measurements
      double weightFactor;


         /// Pointer to stochastic model for coordinates
      StochasticModel* pCoordStoModel;


         /// Pointer to stochastic model for troposphere
      StochasticModel* pTropoStoModel;


         /// Pointer to stochastic model for receiver clock
      StochasticModel* pClockStoModel;


         /// Pointer to stochastic model for phase biases
      StochasticModel* pBiasStoModel;


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


         /// Random Walk stochastic model
      static RandomWalkModel rwalkModel;


         /// White noise stochastic model
      static WhiteNoiseModel whitenoiseModel;


         /// Phase biases stochastic model (constant + white noise)
      static PhaseAmbiguityModel biasModel;


         /// Total number of satellites
      static int totalSVs;


         /// Initial index assigned to this class.
      static int classIndex;

         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


   }; // class SolverPPP

      //@}

} // namespace
#endif // SOLVERPPP_HPP
