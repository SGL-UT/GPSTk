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

/**
 * @file DDBase.hpp
 * Main include file for DDBase - the ARL:UT network precise position estimation
 * using double differenced GPS carrier phase.
 */

//------------------------------------------------------------------------------------
#ifndef CLASS_DDBASE_PROCESSOR_INCLUDE
#define CLASS_DDBASE_PROCESSOR_INCLUDE

//------------------------------------------------------------------------------------
// includes
// system
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <ctime>

// GPSTk
//#define RANGECHECK // if defined, Vector and Matrix will throw on invalid index.
#include "Exception.hpp"
#include "StringUtils.hpp"
#include "CommonTime.hpp"
#include "TropModel.hpp"
#include "SP3EphemerisStore.hpp"
#include "GPSEphemerisStore.hpp"
#include "EarthOrientation.hpp"
#include "Position.hpp"
#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "Epoch.hpp"

// DDBase
#include "GSatID.hpp"             // this one first
#include "DataStructures.hpp"
#include "DDid.hpp"
#include "constants.hpp"
#include "format.hpp"

//------------------------------------------------------------------------------------
extern clock_t totaltime;        // for timing tests

//------------------------------------------------------------------------------------
// prototypes
int Configure(int which) throw(gpstk::Exception);        // Configure.cpp
int ReadAndProcessRawData(void) throw(gpstk::Exception); // ReadRawData.cpp
int EditRawDataBuffers(void) throw(gpstk::Exception);    // EditRawDataBuffers.cpp
int OutputRawDataBuffers(void) throw(gpstk::Exception);  // EditRawDataBuffers.cpp
int EphemerisImprovement(void) throw(gpstk::Exception);  // EphemerisImprovement.cpp
int ClockModel(void) throw(gpstk::Exception);            // ClockModel.cpp
int Synchronization(void) throw(gpstk::Exception);       // Synchronization.cpp
int RecomputeFromEphemeris(void) throw(gpstk::Exception);// Synchronization.cpp
int Timetable(void) throw(gpstk::Exception);             // Timetable.cpp
int QueryTimeTable(gpstk::SDid& sdid, gpstk::CommonTime& tt) throw(gpstk::Exception);
int QueryTimeTable(std::string bl, int& beg, int& end) throw(gpstk::Exception);
int DoubleDifference(void) throw(gpstk::Exception);      // DoubleDifference.cpp
int EditDDs(void) throw(gpstk::Exception);               // EditDDs.cpp
int Estimation(void) throw(gpstk::Exception);            // Estimation.cpp

//------------------------------------------------------------------------------------
// Global data -- see DDBase.cpp where these are declared and documented
extern std::string Title;
extern std::string PrgmDesc;
extern std::ofstream oflog;
//extern CommandInput CI;   put in CommandInput.hpp
extern std::vector<std::string> Baselines;

extern gpstk::CommonTime SolutionEpoch;
extern gpstk::CommonTime FirstEpoch,LastEpoch,MedianEpoch;
extern int Count; 
extern int maxCount;
extern double wave;

extern gpstk::XvtStore<gpstk::SatID> *pEph;
extern gpstk::EOPStore EOPList;
extern gpstk::EarthOrientation eorient;

extern std::map<std::string,Station> Stations;
extern std::vector<ObsFile> ObsFileList;

extern std::map<gpstk::DDid,DDData> DDDataMap;

#endif

// include the 'command line input configuration' structure
#include "CommandInput.hpp"

// nothing below this
