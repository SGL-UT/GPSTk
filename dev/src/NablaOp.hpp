
/**
 * @file NablaOp.hpp
 * This is a class to apply the Nabla operator (differences on satellite-related data) to GNSS data structures.
 */

#ifndef NABLAOP_HPP
#define NABLAOP_HPP

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
       * This class applies the Nabla operator (differences on satellite-related data) 
       * to GNSS data structures.
       *
       * A typical way to use this class follows:
       *
       * @code
       *   // Input observation file stream
       *   RinexObsStream rin("ebre0300.02o");
       *   // Reference position of receiver station
       *   Position nominalPos(4833520.2269, 41537.00768, 4147461.489);
       *
       *   // Some more code and definitions here...
       *
       *   gnssRinex gRin;  // GNSS data structure for rover data
       *
       *   // Set defaults of models. A typical C1-based modeling is used
       *   ModeledPR model(nominalPos, ionoStore, mopsTM, bceStore, TypeID::C1, true);
       *
       *   // Create an object to compute the single differences on satellite-related
       *   // data. Highest satellite will be used as reference and prefit residuals will
       *   // be differenced (this is the default)
       *   NablaOp nabla;
       *
       *   while(rin >> gRin) {
       *
       *      // By default, difference is applied on code prefit residuals, and therefore
       *      // you need a modeler object before nabla object
       *      gRin >> model >> nabla;
       *   }
       *
       * @endcode
       *
       * The "NablaOp" object will visit every satellite in the GNSS data structure that
       * is "gRin" and, unless told otherwise with "setRefSat()" method, will pick up as
       * reference satellite the satellite with the highest elevation.
       *
       * Afterwards, it will REMOVE reference satellite data from the GNSS data structure
       * and will substract the specified type or types (code prefit residuals by default) 
       * from the corresponding data in the rest of the "gRef" data structure.
       * 
       * Take notice that in the default case the code prefit residuals were computed by 
       * the "ModeledPR" and "ModeledReferencePR" objects, so those steps are mandatory.
       *
       * Be warned that, by default, if a given satellite does not have in "gRin" the data
       * required to be differenced, it will be summarily deleted from the data structure.
       *
       * @sa DeltaOp.hpp for differences on ground-related data.
       *
       */
    class NablaOp
    {
    public:

        /// Default constructor. By default it will difference prefitC data and will take as reference satellite the one with the highest elevation.
        NablaOp() : lookReferenceSat(true)
        {
            diffTypes.insert(TypeID::prefitC);
        };


        /** Common constructor taking as input the reference satellite to be used.
         *
         * @param rSat      SatID of satellite to be used as reference.
         */
        NablaOp(const SatID& rSat) : refSat(rSat), lookReferenceSat(false)
        {
            diffTypes.insert(TypeID::prefitC);
        };


        /** Common constructor taking as input the type of data values to be differenced.
         *
         * @param difftype  TypeID of data values to be differenced.
         */
        NablaOp(const TypeID& difftype) : lookReferenceSat(true)
        {
            diffTypes.insert(difftype);
        }


        /** Common constructor taking as input the type of data values to be differenced and the reference satellite.
         *
         * @param rSat      SatID of satellite to be used as reference.
         * @param difftype  TypeID of data values to be differenced.
         */
        NablaOp(const SatID& rSat, const TypeID& difftype) : refSat(rSat), lookReferenceSat(false)
        {
            diffTypes.insert(difftype);
        }


        /** Common constructor taking as input a set of types of data values to be differenced and the reference satellite.
         *
         * @param rSat      SatID of satellite to be used as reference.
         * @param diffSet   TypeIDSet of data values to be differenced.
         */
        NablaOp(const SatID& rSat, const TypeIDSet& diffSet) : refSat(rSat), lookReferenceSat(false), diffTypes(diffSet) {}


        /** Method to set the reference satellite to be used. It is not common to use this method.
         * @param rSat      SatID of reference satellite to be used.
         */
        virtual void setRefSat(const SatID& rSat)
        {
            refSat = rSat;
            lookReferenceSat = false;
        };


        /// Method to set that the reference satellite will be the one with highest elevation. This is the default.
        virtual void useHighestSat()
        {
            lookReferenceSat = true;
        };


        /// Method to get the reference satellite being used.
        virtual SatID getRefSat() const
        {
           return refSat;
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
        virtual ~NablaOp() {};


    private:


        /// SatID of reference satellite.
        SatID refSat;


        /// Flag indicating if we have to look for a reference satellite.
        bool lookReferenceSat;



        /// Set (TypeIDSet) containing the types of data to be differenced.
        TypeIDSet diffTypes;


   }; // class NablaOp


    /// Input operator from gnssSatTypeValue to NablaOp.
    inline gnssSatTypeValue& operator>>(gnssSatTypeValue& gData, NablaOp& nabla) 
    {
            nabla.Difference(gData);
            return gData;
    }


    /// Input operator from gnssRinex to NablaOp.
    inline gnssRinex& operator>>(gnssRinex& gData, NablaOp& nabla) 
    {
            nabla.Difference(gData);
            return gData;
    }

   //@}

} // namespace

#endif
