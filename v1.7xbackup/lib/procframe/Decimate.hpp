#pragma ident "$Id$"

/**
 * @file Decimate.hpp
 * This class decimates GNSS Data Structures data given a sampling interval,
 * a tolerance, and a starting epoch.
 */
#ifndef DECIMATE_HPP
#define DECIMATE_HPP

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


#include "Exception.hpp"
#include "ProcessingClass.hpp"



namespace gpstk
{

      /// Thrown when some epoch data must be decimated
      /// @ingroup exceptiongroup
    NEW_EXCEPTION_CLASS(DecimateEpoch, gpstk::Exception);


      /** @addtogroup DataStructures */
      //@{


      /** This class decimates GNSS Data Structures data.
       *
       * This class is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
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
       *      // Set up Niell tropospheric model
       *   NeillTropModel neillTM( nominalPos.getAltitude(),
       *                           nominalPos.getGeodeticLatitude(),
       *                           30 );
       *
       *      // Objects to compute the model
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
       *      // Object to decimate data: Take data each 900 s, tolerance 5 s
       *   Decimate decimateData( 900.0,
       *                          5.0,
       *                          SP3EphList.getInitialTime() );
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
       *               >> decimateData   // Continue processing only each 900 s
       *               >> mopsWeights
       *               >> pppSolver;
       *      }
       *      catch(DecimateEpoch& d)
       *      {
       *            // If we catch a DecimateEpoch exception, just continue
       *            // with the next epoch.
       *         continue;
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
       * Each Decimate object has a sampling interval, a tolerance and a epoch
       * to start decimation. If decimation conditions are met, it returns the
       * incoming GNSS Data Structure without changes and the processing chain
       * continues.
       *
       * Otherwise, it will stop the processing chain and throw a
       * "DecimateEpoch" exception. The program then must handle it
       * appropriately, usually just issuing a 'continue' statement in order
       * to process next epoch.
       *
       * @sa Synchronize.hpp for another time management class.
       *
       */
   class Decimate : public ProcessingClass
   {
   public:

         /// Default constructor
      Decimate()
         : sampling(30.0),tolerance(0.5),lastEpoch(DayTime::BEGINNING_OF_TIME)
      { setIndex(); };


         /** Common constructor.
          *
          * @param sampleInterval      Sampling interval, in seconds.
          * @param tol                 Tolerance, in seconds.
          * @param initialEpoch        Epoch to start decimation.
          */
      Decimate( const double sampleInterval,
                const double tol,
                const DayTime& initialEpoch )
         : sampling(sampleInterval), tolerance(tol), lastEpoch(initialEpoch)
      { setIndex(); };


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param time      Epoch corresponding to the data.
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( const DayTime& time,
                                        satTypeValueMap& gData )
         throw(DecimateEpoch);


         /** Returns a gnnsSatTypeValue object, adding the new data
          *  generated when calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(DecimateEpoch)
      { Process(gData.header.epoch, gData.body); return gData; };


         /** Returns a gnnsRinex object, adding the new data generated
          *  when calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(DecimateEpoch)
      { Process(gData.header.epoch, gData.body); return gData; };


         /// Returns sampling interval, in seconds.
      virtual double getSampleInterval(void) const
      { return sampling; };


         /** Sets sampling interval.
          *
          * @param sampleInterval      Sampling interval, in seconds.
          */
      virtual Decimate& setSampleInterval(const double sampleInterval);


         /// Returns tolerance, in seconds.
      virtual double getTolerance(void) const
      { return tolerance; };


         /** Sets tolerance, in seconds.
          *
          * @param tol                 Tolerance, in seconds.
          */
      virtual Decimate& setTolerance(const double tol);


         /** Sets epoch to start decimation.
          *
          * @param initialEpoch        Epoch to start decimation.
          */
      virtual Decimate& setInitialEpoch(const DayTime& initialEpoch)
      { lastEpoch = initialEpoch; return (*this); };


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~Decimate() {};


   private:


         /// Sampling interval, in seconds
      double sampling;

         /// Tolerance, in seconds
      double tolerance;

         /// Last processed epoch
      DayTime lastEpoch;

         /// Initial index assigned to this class.
      static int classIndex;

         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


   }; // End of class 'Decimate'

      //@}

}  // End of namespace gpstk
#endif   // DECIMATE_HPP
