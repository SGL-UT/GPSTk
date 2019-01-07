//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//  Octavian Andrei - FGI ( http://www.fgi.fi ). 2008, 2009, 2011
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file IonexModel.hpp
 * This is a class to compute the main values related to a given
 * GNSS IONEX model, i.e., TEC value, ionospheric mapping function
 * and slant ionospheric delay.
 */

#ifndef GPSTK_IONEXMODEL_HPP
#define GPSTK_IONEXMODEL_HPP

#include "IonexStore.hpp"
#include "Position.hpp"
#include "ProcessingClass.hpp"
#include "TypeID.hpp"



namespace gpstk
{

      /// @ingroup GPSsolutions 
      //@{

      /** This is a class to compute the main values related to a given
       * GNSS IONEX model, i.e., TEC value, ionospheric mapping function
       * and slant ionospheric delay.
       *
       * This class is intended to be used with GNSS Data Structures (GDS).
       *
       * A typical way to use this class follows:

       * @code
       *      // Input observation file stream
       *   RinexObsStream rin("ebre030a.02o");
       *
       *      // Load precise ephemeris file
       *   SP3EphemerisStore sp3Eph;
       *   sp3Eph.loadFile("igs11513.sp3");
       *
       *      // Load IONEX file
       *   IonexStore IonexMapList;
       *   IonexMapList.loadFile("codg0300.02i");
       *
       *      // Reference position of receiver station
       *   Position nominalPos(4833520.2269, 41537.00768, 4147461.489);
       *
       *      // Some more code and definitions here...
       *
       *   gnssRinex gRin;  // GNSS data structure for fixed station data
       *
       *      // This object will compute the ionex modeler
       *   IonexModel ionex(nominalPos, IonexMapList);
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> model >> ionex;
       *   }
       *
       * @endcode
       *
       * The "IonexModel" object will visit every satellite in
       * the GNSS data structure that is "gRin" and will try to compute
       * the main values of the corresponding IONEX model:
       * Total Electron Content value (TECU), ionospheric mapping function,
       * ionospheric slant correction (meters).
       *
       * When used with the ">>" operator, this class returns the same
       * incoming data structure with the extra data inserted along their
       * corresponding satellites.
       *
       * Be warned that if a given satellite does NOT have the information
       * needed (elevation and azimuth ARE REQUIRED), it will be summarily 
       * deleted from the data structure. This also implies that if you try 
       * to use a "IonexModel" object without first defining the IONEX model, 
       * then ALL satellites will be deleted.
       *
       * @sa IonexStore.hpp
       *
       */
   class IonexModel : public ProcessingClass
   {
   public:


         /// Default constructor.
      IonexModel() : pDefaultMaps(NULL), useDCB(true)
      { };


         /// Explicit constructor, taking as input a Position object
         /// containing reference station coordinates.
      IonexModel(const Position& RxCoordinates)
         throw(Exception);


         /** Explicit constructor, taking as input reference station
          *  coordinates and ionex maps (Ionex Store object) to be used.
          *
          * @param RxCoordinates    Receiver coordinates.
          * @param istore           IonexStore object to be used by default.
          * @param dObservable      Observable type to be used by default.
          * @param applyDCB         Whether or not P1 observable will be
          *                         corrected from DCB effect.
          * @param ionoMap          Type of ionosphere mapping function (string)
          *                         @sa IonexStore::iono_mapping_function
          */
      IonexModel( const Position& RxCoordinates,
                  IonexStore& istore,
                  const TypeID& dObservable = TypeID::P1,
                  const bool& applyDCB = true,
                  const std::string& ionoMap = "SLM" )
         throw(Exception);


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling a modeling object.
          *
          * @param time      Epoch.
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( const CommonTime& time,
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


         /// Method to get the default observable for computations.
      virtual TypeID getDefaultObservable() const
      { return defaultObservable; };


         /** Method to set the default observable for computations.
          *
          * @param type      TypeID object to be used by default
          */
      virtual IonexModel& setDefaultObservable(const TypeID& type)
      { defaultObservable = type; return (*this); };


         /// Method to get a pointer to the default maps to be used
         /// with GNSS data structures.
      virtual IonexStore* getDefaultMaps(void) const
      { return pDefaultMaps; };


         /** Method to set the default ionex maps (IonexStore object)
          *  to be used with GNSS data structures.
          *
          * @param istore   IonexStore object to be used by default
          */
      virtual IonexModel& setDefaultMaps(IonexStore& istore)
      { pDefaultMaps = &istore; return (*this); };


         /// Method to get if DCB is being used
      virtual bool getUseDCB(void) const
      { return useDCB; };


         /** Method to set if DCB will be used.
          *
          * @param applyDCB   Boolean value indicating whether or not DCB
          *                   values will be used.
          */
      virtual IonexModel& setUseDCB(bool applyDCB)
      { useDCB = applyDCB; return (*this); };


         /** Method to set the initial (a priori) position of receiver.
          *
          * @return
          *  0 if OK
          *  -1 if problems arose
          */
      virtual int setInitialRxPosition(const Position& RxCoordinates)
         throw(GeometryException);


         /// Method to set the initial (a priori) position of receiver.
      virtual int setInitialRxPosition(void)
         throw(GeometryException);


         /// Method to get the default ionosphere mapping function type.
      virtual std::string getIonoMapType() const
      { return ionoMapType; };


         /** Method to set the default ionosphere mapping function type. 
          *
          * @param ionoMapType      Type of ionosphere mapping function (string)
          *                         @sa IonexStore::iono_mapping_function
          *
          * @warning No implementation for JPL's mapping function.
          */
      virtual IonexModel& setIonoMapType(const std::string& ionoMap);


         /** Method to get DCB corrections.
          *
          * @param time       Epoch.
          * @param Maps       Store that contains the Ionex maps.
          * @param sat        SatID of satellite of interest
          *
          * @ return          Differential Code Bias (nano-seconds)
          */
      virtual double getDCBCorrections( const CommonTime& time,
                                        const IonexStore& Maps,
                                        SatID sat )
         throw();


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor.
      virtual ~IonexModel() {};


   protected:


         /// Default observable to be used when fed with GNSS data structures.
      TypeID defaultObservable;


         /// Pointer to default Ionex object when working with GNSS
         /// data structures.
      IonexStore* pDefaultMaps;


         /// Either estimated or "a priori" position of receiver
      Position rxPos;


         /// Whether or not the DCB effect will be applied to correct
         /// P1-code measurements (to make them consistent with LC 
         /// satellite clocks).
      bool useDCB;


         /// Type of ionosphere mapping function 
         ///  @sa IonexStore::iono_mapping_function
      std::string ionoMapType;


         /// the mean value for the height of the ionosphere for which 
         /// the TEC values are extracted.
      double ionoHeight;


   }; // End of class 'IonexModel'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_IONEXMODEL_HPP
