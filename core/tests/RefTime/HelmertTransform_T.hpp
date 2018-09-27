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

#ifndef X_HELMERT_TRANSFORM_HPP
#define X_HELMERT_TRANSFORM_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "HelmertTransform.hpp"
#include "ReferenceFrame.hpp"
#include "Position.hpp"
#include "Xvt.hpp"
#include "Triple.hpp"
#include "Vector.hpp"


class HelmertTransform_T : public CPPUNIT_NS :: TestFixture
{
   CPPUNIT_TEST_SUITE (HelmertTransform_T);
   CPPUNIT_TEST (positionTransformTest);
   CPPUNIT_TEST (xtTransformTest);
   CPPUNIT_TEST (xvtTransformTest);
   CPPUNIT_TEST (triplePosTransformTest);
   CPPUNIT_TEST (tripleVelTransformTest);
   CPPUNIT_TEST (vectorPosTransformTest);
   CPPUNIT_TEST (vectorVelTransformTest);
   CPPUNIT_TEST_SUITE_END ();

   public:
   void setUp();
   void tearDown() {   };

   protected:
   void positionTransformTest();
   void xtTransformTest();
   void xvtTransformTest();
   void triplePosTransformTest();
   void tripleVelTransformTest();
   void vectorPosTransformTest();
   void vectorVelTransformTest();

   private:

};

#endif
