
/**
 * @file DeltaOp.hpp
 * This is a class to apply the Delta operator (differences on ground-related data) to GNSS data structures.
 */

#ifndef DELTAOP_HPP
#define DELTAOP_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007
//
//============================================================================



#include "TypeID.hpp"
#include "DataStructures.hpp"


namespace gpstk
{
    /** @addtogroup GPSsolutions */
      //@{

      /**
       * This class applies the Delta operator (differences on ground-related data) 
       * to GNSS data structures.
       *
       * 
       * A typical way to use this class follows:
       *
       * @code
       *   // Input observation file stream for ROVER
       *   RinexObsStream rin("ebre0300.02o");
       *   // Reference position of receiver station
       *   Position nominalPos(4833520.2269, 41537.00768, 4147461.489);
       *
       *   // Input observation file stream for REFERENCE STATION
       *   RinexObsStream rinRef("garr1900.07o");
       *   // Reference station nominal position
       *  Position nominalPosRef(4796983.7690, 160308.7500, 4187339.9860);
       *
       *   // Some more code and definitions here...
       *
       *   gnssRinex gRin;  // GNSS data structure for rover data
       *   gnssRinex gRef;  // GNSS data structure for reference station data
       *
       *   // Set defaults of models. A typical C1-based modeling is used
       *   ModeledPR model(nominalPos, ionoStore, mopsTM, bceStore, TypeID::C1, true);
       *   ModeledReferencePR modelRef(nominalPosRef, ionoStore, mopsTM, bceStore, TypeID::C1, true);
       *
       *   // Create an object to compute the single differences of prefit residuals
       *   DeltaOp delta;
       *
       *
       *   while(rin >> gRin) {
       *
       *      rinRef >> gRef;   // Be sure that data streams ARE synchronized!!!
       *      delta.setRefData(gRef.body);   // Set the reference data to be differenced
       *
       *      gRef >> modelRef;     // Apply model to reference data
       *
       *      // By default, difference is applied on code prefit residuals
       *      gRin >> model >> delta >> solver;
       *   }
       *
       * @endcode
       *
       * The "DeltaOp" object will visit every satellite in the GNSS data structure that
       * is "gRin" and will substract from the specified type or types (code prefit
       * residuals by default) the corresponding data in the "gRef" data structure.
       * 
       * Take notice that in the default case the code prefit residuals were computed by 
       * the "ModeledPR" and "ModeledReferencePR" objects, so those steps are mandatory.
       *
       * Be warned that, by default, if a given satellite does not have in "gRin" the data
       * required to be differenced, it will be summarily deleted from the data structure.
       *
       * @sa NablaOp.hpp for differences on satellite-related data.
       *
       */
    class DeltaOp
    {
    public:

        /// Default constructor. By default it will difference prefitC data and will delete satellites present in reference station data but missing in input data.
        DeltaOp() : deleteMissingSats(true)
        {
            diffTypes.insert(TypeID::prefitC);
        };


        /** Common constructor taking a satTypeValueMap as reference station data.
         * By default it will difference prefitC data and will delete satellites
         * present in reference station data but missing in input data.
         *
         * @param gData     satTypeValueMap data object holding the reference station data.
         * @param delSats   Boolean value setting if satellites present in reference station data but missing in input data will be deleted from the later (this is the default).
         */
        DeltaOp(const satTypeValueMap& gData, const bool& delSats=true) : refData(gData), deleteMissingSats(delSats)
        {
            diffTypes.insert(TypeID::prefitC);
        }


        /** Common constructor taking a satTypeValueMap as reference station data.
         * By default it will delete satellites present in reference station data 
         * but missing in input data.
         *
         * @param gData     satTypeValueMap data object holding the reference station data.
         * @param difftype  TypeID of data values to be differenced.
         * @param delSats   Boolean value setting if satellites present in reference station data but missing in input data will be deleted from the later (this is the default).
         */
        DeltaOp(const satTypeValueMap& gData, const TypeID& difftype, const bool& delSats=true) : refData(gData), deleteMissingSats(delSats)
        {
            diffTypes.insert(difftype);
        }


        /** Common constructor taking a satTypeValueMap as reference station data.
         * By default it will delete satellites present in reference station data 
         * but missing in input data.
         *
         * @param gData     satTypeValueMap data object holding the reference station data.
         * @param diffSet   TypeIDSet of data values to be differenced.
         * @param delSats   Boolean value setting if satellites present in reference station data but missing in input data will be deleted from the later (this is the default).
         */
        DeltaOp(const satTypeValueMap& gData, const TypeIDSet& diffSet, const bool& delSats=true) : refData(gData), deleteMissingSats(delSats), diffTypes(diffSet) {}


        /** Common constructor taking a gnssSatTypeValue as reference station data.
         * By default it will difference prefitC data and will delete satellites
         * present in reference station data but missing in input data.
         *
         * @param gData     gnssSatTypeValue data object holding the reference station data.
         * @param delSats   Boolean value setting if satellites present in reference station data but missing in input data will be deleted from the later (this is the default).
         */
        DeltaOp(const gnssSatTypeValue& gData, const bool& delSats=true) : refData(gData.body), deleteMissingSats(delSats)
        {
            diffTypes.insert(TypeID::prefitC);
        }


        /** Common constructor taking a gnssSatTypeValue as reference station data.
         * By default it will delete satellites present in reference station data 
         * but missing in input data.
         *
         * @param gData     gnssSatTypeValue data object holding the reference station data.
         * @param difftype  TypeID of data values to be differenced.
         * @param delSats   Boolean value setting if satellites present in reference station data but missing in input data will be deleted from the later (this is the default).
         */
        DeltaOp(const gnssSatTypeValue& gData, const TypeID& difftype, const bool& delSats=true) : refData(gData.body), deleteMissingSats(delSats)
        {
            diffTypes.insert(difftype);
        }


        /** Common constructor taking a gnssSatTypeValue as reference station data.
         * By default it will delete satellites present in reference station data 
         * but missing in input data.
         *
         * @param gData     gnssSatTypeValue data object holding the reference station data.
         * @param diffSet   TypeIDSet of data values to be differenced.
         * @param delSats   Boolean value setting if satellites present in reference station data but missing in input data will be deleted from the later (this is the default).
         */
        DeltaOp(const gnssSatTypeValue& gData, const TypeIDSet& diffSet, const bool& delSats=true) : refData(gData.body), deleteMissingSats(delSats), diffTypes(diffSet) {}


        /** Common constructor taking a gnssRinex as reference station data.
         * By default it will difference prefitC data and will delete satellites
         * present in reference station data but missing in input data.
         *
         * @param gData     gnssRinex data object holding the reference station data.
         * @param delSats   Boolean value setting if satellites present in reference station data but missing in input data will be deleted from the later (this is the default).
         */
        DeltaOp(const gnssRinex& gData, const bool& delSats=true) : refData(gData.body), deleteMissingSats(delSats)
        {
            diffTypes.insert(TypeID::prefitC);
        }


        /** Common constructor taking a gnssRinex as reference station data.
         * By default it will delete satellites present in reference station data 
         * but missing in input data.
         *
         * @param gData     gnssRinex data object holding the reference station data.
         * @param difftype  TypeID of data values to be differenced.
         * @param delSats   Boolean value setting if satellites present in reference station data but missing in input data will be deleted from the later (this is the default).
         */
        DeltaOp(const gnssRinex& gData, const TypeID& difftype, const bool& delSats=true) : refData(gData.body), deleteMissingSats(delSats)
        {
            diffTypes.insert(difftype);
        }


        /** Common constructor taking a gnssRinex as reference station data.
         * By default it will delete satellites present in reference station data 
         * but missing in input data.
         *
         * @param gData     gnssRinex data object holding the reference station data.
         * @param diffSet   TypeIDSet of data values to be differenced.
         * @param delSats   Boolean value setting if satellites present in reference station data but missing in input data will be deleted from the later (this is the default).
         */
        DeltaOp(const gnssRinex& gData, const TypeIDSet& diffSet, const bool& delSats=true) : refData(gData.body), deleteMissingSats(delSats), diffTypes(diffSet) {}



        /** Method to set the satTypeValueMap data object holding the reference station data.
         * @param gData     satTypeValueMap data object holding the reference station data.
         */
        virtual void setRefData(const satTypeValueMap& gData)
        {
            refData = gData;
        };


        /// Method to get the satTypeValueMap data object holding the reference station data.
        virtual satTypeValueMap getRefData() const
        {
           return refData;
        };


        /** Method to set if satellites present in reference station data but missing in input data will be deleted from the later (this is the default).
         * @param deleteSats    Boolean stating if satellites will be deleted or not.
         */
        virtual void setDeleteMissingSats(const bool& deleteSats)
        {
            deleteMissingSats = deleteSats;
        };


        /// Method to get the deleteMissingSats field value (i.e., if missing satellites will be deleted or not).
        virtual bool getDeleteMissingSats() const
        {
           return deleteMissingSats;
        };


        /** Method to set the data values to be differenced. The previous type values will be deleted. If this is not what you want, see method addDiffType.
         * @param difftype      TypeID of data values to be differenced.
         */
        virtual void setDiffType(const TypeID& difftype)
        {
            diffTypes.clear();      // Delete the previous types
            diffTypes.insert(difftype);
        };


        /** Method to add a data value type to be differenced.
         * @param difftype      TypeID of data values to be added to the ones being differenced.
         */
        virtual void addDiffType(const TypeID& difftype)
        {
            diffTypes.insert(difftype);
        };


        /** Method to set the data values to be differenced. The previous type values will be deleted. If this is not what you want, see method addDiffType.
         * @param diffSet       TypeIDSet of data values to be differenced.
         */
        virtual void setDiffTypeSet(const TypeIDSet& diffSet)
        {
            diffTypes.clear();      // Delete the previous types
            diffTypes = diffSet;
        };


        /** Method to add a set of data value types to be differenced.
         * @param diffSet       TypeIDSet of data values to be added to the ones being differenced.
         */
        virtual void addDiffTypeSet(const TypeIDSet& diffSet)
        {
            TypeIDSet::const_iterator pos;
            for (pos = diffSet.begin(); pos != diffSet.end(); ++pos) diffTypes.insert(*pos);
        };


        /// Method to get the set of data value types to be differenced.
        virtual TypeIDSet getDiffTypeSet() const
        {
           return diffTypes;
        };


        /** Returns a reference to a satTypeValueMap object after differencing the data type values given in the diffTypes field with respect to reference station data in refData field.
         *
         * @param gData     Data object holding the data.
         */
        virtual satTypeValueMap& Difference(satTypeValueMap& gData);


        /** Returns a reference to a gnssSatTypeValue object after differencing the data type values given in the diffTypes field with respect to reference station data in refData field.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssSatTypeValue& Difference(gnssSatTypeValue& gData) 
        {
            (*this).Difference(gData.body);
            return gData;
        };


        /** Returns a reference to a gnnsRinex object after differencing the data type values given in the diffTypes field with respect to reference station data in refData field.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssRinex& Difference(gnssRinex& gData)
        {
            (*this).Difference(gData.body);
            return gData;
        };


        /// Destructor.
        virtual ~DeltaOp() {};


    private:


        /// satTypeValueMap data structure containing reference station data.
        satTypeValueMap refData;


        /// Flag indicating if satellites present in reference station data but missing in input data will be deleted.
        bool deleteMissingSats;



        /// Set (TypeIDSet) containing the types of data to be differenced.
        TypeIDSet diffTypes;


   }; // class DeltaOp


    /// Input operator from gnssSatTypeValue to DeltaOp.
    inline gnssSatTypeValue& operator>>(gnssSatTypeValue& gData, DeltaOp& delta) 
    {
            delta.Difference(gData);
            return gData;
    }


    /// Input operator from gnssRinex to DeltaOp.
    inline gnssRinex& operator>>(gnssRinex& gData, DeltaOp& delta) 
    {
            delta.Difference(gData);
            return gData;
    }

   //@}

} // namespace

#endif
