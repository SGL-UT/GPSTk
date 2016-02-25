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

/// @file SatPass.hpp
/// Data for one complete satellite pass.

#ifndef GPSTK_SATELLITE_PASS_INCLUDE
#define GPSTK_SATELLITE_PASS_INCLUDE

#include <string>
#include <vector>
#include <map>
#include <ostream>

#include "Epoch.hpp"
#include "RinexSatID.hpp"
#include "GNSSconstants.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsData.hpp"
#include "RinexUtilities.hpp"
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
protected:
   // --------------- SatPassData data structure for internal use only ----------
   //
   struct SatPassData {
      // member data ----------------------------------
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

      // private member functions ---------------------

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
   RinexSatID sat;

   /// STL map relating strings identifying obs types with indexes in SatPassData
   std::map<std::string,unsigned int> indexForLabel;
   std::map<unsigned int,std::string> labelForIndex;

      // above determined at construction; the rest determined by input data

   /// nominally, timetags of the first and last data points; in fact firstTime is
   /// used with toffset to give the full time, so these are actually times - toffset
   Epoch firstTime,lastTime;

   /// number of timetags with good data in the data arrays.
   unsigned int ngood;

   /// ALL data in the pass, stored in SatPassData objects, in time order
   std::vector<SatPassData> spdvector;

   // --------------- private member functions ------------------------

   /// called by constructors to initialize - see doc for them.
   void init(RinexSatID sat, double dt, std::vector<std::string> obstypes) throw();

   /// add a complete SatPassData at time tt
   /// @return n>=0 if data was added successfully, n is the index of the new data
   ///            -1 if a gap is found (no data is added),
   ///            -2 if time tag is out of order (no data is added)
   int push_back(const Epoch tt, SatPassData& spd) throw();

   /// get a complete SatPassData at count i
   struct SatPassData getData(unsigned int i) const throw(Exception);

public:
   // ------------------ friends --------------------------------------
   /// class gdc is used to detect and correct cycleslips
   friend class gdc;

   /// class SatPassIterator allows the caller to access all the data in a list
   /// of SatPass objects in time order.
   friend class SatPassIterator;

   // friends in SatPassUtilities.hpp
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
   /// If a file cannot be opened, or it is not RINEX observation, simply skip it.
   /// @param filenames vector of input RINEX observation file names
   /// @param obstypes  vector of observation types to include in SatPass (may
   ///                   be empty: include all)
   /// @param dt        data interval of existing SPList and input files
   /// @param SPList    vector of SatPass objects to fill, need not be empty, but
   ///                   if not empty, must have consistent obs types and dt.
   /// @param lenient   if true (default), be lenient in reading the RINEX format
   /// @param beginTime reject data before this time (BEGINNING_OF_TIME)
   /// @param endTime   reject data after this time (END_OF TIME)
   /// @return -1 if the filenames list is empty, otherwise return the number of
   ///                files successfully read (may be less than the number input).
   /// @throw gpstk Exceptions if there are exceptions while reading, if the data
   ///              in the file is out of time order.
   /// @deprecated in favor of RinexObsFilesLoader version
   friend int SatPassFromRinexFiles(std::vector<std::string>& filenames,
                                    std::vector<std::string>& obstypes,
                                    double dt,
                                    std::vector<SatPass>& SPList,
                                    std::vector<RinexSatID> exSats,
                                    bool lenient,
                                    Epoch beginTime,
                                    Epoch endTime) throw(Exception);

   /// Iterate over the input vector of SatPass objects (sorted to be in time
   /// order) and write them, with the given header, to a RINEX observation file
   /// of the given filename.
   /// @return -1 if the file could not be opened, otherwise return 0.
   friend int SatPassToRinexFile(std::string filename,
                                 RinexObsHeader& header,
                                 std::vector<SatPass>& SPList) throw(Exception);

   // ------------------ configuration --------------------------------
   /// Constructor for the given sat; default obs types are L1, L2, P1, P2,
   /// in that order; dt is the nominal time spacing of the data.
   /// NB. dt MUST be correct.
   /// @param sat the satellite from which this data comes
   /// @param dt  the nominal time spacing (seconds) of the data
   SatPass(RinexSatID sat, double dt) throw();

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
   SatPass(RinexSatID sat, double dt, std::vector<std::string> obstypes) throw();

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
   int addData(const Epoch tt, std::vector<std::string>& obstypes,
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
   int addData(const Epoch tt, const std::vector<std::string>& obstypes,
                                 const std::vector<double>& data,
                                 const std::vector<unsigned short>& lli,
                                 const std::vector<unsigned short>& ssi,
                                 const unsigned short flag=SatPass::OK)
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

   // -------------------------------- set only --------------------------------
   /// change the maximum time gap (in seconds) allowed within any SatPass
   /// @param gap  The maximum time gap (in seconds) allowed within any SatPass
   /// @return the input value.
   static double setMaxGap(const double gap) { maxGap = gap; return maxGap; }

   /// set timetag output format
   /// @param fmt  The format of time tags in the output
   ///             (cf. gpstk Epoch::printTime() for syntax)
   void setOutputFormat(std::string fmt, int round=3)
      { outFormat = fmt; outRound=round; }

   /// get the timetag output format
   /// @return The format of time tags in the output
   ///         (cf. gpstk Epoch::printTime() for syntax)
   std::string getOutputFormat(void) { return outFormat; }

   /// set the flag at one index to flag - use the SatPass constants OK, etc.
   /// @param  i    index of the data of interest
   /// @param  flag flag (e.g. SatPass::BAD).
   void setFlag(unsigned int i, unsigned short flag) throw(Exception);

   // -------------------------------- get only --------------------------------
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

   /// @return the earliest time (full, including toffset) in this SatPass data
   Epoch getFirstTime(void) const throw();
   
   /// @return the latest time (full, including toffset) in this SatPass data
   Epoch getLastTime(void) const throw();

   /// @return the earliest time of good data in this SatPass data
   Epoch getFirstGoodTime(void) const throw() {
      for(int j=0; j<spdvector.size(); j++) if(spdvector[j].flag & OK) {
         return time(j);
      }
      return CommonTime::END_OF_TIME;
   }

   /// @return the latest time of good data in this SatPass data
   Epoch getLastGoodTime(void) const throw() {
      for(int j=spdvector.size()-1; j>=0; j--) if(spdvector[j].flag & OK) {
         return time(j);
      }
      return CommonTime::BEGINNING_OF_TIME;
   }

   /// get the satellite of this SatPass
   /// @return the satellite of this SatPass data
   RinexSatID getSat(void) const throw() { return sat; }

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

   /// Access the data for either of two obs type at one index, as r-value only
   /// @param  i     index of the data of interest
   /// @param  type1 observation type (e.g. "P1") of the data of interest
   /// @param  type2 observation type (e.g. "C1") of the data of interest
   /// @return the data of the given type at the given index
   double data(unsigned int i, std::string type1, std::string type2) throw(Exception);

   /// Access the LLI for either of two obs type at one index, as r-value only
   /// @param  i     index of the data of interest
   /// @param  type1 observation type (e.g. "P1") of the data of interest
   /// @param  type2 observation type (e.g. "C1") of the data of interest
   /// @return the LLI of the given type at the given index
   unsigned short LLI(unsigned int i, std::string type1, std::string type2)
      throw(Exception);

   /// Access the ssi for either of two obs type at one index, as r-value only
   /// @param  i     index of the data of interest
   /// @param  type1 observation type (e.g. "P2") of the data of interest
   /// @param  type2 observation type (e.g. "C2") of the data of interest
   /// @return the SSI of the given type at the given index
   unsigned short SSI(unsigned int i, std::string type1, std::string type2)
      throw(Exception);

   /// Test whether the object has obstype type
   /// @return true if this obstype was passed to the c'tor (i.e. is in indexForLabel)
   inline bool hasType(std::string type) throw()
   {
      return (indexForLabel.find(type) != indexForLabel.end());
   }

   /// Access the obstypes (as strings)
   std::vector<std::string> getObstypes(void) {
      std::vector<std::string> ots;
      for(int i=0; i<labelForIndex.size(); i++)
         ots.push_back(labelForIndex[i]);
      return ots;
   }

   // -------------------------------- utils ---------------------------------
   /// clear the data (but not the obs types) from the arrays
   void clear(void) throw() { spdvector.clear(); }

   /// compute the timetag associated with index i in the data array
   /// @param  i   index of the data of interest
   /// @return the time tag at the given index.
   Epoch time(unsigned int i) const throw(Exception);

   /// compute the index to which the input time tt is closest;
   /// @param tt the time tag of interest
   /// @return -1 if not within the time limits of the SatPass, else index of tt
   int index(const Epoch& tt) const throw(Exception)
   {
      int count = countForTime(tt);
      if(count < 0) return -1;
      for(int i=0; i<spdvector.size(); i++)
         if(count == spdvector[i].ndt) return i;
      return -1;
   }

   /// substitute obstype labels - used e.g. to ignore C1/P1 differences
   /// Replace all instances of OT with subs[OT], if subst[OT] exists.
   /// the input map must NOT have circularities: e.g. <C1,P1>, <P1,Q1>
   void renameObstypes(std::map<std::string, std::string>& subst)
      throw(Exception)
   {
      unsigned int i;
      std::map<std::string, std::string>::const_iterator it = subst.begin();
      std::map<unsigned int,std::string>::iterator jt;
      while(it != subst.end()) {
         if(indexForLabel.find(it->first) == indexForLabel.end()) continue;

         i = indexForLabel[it->first];
         indexForLabel.insert(
            std::map<std::string,unsigned int>::value_type(it->second,i));
         indexForLabel.erase(it->first);

         for(jt=labelForIndex.begin(); jt!=labelForIndex.end(); ++jt) {
            if(jt->second != it->first) continue;
            i = jt->first;        
            labelForIndex.erase(i);
            labelForIndex.insert(
               std::map<unsigned int,std::string>::value_type(i,it->second));
         }

         ++it;
      }
   }

   // sorting ----------------------------------------------------------
   /// 'less than' is required for sort() and map<SatPass,...>.find(SatPass)
   bool operator<(const SatPass& right) const
   {
      if(firstTime == right.firstTime)
         return (sat < right.sat);
      return firstTime < right.firstTime;
   }

   // edit -------------------------------------------------------------
   /// return true if the given timetag is or could be part of this pass
   /// @param tt        the time tag of interest
   /// @return true if the given time tag lies within the time interval covered
   /// by this object.
   bool includesTime(const Epoch& tt) const throw();

   /// Truncate all data at and after the given time.
   /// return -1 if ttag is at or before the start of this pass,
   /// return +1 if ttag is at or after the end of this pass,
   /// else return 0
   int trimAfter(const Epoch ttag) throw(Exception);

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
   /// @param refTime Reference Epoch for the decimation, default is to use first
   ///                  in pass
   void decimate(const int N, Epoch refTime=CommonTime::BEGINNING_OF_TIME)
      throw(Exception);

   // compare ----------------------------------------------------------
   /// Determine if there is overlap between data in this SatPass and another,
   /// that is the time limits overlap. If pointers are defined, return:
   /// pdelt: the time in seconds of the overlap,
   /// pttb: begin time of the overlap
   /// ptte: end time of the overlap
   bool hasOverlap(const SatPass& that, double *pdelt=NULL,
                   Epoch *pttb=NULL, Epoch *ptte=NULL) throw()
   {
      if(lastTime <= that.firstTime)                           // iiiiii  aaaaaaa
         return false;
      if(that.lastTime <= firstTime)                           // aaaaaa  iiiiiii
         return false;
      if(that.firstTime >= firstTime) {                        // iiiiiiii
         if(lastTime <= that.lastTime) {                       //     aaaaaaaaaa
            if(pdelt) *pdelt = lastTime - that.firstTime;
            if(pttb) *pttb = that.firstTime;
            if(ptte) *ptte = lastTime;
         }
         else {                                                // iiiiiiiiiiii
            if(pdelt) *pdelt = that.lastTime - that.firstTime; //    aaaaa
            if(pttb) *pttb = that.firstTime;
            if(ptte) *ptte = that.lastTime;
         }
         return true;
      }
      else {  // if(firstTime > that.firstTime)                //    iiii
         if(that.lastTime >= lastTime) {                       // aaaaaaaaa
            if(pdelt) *pdelt = lastTime - firstTime;
            if(pttb) *pttb = firstTime;
            if(ptte) *ptte = lastTime;
         }
         else {                                                //    iiiiiiiiii
            if(pdelt) *pdelt = that.lastTime - firstTime;      // aaaaaaaaa
            if(pttb) *pttb = firstTime;
            if(ptte) *ptte = that.lastTime;
         }
         return true;
      }
      return true;      // never reached
   }

   /// Determine if there is common-view between this SatPass and another, that is
   /// if the satellites match and the time limits overlap. Return the time in
   /// seconds of the overlap.
   bool hasCommonView(const SatPass& that, double *pdelt=NULL,
                                    Epoch *pttb=NULL, Epoch *ptte=NULL) throw()
   {
      if(sat != that.sat) return false;
      return hasOverlap(that, pdelt, pttb, ptte);
   }

   // analysis - also see SatPassUtilities -----------------------------
   /// compute the GLO channel
   /// start at n, then set n before returning; return false if failure
   /// challenge is at low elevation, L1 is slightly better than L2, but need both
   /// return true if successful, false if failed; also return string msg, which is
   /// FINAL sat n wk sow(beg) wk sow(end) npt stddev slope sl/std stddev(slope) [??]
   /// NB if "??" appears at end of msg, results questionable (stddev(slope) is high)
   bool getGLOchannel(int& n, std::string& msg) throw(Exception);

   /// Smooth pseudorange and debias phase, by computing the best estimate of the
   /// range-minus-phase over the whole pass, and subtracting this bias from the raw
   /// phase. NB. remove an INTEGER (cycles) to get "debiased phase" but a REAL (m)
   /// to get "smoothed pseudorange"; thus smoothed pseudorange != debiased phase.
   /// This routine can be called ONLY if dual frequency range and phase data
   /// (C/P1,C/P2,L1,L2) have been stored, and should be called ONLY after ALL
   /// cycleslips have been removed.
   /// @param  smoothPR  if true, replace the pseudorange (P1 and P2) with smoothed
   ///                   ranges (= phase + REAL estimate of range-minus-phase in m).
   /// @param  smoothPH  if true, replace the phase (L1 and L2) with debiased phase
   ///                   (= phase + INTEGER estimate of range-minus-phase in cycles).
   /// @param  msg       a string containing a single-line statistical summary of
   ///                   the smoothing results, and the estimated phase biases.
   /// @param  wl1       wavelength of L1 for this data
   /// @param  wl2       wavelength of L2 for this data
   ///  This string consists of the following 18 fields separated by whitespace.
   ///  1) "SMT", 2) satellite id (e.g. G07), 3) start GPS week, 4) start GPS
   ///  seconds of week, 5) end GPS week, 6) end GPS seconds of week,
   ///  (7-11 are for the L1 bias data in units of meters)
   ///  7) number, 8) average, 9) std deviation, 10) minimum, and 11) maximum,
   ///  (12-16 are for the L2 bias data in units of meters)
   ///  12) number, 13) average, 14) std deviation, 15) minimum, and 16) maximum,
   ///  17) the L1 bias in cycles, 18) the L2 bias in cycles.
   void smooth(const bool smoothPR, const bool smoothPH, std::string& msg,
            const double& wl1=L1_WAVELENGTH_GPS, const double& wl2=L2_WAVELENGTH_GPS)
      throw(Exception);

   // output -----------------------------------------------------------
   /// Dump one line summary of the pass; do not put linefeed at end
   /// @param msg  put message msg1 at beginning of line
   /// @param fmt  use time format
   /// @return string containing summary, without LF at end.
   std::string toString(std::string msg="",
      std::string fmt="%04Y/%02m/%02d %02H:%02M:%06.3f = %04F %w %10.3g") throw()
   {
      std::ostringstream os;
      os << msg << " " << sat << " N " << std::setw(4) << size()
         << " good " << std::setw(4) << ngood
         << " times " << printTime(getFirstTime(),fmt)
         << " to " << printTime(getLastTime(),fmt)
         << " obs:";
      std::vector<std::string> ots = getObstypes();
      for(int i=0; i<ots.size(); i++) os << " " << ots[i];
      return os.str();
   }
   /// Dump all the data in the pass, one line per timetag.
   /// @param os    output stream
   /// @param msg1  put message msg1 at beginning of each line,
   /// @param msg2  put msg2 at the end of the first (#comment) line.
   void dump(std::ostream& os, std::string msg1, std::string msg2=std::string())
      throw();

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

   /// format string, as defined in class Epoch, for output of times
   /// used by smooth (debug), dump and operator<<
   static int outRound;
   static std::string outFormat;
   static std::string longfmt;

protected:
   /// compute the count associated with the time tt
   /// @param tt the time tag of interest
   int countForTime(const Epoch& tt) const throw(Exception)
   {
      return int((tt-firstTime)/dt + 0.5);
   }

}; // end class SatPass

/// Stream output for SatPass.
/// @param os output stream to write to
/// @param sp SatPass to write
/// @return reference to os.
std::ostream& operator<<(std::ostream& os, gpstk::SatPass& sp);

}  // end namespace gpstk

// -----------------------------------------------------------------------------------
#endif // GPSTK_SATELLITE_PASS_INCLUDE
