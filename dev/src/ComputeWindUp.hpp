
/**
 * @file ComputeWindUp.hpp
 * This class smoothes a given code observable using the corresponding phase observable.
 */

#ifndef COMPUTE_WINDUP_GPSTK
#define COMPUTE_WINDUP_GPSTK

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



#include <string>
#include "ProcessingClass.hpp"
#include "Triple.hpp"
#include "Position.hpp"
#include "SunPosition.hpp"
#include "XvtStore.hpp"
#include "SatDataReader.hpp"
#include "geometry.hpp"



namespace gpstk
{

    /** @addtogroup DataStructures */
    //@{


    /** This class computes the wind-up effect on the phase observables, correcting them.
     * This class is meant to be used with the GNSS data structures objects
     * found in "DataStructures" class.
     *
     * A typical way to use this class follows:
     *
     * @code
     *   RinexObsStream rin("ebre0300.02o");
     *
     *   gnssRinex gRin;
     *   ComputeWindUp windup;
     *
     *   while(rin >> gRin) {
     *      gRin >> markCSC1 >> smoothC1;
     *   }
     * @endcode
     *
     * The "ComputeWindUp" object will visit every satellite in the GNSS data
     * structure that is "gRin" and will smooth the given code observation using
     * the corresponding phase observation.
     *
     * By default, the algorithm will cumpute wind-up on L1 and L2 observables. 
     * You can change these settings with the appropriate set methods.
     *
     * When used with the ">>" operator, this class returns the same incoming
     * data structure with the phase observations corrected. Be warned that if 
     * a given satellite does not have the observations required, it will be 
     * summarily deleted from the data structure.
     *
     * \warning ComputeWindUp objects store their internal state, so you MUST 
     * NOT use the SAME object to process DIFFERENT data streams.
     *
     */
    class ComputeWindUp : public ProcessingClass
    {
    public:

        /** Common constructor
         *
         * @param ephem     Satellite ephemeris.
         * @param stapos    Nominal position of receiver station.
         * @param filename  Name of "PRN_GPS"-like file containing satellite data.
         *
         * @warning If filename is not given, this class will look for a file
         * named "PRN_GPS" in the current directory.
         */
        ComputeWindUp(XvtStore<SatID>& ephem, const Position& stapos, string filename="PRN_GPS") : ephemeris(ephem), nominalPos(stapos), satData(filename), fileData(filename)
        {

            setIndex();

        };


        /** Returns a satTypeValueMap object, adding the new data generated when calling this object.
         *
         * @param time      Epoch corresponding to the data.
         * @param gData     Data object holding the data.
         */
        virtual satTypeValueMap& Process(const DayTime& time, satTypeValueMap& gData);


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


        /// Returns name of "PRN_GPS"-like file containing satellite data.
        virtual string getFilename(void) const
        {
            return fileData;
        };


        /** Sets name of "PRN_GPS"-like file containing satellite data.
         * @param name      Name of satellite data file.
         */
        virtual void setFilename(const string& name)
        {
            fileData = name;
            satData.open(fileData);

            return;
        };


        /// Returns nominal position of receiver station.
        virtual Position getNominalPosition(void) const
        {
            return nominalPos;
        };


        /** Sets  nominal position of receiver station.
         * @param stapos    Nominal position of receiver station.
         */
        virtual void setNominalPosition(const Position& stapos)
        {
            nominalPos = stapos;

            return;
        };


        /// Returns a reference to satellite ephemeris object currently in use.
        virtual XvtStore<SatID>& getEphemeris(void) const
        {
            return ephemeris;
        };


        /** Sets satellite ephemeris object to be used.
         * @param ephem     Satellite ephemeris object.
         */
        virtual void setEphemeris(const XvtStore<SatID>& ephem)
        {
            ephemeris = ephem;

            return;
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
        virtual ~ComputeWindUp() {};


    private:


        /// Satellite ephemeris to be used
        XvtStore<SatID>& ephemeris;


        /// Receiver position
        Position nominalPos;


        /// Object to read satellite data file (PRN_GPS)
        SatDataReader satData;


        /// Name of "PRN_GPS"-like file containing satellite data.
        string fileData;


        /// A structure used to store phase data.
        struct phaseData
        {
            // Default constructor initializing the data in the structure
            phaseData() : previousPhase(0.0) {};

            double previousPhase;      ///< Previous phase.
        };


        /// Map to store station phase data
        map<SatID, phaseData> phase_station;


        /// Map to store satellite phase data
        map<SatID, phaseData> phase_satellite;


        /** Compute the value of the wind-up, in cycles.
         * @param sat       Satellite IDmake
         * @param time      Epoch of interest
         * @param satpos    Satellite position, as a Triple
         * @param sunpos    Sun position, as a Triple
         * @return Wind-up computation, in cycles
         */
        virtual double getWindUp(const SatID& sat, const DayTime& time, const Triple& satpos, const Triple& sunpos);



        /// Initial index assigned to this class.
        static int classIndex;

        /// Index belonging to this object.
        int index;

        /// Sets the index and increment classIndex.
        void setIndex(void) { (*this).index = classIndex++; }; 


   }; // end class ComputeWindUp


   //@}
   
}

#endif
