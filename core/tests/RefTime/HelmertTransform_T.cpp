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

#include "HelmertTransform_T.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (HelmertTransform_T);

using namespace gpstk;

void HelmertTransform_T::setUp()
{

}

void HelmertTransform_T::positionTransformTest()
{
   HelmertTransform helmert = HelmertTransform();

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
void HelmertTransform_T::xtTransformTest()
{
   HelmertTransform helmert = HelmertTransform();

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
void HelmertTransform_T::xvtTransformTest()
{
   HelmertTransform helmert = HelmertTransform();

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
void HelmertTransform_T::triplePosTransformTest()
{
   HelmertTransform helmert = HelmertTransform();

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
void HelmertTransform_T::tripleVelTransformTest()
{
   HelmertTransform helmert = HelmertTransform();

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
void HelmertTransform_T::vectorPosTransformTest()
{
   HelmertTransform helmert = HelmertTransform();

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
void HelmertTransform_T::vectorVelTransformTest()
{
   HelmertTransform helmert = HelmertTransform();

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
