
/**
 * @file ComputeIURAWeights.hpp
 * This is the base class to ease computing combination of data for GNSS data structures.
 */

#ifndef Compute_IURA_WEIGHTS_GPSTK
#define Compute_IURA_WEIGHTS_GPSTK

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
//  Dagoberto Salazar - gAGE. 2007
//
//============================================================================



#include "WeightBase.hpp"
#include "EngEphemeris.hpp"
#include "TabularEphemerisStore.hpp"
#include "BCEphemerisStore.hpp"
#include "DataStructures.hpp"


namespace gpstk
{

    /** @addtogroup DataStructures */
    //@{


    /** This class computes satellites weights based on URA Index.
     * This class is meant to be used with the GNSS data structures objects
     * found in "DataStructures" class.
     *
     * A typical way to use this class follows:
     *
     * @code
     *   RinexObsStream rin("ebre0300.02o");
     *   RinexNavStream rnavin("brdc0300.02n");
     *   BCEphemerisStore bceStore;
     *   while (rnavin >> rNavData) bceStore.addEphemeris(rNavData);
     *   bceStore.SearchPast();  // This is the default
     *
     *   gnssRinex gRin;
     *   ComputeIURAWeights iuraW(bceStore);
     *
     *   while(rin >> gRin) {
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
     * satellites. Be warned that if a given satellite is not found, it will 
     * be summarily deleted from the data structure.
     *
     */

    class ComputeIURAWeights : public WeightBase
    {
    public:

        /// Default constructor
        ComputeIURAWeights() : pBCEphemeris(NULL), pTabEphemeris(NULL) {};


        /** Common constructor
         *
         * @param bcephem   BCEphemerisStore object holding the ephemeris.
         */
        ComputeIURAWeights(BCEphemerisStore& bcephem) : pBCEphemeris(&bcephem), pTabEphemeris(NULL) {};


        /** Common constructor
         *
         * @param tabephem  TabularEphemerisStore object holding the ephemeris.
         */
        ComputeIURAWeights(TabularEphemerisStore& tabephem) : pBCEphemeris(NULL), pTabEphemeris(&tabephem) {};


        /** Common constructor
         *
         * @param ephem  EphemerisStore object holding the ephemeris.
         */
        ComputeIURAWeights(EphemerisStore& ephem)
        {
            setDefaultEphemeris(ephem);
        };


        /** Returns a satTypeValueMap object, adding the new data generated when calling this object.
         *
         * @param gData     Data object holding the data.
         */
        virtual satTypeValueMap& getIURAWeight(const DayTime& time, satTypeValueMap& gData)
        {
            double weight(0.000001);   // By default a very small value
            SatIDSet satRejectedSet;

            // Loop through all the satellites
            satTypeValueMap::iterator it;
            for (it = gData.begin(); it != gData.end(); ++it) 
            {
                try
                {
                    // Try to extract the weight value
                    if (pBCEphemeris != NULL)
                    {
                        weight = getWeight( ((*it).first), time, pBCEphemeris );
                    } else {
                        if (pTabEphemeris != NULL) weight = getWeight( ((*it).first), time, pTabEphemeris );
                    }
                }
                catch(...)
                {
                    // If some value is missing, then schedule this satellite for removal
                    satRejectedSet.insert( (*it).first );
                    continue;
                }
                // If everything is OK, then get the new value inside the structure
                (*it).second[TypeID::weight] = weight;
            }
            // Remove satellites with missing data
            gData.removeSatID(satRejectedSet);

            return gData;
        };


        /** Returns a gnnsSatTypeValue object, adding the new data generated when calling this object.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssSatTypeValue& getIURAWeight(gnssSatTypeValue& gData)
        {
            (*this).getIURAWeight(gData.header.epoch, gData.body);
            return gData;
        };


        /** Returns a gnnsRinex object, adding the new data generated when calling this object.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssRinex& getIURAWeight(gnssRinex& gData)
        {
            (*this).getIURAWeight(gData.header.epoch, gData.body);
            return gData;
        };


        /** Method to set the default ephemeris to be used with GNSS data structures.
         * @param ephem     EphemerisStore object to be used
         */
        virtual void setDefaultEphemeris(EphemerisStore& ephem)
        {
            // Let's check what type ephem belongs to
            if (dynamic_cast<BCEphemerisStore*>(&ephem))
            {
                pBCEphemeris = dynamic_cast<BCEphemerisStore*>(&ephem);
                pTabEphemeris = NULL;
            } else {
                pBCEphemeris = NULL;
                pTabEphemeris = dynamic_cast<TabularEphemerisStore*>(&ephem);
            }
        };


        /** Method to set the default ephemeris to be used with GNSS data structures.
         * @param ephem     BCEphemerisStore object to be used
         */
        virtual void setDefaultEphemeris(BCEphemerisStore& ephem)
        {
            pBCEphemeris = &ephem;
            pTabEphemeris = NULL;
        };


        /** Method to set the default ephemeris to be used with GNSS data structures.
         * @param ephem     TabularEphemerisStore object to be used
         */
        virtual void setDefaultEphemeris(TabularEphemerisStore& ephem)
        {
            pBCEphemeris = NULL;
            pTabEphemeris = &ephem;
        };


        /// Destructor
        virtual ~ComputeIURAWeights() {};


    private:

        /// Pointer to default broadcast ephemeris to be used.
        BCEphemerisStore* pBCEphemeris;

        /// Pointer to default precise ephemeris to be used.
        TabularEphemerisStore* pTabEphemeris;


        /** Method to really get the weight of a given satellite.
         *
         * @param sat           Satellite
         * @param time          Epoch
         * @param preciseEph    Precise ephemerisStore object to be used
         */
        virtual double getWeight(const SatID& sat, const DayTime& time, const TabularEphemerisStore* preciseEph) throw(InvalidWeights)
        {
            try
            {
                // Look if this satellite is present in ephemeris
                preciseEph->getSatXvt(sat, time);
            }
            catch(...)
            {
                InvalidWeights eWeight("Satellite not found.");
                GPSTK_THROW(eWeight);
            }
            // An URA of 0.1 m is assumed for all satellites, so sigma=0.1*0.1= 0.01 m^2
            return 100.0;
        };


        /** Method to really get the weight of a given satellite.
         *
         * @param sat       Satellite
         * @param time      Epoch
         * @param bcEph     Broadcast EphemerisStore object to be used
         */
        virtual double getWeight(const SatID& sat, const DayTime& time, const BCEphemerisStore* bcEph) throw(InvalidWeights)
        {
            int iura(1000000);   // By default a very big value
            double sigma(1000000.0);
            EngEphemeris engEph;

            try
            {
                // Look if this satellite is present in ephemeris
                engEph = bcEph->findEphemeris(sat, time);
                // If so, wet the IURA
                iura = engEph.getAccFlag();
            }
            catch(...)
            {
                InvalidWeights eWeight("Satellite not found.");
                GPSTK_THROW(eWeight);
            }
            // Compute and return the weight
            sigma = gpstk::ura2nominalAccuracy(iura);
            return ( 1.0 / (sigma*sigma) );
        };


   }; // end class ComputeIURAWeights


    /// Input operator from gnssSatTypeValue to ComputeIURAWeights.
    inline gnssSatTypeValue& operator>>(gnssSatTypeValue& gData, ComputeIURAWeights& right)
    {
            right.getIURAWeight(gData);
            return gData;
    }


    /// Input operator from gnssRinex to ComputeIURAWeights.
    inline gnssRinex& operator>>(gnssRinex& gData, ComputeIURAWeights& right)
    {
            right.getIURAWeight(gData);
            return gData;
    }

   

   //@}
   
}

#endif
