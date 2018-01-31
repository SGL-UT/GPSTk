//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  Copyright 2004, The University of Texas at Austin
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
//============================================================================

//============================================================================
//
// This software developed by Applied Research Laboratories at the University of
// Texas at Austin, under contract to an agency or agencies within the U.S.
// Department of Defense. The U.S. Government retains all rights to use,
// duplicate, distribute, disclose, or release this software.
//
// Pursuant to DoD Directive 523024
//
// DISTRIBUTION STATEMENT A: This software has been approved for public
//                           release, distribution is unlimited.
//
//=============================================================================

/*
 * ord.hpp - ORD Header File
 *
 * This file should expose all of the 'atomic' entry points for calculating the Observed Range Deviation.
 *
 * Still to do:
 *   - Rename raw_range_x() methods to more appropriate names.
 *   - Replace raw_range index with proper enumeration.
 */

#ifndef CORE_LIB_ORD_ORD_HPP_
#define CORE_LIB_ORD_ORD_HPP_

#include <vector>

#include "IonoModelStore.hpp"
#include "CommonTime.hpp"
#include "SatID.hpp"
#include "Position.hpp"
#include "XvtStore.hpp"
#include "TropModel.hpp"

namespace gpstk {
namespace ord {
// Given a set of frequency and pseudorange pairs, attempts to compensate
// for ionospheric effects.
// Comment: Vectors are used because they carry a size parameter, another
//          collection type may be more appropriate.
// Parameters:
//      frequencies (vector double): Signal frequencies.
//      pseudoranges (vector double): Pseudorange values, corresponding
//                                    to frequency array.
// Returns: Corrected pseudorange in meters
double IonosphereFreeRange(const std::vector<double>& frequencies,
        const std::vector<double>& pseudoranges);

// Given an ionosphere model, and locations of receiver and satellite,
// range correction due to ionospheric effects.
// Parameters:
//      iono_model (IonoModelStore): Class that encapsulates ionospheric models
//      rx_loc (Position): The location of the receiver.
//      sv_loc (Xvt): The location of the satellite at time of interest.
// Returns: range correction (delta) in meters
double IonosphereModelCorrection(const gpstk::IonoModelStore& ionoModel,
        const gpstk::Position& rxLoc, const gpstk::Xvt& svXvt);

// Given a satellite id, a time, and an ephemeris store, retrieves the
// satellite location/velocity in xvt instance.
// Note: This is a relatively thin wrapper for XvtStore.getXvt() to bring
//       the method into the same namespace as the other range calculations.
// Parameters:
//      sat_id (SatID): Identifier for the satellite
//      time (CommonTime): The time of interest.
//      ephemeris (XvtStore): The ephemeris to query against.
// Returns: Xvt instance containing satellite location/velocity
gpstk::Xvt getSvXvt(const gpstk::SatID& sat_id, const gpstk::CommonTime& time,
        const gpstk::XvtStore<gpstk::SatID>& ephemeris);

// Calculate the raw range at RECEIVE time per RECEIVER clock.
// Parameters:
//      rx_loc (Position): The location of the receiver.
//      sat_id (SatID): Identifier for the satellite
//      time (CommonTime): The nominal receive time.
//      ephemeris (XvtStore): The ephemeris to query against.
// Returns:
//        sv_xvt (Xvt): Satellite location/velocity
//      (primary) Range in meters
double RawRange1(const gpstk::Position& rx_loc, const gpstk::SatID& sat_id,
        const gpstk::CommonTime& time,
        const gpstk::XvtStore<gpstk::SatID>& ephemeris, gpstk::Xvt& sv_xvt);

// Calculate the raw range at TRANSMIT time per the RECEIVER clock.
// Parameters:
//      pseudorange (double): Pseudorange in meters to seed the calculation.
//      rx_loc (Position): The location of the receiver.
//      sat_id (SatID): Identifier for the satellite
//      time (CommonTime): The nominal receive time.
//      ephemeris (XvtStore): The ephemeris to query against.
// Returns:
//      (primary) Range in meters
//        sv_xvt (Xvt): Satellite location/velocity
double RawRange2(double pseudorange, const gpstk::Position& rx_loc,
        const gpstk::SatID& sat_id, const gpstk::CommonTime& time,
        const gpstk::XvtStore<gpstk::SatID>& ephemeris, gpstk::Xvt& sv_xvt);

// Calculate the raw range at TRANSMIT time per the SATELLITE clock
// Parameters:
//      pseudorange (double): Pseudorange in meters to seed the calculation.
//      rx_loc (Position): The location of the receiver.
//      sat_id (SatID): Identifier for the satellite
//      time (CommonTime): The transmit time reported by satellite.
//      ephemeris (XvtStore): The ephemeris to query against.
// Returns:
//      (primary) Range in meters
//        sv_xvt (Xvt): Satellite location/velocity
double RawRange3(double pseudorange, const gpstk::Position& rx_loc,
        const gpstk::SatID& sat_id, const gpstk::CommonTime& time,
        const gpstk::XvtStore<gpstk::SatID>& ephemeris, gpstk::Xvt& sv_xvt);

// Calculate the raw range at TRANSMIT time per RECEIVER clock, without
// seeding the pseudorange.
// Parameters:
//      rx_loc (Position): The location of the receiver.
//      sat_id (SatID): Identifier for the satellite
//      time (CommonTime): The nominal receive time.
//      ephemeris (XvtStore): The ephemeris to query against.
// Returns:
//      (primary) Range in meters
//        sv_xvt (Xvt): Satellite location/velocity
double RawRange4(const gpstk::Position& rx_loc, const gpstk::SatID& sat_id,
        const gpstk::CommonTime& time,
        const gpstk::XvtStore<gpstk::SatID>& ephemeris, gpstk::Xvt& sv_xvt);

// Calculate the range delta due to clock bias.
// Note: Most of the work is actually done by the Xvt object.
// Parameters:
//      sv_xvt (Xvt): Satellite location/velocity
// Returns: Range correction (delta) in meters
double SvClockBiasCorrection(const gpstk::Xvt& svXvt);

// Calculate the range delta due to relativistic effects
// Note: Most of the work is actually done by the Xvt object.
// Parameters:
//        sv_xvt (Xvt): Satellite location/velocity
// Returns:
//        Range correction (delta) in meters
double SvRelativityCorrection(gpstk::Xvt& svXvt);

// Given a troposphere model, and locations of receiver and satellite,
// calculates tropospheric effects.
// Parameters:
//      trop_model (TropModel): Class that encapsulates ionospheric models
//      rx_loc (Position): The location of the receiver.
//      sv_loc (Xvt): The location of the satellite at time of interest.
// Returns: range correction (delta) in meters
double TroposphereCorrection(const gpstk::TropModel& trop_model,
        const gpstk::Position& rx_loc, const gpstk::Xvt& sv_xvt);

// Example method that applies _all_ corrections to generate the
// Observed Range Deviation.
// This is intended to indicate how the above methods will be used.
// Parameters:
//      frequencies (vector double): Signal frequencies.
//      pseudoranges (vector double): Pseudorange values, corresponding
//                                    to frequency array.
//      trop_model (TropModel): Class that encapsulates ionospheric models
//      rx_loc (Position): The location of the receiver.
//      sat_id (SatID): Identifier for the satellite
//      transmit_time (CommonTime): The transmit time reported by satellite.
//      receive_time (CommonTime): The nominal receive time.
//      iono_model (IonoModelStore): Class that encapsulates ionospheric models
//      trop_model (TropModel): Class that encapsulates ionospheric models
//      ephemeris (XvtStore): The ephemeris to query against.
// Returns:
//      (double) ORD - Observed range deviation from pseudorange
//                     (or effective pseudorange if multiple are provided)
double calculate_ord(const std::vector<double>& frequencies,
        const std::vector<double>& pseudoranges, const gpstk::Position& rx_loc,
        const gpstk::SatID& sat_id, const gpstk::CommonTime& transmit_time,
        const gpstk::CommonTime& receive_time,
        const gpstk::IonoModelStore& iono_model,
        const gpstk::TropModel& trop_model,
        const gpstk::XvtStore<gpstk::SatID>& ephemeris, int range_method) {
    double ps_range = IonosphereFreeRange(frequencies, pseudoranges);

    gpstk::Xvt sv_xvt;
    // find raw_range
    double range = 0;
    switch (range_method) {
    case 1:
        range = RawRange1(rx_loc, sat_id, receive_time, ephemeris, sv_xvt);
        break;
    case 2:
        range = RawRange2(ps_range, rx_loc, sat_id, receive_time, ephemeris,
                sv_xvt);
        break;
    case 3:
        range = RawRange3(ps_range, rx_loc, sat_id, transmit_time, ephemeris,
                sv_xvt);
        break;
    case 4:
        range = RawRange4(rx_loc, sat_id, receive_time, ephemeris, sv_xvt);
        break;
    }

    // apply sv relativity correction
    range += SvRelativityCorrection(sv_xvt);

    // apply sv clock bias correction
    range += SvClockBiasCorrection(sv_xvt);

    // apply troposphere model correction
    range += TroposphereCorrection(trop_model, rx_loc, sv_xvt);

    // apply ionosphere model correction
    range += IonosphereModelCorrection(iono_model, rx_loc, sv_xvt);

    return ps_range - range;
}

}  // namespace ord
}  // namespace gpstk

#endif  // CORE_LIB_ORD_ORD_HPP_
