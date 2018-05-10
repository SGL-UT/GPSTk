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
#include "ord.hpp"

#include "OrdMockClasses.hpp"

using std::vector;

using gpstk::SatID;
using gpstk::CommonTime;
using gpstk::Xvt;
using gpstk::Position;
using gpstk::ord::IonosphereFreeRange;
using gpstk::ord::RawRange1;
using gpstk::ord::RawRange2;
using gpstk::ord::RawRange3;
using gpstk::ord::RawRange4;
using gpstk::ord::SvRelativityCorrection;
using gpstk::ord::TroposphereCorrection;
using gpstk::ord::IonosphereModelCorrection;

using ::testing::Return;
using ::testing::Invoke;
using ::testing::Throw;
using ::testing::_;

TEST(OrdTestCase, TestBasicIonosphereFreeRange) {
    static const double arr[] = {1.0, 2.0};

    std::vector<double> frequencies(arr, arr + sizeof(arr) / sizeof(arr[0]) );
    std::vector<double> pseudoranges(arr, arr + sizeof(arr) / sizeof(arr[0]) );

    double return_value = IonosphereFreeRange(frequencies, pseudoranges);

    ASSERT_GT(return_value, 0);
}

TEST(OrdTestCase, TestBasicIonosphereFreeRangeRequiresMoreThanOne) {
    static const double arr[] = {1.0};

    std::vector<double> frequencies(arr, arr + sizeof(arr) / sizeof(arr[0]) );
    std::vector<double> pseudoranges(arr, arr + sizeof(arr) / sizeof(arr[0]) );

    ASSERT_THROW(IonosphereFreeRange(frequencies, pseudoranges),
            gpstk::Exception);
}

TEST(OrdTestCase, TestBasicIonosphereFreeRangeRejectsHigherThanDual) {
    static const double arr[] = {1.0, 2.0, 3.0, 4.0, 5.0};

    std::vector<double> frequencies(arr, arr + sizeof(arr) / sizeof(arr[0]) );
    std::vector<double> pseudoranges(arr, arr + sizeof(arr) / sizeof(arr[0]) );

    ASSERT_THROW(IonosphereFreeRange(frequencies, pseudoranges),
            gpstk::Exception);
}


TEST(OrdTestCase, TestBasicIonosphereFreeRangeRejectsSizeMismatch) {
    static const double arr[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    static const double shortarr[] = {1.0, 2.0, 3.0, 4.0};

    std::vector<double> frequencies(shortarr,
            shortarr + sizeof(shortarr) / sizeof(shortarr[0]) );
    std::vector<double> pseudoranges(arr,
            arr + sizeof(arr) / sizeof(arr[0]) );

    ASSERT_THROW(IonosphereFreeRange(frequencies, pseudoranges),
            gpstk::Exception);
}


TEST(OrdTestCase, TestGetXvtFromStore) {
    MockXvtStore foo;
    SatID satId(10, SatID::systemUserDefined);
    CommonTime time(CommonTime::BEGINNING_OF_TIME);
    Xvt fakeXvt;

    EXPECT_CALL(foo, getXvt(satId, time)).WillOnce(Return(fakeXvt));

    Xvt resultXvt = gpstk::ord::getSvXvt(satId, time, foo);

    // This assertion is a proxy for verifying that the two Xvt instances are
    // the same.
    ASSERT_EQ(fakeXvt.clkbias, resultXvt.clkbias);
    ASSERT_EQ(resultXvt.x.theArray[0], fakeXvt.x.theArray[0]);
}

TEST(OrdTestCase, TestRawRange1) {
    MockXvtStore foo;
    Position rxLocation(10, 10, 0);
    gpstk::SatID satId(10, gpstk::SatID::systemUserDefined);
    gpstk::CommonTime time(gpstk::CommonTime::BEGINNING_OF_TIME);
    Xvt fakeXvt;
    fakeXvt.x = gpstk::Triple(100, 100, 100);
    fakeXvt.v = gpstk::Triple(0, 0, 0);
    Xvt returnedXvt;

    EXPECT_CALL(foo, getXvt(satId, _)).WillRepeatedly(Return(fakeXvt));

    double resultrange = RawRange1(rxLocation, satId, time, foo, returnedXvt);

    ASSERT_GT(resultrange, 0);
    // Can't really check returnedXvt, since it will have been rotated
    // by the earth.
    // ASSERT_EQ(returnedXvt.x.theArray[0], fakeXvt.x.theArray[0]);
}

TEST(OrdTestCase, TestRawRange1HandlesException) {
    MockXvtStore foo;
    Position rxLocation(10, 10, 0);
    SatID satId(10, gpstk::SatID::systemUserDefined);
    CommonTime time(gpstk::CommonTime::BEGINNING_OF_TIME);
    Xvt fakeXvt;
    fakeXvt.x = gpstk::Triple(100, 100, 100);
    fakeXvt.v = gpstk::Triple(0, 0, 0);
    Xvt returnedXvt;

    EXPECT_CALL(foo, getXvt(satId, _)).WillOnce(
            Throw(gpstk::InvalidRequest("Unsupported satellite system")));

    ASSERT_THROW(RawRange1(rxLocation, satId, time, foo, returnedXvt),
            gpstk::Exception);
}

TEST(OrdTestCase, TestRawRange2) {
    MockXvtStore foo;
    Position rxLocation(10, 10, 0);
    gpstk::SatID satId(10, gpstk::SatID::systemUserDefined);
    gpstk::CommonTime time(gpstk::CommonTime::BEGINNING_OF_TIME);
    Xvt fakeXvt;
    fakeXvt.x = gpstk::Triple(100, 100, 100);
    fakeXvt.v = gpstk::Triple(0, 0, 0);
    Xvt returnedXvt;

    EXPECT_CALL(foo, getXvt(satId, _)).WillRepeatedly(Return(fakeXvt));

    double range = RawRange2(0, rxLocation, satId, time, foo, returnedXvt);

    ASSERT_GT(range, 0);

    // Check to see that returnedXvt has been assigned _something_.
    ASSERT_GT(returnedXvt.x.theArray[0], 0);
}

TEST(OrdTestCase, TestRawRange2HandlesException) {
    MockXvtStore foo;
    Position rxLocation(10, 10, 0);
    SatID satId(10, gpstk::SatID::systemUserDefined);
    CommonTime time(gpstk::CommonTime::BEGINNING_OF_TIME);
    Xvt fakeXvt;
    fakeXvt.x = gpstk::Triple(100, 100, 100);
    fakeXvt.v = gpstk::Triple(0, 0, 0);
    Xvt returnedXvt;

    EXPECT_CALL(foo, getXvt(satId, _)).WillOnce(
            Throw(gpstk::InvalidRequest("Unsupported satellite system")));

    ASSERT_THROW(RawRange2(0, rxLocation, satId, time, foo, returnedXvt),
            gpstk::Exception);
}

TEST(OrdTestCase, TestRawRange3) {
    MockXvtStore foo;
    Position rxLocation(10, 10, 0);
    gpstk::SatID satId(10, gpstk::SatID::systemUserDefined);
    gpstk::CommonTime time(gpstk::CommonTime::BEGINNING_OF_TIME);
    Xvt fakeXvt;
    fakeXvt.x = gpstk::Triple(100, 100, 100);
    fakeXvt.v = gpstk::Triple(0, 0, 0);
    Xvt returnedXvt;

    EXPECT_CALL(foo, getXvt(satId, _)).WillRepeatedly(Return(fakeXvt));

    double range = RawRange3(0, rxLocation, satId, time, foo, returnedXvt);

    ASSERT_GT(range, 0);

    // Check to see that returnedXvt has been assigned _something_.
    ASSERT_GT(returnedXvt.x.theArray[0], 0);
}

TEST(OrdTestCase, TestRawRange3HandlesException) {
    MockXvtStore foo;
    Position rxLocation(10, 10, 0);
    SatID satId(10, gpstk::SatID::systemUserDefined);
    CommonTime time(gpstk::CommonTime::BEGINNING_OF_TIME);
    Xvt fakeXvt;
    fakeXvt.x = gpstk::Triple(100, 100, 100);
    fakeXvt.v = gpstk::Triple(0, 0, 0);
    Xvt returnedXvt;

    EXPECT_CALL(foo, getXvt(satId, _)).WillOnce(
            Throw(gpstk::InvalidRequest("Unsupported satellite system")));

    ASSERT_THROW(RawRange3(0, rxLocation, satId, time, foo, returnedXvt),
            gpstk::Exception);
}

TEST(OrdTestCase, TestRawRange4) {
    MockXvtStore foo;
    Position rxLocation(10, 10, 0);
    gpstk::SatID satId(10, gpstk::SatID::systemUserDefined);
    gpstk::CommonTime time(gpstk::CommonTime::BEGINNING_OF_TIME);
    Xvt fakeXvt;
    fakeXvt.x = gpstk::Triple(100, 100, 100);
    fakeXvt.v = gpstk::Triple(0, 0, 0);
    Xvt returnedXvt;

    EXPECT_CALL(foo, getXvt(satId, _)).WillRepeatedly(Return(fakeXvt));

    double resultrange = RawRange4(rxLocation, satId, time, foo, returnedXvt);

    ASSERT_GT(resultrange, 0);

    // Check to see that returnedXvt has been assigned _something_.
    ASSERT_GT(returnedXvt.x.theArray[0], 0);
}

TEST(OrdTestCase, TestRawRange4HandlesException) {
    MockXvtStore foo;
    Position rxLocation(10, 10, 0);
    SatID satId(10, gpstk::SatID::systemUserDefined);
    CommonTime time(gpstk::CommonTime::BEGINNING_OF_TIME);
    Xvt fakeXvt;
    fakeXvt.x = gpstk::Triple(100, 100, 100);
    fakeXvt.v = gpstk::Triple(0, 0, 0);
    Xvt returnedXvt;

    EXPECT_CALL(foo, getXvt(satId, _)).WillOnce(
            Throw(gpstk::InvalidRequest("Unsupported satellite system")));

    ASSERT_THROW(RawRange4(rxLocation, satId, time, foo, returnedXvt),
            gpstk::Exception);
}

TEST(OrdTestCase, TestSvRelativityCorrection) {
    MockXvt sv_xvt;

    EXPECT_CALL(sv_xvt, computeRelativityCorrection()).WillOnce(Return(5.6));

    double return_value = gpstk::ord::SvRelativityCorrection(sv_xvt);

    // Only verify that the number is less than -1e6
    // --- it's been multiplied by the speed of light.
    ASSERT_LT(return_value, -1e6);
}

TEST(OrdTestCase, TestTropoCorrection) {
    Xvt fakeXvt;
    fakeXvt.x = gpstk::Triple(100, 100, 100);
    fakeXvt.v = gpstk::Triple(0, 0, 0);
    MockTropo tropo;

    Position rxLocation(10, 10, 0);

    EXPECT_CALL(tropo, correction_wrap(_)).WillOnce(Return(42.0));

    double return_value = TroposphereCorrection(tropo, rxLocation, fakeXvt);

    ASSERT_EQ(return_value, 42.0);
}

TEST(OrdTestCase, TestIonoCorrection) {
    Xvt fakeXvt;
    fakeXvt.x = gpstk::Triple(100, 100, 100);
    fakeXvt.v = gpstk::Triple(0, 0, 0);
    CommonTime time(gpstk::CommonTime::BEGINNING_OF_TIME);
    MockIono iono;

    Position rxLocation(10, 10, 0);

    EXPECT_CALL(iono, getCorrection_wrap(_, _, _, _, _)).WillOnce(Return(42.0));

    double return_value = IonosphereModelCorrection(iono, time,
            gpstk::IonoModel::L1, rxLocation, fakeXvt);

    ASSERT_EQ(return_value, -42.0);
}


