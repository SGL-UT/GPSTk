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

#ifndef GPSTK_EPHREADER_HPP
#define GPSTK_EPHREADER_HPP

/** @file This is a class that reads in ephemeris data without the
    caller needing to know the format the data is suppllied in. The 
    navigation data formats that are (to be) supported: rinex nav, fic,
    sp3, mdp, Yuma, and SEM.
    Unlike the ObsReader, this reads in the entire file at once.
**/

#include <string>
#include <vector>

#include "Exception.hpp"
#include "XvtStore.hpp"
#include "SatID.hpp"
#include "GPSWeekSecond.hpp"

namespace gpstk
{
   class EphReader
   {
   public:
      EphReader()
         : verboseLevel(0), eph(NULL) {};

      EphReader(const std::string& fn) 
	 throw(FileMissingException,FFStreamError)
         : verboseLevel(0), eph(NULL) { read(fn); };
   
      int verboseLevel;

      void read(const std::string& fn)
         throw(FileMissingException,FFStreamError);

      std::vector<std::string> filesRead;

         /// Set the various file-reading classes to update their 10-bit
         /// weeks to be within 1/2 a GPS Epoch (512 weeks) of the specified
         /// week.
      static void modify10bitWeeks(short week);

      typedef XvtStore<SatID> EphemerisStore;
      EphemerisStore* eph;

      static std::string formatsUnderstood()
      { return "RINEX nav, FIC, MDP, SP3, YUMA, and SEM";}

   private:
      void read_rinex_nav_data(const std::string& fn);
      void read_fic_data(const std::string& fn);
      void read_sp3_data(const std::string& fn);
      void read_yuma_data(const std::string& fn);
      void read_sem_data(const std::string& fn);
      void read_mdp_data(const std::string& fn);
   };
}
#endif
