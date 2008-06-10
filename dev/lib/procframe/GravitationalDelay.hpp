#pragma ident "$Id$"

/**
 * @file GravitationalDelay.hpp
 * This class computes the delay in the signal due to changes in gravity field.
 */

#ifndef GRAVITATIONALDELAY_HPP
#define GRAVITATIONALDELAY_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008
//
//============================================================================



#include <math.h>
#include "Position.hpp"
#include "ProcessingClass.hpp"



namespace gpstk
{

      /** @addtogroup GPSsolutions */
      //@{


      /** This class computes the delay in the signal due to changes in the
       *  gravity field.
       *
       * This class is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       *      // Input observation file stream
       *   RinexObsStream rin("ebre0300.02o");
       *
       *      // Load the precise ephemeris file
       *   SP3EphemerisStore sp3Eph;
       *   sp3Eph.loadFile("igs11513.sp3");

       *      // Reference position of receiver station
       *   Position nominalPos(4833520.2269, 41537.00768, 4147461.489);
       *
       *      // Object to compute gravitational delay effects
       *   GravitationalDelay grDelay(nominalPos);
       *
       *      // Some more code and definitions here...
       *
       *   gnssRinex gRin;  // GNSS data structure for fixed station data
       *
       *      // Set defaults of models. A typical C1-based modeling is used
       *   BasicModel model( nominalPos, sp3Eph);
       *
       *   while(rin >> gRin)
       *   {
       *
       *         // Apply the model on the GDS
       *      gRin >> model >> grDelay;
       *   }
       * @endcode
       *
       * The "GravitationalDelay" object will visit every satellite in the GNSS
       * data structure that is "gRin" and will compute the delay in the signal
       * caused by the change in the gravity field when the signal travels from
       * satellite to receiver.
       *
       * This effect is small, ranging from about 0.01 to 0.02 m, and is only
       * used for precise positioning. The result is stored in the GDS with its
       * own type: "TypeID::gravDelay" (in meters) and should be used in the 
       * prefit residual computation process, like the more general relativity
       * effect. Class "LinearCombinations" takes it into account.
       *
       * \warning Be aware that this class DOES NEED both the receiver and
       * satellite position. The former is set with the common constructor or
       * the appropriate method, while you must use a class such a "BasicModel"
       * to compute the former and add it to the GDS.
       *
       * @sa BasicModel.hpp and LinearCombinations.hpp.
       *
       */
   class GravitationalDelay : public ProcessingClass
   {
      public:

         /// Default constructor.
      GravitationalDelay() : nominalPos(0.0, 0.0, 0.0)
      { setIndex(); };


         /** Common constructor
          *
          * @param stapos    Nominal position of receiver station.
          */
      GravitationalDelay(const Position& stapos) : nominalPos(stapos)
      { setIndex(); };


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param epoch     Time of observations.
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( const DayTime& epoch,
                                        satTypeValueMap& gData );


         /** Returns a gnnsSatTypeValue object, adding the new data generated
          *  when calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
      { Process(gData.header.epoch, gData.body); return gData; };


         /** Returns a gnnsRinex object, adding the new data generated when
          *  calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData);


         /// Returns nominal position of receiver station.
      virtual Position getNominalPosition(void) const
      { return nominalPos; };


         /** Sets nominal position of receiver station.
          * @param stapos    Nominal position of receiver station.
          */
      virtual GravitationalDelay& setNominalPosition(const Position& stapos)
        { nominalPos = stapos; return (*this); };


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~GravitationalDelay() {};


   private:


         /// Receiver position
      Position nominalPos;


         /// Initial index assigned to this class.
      static int classIndex;

         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


   }; // end class GravitationalDelay


   //@}

}

#endif   // GRAVITATIONALDELAY_HPP
