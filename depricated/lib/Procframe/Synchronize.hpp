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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008, 2011
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
 * @file Synchronize.hpp
 * This class synchronizes two GNSS Data Structures data streams.
 */

#ifndef GPSTK_SYNCHRONIZE_HPP
#define GPSTK_SYNCHRONIZE_HPP

#include <list>
#include "Exception.hpp"
#include "ProcessingClass.hpp"



namespace gpstk
{

      /// Thrown when synchronization was not possible at a given epoch
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(SynchronizeException, gpstk::Exception);


      /// @ingroup DataStructures 
      //@{


      /** This class synchronizes two GNSS Data Structures data streams.
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
       *      // Create the input observation file stream for REFERENCE STATION
       *   RinexObsStream rinRef("bell0300.02o");
       *
       *      // GDS for rover data
       *   gnssRinex gRin;
       *
       *      // GDS for reference station data
       *   gnssRinex gRef;
       *
       *      // Create an object to synchronize rover and reference station
       *      // data streams. This object will take data out from "rinRef"
       *      // until it is synchronized with data in "gRin". Default
       *      // synchronization tolerance is 1 s, but we change it to 2.5 s.
       *   Synchronize synchro( rinRef,
       *                        gRin,
       *                        2.5 );
       *
       *      // Create an object to compute the single differences of
       *      // prefit residuals
       *   DeltaOp delta;   // By default, it'll work on code prefit residuals
       *
       *
       *      // More declarations here....
       *
       *
       *     // PROCESSING PART
       *
       *   while(rin >> gRin)
       *   {
       *         // First, let's synchronize and process reference station data
       *      try
       *      {
       *
       *         gRef >> synchro >> myFilter >> modelRef;
       *
       *         delta.setRefData(gRef.body);
       *
       *      }
       *      catch(SynchronizeException& e)   // THIS IS VERY IMPORTANT IN
       *      {                                // ORDER TO MANAGE A POSSIBLE
       *         cout << endl;                 // DESYNCHRONIZATION!!!
       *         continue;
       *      }
       *      catch(...)
       *      {
       *         cerr << "Exception when processing reference station data "
       *              << "at epoch: " << gRef.header.epoch << endl;
       *      }
       *
       *
       *         // Rover data processing is done here:
       *      try
       *      {
       *
       *         gRin >> myFilter >> model >> delta >> baseChange >> solverNEU;
       *
       *      }
       *      catch(...)
       *      {
       *         cerr << "Exception at epoch: " << gRin10.header.epoch << endl;
       *      }
       *
       *         // Print results
       *      cout << solverNEU.getSolution(TypeID::dLat) << "  ";
       *      cout << solverNEU.getSolution(TypeID::dLon) << "  ";
       *      cout << solverNEU.getSolution(TypeID::dH)   << "  ";
       *      cout << endl;
       *   }
       * @endcode
       *
       * Each Synchronize object will take data out of a RinexObsStream object
       * holding reference data until the data stream is synchronized (within
       * a given tolerance) with another data stream.
       *
       * If synchronization is not possible at a given epoch, it will throw a
       * "SynchronizeException" exception. The program then must handle it
       * appropriately, usually just issuing a 'continue' statement in order to
       * process next epoch.
       *
       * @sa Decimate.hpp for another time management class.
       *
       */
   class Synchronize : public ProcessingClass
   {
   public:

         /// Default constructor
      Synchronize()
         : pRinexRef(NULL), pgRov1(NULL), tolerance(1.0),
           firstTime(true)
      { };


         /** Common constructor.
          *
          * @param rinexObs      RinexObsStream object of reference data.
          * @param roverData     gnssRinex that holds ROVER receiver data
          * @param tol           Tolerance, in seconds.
          */
      Synchronize( RinexObsStream& rinexObs,
                   gnssRinex& roverData,
                   const double tol = 1.0 )
         : tolerance(tol), firstTime(true)
      { setReferenceSource(rinexObs); setRoverData(roverData); };


         /** Common constructor.
          *
          * @param rinexObs      RinexObsStream object of reference data.
          * @param roverData     gnssSatTypeValue that holds ROVER receiver data
          * @param tol           Tolerance, in seconds.
          */
      Synchronize( RinexObsStream& rinexObs,
                   gnssSatTypeValue& roverData,
                   const double tol = 1.0 )
         : tolerance(tol), firstTime(true)
      { setReferenceSource(rinexObs); setRoverData(roverData); };


         /** Returns a gnnsSatTypeValue object, adding the new data
          *  generated when calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(SynchronizeException);


         /** Returns a gnnsRinex object, adding the new data generated
          *  when calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(SynchronizeException);


         /// Returns tolerance, in seconds.
      virtual double getTolerance(void) const
      { return tolerance; };


         /** Sets tolerance, in seconds.
          *
          * @param tol                 Tolerance, in seconds.
          */
      virtual Synchronize& setTolerance(const double tol);


         /// Returns a pointer to the RinexObsStream object of reference data.
      virtual RinexObsStream* getPtrReferenceSource(void) const
      { return pRinexRef; };


         /** Sets the RinexObsStream object of reference data.
          *
          * @param rinexObs      RinexObsStream object of reference data.
          */
      virtual Synchronize& setReferenceSource(RinexObsStream& rinexObs)
      { pRinexRef = &rinexObs; firstTime=true; return (*this); }


         /** Sets the gnssRinex that holds ROVER receiver data.
          *
          * @param roverData     gnssRinex that holds ROVER receiver data
          */
      virtual Synchronize& setRoverData(gnssRinex& roverData)
      { pgRov1 = (gnssSatTypeValue*)(&roverData); return (*this); }


         /** Sets the gnssSatTypeValue that holds ROVER receiver data.
          *
          * @param roverData     gnssSatTypeValue that holds ROVER receiver data
          */
      virtual Synchronize& setRoverData(gnssSatTypeValue& roverData)
      { pgRov1 = &roverData; return (*this); }


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~Synchronize() {};


   protected:

      virtual gnssRinex& Process(CommonTime time, gnssRinex& gData)
         throw(SynchronizeException);

         /// gnssRinex data buffer
      std::list<gnssRinex> gnssRinexBuffer;


         /// Pointer to input observation file stream for reference station.
      RinexObsStream* pRinexRef;


         /// Pointer to gnnsRinex data structure (GDS) that holds ROVER data.
      gnssSatTypeValue* pgRov1;


         /// Tolerance, in seconds.
      double tolerance;

         /// Flag to mark that first data batch was read.
      bool firstTime;


   }; // End of class 'Synchronize'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_SYNCHRONIZE_HPP
