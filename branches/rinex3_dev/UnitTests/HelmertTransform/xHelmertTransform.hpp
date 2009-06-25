#ifndef X_HELMERT_TRANSFORM_HPP
#define X_HELMERT_TRANSFORM_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "HelmertTransform.hpp"
#include "ReferenceFrame.hpp"
#include "Position.hpp"
#include "Xt.hpp"
#include "Xvt.hpp"
#include "Triple.hpp"
#include "Vector.hpp"

using namespace std;

class xHelmertTransform : public CPPUNIT_NS :: TestFixture
{
   CPPUNIT_TEST_SUITE (xHelmertTransform);
   CPPUNIT_TEST (instanceTest);
   CPPUNIT_TEST (getTransformTest);
   CPPUNIT_TEST (defineTransformTest);
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
   void instanceTest();
   void getTransformTest();
   void defineTransformTest();
   void positionTransformTest();
   void xtTransformTest();
   void xvtTransformTest();
   void triplePosTransformTest();
   void tripleVelTransformTest();
   void vectorPosTransformTest();
   void vectorVelTransformTest();
   
   private:
   
}

#endif
