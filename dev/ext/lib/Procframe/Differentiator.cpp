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
 * @file Differentiator.cpp
 * This class computes the numerical derivative of a given TypeID.
 */

#include "Differentiator.hpp"


namespace gpstk
{

      // Returns a string identifying this object.
   std::string Differentiator::getClassName() const
   { return "Differentiator"; }



      /* Common constructor.
       *
       * @param inType           TypeID to be differentiated.
       * @param outType          TypeID to store the derivative of inType.
       * @param samplingPeriod   Sampling period, in seconds.
       * @param tol              Tolerance, in seconds.
       * @param useArc           Whether satellite arcs will be used or not.
       */
   Differentiator::Differentiator( const TypeID& inType,
                                   const TypeID& outType,
                                   double samplingPeriod,
                                   double tol,
                                   bool useArc )
      : inputType(inType), outputType(outType), useSatArcs(useArc),
        watchCSFlag(TypeID::CSL1)
   {

      setSamplingPeriod(samplingPeriod);
      setTolerance(tol);

   }  // End of 'Differentiator::Differentiator()'



      /* Method to set the sampling period to be used, in seconds.
       *
       * @param samplingPeriod      Sampling period, in seconds.
       */
   Differentiator& Differentiator::setSamplingPeriod(double samplingPeriod)
   {

         // Check that samplingPeriod is bigger than zero
      if (samplingPeriod > 0.0)
      {
         Ts = samplingPeriod;
      }
      else
      {
         Ts = 1.0;                              // By default, 1 s
      }

         // Update delay
      delay = Ts * 5.0;           // Actualizar si se cambia el diferenciador

      return (*this);

   }  // End of 'Differentiator::setSamplingPeriod()'



      /* Method to set the tolerance to be used, in seconds.
       *
       * @param tol              Tolerance, in seconds.
       */
   Differentiator& Differentiator::setTolerance(double tol)
   {

         // Check that tolerance is bigger than zero
      if (tol > 0.0)
      {
         tolerance = tol;
      }
      else
      {
         tolerance = 0.005;                        // By default, 0.005 s
      }

      return (*this);

   }  // End of 'Differentiator::setTolerance()'



      /* Returns the data value (double) corresponding to provided SourceID
       * and SatID.
       *
       * @param source        Source to be looked for.
       * @param satellite     Satellite to be looked for.
       */
   double Differentiator::getValue( const SourceID& source,
                                    const SatID& satellite ) const
      throw( SourceIDNotFound, SatIDNotFound )
   {

         // Look for the SourceID
      std::map<SourceID, std::map<SatID, double> >::const_iterator itObs(
                                                svDerivativesMap.find(source) );
      if( itObs != svDerivativesMap.end() )
      {

            // Look for the SatID
         std::map<SatID, double>::const_iterator itSat(
                                             (*itObs).second.find(satellite) );
         if( itSat != (*itObs).second.end() )
         {
            return (*itSat).second;
         }
         else
         {
            GPSTK_THROW(SatIDNotFound("SatID not found in map"));
         }

      }
      else
      {
         GPSTK_THROW(SourceIDNotFound("SourceID not found in map"));
      }

   }  // End of method 'Differentiator::getValue()'



      /* Computes the derivatives, which will be stored in
       * field 'svDerivativesMap'.
       *
       * @param epoch     Time of observations.
       * @param source    Source of the observations.
       * @param gData     Data object holding the data.
       */
   void Differentiator::Compute( const CommonTime& epoch,
                                 const SourceID& source,
                                 const satTypeValueMap& gData )
      throw(ProcessingException)
   {

         // Clear the std::map with the derivatives
      svDerivativesMap.clear();

      try
      {

            // Loop through all the satellites
         for( satTypeValueMap::const_iterator it = gData.begin();
              it != gData.end();
              ++it )
         {

               // Get the value to be differentiated
            double value(0.0);

            try
            {
                  // Try to extract value
               value = (*it).second.getValue(inputType);
            }
            catch(...)
            {

                  // If value is missing, skip this satellite
               continue;

            }

               // Check if satellite currently has entries
            std::map<SatID, filterData>::const_iterator itDat(
                                       svData[ source ].find( (*it).first ) );
            if( itDat == svData[source].end() )
            {

                  // If it doesn't have an entry, insert one
               filterData fData;

                  // Configure with sampling period
               fData.filter.setT(Ts);

                  // Insert into map
               svData[ source ][ (*it).first ] = fData;

            }


               // Place to store if there was a cycle slip. False by default
            bool csflag(false);


               // Check if we want to use satellite arcs of cycle slip flags
            if(useSatArcs)
            {

               double arcN(0.0);

               try
               {

                     // Try to extract satellite's arc value
                  arcN = (*it).second.getValue(TypeID::satArc);

               }
               catch(...)
               {

                     // If data is missing we will ignore this satellite
                  continue;

               }


                  // Check if satellite arc has changed
               if( svData[ source ][ (*it).first ].arcNumber != arcN )
               {

                     // Set flag
                  csflag = true;

                     // Update satellite arc information
                  svData[ source ][(*it).first].arcNumber = arcN;
               }

            }  // End of first part of 'if(useSatArcs)'
            else
            {

               double flag(0.0);

               try
               {

                     // Try to extract satellite's cycle slip flag
                  flag = (*it).second.getValue(watchCSFlag);

               }
               catch(...)
               {

                     // If data is missing we will ignore this satellite
                  continue;

               }

                  // Check if there was a cycle slip
               if( flag > 0.0)
               {
                     // Set flag
                  csflag = true;
               }

            }  // End of second part of 'if(useSatArcs)...'

               // Compute time difference (in seconds) between this epoch and
               // previous epoch.
            double tDiff( std::abs(epoch -
                              svData[ source ][ (*it).first ].previousEpoch) );

               // If there was an arc change or cycle slip, or there was a
               // data gap (time difference bigger than sampling time), let's
               // reset the differentiator
            if( ( csflag ) ||
                ( std::abs( tDiff - Ts ) > tolerance ) )
            {

                  // Filter is reset, and method 'isValid()' will return false
               svData[ source ][ (*it).first ].filter.Reset();
            }

               // Update 'previousEpoch'
            svData[ source ][ (*it).first ].previousEpoch = epoch;

               // Compute derivative
            double result( svData[source][(*it).first].filter.Compute(value) );

               // If result is valid, insert value in derivatives map
            if( svData[ source ][ (*it).first ].filter.isValid() )
            {
               svDerivativesMap[ source ][ (*it).first ] = result;
            }

         }

            // Let's return
         return;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'Differentiator::Process()'



      /* Returns a gnnsSatTypeValue object, adding the new data generated
       *  when calling this object.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& Differentiator::Process(gnssSatTypeValue& gData)
      throw(ProcessingException)
   {

      try
      {

         Compute(gData.header.epoch, gData.header.source, gData.body);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'Differentiator::Process()'



      /* Returns a gnnsRinex object, adding the new data generated when
       *  calling this object.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& Differentiator::Process(gnssRinex& gData)
      throw(ProcessingException)
   {

      try
      {

         Compute(gData.header.epoch, gData.header.source, gData.body);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'Differentiator::Process()'



      /* Returns a gnssDataMap object, adding the new data generated when
       * calling this object.
       *
       * @param gData    Data object holding the data.
       */
   gnssDataMap& Differentiator::Process(gnssDataMap& gData)
      throw(ProcessingException)
   {

      try
      {

            // Iterate through all the data structure
         for( gnssDataMap::iterator itGdata = gData.begin();
              itGdata != gData.end();
              ++itGdata )
         {

               // Get epoch
            CommonTime workEpoch( (*itGdata).first );

               // Get a set with the SourceID in current data element
            SourceIDSet sourceSet( (*itGdata).second.getSourceIDSet() );

               // Loop through all the SourceID's
            for( SourceIDSet::const_iterator itSource = sourceSet.begin();
                 itSource != sourceSet.end();
                 ++itSource )
            {

                  // Compute the derivatives
               Compute( workEpoch,
                       (*itSource),
                       (*itGdata).second[ (*itSource) ] );

                  // We have the derivatives, so let's insert them into GDS
               for( std::map<SatID, double>::const_iterator
                              itSat = svDerivativesMap[ (*itSource) ].begin();
                    itSat != svDerivativesMap[ (*itSource) ].end();
                    ++itSat )
               {

                  double value( svDerivativesMap[(*itSource)][(*itSat).first] );

                     // Insert the derivatives in the right place
                  try
                  {

                        // We must take into account the delay
                     gData.insertValue( (workEpoch - delay),
                                        (*itSource),
                                        (*itSat).first,
                                        outputType,
                                        value );

                  }
                  catch(...)
                  {
                        // If it wasn't possible to introduce the derivative,
                        // let's continue
                     continue;
                  }

               }  // End of 'for( std::map<SatID, double>::const_iterator itSat'

            }  // End of 'for( SourceIDSet::const_iterator itSource = ...'

         }  // End of 'for( gnssDataMap::iterator itGdata = gData.begin();'

            // Let's return
         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'Differentiator::Process()'


}  // End of namespace gpstk
