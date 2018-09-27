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


#include "YDSTime.hpp"
#include "MOPSTropModel.hpp"

#define THROW_IF_INVALID_DETAILED() {if (!valid) {                   \
         InvalidTropModel e;                                            \
         if(!validLat) e.addText("Invalid trop model: validLat");       \
         if(!validHeight)  e.addText("Invalid trop model: validHeight"); \
         if(!validTime)   e.addText("Invalid trop model: day of year"); \
         GPSTK_THROW(e);}}

namespace gpstk
{
      /* Tropospheric model implemented in the RTCA "Minimum Operational
       * Performance Standards" (MOPS), version C.
       *
       * This model is described in the RTCA "Minimum Operational Performance
       * Standards" (MOPS), version C (RTCA/DO-229C), in Appendix A.4.2.4.
       * Although originally developed for SBAS systems (EGNOS, WAAS), it may
       * be suitable for other uses as well.
       *
       * This model needs the day of year, satellite elevation (degrees),
       * receiver height over mean sea level (meters) and receiver latitude in
       * order to start computing.
       *
       * On the other hand, the outputs are the tropospheric correction (in
       * meters) and the sigma-squared of tropospheric delay residual error
       * (meters^2).
       *
       * A typical way to use this model follows:
       *
       * @code
       *   MOPSTropModel mopsTM;
       *   mopsTM.setReceiverLatitude(lat);
       *   mopsTM.setReceiverHeight(height);
       *   mopsTM.setDayOfYear(doy);
       * @endcode
       *
       * Once all the basic model parameters are set (latitude, height and day
       * of year), then we are able to compute the tropospheric correction as
       * a function of elevation:
       *
       * @code
       *   trop = mopsTM.correction(elevation);
       * @endcode
       *
       */

      // Some specific constants
   static const double MOPSg=9.80665;
   static const double MOPSgm=9.784;
   static const double MOPSk1=77.604;
   static const double MOPSk2=382000.0;
   static const double MOPSRd=287.054;


      // Empty constructor
   MOPSTropModel::MOPSTropModel(void)
   {
      validHeight = false;
      validLat    = false;
      validTime   = false;
      valid       = false;
   }


      /* Constructor to create a MOPS trop model providing the height of
       *  the receiver above mean sea level (as defined by ellipsoid model),
       *  its latitude and the day of year.
       *
       * @param ht   Height of the receiver above mean sea level, in meters.
       * @param lat  Latitude of receiver, in degrees.
       * @param doy  Day of year.
       */
   MOPSTropModel::MOPSTropModel( const double& ht,
                                 const double& lat,
                                 const int& doy )
   {
      setReceiverHeight(ht);
      setReceiverLatitude(lat);
      setDayOfYear(doy);
   }


      /* Constructor to create a MOPS trop model providing the position of
       *  the receiver and current time.
       *
       * @param RX   Receiver position.
       * @param time Time.
       */
   MOPSTropModel::MOPSTropModel(const Position& RX, const CommonTime& time)
   {
      setReceiverHeight(RX.getAltitude());
      setReceiverLatitude(RX.getGeodeticLatitude());
      setDayOfYear(time);
   }


      // Compute and return the full tropospheric delay. The receiver height,
      // latitude and Day oy Year must has been set before using the
      // appropriate constructor or the provided methods.
      // @param elevation Elevation of satellite as seen at receiver, in
      // degrees
   double MOPSTropModel::correction(double elevation) const
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID_DETAILED();

      if(elevation < 5.0) return 0.0;

      double map = MOPSTropModel::mapping_function(elevation);

         // Compute tropospheric delay
      double tropDelay = ( MOPSTropModel::dry_zenith_delay() +
                           MOPSTropModel::wet_zenith_delay() ) * map;

      return tropDelay;

   }


      // Compute and return the full tropospheric delay, given the positions of
      // receiver and satellite. This version is most useful within
      // positioning algorithms, where the receiver position may vary; it
      // computes the elevation (and other receiver location information as
      // height and latitude) and passes them to appropriate methods. You must
      // set time using method setDayOfYear() before calling this method.
      // @param RX  Receiver position
      // @param SV  Satellite position
   double MOPSTropModel::correction( const Position& RX,
                                     const Position& SV )
      throw(InvalidTropModel)
   {
      try
      {
         setReceiverHeight( RX.getAltitude() );
         setReceiverLatitude(RX.getGeodeticLatitude());
         setWeather();
      }
      catch(GeometryException& e)
      {
         valid = false;
      }

      if(!valid) throw InvalidTropModel("Invalid model");

      double c;
      try
      {
         c = MOPSTropModel::correction(RX.elevationGeodetic(SV));
      }
      catch(InvalidTropModel& e)
      {
         GPSTK_RETHROW(e);
      }

      return c;

   }


      // Compute and return the full tropospheric delay, given the positions of
      // receiver and satellite and the time tag. This version is most useful
      // within positioning algorithms, where the receiver position may vary;
      // it computes the elevation (and other receiver location information as
      // height  and latitude) and passes them to appropriate methods.
      // @param RX  Receiver position in ECEF cartesian coordinates (meters)
      // @param SV  Satellite position in ECEF cartesian coordinates (meters)
      // @param tt  Time (CommonTime object).
   double MOPSTropModel::correction( const Position& RX,
                                     const Position& SV,
                                     const CommonTime& tt )
      throw(InvalidTropModel)
   {
      setDayOfYear(tt);

      return MOPSTropModel::correction(RX,SV);
   }


      // Compute and return the full tropospheric delay, given the positions of
      // receiver and satellite and the day of the year. This version is most
      // useful within positioning algorithms, where the receiver position may
      // vary; it computes the elevation (and other receiver location
      // information as height and latitude) and passes them to appropriate
      // methods.
      // @param RX  Receiver position in ECEF cartesian coordinates (meters)
      // @param SV  Satellite position in ECEF cartesian coordinates (meters)
      // @param doy Day of year.
   double MOPSTropModel::correction( const Position& RX,
                                     const Position& SV,
                                     const int& doy )
      throw(InvalidTropModel)
   {
      setDayOfYear(doy);

      return MOPSTropModel::correction(RX,SV);
   }



      // deprecated
      // Compute and return the full tropospheric delay, given the positions of
      // receiver and satellite. You must set time using method setDayOfYear()
      // before calling this method.
      // @param RX  Receiver position in ECEF cartesian coordinates (meters)
      // @param SV  Satellite position in ECEF cartesian coordinates (meters)
      // This function is deprecated; use the Position version
   double MOPSTropModel::correction( const Xvt& RX,
                                     const Xvt& SV )
      throw(InvalidTropModel)
   {
      Position R(RX),S(SV);

      return MOPSTropModel::correction(R,S);
   }


      // deprecated
      // Compute and return the full tropospheric delay, given the positions of
      // receiver and satellite and the time tag. This version is most useful
      // within positioning algorithms, where the receiver position may vary;
      // it computes the elevation (and other receiver location information as
      // height and latitude) and passes them to appropriate methods.
      // @param RX  Receiver position in ECEF cartesian coordinates (meters)
      // @param SV  Satellite position in ECEF cartesian coordinates (meters)
      // @param tt  Time (CommonTime object).
      // This function is deprecated; use the Position version
   double MOPSTropModel::correction( const Xvt& RX,
                                     const Xvt& SV,
                                     const CommonTime& tt )
      throw(InvalidTropModel)
   {
      setDayOfYear(tt);
      Position R(RX),S(SV);

      return MOPSTropModel::correction(R,S);
   }


      // deprecated
      // Compute and return the full tropospheric delay, given the positions of
      // receiver and satellite and the day of the year. This version is most
      // useful within positioning algorithms, where the receiver position may
      // vary; it computes the elevation (and other receiver location
      // information as height and latitude) and passes them to appropriate
      // methods.
      // @param RX  Receiver position in ECEF cartesian coordinates (meters)
      // @param SV  Satellite position in ECEF cartesian coordinates (meters)
      // @param doy Day of year.
      // This function is deprecated; use the Position version
   double MOPSTropModel::correction( const Xvt& RX,
                                     const Xvt& SV,
                                     const int& doy )
      throw(InvalidTropModel)
   {
      setDayOfYear(doy);
      Position R(RX),S(SV);

      return MOPSTropModel::correction(R,S);
   }


      // Compute and return the zenith delay for the dry component of the
      // troposphere
   double MOPSTropModel::dry_zenith_delay(void) const
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID();

      double ddry, zh_dry, exponent;

         // Set the extra parameters
      double P = MOPSParameters(0);
      double T = MOPSParameters(1);
      double beta = MOPSParameters(3);

         // Zero-altitude dry zenith delay:
      zh_dry = 0.000001*(MOPSk1*MOPSRd)*P/MOPSgm;

         // Zenith delay terms at MOPSHeight meters of height above mean sea
         // level
      exponent = MOPSg/MOPSRd/beta;
      ddry = zh_dry * std::pow( (1.0 - beta*MOPSHeight/T), exponent );

      return ddry;

   }


      // Compute and return the zenith delay for the wet component of the
      // troposphere
   double MOPSTropModel::wet_zenith_delay(void) const
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID();

      double dwet, zh_wet, exponent;

         // Set the extra parameters
      double T = MOPSParameters(1);
      double e = MOPSParameters(2);
      double beta = MOPSParameters(3);
      double lambda = MOPSParameters(4);

         // Zero-altitude wet zenith delay:
      zh_wet = (0.000001*MOPSk2)*MOPSRd/(MOPSgm*(lambda+1.0)-beta*MOPSRd)*e/T;

         // Zenith delay terms at MOPSHeight meters of height above mean sea
         // level
      exponent = ( (lambda+1.0)*MOPSg/MOPSRd/beta)-1.0;
      dwet= zh_wet * std::pow( (1.0 - beta*MOPSHeight/T), exponent );

      return dwet;

   }  // end MOPSTropModel::wet_zenith_delay()


      // This method configure the model to estimate the weather using height,
      // latitude and day of year (DOY). It is called automatically when
      // setting those parameters.
   void MOPSTropModel::setWeather()
      throw(InvalidTropModel)
   {
      if(!validLat)
      {
         valid = false;
         InvalidTropModel e(
            "MOPSTropModel must have Rx latitude before computing weather");
         GPSTK_THROW(e);
      }

      if(!validTime)
      {
         valid = false;
         InvalidTropModel e(
            "MOPSTropModel must have day of year before computing weather");
         GPSTK_THROW(e);
      }

         // In order to compute tropospheric delay we need to compute some
         // extra parameters
      try
      {
         prepareParameters();
      }
      catch(InvalidTropModel& e)
      {
         GPSTK_RETHROW(e);
      }

      valid = validHeight && validLat && validTime;
   }


      /* Define the receiver height; this is required before calling
       *  correction() or any of the zenith_delay routines.
       *
       * @param ht   Height of the receiver above mean sea level, in meters.
       */
   void MOPSTropModel::setReceiverHeight(const double& ht)
   {
      MOPSHeight = ht;
      validHeight = true;

         // Change the value of field "valid" if everything is already set
      valid = validHeight && validLat && validTime;

         // If model is valid, set the appropriate parameters
      if (valid) setWeather();
   }


      /* Define the receiver latitude; this is required before calling
       *  correction() or any of the zenith_delay routines.
       *
       * @param lat  Latitude of receiver, in degrees.
       */
   void MOPSTropModel::setReceiverLatitude(const double& lat)
   {
      MOPSLat = lat;
      validLat = true;

         // Change the value of field "valid" if everything is already set
      valid = validHeight && validLat && validTime;

         // If model is valid, set the appropriate parameters
      if (valid) setWeather();
   }


      /* Set the time when tropospheric correction will be computed for, in
       *  days of the year.
       *
       * @param doy  Day of the year.
       */
   void MOPSTropModel::setDayOfYear(const int& doy)
   {

      if ( (doy>=1) && (doy<=366))
      {
         validTime = true;
      }
      else
      {
         validTime = false;
      }

      MOPSTime = doy;

         // Change the value of field "valid" if everything is already set
      valid = validHeight && validLat && validTime;

         // If model is valid, set the appropriate parameters
      if (valid) setWeather();
   }


      /* Set the time when tropospheric correction will be computed for, in
       *  days of the year.
       *
       * @param time  Time object.
       */
   void MOPSTropModel::setDayOfYear(const CommonTime& time)
   {
      MOPSTime = (int)(static_cast<YDSTime>(time)).doy;
      validTime = true;

         // Change the value of field "valid" if everything is already set
      valid = validHeight && validLat && validTime;

         // If model is valid, set the appropriate parameters
      if (valid) setWeather();
   }


      /* Convenient method to set all model parameters in one pass.
       *
       * @param time  Time object.
       * @param rxPos Receiver position object.
       */
   void MOPSTropModel::setAllParameters( const CommonTime& time,
                                         const Position& rxPos )
   {
      MOPSTime = (int)(static_cast<YDSTime>(time)).doy;
      validTime = true;
      MOPSLat = rxPos.getGeodeticLatitude();
      validHeight = true;
      MOPSLat = rxPos.getHeight();
      validLat = true;

         // Change the value of field "valid" if everything is already set
      valid = validHeight && validLat && validTime;

         // If model is valid, set the appropriate parameters
      if (valid) setWeather();
   }


      // Compute and return the sigma-squared value of tropospheric delay
      // residual error (meters^2)
      // @param elevation  Elevation of satellite as seen at receiver,
      //                   in degrees
   double MOPSTropModel::MOPSsigma2(double elevation)
      throw(InvalidTropModel)
   {
      double map_f;

         // If elevation is below bounds, fail in a sensible way returning a
         // very big sigma value
      if(elevation < 5.0)
      {
         return 9.9e9;
      }
      else
      {
         map_f = MOPSTropModel::mapping_function(elevation);
      }

         // Compute residual error for tropospheric delay
      double MOPSsigma2trop = (0.12*map_f)*(0.12*map_f);

      return MOPSsigma2trop;
   }


      // The MOPS tropospheric model needs to compute several extra parameters
   void MOPSTropModel::prepareParameters(void)
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID();

      try
      {
            // We need to read some data
         prepareTables();

            // Declare some variables
         int idmin, j, index;
         double fact, axfi;
         Vector<double> avr0(5);
         Vector<double> svr0(5);

            // Resize MOPSParameters as appropriate
         MOPSParameters.resize(5);

         if (MOPSLat >= 0.0)
         {
            idmin = 28;
         }
         else
         {
            idmin = 211;
         }

            // Fraction of the year in radians
         fact = 2.0*PI*((double)(MOPSTime-idmin))/365.25;

         axfi = ABS(MOPSLat);

         if ( axfi <= 15.0 )                    index=0;
         if ( (axfi > 15.0) && (axfi <= 30.0) ) index=1;
         if ( (axfi > 30.0) && (axfi <= 45.0) ) index=2;
         if ( (axfi > 45.0) && (axfi <= 60.0) ) index=3;
         if ( (axfi > 60.0) && (axfi <  75.0) ) index=4;
         if ( axfi >= 75.0 )                     index=5;

         for (j=0; j<5; j++)
         {
            if (index == 0) {
               avr0(j)=avr(index,j);
               svr0(j)=svr(index,j);
            }
            else
            {
               if (index < 5)
               {
                  avr0(j) = avr(index-1,j) + (avr(index,j)-avr(index-1,j)) *
                            (axfi-fi0(index-1))/(fi0( index)-fi0(index-1));

                  svr0(j) = svr(index-1,j) + (svr(index,j)-svr(index-1,j)) *
                            (axfi-fi0(index-1))/(fi0( index)-fi0(index-1));
               }
               else
               {
                  avr0(j) = avr(index-1,j);
                  svr0(j) = svr(index-1,j);
               }
            }

            MOPSParameters(j) = avr0(j)-svr0(j)*std::cos(fact);
         }

      } // end try
      catch (...)
      {
         InvalidTropModel e("Problem computing extra MOPS parameters.");
         GPSTK_RETHROW(e);
      }
   }


      // The MOPS tropospheric model uses several predefined data tables
   void MOPSTropModel::prepareTables(void)
   {
      avr.resize(5,5);
      svr.resize(5,5);
      fi0.resize(5);


         // Table avr (Average):

      avr(0,0) = 1013.25; avr(0,1) = 299.65; avr(0,2) = 26.31;
         avr(0,3) = 0.0063; avr(0,4) = 2.77;

      avr(1,0) = 1017.25; avr(1,1) = 294.15; avr(1,2) = 21.79;
         avr(1,3) = 0.00605; avr(1,4) = 3.15;

      avr(2,0) = 1015.75; avr(2,1) = 283.15; avr(2,2) = 11.66;
         avr(2,3) = 0.00558; avr(2,4) = 2.57;

      avr(3,0) = 1011.75; avr(3,1) = 272.15; avr(3,2) = 6.78;
         avr(3,3) = 0.00539; avr(3,4) = 1.81;

      avr(4,0) = 1013.00; avr(4,1) = 263.65; avr(4,2) = 4.11;
         avr(4,3) = 0.00453; avr(4,4) = 1.55;


         // Table svr (Seasonal Variation):

      svr(0,0) = 0.00; svr(0,1) = 0.00; svr(0,2) = 0.00;
         svr(0,3) = 0.00000; svr(0,4) = 0.00;

      svr(1,0) = -3.75; svr(1,1) = 7.00; svr(1,2) = 8.85;
         svr(1,3) = 0.00025; svr(1,4) = 0.33;

      svr(2,0) = -2.25; svr(2,1) = 11.00; svr(2,2) = 7.24;
         svr(2,3) = 0.00032; svr(2,4) = 0.46;

      svr(3,0) = -1.75; svr(3,1) = 15.00; svr(3,2) = 5.36;
         svr(3,3) = 0.00081; svr(3,4) = 0.74;

      svr(4,0) = -0.50; svr(4,1) = 14.50; svr(4,2) = 3.39;
         svr(4,3) = 0.00062; svr(4,4) = 0.30;


         // Table fi0 (Latitude bands):

      fi0(0) = 15.0; fi0(1) = 30.0; fi0(2) = 45.0;
         fi0(3) = 60.0; fi0(4) = 75.0;
   }
}
