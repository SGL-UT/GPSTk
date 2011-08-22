#pragma ident "$Id: $"

/*********************************************************************
*  $Id:$
*
*  Test program from July 2011.  Written to test the PackedNavBits 
*  module as it was being developed.
*
*********************************************************************/
#include <stdio.h>
#include <math.h>

#include "PackedNavBits.hpp"
#include "CivilTime.hpp"
#include "CommonTime.hpp"
#include "GPSWeekSecond.hpp"
#include "GNSSconstants.hpp"
#include "xPackedNavBitsgpsNavMsg.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (xPackedNavBitsgpsNavMsg);

using namespace std;
using namespace gpstk;

void xPackedNavBitsgpsNavMsg::setUp(void)
{
}

void xPackedNavBitsgpsNavMsg::firstTest(void)
{
      // Set time to Day 153, 2011 (6/2/2011) at noon
   CivilTime g( 2011, 6, 2, 12, 14, 44.0, TimeSystem::GPS );
   CommonTime TransmitTime = g.convertToCommonTime();

   SatID satSys(1, SatID::systemGPS);
   ObsID obsID( ObsID::otNavMsg, ObsID::cbL2, ObsID::tcC2LM );

      // Test Unsigned Integers
   unsigned long u_i1 = 32767;
   int u_n1           = 16;
   int u_s1           = 1;

   unsigned long u_i2 = 1;
   int u_n2           = 8;
   int u_s2           = 1;

   unsigned long u_i3 = 255;
   int u_n3           = 8;
   int u_s3           = 1;

   unsigned long u_i4 = 604500;
   int u_n4           = 11;
   int u_s4           = 300;

      // Test Signed Integers
   long s_i1 = 15;
   int s_n1  = 5;
   int s_s1  = 1;

   long s_i2 = -16;
   int s_n2  = 5;
   int s_s2  = 1;

   long s_i3 = -1;
   int s_n3  = 5;
   int s_s3  = 1;

   long s_i4 = 0;
   int s_n4  = 6;
   int s_s4  = 1;
   
   long s_i5 = 4194304;
   int s_n5  = 24;
   int s_s5  = 1;

      // Test Unsigned Doubles
   double d_i1 = 0.490005493;
   int d_n1    = 16;
   int d_s1    = -16;

   double d_i2 = -0.5;
   int d_n2    = 16;
   int d_s2    = -16;

   double d_i3 = 0;
   int d_n3    = 16;
   int d_s3    = -16;

      // Test Signed Doubles
   double d_i4 = 32000.0;
   int d_n4    = 16;
   int d_s4    = 0;

      // Test Semi-Circles
   double sd_i1 = PI-2*pow(2,-31);
   int sd_n1    = 32;
   int sd_s1    = -31;

   double sd_i2 = -PI;
   int sd_n2    = 32;
   int sd_s2    = -31;

      //Test Data copied from RINEX file for PRN3, week 1638, day 153 2011
   double rToe         = 388800.0;
   int n_rToe          = 16;
   int s_rToe          = 4;

   unsigned long riodc = 22;
   int n_riodc         = 8;
   int s_riodc         = 1;

   unsigned long riode = 22;
   int n_riode         = 8;
   int s_riode         = 1;

   unsigned long raodo = 10;
   int n_raodo         = 5;
   int s_raodo         = 1;

   unsigned long rfitInt = 0;
   int n_rfitInt    = 1;
   int s_rfitInt    = 1;

   double rToc      = 388800.0;
   int n_rToc       = 16;
   int s_rToc       = 4;

   double rCuc      = 9.57399606705E-07;
   int n_rCuc       = 16;
   int s_rCuc       = -29;

   double rCus      = 8.35768878460E-06;
   int n_rCus       = 16;
   int s_rCus       = -29;

   double rCrc      = 2.03562500000E+02;
   int n_rCrc       = 16;
   int s_rCrc       = -5;

   double rCrs      = 1.87812500000E+01;
   int n_rCrs       = 16;
   int s_rCrs       = -5;

   double rCic      = -2.30967998505E-07;
   int n_rCic       = 16;
   int s_rCic       = -29;

   double rCis      = 5.02914190292E-08;
   int n_rCis       = 16;
   int s_rCis       = -29;

   double rM0       = 1.05539162795E+00;
   int n_rM0        = 32;
   int s_rM0        = -31;

   double rdn       = 5.39093883996E-09;
   int n_rdn        = 16;
   int s_rdn        = -43;

   double recc      = 1.42575260252E-02;
   int n_recc       = 32;
   int s_recc       = -33;

   double rAhalf    = 5.15365527534E+03; 
   int n_rAhalf     = 32;
   int s_rAhalf     = -19;

   double rOMEGA0   = -2.16947563164E+00;
   int n_rOMEGA0    = 32;
   int s_rOMEGA0    = -31;

   double ri0       = 9.28692497530E-01;
   int n_ri0        = 32;
   int s_ri0        = -31;

   double rw        = 1.09154604931E+00;
   int n_rw         = 32;
   int s_rw         = -31;

   double rOMEGAdot = -8.56285667735E-09;
   int n_rOMEGAdot  = 24;
   int s_rOMEGAdot  = -43;

   double ridot     = 5.52880172536E-10;
   int n_ridot      = 14;
   int s_ridot       = -43;

   double raf0      = 7.23189674318E-04;
   int n_raf0       = 22;
   int s_raf0       = -31;

   double raf1      = 5.11590769747E-12;
   int n_raf1       = 16;
   int s_raf1       = -43;

   double raf2      = 0.0;
   int n_raf2       = 8;
   int s_raf2       = -55;

   double rTgd      = -4.65661287308E-09;
   int n_rTgd       = 8;
   int s_rTgd       = -31;

   ofstream outf("Logs/PackedNavBits_Output", ios::out);
   outf.precision(11);

      // First Test Case. Create PNB object.
   PackedNavBits pnb;
   
      /* Pack */
   pnb.setSatID(satSys);
   pnb.setObsID(obsID);
   pnb.setTime(TransmitTime);

/*   pnb.addUnsignedLong(u_i1, u_n1, u_s1);
   pnb.addUnsignedLong(u_i2, u_n2, u_s2);
   pnb.addUnsignedLong(u_i3, u_n3, u_s3);
   pnb.addUnsignedLong(u_i4, u_n4, u_s4);

   pnb.addLong(s_i1, s_n1, s_s1);
   pnb.addLong(s_i2, s_n2, s_s2);
   pnb.addLong(s_i3, s_n3, s_s3);
   pnb.addLong(s_i4, s_n4, s_s4);
   pnb.addLong(s_i5, s_n5, s_s5);


   pnb.addSignedDouble(d_i1, d_n1, d_s1);
   pnb.addSignedDouble(d_i2, d_n2, d_s2);
   pnb.addSignedDouble(d_i3, d_n3, d_s3);
   pnb.addUnsignedDouble(d_i4, d_n4, d_s4);

   pnb.addDoubleSemiCircles(sd_i1, sd_n1, sd_s1);
   pnb.addDoubleSemiCircles(sd_i2, sd_n2, sd_s2); */

   /* Unpack */
 /*  outf << endl;
   outf << "Unpacked Unsigned Integers:" << endl;
   int startbit = 0;
   outf << "Number 32767:  " << pnb.asUnsignedLong(startbit, u_n1, u_s1) << endl;
   startbit += u_n1;
   outf << "Number 1:      " << pnb.asUnsignedLong(startbit, u_n2, u_s2) << endl;
   startbit += u_n2;
   outf << "Number 255:    " << pnb.asUnsignedLong(startbit, u_n3, u_s3) << endl;
   startbit += u_n3;
   outf << "Number 604500: " << pnb.asUnsignedLong(startbit, u_n4, u_s4) << endl;
   startbit += u_n4;
   outf << endl;

   outf << "Unpacked Signed Integers: " << endl;
   outf << "Number 15:      " << pnb.asLong(startbit, s_n1, s_s1) << endl;
   startbit += s_n1;
   outf << "Number -16:     " << pnb.asLong(startbit, s_n2, s_s2) << endl;
   startbit += s_n2;
   outf << "Number -1:      " << pnb.asLong(startbit, s_n3, s_s3) << endl;
   startbit += s_n3;
   outf << "Number 0:       " << pnb.asLong(startbit, s_n4, s_s4) << endl;
   startbit += s_n4;
   outf << "Number 4194304: " << pnb.asLong(startbit, s_n5, s_s5) << endl;
   startbit += s_n5;
   outf << endl;

   outf << "Unpacked Signed Doubles: " << endl;
   outf << "Number 0.490005493: " << pnb.asSignedDouble(startbit, d_n1, d_s1) << endl;
   startbit += d_n1;
   outf << "Number -0.5:        " << pnb.asSignedDouble(startbit, d_n2, d_s2) << endl;
   startbit += d_n2;
   outf << "Number 0:           " << pnb.asSignedDouble(startbit, d_n3, d_s3) << endl;
   startbit += d_n3;
   outf << endl;

   outf << "Unpacked Unsigned Doubles: " << endl;
   outf << "Number 32000.0: " << pnb.asUnsignedDouble(startbit, d_n4, d_s4) << endl;
   startbit += d_n4;
   outf << endl;

   outf << "Unpacked Double Semi-Circles: " << endl;
   outf << "Number PI:  " << pnb.asDoubleSemiCircles(startbit, sd_n1, sd_s1) << endl;
   startbit += sd_n1;
   outf << "Number -PI: " << pnb.asDoubleSemiCircles(startbit, sd_n2, sd_s2) << endl; */

      // Pack legacy nav message data
   pnb.addSignedDouble(rTgd, n_rTgd, s_rTgd);
   pnb.addUnsignedLong(riodc, n_riodc, s_riodc);
   pnb.addUnsignedDouble(rToc, n_rToc, s_rToc);
   pnb.addSignedDouble(raf2, n_raf2, s_raf2);
   pnb.addSignedDouble(raf1, n_raf1, s_raf1);
   pnb.addSignedDouble(raf0, n_raf0, s_raf0);
   pnb.addUnsignedLong(riode, n_riode, s_riode);
   pnb.addSignedDouble(rCrs, n_rCrs, s_rCrs);
   pnb.addDoubleSemiCircles(rdn, n_rdn, s_rdn);
   pnb.addDoubleSemiCircles(rM0, n_rM0, s_rM0);
   pnb.addSignedDouble(rCuc, n_rCuc, s_rCuc);
   pnb.addUnsignedDouble(recc, n_recc, s_recc);
   pnb.addSignedDouble(rCus, n_rCus, s_rCus);
   pnb.addUnsignedDouble(rAhalf, n_rAhalf, s_rAhalf);
   pnb.addUnsignedDouble(rToe, n_rToe, s_rToe);
   pnb.addUnsignedLong(rfitInt, n_rfitInt, s_rfitInt);
   pnb.addUnsignedLong(raodo, n_raodo, s_raodo);
   pnb.addSignedDouble(rCic, n_rCic, s_rCic);
   pnb.addDoubleSemiCircles(rOMEGA0, n_rOMEGA0, s_rOMEGA0);
   pnb.addSignedDouble(rCis, n_rCis, s_rCis);
   pnb.addDoubleSemiCircles(ri0, n_ri0, s_ri0);
   pnb.addSignedDouble(rCrc, n_rCrc, s_rCrc);
   pnb.addDoubleSemiCircles(rw, n_rw, s_rw);
   pnb.addDoubleSemiCircles(rOMEGAdot, n_rOMEGAdot, s_rOMEGAdot);
   pnb.addUnsignedLong(riode, n_riode, s_riode);
   pnb.addDoubleSemiCircles(ridot, n_ridot, s_ridot); 
   
      // Unpack the legacy nav message data and get back the results in engineering terms
      // Test Data copied from RINEX file for PRN3, week 1638, day 153 2011
   int startbit = 0;
   outf << "Tgd:      " << pnb.asSignedDouble(startbit, n_rTgd, s_rTgd) << endl;
   startbit += n_rTgd;
   outf << "IODC:     " << pnb.asUnsignedLong(startbit, n_riodc, s_riodc) << endl;
   startbit += n_riodc;
   outf << "Toc:      " << pnb.asUnsignedDouble(startbit, n_rToc, s_rToc) << endl;
   startbit += n_rToc;
   outf << "af2:      " << pnb.asSignedDouble(startbit, n_raf2, s_raf2) << endl;
   startbit += n_raf2;
   outf << "af1:      " << pnb.asSignedDouble(startbit, n_raf1, s_raf1) << endl;
   startbit += n_raf1;
   outf << "af0:      " << pnb.asSignedDouble(startbit, n_raf0, s_raf0) << endl;
   startbit += n_raf0;
   outf << "IODE:     " << pnb.asUnsignedLong(startbit, n_riode, s_riode) << endl;
   startbit += n_riode;
   outf << "Crs:      " << pnb.asSignedDouble(startbit, n_rCrs, s_rCrs) << endl;
   startbit += n_rCrs;
   outf << "dn:       " << pnb.asDoubleSemiCircles(startbit, n_rdn, s_rdn) << endl;
   startbit += n_rdn;
   outf << "M0:       " << pnb.asDoubleSemiCircles(startbit, n_rM0, s_rM0) << endl;
   startbit += n_rM0;
   outf << "Cuc:      " << pnb.asSignedDouble(startbit, n_rCuc, s_rCuc) << endl;
   startbit += n_rCuc;
   outf << "ecc:      " << pnb.asUnsignedDouble(startbit, n_recc, s_recc) << endl;
   startbit += n_recc;
   outf << "Cus:      " << pnb.asSignedDouble(startbit, n_rCus, s_rCus) << endl;
   startbit += n_rCus;
   outf << "Ahalf:    " << pnb.asUnsignedDouble(startbit, n_rAhalf, s_rAhalf) << endl;
   startbit += n_rAhalf;
   outf << "Toe:      " << pnb.asUnsignedDouble(startbit, n_rToe, s_rToe) << endl;
   startbit += n_rToe;
   outf << "fitInt:   " << pnb.asUnsignedLong(startbit, n_rfitInt, s_rfitInt) << endl;
   startbit += n_rfitInt;
   outf << "AODO:     " << pnb.asUnsignedLong(startbit, n_raodo, s_raodo) << endl;
   startbit += n_raodo;
   outf << "Cic:      " << pnb.asSignedDouble(startbit, n_rCic, s_rCic) << endl;
   startbit += n_rCic;
   outf << "OMEGA0:   " << pnb.asDoubleSemiCircles(startbit, n_rOMEGA0, s_rOMEGA0) << endl;
   startbit += n_rOMEGA0;
   outf << "Cis:      " << pnb.asSignedDouble(startbit, n_rCis, s_rCis) << endl;
   startbit += n_rCis;
   outf << "i0:       " << pnb.asDoubleSemiCircles(startbit, n_ri0, s_ri0) << endl;
   startbit += n_ri0;
   outf << "Crc:      " << pnb.asSignedDouble(startbit, n_rCrc, s_rCrc) << endl;
   startbit += n_rCrc;
   outf << "w:        " << pnb.asDoubleSemiCircles(startbit, n_rw, s_rw) << endl;
   startbit += n_rw;
   outf << "OMEGAdot: " << pnb.asDoubleSemiCircles(startbit, n_rOMEGAdot, s_rOMEGAdot) << endl;
   startbit += n_rOMEGAdot;
   outf << "IODE:     " << pnb.asUnsignedLong(startbit, n_riode, s_riode) << endl;
   startbit += n_riode;
   outf << "idot:     " << pnb.asDoubleSemiCircles(startbit, n_ridot, s_ridot) << endl;

   outf << endl;
   outf << "Time of Transmission: " << pnb.getTransmitTime() << endl;
   outf << "Time of Transmission pnb: " << GPSWeekSecond(pnb.getTransmitTime()).printf("%F, %g") << endl;

      /* Resize the vector holding the packed nav message data. */
   pnb.trimsize();

   outf << endl;
   outf << "PNB object dump:" << endl;
   outf << pnb << endl;

   CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/PackedNavBits_Truth",(char*)"Logs/PackedNavBits_Output"));
}

bool xPackedNavBitsgpsNavMsg :: fileEqualTest (char* handle1, char* handle2)
{
	bool isEqual = false;
	
	ifstream File1;
	ifstream File2;
	
	std::string File1Line;
	std::string File2Line;
	
	File1.open(handle1);
	File2.open(handle2);
	
	while (!File1.eof())
	{
		if (File2.eof()) 
			return isEqual;
		getline (File1, File1Line);
		getline (File2, File2Line);

		if (File1Line != File2Line)
			return isEqual;
	}
	if (!File2.eof())
		return isEqual;
	else
		return isEqual = true;
}
