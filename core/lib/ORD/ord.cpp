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

#include <typeinfo>
#include <vector>
#include "ord.hpp"
#include "GPSEllipsoid.hpp"

using std::vector;
using std::cout;

namespace gpstk {
namespace ord {

// When calculating range with the receiver's clock, the rotation of the earth
// during the time between transmission and receipt must be included. This
// updates svPosVel to account for that rotation.
//
Xvt rotateEarth(const Position& Rx, const Xvt& svPosVel,
        const EllipsoidModel& ellipsoid) {
    Xvt revisedXvt(svPosVel);

    double tof = RSS(svPosVel.x[0] - Rx.X(), svPosVel.x[1] - Rx.Y(),
            svPosVel.x[2] - Rx.Z()) / ellipsoid.c();
    double wt = ellipsoid.angVelocity() * tof;
    double sx = ::cos(wt) * svPosVel.x[0] + ::sin(wt) * svPosVel.x[1];
    double sy = -::sin(wt) * svPosVel.x[0] + ::cos(wt) * svPosVel.x[1];
    revisedXvt.x[0] = sx;
    revisedXvt.x[1] = sy;
    sx = ::cos(wt) * svPosVel.v[0] + ::sin(wt) * svPosVel.v[1];
    sy = -::sin(wt) * svPosVel.v[0] + ::cos(wt) * svPosVel.v[1];
    revisedXvt.v[0] = sx;
    revisedXvt.v[1] = sy;

    return revisedXvt;
}

double IonosphereFreeRange(const std::vector<double>& frequencies,
        const std::vector<double>& pseudoranges) {
    return 0;
}

double IonosphereModelCorrection(const gpstk::IonoModelStore& iono_model,
        const gpstk::Position& rx_loc, const gpstk::Xvt& sv_xvt) {
    return 0;
}

gpstk::Xvt getSvXvt(const gpstk::SatID& satId, const gpstk::CommonTime& time,
        const gpstk::XvtStore<gpstk::SatID>& ephemeris) {
    return ephemeris.getXvt(satId, time);
}

double RawRange1(const gpstk::Position& rxLoc, const gpstk::SatID& satId,
        const gpstk::CommonTime& timeReceived,
        const gpstk::XvtStore<gpstk::SatID>& ephemeris, gpstk::Xvt& svXvt) {
    try {
        int nit;
        double tof, tof_old, rawrange;
        GPSEllipsoid ellipsoid;

        CommonTime transmit(timeReceived);
        Xvt svPosVel;     // Initialize to zero

        nit = 0;
        tof = 0.07;       // Initial guess 70ms
        do {
            // best estimate of transmit time
            transmit = timeReceived;
            transmit -= tof;
            tof_old = tof;
            // get SV position
            try {
                svPosVel = ephemeris.getXvt(satId, transmit);
            } catch (InvalidRequest& e) {
                GPSTK_RETHROW(e);
            }

            svPosVel = rotateEarth(rxLoc, svPosVel, ellipsoid);
            // update raw range and time of flight
            rawrange = RSS(svPosVel.x[0] - rxLoc.X(), svPosVel.x[1] - rxLoc.Y(),
                    svPosVel.x[2] - rxLoc.Z());
            tof = rawrange / ellipsoid.c();
        } while (ABS(tof-tof_old) > 1.e-13 && ++nit < 5);

        svXvt = svPosVel;

        return rawrange;
    } catch (gpstk::Exception& e) {
        GPSTK_RETHROW(e);
    }
}

double RawRange2(double pseudorange, const gpstk::Position& rx_loc,
        const gpstk::SatID& sat_id, const gpstk::CommonTime& time,
        const gpstk::XvtStore<gpstk::SatID>& ephemeris, gpstk::Xvt& sv_xvt) {
    return 0;
}

double RawRange3(double pseudorange, const gpstk::Position& rx_loc,
        const gpstk::SatID& sat_id, const gpstk::CommonTime& time,
        const gpstk::XvtStore<gpstk::SatID>& ephemeris, gpstk::Xvt& sv_xvt) {
    return 0;
}

double RawRange4(const gpstk::Position& rx_loc, const gpstk::SatID& sat_id,
        const gpstk::CommonTime& time,
        const gpstk::XvtStore<gpstk::SatID>& ephemeris, gpstk::Xvt& sv_xvt) {
    return 0;
}

double SvClockBiasCorrection(const gpstk::Xvt& svXvt) {
    double svclkbias = svXvt.clkbias * C_MPS;
    double svclkdrift = svXvt.clkdrift * C_MPS;
    return -svclkbias;
}

double SvRelativityCorrection(gpstk::Xvt& svXvt) {
    std::cout << "Data Type of svXvt is:" << typeid(svXvt).name() << std::endl;
    double relativity = svXvt.computeRelativityCorrection() * C_MPS;
    return -relativity;
}

double TroposphereCorrection(const gpstk::TropModel& trop_model,
        const gpstk::Position& rx_loc, const gpstk::Xvt& sv_xvt) {
    return 0;
}

}  // namespace ord
}  // namespace gpstk
