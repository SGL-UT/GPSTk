#pragma ident "$Id$"
// file: xBinUtils.cpp

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
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#include "xBinUtils.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (xBinUtils);


void xBinUtils :: setUp (void)
{ 
}

void xBinUtils :: firstTest (void)
{ 
	cout.setf(ios_base::hex,ios_base::basefield);
	
	unsigned char Store[12] = {0x11,0x33,0x77,0xFF,0xBB,0x22,0xDD,0x44,0x66,0xEE,0x99,0x66};
	
	char a;
	unsigned char *ptra = (unsigned char *)& a;
	for (int i = sizeof(char)-1;i>=0;i--)
	{
		*ptra = Store[i];
		*ptra++;
	}
	
	short b;
	unsigned char *ptrb = (unsigned char *)& b;
	for (int i = sizeof(short)-1;i>=0;i--)
	{
		*ptrb = Store[i];
		*ptrb++;
	}
	
	int c;
	unsigned char *ptrc = (unsigned char *)& c;
	for (int i = sizeof(int)-1;i>=0;i--)
	{
		*ptrc = Store[i];
		*ptrc++;
	}
	
	long d;
	unsigned char *ptrd = (unsigned char *)& d;
	for (int i = sizeof(long)-1;i>=0;i--)
	{
		*ptrd = Store[i];
		*ptrd++;
	}
	
	float e;
	unsigned char *ptre = (unsigned char *)& e;
	for (int i = sizeof(float)-1;i>=0;i--)
	{
		*ptre = Store[i];
		*ptre++;
	}
	
	double f;
	unsigned char *ptrf = (unsigned char *)& f;
	for (int i = sizeof(double)-1;i>=0;i--)
	{
		*ptrf = Store[i];
		*ptrf++;
	}
	
	long double g;
	unsigned char *ptrg = (unsigned char *)& g;
	for (int i = sizeof(long double)-1;i>=0;i--)
	{
		*ptrg = Store[i];
		*ptrg++;
	}

	char a1;
	unsigned char *ptra1 = (unsigned char *)& a1;
	for (int i =0;i<sizeof(char);i++)
	{
		*ptra1 = Store[i];
		*ptra1++;
	}
	
	short b1;
	unsigned char *ptrb1 = (unsigned char *)& b1;
	for (int i =0;i<sizeof(short);i++)
	{
		*ptrb1 = Store[i];
		*ptrb1++;
	}
	
	int c1;
	unsigned char *ptrc1 = (unsigned char *)& c1;
	for (int i =0;i<sizeof(int);i++)
	{
		*ptrc1 = Store[i];
		*ptrc1++;
	}
	
	long d1;
	unsigned char *ptrd1 = (unsigned char *)& d1;
	for (int i =0;i<sizeof(long);i++)
	{
		*ptrd1 = Store[i];
		*ptrd1++;
	}
	
	float e1;
	unsigned char *ptre1 = (unsigned char *)& e1;
	for (int i =0;i<sizeof(float);i++)
	{
		*ptre1 = Store[i];
		*ptre1++;
	}
	
	double f1;
	unsigned char *ptrf1 = (unsigned char *)& f1;
	for (int i =0;i<sizeof(double);i++)
	{
		*ptrf1 = Store[i];
		*ptrf1++;
	}
	
	long double g1;
	unsigned char *ptrg1 = (unsigned char *)& g1;
	for (int i =0;i<sizeof(long double);i++)
	{
		*ptrg1 = Store[i];
		*ptrg1++;
	}
	
	gpstk::BinUtils::twiddle(a);
	gpstk::BinUtils::twiddle(b);
	gpstk::BinUtils::twiddle(c);
	gpstk::BinUtils::twiddle(d);
	gpstk::BinUtils::twiddle(e);
	gpstk::BinUtils::twiddle(f);
	gpstk::BinUtils::twiddle(g);
	
	CPPUNIT_ASSERT_EQUAL(a1,a);
	CPPUNIT_ASSERT_EQUAL(b1,b);
	CPPUNIT_ASSERT_EQUAL(c1,c);
	CPPUNIT_ASSERT_EQUAL(d1,d);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(e1,e,1e-50);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(f1,f,1e-50);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(g1,g,1e-50);
	
}
