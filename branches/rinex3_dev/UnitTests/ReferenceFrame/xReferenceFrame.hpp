#ifndef X_REFERENCE_FRAME_HPP
#define X_REFERENCE_FRAME_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "ReferenceFrame.hpp"
namespace gpstk{
   class xReferenceFrame : public CPPUNIT_NS :: TestFixture
   {
      CPPUNIT_TEST_SUITE (xReferenceFrame);
      CPPUNIT_TEST (getFrameTest);
      CPPUNIT_TEST (asStringTest);
      CPPUNIT_TEST (equalityTest);
      CPPUNIT_TEST (inequalityTest);
      CPPUNIT_TEST (greaterThanTest);
      CPPUNIT_TEST (greaterThanOrEqualToTest);
      CPPUNIT_TEST (lessThanTest);
      CPPUNIT_TEST (lesserThanOrEqualToTest);
      CPPUNIT_TEST (setReferenceFrameTest);
      CPPUNIT_TEST (createReferenceFrameTest);
      CPPUNIT_TEST_SUITE_END ();
      
      public:
      void setUp(void) {   };
      void tearDown(void) {   };
      
      protected:
      void createReferenceFrameTest();
      void getFrameTest();
      void asStringTest();
      void equalityTest();
      void inequalityTest();
      void greaterThanTest();
      void lessThanTest();
      void greaterThanOrEqualToTest();
      void lesserThanOrEqualToTest();
      void setReferenceFrameTest();
      
   };
}
#endif
