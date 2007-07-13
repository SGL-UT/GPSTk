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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2006, 2007
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
       * This class may be used either in a Vector- and Matrix-oriented way, or
       * with GNSS data structure objects from "DataStructures" class. In any
       * case, it is intented to be used with stations where the position 
       * changes with time.
       *
       * A typical way to use this class with GNSS data structures follows:
       *
       * @code
       *   RinexObsStream rin("ebre0300.02o");  // Data stream
       *   RinexNavStream rnavin("brdc0300.02n");   // Ephemeris data stream
       *   RinexNavData rNavData;
       *   BCEphemerisStore bceStore;
       *   while (rnavin >> rNavData) bceStore.addEphemeris(rNavData);
       *   bceStore.SearchPast();  // This is the default
       *
       *   RinexNavHeader rNavHeader;
       *   IonoModelStore ionoStore;
       *   IonoModel ioModel;
       *   rnavin >> rNavHeader;    // Read navigation RINEX header
       *   ioModel.setModel(rNavHeader.ionAlpha, rNavHeader.ionBeta);
       *   ionoStore.addIonoModel(DayTime::BEGINNING_OF_TIME, ioModel);
       *
       *   // EBRE station nominal position
       *   Position nominalPos(4833520.3800, 41536.8300, 4147461.2800);
       *
       *   // Declare a tropospheric model object, setting the defaults
       *   MOPSTropModel mopsTM(nominalPos.getAltitude(), nominalPos.getGeodeticLatitude(), 30);
       *
       *   // Declare the modeler object, setting all the parameters in one pass
       *   // As stated, it will compute the model using the C1 observable
       *   ModeledPR model(ionoStore, mopsTM, bceStore, TypeID::C1);
       *
       *   // Set the initial apriori position using the Bancroft method
       *   model.Prepare();
       *
       *   gnssRinex gRin;
       *
       *   while(rin >> gRin) {
       *      gRin >> model;
       *   }
       * @endcode
       *
       * The "ModeledPR" object will visit every satellite in the 
       * GNSS data structure that is "gRin" and will try to compute its 
       * model: Prefit residual, geometric distance, relativity delay,
       * ionospheric/tropospheric corrections, geometry matrix, etc.
       *
       * When used with the ">>" operator, this class returns the same incoming
       * data structure with the extra data inserted along their corresponding
       * satellites. Be warned that if a given satellite does not 
       * have the observations required, it will be summarily deleted from the data
       * structure.
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
         * ionospheric and tropospheric models, ephemeris to be used, default observable
         * and whether TGD will be computed or not.
         *
         * This constructor is meant to be used when working with GNSS data structures
         * in order to set the basic parameters from the beginning.
         *
         * @param RxCoordinates Initial receiver coordinates.
         * @param dIonoModel    Ionospheric model to be used by default.
         * @param dTropoModel   Tropospheric model to be used by default.
         * @param dEphemeris    EphemerisStore object to be used by default.
         * @param dObservable   Observable type to be used by default.
         * @param usetgd        Whether TGD will be used by default or not.
         *
         * @sa DataStructures.hpp.
         */
        ModeledPR(const Position& RxCoordinates, IonoModelStore& dIonoModel, TropModel& dTropoModel, EphemerisStore& dEphemeris, const TypeID& dObservable, bool usetgd = true) throw(Exception) { 
            InitializeValues();
            Prepare(RxCoordinates);
            setDefaultIonoModel(dIonoModel);
            setDefaultTropoModel(dTropoModel);
            setDefaultObservable(dObservable);
            setDefaultEphemeris(dEphemeris);
            useTGD = usetgd;
        };


        /** Explicit constructor, taking as input initial receiver coordinates, default
         * ionospheric model, ephemeris to be used, default observable and whether TGD
         * will be computed or not.
         *
         * The default tropospheric model will be set to NULL.
         *
         * This constructor is meant to be used when working with GNSS data structures
         * in order to set the basic parameters from the beginning.
         *
         * @param RxCoordinates Initial receiver coordinates.
         * @param dIonoModel    Ionospheric model to be used by default.
         * @param dEphemeris    EphemerisStore object to be used by default.
         * @param dObservable   Observable type to be used by default.
         * @param usetgd        Whether TGD will be used by default or not.
         *
         * @sa DataStructures.hpp.
         */
        ModeledPR(const Position& RxCoordinates, IonoModelStore& dIonoModel, EphemerisStore& dEphemeris, const TypeID& dObservable, bool usetgd = true) throw(Exception) { 
            InitializeValues();
            Prepare(RxCoordinates);
            setDefaultIonoModel(dIonoModel);
            pDefaultTropoModel = NULL;
            setDefaultObservable(dObservable);
            setDefaultEphemeris(dEphemeris);
            useTGD = usetgd;
        };


        /** Explicit constructor, taking as input initial receiver coordinates, default
         * tropospheric model, ephemeris to be used, default observable and whether TGD
         * will be computed or not.
         *
         * The default ionospheric model will be set to NULL.
         *
         * This constructor is meant to be used when working with GNSS data structures
         * in order to set the basic parameters from the beginning.
         *
         * @param RxCoordinates Initial receiver coordinates.
         * @param dTropoModel   Tropospheric model to be used by default.
         * @param dEphemeris    EphemerisStore object to be used by default.
         * @param dObservable   Observable type to be used by default.
         * @param usetgd        Whether TGD will be used by default or not.
         *
         * @sa DataStructures.hpp.
         */
        ModeledPR(const Position& RxCoordinates, TropModel& dTropoModel, EphemerisStore& dEphemeris, const TypeID& dObservable, bool usetgd = true) throw(Exception) { 
            InitializeValues();
            Prepare(RxCoordinates);
            pDefaultIonoModel = NULL;
            setDefaultTropoModel(dTropoModel);
            setDefaultObservable(dObservable);
            setDefaultEphemeris(dEphemeris);
            useTGD = usetgd;
        };


        /** Explicit constructor, taking as input initial receiver coordinates,
         * ephemeris to be used, default observable and whether TGD will be 
         * computed or not.
         *
         * Both the tropospheric and ionospheric models will be set to NULL.
         *
         * This constructor is meant to be used when working with GNSS data structures
         * in order to set the basic parameters from the beginning.
         *
         * @param RxCoordinates Initial receiver coordinates.
         * @param dEphemeris    EphemerisStore object to be used by default.
         * @param dObservable   Observable type to be used by default.
         * @param usetgd        Whether TGD will be used by default or not.
         *
         * @sa DataStructures.hpp.
         */
        ModeledPR(const Position& RxCoordinates, EphemerisStore& dEphemeris, const TypeID& dObservable, bool usetgd = true) throw(Exception) { 
            InitializeValues();
            Prepare(RxCoordinates);
            pDefaultIonoModel = NULL;
            pDefaultTropoModel = NULL;
            setDefaultObservable(dObservable);
            setDefaultEphemeris(dEphemeris);
            useTGD = usetgd;
        };


        /** Explicit constructor, taking as input default ionospheric and tropospheric
         * models, ephemeris to be used, default observable and whether TGD will be
         * computed or not.
         *
         * This constructor is meant to be used when working with GNSS data structures
         * in order to set the basic parameters from the beginning.
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


        /** Explicit constructor, taking as input default ionospheric model, 
         * ephemeris to be used, default observable and whether TGD will be
         * computed or not.
         *
         * This constructor is meant to be used when working with GNSS data structures
         * in order to set the basic parameters from the beginning.
         *
         * @param dIonoModel    Ionospheric model to be used by default.
         * @param dObservable   Observable type to be used by default.
         * @param dEphemeris    EphemerisStore object to be used by default.
         * @param usetgd        Whether TGD will be used by default or not.
         *
         * @sa DataStructures.hpp.
         */
        ModeledPR(IonoModelStore& dIonoModel, EphemerisStore& dEphemeris, const TypeID& dObservable, bool usetgd = true) throw(Exception) { 
            InitializeValues();
            setDefaultIonoModel(dIonoModel);
            pDefaultTropoModel = NULL;
            setDefaultObservable(dObservable);
            setDefaultEphemeris(dEphemeris);
            useTGD = usetgd;
        };


        /** Explicit constructor, taking as input default tropospheric model, 
         * ephemeris to be used, default observable and whether TGD will be
         * computed or not.
         *
         * This constructor is meant to be used when working with GNSS data structures
         * in order to set the basic parameters from the beginning.
         *
         * @param dTropoModel   Tropospheric model to be used by default.
         * @param dObservable   Observable type to be used by default.
         * @param dEphemeris    EphemerisStore object to be used by default.
         * @param usetgd        Whether TGD will be used by default or not.
         *
         * @sa DataStructures.hpp.
         */
        ModeledPR(TropModel& dTropoModel, EphemerisStore& dEphemeris, const TypeID& dObservable, bool usetgd = true) throw(Exception) { 
            InitializeValues();
            pDefaultIonoModel = NULL;
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
        virtual int Prepare(const DayTime& Tr, std::vector<SatID>& Satellite, std::vector<double>& Pseudorange, const EphemerisStore& Eph);


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
        virtual int Prepare(const DayTime& Tr, const Vector<SatID>& Satellite, const Vector<double>& Pseudorange, const EphemerisStore& Eph) 
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
         * @param time      DayTime object for this epoch
         * @param data      A satTypeValueMap data structure holding the data
         *
         * @return
         *  0 if OK
         *  -1 if problems arose
         */
        virtual inline int Prepare(const DayTime& time, const satTypeValueMap& data)
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


        /** Method to set an a priori position of receiver using Bancroft method. Intended to be used
         * with GNSS data structures.
         *
         * @param gData         GNSS data structure to be used
         *
         * @return
         *  0 if OK
         *  -1 if problems arose
         */
        virtual inline int Prepare(const gnssSatTypeValue& gData)
        {
            return ((*this).Prepare(gData.header.epoch, gData.body));
        };


        /** Method to set the initial (a priori) position of receiver before 
         * Compute() method.
         * @return
         *  0 if OK
         *  -1 if problems arose
         */
        virtual int Prepare(const double& aRx, const double& bRx, const double& cRx, 
            Position::CoordinateSystem s=Position::Cartesian,
            GeoidModel *geoid=NULL) throw(GeometryException);


        /** Method to set the initial (a priori) position of receiver before 
         * Compute() method.
         * @return
         *  0 if OK
         *  -1 if problems arose
         */
        virtual int Prepare(const Position& RxCoordinates) throw(GeometryException);


        /** Returns a satTypeValueMap object, adding the new data generated when calling a modeling object.
         *
         * @param time      Epoch.
         * @param gData     Data object holding the data.
         */
        virtual satTypeValueMap& processModel(const DayTime& time, satTypeValueMap& gData) throw(Exception);


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
