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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2006, 2007, 2008, 2011
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
 * @file ComputeIURAWeights.hpp
 * This class computes satellites weights based on URA Index and is
 * meant to be used with GNSS data structures.
 */

#ifndef GPSTK_COMPUTEIURAWEIGHTS_HPP
#define GPSTK_COMPUTEIURAWEIGHTS_HPP

#include "WeightBase.hpp"
#include "EngEphemeris.hpp"
#include "SP3EphemerisStore.hpp"
#include "GPSEphemerisStore.hpp"
#include "ProcessingClass.hpp"


namespace gpstk
{

      /// @ingroup DataStructures 
      //@{


      /** This class computes satellites weights based on URA Index.
       *
       * This class is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       *   RinexObsStream rin("ebre0300.02o");
       *   RinexNavStream rnavin("brdc0300.02n");
       *   RinexNavData rNavData;
       *   GPSEphemerisStore bceStore;
       *   while (rnavin >> rNavData) bceStore.addEphemeris(rNavData);
       *   bceStore.SearchPast();  // This is the default
       *
       *      // Declare a GDS object
       *   gnssRinex gRin;
       *
       *      // Create a 'ComputeIURAWeights' object
       *   ComputeIURAWeights iuraW(bceStore);
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> iuraW;
       *   }
       * @endcode
       *
       * The "ComputeIURAWeights" object will visit every satellite in the
       * GNSS data structure that is "gRin" and will try to compute its weight
       * based on the corresponding IURA. For precise ephemeris, a fixed value
       * of IURA = 0.1 m will be set, returning a weight of 100.
       *
       * When used with the ">>" operator, this class returns the same incoming
       * data structure with the weights inserted along their corresponding
       * satellites. Be warned that if it is not possible to compute the
       * weight for a given satellite, it will be summarily deleted from
       * the data structure.
       *
       * @sa ComputeMOPSWeights.hpp.
       */
   class ComputeIURAWeights : public WeightBase, public ProcessingClass
   {
   public:


         /// Default constructor
      ComputeIURAWeights() : pBCEphemeris(NULL), pTabEphemeris(NULL)
      { };


         /** Common constructor
          *
          * @param bcephem   GPSEphemerisStore object holding the ephemeris.
          */
      ComputeIURAWeights(GPSEphemerisStore& bcephem)
         : pBCEphemeris(&bcephem), pTabEphemeris(NULL)
      { };


         /** Common constructor
          *
          * @param tabephem  SP3EphemerisStore object holding the
          *                  ephemeris.
          */
      ComputeIURAWeights(SP3EphemerisStore& tabephem)
         : pBCEphemeris(NULL), pTabEphemeris(&tabephem)
      { };


         /** Common constructor
          *
          * @param ephem  XvtStore<SatID> object holding the ephemeris.
          */
      ComputeIURAWeights(XvtStore<SatID>& ephem)
      { setDefaultEphemeris(ephem); };


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( const CommonTime& time,
                                        satTypeValueMap& gData )
         throw(ProcessingException);


         /** Returns a gnnsSatTypeValue object, adding the new data
          *  generated when calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException)
      { Process(gData.header.epoch, gData.body); return gData; };


         /** Returns a gnnsRinex object, adding the new data generated
          *  when calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException)
      { Process(gData.header.epoch, gData.body); return gData; };


         /** Method to set the default ephemeris to be used with GNSS
          *  data structures.
          *
          * @param ephem     EphemerisStore object to be used
          */
      virtual ComputeIURAWeights& setDefaultEphemeris(XvtStore<SatID>& ephem);


         /** Method to set the default ephemeris to be used with GNSS
          *  data structures.
          *
          * @param ephem     GPSEphemerisStore object to be used
          */
      virtual ComputeIURAWeights& setDefaultEphemeris(GPSEphemerisStore& ephem)
      { pBCEphemeris = &ephem; pTabEphemeris = NULL; return (*this); };


         /** Method to set the default ephemeris to be used with GNSS
          *  data structures.
          *
          * @param ephem     SP3EphemerisStore object to be used
          */
      virtual ComputeIURAWeights& setDefaultEphemeris(SP3EphemerisStore& ephem)
      { pBCEphemeris = NULL; pTabEphemeris = &ephem; return (*this); };


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~ComputeIURAWeights() {};


   protected:


         /// Pointer to default broadcast ephemeris to be used.
      GPSEphemerisStore* pBCEphemeris;


         /// Pointer to default precise ephemeris to be used.
      SP3EphemerisStore* pTabEphemeris;


         /** Method to really get the weight of a given satellite.
          *
          * @param sat           Satellite
          * @param time          Epoch
          * @param preciseEph    Precise ephemerisStore object to be used
          */
      virtual double getWeight( const SatID& sat,
                                const CommonTime& time,
                                const SP3EphemerisStore* preciseEph )
         throw(InvalidWeights);


         /** Method to really get the weight of a given satellite.
          *
          * @param sat       Satellite
          * @param time      Epoch
          * @param bcEph     Broadcast EphemerisStore object to be used
          */
      virtual double getWeight( const SatID& sat,
                                const CommonTime& time,
                                const GPSEphemerisStore* bcEph )
         throw(InvalidWeights);


   }; // End of class 'ComputeIURAWeights'

      //@}

}  // End of namespace gpstk

#endif // GPSTK_COMPUTEIURAWEIGHTS_HPP
