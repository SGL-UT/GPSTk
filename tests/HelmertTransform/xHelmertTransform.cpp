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

#include "xHelmertTransform.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (xHelmertTransform);

using namespace gpstk;

void xHelmertTransform::setUp()
{

}

void xHelmertTransform::instanceTest()
{
   HelmertTransform& helmert = HelmertTransform::instance();
   HelmertTransform& helmTwo = HelmertTransform::instance();

   CPPUNIT_ASSERT_EQUAL(&helmert, &helmTwo);
}
void xHelmertTransform::getTransformTest()
{
   HelmertTransform& helmert = HelmertTransform::instance();

   ReferenceFrame rf0(ReferenceFrame::Unknown);
   ReferenceFrame rf1(ReferenceFrame::PZ90);
   ReferenceFrame rf2(ReferenceFrame::WGS84);
   ReferenceFrame rf3;
   rf3 = rf3.createReferenceFrame("Hello World!");

   HelmertTransform::Transform t1, t2, t3, t4, t5, t6, t7, t8, t9;
   CPPUNIT_ASSERT_NO_THROW(t1 = helmert.getTransform(rf1, rf2));
   CPPUNIT_ASSERT_THROW( t2 = helmert.getTransform(rf2, rf1), InvalidParameter );
   CPPUNIT_ASSERT_THROW( t3 = helmert.getTransform(rf3, rf1), InvalidParameter );
   CPPUNIT_ASSERT_THROW( t4 = helmert.getTransform(rf2, rf3), InvalidParameter );
   CPPUNIT_ASSERT_THROW( t5 = helmert.getTransform(rf1, rf1), InvalidParameter );
   CPPUNIT_ASSERT_THROW( t6 = helmert.getTransform(rf0, rf1), InvalidParameter );
   CPPUNIT_ASSERT_THROW( t7 = helmert.getTransform(rf2, rf0), InvalidParameter );
   CPPUNIT_ASSERT_THROW( t8 = helmert.getTransform(rf3, rf0), InvalidParameter );
   CPPUNIT_ASSERT_THROW( t9 = helmert.getTransform(rf0, rf0), InvalidParameter );

}
void xHelmertTransform::defineTransformTest()
{
   HelmertTransform& helmert = HelmertTransform::instance();

   ReferenceFrame initial, final;
   initial = initial.createReferenceFrame("Initial");
   final = final.createReferenceFrame("Final");

   HelmertTransform::TransformParameters tp;
         //100% scale
      tp.scale = 1;
         //No rot
      tp.r1 = 0;
      tp.r2 = 0;
      tp.r3 = 0;
         //Translation of 10, all axis
      tp.t1 = 10;
      tp.t2 = 10;
      tp.t3 = 10;

   CPPUNIT_ASSERT_THROW( HelmertTransform::Transform t1 = helmert.getTransform(initial, final), InvalidParameter );

      //Note: Should probably change the prototype to follow the get method's convention...
   CPPUNIT_ASSERT_NO_THROW( helmert.defineTransform( tp, final, initial ) );

   CPPUNIT_ASSERT_NO_THROW( HelmertTransform::Transform t2 = helmert.getTransform( initial, final ) );

}
void xHelmertTransform::positionTransformTest()
{
   HelmertTransform& helmert = HelmertTransform::instance();

      //Use the previously defined transform
   ReferenceFrame initial("Initial");
   ReferenceFrame final("Final");

   Position pos(150, 150, 150, Position::Cartesian, NULL, initial);
      //100% scale, 10m translation
   Position comp(310, 310, 310, Position::Cartesian, NULL, final);
   Position pos2 = pos;

   pos = helmert.transform(final, pos);

   CPPUNIT_ASSERT( pos.getX() == comp.getX() );
   CPPUNIT_ASSERT( pos.getY() == comp.getY() );
   CPPUNIT_ASSERT( pos.getZ() == comp.getZ() );

   pos = helmert.transform(initial, pos);

   CPPUNIT_ASSERT( pos.getX() == pos2.getX() );
   CPPUNIT_ASSERT( pos.getY() == pos2.getY() );
   CPPUNIT_ASSERT( pos.getZ() == pos2.getZ() );
}
void xHelmertTransform::xtTransformTest()
{
   HelmertTransform& helmert = HelmertTransform::instance();

   ReferenceFrame initial("Initial");
   ReferenceFrame final("Final");

   Xt pos, comp, pos2;
   pos.x = Triple(150, 150, 150);
      pos.frame = initial;
   comp.x = Triple(310, 310, 310);
      comp.frame = final;
   pos2.x = Triple(150, 150, 150);
      pos2.frame = initial;

   pos = helmert.transform(final, pos);

   CPPUNIT_ASSERT( pos.x[0] == comp.x[0] );
   CPPUNIT_ASSERT( pos.x[1] == comp.x[1] );
   CPPUNIT_ASSERT( pos.x[2] == comp.x[2] );

   pos = helmert.transform(initial, pos);

   CPPUNIT_ASSERT( pos.x[0] == pos2.x[0] );
   CPPUNIT_ASSERT( pos.x[1] == pos2.x[1] );
   CPPUNIT_ASSERT( pos.x[2] == pos2.x[2] );
}
void xHelmertTransform::xvtTransformTest()
{
   HelmertTransform& helmert = HelmertTransform::instance();

   ReferenceFrame initial("Initial");
   ReferenceFrame final("Final");

   Xvt pos, comp, pos2;
   pos.x = Triple(150, 150, 150);
   pos.v = Triple(150, 150, 150);
      pos.frame = initial;
   comp.x = Triple(310, 310, 310);
   comp.v = Triple(300, 300, 300);
      comp.frame = final;
   pos2.x = Triple(150, 150, 150);
   pos2.v = Triple(150, 150, 150);
      pos2.frame = initial;

   pos = helmert.transform(final, pos);

   CPPUNIT_ASSERT( pos.x[0] == comp.x[0] );
   CPPUNIT_ASSERT( pos.x[1] == comp.x[1] );
   CPPUNIT_ASSERT( pos.x[2] == comp.x[2] );
   CPPUNIT_ASSERT( pos.v[0] == comp.v[0] );
   CPPUNIT_ASSERT( pos.v[1] == comp.v[1] );
   CPPUNIT_ASSERT( pos.v[2] == comp.v[2] );

   pos = helmert.transform(initial, pos);

   CPPUNIT_ASSERT( pos.x[0] == pos2.x[0] );
   CPPUNIT_ASSERT( pos.x[1] == pos2.x[1] );
   CPPUNIT_ASSERT( pos.x[2] == pos2.x[2] );
   CPPUNIT_ASSERT( pos.v[0] == pos2.v[0] );
   CPPUNIT_ASSERT( pos.v[1] == pos2.v[1] );
   CPPUNIT_ASSERT( pos.v[2] == pos2.v[2] );
}
void xHelmertTransform::triplePosTransformTest()
{
   HelmertTransform& helmert = HelmertTransform::instance();

   ReferenceFrame initial("Initial");
   ReferenceFrame final("Final");

   Triple pos = Triple(150, 150, 150);
   Triple comp = Triple(310, 310, 310);
   Triple pos2 = Triple(150, 150, 150);

   pos = helmert.posTransform(initial, final, pos);

   CPPUNIT_ASSERT( pos[0] == comp[0] );
   CPPUNIT_ASSERT( pos[1] == comp[1] );
   CPPUNIT_ASSERT( pos[2] == comp[2] );

   pos = helmert.posTransform(final, initial, pos);

   CPPUNIT_ASSERT( pos[0] == pos2[0] );
   CPPUNIT_ASSERT( pos[1] == pos2[1] );
   CPPUNIT_ASSERT( pos[2] == pos2[2] );
}
void xHelmertTransform::tripleVelTransformTest()
{
   HelmertTransform& helmert = HelmertTransform::instance();

   ReferenceFrame initial("Initial");
   ReferenceFrame final("Final");

   Triple vel = Triple(150, 150, 150);
   Triple comp = Triple(300, 300, 300);
   Triple vel2 = Triple(150, 150, 150);

   vel = helmert.velTransform(initial, final, vel);

   CPPUNIT_ASSERT( vel[0] == comp[0] );
   CPPUNIT_ASSERT( vel[1] == comp[1] );
   CPPUNIT_ASSERT( vel[2] == comp[2] );

   vel = helmert.velTransform(final, initial, vel);

   CPPUNIT_ASSERT( vel[0] == vel2[0] );
   CPPUNIT_ASSERT( vel[1] == vel2[1] );
   CPPUNIT_ASSERT( vel[2] == vel2[2] );
}
void xHelmertTransform::vectorPosTransformTest()
{
   HelmertTransform& helmert = HelmertTransform::instance();

   ReferenceFrame initial("Initial");
   ReferenceFrame final("Final");

   Vector<double> pos(3,0.0);
   Vector<double> comp(3,0.0);
   Vector<double> pos2(3,0.0);

   pos(0) = 150;
   pos(1) = 150;
   pos(2) = 150;

   comp(0) = 310;
   comp(1) = 310;
   comp(2) = 310;

   pos2(0) = 150;
   pos2(1) = 150;
   pos2(2) = 150;

   pos = helmert.posTransform(initial, final, pos);

   CPPUNIT_ASSERT(pos(0) == comp(0));
   CPPUNIT_ASSERT(pos(1) == comp(1));
   CPPUNIT_ASSERT(pos(2) == comp(2));

   pos = helmert.posTransform(final, initial, pos);

   CPPUNIT_ASSERT(pos(0) == pos2(0));
   CPPUNIT_ASSERT(pos(1) == pos2(1));
   CPPUNIT_ASSERT(pos(2) == pos2(2));
}
void xHelmertTransform::vectorVelTransformTest()
{
   HelmertTransform& helmert = HelmertTransform::instance();

   ReferenceFrame initial("Initial");
   ReferenceFrame final("Final");

   Vector<double> vel(3,0.0);
   Vector<double> comp(3,0.0);
   Vector<double> vel2(3,0.0);

   vel(0) = 150;
   vel(1) = 150;
   vel(2) = 150;

   comp(0) = 300;
   comp(1) = 300;
   comp(2) = 300;

   vel2(0) = 150;
   vel2(1) = 150;
   vel2(2) = 150;

   vel = helmert.velTransform(initial, final, vel);

   CPPUNIT_ASSERT(vel(0) == comp(0));
   CPPUNIT_ASSERT(vel(1) == comp(1));
   CPPUNIT_ASSERT(vel(2) == comp(2));

   vel = helmert.velTransform(final, initial, vel);

   CPPUNIT_ASSERT(vel(0) == vel2(0));
   CPPUNIT_ASSERT(vel(1) == vel2(1));
   CPPUNIT_ASSERT(vel(2) == vel2(2));
}
