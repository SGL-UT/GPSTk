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

/**
 * @file VisSupport.hpp - Support functions shared by compStaVis and compSatVis.
 */

#ifndef GPSTK_VISSUPPORT_HPP
#define GPSTK_VISSUPPORT_HPP


#include <iostream>
#include <string>
#include <list>

#include "StringUtils.hpp"
#include "CommandOption.hpp"
#include "FileFilterFrame.hpp"

// rinex
#include "RinexNavStream.hpp"
#include "RinexNavData.hpp"
#include "RinexNavHeader.hpp"

// fic
#include "FICStream.hpp"
#include "FICHeader.hpp"
#include "FICData.hpp"
#include "FICFilterOperators.hpp"

#include "AlmOrbit.hpp"
#include "GPSAlmanacStore.hpp"
#include "YumaAlmanacStore.hpp"
#include "SEMAlmanacStore.hpp"
#include "GPSEphemerisStore.hpp"

// monitor station coordinates
#include "MSCStore.hpp"
#include "ECEF.hpp"

// SP3
#include "SP3Stream.hpp"
#include "SP3EphemerisStore.hpp"

#include "EngEphemeris.hpp"

namespace gpstk
{
   typedef std::map<std::string,gpstk::ECEF> StaPosList; 

   namespace VisSupport
   {
      StaPosList  getStationCoordinates( 
                       const gpstk::CommandOptionWithAnyArg& mscFileName,
                       const gpstk::DayTime&                 dt, 
                       const gpstk::CommandOptionWithAnyArg& includeStation,
                       const gpstk::CommandOptionWithAnyArg& excludeStation);
                       
      void readFICNavData( 
                       const gpstk::CommandOptionWithAnyArg& navFileNameOpt,
                             gpstk::GPSAlmanacStore&         BCAlmList,  
                             gpstk::GPSEphemerisStore&       BCEphList);
                             
      void readRINEXNavData(
                       const gpstk::CommandOptionWithAnyArg& navFileNameOpt,
                             gpstk::GPSEphemerisStore&       BCEphList);
                           
      void readYumaData(
                       const gpstk::CommandOptionWithAnyArg& navFileNameOpt,
                             gpstk::YumaAlmanacStore&        yumaAlmStore );
                           
      void readSEMData(
                       const gpstk::CommandOptionWithAnyArg& navFileNameOpt,
                             gpstk::SEMAlmanacStore&         semAlmStore );
                             
      void readPEData(
                       const gpstk::CommandOptionWithAnyArg& navFileNameOpt,
                             gpstk::SP3EphemerisStore&       SP3EphList );

      bool checkIOD( const gpstk::EngEphemeris ee );

   } // namespace VisSupport

   
} // namespace gpstk

#endif
