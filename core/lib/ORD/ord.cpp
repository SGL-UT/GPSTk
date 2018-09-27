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
#include "GNSSconstants.hpp"

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
    // Check vectors are same length
    if (frequencies.size() != pseudoranges.size()) {
        gpstk::Exception exc(
            "Mismatch between frequency and pseudorange array size");
        GPSTK_THROW(exc)
    }

    // Check vectors are at least two
    if (frequencies.size() < 2) {
        gpstk::Exception exc(
            "Multiple frequency and range values are required.");
        GPSTK_THROW(exc)
    }

    // Check vectors aren't greater than two
    if (frequencies.size() > 2) {
        gpstk::Exception exc(
            "Only dual-frequency ionosphere correction is supported.");
        GPSTK_THROW(exc)
    }

    // TODO(someone): Add proper gamma calculation for arbitrary
    //                number of frequencies.
    const double gamma = (frequencies[0]/frequencies[1]) *
                         (frequencies[0]/frequencies[1]);

    // for dual frequency see IS-GPS-200, section 20.3.3.3.3.3
    double icpr = (pseudoranges[1] - gamma * pseudoranges[0])/(1-gamma);

    return icpr;
}

double IonosphereModelCorrection(const gpstk::IonoModelStore& ionoModel,
        const gpstk::CommonTime& time, double freq,
        const gpstk::Position& rxLoc, const gpstk::Xvt& svXvt) {
    Position trx(rxLoc);
    Position svPos(svXvt);

    double elevation = trx.elevation(svPos);
    double azimuth = trx.azimuth(svPos);

    // TODO(someone): IonoModel assumes only L1 and L2 frequencies, this
    // should be updated to work with an arbitrary frequency.
    double iono = ionoModel.getCorrection(time, trx,
                                          elevation, azimuth,
                                          IonoModel::L1);
    return -iono;
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

double RawRange2(double pseudorange, const gpstk::Position& rxLoc,
        const gpstk::SatID& satId, const gpstk::CommonTime& time,
        const gpstk::XvtStore<gpstk::SatID>& ephemeris, gpstk::Xvt& svXvt) {
    try {
        CommonTime tt, transmit;
        Xvt svPosVel;     // Initialize to zero
        double rawrange;
        GPSEllipsoid ellipsoid;

        // 0-th order estimate of transmit time = receiver - pseudorange/c
        transmit = time;
        transmit -= pseudorange / C_MPS;
        tt = transmit;

        // correct for SV clock
        for (int i = 0; i < 2; i++) {
            // get SV position
            try {
                svPosVel = ephemeris.getXvt(satId, tt);
            } catch (InvalidRequest& e) {
                GPSTK_RETHROW(e);
            }
            tt = transmit;
            // remove clock bias and relativity correction
            tt -= (svPosVel.clkbias + svPosVel.relcorr);
        }

        svPosVel = rotateEarth(rxLoc, svPosVel, ellipsoid);

        // raw range
        rawrange = RSS(svPosVel.x[0] - rxLoc.X(),
                       svPosVel.x[1] - rxLoc.Y(),
                       svPosVel.x[2] - rxLoc.Z());

        svXvt = svPosVel;

        return rawrange;
    } catch (gpstk::Exception& e) {
        GPSTK_RETHROW(e);
    }
}

double RawRange3(double pseudorange, const gpstk::Position& rxLoc,
        const gpstk::SatID& satId, const gpstk::CommonTime& time,
        const gpstk::XvtStore<gpstk::SatID>& ephemeris, gpstk::Xvt& svXvt) {
    Position trx(rxLoc);
    trx.asECEF();

    Xvt svPosVel = ephemeris.getXvt(satId, time);

    // compute rotation angle in the time of signal transit

    // While this is quite similiar to rotateEarth, its not the same
    // and jcl doesn't know which is really correct
    // BWT this uses the measured pseudorange, corrected for SV clock and
    // relativity, to compute the time of flight; rotateEarth uses the value
    // computed from the receiver position and the ephemeris. They should be
    // very nearly the same, and multiplying by angVel/c should make the angle
    // of rotation very nearly identical.
    GPSEllipsoid ell;
    double range(pseudorange/ell.c() - svPosVel.clkbias - svPosVel.relcorr);
    double rotation_angle = -ell.angVelocity() * range;
    svPosVel.x[0] = svPosVel.x[0] - svPosVel.x[1] * rotation_angle;
    svPosVel.x[1] = svPosVel.x[1] + svPosVel.x[0] * rotation_angle;
    // svPosVel.x[2] = svPosVel.x[2];  // ?? Reassign for readability ??

    double rawrange = trx.slantRange(svPosVel.x);

    svXvt = svPosVel;
    return rawrange;
}

double RawRange4(const gpstk::Position& rxLoc, const gpstk::SatID& satId,
        const gpstk::CommonTime& time,
        const gpstk::XvtStore<gpstk::SatID>& ephemeris, gpstk::Xvt& svXvt) {
    try {
       gpstk::GPSEllipsoid gm;
       Xvt svPosVel = ephemeris.getXvt(satId, time);
       double pr = svPosVel.preciseRho(rxLoc, gm);
       return RawRange2(pr, rxLoc, satId, time, ephemeris, svXvt);
    }
    catch(gpstk::Exception& e) {
       GPSTK_RETHROW(e);
    }
}

double SvClockBiasCorrection(const gpstk::Xvt& svXvt) {
    double svclkbias = svXvt.clkbias * C_MPS;
    double svclkdrift = svXvt.clkdrift * C_MPS;
    return -svclkbias;
}

double SvRelativityCorrection(gpstk::Xvt& svXvt) {
    double relativity = svXvt.computeRelativityCorrection() * C_MPS;
    return -relativity;
}

double TroposphereCorrection(const gpstk::TropModel& tropModel,
        const gpstk::Position& rxLoc, const gpstk::Xvt& svXvt) {
    Position trx(rxLoc);
    Position svPos(svXvt);

    double elevation = trx.elevation(svPos);

    double trop = tropModel.correction(elevation);

    return trop;
}

}  // namespace ord
}  // namespace gpstk
