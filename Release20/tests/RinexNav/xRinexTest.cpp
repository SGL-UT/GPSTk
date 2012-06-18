
#include "Rinex3NavData.cpp"
#include <iostream>

using namespace std;
using namespace gpstk;


int main(int argc, char* argv[])
{
      // Set time to Day 153, 2011 (6/2/2011) at noon
   CivilTime g( 2011, 6, 2, 12, 14, 44.0, TimeSystem::GPS );
   CommonTime dt = g.convertToCommonTime();

   short weeknum   = 1638;     // By rules of Kepler Orbit, this must be week of Toe
  
   // Test data from 06/02/2011

   long subframe1[10] = { 0x22C2663D, 0x1F0E29B8, 0x2664002B, 0x09FCC1B6, 0x0F60EB8A,
                          0x1299CE93, 0x29CD3DB6, 0x0597BB0F, 0x00000B68, 0x17B28E5C };
   long subframe2[10] = { 0x22C2663D, 0x1F0E4A28, 0x05809675, 0x0EBD8AF1, 0x00089344,
                          0x008081F8, 0x1330CC2C, 0x0461E855, 0x034F8045, 0x17BB1E68 };
   long subframe3[10] = { 0x22C2663D, 0x1F0E6BA0, 0x3FE129CD, 0x26E31837, 0x0006C96A,
                          0x35A74DFC, 0x065C8B0F, 0x1E4F400A, 0x3FE8966D, 0x05860C44 };
   

   //The dump from ee.dump() and ee_copy.dump() should be the same
   //    an EngEphemeris object is created, then used to create a Rinex3NavData
   EngEphemeris ee;
   ee.addSubframe(subframe1, weeknum, 3, 1);
   ee.addSubframe(subframe2, weeknum, 3, 1);
   ee.addSubframe(subframe3, weeknum, 3, 1);
   ee.dump();

   Rinex3NavData rnd = Rinex3NavData(ee); //constructor

   EngEphemeris ee_copy;
   ee_copy = EngEphemeris(rnd);           //cast
   ee_copy.dump();
}
