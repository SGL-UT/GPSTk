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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2011
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
 * @file Differentiator.hpp
 * This class computes the numerical derivative of a given TypeID.
 */

#ifndef GPSTK_DIFFERENTIATOR_HPP
#define GPSTK_DIFFERENTIATOR_HPP

#include "ProcessingClass.hpp"
#include "FIRDifferentiator5thOrder.hpp"



namespace gpstk
{

      /// @ingroup GPSsolutions 
      //@{


      /** This class computes the numerical derivative of a given TypeID.
       *
       * This class is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       *      // Input observation file stream
       *   RinexObsStream rin("ebre0300.02o");
       *
       *   gnssRinex gRin;  // GNSS data structure
       *
       *      // Declare Differentiator object. It will compute the numerical
       *      // derivative of 'TypeID::L1' for each satellite, and will save
       *      // the result as 'TypeID::L1dot'.
       *   Differentiator diff( TypeID::L1, TypeID::L1dot );
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> diff;
       *   }
       * @endcode
       *
       * The "Differentiator" object will visit every satellite in the GNSS
       * data structure that is "gRin", computing the numerical derivative of
       * input "TypeID::L1" and storing the result indexed as the output
       * "TypeID::L1dot".
       *
       * \warning Objects in this class store their internal state, so you
       * MUST NOT use the SAME object to process DIFFERENT data streams.
       *
       */
   class Differentiator : public ProcessingClass
   {
   public:

         /// Default constructor.
      Differentiator()
         : inputType(TypeID::L1), outputType(TypeID::L1dot),
           tolerance(0.005), useSatArcs(true), watchCSFlag(TypeID::CSL1)
      { setSamplingPeriod(1.0); };


         /** Common constructor.
          *
          * @param inType           TypeID to be differentiated.
          * @param outType          TypeID to store the derivative of inType.
          * @param samplingPeriod   Sampling period, in seconds.
          * @param tol              Tolerance, in seconds.
          * @param useArc           Whether satellite arcs will be used or not.
          */
      Differentiator( const TypeID& inType,
                      const TypeID& outType,
                      double samplingPeriod,
                      double tol,
                      bool useArc = true );


         /// Resets this object, cleaning all its internal differentiators.
      virtual void Reset(void)
      { svData.clear(); return; };


         /// Method to get the TypeID being differentiated.
      virtual TypeID getInputType() const
      { return inputType; };


         /** Method to set the TypeID being differentiated.
          *
          * @param inType        TypeID to be differentiated.
          */
      virtual Differentiator& setInputType(const TypeID& inType)
      { inputType = inType; return (*this); };


         /// Method to get the TypeID to store the derivative.
      virtual TypeID getOutputType() const
      { return outputType; };


         /** Method to set the TypeID to store the derivative.
          *
          * @param outType        TypeID to store the derivative.
          */
      virtual Differentiator& setOutputType(const TypeID& outType)
      { outputType = outType; return (*this); };


         /// Method to get if this class will use satellite arcs (true) or
         /// cycle slip flags (false).
      virtual bool getUseSatArc() const
      { return useSatArcs; };


         /** Method to set if this class will use satellite arcs (true) or
          *  cycle slip flags (false).
          *
          * @param useArc           Whether this class will use satellite arcs
          *                         (true) or cycle slip flags (false).
          */
      virtual Differentiator& setUseSatArc(bool useArc)
      { useSatArcs = useArc; return (*this); };


         /// Method to get the default CS flag type being used.
      virtual TypeID getCSFlag() const
      { return watchCSFlag; };


         /** Method to set the default CS flag type to be used.
          *
          * @param watchFlag     Cycle slip flag to be watched.
          */
      virtual Differentiator& setCSFlag(const TypeID& watchFlag)
      { watchCSFlag = watchFlag; return (*this); };


         /// Method to get the sampling period being used, in seconds.
      virtual double getSamplingPeriod() const
      { return Ts; };


         /** Method to set the sampling period to be used, in seconds.
          *
          * @param samplingPeriod      Sampling period, in seconds.
          */
      virtual Differentiator& setSamplingPeriod(double samplingPeriod);


         /// Method to get the tolerance being used, in seconds.
      virtual double getTolerance() const
      { return tolerance; };


         /** Method to set the tolerance to be used, in seconds.
          *
          * @param tol              Tolerance, in seconds.
          */
      virtual Differentiator& setTolerance(double tol);


         /// Method to get the delay, in seconds, of the derivative.
      virtual double getDelay() const
      { return delay; };


         /// Method to get a std::map holding the derivative of every
         /// source/satellite combination.
      virtual std::map<SourceID, std::map<SatID, double> >
         getSvDerivativesMap() const
      { return svDerivativesMap; };


         /** Returns the data value (double) corresponding to provided SourceID
          *  and SatID.
          *
          * @param source        Source to be looked for.
          * @param satellite     Satellite to be looked for.
          */
      double getValue( const SourceID& source,
                       const SatID& satellite ) const
         throw( SourceIDNotFound, SatIDNotFound );


         /** Returns a gnnsSatTypeValue object, adding the new data generated
          *  when calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException);


         /** Returns a gnnsRinex object, adding the new data generated when
          *  calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException);


         /** Returns a gnssDataMap object, adding the new data generated when
          *  calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssDataMap& Process(gnssDataMap& gData)
         throw(ProcessingException);


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~Differentiator() {};


   private:


         /// Structure used to store SV alignment data.
      struct filterData
      {
            // Default constructor initializing the data in the structure
         filterData() : arcNumber(0.0),
                        previousEpoch(CommonTime::BEGINNING_OF_TIME),
                        filter(1.0) {};

         double arcNumber;                   ///< Current arc number
         CommonTime previousEpoch;              ///< Previous epoch
         FIRDifferentiator5thOrder filter;   ///< Differentiator

         // NOTE: Change the former to obtain another differentiation
      };


   protected:


         /// TypeID being differentiated.
      TypeID inputType;


         /// TypeID to store the derivative.
      TypeID outputType;


         /// Sampling period, in seconds.
      double Ts;


         /// Tolerance, in seconds.
      double tolerance;


         /// Delay of the derivative, in seconds.
      double delay;


         /** Flag indicating if this class will watch for satellite arcs (true)
          *  or cycle slip flags (false).
          */
      bool useSatArcs;


         /// If field 'useSatArcs' is false, cycle slip flag to be watched.
      TypeID watchCSFlag;


         /// Map holding the derivative of every source/satellite combination.
      std::map<SourceID, std::map<SatID, double> > svDerivativesMap;


         /// Map holding the information for all sources and satellites
      std::map<SourceID, std::map<SatID, filterData> > svData;


         /** Computes the derivatives, which will be stored in
          *  field 'svDerivativesMap'.
          *
          * @param epoch     Time of observations.
          * @param source    Source of the observations.
          * @param gData     Data object holding the data.
          */
      void Compute( const CommonTime& epoch,
                    const SourceID& source,
                    const satTypeValueMap& gData )
         throw(ProcessingException);


   }; // End of class 'Differentiator'


   //@}

}  // End of namespace gpstk

#endif   // GPSTK_DIFFERENTIATOR_HPP
