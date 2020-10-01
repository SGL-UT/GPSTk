//==============================================================================
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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

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

/// Given a set of frequency and pseudorange pairs, attempts to compensate
/// for ionospheric effects. Vectors are used because they carry a size
/// parameter, another collection type may be more appropriate.
/// @param frequencies Signal frequencies.
/// @param pseudoranges Pseudorange values for frequencies.
/// @return Corrected pseudorange in meters
double IonosphereFreeRange(const std::vector<double>& frequencies,
        const std::vector<double>& pseudoranges);

/// Given an ionosphere model, and locations of receiver and satellite,
/// range correction due to ionospheric effects.
/// TODO(someone): IonoModel assumes only L1 and L2 frequencies, this
/// should be updated to work with an arbitrary frequency.  Currently
/// This call assumes frequency is L1.
/// @param ionoModel Class that encapsulates ionospheric models
/// @params time The time of interest.
/// @params frequency Frequency of interest - see note above.
/// @param rx_loc The location of the receiver.
/// @param sv_loc The location of the satellite at time of interest.
/// @return Range correction (delta) in meters
double IonosphereModelCorrection(const gpstk::IonoModelStore& ionoModel,
        const gpstk::CommonTime& time, double frequency,
        const gpstk::Position& rxLoc, const gpstk::Xvt& svXvt);

/// Given a satellite id, a time, and an ephemeris store, retrieves the
/// satellite location/velocity in xvt instance. This is a relatively thin
/// wrapper for XvtStore.getXvt() to bring the method into the same
/// namespace as the other range calculations.
/// @params sat_id Identifier for the satellite
/// @params time The time of interest.
/// @params ephemeris The ephemeris to query against.
/// @return Xvt instance containing satellite location/velocity
gpstk::Xvt getSvXvt(const gpstk::SatID& sat_id, const gpstk::CommonTime& time,
        const gpstk::XvtStore<gpstk::SatID>& ephemeris);

/// Calculate the raw range at RECEIVE time per RECEIVER clock.
/// @params rx_loc The location of the receiver.
/// @params sat_id Identifier for the satellite
/// @params time The nominal receive time.
/// @params ephemeris The ephemeris to query against.
/// @params sv_xvt Final SV Position/Velocity returned here.
/// @return Range in meters
double RawRange1(const gpstk::Position& rx_loc, const gpstk::SatID& sat_id,
        const gpstk::CommonTime& time,
        const gpstk::XvtStore<gpstk::SatID>& ephemeris, gpstk::Xvt& sv_xvt);

/// Calculate the raw range at TRANSMIT time per the RECEIVER clock.
/// @params pseudorange Pseudorange in meters to seed the calculation.
/// @params rx_loc The location of the receiver.
/// @params sat_id Identifier for the satellite
/// @params time The nominal receive time.
/// @params ephemeris The ephemeris to query against.
/// @params sv_xvt Final SV Position/Velocity returned here.
/// @return Range in meters
double RawRange2(double pseudorange, const gpstk::Position& rx_loc,
        const gpstk::SatID& sat_id, const gpstk::CommonTime& time,
        const gpstk::XvtStore<gpstk::SatID>& ephemeris, gpstk::Xvt& sv_xvt);

/// Calculate the raw range at TRANSMIT time per the SATELLITE clock
/// @params pseudorange Pseudorange in meters to seed the calculation.
/// @params rx_loc The location of the receiver.
/// @params sat_id Identifier for the satellite
/// @params time The transmit time reported by satellite.
/// @params ephemeris The ephemeris to query against.
/// @params sv_xvt Final SV Position/Velocity returned here.
/// @return Range in meters
double RawRange3(double pseudorange, const gpstk::Position& rx_loc,
        const gpstk::SatID& sat_id, const gpstk::CommonTime& time,
        const gpstk::XvtStore<gpstk::SatID>& ephemeris, gpstk::Xvt& sv_xvt);

/// Calculate the raw range at TRANSMIT time per RECEIVER clock, without
/// seeding the pseudorange.
/// @params rx_loc The location of the receiver.
/// @params sat_id Identifier for the satellite
/// @params time The nominal receive time.
/// @params ephemeris The ephemeris to query against.
/// @params sv_xvt Final SV Position/Velocity returned here.
/// @return Range in meters
double RawRange4(const gpstk::Position& rx_loc, const gpstk::SatID& sat_id,
        const gpstk::CommonTime& time,
        const gpstk::XvtStore<gpstk::SatID>& ephemeris, gpstk::Xvt& sv_xvt);

/// Calculate the range delta due to clock bias.
/// Note: Most of the work is actually done by the Xvt object.
/// @params sv_xvt Satellite location/velocity
/// @returns Range correction (delta) in meters
double SvClockBiasCorrection(const gpstk::Xvt& svXvt);

// Calculate the range delta due to relativistic effects
// Note: Most of the work is actually done by the Xvt object.
/// @params sv_xvt Satellite location/velocity
/// @returns Range correction (delta) in meters
double SvRelativityCorrection(gpstk::Xvt& svXvt);

/// Given a troposphere model, and locations of receiver and satellite,
/// calculates tropospheric effects.
/// @param trop_model Class that encapsulates troposphere models
/// @param rx_loc The location of the receiver.
/// @param sv_loc The location of the satellite at time of interest.
/// @return Range correction (delta) in meters
double TroposphereCorrection(const gpstk::TropModel& trop_model,
        const gpstk::Position& rx_loc, const gpstk::Xvt& sv_xvt);

/// Example method that applies _all_ corrections to generate the
/// Observed Range Deviation.
/// This is intended to be a sample showing how the above methods will be used.
/// Parameters:
/// @params frequencies Signal frequencies.
/// @params pseudoranges Pseudorange values, corresponding to frequency array.
/// @params trop_model Class that encapsulates ionospheric models
/// @params rx_loc The location of the receiver.
/// @params sat_id Identifier for the satellite
/// @params transmit_time The transmit time reported by satellite.
/// @params receive_time The nominal receive time.
/// @params iono_model Class that encapsulates ionospheric models
/// @params trop_model Class that encapsulates troposphere models
/// @params ephemeris The ephemeris to query against.
/// @returns Observed range deviation from 1st pseudorange
double calculate_ord(const std::vector<double>& frequencies,
        const std::vector<double>& pseudoranges, const gpstk::Position& rx_loc,
        const gpstk::SatID& sat_id, const gpstk::CommonTime& transmit_time,
        const gpstk::CommonTime& receive_time,
        const gpstk::IonoModelStore& iono_model,
        const gpstk::TropModel& trop_model,
        const gpstk::XvtStore<gpstk::SatID>& ephemeris, int range_method);

}  // namespace ord
}  // namespace gpstk

#endif  // CORE_LIB_ORD_ORD_HPP_
