#pragma ident "$Id$"

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

#ifndef DDEPOCH_HPP
#define DDEPOCH_HPP

#include <DayTime.hpp>
#include <stl_helpers.hpp>
#include <icd_200_constants.hpp>

#include "ElevationRange.hpp"
#include "SvElevationMap.hpp"

typedef std::map<gpstk::ObsID, double> OIDM;
typedef std::map<gpstk::SatID, OIDM > SvOIDM;
typedef std::map<gpstk::SatID, short > SvShortMap;
typedef std::map<gpstk::SatID, double > SvDoubleMap;
typedef std::pair<gpstk::SatID, gpstk::SatID > SatIdPair;
typedef std::map<SatIdPair, OIDM > PrOIDM;

struct DDEpoch
{
   DDEpoch() : valid(false){};
   
   SvOIDM ddSvOIDM;
   PrOIDM ddPrOIDM;
   
   mutable SvShortMap health;
   mutable SvDoubleMap rangeRate;
   mutable SvDoubleMap elevation;

   double clockOffset;
   gpstk::SatID masterPrn;
   bool valid;
   static unsigned debugLevel;

   // Computes a single difference between two sets of obs
   OIDM singleDifference(
      const gpstk::SvObsEpoch& rx1obs,
      const gpstk::SvObsEpoch& rx2obs,
      double rangeRate);
   
   // Sets the valid flag true if successfull
   // also sets the masterPrn to the one actually used
   void doubleDifference(
      const gpstk::ObsEpoch& rx1,
      const gpstk::ObsEpoch& rx2);

   // Criteria for the masterPrn:
   //   it has an elevation > the min, 
   //   it it on the way up (i.e. doppler>0),
   //   there is a record for it on the other receiver     
   void selectMasterPrn(
      const gpstk::ObsEpoch& rx1, 
      const gpstk::ObsEpoch& rx2);

};


struct DDEpochMap : public std::map<gpstk::DayTime, DDEpoch>
{

   // compute the double difference of all common epochs
   void compute(
      const gpstk::ObsEpochMap& rx1,
      const gpstk::ObsEpochMap& rx2,
      SvElevationMap& pem);

   // Returns a string containing a statistical summary of the double difference
   // residuals for the specified obs type within the given elevation range.
   std::string computeStats(
      const gpstk::ObsID oid,
      const ElevationRange& er,
      const double sigma) const;

   void outputStats(
      std::ostream& s,
      const ElevationRangeList elr,
      const double sigma) const;

   void outputAverages(std::ostream& s) const;
   void dump(std::ostream& s) const;
   
   static unsigned debugLevel;
   static bool useMasterSV;
   unsigned long windowLength;    // seconds
   //unsigned long minimumSNR;      // dB
};


#endif
