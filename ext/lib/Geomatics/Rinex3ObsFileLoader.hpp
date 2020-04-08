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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/// @file Rinex3ObsFileLoader.hpp  Read a RINEX3 file, selecting, counting and saving
/// obs data, then allow caller to access that data easily.

#ifndef GPSTK_RINEX3_FILE_LOADER_INCLUDE
#define GPSTK_RINEX3_FILE_LOADER_INCLUDE

//------------------------------------------------------------------------------------
// system includes
#include <string>
#include <vector>

// GPSTk
#include "Exception.hpp"
#include "CommonTime.hpp"
#include "stl_helpers.hpp"          // vectorindex
#include "MostCommonValue.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsData.hpp"

// gpstk-geomatics
#include "SatPass.hpp"

namespace gpstk {

//--------------------------------------------------------------------------------
/// Class to determine fundamental information about a RINEX obs file, from both
/// the header and by reading part or all of the file, and then load a requested
/// subset of the data into a store.
/// NB. more than one file can be given, but it is assumed these files are "congruent"
/// - from the same site, with the same obs types, and in time order - as if one file.
/// The operation of the class is as follows.
/// 1. Declare an object, and give it a list of (Rinex Obs) files
///    [cf. ctor(filename) or ctor(filenames) or member files(filenames)].
/// 2. Configure the object, using e.g. saveTheData(true), excludeSat(sat), etc
/// 3. Specify which ObsIDs to save - e.g. GC1* GC2* GL1* GL2*
/// 4. Run loadFiles(msg) to read the files (any error messages output in msg)
/// 5. Read the output: dumpSatObsTable() or dumpData() [if saved], and access output
/// 6. Optionally write the output to vector of SatPass with WriteSatPassList()
/// 7. Reset and go again reset() or reset(vector<files>)
class Rinex3ObsFileLoader
{
private:
   static const double dttol;             ///< tolerance in comparing times

   // input
   std::vector<std::string> filenames;    ///< input RINEX obs file names
   int nepochsToRead;                     ///< number of epochs to read (default:all)
   bool saveData;                         ///< if true save the data (F)
   std::string timefmt;                   ///< format for time tags in output
   // editing
   double dtdec;                          ///< decimate to this time step
   CommonTime startTime, stopTime;        ///< edit to start and stop times

   /// input list of wanted ObsIDs, may include "*"-the "any" sys/type/band/trk-code
   /// thus when the header is read this list is expanded into wantedObsTypes
   std::vector<std::string> inputWantedObsTypes;

   /// list of wanted RinexObsIDs, without any "*", which appear in header(s)
   std::vector<std::string> wantedObsTypes;

   /// count of prn/obs for wanted obs types
   /// NB the vector<int> is parallel to wantedObsTypes;
   std::map<RinexSatID, std::vector<int> > SatObsCountMap;

   /// total counts per obs for wanted obs types (parallel to wantedObsTypes)
   std::vector<int> countWantedObsTypes;

   // used internally
   MostCommonValue mcv;                   ///< estimator of the raw time interval
   CommonTime prevtime;                   ///< used in reading loop for dt
   double rawdt;                          ///< best est. of time interval, no decimate
   std::vector<SatID> exSats;             ///< exclude satellites, incl. (-1,sys)

   /// map[sat]=string containing space-separated obs types to be read into SatPass
   /// cf. cfgEditObsTypes()
   std::map<SatID,std::string> mapSatStr;

   // output
   int nepochs;                           ///< number of epochs read
   double nominalDT;                      ///< nominal timestep (rawdt or dtdec)
   CommonTime begDataTime, endDataTime;   ///< first and last times of data
   std::vector<std::string> obstypes;     ///< RINEX obs types found in data
   std::vector<Rinex3ObsHeader> headers;  ///< headers from reading filenames

   /// vector of all input data - filled only if saveData is true.
   std::vector<Rinex3ObsData> datastore;

   /// initialization used by the constructors
   void init(void)
   {
      saveData = false;
      nepochsToRead = -1;
      timefmt = std::string("%04Y/%02m/%02d %02H:%02M:%02S");
      reset();
   }

public:

   // constructors ------------------------------------------------------------
   /// empty constructor
   Rinex3ObsFileLoader(void)
   {
      init();
   }

   /// constructor with one file name
   /// @param[in] in_filename RINEX obs file name to load
   Rinex3ObsFileLoader(std::string in_filename)
   {
      init();
      filenames.clear();
      headers.clear();
      filenames.push_back(in_filename);
   }

   /// constructor with vector of file names
   /// @param[in] in_filenames vector of RINEX obs file names to load
   Rinex3ObsFileLoader(std::vector<std::string> in_filenames)
   {
      init();
      files(in_filenames);
   }

   // resets ------------------------------------------------------------
   /// reset - called before reading another set of files, clears all info
   inline void reset(void)
   {
      nepochs = 0;
      // nominalDT: nominal timestep (rawdt or dtdec)
      dtdec = rawdt = nominalDT = -1.0;
      stopTime = begDataTime = CommonTime::END_OF_TIME;
      startTime = endDataTime = CommonTime::BEGINNING_OF_TIME;
      prevtime = CommonTime::BEGINNING_OF_TIME;

      begDataTime.setTimeSystem(TimeSystem::Any);
      endDataTime.setTimeSystem(TimeSystem::Any);
      startTime.setTimeSystem(TimeSystem::Any);
      stopTime.setTimeSystem(TimeSystem::Any);
      prevtime.setTimeSystem(TimeSystem::Any);

      obstypes.clear();
      mcv.reset();
      datastore.clear();
      exSats.clear();
      headers.clear();
      inputWantedObsTypes.clear();
      wantedObsTypes.clear();
      SatObsCountMap.clear();
   }

   /// reset with new filenames
   /// @param[in] in_filenames vector of RINEX obs file names to load
   inline void reset(std::vector<std::string> in_filenames)
   {
      reset();
      files(in_filenames);
   }

   // configure: before loadFiles() -------------------------------------
   // configure: files, epochs, save?, time limits, etc -----------------

   /// Define the files to be loaded
   /// @param[in] in_filenames vector of RINEX obs file names to load
   inline void files(std::vector<std::string> in_filenames)
   {
      filenames.clear();
      filenames = in_filenames;
   }

   /// Add to list of RINEX 3 ObsIDs (4-character) that are desired for output
   /// Must be a valid 4-char ObsID, must begin with system character, and must not
   /// be a duplicate. E.g. GC1* GC2* GL1* GL2* JS1Z RL2C
   /// @param[in] srot string RINEX 3 ObsID to save, must be 4-char, can use '*'
   /// @return false if invalid input or not a valid ObsID
   inline bool loadObsID(std::string srot)
   {
      if(srot.size() != 4) return false;

      // NB isValidRinexObsID() does not handle system = "*"
      bool is3(srot.substr(0,1) == "*");

      // valid Rinex obs ID
      if(isValidRinexObsID(is3 ? srot.substr(1,3) : srot)) {
         if(vectorindex(inputWantedObsTypes,srot) == -1) {
            inputWantedObsTypes.push_back(srot);
            return true;
         }
      }
      return false;
   }

   /// limit the number of epochs to be read; set -1 (default) to read all
   /// @param[in] n maximum number of epochs to read
   inline void nEpochsToRead(int n) { nepochsToRead = n; }

   /// set save data flag
   /// @param b bool if true, then save the data, otherwise just the headers
   inline void saveTheData(bool b) { saveData = b; }
   /// access save data flag
   /// @return bool if true, then save the data, otherwise just the headers
   inline bool dataSaved(void) { return saveData; }

   /// set the start time
   /// @param[in] tt start time, ignore data before this time
   inline void setStartTime(const CommonTime& tt) { startTime = tt; }
   /// set the stop time
   /// @param[in] tt stop time, ignore data after this time
   inline void setStopTime(const CommonTime& tt) { stopTime = tt; }
   /// turn on decimation, to even seconds-of-week / this delta time
   /// @param[in] dt decimate data to this time step (on even seconds-of-week)
   inline void setDecimation(double dt) { dtdec = dt; }
   /// set time format
   /// @param[in] format (cf. gpstk::Epoch::printf) for time output in dumps
   inline void setTimeFormat(std::string fmt) { timefmt = fmt; }

   /// satellites to be excluded; this may be SatID = (-1, system);
   /// @param[in] sat satellite to be excluded [do not exclude system this way]
   inline void excludeSat(SatID sat) { exSats.push_back(sat); }
   /// vector of satellites to be excluded
   /// @param[in] vector of specific satellites to exclude
   inline void excludeSats(std::vector<SatID> sats)
      { for(int i=0; i<sats.size(); i++) excludeSat(sats[i]); }

   // access results: after loadFiles() ---------------------------------

   /// write a summary of the entire loader configuration/output to a string
   /// @return summary as a single string
   std::string asString(void);

   // get begin time of data
   /// @return beginning time of the entire dataset
   inline CommonTime getDataBeginTime(void) const { return begDataTime; }
   // get end time of data
   /// @return end time of the entire dataset
   inline CommonTime getDataEndTime(void) const { return endDataTime; }

   /// access final list of wanted ObsIDs (no "*")
   /// @return vector of strings, wanted ObsIDs as found in header(s)
   inline std::vector<std::string> getWantedObsTypes(void) const
      { return wantedObsTypes; }

   /// access Sat/Obs counts for list of wanted ObsIDs
   /// @return map<RinexSatID, vector<int>> of counts of data found for Sat/ObsID
   inline std::map<RinexSatID, std::vector<int> > getWantedSatObsCountMap(void) const
      { return SatObsCountMap; }
   /// access total Obs counts for list of wanted ObsIDs
   /// @return vector<int> total epoch counts for wanted obs types
   ///     (parallel to wantedObsTypes)
   inline std::vector<int> getTotalObsCounts(void) const
      { return countWantedObsTypes; }

   /// return the raw data interval, unless decimation has been
   /// done, then return the decimated interval.
   /// @return nominal time step of stored data (== raw and/or decimated)
   inline double getDT(void) { return nominalDT; }

   // TD check index and dim
   /// return a full Rinex3ObsHeader
   /// @param[in] index in input filenames vector
   /// @return Rinex3ObsHeader of corresponding file
   inline Rinex3ObsHeader getFullHeader(unsigned int i) const
      { return headers[i]; }

   /// get the size of the data store
   /// @return size (number of epochs) in the store
   inline const int getStoreSize(void) const { return datastore.size(); }

   /// access the data store
   /// @return const ref to the datastore: vector<Rinex3ObsData>
   inline const std::vector<Rinex3ObsData>& getStore(void) const
      { return datastore; }

   // Read the files ----------------------------------------------------

   /// Read the files already defined
   /// @param[out] errmsg an error/warning message, blank for success
   /// @param[out] msg an informative message
   /// @return 0 ok, >0 number of files read
   int loadFiles(std::string& errmsg, std::string& msg);

   // Utilities ---------------------------------------------------------
   /// Write the stored data to a list of SatPass objects, given a vector of obstypes
   /// and (for each system) a parallel vector of indexes into the Loader's ObsIDs
   /// (getWantedObsTypes()), and a vector of SatPass to be written to.
   /// SPList need not be empty; however if not empty, obstypes must be identical to
   /// those of existing SatPasses.
   /// @param[in] obstypes map of <sys,vector<ObsID>> for SatPass (2or3-char obsID)
   /// @param[in] indexLoadOT map<char,vector<int>> with key=system char,
   ///    value=vector parallel to obstypes with elements equal to
   ///    {index in loader's ObsIDs for each obstype, or -1 if not in loader}
   /// @param[in,out] SPList vector of SatPass to which data store is written
   /// @return >0 number of passes created, -1 inconsistent input, -2 obstypes
   ///   inconsistent with existing SatPass, -3 Loader not configured to save data, 
   ///   -4 no data -5 obstypes not provided for all systems
   int WriteSatPassList(const std::map<char, std::vector<std::string> >& obstypes,
                        const std::map<char, std::vector<int> >& indexLoadOT,
                        std::vector<SatPass>& SPList);

   /// Dump the SatObsCount table
   /// @param ostream s to which to write the table
   void dumpSatObsTable(std::ostream& s) const;

   /// Dump the stored data for one epoch - NB setTimeFormat()
   /// @param ostream s to which to write the data
   void dumpStoreEpoch(std::ostream& s, const gpstk::Rinex3ObsData& rod) const;

   /// Dump the stored data - NB setTimeFormat()
   /// @param ostream s to which to write the data
   void dumpStoreData(std::ostream& s) const;

}; // end class Rinex3ObsFileLoader

// TD where else to put this?
/// Utility to dump a table of all valid RinexObsIDs for all systems and frequencies
/// @param ostream s to which to write the data
void dumpAllRinex3ObsTypes(std::ostream& s);

//@}

} // end namespace gpstk

#endif      // GPSTK_RINEX3_FILE_LOADER_INCLUDE
