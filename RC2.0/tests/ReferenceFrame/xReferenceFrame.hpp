#pragma ident "$Id: xReferenceFrame.hpp 2011-06-24 14:11:10 nwu $"

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
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
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

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
