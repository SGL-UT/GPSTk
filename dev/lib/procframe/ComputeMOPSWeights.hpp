
/**
 * @file ComputeMOPSWeights.hpp
 * This class computes satellites weights based on the Appendix J of MOPS C, and is meant to be used with GNSS data structures.
 */

#ifndef COMPUTE_MOPS_WEIGHTS_GPSTK
#define COMPUTE_MOPS_WEIGHTS_GPSTK

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2006, 2007
//
//============================================================================



#include "WeightBase.hpp"
#include "EngEphemeris.hpp"
#include "TabularEphemerisStore.hpp"
#include "GPSEphemerisStore.hpp"
#include "ComputeIURAWeights.hpp"
#include "TropModel.hpp"
#include "geometry.hpp"             // DEG_TO_RAD

namespace gpstk
{

    /** @addtogroup DataStructures */
    //@{


    /** This class computes satellites weights based on the Appendix J of MOPS C.
     * It is meant to be used with the GNSS data structures objects
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
     *   RinexNavHeader rNavHeader;
     *   IonoModelStore ionoStore;
     *   IonoModel ioModel;
     *   rnavin >> rNavHeader;
     *   ioModel.setModel(rNavHeader.ionAlpha, rNavHeader.ionBeta);
     *   ionoStore.addIonoModel(DayTime::BEGINNING_OF_TIME, ioModel);
     *
     *   Position nominalPos(4833520.2269, 41537.00768, 4147461.489);
     *
     *   MOPSTropModel mopsTM;
     *   mopsTM.setReceiverHeight(nominalPos.getAltitude());
     *   mopsTM.setReceiverLatitude(nominalPos.getGeodeticLatitude());
     *   mopsTM.setDayOfYear(30);    // Day of the year
     *
     *   ModeledPR modelRef(nominalPos, ionoStore, mopsTM, bceStore, TypeID::C1, true);
     *
     *   gnssRinex gRin;
     *   ComputeMOPSWeights mopsW(nominalPos, bceStore);
     *
     *   while(rin >> gRin) {
     *      gRin >> modelRef >> mopsW;
     *   }
     * @endcode
     *
     * The "ComputeMOPSWeights" object will visit every satellite in the
     * GNSS data structure that is "gRin" and will try to compute its weight
     * based on the MOPS algorithm.
     *
     * It is very important to note that MOPS algorithm demands a proper
     * modeling  of the observable before starting, otherwise it won't work.
     * That is the reason of the long initialization phase, where the ionospheric
     * model (ionoStore), the MOPS tropospheric model (mopsTM) and the general
     * model (modelRef) objects are set up.
     *
     * When used with the ">>" operator, this class returns the same incoming
     * data structure with the weights inserted along their corresponding
     * satellites. Be warned that if it is not possible to compute the weight 
     * for a given satellite, it will be summarily deleted from the data
     * structure.
     *
     */

    class ComputeMOPSWeights : public ComputeIURAWeights
    {
    public:

        /// Default constructor. Generates an invalid object.
        ComputeMOPSWeights() : receiverClass(2)
        {
            pBCEphemeris = NULL;
            pTabEphemeris = NULL;
            setIndex();
        }



        /** Common constructor
         *
         * @param pos       Reference position.
         * @param bcephem   GPSEphemerisStore object holding the ephemeris.
         * @param rxClass   Receiver class. By default, it is 2.
         */
        ComputeMOPSWeights(const Position& pos, GPSEphemerisStore& bcephem, int rxClass = 2) : receiverClass(rxClass), nominalPos(pos)
        {
            setDefaultEphemeris(bcephem);
            setIndex();
        };


        /** Common constructor
         *
         * @param pos       Reference position.
         * @param tabephem  TabularEphemerisStore object holding the ephemeris.
         * @param rxClass   Receiver class. By default, it is 2.
         */
        ComputeMOPSWeights(const Position& pos, TabularEphemerisStore& tabephem, int rxClass = 2) : receiverClass(rxClass), nominalPos(pos)
        {
            setDefaultEphemeris(tabephem);
            setIndex();
        };


        /** Returns a satTypeValueMap object, adding the new data generated when calling this object.
         *
         * @param time      Epoch corresponding to the data.
         * @param gData     Data object holding the data.
         */
        virtual satTypeValueMap& Process(const DayTime& time, satTypeValueMap& gData)
        {
            // IURA weights are needed
            ComputeIURAWeights::Process(time, gData);

            double weight(0.000001);   // By default a very small value
            SatIDSet satRejectedSet;

            // Loop through all the satellites
            satTypeValueMap::iterator it;
            for (it = gData.begin(); it != gData.end(); ++it) 
            {
                try
                {
                    weight = getWeight( ((*it).first), ((*it).second) );
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
        virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
        {
            (*this).Process(gData.header.epoch, gData.body);
            return gData;
        };


        /** Returns a gnnsRinex object, adding the new data generated when calling this object.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssRinex& Process(gnssRinex& gData)
        {
            (*this).Process(gData.header.epoch, gData.body);
            return gData;
        };


        /** Method to set the default ephemeris to be used with GNSS data structures.
         * @param ephem     TabularEphemerisStore object to be used
         */
        virtual void setPosition(const Position& pos)
        {
            nominalPos = pos;
        };


        /// Returns an index identifying this object.
        virtual int getIndex(void) const;


        /// Returns a string identifying this object.
        virtual std::string getClassName(void) const;


        /** Sets the index to a given arbitrary value. Use with caution.
         *
         * @param newindex      New integer index to be assigned to current object.
         */
        void setIndex(const int newindex) { (*this).index = newindex; };


        /// Destructor
        virtual ~ComputeMOPSWeights() {};


    private:

        /// Default receiver class (the usual value is 2).
        int receiverClass;


        /// Nominal position used for computing weights.
        Position nominalPos;


        /** Method to really get the MOPS weight of a given satellite.
         *
         * @param sat           Satellite
         *
         */
        virtual double getWeight(const SatID& sat, typeValueMap& tvMap) throw(InvalidWeights)
        {
            double weight(0.000001);    // Receiver noise sigma^2 in meters^2, by default a very big value

            double sigma2rx(1000000.0);    // Receiver noise sigma^2 in meters^2, by default a very big value
            if (receiverClass==1) sigma2rx = 0.25; else sigma2rx = 0.36;

            // Some extra variables. By default a very big value
            double sigma2ura(1000000.0), sigma2multipath(1000000.0), sigma2trop(1000000.0), sigma2uire(1000000.0);

            try
            {
                // We need a MOPSTropModel object. Parameters must be valid but they have no importance
                MOPSTropModel mopsTrop(0.0, 0.0, 1);
                // At first, the weight type have just the IURA weight
                sigma2ura = (1.0 / tvMap(TypeID::weight) );

                sigma2multipath = 0.13 + (0.53 * std::exp( - (tvMap(TypeID::elevation)) / 10.0));
                sigma2trop = mopsTrop.MOPSsigma2(tvMap(TypeID::elevation));
                sigma2uire = sigma2iono(tvMap(TypeID::ionoSlant), tvMap(TypeID::elevation), tvMap(TypeID::azimuth), nominalPos);
                weight = 1.0 / (sigma2rx + sigma2ura + sigma2multipath + sigma2trop + sigma2uire);
            }
            catch(...)
            {
                InvalidWeights eWeight("Problem when computing weights. Did you call a modeler class?.");
                GPSTK_THROW(eWeight);
            }

            return weight;
        };


        // Compute ionospheric sigma^2 according to Appendix J.2.3 and Appendix A.4.4.10.4 in MOPS-C
        double sigma2iono(const double& ionoCorrection, const double& elevation, const double& azimuth, const Position& rxPosition) throw(InvalidWeights)
        {
            // First, let's found magnetic latitude according to ICD-GPS-200, section 20.3.3.5.2.6
            double azRad = azimuth * DEG_TO_RAD;
            double elevRad = elevation * DEG_TO_RAD;
            double cosElev = std::cos(elevRad);
            double svE = elevation / 180.0;     // Semi-circles

            double phi_u = rxPosition.getGeodeticLatitude() / 180.0;        // Semi-circles
            double lambda_u = rxPosition.getLongitude() / 180.0;    // Semi-circles
      
            double psi = (0.0137 / (svE + 0.11)) - 0.022;       // Semi-circles
      
           double phi_i = phi_u + psi * std::cos(azRad);        // Semi-circles
            if (phi_i > 0.416)
                phi_i = 0.416;
            if (phi_i < -0.416)
                phi_i = -0.416;

            double lambda_i = lambda_u + ( psi * std::sin(azRad) / std::cos(phi_i*PI) ); // Semi-circles
      
            double phi_m = phi_i + 0.064 * std::cos((lambda_i - 1.617)*PI);     // Semi-circles

            // Convert magnetic latitude to degrees
            phi_m = std::abs(phi_m * 180.0);

            // Estimate vertical ionospheric delay according to MOPS-C
            double tau_vert;
            if ( (phi_m >= 0.0) && (phi_m <= 20.0) ) { tau_vert = 9.0; }
            else
            {
                if ( (phi_m > 20.0) && (phi_m <= 55.0) ) { tau_vert = 4.5; }
                else tau_vert = 6.0;
            }

            // Compute obliquity factor
            double fpp = ( 1.0 / (std::sqrt(1.0 - 0.898665418 * cosElev * cosElev)) );

            double sigma2uire = ( (ionoCorrection*ionoCorrection) / 25.0 );

            double fact = ( (fpp*tau_vert) * (fpp*tau_vert) );

            if (fact > sigma2uire) sigma2uire = fact;

            return sigma2uire;

        }  // End of sigma2iono()


        /// Initial index assigned to this class.
        static int classIndex;

        /// Index belonging to this object.
        int index;

        /// Sets the index and increment classIndex.
        void setIndex(void) { (*this).index = classIndex++; }; 


    }; // end class ComputeMOPSWeights


   //@}
   
}

#endif
