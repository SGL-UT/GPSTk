//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

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
#include "EOPStore.hpp"
#include "Position.hpp"
#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "Epoch.hpp"
#include "Matrix.hpp"

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
/**
 * @throw Exception
 */
int Configure(int which);        // Configure.cpp
/**
 * @throw Exception
 */
int ReadAndProcessRawData(void); // ReadRawData.cpp
/**
 * @throw Exception
 */
int EditRawDataBuffers(void);    // EditRawDataBuffers.cpp
/**
 * @throw Exception
 */
int OutputRawDataBuffers(void);  // EditRawDataBuffers.cpp
/**
 * @throw Exception
 */
int EphemerisImprovement(void);  // EphemerisImprovement.cpp
/**
 * @throw Exception
 */
int ClockModel(void);            // ClockModel.cpp
/**
 * @throw Exception
 */
int Synchronization(void);       // Synchronization.cpp
/**
 * @throw Exception
 */
int RecomputeFromEphemeris(void);// Synchronization.cpp
/**
 * @throw Exception
 */
int Timetable(void);             // Timetable.cpp
/**
 * @throw Exception
 */
int QueryTimeTable(gpstk::SDid& sdid, gpstk::CommonTime& tt);
/**
 * @throw Exception
 */
int QueryTimeTable(std::string bl, int& beg, int& end);
/**
 * @throw Exception
 */
int DoubleDifference(void);      // DoubleDifference.cpp
/**
 * @throw Exception
 */
int EditDDs(void);               // EditDDs.cpp
/**
 * @throw Exception
 */
int Estimation(void);            // Estimation.cpp
/**
 * @throw Exception
 */
gpstk::Matrix<double> SingleAxisRotation(double angle, const int axis);
   // DDBase.cpp

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
