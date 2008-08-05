#pragma ident "$Id$"

/**
 * @file SolverPPPFB.hpp
 * Class to compute the PPP solution in forwards-backwards mode.
 */

#ifndef SOLVERPPPFB_HPP
#define SOLVERPPPFB_HPP

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
#include <list>


namespace gpstk
{

      /** @addtogroup GPSsolutions */
      /// @ingroup math

      //@{

      /** This class computes the Precise Point Positioning (PPP) solution
       *  using a Kalman solver that combines ionosphere-free code and phase
       *  measurements. Also, this class uses the 'forwards-backwards'
       *  approach, where the provided data set is processed from 'past to
       *  future' and from 'future to past' several times.
       *
       * This approach improves the final solution because it takes advantage
       * of a better phase ambiguity resolution. On the other hand, it is only
       * applicable in post-process mode (of course).
       *
       * In reality, "SolverPPPFB.hpp" objects are really "SolverPPP.hpp"
       * objects at their core, with some wrapper code that takes 'normal'
       * forwards input data, stores them, and feeds the internal "SolverPPP"
       * object with a continuous data stream formed by several instances of
       * forwards input data, "mirrored" input data (oldest is newest and
       * viceversa), forwards input data again, and so on.
       *
       * In order to achieve this, SolverPPPFB.hpp" objects work in three
       * distinct phases:
       *
       *    \li Initial "Process()" phase: This does a "normal" forward
       *        processing, like the "SolverPPP.hpp" objects. Data feed during
       *        this phase are internally stored for later phases.
       *    \li "ReProcess()" phase: In this phase the filter will process the
       *        stored data in "backwards-forwards" fashion as many times as
       *        set by the provided "cycles" parameter (by default, "cycles" is
       *        equal to 1). This phase always ends in backwards processing.
       *    \li "LastProcess()" phase: This is the last phase and it is always
       *        done in forwards mode. During this phase you will get your
       *        final results.
       *
       * Take due note that the "SolverPPPFB.hpp" class is designed to be used
       * ONLY with GNSS data structure objects from "DataStructures" class.
       *
       * A typical way to use this class follows:
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
       *                           nominalPos.getGeodeticLatitude(),
       *                           30 );
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
       *      // Declare a SolverPPPFB object
       *   SolverPPPFB pppSolver;
       *
       *     // PROCESSING PART
       *
       *   gnssRinex gRin;
       *
       *      // --->>> Process() phase <<<--- //
       *
       *   while(rin >> gRin)
       *   {
       *      try
       *      {
       *         gRin  >> basicM
       *               >> correctObs
       *               >> compWindup
       *               >> computeTropo
       *               >> linear1      // Compute combinations
       *               >> pcFilter
       *               >> markCSLI2
       *               >> markCSMW
       *               >> markArc
       *               >> linear2      // Compute prefit residuals
       *               >> phaseAlign
       *               >> pppSolver;
       *      }
       *      catch(...)
       *      {
       *         cerr << "Unknown exception at epoch: " << time << endl;
       *         continue;
       *      }
       *
       *   }   // End of 'while(rin >> gRin)'
       *
       *
       *      // --->>> ReProcess() phase <<<--- //
       *
       *   try
       *   {
       *
       *         // Now, let's do the forward-backward cycles (4)
       *      pppSolver.ReProcess(4);
       *
       *   }
       *   catch(Exception& e)
       *   {
       *      cerr << "Exception: " << e << endl;
       *   }
       *
       *
       *      // --->>> LastProcess() phase <<<--- //
       *
       *      // Loop over all data epochs, again
       *   while( pppSolver.LastProcess(gRin) )  // True while there are data
       *   {
       *
       *         // In this case, gRin is an output from 'LastProcess()'
       *      DayTime time(gRin.header.epoch);
       *
       *         // Print results
       *      cout << time.DOYsecond() << "  ";     // Epoch - Output field #1
       *
       *      cout << pppSolver.getSolution(TypeID::dLat) << "  "; // dLat - #2
       *      cout << pppSolver.getSolution(TypeID::dLon) << "  "; // dLon - #3
       *      cout << pppSolver.getSolution(TypeID::dH) << "  ";   // dH   - #4
       *      cout << pppSolver.getSolution(TypeID::wetMap) << "  "; // Tropo-#5
       *
       *   }
       * @endcode
       *
       * The "SolverPPPFB" object will also insert back postfit residual data
       * (both code and phase) into "gRin" if it successfully solves the
       * equation system.
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
       * In any case, the "SolverPPPFB" object will also automatically add and
       * estimate the ionosphere-free phase ambiguities. The independent vector
       * is composed of the code and phase prefit residuals.
       *
       * This class expects some weights assigned to each satellite. That can
       * be achieved with objects from classes such as "ComputeIURAWeights",
       * "ComputeMOPSWeights", etc.
       *
       * If these weights are not assigned, then the "SolverPPPFB" object will
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
       * \warning "SolverPPPFB" is based on a Kalman filter, and Kalman filters
       * are objets that store their internal state, so you MUST NOT use the
       * SAME object to process DIFFERENT data streams.
       *
       * @sa SolverBase.hpp, SolverLMS.hpp and CodeKalmanSolver.hpp for
       * base classes, as well as SolverPPP.hpp for a similar class.
       *
       */
   class SolverPPPFB : public ProcessingClass
   {
   public:

         /** Common constructor.
          *
          * @param useNEU   If true, will compute dLat, dLon, dH coordinates;
          *                 if false (the default), will compute dx, dy, dz.
          */
      SolverPPPFB(bool useNEU = false);


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


         /** Reprocess the data stored during a previous 'Process()' call.
          *
          * @param cycles     Number of forward-backward cycles (1 by default).
          *
          * \warning The minimum number of cycles allowed is "1". In fact, if
          * you introduce a smaller number, 'cycles' will be set to "1".
          */
      virtual void ReProcess(const int cycles = 1)
         throw(ProcessingException);


         /** Process the data stored during a previous 'ReProcess()' call, one
          *  item at a time, and always in forward mode.
          *
          * @param gData      Data object that will hold the resulting data.
          *
          * @return FALSE when all data is processed, TRUE otherwise.
          */
      virtual bool LastProcess(gnssSatTypeValue& gData)
         throw(ProcessingException);


         /** Process the data stored during a previous 'ReProcess()' call, one
          *  item at a time, and always in forward mode.
          *
          * @param gData      Data object that will hold the resulting data.
          *
          * @return FALSE when all data is processed, TRUE otherwise.
          */
      virtual bool LastProcess(gnssRinex& gData)
         throw(ProcessingException);


         /** Sets if a NEU system will be used.
          *
          * @param useNEU  Boolean value indicating if a NEU system will
          *                be used
          *
          */
      virtual SolverPPPFB& setNEU( bool useNEU );


         /** Get the weight factor multiplying the phase measurements sigmas.
          *  This factor is the code_sigma/phase_sigma ratio.
          */
      virtual double getWeightFactor(void) const
      { return pppFilter.getWeightFactor(); };


         /** Set the weight factor multiplying the phase measurement sigma
          *
          * @param factor      Factor multiplying the phase measurement sigma
          *
          * \warning This factor should be the code_sigma/phase_sigma ratio.
          * For instance, if we assign a code sigma of 1 m and a phase sigma
          * of 10 cm, the ratio is 100, and so should be "factor".
          */
      virtual SolverPPPFB& setWeightFactor(double factor)
      { pppFilter.setWeightFactor(factor); return (*this); };


         /// Get coordinates stochastic model pointer
      virtual StochasticModel* getCoordinatesModel(void) const
      { return pppFilter.getCoordinatesModel(); };


         /** Set coordinates stochastic model
          *
          * @param pModel      Pointer to StochasticModel associated with
          *                    coordinates.
          */
      virtual SolverPPPFB& setCoordinatesModel(StochasticModel* pModel)
      { pppFilter.setCoordinatesModel(pModel); return (*this); };


         /// Get wet troposphere stochastic model pointer
      virtual StochasticModel* getTroposphereModel(void) const
      { return pppFilter.getTroposphereModel(); };


         /** Set wet troposphere stochastic model
          *
          * @param pModel      Pointer to StochasticModel associated with
          *                    wet troposphere.
          */
      virtual SolverPPPFB& setTroposphereModel(StochasticModel* pModel)
      { pppFilter.setTroposphereModel(pModel); return (*this); };


         /// Get receiver clock stochastic model pointer
      virtual StochasticModel* getReceiverClockModel(void) const
      { return pppFilter.getReceiverClockModel(); };


         /** Set receiver clock stochastic model
          *
          * @param pModel      Pointer to StochasticModel associated with
          *                    receiver clock.
          */
      virtual SolverPPPFB& setReceiverClockModel(StochasticModel* pModel)
      { pppFilter.setReceiverClockModel(pModel); return (*this); };


         /// Get phase biases stochastic model pointer
      virtual StochasticModel* getPhaseBiasesModel(void) const
      { return pppFilter.getPhaseBiasesModel(); };


         /** Set phase biases stochastic model.
          *
          * @param pModel      Pointer to StochasticModel associated with
          *                    receiver clock.
          *
          * \warning This method should be used with caution, because model
          * must be of PhaseAmbiguityModel class in order to make sense.
          */
      virtual SolverPPPFB& setPhaseBiasesModel(StochasticModel* pModel)
      { pppFilter.setPhaseBiasesModel(pModel); return (*this); };


         /// Get the State Transition Matrix (phiMatrix)
      virtual Matrix<double> getPhiMatrix(void) const
      { return pppFilter.getPhiMatrix(); };


         /** Returns the solution associated to a given TypeID.
          *
          * @param type    TypeID of the solution we are looking for.
          */
      virtual double getSolution(const TypeID& type) const
         throw(InvalidRequest)
      { return pppFilter.getSolution(type); };


         /** Returns the variance associated to a given TypeID.
          *
          * @param type    TypeID of the variance we are looking for.
          */
      virtual double getVariance(const TypeID& type) const
         throw(InvalidRequest)
      {  return pppFilter.getVariance(type); };


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor.
      virtual ~SolverPPPFB() {};


   private:


         /// Boolean indicating if this is the first iteration of this filter
      bool firstIteration;


         /// List holding the information regarding every observation
      std::list<gnssRinex> ObsData;


         /// Set storing the TypeID's that we want to keep
      TypeIDSet keepTypeSet;


         /// General PPP Kalman filter object
      SolverPPP pppFilter;


         /// Initial index assigned to this class.
      static int classIndex;

         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


   }; // End of class 'SolverPPPFB'

      //@}

}  // End of namespace gpstk
#endif   // SOLVERPPPFB_HPP
