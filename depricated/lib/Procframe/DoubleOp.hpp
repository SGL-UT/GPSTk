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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010, 2011
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
 * @file DoubleOp.hpp
 * This is a class to apply the Double Difference operator (differences on
 * ground-related data and satellite) to GNSS data structures.
 */

#ifndef GPSTK_DOUBLEOP_HPP
#define GPSTK_DOUBLEOP_HPP

#include "DeltaOp.hpp"
#include "NablaOp.hpp"


namespace gpstk
{
      /// @ingroup GPSsolutions 
      //@{

      /** This class applies the double difference (differences
       *  on ground and satellite -related data) to GNSS data structures.
       *
       * A typical way to use this class follows:
       *
       * @code
       *      // Input observation file stream for ROVER
       *   RinexObsStream rin("ebre0300.02o");
       *      // Reference position of receiver station
       *   Position nominalPos(4833520.2269, 41537.00768, 4147461.489);
       *
       *      // Input observation file stream for REFERENCE STATION
       *   RinexObsStream rinRef("garr1900.07o");
       *      // Reference station nominal position
       *   Position nominalPosRef(4796983.7690, 160308.7500, 4187339.9860);
       *
       *      // Some more code and definitions here...
       *
       *      // GNSS data structure for rover data
       *   gnssRinex gRin;
       *
       *      // GNSS data structure for reference station data
       *   gnssRinex gRef;
       *
       *      // Set model defaults. A typical C1-based modeling is used
       *      // Set up rover's modeling object
       *   ModeledPR model( nominalPos,
       *                    ionoStore,
       *                    mopsTM,
       *                    bceStore,
       *                    TypeID::C1,
       *                    true );
       *
       *      // Set up reference station's modeling object
       *   ModeledReferencePR modelRef( nominalPosRef,
       *                                ionoStore,
       *                                mopsTM,
       *                                bceStore,
       *                                TypeID::C1,
       *                                true );
       *
       *      // Create an object to compute the single differences
       *      // of prefit residuals
       *   DoubleOp delta;
       *
       *
       *   while(rin >> gRin)
       *   {
       *
       *      rinRef >> gRef;   // Be sure that data streams ARE synchronized
       *                        // before following on!!!
       *
       *         // Set the reference data to be differenced
       *      delta.setRefData(gRef.body);
       *
       *         // Apply model to reference data
       *      gRef >> modelRef;
       *
       *         // By default, difference is applied on code prefit residuals
       *      gRin >> model >> delta >> solver;
       *   }
       *
       * @endcode
       *
       * The "DoubleOp" object will visit every satellite in the GNSS data
       * structure that is "gRin" and will substract from the specified type
       * or types (code prefit residuals by default) the corresponding data in
       * the "gRef" data structure.
       *
       * Afterwards, it will REMOVE reference satellite data from the GNSS data
       * structure and will substract the specified type or types (code prefit
       * residuals, dx, dy, dz by default) from the corresponding data in the
       * rest of the "gRef" data structure.
       *
       * Take notice that in the default case the code prefit residuals were
       * computed by objects of classes such as "ModeledPR" and
       * "ModeledReferencePR" (among others) objects, so those steps are
       * mandatory.
       *
       * Be warned that, by default, if a given satellite in "gRin" does not
       * have the data required to be differenced, it will be summarily deleted
       * from the data structure.
       *
       * @warning  A satellite will be kept as the reference satellite until
       * it's elevation less than 'refSatMinElev', then a new satellite with 
       * the highest elevation will be picked up as the reference satellite.
       *
       * @sa DoubleOp.hpp for double differences on ground and satellite 
       * related data.
       *
       */
   class DoubleOp : public ProcessingClass
   {
   public:

         /// Constructor
      DoubleOp()
         : refSatMinElev(35.0)
      {}

         /** Common constructor taking as input the type of data values
          *  to be differenced.
          *
          * @param difftype   TypeID of data values to be differenced.
          */
      DoubleOp(const TypeID& difftype)
         : refSatMinElev(35.0)
      {  
         sdStations.setDiffType(difftype); sdSatellites.setDiffType(difftype);
      };
         
         /** Method to set data type values to be differenced.
          *
          * @param difftype      TypeID of data values to be differenced.
          *
          * \warning  Previously set data type values will be deleted. If this
          * is not what you want, see method 'addDiffType()'.
          */
      virtual DoubleOp& setDiffType(const TypeID& difftype)
      { 
         sdStations.setDiffType(difftype); sdSatellites.setDiffType(difftype);
         return (*this); }
      ;


         /** Method to add a data value type to be differenced.
          *
          * @param difftype      TypeID of data values to be added to the ones
          *                      being differenced.
          */
      virtual DoubleOp& addDiffType(const TypeID& difftype)
      { 
         sdStations.addDiffType(difftype); sdSatellites.addDiffType(difftype);
         return (*this); 
      };


         /** Method to establish a set of data values to be differenced.
          *
          * @param diffSet       TypeIDSet of data values to be differenced.
          *
          * \warning  Previously set data type values will be deleted. If this
          * is not what you want, see method 'addDiffTypeSet()'.
          */
      virtual DoubleOp& setDiffTypeSet(const TypeIDSet& diffSet)
      { 
         sdStations.setDiffTypeSet(diffSet); sdSatellites.setDiffTypeSet(diffSet);
         return (*this); 
      };


         /** Method to add a set of data value types to be differenced.
          *
          * @param diffSet    TypeIDSet of data values to be added to the
          *                   ones being differenced.
          */
      virtual DoubleOp& addDiffTypeSet(const TypeIDSet& diffSet)
      {
         sdStations.addDiffTypeSet(diffSet); sdSatellites.addDiffTypeSet(diffSet);
         return (*this); 
      }


         /// Method to get the set of data value types to be differenced.
      virtual TypeIDSet getDiffTypeSet(void) const
      { return sdStations.getDiffTypeSet(); };

         /** Method to set the satTypeValueMap data object holding reference
          *  station data.
          *
          * @param gData      satTypeValueMap data object holding reference
          *                   station data.
          */
      virtual DoubleOp& setRefData(const satTypeValueMap& gData)
      { sdStations.setRefData(gData); return (*this); };


         /** Method to set the gnssSatTypeValue data object holding reference
          *  station data.
          *
          * @param gData      gnssSatTypeValue data object holding reference
          *                   station data.
          */
      virtual DoubleOp& setRefData(const gnssSatTypeValue& gData)
      { return setRefData( gData.body ); };


         /** Method to set the gnssRinex data object holding reference
          *  station data.
          *
          * @param gData      gnssRinex data object holding reference
          *                   station data.
          */
      virtual DoubleOp& setRefData(const gnssRinex& gData)
      { return setRefData( gData.body ); };


         /// Method to get the satTypeValueMap data object holding reference
         /// station data.
      virtual satTypeValueMap getRefData(void) const
      { return sdStations.getRefData(); };


          /** Method to set the gnssRinex data object holding reference
          *  station data.
          *
          * @param refSource  SourceID data object holding reference
          *                   station SourceID.
          */
      virtual DoubleOp& setRefSourceID(const SourceID& refSource)
      { refSourceID = refSource; return (*this); };


         /// Method to get the satTypeValueMap data object holding reference
         /// station SourceID.
      virtual SourceID getRefSourceID(void) const
      { return refSourceID; };
      

         /** Method to set the reference satellite to be used.
          *
          * @param rSat       SatID of reference satellite to be used.
          */
      virtual DoubleOp& setRefSat(const SatID& rSat)
      { refSatID = rSat; return (*this); };


         /// Method to get the reference satellite being used.
      virtual SatID getRefSat(void) const
      { return refSatID; };

          /** Method to set the minimum elevation allowed for the reference.
          *
          * @param rSat       minimum elevation allowed for the reference 
          *                   satellite.
          */
      virtual DoubleOp& setRefSatMinElevation(const double& minElevation)
      { refSatMinElev = minElevation; return (*this); };


         /// Method to get the minimum elevation allowed for the reference 
         /// satellite in degree.
      virtual double getRefSatMinElevation() const
      { return refSatMinElev; };


         /** Returns a reference to a satTypeValueMap object after differencing
          *  data type values given in 'diffTypes' field with respect to
          *  reference station data in 'refData' field.
          *
          * @param gData      Data object holding the data.
          */
      virtual satTypeValueMap& Process(satTypeValueMap& gData)
         throw(ProcessingException);


         /** Returns a reference to a gnssSatTypeValue object after
          *  differencing data type values given in 'diffTypes' field with
          *  respect to reference station data in 'refData' field.
          *
          * @param gData      Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException)
      { Process(gData.body); return gData; };


         /** Returns a reference to a gnnsRinex object after differencing
          *  data type values given in 'diffTypes' field with respect to
          *  reference station data in 'refData' field.
          *
          * @param gData      Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException)
      { Process(gData.body); return gData; };


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;

         /// Destructor
      ~DoubleOp(){}
      
   protected:
         
         /// Object to do single difference between receivers
      DeltaOp sdStations;
         
         /// Object to do single difference between satellites
      NablaOp sdSatellites;

         /// Reference receiver SourceID to get SD between stations
      SourceID refSourceID;

         /// Reference satellite SatID to get SD between satellites
      SatID refSatID;

         /// Minimum elevation for the reference satellite
         /// default is 35 degree
      double refSatMinElev;   

   }; // End of class 'DoubleOp'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_DOUBLEOP_HPP
