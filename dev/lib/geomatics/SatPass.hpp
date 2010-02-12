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

/**
 * @file SatPass.hpp
 * Data for one complete satellite pass overhead.
 */

#ifndef GPSTK_SATELLITE_PASS_INCLUDE
#define GPSTK_SATELLITE_PASS_INCLUDE

#include <vector>
#include <map>

#include "DayTime.hpp"
#include "GSatID.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsData.hpp"
#include "Exception.hpp"

namespace gpstk {

/// class SatPass holds all range and phase data for a full satellite pass.
/// Constructed and filled by the calling program, it is used to pass data into
/// and out of the GPSTK discontinuity corrector.
/// NB. if objects of this class are combined together, e.g. in STL containers
/// such as list or vector, they MUST be consistently defined, namely the number
/// of observation types must be the same, otherwise a nasty segmentation fault
/// can occur when building the STL container.
class SatPass {
public:
   // ------------------ configuration --------------------------------

   // public member functions -----------------------------------------

   /// Constructor for the given sat; default obs types are L1, L2, P1, P2,
   /// in that order; dt is the nominal time spacing of the data.
   /// NB. dt MUST be correct.
   /// @param sat the satellite from which this data comes
   /// @param dt  the nominal time spacing (seconds) of the data
   SatPass(GSatID sat, double dt) throw();

   /// Constructor from a list of strings <=> RINEX obs types to be read
   /// NB. The number of obstypes determines the size of the SatPass object;
   /// therefore objects with different numbers of obs types must not be
   /// combined together in things like STL containers, which assume a fixed
   /// size for all objects of one class.
   /// NB. dt MUST be correct.
   /// @param sat the satellite from which this data comes
   /// @param dt  the nominal time spacing (seconds) of the data
   /// @param obstypes  a vector of strings, each string being a 2-character
   ///                  RINEX observation type, e.g. "L1", "P2", to be stored.
   SatPass(GSatID sat, double dt, std::vector<std::string> obstypes) throw();

   // d'tor, copy c'tor are built by compiler; so is operator= but don't use it!
   SatPass& operator=(const SatPass& right) throw();

   // Add data to the arrays at timetag tt; calls must be made in time order.
   // Caller sets the flag to either BAD or OK later using flag().

   /// Add vector of data, identified by obstypes (same as used in c'tor) at tt,
   /// Flag is set 'good' and lli=ssi=0
   /// @param tt        the time tag of interest
   /// @param obstypes  a vector of strings, each string being a 2-character
   ///                  RINEX observation type, e.g. "L1", "P2", to be stored.
   ///                  This MUST match the list used in the constructor.
   /// @param data      a vector of data values, parallel to the obstypes vector
   /// @return n>=0 if data was added successfully, n is the index of the new data
   ///        -1 if a gap is found (no data is added),
   ///        -2 if time tag is out of order (no data is added)
   int addData(const DayTime tt, std::vector<std::string>& obstypes,
                                  std::vector<double>& data) throw(Exception);

   /// Add vector of data, identified by obstypes (same as used in c'tor) at tt,
   /// Flag, lli and ssi are set using input (parallel to data).
   /// @param tt        the time tag of interest
   /// @param obstypes  a vector of strings, each string being a 2-character
   ///                  RINEX observation type, e.g. "L1", "P2", to be stored.
   ///                  This MUST match the list used in the constructor.
   /// @param data      a vector of data values, parallel to the obstypes vector
   /// @param lli       a vector of LLI values, parallel to the obstypes vector
   /// @param ssi       a vector of SSI values, parallel to the obstypes vector
   /// @return n>=0 if data was added successfully, n is the index of the new data
   ///        -1 if a gap is found (no data is added),
   ///        -2 if time tag is out of order (no data is added)
   int addData(const DayTime tt, std::vector<std::string>& obstypes,
                                  std::vector<double>& data,
                                  std::vector<unsigned short>& lli,
                                  std::vector<unsigned short>& ssi,
                                  unsigned short flag=SatPass::OK)
      throw(Exception);

   /// Add data as found in RinexObsData. No action if this->sat is not found.
   /// Pull out time tag and all data in obs type list. All flags are set 'good'.
   /// @param robs  Rinex observation data from which to pull data. Only data for
   ///              the object's satellite is added.
   /// @return n>=0 if data was added successfully, n is the index of the new data
   ///        -1 if a gap is found (no data is added),
   ///        -2 if time tag is out of order (no data is added)
   ///        -3 if the satellite was not found in the RinexObsData (no data added)
   int addData(const RinexObsData& robs) throw();

   /// Smooth pseudorange and debias phase.
   /// This routine can be called only if dual frequency range and phase data
   /// (P1,P2,L1,L2) have been stored, and should be called only after all
   /// cycleslips have been removed.
   /// @param  smoothPR  if true, replace the pseudorange (P1 and P2) with smoothed
   ///                   ranges.
   /// @param  smoothPH  if true, replace the phase (L1 and L2) with debiased
   ///                   phase.
   /// @param  msg       a string containing a single-line statistical summary of
   ///                   the smoothing results, and the estimated phase biases.
   ///  This string consists of the following 18 fields separated by whitespace.
   ///  1) "SMT", 2) satellite id (e.g. G07), 3) start GPS week, 4) start GPS
   ///  seconds of week, 5) end GPS week, 6) end GPS seconds of week,
   ///  (7-11 are for the L1 bias data in units of meters)
   ///  7) number, 8) average, 9) std deviation, 10) minimum, and 11) maximum,
   ///  (12-16 are for the L2 bias data in units of meters)
   ///  12) number, 13) average, 14) std deviation, 15) minimum, and 16) maximum,
   ///  17) the L1 bias in cycles, 18) the L2 bias in cycles.
   void smooth(bool smoothPR, bool smoothPH, std::string& msg) throw(Exception);

   // -------------------------- get and set routines --------------------------

   // can change ssi, lli, data, but not times,sat,dt,ngood,count
   // get and set flag so you can update ngood
   // lvalue for the data or SSI/LLI arrays of this SatPass at index i

   /// Access the status; l-value may be assigned SP.status() = 1;
   int& status(void) throw() { return Status; }

   /// Access the data for one obs type at one index, as either l-value or r-value
   /// @param  i    index of the data of interest
   /// @param  type observation type (e.g. "L1") of the data of interest
   /// @return the data of the given type at the given index
   double& data(unsigned int i, std::string type) throw(Exception);

   /// Access the time offset from the nominal time (i.e. timetag) at one index
   /// (epoch), as either l-value or r-value
   /// @param  i    index of the data of interest
   /// @return the time offset from nominal at the given index
   double& timeoffset(unsigned int i) throw(Exception);

   /// Access the LLI for one obs type at one index, as either l-value or r-value
   /// @param  i    index of the data of interest
   /// @param  type observation type (e.g. "L1") of the data of interest
   /// @return the LLI of the given type at the given index
   unsigned short& LLI(unsigned int i, std::string type) throw(Exception);

   /// Access the ssi for one obs type at one index, as either l-value or r-value
   /// @param  i    index of the data of interest
   /// @param  type observation type (e.g. "L1") of the data of interest
   /// @return the SSI of the given type at the given index
   unsigned short& SSI(unsigned int i, std::string type) throw(Exception);

   // -------------------------------- set routines ----------------------------

   /// change the maximum time gap (in seconds) allowed within any SatPass
   /// @param gap  The maximum time gap (in seconds) allowed within any SatPass
   /// @return the input value.
   static double setMaxGap(const double gap) { maxGap = gap; return maxGap; }

   /// set timetag output format
   /// @param fmt  The format of time tags in the output
   ///             (cf. gpstk DayTime::printf() for syntax)
   void setOutputFormat(std::string fmt) { outFormat = fmt; };

   /// set the flag at one index to flag - use the SatPass constants OK, etc.
   /// @param  i    index of the data of interest
   /// @param  flag flag (e.g. SatPass::BAD).
   void setFlag(unsigned int i, unsigned short flag) throw(Exception);

   // -------------------------------- get routines ----------------------------

   /// get the max. gap limit size (seconds); for all SatPass objects
   /// @return the current value of maximum gap (sec)
   double getMaxGap(void) const throw() { return maxGap; }

   /// get the list of obstypes
   /// @return the vector of strings giving RINEX obs types
   std::vector<std::string> getObsTypes(void) throw() {
      std::vector<std::string> v;
      for(int i=0; i<labelForIndex.size(); i++) v.push_back(labelForIndex[i]);
      return v;
   }

   /// get the flag at one index
   /// @param  i    index of the data of interest
   /// @return the flag for the given index
   unsigned short getFlag(unsigned int i) throw(Exception);

   /// @return the earliest time in this SatPass data
   DayTime getFirstTime(void) const throw() { return firstTime; }
   
   /// Reset the first and last times for the pass. This should be necessary only
   /// when the timeoffset's have been altered...use with caution.
   /// @param  tt   new first time
   //void resetFirstTime(DayTime& tt) throw()
   //   { firstTime = tt; }
   //void resetLastTime(DayTime& tt) throw()
   //   { lastTime = tt; }

   /// @return the latest time in this SatPass data
   DayTime getLastTime(void) const throw() { return lastTime; }

   /// @return the earliest time of good data in this SatPass data
   DayTime getFirstGoodTime(void) const throw() {
      for(int j=0; j<spdvector.size(); j++) if(spdvector[j].flag & OK) {
         return time(j);
      }
      return DayTime::END_OF_TIME;
   }

   /// @return the latest time of good data in this SatPass data
   DayTime getLastGoodTime(void) const throw() {
      for(int j=spdvector.size()-1; j>=0; j--) if(spdvector[j].flag & OK) {
         return time(j);
      }
      return DayTime::BEGINNING_OF_TIME;
   }

   /// get the satellite of this SatPass
   /// @return the satellite of this SatPass data
   GSatID getSat(void) const throw() { return sat; }

   /// get the time interval of this SatPass
   /// @return the nominal time step (seconds) in this data
   double getDT(void) const throw() { return dt; }

   /// get the number of good points in this SatPass
   /// @return the number of good points (flag != BAD) in this object
   int getNgood(void) const throw() { return ngood; }

   /// get the size of (the arrays in) this SatPass
   /// @return the size of the data array in this object
   unsigned int size(void) const throw() { return spdvector.size(); }

   /// get one element of the count array of this SatPass
   /// @param  i   index of the data of interest
   /// @return the count at the given index. Count is the number of timesteps DT
   /// between the first time tag and the current time tag.
   unsigned int getCount(unsigned int i) const throw(Exception);

   // -------------------------------- utils ---------------------------------

   /// clear the data (but not the obs types) from the arrays
   void clear(void) throw() { spdvector.clear(); }

   /// compute the timetag associated with index i in the data array
   /// @param  i   index of the data of interest
   /// @return the time tag at the given index.
   DayTime time(unsigned int i) const throw(Exception);

   /// return true if the given timetag is or could be part of this pass
   /// @param tt        the time tag of interest
   /// @return true if the given time tag lies within the time interval covered
   /// by this object.
   bool includesTime(const DayTime& tt) const throw();

   /// create a new SatPass from the given one, starting at count N.
   /// modify this SatPass to end just before N.
   /// return true if successful.
   bool split(int N, SatPass &newSP);

   /// Decimate the data in the SatPass by (integer) factor N, referencing refTime;
   /// that is keep only epochs that satisfy time=refTime+n*N*dt where n is also an
   /// integer, and dt is the current time spacing of the time spacing of the SatPass.
   /// This routine decimates the data, reduces the arrays, and may change the
   /// start and stop times and ngood; time offsets are not changed.
   /// @param N       New time spacing is N(>1) times the current time spacing
   /// @param refTime Reference DayTime for the decimation, default is to use first
   ///                  in pass
   void decimate(const int N, DayTime refTime=DayTime::BEGINNING_OF_TIME)
      throw(Exception);

   /// Dump all the data in the pass, one line per timetag.
   /// @param os    output stream
   /// @param msg1  put message msg1 at beginning of each line,
   /// @param msg2  put msg2 at the end of the first (#comment) line.
   void dump(std::ostream& os, std::string msg1, std::string msg2=std::string())
      throw();

   /// 'less than' is required for sort() and map<SatPass,...>.find(SatPass)
   bool operator<(const SatPass& right) const
      { return firstTime < right.firstTime; }

   // other ---------------------------------------------------------

   /// Write a one-line summary of the object, consisting of total points,
   /// satellite, number of good points, status, start time, end time,
   /// time step (sec), and observation types.
   /// @param os  output stream
   /// @param sp  SatPass object to output
   friend std::ostream& operator<<(std::ostream& os, SatPass& sp);

   // public member data -----------------------------------------------

   /// flag indicating bad data
   static const unsigned short BAD;

   /// flag indicating good data with no phase discontinuity
   /// NB test for 'good' data using (flag != SatPass::BAD),
   ///                           NOT (flag == SatPass::OK).
   static const unsigned short OK;

   /// flag indicating good data with phase discontinuity on L1 only.
   /// if(flag & LL1) then there is an L1 discontinuity
   static const unsigned short LL1;

   /// flag indicating good data with phase discontinuity on L2 only.
   /// if(flag & LL2) then there is an L2 discontinuity
   static const unsigned short LL2;

   /// flag indicating good data with phase discontinuity on both L1 and L2.
   /// if(flag & LL3) then there are L1 and L2 discontinuities
   static const unsigned short LL3;

   /// size of maximum time gap, in seconds, allowed within SatPass data.
   static double maxGap;

   /// format string, as defined in class DayTime, for output of times
   /// used by smooth (debug), dump and operator<<
   static std::string outFormat;

protected:
   /// compute the count associated with the time tt; return -1 if not within
   /// the time limits of the SatPass
   /// @param tt the time tag of interest
   int countForTime(const DayTime& tt) const throw(Exception)
      { return int((tt-firstTime)/dt + 0.5); }

   // --------------- SatPassData data structure for internal use only ----------
   //
   struct SatPassData {

      // ---------------------- member data ----------------------------------

      /// a flag (cf. SatPass::BAD, etc.) that is set to OK at creation
      /// then reset by other processing.
      unsigned short flag;
      /// time 'count' : time of data = FirstTime + ndt * dt + offset
      unsigned int ndt;
      /// offset of time from integer number * dt since FirstTime.
      double toffset;
      /// data for one epoch of RINEX data
      std::vector<double> data;
      /// loss-of-lock and signal-strength indicators (from RINEX) for data
      /// parallel to data vector
      std::vector<unsigned short> lli,ssi;

      // ---------------------- private member functions ---------------------

      /// constructor
      /// @param n the number of data types to be stored, default 4
      SatPassData(unsigned short n=4) : flag(SatPass::OK), ndt(0), toffset(0.0)
      {
         data = std::vector<double>(n,0.0);
         lli = std::vector<unsigned short>(n,0);
         ssi = std::vector<unsigned short>(n,0);
      }

      // d'tor, copy c'tor and operator= are built by compiler,
      // but operator= will not work correctly, b/c a deep copy is needed.
      SatPassData& operator=(const SatPassData& right) throw()
      {
         if(&right != this) {
            flag = right.flag;
            ndt = right.ndt;
            toffset = right.toffset;
            data.resize(right.data.size());
            lli.resize(right.lli.size());
            ssi.resize(right.ssi.size());
            int i;
            for(i=0; i<right.data.size(); i++) data[i] = right.data[i];
            for(i=0; i<right.lli.size(); i++) lli[i] = right.lli[i];
            for(i=0; i<right.ssi.size(); i++) ssi[i] = right.ssi[i];
         }

         return *this;
      }

   }; // end struct SatPassData

   // --------------- private member data -----------------------------
   /// Status flag for use exclusively by the caller. It is set to 0
   /// by the constructors, but otherwise ignored by class SatPass and
   /// class SatPassIterator.
   int Status;

   /// Nominal time spacing of the data; determined on input or by decimate()
   double dt;

   /// Satellite identifier for this data.
   GSatID sat;

   /// STL map relating strings identifying obs types with indexes in SatPassData
   std::map<std::string,unsigned int> indexForLabel;
   std::map<unsigned int,std::string> labelForIndex;

      // above determined at construction; the rest determined by input data

   /// timetags of the first and last data points.
   DayTime firstTime,lastTime;

   /// number of timetags with good data in the data arrays.
   unsigned int ngood;

   /// ALL data in the pass, stored in SatPassData objects, in time order
   std::vector<SatPassData> spdvector;

   // --------------- private member functions ------------------------

   /// called by constructors to initialize - see doc for them.
   void init(GSatID sat, double dt, std::vector<std::string> obstypes) throw();

   /// add a complete SatPassData at time tt
   /// @return n>=0 if data was added successfully, n is the index of the new data
   ///            -1 if a gap is found (no data is added),
   ///            -2 if time tag is out of order (no data is added)
   int push_back(const DayTime tt, SatPassData& spd) throw();

   /// get a complete SatPassData at count i
   struct SatPassData getData(unsigned int i) const throw(Exception);

   // --------------- friend functions --------------------------------

   /// Sort a vector<SatPass> on time, using the firstTime member.
   friend void sort(std::vector<SatPass>& SPList) throw();

   /// Read a set of RINEX observation files, filling a vector of SatPass objects.
   /// Create the SatPass objects using the input list of observation types
   /// and timestep. If there are no obs types given (vector obstypes has size 0),
   /// then use the obs types in the header of the first file read. The files
   /// are sorted on their begin time before processing, and the resulting list
   /// of SatPass objects is in time order. Data before the begin time, and after
   /// the end time, is ignored. The list of SatPass objects need not be empty on
   /// input; however if not empty, then the list must be consistent with the
   /// input timestep and obs type list, and it will be sorted to be in time order.
   /// If a file cannot be opened, or it is not RINEX observations, simply skip it.
   /// @param filenames vector of input RINEX observation file names
   /// @param obstypes  vector of observation types to include in SatPass (may
   ///                   be empty: include all)
   /// @param dt        data interval of existing SPList and input files
   /// @param SPList    vector of SatPass objects to fill, need not be empty, but
   ///                   if not empty, must have consistent obs types and dt.
   /// @param beginTime reject data before this time (BEGINNING_OF_TIME)
   /// @param endTime   reject data after this time (END_OF TIME)
   /// @return -1 if the filenames list is empty, otherwise return the number of
   /// files successfully read (may be less than the number input).
   /// @throw gpstk Exceptions if there are exceptions while reading, or if the
   ///   data in the file is out of time order
   friend int SatPassFromRinexFiles(std::vector<std::string>& filenames,
                                    std::vector<std::string>& obstypes,
                                    double dt,
                                    std::vector<SatPass>& SPList,
               gpstk::DayTime beginTime=gpstk::DayTime::BEGINNING_OF_TIME,
               gpstk::DayTime endTime=gpstk::DayTime::END_OF_TIME)
      throw(Exception);

   /// Iterate over the input vector of SatPass objects (sorted to be in time
   /// order) and write them, with the given header, to a RINEX observation file
   /// of the given filename.
   /// @return -1 if the file could not be opened, otherwise return 0.
   friend int SatPassToRinexFile(std::string filename,
                                 RinexObsHeader& header,
                                 std::vector<SatPass>& SPList)
      throw(Exception);

   /// class SatPassIterator allows the caller to access all the data in a list
   /// of SatPass objects in time order.
   friend class SatPassIterator;

}; // end class SatPass


/// Iterate over a list (vector) of SatPass using this class. NB. this class ignores
/// passes that have Status less than zero, but does not change any Status.
class SatPassIterator {
public:
   /// Explicit (only) constructor. Check the list for consistency (else throw)
   /// and find common time step and obs types, as well as first and last times.
   /// Ignore passes with Status less than zero.
   /// After building a vector<SatPass> (in time order), declare an iterator
   /// using this constructor, then call member function next(RinexObsData&) to
   /// access the data in time order, until it returns non-zero.
   /// @param splist   Vector of (consistent) SatPass objects
   /// @return 1 for success, 0 at the end of the dataset.
   /// @throw if input list is empty, or
   ///        if elements in the list have differing data interval or obs types, or
   ///        if any observation type is not registered (cf. RinexUtilities.hpp)
   explicit SatPassIterator(std::vector<SatPass>& splist) throw(Exception);

   /// Access (all of) the data for the next epoch. As long as this function
   /// returns non-zero, there is more data to be accessed.
   /// Ignore passes with Status less than zero.
   /// @param indexMap  map<unsigned int, unsigned int> defined so that all the
   ///                  data in the current iteration is found at
   ///                  SatPassList[i].data(j) where indexMap[i] = j.
   /// @return 1 for success, 0 at the end of the dataset.
   /// @throw if time tags are out of order.
   int next(std::map<unsigned int, unsigned int>& indexMap) throw(Exception);

   /// Access (all of) the data for the next epoch. As long as this function
   /// returns non-zero, there is more data to be accessed.
   /// Ignore passes with Status less than zero.
   /// NB. If SatPass obs types are not registered (cf. RinexUtilities.hpp), then
   /// data will NOT be added to RinexObsData.
   /// @param robs  RinexObsData in which data is returned.
   /// @return 1 for success, 0 at the end of the dataset.
   /// @throw if time tags are out of order
   int next(RinexObsData& robs) throw(Exception);

   /// Restart the iteration, i.e. return to the initial time
   void reset(void) throw();

   /// Get the first (earliest) time found in the SatPass list.
   DayTime getFirstTime(void) throw() { return FirstTime; }

   /// Get the last (latest) time found in the SatPass list.
   DayTime getLastTime(void) throw() { return LastTime; }

   /// @return the earliest time of good data in this SatPass list
   DayTime getFirstGoodTime(void) const throw() {
      DayTime ttag = LastTime;
      for(int i=0; i<SPList.size(); i++)
         if(SPList[i].getFirstGoodTime() < ttag)
            ttag = SPList[i].getFirstGoodTime();
      return ttag;
   }

   /// @return the latest time of good data in this SatPass list
   DayTime getLastGoodTime(void) const throw() {
      DayTime ttag = FirstTime;
      for(int i=0; i<SPList.size(); i++)
         if(SPList[i].getLastGoodTime() > ttag)
            ttag = SPList[i].getLastGoodTime();
      return ttag;
   }

   /// Get the time interval, which is common to all the SatPass in the list.
   double getDT(void) throw() { return DT; }

   /// get a map of pairs of indexes for the current epoch. call this after calling
   /// next() to get pairs (i,j) where the data returned by next() is the same as
   /// SatPassList[i].data(j,<obstype>), for each i in the map, and j=map[i].
   std::map<unsigned int,unsigned int> getIndexes(void) throw()
      { return nextIndexMap; }

private:
   SatPassIterator(const SatPassIterator&);              // DO NOT implement
   SatPassIterator& operator=(const SatPassIterator&);   // DO NOT implement

   /// count of the current epoch, = 0,1,...
   int currentN;                    // current time = FirstTime + currentN*DT

   /// time step (seconds) found in all objects in the list (constructor throws
   /// if they are not identical).
   double DT;

   /// first (earliest) start time of the passes in the list, and
   /// last (latest) end time of the passes in the list.
   DayTime FirstTime,LastTime;

   /// index of the current object in the list for this satellite
   std::map<GSatID,int> listIndex;

   /// index of the data vector (spdvector) of the current object in the list
   /// for this satellite
   std::map<GSatID,int> dataIndex;

   /// offset in count of the current object in the list for this satellite
   std::map<GSatID,int> countOffset;

   /// vector parallel to SPList used to mark current use by iteration;
   /// value is "-1,0,1" as SPList[i] is "yet to be used, in use, done".
   /// This has nothing to do with SatPass::Status()
   std::vector<int> indexStatus;

   /// reference to the vector of passes being processed
   std::vector<SatPass>& SPList;

   /// map of indexes i,j, created by next(), such that data returned by next() is
   /// found at SatPassList[i].spdvector[j] where map[i]=j.
   std::map<unsigned int,unsigned int> nextIndexMap;

}; // end class SatPassIterator

/// Stream output for SatPass.
/// @param os output stream to write to
/// @param sp SatPass to write
/// @return reference to os.
std::ostream& operator<<(std::ostream& os, gpstk::SatPass& sp);

}  // end namespace gpstk

// -----------------------------------------------------------------------------------
#endif // GPSTK_SATELLITE_PASS_INCLUDE
