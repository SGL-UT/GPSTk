#pragma ident "$Id$"

/**
 * @file DeltaOp.hpp
 * This is a class to apply the Delta operator (differences on
 * ground-related data) to GNSS data structures.
 */

#ifndef GPSTK_DELTAOP_HPP
#define GPSTK_DELTAOP_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009
//
//============================================================================



#include "TypeID.hpp"
#include "ProcessingClass.hpp"


namespace gpstk
{

      /** @addtogroup GPSsolutions */
      //@{

      /** This class applies the Delta operator (differences
       *  on ground-related data) to GNSS data structures.
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
       *   DeltaOp delta;
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
       * The "DeltaOp" object will visit every satellite in the GNSS data
       * structure that is "gRin" and will substract from the specified type
       * or types (code prefit residuals by default) the corresponding data in
       * the "gRef" data structure.
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
       * @sa NablaOp.hpp for differences on satellite-related data.
       *
       */
   class DeltaOp : public ProcessingClass
   {
   public:


         /// Default constructor. By default, it will difference prefitC data
         /// and will delete satellites present in reference station data but
         /// missing in input data.
      DeltaOp()
         : deleteMissingSats(true)
      { diffTypes.insert(TypeID::prefitC); setIndex(); };


         /** Common constructor taking a satTypeValueMap as reference
          *  station data.
          *
          * By default it will difference prefitC data and will delete
          * satellites present in reference station data but missing in
          * input data.
          *
          * @param gData     satTypeValueMap data object holding reference
          *                  station data.
          * @param delSats   Boolean value setting if satellites present in
          *                  reference station data but missing in input data
          *                  will be deleted from the later (this is the
          *                  default behaviour).
          */
      DeltaOp( const satTypeValueMap& gData,
               const bool& delSats = true )
         : refData(gData), deleteMissingSats(delSats)
      { diffTypes.insert(TypeID::prefitC); setIndex(); };


         /** Common constructor taking a satTypeValueMap as reference
          *  station data.
          *
          * By default it will delete satellites present in reference station
          * data but missing in input data.
          *
          * @param gData      satTypeValueMap data object holding reference
          *                   station data.
          * @param difftype   TypeID of data values to be differenced.
          * @param delSats    Boolean value setting if satellites present in
          *                   reference station data but missing in input data
          *                   will be deleted from the later (this is the
          *                   default behaviour).
          */
      DeltaOp( const satTypeValueMap& gData,
               const TypeID& difftype,
               const bool& delSats = true )
         : refData(gData), deleteMissingSats(delSats)
      { diffTypes.insert(difftype); setIndex(); };


         /** Common constructor taking a satTypeValueMap as reference
          *  station data.
          *
          * By default it will delete satellites present in reference
          * station data but missing in input data.
          *
          * @param gData      satTypeValueMap data object holding reference
          *                   station data.
          * @param diffSet    TypeIDSet of data values to be differenced.
          * @param delSats    Boolean value setting if satellites present in
          *                   reference station data but missing in input data
          *                   will be deleted from the later (this is the
          *                   default behaviour).
          */
      DeltaOp( const satTypeValueMap& gData,
               const TypeIDSet& diffSet,
               const bool& delSats = true )
         : refData(gData), deleteMissingSats(delSats), diffTypes(diffSet)
      { setIndex(); };


         /** Common constructor taking a gnssSatTypeValue as reference
          *  station data.
          *
          * By default it will difference prefitC data and will delete
          * satellites present in reference station data but missing in input
          * data.
          *
          * @param gData      gnssSatTypeValue data object holding reference
          *                   station data.
          * @param delSats    Boolean value setting if satellites present in
          *                   reference station data but missing in input data
          *                   will be deleted from the later (this is the
          *                   default behaviour).
          */
      DeltaOp( const gnssSatTypeValue& gData,
               const bool& delSats = true )
         : refData(gData.body), deleteMissingSats(delSats)
      { diffTypes.insert(TypeID::prefitC); setIndex(); };


         /** Common constructor taking a gnssSatTypeValue as reference
          *  station data.
          *
          * By default it will delete satellites present in reference station
          * data but missing in input data.
          *
          * @param gData      gnssSatTypeValue data object holding reference
          *                   station data.
          * @param difftype   TypeID of data values to be differenced.
          * @param delSats    Boolean value setting if satellites present in
          *                   reference station data but missing in input data
          *                   will be deleted from the later (this is the
          *                   default behaviour).
          */
      DeltaOp( const gnssSatTypeValue& gData,
               const TypeID& difftype,
               const bool& delSats = true )
         : refData(gData.body), deleteMissingSats(delSats)
      { diffTypes.insert(difftype); setIndex(); };


         /** Common constructor taking a gnssSatTypeValue as reference
          *  station data.
          *
          * By default it will delete satellites present in reference
          * station data but missing in input data.
          *
          * @param gData      gnssSatTypeValue data object holding reference
          *                   station data.
          * @param diffSet    TypeIDSet of data values to be differenced.
          * @param delSats    Boolean value setting if satellites present in
          *                   reference station data but missing in input data
          *                   will be deleted from the later (this is the
          *                   default behaviour).
          */
      DeltaOp( const gnssSatTypeValue& gData,
               const TypeIDSet& diffSet,
               const bool& delSats = true )
         : refData(gData.body), deleteMissingSats(delSats), diffTypes(diffSet)
      { setIndex(); };


         /** Common constructor taking a gnssRinex as reference station data.
          *
          * By default it will difference prefitC data and will delete
          * satellites present in reference station data but missing in
          * input data.
          *
          * @param gData      gnssRinex data object holding reference
          *                   station data.
          * @param delSats    Boolean value setting if satellites present in
          *                   reference station data but missing in input data
          *                   will be deleted from the later (this is the
          *                   default behaviour).
          */
      DeltaOp( const gnssRinex& gData,
               const bool& delSats = true )
         : refData(gData.body), deleteMissingSats(delSats)
      { diffTypes.insert(TypeID::prefitC); setIndex(); };


         /** Common constructor taking a gnssRinex as reference station data.
          *
          * By default it will delete satellites present in reference station
          * data but missing in input data.
          *
          * @param gData      gnssRinex data object holding reference
          *                   station data.
          * @param difftype   TypeID of data values to be differenced.
          * @param delSats    Boolean value setting if satellites present in
          *                   reference station data but missing in input data
          *                   will be deleted from the later (this is the
          *                   default behaviour).
          */
      DeltaOp( const gnssRinex& gData,
               const TypeID& difftype,
               const bool& delSats = true )
         : refData(gData.body), deleteMissingSats(delSats)
      { diffTypes.insert(difftype); setIndex(); };


         /** Common constructor taking a gnssRinex as reference station data.
          *
          * By default it will delete satellites present in reference station
          * data  but missing in input data.
          *
          * @param gData      gnssRinex data object holding reference
          *                   station data.
          * @param diffSet    TypeIDSet of data values to be differenced.
          * @param delSats    Boolean value setting if satellites present in
          *                   reference station data but missing in input data
          *                   will be deleted from the later (this is the
          *                   default behaviour).
          */
      DeltaOp( const gnssRinex& gData,
               const TypeIDSet& diffSet,
               const bool& delSats = true )
         : refData(gData.body), deleteMissingSats(delSats), diffTypes(diffSet)
      { setIndex(); };


         /** Method to set the satTypeValueMap data object holding reference
          *  station data.
          *
          * @param gData      satTypeValueMap data object holding reference
          *                   station data.
          */
      virtual DeltaOp& setRefData(const satTypeValueMap& gData)
      { refData = gData; return (*this); };


         /** Method to set the gnssSatTypeValue data object holding reference
          *  station data.
          *
          * @param gData      gnssSatTypeValue data object holding reference
          *                   station data.
          */
      virtual DeltaOp& setRefData(const gnssSatTypeValue& gData)
      { return setRefData( gData.body ); };


         /** Method to set the gnssRinex data object holding reference
          *  station data.
          *
          * @param gData      gnssRinex data object holding reference
          *                   station data.
          */
      virtual DeltaOp& setRefData(const gnssRinex& gData)
      { return setRefData( gData.body ); };


         /// Method to get the satTypeValueMap data object holding reference
         /// station data.
      virtual satTypeValueMap getRefData(void) const
      { return refData; };


         /** Method to set if satellites present in reference station data but
          *  missing in input data will be deleted from the later (this is the
          *  default behaviour).
          *
          * @param deleteSats    Boolean stating if satellites will be
          *                      deleted (true) or not (false).
          */
      virtual DeltaOp& setDeleteMissingSats(const bool& deleteSats)
      { deleteMissingSats = deleteSats; return (*this); };


         /// Method to get the deleteMissingSats field value, i.e., if missing
         /// satellites will be deleted (returns true) or not (returns false).
      virtual bool getDeleteMissingSats(void) const
      { return deleteMissingSats; };


         /** Method to set the data values to be differenced.
          *
          * @param difftype      TypeID of data values to be differenced.
          *
          * \warning The previously set type values will be deleted. If this
          * is not what you want, see method addDiffType.
          */
      virtual DeltaOp& setDiffType(const TypeID& difftype)
      { diffTypes.clear(); diffTypes.insert(difftype); return (*this); };


         /** Method to add a data value type to be differenced.
          *
          * @param difftype      TypeID of data values to be added to the ones
          *                      being differenced.
          */
      virtual DeltaOp& addDiffType(const TypeID& difftype)
      { diffTypes.insert(difftype); return (*this); };


         /** Method to set a set of data values to be differenced.
          *
          * @param diffSet       TypeIDSet of data values to be differenced.
          *
          * \warning The previously set type values will be deleted. If this
          * is not what you want, see method addDiffType.
          */
      virtual DeltaOp& setDiffTypeSet(const TypeIDSet& diffSet)
      { diffTypes.clear(); diffTypes = diffSet; return (*this); };


         /** Method to add a set of data value types to be differenced.
          *
          * @param diffSet       TypeIDSet of data values to be added to the
          *                      ones being differenced.
          */
      virtual DeltaOp& addDiffTypeSet(const TypeIDSet& diffSet);


         /// Method to get the set of data value types to be differenced.
      virtual TypeIDSet getDiffTypeSet(void) const
      { return diffTypes; };


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


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor.
      virtual ~DeltaOp() {};


   private:


         /// satTypeValueMap data structure containing reference station data.
      satTypeValueMap refData;


         /// Flag indicating if satellites present in reference station data
         /// but missing in input data will be deleted.
      bool deleteMissingSats;


         /// Set (TypeIDSet) containing the types of data to be differenced.
      TypeIDSet diffTypes;


         /// Initial index assigned to this class.
      static int classIndex;

         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


   }; // End of class 'DeltaOp'

      //@}

}  // End of namespace gpstk
#endif   // GPSTK_DELTAOP_HPP
