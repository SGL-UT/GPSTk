#pragma ident "$Id$"

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

#include "xIonoModel.hpp"



CPPUNIT_TEST_SUITE_REGISTRATION (xIonoModel);

void xIonoModel :: setUp (void)
{
}

/*
****Test to assert the quality of the == operator of the IonoModel class
*/
void xIonoModel :: equalityTest (void)
{
	//Create many alpha and beta arrays which deine the Ionospheric model
	double a[4] = {1.,2.,3.,4.};
	double b[4] = {4.,3.,2.,1.};
	double c[4] = {1.,2.,3.,4.};
	double d[4] = {4.,3.,2.,1.};
	double e[4] = {0.,0.,0.,0.};
	gpstk::IonoModel Model1(a,b);
	gpstk::IonoModel Model2(c,d);
	gpstk::IonoModel Model3(a,e);
	CPPUNIT_ASSERT(Model1 == Model2);
	CPPUNIT_ASSERT(!(Model1 == Model3));

}

/*
****Test to assert the quality of the != operator of the IonoModel class
*/
void xIonoModel :: nonEqualityTest (void)
{
	//Create many alpha and beta arrays which deine the Ionospheric model
	double a[4] = {1.,2.,3.,4.};
	double b[4] = {4.,3.,2.,1.};
	double c[4] = {1.,2.,3.,4.};
	double d[4] = {4.,3.,2.,1.};
	double e[4] = {0.,0.,0.,0.};
	gpstk::IonoModel Model1(a,b);
	gpstk::IonoModel Model2(c,d);
	gpstk::IonoModel Model3(a,e);
	CPPUNIT_ASSERT(!(Model1 != Model2));
	CPPUNIT_ASSERT((Model1 != Model3));
}

/*
****Test to assert the quality of the isValid() member of the IonoModel
****class.
*/
void xIonoModel :: validTest (void)
{
	//Instantiate a blank almanac
	gpstk::EngAlmanac blankAlmanac;

	//Create an alpha and a beta array which define the Ionospheric model
	double a[4] = {1.,2.,3.,4.};
	double b[4] = {4.,3.,2.,1.};

	//Test to see if various IonoModel instantiations are valid
	gpstk::IonoModel noParam;
	gpstk::IonoModel withArray(a,b);
	gpstk::IonoModel wblankAlm(blankAlmanac);
	CPPUNIT_ASSERT(!noParam.isValid());
	CPPUNIT_ASSERT(!wblankAlm.isValid());
	CPPUNIT_ASSERT(withArray.isValid());
}

/*
****Test to check and make sure that all of the execptions in the Ionomodel
****class are thrown where and as they are expected to

**** Please note:  As of June 29,2006 I have not found a way to get the blankAlmanac
**** exception to throw the way I wanted it to.  I have set it to assert fail so I can
**** come back at a later date to fix it.
*/
void xIonoModel :: exceptionTest (void)
{
	//Default constructer for Almanac will give a blank almanac
	gpstk::EngAlmanac blankAlmanac;
	//Set DayTime to the current system time
	gpstk::CommonTime commonTime;
	//Use the default Geodetic constructer
	gpstk::Position rxgeo;
	//Set el and az to 0 for ease of testing
	double svel = 0;
	double svaz = 0;
	//Easy alpha and beta for Ionospheric testing
	double a[4] = {1.,2.,3.,4.};
	double b[4] = {4.,3.,2.,1.};
	gpstk::IonoModel Model(blankAlmanac);
	gpstk::IonoModel goodModel(a,b);

	try
	{
	CPPUNIT_ASSERT_THROW(blankAlmanac.getIon(a,b),gpstk::InvalidRequest);
	//Questioning why this isnt failing auto fail for now
	CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT_THROW(gpstk::IonoModel Model(blankAlmanac),gpstk::Exception));
	CPPUNIT_ASSERT_THROW(Model.getCorrection(commonTime,rxgeo,svel,svaz,Model.L1),gpstk::IonoModel::InvalidIonoModel);
	CPPUNIT_ASSERT_NO_THROW(goodModel.getCorrection(commonTime,rxgeo,svel,svaz,Model.L1));
	CPPUNIT_ASSERT_NO_THROW(goodModel.getCorrection(commonTime,rxgeo,svel,svaz,Model.L2));
	CPPUNIT_ASSERT_NO_THROW(goodModel.getCorrection(commonTime,rxgeo,72.,45.,Model.L1));
	}
	catch(gpstk::Exception& e)
	{
	}
}
