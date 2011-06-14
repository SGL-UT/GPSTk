#pragma ident "$Id:$"



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






/**
 * @file BrcClockCorrection.cpp
 * Ephemeris data encapsulated in engineering terms
 */

#include "StringUtils.hpp"
#include "icd_gps_constants.hpp"
#include "GPSGeoid.hpp"
#include "BrcClockCorrection.hpp"

#include <cmath>

namespace gpstk
{
   using namespace std;
   using namespace gpstk;

   BrcClockCorrection::BrcClockCorrection()
      throw()
   {
      dataLoaded = false;

      PRNID = weeknum = 0;

      SatSystemID = ' ';

      healthy = false;     
 
      Toc = af0 = af1 = af2 = accuracy = 0.0;
   }

   BrcClockCorrection::BrcClockCorrection(const char SatSystemIDArg, const ObsID obsIDArg, const short PRNIDArg,
		  const double TocArg, const short weeknumArg,
	     const double accuracyArg, const bool healthyArg,
		  const double af0Arg, const double af1Arg,
		  const double af2Arg )
   {
      loadData(SatSystemIDArg, obsIDArg, PRNIDArg,
		  TocArg, weeknumArg,
	     accuracyArg, healthyArg,
		  af0Arg, af1Arg, af2Arg );

   }

		/// Legacy GPS Subframe 1-3  
   BrcClockCorrection::BrcClockCorrection(const ObsID obsIDArg, const short PRNID, const short fullweeknum,
		      const long subframe1[10] )
   {
     loadData(obsIDArg, PRNID,fullweeknum,
		      subframe1 );
   }

   void BrcClockCorrection::loadData(const char SatSystemIDArg, const ObsID obsIDArg, const short PRNIDArg,
		  const double TocArg, const short weeknumArg,
	     const double accuracyArg, const bool healthyArg,
		  const double af0Arg, const double af1Arg,
		  const double af2Arg )
   {
	SatSystemID = SatSystemIDArg;
	obsID       = obsIDArg;
	PRNID       = PRNIDArg;
	Toc         = TocArg;
	weeknum     = weeknumArg;
	accuracy    = accuracyArg;
	healthy     = healthyArg;
	af0         = af0Arg;
	af1         = af1Arg;
	af2         = af2Arg;
	dataLoaded  = true;
   }

   void BrcClockCorrection::loadData(const ObsID obsIDArg, const short PRNIDArg, const short fullweeknum,
		const long subframe1[10] )
		throw(InvalidParameter)
    {
         double ficked[60];

 	    //Load overhead members
  	 SatSystemID = 'G';
	 obsID = obsIDArg;
	 PRNID = PRNIDArg;

	    //Convert Subframe 1
	if (!subframeConvert(subframe1, fullweeknum, ficked))
	{
	   InvalidParameter exc("Subframe 1 not valid.");
	   GPSTK_THROW(exc);
	}

	     weeknum       = static_cast<short>( ficked[5] );
	     short accFlag = static_cast<short>( ficked[7] );
	     short health  = static_cast<short>( ficked[8] );
        Toc           = ficked[12];
        af2           = ficked[13];
        af1           = ficked[14];
        af0           = ficked[15];
	     //Convert the accuracy flag to a value...
	     accuracy = gpstk::ura2accuracy(accFlag);
	     healthy = false;
	     if (health == 0)
	     healthy = true;
	     dataLoaded = true;
	 
	return;
    }
	     
   bool BrcClockCorrection :: hasData() const
   {
      return(dataLoaded);
   }

   DayTime BrcClockCorrection::getEpochTime() const
      throw(InvalidRequest)
   {
      DayTime toReturn(0.L);
         toReturn.setGPSfullweek(getFullWeek(), getToc());
      return toReturn;
   }

   double BrcClockCorrection::svClockBias(const DayTime& t) const
      throw(gpstk::InvalidRequest)
   {
      double dtc,elaptc;
      elaptc = t - getEpochTime();
      dtc = getAf0() + elaptc * ( getAf1() + elaptc * getAf2() );

      return dtc;
   }

   double BrcClockCorrection::svClockBiasM(const DayTime& t) const
      throw(gpstk::InvalidRequest)
   {
      double ret = svClockBias(t);
      ret = ret*C_GPS_M;
      return (ret);
   }

   double BrcClockCorrection::svClockDrift(const DayTime& t) const
      throw(gpstk::InvalidRequest)
   {
      double drift,elaptc;
      elaptc = t - getEpochTime();
      drift = getAf1() + elaptc * getAf2();
      return drift;
   }

   short BrcClockCorrection::getPRNID() const
      throw(InvalidRequest)
   {
      if(!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return PRNID;
   }
     
   short BrcClockCorrection::getFullWeek()  const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return weeknum;
   }
     
   double BrcClockCorrection::getAccuracy()  const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return accuracy;
   }   

   double BrcClockCorrection::getToc() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return Toc;
   }

   double BrcClockCorrection::getAf0() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return af0;
   }

   double BrcClockCorrection::getAf1() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return af1;
   }

   double BrcClockCorrection::getAf2() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return af2;
   }             
       
} // namespace
