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

using ::testing::Return;
using ::testing::Invoke;
using ::testing::_;

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

    EXPECT_CALL(foo, getXvt(satId, _)).WillRepeatedly(Return(fakeXvt));

    double resultRange = gpstk::ord::RawRange1(rxLocation, satId, time, foo,
            fakeXvt);
    resultRange += gpstk::ord::SvClockBiasCorrection(fakeXvt);
    resultRange += gpstk::ord::SvRelativityCorrection(fakeXvt);

    CorrectedEphemerisRange cer;

    double originalRange = cer.ComputeAtReceiveTime(time, rxLocation, satId,
            foo);

    // Compare the new calculation to the old, for our contrived variables.
    ASSERT_EQ(resultRange, originalRange);
}
