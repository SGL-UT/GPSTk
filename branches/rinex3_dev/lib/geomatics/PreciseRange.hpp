#pragma ident "$Id$"

/**
 * @file PreciseRange.hpp
 * Include file defining class PreciseRange: computation of range and associated
 * quantities from XvtStore, given receiver position and time.
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

//------------------------------------------------------------------------------------
#ifndef PRECISE_EPHEMERIS_RANGE_INCLUDE
#define PRECISE_EPHEMERIS_RANGE_INCLUDE

//------------------------------------------------------------------------------------
// GPSTk
#include "DayTime.hpp"
#include "Position.hpp"
#include "XvtStore.hpp"
#include "SatID.hpp"
#include "Matrix.hpp"

// geomatics
#include "AntexData.hpp"
#include "SolarSystem.hpp"
#include "EarthOrientation.hpp"

//------------------------------------------------------------------------------------
namespace gpstk
{
   /** @addtogroup ephemcalc */
   //@{

   /// class PreciseRange. Compute the corrected range from receiver
   /// at position Rx, to the GPS satellite given by SatID sat, as well as azimuth,
   /// elevation, etc., given a nominal timetag (either received or transmitted
   /// time) and an XvtStore.
   class PreciseRange
   {
   public:
         /// Default constructor.
      PreciseRange() {}

      /// Compute the corrected range at transmit time from ephemeris is the given
      /// XvtStore, from receiver at position Rx with measured pseudorange pr and
      /// time tag nomRecTime, to the GPS satellite given by SatID sat,
      /// as well as all the CER quantities.
      /// @param DayTime nomRecTime  nominal receive time
      /// @param double pr           measured pseudorange at this time
      /// @param Position& Rx        receiver position
      /// @param SatID sat           satellite
      /// @param AntexData& antenna  satellite antenna data;
      ///                               if not valid, no PCO/V correction is done
      /// @param SolarSystem SSEph   Solar system ephemeris
      /// @param EarthOrientation EO Earth orientation parameters appropriate for time
      /// @param XvtStore Eph        Ephemeris store
      /// @param bool isCOM          if true, Eph is Center-of-mass,
      ///                               else antenna-phase-center, default false.
      /// @return corrected raw range
      /// @throw if ephemeris is not found
      double ComputeAtTransmitTime(const DayTime& nomRecTime,
                                   const double pr,
                                   const Position& Rx,
                                   const SatID sat,
                                   const AntexData& antenna,
                                   const SolarSystem& SSEph,
                                   const EarthOrientation& EO,
                                   const XvtStore<SatID>& Eph,
                                   const bool isCOM=false)
         throw(Exception);

      /// Version with no antenna; cf. doc for other version.
      double ComputeAtTransmitTime(const DayTime& nomRecTime,
                                   const double pr,
                                   const Position& Rx,
                                   const SatID sat,
                                   const XvtStore<SatID>& Eph)
         throw(Exception)
      {
         // antdummy will be invalid, so antenna computations will be skipped;
         // thus SolarSystem and EarthOrientation will never be needed.
         SolarSystem ssdum;
         EarthOrientation eodum;
         AntexData antdummy;
         return ComputeAtTransmitTime(nomRecTime,pr,Rx,sat,antdummy,ssdum,eodum,Eph);
      }

      /// Version without high-accuracy SolarSystem; cf. doc for other version.
      double ComputeAtTransmitTime(const DayTime& nomRecTime,
                                   const double pr,
                                   const Position& Rx,
                                   const SatID sat,
                                   const AntexData& antenna,
                                   const XvtStore<SatID>& Eph)
         throw(Exception)
      {
         // ssdummy will be invalid, so SolarPosition will be used;
         // thus EarthOrientation will never be needed.
         SolarSystem ssdummy;
         EarthOrientation eodum;
         return ComputeAtTransmitTime(nomRecTime,pr,Rx,sat,antenna,ssdummy,eodum,Eph);
      }

      /// The computed raw (geometric) range in meters, with NO corrections applied;
      /// to correct it, use
      /// rawrange -= satclkbias+relativity+relativity2-satLOSPCO-satLOSPCV.
      double rawrange;

      /// The relativity correction in meters, and high precision correction
      double relativity, relativity2;

      /// The satellite position (m) and velocity (m/s) in ECEF coordinates
      Position SatR, SatV;

      /// The satellite clock bias (m) and drift (m/s) at transmit time, from XvtStore
      double satclkbias, satclkdrift;

      /// The satellite elevation (spheroidal), as seen at the receiver, in degrees.
      double elevation;

      /// The satellite azimuth (spheroidal), as seen at the receiver, in degrees.
      double azimuth;

      /// The satellite elevation (geodetic), as seen at the receiver, in degrees.
      double elevationGeodetic;

      /// The satellite azimuth (geodetic), as seen at the receiver, in degrees.
      double azimuthGeodetic;

      /// The computed transmit time of the signal.
      DayTime transmit;

      /// The direction cosines of the satellite, as seen at the receiver (XYZ).
      Triple cosines;

      /// The net line-of-sight offset, in the direction from sat to rx,
      /// of the antenna PCO and PCVs, meters
      double satLOSPCO,satLOSPCV;

      /// The Satellite PCO vector, in ECEF XYZ, meters (from COM to PC)
      Vector<double> SatPCOXYZ;

      /// Net time delay due to Sagnac effect in seconds
      double Sagnac;


   }; // end class PreciseRange

   /// Compute relativity correction (meters) from the satellite position and velocity
   double RelativityCorrection(const Position& R, const Position& V);

   //@}

}  // namespace gpstk

#endif // PRECISE_EPHEMERIS_RANGE_INCLUDE
