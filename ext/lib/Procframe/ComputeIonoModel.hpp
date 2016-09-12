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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2009, 2010, 2011
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
 * @file ComputeIonoModel.hpp
 * This is a class to compute the main values related to a given
 * GNSS ionospheric model.
 */

#ifndef GPSTK_COMPUTE_IONO_MODEL_HPP
#define GPSTK_COMPUTE_IONO_MODEL_HPP

#include "ProcessingClass.hpp"
#include "IonexStore.hpp"
#include "IonoModelStore.hpp"


namespace gpstk
{

      /// @ingroup GPSsolutions 
      //@{

      /** This is a class to compute the main values related to a given
       *  GNSS ionospheric model.
       *
       * This class is intended to be used with GNSS Data Structures (GDS).
       * It is a more modular alternative to classes such as ModelObs
       * and ModelObsFixedStation.
       *
       * A typical way to use this class follows:
       *
       * @code
       *      // Input observation file stream
       *   RinexObsStream rin("ebre0300.02o");
       *
       *      // Now, create the ComputeTropModel object
       *   ComputeIonoModel computeIono(nominalPosition);
       *   computeTropo.setIonosphereMap(ionexFile);
       *
       *   gnssRinex gRin;  // GNSS data structure for fixed station data
       *
       *   while(rin >> gRin)
       *   {
       *         // Apply the ionoospheric model on the GDS
       *      gRin >> computeIono;
       *   }
       *
       * @endcode
       *
       * The "ComputeIonoModel" object will visit every satellite in
       * the GNSS data structure that is "gRin" and will try to compute
       * the main values of the corresponding ionospheric model.
       *
       * When used with the ">>" operator, this class returns the same
       * incoming data structure with the extra data inserted along their
       * corresponding satellites.
       *
       * Be warned that if a given satellite does not have the information
       * needed (mainly elevation), it will be summarily deleted from the data
       * structure. 
       *
       */
   class ComputeIonoModel : public ProcessingClass
   {
   public:
      enum IonoModelType
      {
         Zero = 0,     ///< Don't do ionospheric delay correction
         Klobuchar,    ///< Klobuchar
         Ionex,        ///< Ionospheric maps
         DualFreq      ///< Compute from P1 and P2
      };

   public:

         /// Default constructor.
      ComputeIonoModel()
         : ionoType(Zero), nominalPos(0.0,0.0,0.0)
      { };


         /** Common constructor
          *
          * @param stapos    Nominal position of receiver station.
          */
      ComputeIonoModel(const Position& stapos) 
         : ionoType(Zero), nominalPos(stapos)
      { };


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling a modeling object.
          *
          * @param time      Epoch.
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( const CommonTime& time,
                                        satTypeValueMap& gData )
         throw(ProcessingException);


         /** Returns a gnnsSatTypeValue object, adding the new data generated
          *  when calling a modeling object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException)
      { Process(gData.header.epoch, gData.body); return gData; };


         /** Returns a gnnsRinex object, adding the new data generated when
          *  calling a modeling object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException)
      { Process(gData.header.epoch, gData.body); return gData; };


      virtual ComputeIonoModel& setZeroModel()
      { ionoType = Zero; return (*this); }

         /// Correct ionospheric delay with klobuchar model
      virtual ComputeIonoModel& setKlobucharModel(const double a[4], 
                                                  const double b[4]);

         /// Correct ionospheric delay with klobuchar model
      virtual ComputeIonoModel& setKlobucharModel(const IonoModel& im);


         /// Correct ionospheric delay with klobuchar model
      virtual ComputeIonoModel& setklobucharModel(const std::string& brdcFile);


         /// Correct ionospheric delay with ionex file
      virtual ComputeIonoModel& setIonosphereMap(const std::string& ionexFile);

         /// Correct ionospheric delay with dual frequency code 
      virtual ComputeIonoModel& setDualFreqModel()
      { ionoType=DualFreq; return (*this); }

         /// Returns nominal position of receiver station.
      virtual Position getNominalPosition(void) const
      { return nominalPos; };


         /** Sets nominal position of receiver station.
          * @param stapos    Nominal position of receiver station.
          */
      virtual ComputeIonoModel& setNominalPosition(const Position& stapos)
        { nominalPos = stapos; return (*this); };

         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor.
      virtual ~ComputeIonoModel() {};


   private:


      IonoModelType  ionoType;

         /// Object to calculate ionospheric delay with klobuchar model
      IonoModelStore klbStore;

         /// Object to calculate ionospheric delay with ionospheric map model
      IonexStore gridStore;

         /// Receiver position
      Position nominalPos;


   }; // End of class 'ComputeIonoModel'

      //@}

} // End of namespace gpstk

#endif   // GPSTK_COMPUTE_IONO_MODEL_HPP
