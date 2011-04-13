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

/**
 * @file DataStructures.hpp
 * Include file defining the data containers for program DDBase.
 */

#ifndef CLASS_DDBASE_DATA_STRUCT_INCLUDE
#define CLASS_DDBASE_DATA_STRUCT_INCLUDE

//------------------------------------------------------------------------------------
// system includes
#include <string>
#include <vector>

// GPSTk
#include "PRSolution.hpp"
#include "Stats.hpp"

// DDBase
#include "DDBase.hpp"

//------------------------------------------------------------------------------------
// Data structures

// structure for raw data
typedef struct data_structure {
   double L1;     // cycles
   double L2;     // cycles
   double P1;     // m
   double P2;     // m
   double D1;     // Hz  optional when fit to phase used in synchronization
   double D2;     // Hz
   double S1;     // dB-Hz
   double S2;     // dB-Hz
   double ER;     // m
   double elev;   // degrees
   double az;     // degrees
} DataStruct;

// structure for buffered raw good data
class RawData {
public:
   std::vector<double> L1;      // cycles
   std::vector<double> L2;      // cycles
   std::vector<double> P1;      // m
   std::vector<double> P2;      // m
   std::vector<double> S1;      // db-Hz
   std::vector<double> S2;      // db-Hz
   std::vector<double> ER;      // m
   std::vector<double> elev;    // deg
   std::vector<double> az;      // deg
   std::vector<int> count;      // epoch count since FirstEpoch
};

// structure for computing single differences -- just counts and min,max elevation
class SDData {
public:
   double elevmin,elevmax;
   std::vector<int> count;
};

// structure for buffered DDs with biases
class DDData {
public:
   double L1bias,L2bias;
   double prevL1,prevL2;
   // these vectors and count must remain parallel
   std::vector<double> DDL1,DDL2,DDP1,DDP2,DDER;   // data for each point
   std::vector<int> count;                         // count for each point
   std::vector<int> resets;                        // collection of indexes into
                                                   //    count[] where bias is reset
   //DDData(void) : last_buffer_index(0) {};
};

// both reference and unknown positions
class Station {
public:
   bool fixed;                      // if true, hold position fixed, else solve for it
   bool usePRS;                     // if true, use ave. PR solution as position
   gpstk::Position pos;             // either known or solution or apriori
   gpstk::PRSolution PRS;           // pseudorange solution, includes clock bias
   gpstk::Stats<double> PRSXstats;  // stats on pseudorange solution
   gpstk::Stats<double> PRSYstats;  // stats on pseudorange solution
   gpstk::Stats<double> PRSZstats;  // stats on pseudorange solution

   double ant_azimuth;              // (relative) orientation of the antenna dipole

   std::map<gpstk::GSatID,DataStruct> RawDataMap;
                                    // cleaned, raw data at current epoch
   gpstk::DayTime time;             // timetag (SolutionEpoch) of RawDataMap

      // these buffers must remain parallel
   std::map<gpstk::GSatID,RawData> RawDataBuffers;
                                    // buffers of good raw data
   std::vector<double> ClockBuffer; // buffer of clock solution (m)
   std::vector<double> ClkSigBuffer;// buffer of clock solution sigma (m)
   std::vector<double> RxTimeOffset;// SolutionEpoch minus RxTimetag (sec)
      // TD not used? used in OutputClockData
   std::vector<int> CountBuffer;    // epoch count since FirstEpoch - if data exists

   std::string TropType;            // label from input giving type of trop model
   gpstk::TropModel *pTropModel;    // chosen trop model (defined in CommandInput)
   double temp;                     // temperature in degrees Celsius
   double press;                    // pressure in mbars at sealevel
   double rhumid;                   // relative humidity in % (0-100)

   Station(void) throw();           // empty and only constructor
   ~Station(void) throw();          // destructor - free trop model
};

Station& findStationInList(std::map<std::string,Station>& SL, std::string& label)
   throw(gpstk::Exception);

// Rinex observation input files
class ObsFile {
public:
   std::string name;           // file name, not including path
   std::string label;          // Station label to which this obs file belongs
   gpstk::RinexObsStream ins;  // streams for reading RINEX
      // TD use pointer -- operator= does not work for RinexObsStream,
      // yet operator= necessary to form vector<RinexObsStream>
   gpstk::RinexObsHeader Rhead;// RINEX header record (for reading)
   gpstk::RinexObsData Robs;   // RINEX observation record (for reading)

   double dt;                  // nominal time step <= reading past header
   gpstk::DayTime firstTime;   // first good epoch

   int nread;                  // number of records read (-1=unopened, 0=header read)
   bool valid;                 // set false if unopened or at EOF
   bool getNext;               // flag used by ReadNextObs to synchronize reading 
   int inC1,inP1,inP2;         // indexes in RINEX header for pseudorange
   int inL1,inL2;              // indexes in RINEX header for carrier phase
   int inD1,inD2,inS1,inS2;    // needed or used ??

   ObsFile(void) throw();                          // empty constructor
   ObsFile(const ObsFile& of) throw();             // copy constructor
                                                   // (need for vector<ObsFile>)
   ~ObsFile(void);                                 // destructor
   ObsFile& operator=(const ObsFile& of) throw();  // assignment operator
                                                   // (need for copy c'tor)
};

#endif
// nothing below this
//------------------------------------------------------------------------------------

