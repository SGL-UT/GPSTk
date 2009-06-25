#include "xHelmertTransform.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (xHelmertTransform);


void xHelmertTransform::setUp()
{
   
}

void xHelmertTransform::instanceTest()
{
   HelmertTransform& helmert = HelmertTransform::instance();
   HelmertTransform& hlemTwo = HelmertTransform::instance();
   
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
   
   Transform t1, t2, t3, t4, t5, t6, t7, t8, t9;
   CPPUNIT_ASSERT_NO_THROW(Transform t1 = helmert.getTransform(rf1, rf2));
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
   
   TransformParameters tp;
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
   
   CPPUNIT_ASSERT_THROW( Transform t1 = helmert.getTransform(initial, final), InvalidParameter );
   
      //Note: Should probably change the prototype to follow the get method's convention...
   CPPUNIT_ASSERT_NO_THROW( helmert.defineTransform( tp, final, initial ) );
   
   CPPUNIT_ASSERT_NO_THROW( Transform t2 = helmert.getTransform( initial, final ) );
   
}
void xHelmertTransform::positionTransformTest()
{
   HelmertTransform& helmert = HelmertTransform::instance();
   
      //Use the previously defined transform
   ReferenceFrame initial("Initial");
   ReferenceFrame final("Final");
   
   Position pos(150, 150, 150);
      //100% scale, 10m translation
   Position comp(310, 310, 310);
   Position pos2 = pos;
   
   pos = helmert.transform(initial, final, pos);
   
   CPPUNIT_ASSERT( pos.getX() == comp.getX() );
   CPPUNIT_ASSERT( pos.getY() == comp.getY() );
   CPPUNIT_ASSERT( pos.getZ() == comp.getZ() );
   
   pos = helmert.transform(final, initial, pos);
   
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
   pos.x(150, 150, 150);
   comp.x(310, 310, 310);
   pos2.x(150, 150, 150);
   
   pos = helmert.transform(initial, final, pos);
   
   CPPUNIT_ASSERT( pos.x[0] == comp.x[0] );
   CPPUNIT_ASSERT( pos.x[1] == comp.x[1] );
   CPPUNIT_ASSERT( pos.x[2] == comp.x[2] );
   
   pos = helmert.transform(final, initial, pos);
   
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
   pos.x(150, 150, 150);
   pos.v(150, 150, 150);
   comp.x(310, 310, 310);
   comp.v(300, 300, 300);
   pos2.x(150, 150, 150);
   pos2.v(150, 150, 150);
   
   pos = helmert.transform(initial, final, pos);
   
   CPPUNIT_ASSERT( pos.x[0] == comp.x[0] );
   CPPUNIT_ASSERT( pos.x[1] == comp.x[1] );
   CPPUNIT_ASSERT( pos.x[2] == comp.x[2] );
   CPPUNIT_ASSERT( pos.v[0] == comp.v[0] );
   CPPUNIT_ASSERT( pos.v[1] == comp.v[1] );
   CPPUNIT_ASSERT( pos.v[2] == comp.v[2] );
   
   pos = helmert.transform(final, initial, pos);
   
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
   
   Triple pos(150, 150, 150);
   Triple comp(310, 310, 310);
   Triple pos2(150, 150, 150);
   
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
   
   Triple vel(150, 150, 150);
   Triple comp(300, 300, 300);
   Triple vel2(150, 150, 150);
   
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
   
   Vector pos(3,0.0);
   Vector comp(3,0.0);
   Vector pos2(3,0.0);
   
   pos(0) = 150;
   pos(1) = 150;
   pos(2) = 150;
   
   com(0) = 310;
   com(1) = 310;
   com(2) = 310;
   
   pos2(0) = 150;
   pos2(1) = 150;
   pos2(2) = 150;
   
   pos = helmert.posTransform(initial, final, pos);
   
   CPPUNIT_ASSERT(pos(0) == comp(0));
   CPPUNIT_ASSERT(pos(1) == comp(1));
   CPPUNIT_ASSERT(pos(2) == comp(2));
   
   pos = helmert.posTransform(final, intitial, pos);
   
   CPPUNIT_ASSERT(pos(0) == pos2(0));
   CPPUNIT_ASSERT(pos(1) == pos2(1));
   CPPUNIT_ASSERT(pos(2) == pos2(2));
}
void xHelmertTransform::vectorVelTransformTest()
{
   HelmertTransform& helmert = HelmertTransform::instance();
   
   ReferenceFrame initial("Initial");
   ReferenceFrame final("Final");
   
   Vector vel(3,0.0);
   Vector comp(3,0.0);
   Vector vel2(3,0.0);
   
   vel(0) = 150;
   vel(1) = 150;
   vel(2) = 150;
   
   com(0) = 300;
   com(1) = 300;
   com(2) = 300;
   
   vel2(0) = 150;
   vel2(1) = 150;
   vel2(2) = 150;
   
   vel = helmert.velTransform(initial, final, vel);
   
   CPPUNIT_ASSERT(vel(0) == comp(0));
   CPPUNIT_ASSERT(vel(1) == comp(1));
   CPPUNIT_ASSERT(vel(2) == comp(2));
   
   vel = helmert.velTransform(final, intitial, vel);
   
   CPPUNIT_ASSERT(vel(0) == vel2(0));
   CPPUNIT_ASSERT(vel(1) == vel2(1));
   CPPUNIT_ASSERT(vel(2) == vel2(2));
}
