/**
 * @file ModeledPR.hpp
 * Class to compute modeled pseudoranges of a mobile receiver
 */

#ifndef GPSTK_MODELEDPR_HPP
#define GPSTK_MODELEDPR_HPP

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
//  Dagoberto Salazar - gAGE. 2006
//
//============================================================================



#include "ModeledReferencePR.hpp"
#include "Matrix.hpp"
#include "Bancroft.hpp"
#include "PRSolution.hpp"
#include "DataStructures.hpp"

namespace gpstk
{
      /** @addtogroup GPSsolutions */
      //@{

      /** This class compute modeled pseudoranges from satellites to a mobile receiver.
       *
       * The main difference between this class and ModeledReferencePR is that for a
       * mobile receiver we should "prepare" the computation giving an estimate of 
       * the mobile station position. This position may be the last known 
       * position or it may be estimated using a method such as Bancroft.
       *
       * Prepare() method is used for this.
       *
       * @sa ModeledPseudorangeBase.hpp and ModeledReferencePR.hpp for base classes.
       *
       */
   class ModeledPR : public ModeledReferencePR
   {
    public:

        /// Implicit constructor
        ModeledPR() : modelPrepared(false) {};


        /** Explicit constructor, taking as input initial receiver coordinates, default
         * ionospheric and tropospheric models, and default observable.
         *
         * @param RxCoordinates Initial receiver coordinates.
         * @param dIonoModel    Ionospheric model to be used by default.
         * @param dTropoModel   Tropospheric model to be used by default.
         * @param dObservable   Observable type to be used by default.
         * @param dEphemeris    EphemerisStore object to be used by default.
         * @param usetgd        Whether TGD will be used by default or not.
         *
         * @sa DataStructures.hpp.
         */
        ModeledPR(const Position& RxCoordinates, IonoModelStore& dIonoModel, TropModel& dTropoModel, EphemerisStore& dEphemeris, const TypeID& dObservable, bool usetgd = true) throw(Exception) { 
            InitializeValues();
            setInitialRxPosition(RxCoordinates);
            setDefaultIonoModel(dIonoModel);
            setDefaultTropoModel(dTropoModel);
            setDefaultObservable(dObservable);
            setDefaultEphemeris(dEphemeris);
            useTGD = usetgd;
        };


        /** Explicit constructor, taking as input default ionospheric and tropospheric models, and default observable.
         *
         * @param dIonoModel    Ionospheric model to be used by default.
         * @param dTropoModel   Tropospheric model to be used by default.
         * @param dObservable   Observable type to be used by default.
         * @param dEphemeris    EphemerisStore object to be used by default.
         * @param usetgd        Whether TGD will be used by default or not.
         *
         * @sa DataStructures.hpp.
         */
        ModeledPR(IonoModelStore& dIonoModel, TropModel& dTropoModel, EphemerisStore& dEphemeris, const TypeID& dObservable, bool usetgd = true) throw(Exception) { 
            InitializeValues();
            setDefaultIonoModel(dIonoModel);
            setDefaultTropoModel(dTropoModel);
            setDefaultObservable(dObservable);
            setDefaultEphemeris(dEphemeris);
            useTGD = usetgd;
        };


        /** Method to set an a priori position of receiver using Bancroft method.
         *
         * @param Tr            Time of observation
         * @param Satellite     Vector of satellites in view
         * @param Pseudorange   Pseudoranges measured from mobile to satellites
         * @param Eph           Satellites Ephemeris
         *
         * @return
         *  0 if OK
         *  -1 if problems arose
         */
        int Prepare(const DayTime& Tr, std::vector<SatID>& Satellite, std::vector<double>& Pseudorange, const EphemerisStore& Eph);


        /** Method to set an a priori position of receiver using Bancroft method.
         *
         * @param Tr            Time of observation
         * @param Satellite     Vector of satellites in view
         * @param Pseudorange   Pseudoranges measured from mobile to satellites
         * @param Eph           Satellites Ephemeris
         *
         * @return
         *  0 if OK
         *  -1 if problems arose
         */
        int Prepare(const DayTime& Tr, const Vector<SatID>& Satellite, const Vector<double>& Pseudorange, const EphemerisStore& Eph) 
        {
            int i;
            std::vector<SatID> vSat;
            std::vector<double> vPR;

            // Convert from gpstk::Vector to std::vector
            for (i = 0; i < (int)Satellite.size(); i++)
                vSat.push_back(Satellite[i]);

            for (i = 0; i < (int)Pseudorange.size(); i++)
                vPR.push_back(Pseudorange[i]);

            return Prepare(Tr, vSat, vPR, Eph);

        };


        /** Method to set an a priori position of receiver using Bancroft method. Intended to be used
         * with GNSS data structures.
         *
         * @param gData         GNSS data structure to be used
         *
         * @return
         *  0 if OK
         *  -1 if problems arose
         */
        int Prepare(const gnssSatTypeValue& gData)
        {
            int i;
            std::vector<SatID> vSat;
            std::vector<double> vPR;
            Vector<SatID> Satellite( gData.getVectorOfSatID() );
            Vector<double> Pseudorange( gData.getVectorOfTypeID( (*this).getDefaultObservable() ) );

            // Convert from gpstk::Vector to std::vector
            for (i = 0; i < (int)Satellite.size(); i++)
                vSat.push_back(Satellite[i]);

            for (i = 0; i < (int)Pseudorange.size(); i++)
                vPR.push_back(Pseudorange[i]);

            return Prepare(gData.header.epoch, vSat, vPR, (*( (*this).getDefaultEphemeris())) );

        };


        /** Method to set an a priori position of receiver using Bancroft method. Intended to be used
         * with GNSS data structures.
         *
         * @param time      DayTime object for this epoch
         * @param data      A satTypeValueMap data structure holding the data
         *
         * @return
         *  0 if OK
         *  -1 if problems arose
         */
        int Prepare(const DayTime& time, const satTypeValueMap& data)
        {
            int i;
            std::vector<SatID> vSat;
            std::vector<double> vPR;
            Vector<SatID> Satellite( data.getVectorOfSatID() );
            Vector<double> Pseudorange( data.getVectorOfTypeID( (*this).getDefaultObservable() ) );

            // Convert from gpstk::Vector to std::vector
            for (i = 0; i < (int)Satellite.size(); i++)
                vSat.push_back(Satellite[i]);

            for (i = 0; i < (int)Pseudorange.size(); i++)
                vPR.push_back(Pseudorange[i]);

            return Prepare(time, vSat, vPR, (*( (*this).getDefaultEphemeris())) );

        };


        /** Method to set the initial (a priori) position of receiver before 
         * Compute() method.
         * @return
         *  0 if OK
         *  -1 if problems arose
         */
        int Prepare(const double& aRx, const double& bRx, const double& cRx, 
            Position::CoordinateSystem s=Position::Cartesian,
            GeoidModel *geoid=NULL) throw(GeometryException);


        /** Method to set the initial (a priori) position of receiver before 
         * Compute() method.
         * @return
         *  0 if OK
         *  -1 if problems arose
         */
        int Prepare(const Position& RxCoordinates) throw(GeometryException);


        /// Method to get if the model has been prepared.
        inline bool getModelPrepared() const { return modelPrepared; };


        /** Method to forcefully set whether the model has been prepared.
         *
         * @param prepare       Boolean indicating whether the model has been prepared.
         */
        inline void setModelPrepared(const bool& prepare) { modelPrepared = prepare; };


        /// Destructor.
        virtual ~ModeledPR() throw() {};


    protected:

        bool modelPrepared;


   }; // class ModeledPR

   //@}

} // namespace

#endif
