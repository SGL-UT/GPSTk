
/**
 * @file ModelObsFixedStation.hpp
 * This is a class to compute modeled (corrected) observations from a reference station (whose position is known), using GNSS data structures.
 */

#ifndef GPSTK_MODELOBSFIXEDSTATION_HPP
#define GPSTK_MODELOBSFIXEDSTATION_HPP

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



#include "ProcessingClass.hpp"
#include "EphemerisRange.hpp"
#include "EngEphemeris.hpp"
#include "XvtStore.hpp"
#include "GPSEphemerisStore.hpp"
#include "TropModel.hpp"
#include "IonoModelStore.hpp"


namespace gpstk
{
    /** @addtogroup GPSsolutions */
      //@{

      /**
       * This class computes modeled (corrected) observations from a
       * reference station (whose position is known), using GNSS data
       * structures (GDS).
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
       *   gnssRinex gRin;  // GNSS data structure for fixed station data
       *
       *   // Set defaults of models. A typical C1-based modeling is used
       *   ModelObsFixedStation model(nominalPos, ionoStore, mopsTM, bceStore, TypeID::C1, true);
       *
       *   while(rin >> gRin) {
       *
       *      // Apply the model on the GDS
       *      gRin >> model;
       *   }
       *
       * @endcode
       *
       * The "ModelObsFixedStation" object will visit every satellite in
       * the GNSS data structure that is "gRin" and will try to compute
       * its model: Prefit residual, geometric distance, relativity delay,
       * ionospheric/tropospheric corrections, geometry matrix, etc.
       *
       * When used with the ">>" operator, this class returns the same
       * incoming data structure with the extra data inserted along their
       * corresponding satellites. Be warned that if a given satellite does
       * not have the observations required, it will be summarily deleted
       * from the data structure.
       *
       * @sa ModelObs.hpp for modeling data from a moving receiver.
       *
       */
    class ModelObsFixedStation : public ProcessingClass
    {
    public:

        /// Default constructor. Models C1 observations, use TGD, but doesn't apply atmospheric models
        ModelObsFixedStation() throw(Exception) : minElev(10.0), useTGD(true), pDefaultIonoModel(NULL), pDefaultTropoModel(NULL), defaultObservable(TypeID::C1), pDefaultEphemeris(NULL)
        { 
            InitializeValues();
            setIndex();
        };


        /** Explicit constructor taking as input reference station coordinates.
         *
         * Those coordinates may be Cartesian (X, Y, Z in meters) or Geodetic
         * (Latitude, Longitude, Altitude), but defaults to Cartesian. 
         *
         * Also, a pointer to GeoidModel may be specified, but default is NULL 
         * (in which case WGS84 values will be used).
         *
         * @param aRx   first coordinate [ X(m), or latitude (degrees N) ]
         * @param bRx   second coordinate [ Y(m), or longitude (degrees E) ]
         * @param cRx   third coordinate [ Z, height above ellipsoid or radius, in m ]
         * @param s     coordinate system (default is Cartesian, may be set to Geodetic).
         * @param geoid pointer to GeoidModel (default is null, implies WGS84)
         */
        ModelObsFixedStation(const double& aRx, const double& bRx, const double& cRx, 
            Position::CoordinateSystem s = Position::Cartesian,
            GeoidModel *geoid = NULL) throw(Exception) : minElev(10.0), useTGD(true), pDefaultIonoModel(NULL), pDefaultTropoModel(NULL), defaultObservable(TypeID::C1), pDefaultEphemeris(NULL)
        { 
            InitializeValues();
            setInitialRxPosition(aRx, bRx, cRx, s, geoid);
            setIndex();
        };


        /// Explicit constructor, taking as input a Position object containing reference station coordinates.
        ModelObsFixedStation(const Position& RxCoordinates) throw(Exception) : minElev(10.0), useTGD(true), pDefaultIonoModel(NULL), pDefaultTropoModel(NULL), defaultObservable(TypeID::C1), pDefaultEphemeris(NULL)
        { 
            InitializeValues();
            setInitialRxPosition(RxCoordinates);
            setIndex();
        };


        /** Explicit constructor, taking as input reference station coordinates, default
         * ionospheric and tropospheric models, ephemeris to be used, default observable 
         * and whether TGD will be computed or not.
         *
         * @param RxCoordinates Reference station coordinates.
         * @param dIonoModel    Ionospheric model to be used by default.
         * @param dTropoModel   Tropospheric model to be used by default.
         * @param dEphemeris    EphemerisStore object to be used by default.
         * @param dObservable   Observable type to be used by default.
         * @param usetgd        Whether TGD will be used by default or not.
         *
         */
        ModelObsFixedStation(const Position& RxCoordinates, IonoModelStore& dIonoModel, 
        TropModel& dTropoModel, XvtStore<SatID>& dEphemeris, const TypeID& dObservable,
        bool usetgd = true) throw(Exception) : minElev(10.0)
        { 
            InitializeValues();
            setInitialRxPosition(RxCoordinates);
            setDefaultIonoModel(dIonoModel);
            setDefaultTropoModel(dTropoModel);
            setDefaultObservable(dObservable);
            setDefaultEphemeris(dEphemeris);
            useTGD = usetgd;
            setIndex();
        };


        /** Explicit constructor, taking as input reference station coordinates, default
         * ionospheric model, ephemeris to be used, default observable and whether TGD 
         * will be computed or not.
         *
         * The default tropospheric model will be set to NULL.
         *
         * @param RxCoordinates Reference station coordinates.
         * @param dIonoModel    Ionospheric model to be used by default.
         * @param dEphemeris    EphemerisStore object to be used by default.
         * @param dObservable   Observable type to be used by default.
         * @param usetgd        Whether TGD will be used by default or not.
         *
         */
        ModelObsFixedStation(const Position& RxCoordinates, IonoModelStore& dIonoModel,
        XvtStore<SatID>& dEphemeris, const TypeID& dObservable, bool usetgd = true) throw(Exception) :
        minElev(10.0), pDefaultTropoModel(NULL)
        { 
            InitializeValues();
            setInitialRxPosition(RxCoordinates);
            setDefaultIonoModel(dIonoModel);
            setDefaultObservable(dObservable);
            setDefaultEphemeris(dEphemeris);
            useTGD = usetgd;
            setIndex();
        };


        /** Explicit constructor, taking as input reference station coordinates, default
         * tropospheric model, ephemeris to be used, default observable and whether TGD 
         * will be computed or not.
         *
         * The default ionospheric model will be set to NULL.
         *
         * @param RxCoordinates Reference station coordinates.
         * @param dTropoModel   Tropospheric model to be used by default.
         * @param dEphemeris    EphemerisStore object to be used by default.
         * @param dObservable   Observable type to be used by default.
         * @param usetgd        Whether TGD will be used by default or not.
         *
         */
        ModelObsFixedStation(const Position& RxCoordinates, TropModel& dTropoModel, 
        XvtStore<SatID>& dEphemeris, const TypeID& dObservable, bool usetgd = true) throw(Exception) :
        minElev(10.0), pDefaultIonoModel(NULL)
        { 
            InitializeValues();
            setInitialRxPosition(RxCoordinates);
            setDefaultTropoModel(dTropoModel);
            setDefaultObservable(dObservable);
            setDefaultEphemeris(dEphemeris);
            useTGD = usetgd;
            setIndex();
        };


        /** Explicit constructor, taking as input reference station coordinates, 
         * ephemeris to be used, default observable and whether TGD will be computed 
         * or not.
         *
         * Both the tropospheric and ionospheric models will be set to NULL.
         *
         * @param RxCoordinates Reference station coordinates.
         * @param dEphemeris    EphemerisStore object to be used by default.
         * @param dObservable   Observable type to be used by default.
         * @param usetgd        Whether TGD will be used by default or not.
         *
         */
        ModelObsFixedStation(const Position& RxCoordinates, XvtStore<SatID>& dEphemeris,
        const TypeID& dObservable, bool usetgd = true) throw(Exception) : minElev(10.0), 
        pDefaultIonoModel(NULL), pDefaultTropoModel(NULL)
        { 
            InitializeValues();
            setInitialRxPosition(RxCoordinates);
            setDefaultObservable(dObservable);
            setDefaultEphemeris(dEphemeris);
            useTGD = usetgd;
            setIndex();
        };


        /** Returns a satTypeValueMap object, adding the new data generated when calling a modeling object.
         *
         * @param time      Epoch.
         * @param gData     Data object holding the data.
         */
        virtual satTypeValueMap& Process(const DayTime& time, satTypeValueMap& gData) throw(Exception);


        /** Returns a gnnsSatTypeValue object, adding the new data generated when calling a modeling object.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData) throw(Exception)
        {
            (*this).Process(gData.header.epoch, gData.body);
            return gData;
        };


        /** Returns a gnnsRinex object, adding the new data generated when calling a modeling object.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssRinex& Process(gnssRinex& gData) throw(Exception)
        {
            (*this).Process(gData.header.epoch, gData.body);
            return gData;
        };


        /// Method to get satellite elevation cut-off angle. By default, it is set to 10 degrees.
        virtual double getMinElev() const
        {
           return minElev;
        };


        /// Method to set if instrumental delays (TGD) will be used or not in the modeling (it is set to
        /// true by default).
        virtual void setTGD(bool use)
        {
           useTGD = use;
        };


        /// Method to set satellite elevation cut-off angle. By default, it is set to 10 degrees.
        virtual void setMinElev(double newElevation)
        {
           minElev = newElevation;
        };


        /** Method to set the default ionospheric model.
         * @param dIonoModel    Ionospheric model to be used by default.
         */
        virtual void setDefaultIonoModel(IonoModelStore& dIonoModel)
        {
           pDefaultIonoModel = &dIonoModel;
        };


        /** Method to get a pointer to the default ionospheric model.
         */
        virtual IonoModelStore* getDefaultIonoModel() const
        {
           return pDefaultIonoModel;
        };


        /// Method to set a NULL ionospheric model.
        virtual void setNULLIonoModel()
        {
           pDefaultIonoModel = NULL;
        };


        /** Method to set the default tropospheric model.
         * @param dTropoModel    Tropospheric model to be used by default.
         */
        virtual void setDefaultTropoModel(TropModel& dTropoModel)
        {
           pDefaultTropoModel = &dTropoModel;
        };


        /** Method to get a pointer to the default tropospheric model.
         */
        virtual TropModel* getDefaultTropoModel() const
        {
           return pDefaultTropoModel;
        };


        /// Method to set a NULL tropospheric model.
        virtual void setNULLTropoModel()
        {
           pDefaultTropoModel = NULL;
        };


        /** Method to set the default observable to be used when fed with GNSS data structures.
         * @param type      TypeID object to be used by default
         */
        virtual void setDefaultObservable(const TypeID& type)
        {
           defaultObservable = type;
        };


        /// Method to get the default observable being used with GNSS data structures.
        virtual TypeID getDefaultObservable() const
        {
           return defaultObservable;
        };


        /** Method to set the default XvtStore<SatID> to be used with GNSS data structures.
         * @param ephem     XvtStore<SatID> object to be used by default
         */
        virtual void setDefaultEphemeris(XvtStore<SatID>& ephem)
        {
           pDefaultEphemeris = &ephem;
        };


        /** Method to get a pointer to the default XvtStore<SatID> to be used with GNSS data structures.
         */
        virtual XvtStore<SatID>* getDefaultEphemeris() const
        {
           return pDefaultEphemeris;
        };


        /// Either estimated or "a priori" position of receiver
        Position rxPos;


        /// Returns an index identifying this object.
        virtual int getIndex(void) const;


        /// Returns a string identifying this object.
        virtual std::string getClassName(void) const;


        /** Sets the index to a given arbitrary value. Use with caution.
         *
         * @param newindex      New integer index to be assigned to current object.
         */
        void setIndex(const int newindex) { (*this).index = newindex; };


        /// Destructor.
        virtual ~ModelObsFixedStation() {};


    protected:


        /** Compute the modeled pseudoranges, given satellite ID's, pseudoranges and other data
         * @param Tr            Measured time of reception of the data.
         * @param Satellite     Vector of satellites; on successful return, satellites that
         *                      were excluded by the algorithm are marked by a negative
         *                      Satellite[i].prn
         * @param Pseudorange   Vector of raw pseudoranges (parallel to satellite), in meters.
         * @param Eph           EphemerisStore to be used.
         * @param pTropModel    Pointer to tropospheric model to be used. By default points to NULL.
         * @param pIonoModel    Pointer to ionospheric model to be used. By default points to NULL.
         *
         * @return
         *  Number of satellites with valid data
         *
         * @sa TropModel.hpp, IonoModelStore.hpp.
         */
        int Compute(const DayTime& Tr, Vector<SatID>& Satellite, Vector<double>& Pseudorange, const XvtStore<SatID>& Eph, TropModel *pTropModel=NULL, IonoModelStore *pIonoModel=NULL) throw(Exception);


        /// The elevation cut-off angle for accepted satellites. By default is set to 10 degrees
        double minElev;

        /// Boolean variable indicating if SV instrumental delays (TGD) will be included  in results. It is true by default.
        bool useTGD;

        /// Pointer to default ionospheric model.
        IonoModelStore *pDefaultIonoModel;

        /// Pointer to default tropospheric model.
        TropModel *pDefaultTropoModel;

        /// Default observable to be used when fed with GNSS data structures.
        TypeID defaultObservable;

        /// Pointer to default XvtStore<SatID> object when working with GNSS data structures.
        XvtStore<SatID>* pDefaultEphemeris;

        /// Initialization method
        virtual void InitializeValues() throw(Exception) { 
            setInitialRxPosition();
        };


        /** Method to set the initial (a priori) position of receiver.
         * @return
         *  0 if OK
         *  -1 if problems arose
         */
        virtual int setInitialRxPosition(const double& aRx, const double& bRx, const double& cRx, 
        Position::CoordinateSystem s=Position::Cartesian, GeoidModel *geoid=NULL) throw(GeometryException) 
        {
            try {
                Position rxpos(aRx, bRx, cRx, s, geoid);
                setInitialRxPosition(rxpos);
                return 0;
            }
            catch(GeometryException& e) {
                return -1;
            }
        };


        /// Method to set the initial (a priori) position of receiver.
        virtual int setInitialRxPosition(const Position& RxCoordinates) throw(GeometryException) 
        {
            try {
                (*this).rxPos = RxCoordinates;
                return 0;
            }
            catch(GeometryException& e) {
                return -1;
            }
        };


        /// Method to set the initial (a priori) position of receiver.
        virtual int setInitialRxPosition() throw(GeometryException) 
        {
            try {
                Position rxpos(0.0, 0.0, 0.0, Position::Cartesian, NULL);
                setInitialRxPosition(rxpos);
                return 0;
            }
            catch(GeometryException& e) {
                return -1;
            }
        };


        /// Method to get the tropospheric corrections.
        virtual double getTropoCorrections(TropModel *pTropModel, double elevation) throw() 
        {
            double tropoCorr(0.0);
            try {
                tropoCorr = pTropModel->correction(elevation);
                // Check validity
                if(!(pTropModel->isValid())) tropoCorr = 0.0;
            }
            catch(TropModel::InvalidTropModel& e) {
                tropoCorr = 0.0;
            }
            return tropoCorr;
        };


        /// Method to get the ionospheric corrections.
        virtual double getIonoCorrections(IonoModelStore *pIonoModel, DayTime Tr, Geodetic rxGeo, double elevation, double azimuth) throw() 
        {
            double ionoCorr(0.0);
            try {
                ionoCorr = pIonoModel->getCorrection(Tr, rxGeo, elevation, azimuth);
            }
            catch(IonoModelStore::NoIonoModelFound& e) {
                ionoCorr = 0.0;
            }
            return ionoCorr;
        };


        /// Method to get TGD corrections.
        virtual double getTGDCorrections(DayTime Tr, const XvtStore<SatID>& Eph, SatID sat) throw() 
        {
           try {
              const GPSEphemerisStore& bce = dynamic_cast<const GPSEphemerisStore&>(Eph);
              bce.findEphemeris(sat,Tr);
//              const EngEphemeris& eph = bce.findEphemeris(sat,Tr);
              return bce.findEphemeris(sat,Tr).getTgd() * C_GPS_M;
            }
            catch(...) {
            }
            return 0.0;
        };



    private:


        /// Initial index assigned to this class.
        static int classIndex;

        /// Index belonging to this object.
        int index;

        /// Sets the index and increment classIndex.
        void setIndex(void) { (*this).index = classIndex++; };


   }; // class ModelObsFixedStation


   //@}

} // namespace

#endif
