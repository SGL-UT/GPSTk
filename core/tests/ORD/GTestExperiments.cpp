//
//  Copyright 2004, The University of Texas at Austin

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
