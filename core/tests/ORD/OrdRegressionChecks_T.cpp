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

#include <iostream>
#include <string>

#include "gmock/gmock.h"

#include "Exception.hpp"
#include "XvtStore.hpp"
#include "CommonTime.hpp"
#include "Xvt.hpp"
#include "Triple.hpp"
#include "SatID.hpp"
#include "TimeSystem.hpp"
#include "EphemerisRange.hpp"
#include "ord.hpp"

#include "OrdMockClasses.hpp"

using gpstk::CorrectedEphemerisRange;
using gpstk::GAMMA_GPS;
using gpstk::L1_FREQ_GPS;
using gpstk::L2_FREQ_GPS;

using ::testing::Return;
using ::testing::Invoke;
using ::testing::_;

TEST(OrdTestRegression, TestIonoFreeRange) {
    std::vector<double> frequencies;
    frequencies.push_back(L1_FREQ_GPS);
    frequencies.push_back(L2_FREQ_GPS);
    std::vector<double> pseudoranges;
    pseudoranges.push_back(5000.0);
    pseudoranges.push_back(6000.0);

    for (int i = 0; i < pseudoranges.size(); i++) {
        std::cout << "PR[" << i << "] is: " << pseudoranges[i] << std::endl;
    }

    double range = gpstk::ord::IonosphereFreeRange(frequencies, pseudoranges);

    // Old calculation in ObsRngDev.cpp
    // for dual frequency see IS-GPS-200, section 20.3.3.3.3.3
    double icpr = (pseudoranges[1] - GAMMA_GPS *
                   pseudoranges[0])/(1-GAMMA_GPS);

    // Compare the new calculation to the old, for our contrived variables.
    double delta = fabs(range-icpr);
    std::cout << "difference of: " << delta << std::endl;
    // ASSERT_EQ(range, icpr);
    // TODO(someone) Is this an acceptable difference?
    ASSERT_LT(delta, 1e-5);
}

TEST(OrdTestRegression, TestRawRange1) {
    MockXvtStore foo;
    gpstk::Position rxLocation(10, 10, 0);
    gpstk::SatID satId(10, gpstk::SatID::systemUserDefined);
    gpstk::CommonTime time(gpstk::CommonTime::BEGINNING_OF_TIME);
    gpstk::Xvt fakeXvt;
    fakeXvt.x = gpstk::Triple(100, 100, 100);
    fakeXvt.v = gpstk::Triple(10, 0, 0);
    fakeXvt.clkbias = 10;
    fakeXvt.clkdrift = 10;
    fakeXvt.relcorr = 10;
    Xvt returnedXvt;

    EXPECT_CALL(foo, getXvt(satId, _)).WillRepeatedly(Return(fakeXvt));

    double resultRange = gpstk::ord::RawRange1(rxLocation, satId, time, foo,
            returnedXvt);
    resultRange += gpstk::ord::SvClockBiasCorrection(returnedXvt);
    resultRange += gpstk::ord::SvRelativityCorrection(returnedXvt);

    CorrectedEphemerisRange cer;

    double originalRange = cer.ComputeAtReceiveTime(time, rxLocation, satId,
            foo);

    // Compare the new calculation to the old, for our contrived variables.
    ASSERT_EQ(resultRange, originalRange);
}

TEST(OrdTestRegression, TestRawRange2) {
    MockXvtStore foo;
    gpstk::Position rxLocation(10, 10, 0);
    gpstk::SatID satId(10, gpstk::SatID::systemUserDefined);
    gpstk::CommonTime time(gpstk::CommonTime::BEGINNING_OF_TIME);
    gpstk::Xvt fakeXvt;
    fakeXvt.x = gpstk::Triple(100, 100, 100);
    fakeXvt.v = gpstk::Triple(10, 0, 0);
    fakeXvt.clkbias = 10;
    fakeXvt.clkdrift = 10;
    fakeXvt.relcorr = 10;
    Xvt returnedXvt;

    EXPECT_CALL(foo, getXvt(satId, _)).WillRepeatedly(Return(fakeXvt));

    double resultRange = gpstk::ord::RawRange2(0, rxLocation, satId, time, foo,
            returnedXvt);
    resultRange += gpstk::ord::SvClockBiasCorrection(returnedXvt);
    resultRange += gpstk::ord::SvRelativityCorrection(returnedXvt);

    CorrectedEphemerisRange cer;

    double originalRange = cer.ComputeAtTransmitTime(time, 0, rxLocation, satId,
            foo);

    // Compare the new calculation to the old, for our contrived variables.
    ASSERT_EQ(resultRange, originalRange);
}

TEST(OrdTestRegression, TestRawRange3) {
    MockXvtStore foo;
    gpstk::Position rxLocation(10, 10, 0);
    gpstk::SatID satId(10, gpstk::SatID::systemUserDefined);
    gpstk::CommonTime time(gpstk::CommonTime::BEGINNING_OF_TIME);
    gpstk::Xvt fakeXvt;
    fakeXvt.x = gpstk::Triple(100, 100, 100);
    fakeXvt.v = gpstk::Triple(10, 0, 0);
    fakeXvt.clkbias = 10;
    fakeXvt.clkdrift = 10;
    fakeXvt.relcorr = 10;
    Xvt returnedXvt;

    EXPECT_CALL(foo, getXvt(satId, _)).WillRepeatedly(Return(fakeXvt));

    double resultRange = gpstk::ord::RawRange3(0, rxLocation, satId, time, foo,
            returnedXvt);
    resultRange += gpstk::ord::SvClockBiasCorrection(returnedXvt);
    resultRange += gpstk::ord::SvRelativityCorrection(returnedXvt);

    CorrectedEphemerisRange cer;

    double originalRange = cer.ComputeAtTransmitSvTime(time, 0,
            rxLocation, satId, foo);

    // Compare the new calculation to the old, for our contrived variables.
    ASSERT_EQ(resultRange, originalRange);
}

TEST(OrdTestRegression, TestRawRange4) {
    MockXvtStore foo;
    gpstk::Position rxLocation(10, 10, 0);
    gpstk::SatID satId(10, gpstk::SatID::systemUserDefined);
    gpstk::CommonTime time(gpstk::CommonTime::BEGINNING_OF_TIME);
    gpstk::Xvt fakeXvt;
    fakeXvt.x = gpstk::Triple(100, 100, 100);
    fakeXvt.v = gpstk::Triple(10, 0, 0);
    fakeXvt.clkbias = 10;
    fakeXvt.clkdrift = 10;
    fakeXvt.relcorr = 10;
    Xvt returnedXvt;

    EXPECT_CALL(foo, getXvt(satId, _)).WillRepeatedly(Return(fakeXvt));

    double resultRange = gpstk::ord::RawRange4(rxLocation, satId, time, foo,
            returnedXvt);
    resultRange += gpstk::ord::SvClockBiasCorrection(returnedXvt);
    resultRange += gpstk::ord::SvRelativityCorrection(returnedXvt);

    CorrectedEphemerisRange cer;

    double originalRange = cer.ComputeAtTransmitTime(time, rxLocation, satId,
            foo);

    // Compare the new calculation to the old, for our contrived variables.
    ASSERT_EQ(resultRange, originalRange);
}
