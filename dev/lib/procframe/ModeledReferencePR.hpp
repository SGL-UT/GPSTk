#pragma ident "$Id: $"

/**
 * @file ModeledReferencePR.hpp
 * Class to compute modeled pseudoranges using a reference station
 */

#ifndef MODELEDREFERENCEPR_HPP
#define MODELEDREFERENCEPR_HPP

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



#include "ModeledPseudorangeBase.hpp"
#include "DayTime.hpp"
#include "EngEphemeris.hpp"
#include "XvtStore.hpp"
#include "GPSEphemerisStore.hpp"
#include "EphemerisRange.hpp"
#include "TropModel.hpp"
#include "IonoModel.hpp"
#include "IonoModelStore.hpp"
#include "Geodetic.hpp"
#include "Position.hpp"
#include "icd_200_constants.hpp"
#include "TypeID.hpp"
#include "DataStructures.hpp"


namespace gpstk
{
      /** @addtogroup GPSsolutions */
      //@{

      /** This class compute modeled pseudoranges from satellites to 
       *  a reference station.
       *
       * This class may be used either in a Vector- and Matrix-oriented way, 
       * or with GNSS data structure objects from "DataStructures" class. In
       * any case, it is intented to be used with stations where the position
       * is known (there comes the name: Modeled Reference station
       * PseudoRange).
       *
       * A typical way to use this class with GNSS data structures follows:
       *
       * @code
       *   RinexObsStream rin("ebre0300.02o");  // Data stream
       *   RinexNavStream rnavin("brdc0300.02n");   // Ephemeris data stream
       *   RinexNavData rNavData;
       *   GPSEphemerisStore bceStore;
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
       *   MOPSTropModel mopsTM( nominalPos.getAltitude(),
       *                         nominalPos.getGeodeticLatitude(), 30);
       *
       *   // Declare the modeler object, setting all the parameters in 
       *   // one pass
       *   // As stated, it will compute the model using the C1 observable
       *   ModeledReferencePR modelRef( nominalPos, ionoStore, mopsTM, 
       *                                bceStore, TypeID::C1 );
       *
       *   gnssRinex gRin;
       *
       *   while(rin >> gRin) {
       *      gRin >> modelRef;
       *   }
       * @endcode
       *
       * The "ModeledReferencePR" object will visit every satellite in the 
       * GNSS data structure that is "gRin" and will try to compute its 
       * model: Prefit residual, geometric distance, relativity delay,
       * ionospheric/tropospheric corrections, geometry matrix, etc.
       *
       * When used with the ">>" operator, this class returns the same 
       * incoming data structure with the extra data inserted along their
       * corresponding satellites. Be warned that if a given satellite does 
       * not have the observations required, it will be summarily deleted 
       * from the data structure.
       *
       * @sa ModeledPseudorangeBase.hpp for base class.
       *
       */
   class ModeledReferencePR : public ModeledPseudorangeBase
   {
   public:

         /// Implicit constructor
      ModeledReferencePR() throw(Exception)
         : useTGD(true), pDefaultIonoModel(NULL), pDefaultTropoModel(NULL),
           defaultObservable(TypeID::C1), pDefaultEphemeris(NULL)
      { init(); };


         /** Explicit constructor taking as input reference station
          *  coordinates.
          *
          * Those coordinates may be Cartesian (X, Y, Z in meters) or Geodetic
          * (Latitude, Longitude, Altitude), but defaults to Cartesian. 
          *
          * Also, a pointer to GeoidModel may be specified, but default is 
          * NULL (in which case WGS84 values will be used).
          *
          * @param aRx   first coordinate [ X(m), or latitude (degrees N) ]
          * @param bRx   second coordinate [ Y(m), or longitude (degrees E) ]
          * @param cRx   third coordinate [ Z, height above ellipsoid or
          *              radius, in m ]
          * @param s     coordinate system (default is Cartesian, may be set
          *              to Geodetic).
          * @param geoid pointer to GeoidModel (default is null, implies WGS84)
          */
      ModeledReferencePR( const double& aRx,
                          const double& bRx,
                          const double& cRx, 
                          Position::CoordinateSystem s = Position::Cartesian,
                          GeoidModel *geoid = NULL )
         throw(Exception)
         : useTGD(true), pDefaultIonoModel(NULL), pDefaultTropoModel(NULL),
           defaultObservable(TypeID::C1), pDefaultEphemeris(NULL)
      { init(); setInitialRxPosition(aRx, bRx, cRx, s, geoid); };


         /// Explicit constructor, taking as input a Position object 
         /// containing reference station coordinates.
      ModeledReferencePR(const Position& RxCoordinates) throw(Exception)
         : useTGD(true), pDefaultIonoModel(NULL), pDefaultTropoModel(NULL),
           defaultObservable(TypeID::C1), pDefaultEphemeris(NULL)
      { init(); setInitialRxPosition(RxCoordinates); };


         /** Explicit constructor, taking as input reference station
          *  coordinates, default ionospheric and tropospheric models,
          *  ephemeris to be used, default observable and whether TGD will 
          *  be computed or not.
          *
          * This constructor is meant to be used when working with GNSS data
          * structures in order to set the basic parameters from the beginning.
          *
          * @param RxCoordinates Reference station coordinates.
          * @param dIonoModel    Ionospheric model to be used by default.
          * @param dTropoModel   Tropospheric model to be used by default.
          * @param dEphemeris    EphemerisStore object to be used by default.
          * @param dObservable   Observable type to be used by default.
          * @param usetgd        Whether TGD will be used by default or not.
          *
          * @sa DataStructures.hpp.
          */
      ModeledReferencePR( const Position& RxCoordinates,
                          IonoModelStore& dIonoModel,
                          TropModel& dTropoModel,
                          XvtStore<SatID>& dEphemeris,
                          const TypeID& dObservable,
                          bool usetgd = true )
         throw(Exception);


         /** Explicit constructor, taking as input reference station
          *  coordinates, default ionospheric model, ephemeris to be used,
          *  default observable and whether TGD will be computed or not.
          *
          * The default tropospheric model will be set to NULL.
          *
          * This constructor is meant to be used when working with GNSS data
          * structures in order to set the basic parameters from the
          * beginning.
          *
          * @param RxCoordinates Reference station coordinates.
          * @param dIonoModel    Ionospheric model to be used by default.
          * @param dEphemeris    EphemerisStore object to be used by default.
          * @param dObservable   Observable type to be used by default.
          * @param usetgd        Whether TGD will be used by default or not.
          *
          * @sa DataStructures.hpp.
          */
      ModeledReferencePR( const Position& RxCoordinates,
                          IonoModelStore& dIonoModel,
                          XvtStore<SatID>& dEphemeris,
                          const TypeID& dObservable,
                          bool usetgd = true )
         throw(Exception);


         /** Explicit constructor, taking as input reference station
          *  coordinates, default tropospheric model, ephemeris to be used,
          *  default observable and whether TGD will be computed or not.
          *
          * The default ionospheric model will be set to NULL.
          *
          * This constructor is meant to be used when working with GNSS 
          * data structures in order to set the basic parameters from the
          * beginning.
          *
          * @param RxCoordinates Reference station coordinates.
          * @param dTropoModel   Tropospheric model to be used by default.
          * @param dEphemeris    EphemerisStore object to be used by default.
          * @param dObservable   Observable type to be used by default.
          * @param usetgd        Whether TGD will be used by default or not.
          *
          * @sa DataStructures.hpp.
          */
      ModeledReferencePR( const Position& RxCoordinates,
                          TropModel& dTropoModel,
                          XvtStore<SatID>& dEphemeris,
                          const TypeID& dObservable,
                          bool usetgd = true )
         throw(Exception);


         /** Explicit constructor, taking as input reference station
          *  coordinates, ephemeris to be used, default observable and 
          *  whether TGD will be computed or not.
          *
          * Both the tropospheric and ionospheric models will be set to NULL.
          *
          * This constructor is meant to be used when working with GNSS 
          * data structures in order to set the basic parameters from the
          * beginning.
          *
          * @param RxCoordinates Reference station coordinates.
          * @param dEphemeris    EphemerisStore object to be used by default.
          * @param dObservable   Observable type to be used by default.
          * @param usetgd        Whether TGD will be used by default or not.
          *
          * @sa DataStructures.hpp.
          */
      ModeledReferencePR( const Position& RxCoordinates,
                          XvtStore<SatID>& dEphemeris,
                          const TypeID& dObservable,
                          bool usetgd = true )
         throw(Exception);


         /** Compute the modeled pseudoranges, given satellite ID's,
          *  pseudoranges and other data.
          *
          * @param Tr            Measured time of reception of the data.
          * @param Satellite     Vector of satellites
          * @param Pseudorange   Vector of raw pseudoranges (parallel to
          *                      satellite), in meters.
          * @param Eph           EphemerisStore to be used.
          * @param extraBiases   Vector of extra biases to be added to 
          *                      the model.
          * @param pTropModel    Pointer to tropospheric model to be used. 
          *                      By default it points to NULL.
          * @param pIonoModel    Pointer to ionospheric model to be used.
          *                      By default it points to NULL.
          *
          * @return Number of satellites with valid data
          *
          * @sa TropModel.hpp, IonoModelStore.hpp.
          */
      int Compute( const DayTime& Tr,
                   Vector<SatID>& Satellite,
                   Vector<double>& Pseudorange,
                   const XvtStore<SatID>& Eph,
                   const Vector<double>& extraBiases,
                   TropModel *pTropModel=NULL,
                   IonoModelStore *pIonoModel=NULL )
         throw(Exception);


         /// Compute the modeled pseudoranges, given satellite ID's,
         /// pseudoranges and other data.
      int Compute( const DayTime& Tr,
                   Vector<SatID>& Satellite,
                   Vector<double>& Pseudorange,
                   const XvtStore<SatID>& Eph )
         throw(Exception);


         /// Compute the modeled pseudoranges, given satellite ID's,
         /// pseudoranges and other data.
      int Compute( const DayTime& Tr,
                   Vector<SatID>& Satellite,
                   Vector<double>& Pseudorange,
                   const XvtStore<SatID>& Eph,
                   TropModel *pTropModel )
         throw(Exception);


         /// Compute the modeled pseudoranges, given satellite ID's,
         /// pseudoranges and other data.
      int Compute( const DayTime& Tr,
                   Vector<SatID>& Satellite,
                   Vector<double>& Pseudorange,
                   const XvtStore<SatID>& Eph,
                   const Vector<double>& extraBiases,
                   IonoModelStore *pIonoModel )
         throw(Exception);


         /// Compute the modeled pseudoranges, given satellite ID's,
         /// pseudoranges and other data.
      int Compute( const DayTime& Tr,
                   Vector<SatID>& Satellite,
                   Vector<double>& Pseudorange,
                   const XvtStore<SatID>& Eph,
                   IonoModelStore *pIonoModel )
         throw(Exception);


         /// Compute the modeled pseudoranges, given satellite ID's,
         /// pseudoranges and other data.
      int Compute( const DayTime& Tr,
                   Vector<SatID>& Satellite,
                   Vector<double>& Pseudorange,
                   const XvtStore<SatID>& Eph,
                   TropModel *pTropModel,
                   IonoModelStore *pIonoModel )
         throw(Exception);


         /** Compute just one modeled pseudorange, given satellite ID's,
          *  pseudorange and other data.
          *
          * @param Tr            Measured time of reception of the data.
          * @param Satellite     ID's of satellite
          * @param Pseudorange   Pseudorange (parallel to satellite), in 
          *                      meters.
          * @param Eph           XvtStore<SatID> to be used.
          * @param pTropModel    Pointer to tropospheric model to be used.
          *                      By default points to NULL.
          * @param pIonoModel    Pointer to ionospheric model to be used.
          *                      By default points to NULL.
          * @param extraBiases   Extra bias to be added to the model.
          *
          * @return
          *  1 if satellite has valid data
          *
          * @sa TropModel.hpp, IonoModelStore.hpp.
          */
      int Compute( const DayTime& Tr,
                   SatID& Satellite,
                   double& Pseudorange,
                   const XvtStore<SatID>& Eph,
                   const double& extraBiases,
                   TropModel *pTropModel=NULL,
                   IonoModelStore *pIonoModel=NULL )
         throw(Exception);


         /** Returns a satTypeValueMap object, adding the new data 
          *  generated when calling a modeling object.
          *
          * @param time      Epoch.
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& processModel( const DayTime& time,
                                             satTypeValueMap& gData )
         throw(Exception);


         /** Returns a gnnsSatTypeValue object, adding the new data 
          *  generated when calling a modeling object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& processModel(gnssSatTypeValue& gData)
         throw(Exception)
      { processModel(gData.header.epoch, gData.body); return gData; };


         /** Returns a gnnsRinex object, adding the new data generated
          *  when calling a modeling object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& processModel(gnssRinex& gData)
         throw(Exception)
      { processModel(gData.header.epoch, gData.body); return gData; };


         /// Boolean variable indicating if SV instrumental delays (TGD) 
         /// will be included  in results. It is true by default.
      bool useTGD;


         /// Method to get satellite elevation cut-off angle. By default, 
         /// it is set to 10 degrees.
      virtual double getMinElev() const
      { return minElev; };


         /// Method to set satellite elevation cut-off angle. By default, 
         /// it is set to 10 degrees.
      virtual ModeledReferencePR& setMinElev(double newElevation)
      { minElev = newElevation; return (*this); };


         /// Method to get a pointer to the default ionospheric model.
      virtual IonoModelStore* getDefaultIonoModel() const
      { return pDefaultIonoModel; };


         /** Method to set the default ionospheric model.
          *
          * @param dIonoModel    Ionospheric model to be used by default.
          */
      virtual ModeledReferencePR& setDefaultIonoModel(
                                                IonoModelStore& dIonoModel )
      { pDefaultIonoModel = &dIonoModel; return (*this); };


         /// Method to set a NULL ionospheric model.
      virtual ModeledReferencePR& setNULLIonoModel()
      { pDefaultIonoModel = NULL; return (*this); };


         /// Method to get a pointer to the default tropospheric model.
      virtual TropModel* getDefaultTropoModel() const
      { return pDefaultTropoModel; };


         /** Method to set the default tropospheric model.
          *
          * @param dTropoModel    Tropospheric model to be used by default.
          */
      virtual ModeledReferencePR& setDefaultTropoModel(TropModel& dTropoModel)
      { pDefaultTropoModel = &dTropoModel; return (*this); };


         /// Method to set a NULL tropospheric model.
      virtual ModeledReferencePR& setNULLTropoModel()
      { pDefaultTropoModel = NULL; return (*this); };


         /** Method to set the default extra biases.
          *
          * @param eBiases    Vector with the default extra biases
          */
      virtual ModeledReferencePR& setDefaultExtraBiases(
                                                   Vector<double>& eBiases )
      { extraBiases = eBiases; return (*this); };


         /// Method to get the default observable being used with 
         /// GNSS data structures.
      virtual TypeID getDefaultObservable() const
      { return defaultObservable; };


         /** Method to set the default observable to be used when fed with 
          *  GNSS data structures.
          *
          * @param type      TypeID object to be used by default
          */
      virtual ModeledReferencePR& setDefaultObservable(const TypeID& type)
      { defaultObservable = type; return (*this); };


         /// Method to get a pointer to the default XvtStore<SatID> to be 
         /// used with GNSS data structures.
      virtual XvtStore<SatID>* getDefaultEphemeris() const
      { return pDefaultEphemeris; };


         /** Method to set the default XvtStore<SatID> to be used with 
          *  GNSS data structures.
          *
          * @param ephem     XvtStore<SatID> object to be used by default
          */
      virtual ModeledReferencePR& setDefaultEphemeris(XvtStore<SatID>& ephem)
      { pDefaultEphemeris = &ephem; return (*this); };


         /// Destructor.
      virtual ~ModeledReferencePR() throw () {};


   protected:

         /// Pointer to default ionospheric model.
      IonoModelStore *pDefaultIonoModel;

         /// Pointer to default tropospheric model.
      TropModel *pDefaultTropoModel;

         /// Default observable to be used when fed with GNSS data structures.
      TypeID defaultObservable;

         /// Pointer to default XvtStore<SatID> object when working with 
         /// GNSS data structures.
      XvtStore<SatID>* pDefaultEphemeris;

         /// Initialization method
      virtual void init() throw(Exception);


         /** Method to set the initial (a priori) position of receiver.
          * @return
          *  0 if OK
          *  -1 if problems arose
          */
      virtual int setInitialRxPosition( const double& aRx,
                                        const double& bRx,
                                        const double& cRx, 
                           Position::CoordinateSystem s=Position::Cartesian,
                                        GeoidModel *geoid = NULL )
         throw(GeometryException);


         /// Method to set the initial (a priori) position of receiver.
      virtual int setInitialRxPosition(const Position& RxCoordinates)
         throw(GeometryException);


         /// Method to set the initial (a priori) position of receiver.
      virtual int setInitialRxPosition() throw(GeometryException);


         /// Method to get the tropospheric corrections.
      virtual double getTropoCorrections( TropModel *pTropModel,
                                          double elevation )
         throw();


         /// Method to get the ionospheric corrections.
      virtual double getIonoCorrections( IonoModelStore *pIonoModel,
                                         DayTime Tr,
                                         Geodetic rxGeo,
                                         double elevation,
                                         double azimuth )
         throw();


         /// Method to get TGD corrections.
      virtual double getTGDCorrections( DayTime Tr,
                                        const XvtStore<SatID>& Eph,
                                        SatID sat )
         throw();


   }; // End of class ModeledReferencePR


      /// Input operator from gnssSatTypeValue to ModeledReferencePR.
   inline gnssSatTypeValue& operator>>( gnssSatTypeValue& gData,
                                        ModeledReferencePR& modRefPR )
      throw(Exception)
   { modRefPR.processModel(gData); return gData; }


      /// Input operator from gnssRinex to ModeledReferencePR.
   inline gnssRinex& operator>>( gnssRinex& gData,
                                 ModeledReferencePR& modRefPR )
      throw(Exception)
   { modRefPR.processModel(gData); return gData; }


      //@}

} // namespace
#endif // MODELEDREFERENCEPR_HPP
