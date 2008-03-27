#pragma ident "$Id$"

/**
 * @file BasicModel.hpp
 * This is a class to compute the basic parts of a GNSS model, i.e.:
 * Geometric distance, relativity correction, satellite position at
 * transmission time, satellite elevation and azimuth, etc.
 */

#ifndef BASICMODEL_HPP
#define BASICMODEL_HPP

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


namespace gpstk
{
      /** @addtogroup GPSsolutions */
      //@{

      /** This is a class to compute the basic parts of a GNSS model, i.e.:
       * Geometric distance, relativity correction, satellite position at
       * transmission time, satellite elevation and azimuth, etc.
       *
       * This class is intended to be used with GNSS Data Structures (GDS).
       * It is a more modular alternative to classes such as ModelObs
       * and ModelObsFixedStation.
       *
       * A typical way to use this class follows:
       *
       * @code
       *   // Input observation file stream
       *   RinexObsStream rin("ebre0300.02o");
       *   // Load the precise ephemeris file
       *   SP3EphemerisStore sp3Eph;
       *   sp3Eph.loadFile("igs11513.sp3");

       *   // Reference position of receiver station
       *   Position nominalPos(4833520.2269, 41537.00768, 4147461.489);
       *
       *   // Some more code and definitions here...
       *
       *   gnssRinex gRin;  // GNSS data structure for fixed station data
       *
       *   // Set defaults of models. A typical C1-based modeling is used
       *   BasicModel model( nominalPos, sp3Eph);
       *
       *   while(rin >> gRin)
       *   {
       *
       *         // Apply the model on the GDS
       *      gRin >> model;
       *   }
       *
       * @endcode
       *
       * The "BasicModel" object will visit every satellite in
       * the GNSS data structure that is "gRin" and will try to compute
       * its model: Geometric distance, relativity delay, satellite position
       * at transmission time, satellite elevation and azimuth, etc.
       *
       * When used with the ">>" operator, this class returns the same
       * incoming data structure with the extra data inserted along their
       * corresponding satellites. Be warned that if a given satellite does
       * not have ephemeris information, it will be summarily deleted
       * from the data structure.
       *
       * @sa ModelObs.hpp and ModelObsFixedStation.hpp for classes carrying
       * out a more complete model.
       *
       */
   class BasicModel : public ProcessingClass
   {
   public:

         /// Default constructor. Observable C1 will be used for computations
         /// and satellites with elevation less than 10 degrees will be
         /// deleted.
      BasicModel()
         throw(Exception)
         : minElev(10.0), pDefaultEphemeris(NULL),
           defaultObservable(TypeID::C1), useTGD(false)
      { setInitialRxPosition(); setIndex(); };


         /** Explicit constructor taking as input reference 
          *  station coordinates.
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
          *              radius, in meters ]
          * @param s     coordinate system (default is Cartesian, may be set
          *              to Geodetic).
          * @param geoid pointer to GeoidModel (default is null, implies WGS84)
          */
      BasicModel( const double& aRx,
                  const double& bRx,
                  const double& cRx, 
                  Position::CoordinateSystem s = Position::Cartesian,
                  GeoidModel *geoid = NULL )
         throw(Exception);


         /// Explicit constructor, taking as input a Position object
         /// containing reference station coordinates.
      BasicModel(const Position& RxCoordinates)
         throw(Exception);


         /** Explicit constructor, taking as input reference station
          *  coordinates, ephemeris to be used and whether TGD will
          *  be computed or not.
          *
          * @param RxCoordinates Reference station coordinates.
          * @param dEphemeris    EphemerisStore object to be used by default.
          * @param dObservable   Observable type to be used by default.
          * @param applyTGD      Whether or not C1 observable will be
          *                      corrected from TGD effect.
          *
          */
      BasicModel( const Position& RxCoordinates,
                  XvtStore<SatID>& dEphemeris,
                  const TypeID& dObservable = TypeID::C1,
                  const bool& applyTGD = false )
         throw(Exception);


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling a modeling object.
          *
          * @param time      Epoch.
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( const DayTime& time,
                                        satTypeValueMap& gData )
         throw(Exception);


         /** Returns a gnnsSatTypeValue object, adding the new data generated
          *  when calling a modeling object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(Exception)
      { Process(gData.header.epoch, gData.body); return gData; };


         /** Returns a gnnsRinex object, adding the new data generated when
          *  calling a modeling object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(Exception)
      { Process(gData.header.epoch, gData.body); return gData; };


         /// Method to get satellite elevation cut-off angle. By default, it
         /// is set to 10 degrees.
      virtual double getMinElev() const
      { return minElev; };


         /// Method to set satellite elevation cut-off angle. By default, it
         /// is set to 10 degrees.
      virtual BasicModel& setMinElev(double newElevation)
      { minElev = newElevation; return (*this); };


         /// Method to get the default observable for computations.
      virtual TypeID getDefaultObservable() const
      { return defaultObservable; };


         /** Method to set the default observable for computations.
          *
          * @param type      TypeID object to be used by default
          */
      virtual BasicModel& setDefaultObservable(const TypeID& type)
      { defaultObservable = type; return (*this); };


         /// Method to get a pointer to the default XvtStore<SatID> to be used
         /// with GNSS data structures.
      virtual XvtStore<SatID>* getDefaultEphemeris() const
      { return pDefaultEphemeris; };


         /** Method to set the default XvtStore<SatID> to be used with GNSS
          *  data structures.
          *
          * @param ephem     XvtStore<SatID> object to be used by default
          */
      virtual BasicModel& setDefaultEphemeris(XvtStore<SatID>& ephem)
      { pDefaultEphemeris = &ephem; return (*this); };


         /// Either estimated or "a priori" position of receiver
      Position rxPos;


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /** Sets the index to a given arbitrary value. Use with caution.
          *
          * @param newindex      New integer index to be assigned to
          *                      current object.
          */
      BasicModel& setIndex(const int newindex)
      { index = newindex; return (*this); };


         /// Destructor.
      virtual ~BasicModel() {};


   protected:


         /** Compute the modeled pseudoranges, given satellite ID's,
          *  pseudoranges and other data.
          *
          * @param Tr            Measured time of reception of the data.
          * @param Satellite     Vector of satellites.
          * @param Pseudorange   Vector of raw pseudoranges (parallel to
          *                      satellite), in meters.
          * @param Eph           EphemerisStore to be used.
          *
          * @return
          *  Number of satellites with valid data
          *
          * @sa TropModel.hpp, IonoModelStore.hpp.
          */
      int Compute( const DayTime& Tr,
                   Vector<SatID>& Satellite,
                   Vector<double>& Pseudorange,
                   const XvtStore<SatID>& Eph )
         throw(Exception);


         /// The elevation cut-off angle for accepted satellites.
         /// By default it is set to 10 degrees.
      double minElev;


         /// Pointer to default XvtStore<SatID> object when working with GNSS
         /// data structures.
      XvtStore<SatID>* pDefaultEphemeris;


         /// Default observable to be used when fed with GNSS data structures.
      TypeID defaultObservable;


         /// Whether the TGD effect will be applied to C1 observable or not.
      bool useTGD;


         /** Method to set the initial (a priori) position of receiver.
          * @return
          *  0 if OK
          *  -1 if problems arose
          */
      virtual int setInitialRxPosition( const double& aRx,
                                        const double& bRx,
                                        const double& cRx, 
                           Position::CoordinateSystem s = Position::Cartesian,
                                        GeoidModel *geoid = NULL )
         throw(GeometryException);


         /// Method to set the initial (a priori) position of receiver.
      virtual int setInitialRxPosition(const Position& RxCoordinates)
         throw(GeometryException);


         /// Method to set the initial (a priori) position of receiver.
      virtual int setInitialRxPosition()
         throw(GeometryException);


         /// Method to get TGD corrections.
      virtual double getTGDCorrections( DayTime Tr,
                                        const XvtStore<SatID>& Eph,
                                        SatID sat )
         throw();


   private:


         /// Initial index assigned to this class.
      static int classIndex;


         /// Index belonging to this object.
      int index;


         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


   }; // class BasicModel

      //@}

} // namespace
#endif // BASICMODEL_HPP
