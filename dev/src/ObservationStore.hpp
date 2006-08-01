#pragma ident "$Id$"



/**
 * @file ObservationStore.hpp
 * A class encapsulating observation data (roughly standard RINEX obs and met files).
 * It roughly mirrors the data that is contained in
 * an smoothed data file or the combination of a RINEX obs and met file.
 *
 * The basic idea is that it stands alone.  Any computed values
 * or related data is managed outside of this set of classes.
 */

#ifndef GPSTK_OBSERVATIONSTORE_HPP
#define GPSTK_OBSERVATIONSTORE_HPP

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






#include <map>
#include <set>
#include <iostream>

#include "DayTime.hpp"

namespace gpstk
{
      /// This is a single range/phase/doppler at a single time.  It could be a 
      /// smoothed pseudorange, a raw observation from a single carrier or
      /// whatever.  There is no time since it is expected to be part of an
      /// epoch of observations with a single time.
   struct Observation
   {
         /// Constructor
      Observation() throw()
            :range(0), phase(0), doppler(0), SNR(0), channel(0),
             code(rcOther), carrier(cfOther) {}

      double range;     ///< the range in meters
      double phase;     ///< the phase in meters
      double doppler;   ///< the doppler in meters/sec
      float  SNR;       ///< signal to noise ratio in dB/Hz
      short  channel;   ///< what channel this was collected on

         /// An enumeration of the various types of ranging codes.
      enum RangingCode 
      {
         rcCA,     ///<  CA Code
         rcPY,     ///<  Y Code
         rcZ,      ///<  Z Code
         rcRC,     ///<  RC Code 
         rcOther   ///<  Unspecified
      };
      
         /// An enumeration of the various types of carrier frequencies.
      enum CarrierFrequency 
      {
         cfL1,    ///< L1
         cfL2,    ///< L2
         cfL5,    ///< L5
         cfOther  ///< Unspecified
      };
      
      RangingCode code;         ///< the ranging code for this observation
      CarrierFrequency carrier; ///< the carrier freq. for this observation
      
         /** Friendly output operator.
          * @param s the output stream to which the Observation data is written
          * @param obs the Observation to send to the ostream
          * @return a reference to the modified ostream
          */
      friend std::ostream& operator<<(std::ostream& s, 
                                      const Observation& obs) throw();
   };
   

      /// This is a superstructure for the Observation structure.
      /// It helps with insertion of observations into the RxObsData 
      /// structure.  Look below to see how.
   struct ObservationPlus
   {
      Observation obs;    ///< the observation
      DayTime     time;   ///< measurement time in DayTime format
      unsigned    rxId;   ///< some identifying receiver number
      short       svId;   ///< the PRN number for this measurement's SV
   };

      /// The key for this map is the prn of the SV
   typedef std::map<short, Observation> ObsMap;

      /// This is a set of observations (all PRNs) collected at a single point
      /// in time from a single receiver.
   struct ObsEpoch
   {
         /// Constructor
      ObsEpoch() throw()
            :t(DayTime::END_OF_TIME),dt(0) {};

         /** Insert an ObservationPlus.
          * @param op the ObservationPlust to insert
          */
      void insertObservation(const ObservationPlus& op) throw();
         /** Insert an Observation for a specific SV.
          * @param obs the Observation to insert
          * @param svId the SV that Observation belongs to.
          */
      void insertObservation(const Observation& obs, 
                             const short svId) throw();
     
         /// When this data was collected, in Rx GPS time.
      DayTime t;

         /// The receiver clock offset for this epoch.
         /// (an optional RINEX value)
      double dt;
   
         /// The observations indexed by PRN.
      ObsMap obs;

         /** Friendly output operator.
          * @param s the output stream to send output to.
          * @param oe the ObsEpoch to send to the output stream
          * @return a reference to the modified ostream
          */
      friend std::ostream& operator<<(std::ostream& s, 
                                      const ObsEpoch& oe) throw();
   };


      /// This is a map of those epochs over time.  The key should be the
      /// same time as the time of the epoch
   typedef std::map<DayTime, ObsEpoch> ObsEpochMap;

      /// This is a time history of epochs from a single receiver.
   struct RxObsData
   {
         /// Constructor
      RxObsData() throw() 
            :firstTime(DayTime::END_OF_TIME), lastTime(DayTime::BEGINNING_OF_TIME) {};
     
         /** Insert an ObservationPlus object.
          * @param op the ObservationPlus to insert.
          */ 
      void insertObservation(const ObservationPlus& op) throw();
      
         /** Insert an Observation object for a specific time and SV.
          * @param obs the Observation object
          * @param t the time of the Observation
          * @param svId the SV that the Observation is of.
          */
      void insertObservation(const Observation& obs, 
                             const DayTime& t, 
                             const short svId) throw();

         /** Insert an Epoch of Observations.
          * @param epoch the ObsEpoch object to insert
          */
      void insertEpoch(const ObsEpoch& epoch) throw();
     
      DayTime firstTime; ///< Time of the oldest data in this object.
      DayTime lastTime;  ///< Time of the youngest data in this object.
   
      unsigned rxId;     ///< the receiver id

         /// This is a list of all PRNs that we have at least one observation
         /// from for this receiver.
      std::set<short> prnList;

         // The actual data.
      ObsEpochMap epochs;

         /** Friendly output operator.
          * @param s the output stream that the RxObsData is sent to
          * @param rod the RxObsData that is sent to the output stream
          * @return a reference to the modified ostream
          */
      friend std::ostream& operator<<(std::ostream& s, 
                                      const RxObsData& rod) throw();
   };
   
      /// A Single Weather Observation.
   struct WxObservation
   {
         /// Default Constructor
      WxObservation() throw()
            : t(DayTime::END_OF_TIME), temperatureSource(noWx),
              pressureSource(noWx), humiditySource(noWx)
      {}

         /** Constructor
          * @param t the time of the observation
          * @param temp the recorded temperature (deg. Centigrade)
          * @param pres the recorded pressure (millibars)
          * @param humid the recorded relative humidity (%)
          */
      WxObservation(const DayTime& t, 
                    double temp, 
                    double pres, 
                    double humid)
         throw()
            :t(t),
             temperature(temp), temperatureSource(obsWx),
             pressure(pres), pressureSource(obsWx), 
             humidity(humid), humiditySource(obsWx)
      {}


      DayTime t;  ///< Time that this data was collected, in Rx GPS time.
      float  temperature;  ///< degrees Centigrade
      float  pressure;     ///< millibars
      float  humidity;     ///< percent

         /// An enumeration of the various weather sources.
      enum EWxSrc 
      {
         noWx,  ///< No weather measurements were available.
         defWx, ///< Default weather was used.
         obsWx  ///< The Observed Weather was used.
      };
      
      enum EWxSrc temperatureSource; ///< source of the temperature meas.
      enum EWxSrc pressureSource;    ///< source of the pressure meas.
      enum EWxSrc humiditySource;    ///< source of the humidity meas.

         /** Return whether all weather values in this object are valid.
          * @return whether all weather values in this object are valid
          */
      bool isAllValid() const throw();

         /** Friendly Output Operator.
          * @param s the output stream to which data is sent
          * @param obs the WxObservation from which the data comes
          * @return a reference to the modified ostream
          */
      friend std::ostream& operator<<(std::ostream& s, 
                                      const WxObservation& obs) throw();
   };


      /// This is a map of weather observations over time.  The key 
      /// should be the same time as the time of the epoch.
   typedef std::map<DayTime, WxObservation> WxObsMap;

      /// This is a time history weather data from a single site
   struct WxObsData
   {
         /// Constructor
      WxObsData() throw()
            :firstTime(DayTime::END_OF_TIME), 
             lastTime(DayTime::BEGINNING_OF_TIME) {}
     
         // First and last time of any data in this object
      DayTime firstTime; ///< Time of the oldest data in this object.
      DayTime lastTime;  ///< Time of the youngest data in this object.
   
         /// This is the identifier of the site.
      unsigned rxId;

         /// The actual data.
      WxObsMap obs;

         /** Get the last WxObservation made before time t.
          * @return the WxObservation coming before time t
          */
      WxObservation getMostRecent(const DayTime& t) const throw();
      
         /** Insert a WxObservation.
          * @param obs the WxObservation to insert.
          */
      void insertObservation(const WxObservation& obs) throw();
      
         /**
          * Removes all stored #WxObservation objects older than time \a t.
          * \param t remove #WxObservation objects older than this
          */
      void flush(const DayTime& t) throw();

         /**
          * Find a #WxObservation object for time \a t.
          * A #WxObservation object will be retured. If interpolation is used,
          * the first object immediately before or at time \a t and the first
          * object immediately after time \a t will be linearly interpolated
          * between to return a data point at time \a t. If there is not enough
          * data to interpolate, the nearest real observation will be returned.
          * Any object returned or used in the interpolation scheme must fit
          * within the specified time interval around \a t (if \a iv is one
          * hour, a point must be within an hour before or after time \a t.)
          * \param t the time of interest
          * \param interpolate true if interpolation between points is wanted
          * \param iv time interval (seconds, before and after) around \a t
          * \return a #WxObservation object
          * \exception ObjectNotFound a #WxObservation object not available
          */
      WxObservation getWxObservation(const DayTime& t,
                                     unsigned iv = 3600,
                                     bool interpolate = true) const
         throw(ObjectNotFound);
   };
} // namespace 

#endif 
