
//============================================================================
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


#include "gmock/gmock.h"

#include <iostream>
#include <string>

#include "Exception.hpp"
#include "XvtStore.hpp"
#include "CommonTime.hpp"
#include "Xvt.hpp"
#include "Triple.hpp"
#include "SatID.hpp"
#include "TimeSystem.hpp"
#include "ord.hpp"

#include "OrdMockClasses.hpp"

using namespace gpstk;
using namespace std;

using ::testing::Return;
using ::testing::Invoke;
using ::testing::Throw;
using ::testing::_;


TEST(OrdTestCase, TestBasicIonosphereFreeRange)
{
    std::vector<double> frequencies;
    std::vector<double> pseudoranges;

    double return_value = gpstk::ord::IonosphereFreeRange(frequencies, pseudoranges);

    ASSERT_EQ(return_value, 0);
}


TEST (OrdTestCase, TestGetXvtFromStore)
{
    MockXvtStore foo;
    gpstk::SatID satId(10, gpstk::SatID::systemUserDefined);
    gpstk::CommonTime time(gpstk::CommonTime::BEGINNING_OF_TIME);
    gpstk::Xvt fakeXvt;

    EXPECT_CALL(foo, getXvt(satId, time)).WillOnce(Return(fakeXvt));

    gpstk::Xvt resultXvt = gpstk::ord::getSvXvt(satId, time, foo);

    // This assertion is a proxy for verifyig that the two Xvt instances are the same.
    ASSERT_EQ(fakeXvt.clkbias, resultXvt.clkbias);
}

TEST (OrdTestCase, TestRawRange1)
{
    MockXvtStore foo;
    gpstk::Position rxLocation(10, 10, 0);
    gpstk::SatID satId(10, gpstk::SatID::systemUserDefined);
    gpstk::CommonTime time(gpstk::CommonTime::BEGINNING_OF_TIME);
    gpstk::Xvt fakeXvt;
    fakeXvt.x = gpstk::Triple(100,100,100);
    fakeXvt.v = gpstk::Triple(0,0,0);

    EXPECT_CALL(foo, getXvt(satId, _)).WillRepeatedly(Return(fakeXvt));

    double resultrange = gpstk::ord::RawRange1(rxLocation, satId, time, foo, fakeXvt);

    // This assertion is a proxy for verifyig that the two Xvt instances are the same.
    ASSERT_EQ(resultrange, 0);
}

TEST (OrdTestCase, TestRawRange1HandlesException)
{
    MockXvtStore foo;
    gpstk::Position rxLocation(10, 10, 0);
    gpstk::SatID satId(10, gpstk::SatID::systemUserDefined);
    gpstk::CommonTime time(gpstk::CommonTime::BEGINNING_OF_TIME);
    gpstk::Xvt fakeXvt;
    fakeXvt.x = gpstk::Triple(100,100,100);
    fakeXvt.v = gpstk::Triple(0,0,0);

    EXPECT_CALL(foo, getXvt(satId, _)).WillOnce(Throw(gpstk::InvalidRequest("Unsupported satellite system")));

    ASSERT_THROW(gpstk::ord::RawRange1(rxLocation, satId, time, foo, fakeXvt), gpstk::Exception);
}


TEST(OrdTestCase, TestSvRelativityCorrection)
{
    MockXvt sv_xvt;

    EXPECT_CALL(sv_xvt, computeRelativityCorrection()).WillOnce(Return(5.6));

    double return_value = gpstk::ord::SvRelativityCorrection(sv_xvt);

    // Only verify that the number is creater than 1e6 --- it's been multiplied by the speed of light.
    ASSERT_GT(return_value, 1e6);
}


