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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::Return;

TEST(TestCase, TestName) {
    ASSERT_EQ(4, 2 + 2);
}

class ORDTest: public testing::Test {
 protected:
    virtual void SetUp();
    virtual void TearDown();
};

/**
 *  Sets up a test image for the sizing tests to use.
 */
void ORDTest::SetUp() {
}
/**
 *  Clean up the allocated test image.
 */
void ORDTest::TearDown() {
}

/**
 *
 Tests image crop operation.
 */
TEST_F(ORDTest, testCrop) {
    const int kCropTop = 10;
    const int kCropLeft = 10;
    const int kCropWidth = 320;
    const int kCropHeight = 240;

    ASSERT_LE(kCropTop + kCropHeight, kCropLeft + kCropWidth);
}
/**
 *
 Tests image resize operation.
 */
TEST_F(ORDTest, testImageResize) {
    ASSERT_GE(10, 0);
}

// ----  Test code for Google Mocks Troubleshooting ---
class A {
 public:
    A() {
    }

    virtual ~A() {
    }

    virtual int foo() {
        return 5;
    }
};

class MockA: public A {
 public:
    MOCK_METHOD0(foo, int());
};

int foo_wrapper(A& thefoo) {
    return thefoo.foo();
}

TEST(MockExplorer, TestExpectedMethodCall) {
    MockA myfoo;

    EXPECT_CALL(myfoo, foo()).WillOnce(Return(3));

    int return_value = foo_wrapper(myfoo);

    ASSERT_EQ(return_value, 3);
}
