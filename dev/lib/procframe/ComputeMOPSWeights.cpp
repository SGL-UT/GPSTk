#pragma ident "$Id$"

/**
 * @file ComputeMOPSWeights.cpp
 * This class computes satellites weights based on the Appendix J of MOPS C,
 * and is meant to be used with GNSS data structures.
 */

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2006, 2007, 2008
//
//============================================================================


#include "ComputeMOPSWeights.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int ComputeMOPSWeights::classIndex = 6100000;


      // Returns an index identifying this object.
   int ComputeMOPSWeights::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string ComputeMOPSWeights::getClassName() const
   { return "ComputeMOPSWeights"; }



      /* Returns a satTypeValueMap object, adding the new data
       * generated when calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& ComputeMOPSWeights::Process( const DayTime& time,
                                                 satTypeValueMap& gData )
      throw(ProcessingException)
   {

      try
      {

            // IURA weights are needed, so they are inserted in GDS
         ComputeIURAWeights::Process(time, gData);

         double weight(0.000001);   // By default a very small value
         SatIDSet satRejectedSet;

            // Loop through all the satellites
         satTypeValueMap::iterator it;
         for( it = gData.begin(); it != gData.end(); ++it )
         {

            try
            {
               weight = getWeight( ((*it).first), ((*it).second) );
            }
            catch(...)
            {

                  // If some value is missing, then schedule this satellite
                  // for removal
               satRejectedSet.insert( (*it).first );

               continue;

            }

               // If everything is OK, then get the new value inside
               // the GDS structure
            (*it).second[TypeID::weight] = weight;

         }

            // Remove satellites with missing data
         gData.removeSatID(satRejectedSet);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'ComputeMOPSWeightsWeights::Process()'



      /* Returns a gnnsSatTypeValue object, adding the new data
       * generated when calling this object.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& ComputeMOPSWeights::Process(gnssSatTypeValue& gData)
      throw(ProcessingException)
   {

      try
      {

         Process(gData.header.epoch, gData.body);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'ComputeMOPSWeightsWeights::Process()'



      /* Returns a gnnsRinex object, adding the new data generated
       * when calling this object.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& ComputeMOPSWeights::Process(gnssRinex& gData)
      throw(ProcessingException)
   {

      try
      {

         Process(gData.header.epoch, gData.body);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'ComputeMOPSWeightsWeights::Process()'



      /* Method to set the default ephemeris to be used with GNSS
       * data structures.
       *
       * @param ephem     EphemerisStore object to be used
       */
   ComputeMOPSWeights& ComputeMOPSWeights::setDefaultEphemeris(
                                                   XvtStore<SatID>& ephem )
   {

         // Let's check what type ephem belongs to
      if( dynamic_cast<GPSEphemerisStore*>(&ephem) )
      {
         pBCEphemeris = dynamic_cast<GPSEphemerisStore*>(&ephem);
         pTabEphemeris = NULL;
      }
      else
      {
         pBCEphemeris = NULL;
         pTabEphemeris = dynamic_cast<TabularEphemerisStore*>(&ephem);
      }

      return (*this);

   }  // End of method 'ComputeMOPSWeights::setDefaultEphemeris()'



      /* Method to really get the MOPS weight of a given satellite.
       *
       * @param sat           Satellite
       *
       */
   double ComputeMOPSWeights::getWeight( const SatID& sat,
                                         typeValueMap& tvMap )
      throw(InvalidWeights)
   {

         // Value to store computed iono correction
      double ionocorr(0.0);

         // Receiver noise sigma^2 in meters^2, by default a very big value
      double weight(0.000001);

         // Receiver noise sigma^2 in meters^2, by default a very big value
      double sigma2rx(1000000.0);

         // Set receiver noise according to receiver class
      if( receiverClass==1 )
      {
         sigma2rx = 0.25;
      }
      else
      {
         sigma2rx = 0.36;
      }

         // Some extra variables. By default they have very big values
      double sigma2ura(1000000.0), sigma2multipath(1000000.0);
      double sigma2trop(1000000.0), sigma2uire(1000000.0);

      try
      {
            // We need a MOPSTropModel object. Parameters must be valid
            // but they have no importance
         MOPSTropModel mopsTrop(0.0, 0.0, 1);

            // At first, the weight type have just the IURA weight
            // which must have been computed elsewhere in this class
         sigma2ura = (1.0 / tvMap(TypeID::weight) );

            // Estimate multipath noise according to satellite elevation
         sigma2multipath = 0.13 +
            ( 0.53 * std::exp( -(tvMap(TypeID::elevation)) / 10.0) );

            // Use MOPSTropModel object to compute tropospheric noise
         sigma2trop = mopsTrop.MOPSsigma2(tvMap(TypeID::elevation));

            // Check if ionosphere values were already computed
         if( tvMap.find(defaultIono) != tvMap.end() )
         {
            ionocorr = tvMap(defaultIono);
         }

         sigma2uire = sigma2iono( ionocorr,
                                  tvMap(TypeID::elevation),
                                  tvMap(TypeID::azimuth),
                                  nominalPos );

         weight = 1.0 / ( sigma2rx + sigma2ura + sigma2multipath +
                          sigma2trop + sigma2uire );

      }
      catch(...)
      {
         InvalidWeights eWeight( "Problem when computing weights. Did you \
call a modeler class?." );
         GPSTK_THROW(eWeight);
      }

      return weight;

   }  // End of method 'ComputeMOPSWeightsWeights::getWeight()'



      // Compute ionospheric sigma^2 according to Appendix J.2.3
      // and Appendix A.4.4.10.4 in MOPS-C
   double ComputeMOPSWeights::sigma2iono( const double& ionoCorrection,
                                          const double& elevation,
                                          const double& azimuth,
                                          const Position& rxPosition )
      throw(InvalidWeights)
   {

         // First, let's found magnetic latitude according to ICD-GPS-200,
         // section 20.3.3.5.2.6
      double azRad = azimuth * DEG_TO_RAD;
      double elevRad = elevation * DEG_TO_RAD;
      double cosElev = std::cos(elevRad);
      double svE = elevation / 180.0;     // Semi-circles

      double phi_u = rxPosition.getGeodeticLatitude() / 180.0; // Semi-circles
      double lambda_u = rxPosition.getLongitude() / 180.0;     // Semi-circles

      double psi = (0.0137 / (svE + 0.11)) - 0.022;            // Semi-circles

      double phi_i = phi_u + psi * std::cos(azRad);            // Semi-circles

      if( phi_i > 0.416 )
      {
         phi_i = 0.416;
      }

      if( phi_i < -0.416 )
      {
         phi_i = -0.416;
      }

      double lambda_i = lambda_u +
            ( psi * std::sin(azRad) / std::cos(phi_i*PI) );    // Semi-circles

         // Semi-circles
      double phi_m = phi_i + 0.064 * std::cos((lambda_i - 1.617)*PI);

         // Convert magnetic latitude to degrees
      phi_m = std::abs(phi_m * 180.0);

         // Estimate vertical ionospheric delay according to MOPS-C
      double tau_vert;
      if( (phi_m >= 0.0) && (phi_m <= 20.0) )
      {
         tau_vert = 9.0;
      }
      else
      {
         if( (phi_m > 20.0) && (phi_m <= 55.0) )
         {
            tau_vert = 4.5;
         }
         else
         {
            tau_vert = 6.0;
         }

      }

         // Compute obliquity factor
      double fpp = ( 1.0 / (std::sqrt(1.0 - 0.898665418 * cosElev*cosElev)) );

      double sigma2uire = ( (ionoCorrection*ionoCorrection) / 25.0 );

      double fact = ( (fpp*tau_vert) * (fpp*tau_vert) );

      if (fact > sigma2uire)
      {
         sigma2uire = fact;
      }

      return sigma2uire;

   }  // End of method 'ComputeMOPSWeightsWeights::sigma2iono()'



}  // End of namespace gpstk
