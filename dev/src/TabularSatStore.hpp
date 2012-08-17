#pragma ident "$Id$"

/// @file TabularSatStore.hpp
/// Store a tabular list of data (for example precise ephemeris data from an SP3 file)
/// for several satellites, and compute values at any timetag from this table.

#ifndef GPSTK_TABULAR_SAT_STORE_INCLUDE
#define GPSTK_TABULAR_SAT_STORE_INCLUDE

#include <map>
#include <iostream>
#include <cmath>

#include "Exception.hpp"
#include "SatID.hpp"
#include "CommonTime.hpp"
#include "TimeString.hpp"
#include "Xvt.hpp"
#include "CivilTime.hpp"     

namespace gpstk
{

   /** @addtogroup ephemstore */
   //@{

   /// Store a table of data vs time for each of several satellites.
   /// The data are stored as DataRecords, one for each satellite,time.
   /// The getValue(sat, t) routine interpolates the table for sat at time t and
   /// returns the result as a DataRecord object.
   /// NB this is an abstract class b/c getValue() and others are pure virtual.
   /// NB this class (dump()) requires that operator<<(DataRecord) be defined.
   template <class DataRecord>
   class TabularSatStore
   {

   // member data
   protected:

      // the data tables
      /// std::map with key=CommonTime, value=DataRecord
      typedef std::map<CommonTime, DataRecord> DataTable;

      /// std::map with key=SatID, value=DataTable
      typedef std::map<SatID, DataTable> SatTable;

      /// the data tables: std::map<SatID, std::map<CommonTime, DataRecord> >
      SatTable tables;

      /// Time system of tables; default and initial value is TimeSystem::Any.
      /// Set and maintained by derived classes, using set and checkTimeSystem();
      /// Any call to add records with a specific system sets it; then other
      /// add.. or get.. calls will throw if the time systems do not match.
      TimeSystem storeTimeSystem;

      /// Flags indicating that various data are present and may be accessed with
      /// getValue(t) or other routines, via interpolation of the data tables.
      /// Defaults are false, but deriving class should define in c'tor.
      bool havePosition, haveVelocity, haveClockBias, haveClockDrift;

      /// Flag to check for data gaps (default false).
      /// If this flag is enabled, data gaps wider than member data gapInterval
      /// will throw an InvalidRequest exception in getValue() methods.
      bool checkDataGap;

      /// Smallest time interval (seconds) that constitutes a data gap.
      double gapInterval;

      /// Flag to check the length of available interpolation interval (default false)
      /// If this flag is enabled, interpolation intervals shorter than member data
      /// maxInterval will throw an InvalidRequest exception in getValue() methods.
      bool checkInterval;

      /// Maximum total time interval (seconds) allowed for interpolation.
      /// For example, with dt=900s and 10-point Lagrange interpolation, this should
      /// be (10-1)*900s+1= 8101s.
      double maxInterval;

      /// Flag to allow pick more epochs if one side without enough data(default false)
      /// Enable this option, we can process daily data with loading a single sp3 file
      /// but not three, that will save both memory and loading time especially when
      /// we use 5 seconds clock data from CODE.
      bool smartMode;

      typedef typename DataTable::const_iterator DataTableIterator;

   // member functions
   public:

      /// Default constructor
      TabularSatStore() throw()
         : havePosition(false), haveVelocity(false),
           haveClockBias(false), haveClockDrift(false),
           checkDataGap(false), checkInterval(false),
           smartMode(false),
           storeTimeSystem(TimeSystem::Any)
      {}

      /// Destructor
      virtual ~TabularSatStore() {};

      /// Return data value for the given satellite at the given time (usually via
      /// interpolation of the data table).
      /// @param[in] sat the SatID of the satellite of interest
      /// @param[in] ttag the time (CommonTime) of interest
      /// @return object of type DataRecord containing the data value(s).
      /// @throw InvalidRequest if data value cannot be computed, for example because
      ///  a) the time t does not lie within the time limits of the data table
      ///  b) checkDataGap is true and there is a data gap
      ///  c) checkInterval is true and the interval is larger than maxInterval
      /// NB this function is pure virtual, making the class abstract.
      ///
      /// Derived objects can implement similar routines, for example:
      /// Triple getPosition(const SatID& sat,const CommonTime& t) throw(InvalidRequest);
      /// Triple getVelocity(const SatID& sat,const CommonTime& t) throw(InvalidRequest);
      /// Triple getAccel(const SatID& sat,const CommonTime& t) throw(InvalidRequest);
      /// double getClockBias(const SatID& s,const CommonTime& t) throw(InvalidRequest);
      /// double[2] getClock(const SatID& sat,const CommonTime& t) throw(InvalidRequest);
      /// NB Xvt getXvt(const SatID& sat,const CommonTime& t) throw(InvalidRequest);
      ///   will be provided by another class which inherits this one.
      ///
      virtual DataRecord getValue(const SatID& sat, const CommonTime& ttag)
         const throw(InvalidRequest) = 0;

      /// Locate the given time in the DataTable for the given satellite.
      /// Return two const iterators it1 and it2 (it1 < it2) giving the range of
      /// 2*nhalf points, nhalf on each side of the given time.
      /// Note that a range is returned even if the input time exactly matches one
      /// of the times in the table; in this 'exact match' case the matching time
      /// will be at either it1->first (if input parameter exactReturn is true) or
      /// (it1+nhalf-1) or (it1+nhalf) (if exactReturn is false).
      /// This routine is used to select data from the table for interpolation;
      /// note that DataTable is a map<CommonTime, DataRecord>.
      /// @param[in] sat satellite of interest
      /// @param[in] ttag time of interest, e.g. where interpolation will be conducted
      /// @param[in] nhalf number of table points desired on each side of ttag
      /// @param it1 const reference to const_iterator, points to the interval begin
      /// @param it2 const reference to const_iterator, points to the interval end
      /// @param[in] exactReturn if true and exact match is found, return immediately,
      ///     with the matching time at it1 (== it1->first) [default is true].
      /// @return bool: true if ttag matches a time in the table and exactReturn was
      ///     true, then the matching time is at it1 (it2 is undefined);
      ///     if exactReturn was false, then the range (it1,it2) is valid and the
      ///     matching time is at either (it1+nhalf-1) or (it1+nhalf).
      /// @throw the satellite is not found in the tables, or there is inadequate data
      /// @throw GapInterval is set and there is a data gap larger than the max
      /// @throw MaxInterval is set and the interval is too wide
      virtual bool getTableInterval_old(const SatID& sat,
                                    const CommonTime& ttag,
                                    const int& nhalf,
                                    typename DataTable::const_iterator& it1,
                                    typename DataTable::const_iterator& it2,
                                    bool exactReturn=true)
         const throw(InvalidRequest)
      {
      try {
         // find the DataTable for this sat
         typename std::map<SatID, DataTable>::const_iterator satit;
         satit = tables.find(sat);
         if(satit == tables.end()) {
            InvalidRequest e("Satellite " + gpstk::StringUtils::asString(sat)
                           + " not found.");
            GPSTK_THROW(e);
         }

         // this is the data table for the sat
         const DataTable& dtable(satit->second);
         static const char *fmt=" at time %4Y/%02m/%02d %2H:%02M:%02S";

         // find the timetag in this table
         // NB. throw here if time systems do not match and are not "Any"
         it1 = dtable.find(ttag);
         // is it an exact match?
         bool exactMatch(it1 != dtable.end());

         // user must decide whether to return with exact value; e.g. without
         // velocity data, user needs the interval to compute v from x data
         if(exactMatch && exactReturn) return true;
         
         // lower_bound points to the first element with key >= ttag
         it1 = dtable.lower_bound(ttag);
         
         // Attention:
         // The following piece of code with logic problem and take care!(Wei Yan)

         // at table begin && exact match && interval of only 2: shift up
         if(it1 == dtable.begin() && exactMatch && nhalf==1) {
            ++it1;
         }
         else if(it1 == dtable.begin() || --it1 == dtable.begin()) {
            InvalidRequest e("Inadequate data before requested time for satellite "
               + gpstk::StringUtils::asString(sat) + printTime(ttag,fmt));
            GPSTK_THROW(e);
         }

         it2 = it1;
         if(it2 == dtable.end() || ++it2 == dtable.end()) {
            InvalidRequest e("Inadequate data after requested time for satellite "
               + gpstk::StringUtils::asString(sat) + printTime(ttag,fmt));
            GPSTK_THROW(e);
         }

         // we now have it1->first <= ttag < it2->first and it2 == ++it1

         // check for gap between these two table entries surrounding ttag
         if(checkDataGap && (it2->first-it1->first) > gapInterval) {
            InvalidRequest e("Gap at interpolation time for satellite "
               + gpstk::StringUtils::asString(sat) + printTime(ttag,fmt));
            GPSTK_THROW(e);
         }

         // now expand the interval to include 2*nhalf timesteps; watch for gaps
         typename DataTable::const_iterator kt;
         for(int k=0; k<nhalf-1; k++) {
            kt = it1;
            it1--;
            if(it1 == dtable.begin() && k < nhalf-2) { // k==nhalf-2 on last iter.
               InvalidRequest e("Inadequate data before requested time for satellite "
                  + gpstk::StringUtils::asString(sat) + printTime(ttag,fmt));
               GPSTK_THROW(e);
            }

            kt = it2;
            it2++;
            if(it2 == dtable.end()) {        // at end of table
               if(exactMatch && k==nhalf-2 && it1 != dtable.begin()) {
                  // exact match && at end of interval && with room to move down
                  it2--; it1--;  // move interval down by one
               }
               else {
                  InvalidRequest e(
                     "Inadequate data after requested time for satellite "
                     + gpstk::StringUtils::asString(sat) + printTime(ttag,fmt));
                  GPSTK_THROW(e);
               }
            }
         }

         // check that the interval is not too large
         if(checkInterval && (it2->first - it1->first) > maxInterval) {
            InvalidRequest e("Interpolation interval too large for satellite "
               + gpstk::StringUtils::asString(sat) + printTime(ttag,fmt));
            GPSTK_THROW(e);
         }

         return exactMatch;
      }
      catch(InvalidRequest& ir) { GPSTK_RETHROW(ir); }
      }

      /// Locate the given time in the DataTable for the given satellite.
      /// Return two const iterators it1 and it2 (it1 < it2) giving the range of
      /// 2*nhalf points.
      /// Note that a range is returned even if the input time exactly matches one
      /// of the times in the table; in this 'exact match' case the matching time
      /// will be at either it1->first (if input parameter exactReturn is true) or
      /// (it1+nhalf-1) or (it1+nhalf) (if exactReturn is false).
      /// This routine is used to select data from the table for interpolation;
      /// note that DataTable is a map<CommonTime, DataRecord>.
      /// @param[in] sat satellite of interest
      /// @param[in] ttag time of interest, e.g. where interpolation will be conducted
      /// @param[in] nhalf number of table points desired on each side of ttag
      /// @param it1 const reference to const_iterator, points to the interval begin
      /// @param it2 const reference to const_iterator, points to the interval end
      /// @param[in] exactReturn if true and exact match is found, return immediately,
      ///     with the matching time at it1 (== it1->first) [default is true].
      /// @return bool: true if ttag matches a time in the table and exactReturn was
      ///     true, then the matching time is at it1 (it2 is undefined);
      ///     if exactReturn was false, then the range (it1,it2) is valid and the
      ///     matching time is at either (it1+nhalf-1) or (it1+nhalf).
      /// @throw the satellite is not found in the tables, or there is inadequate data
      /// @throw GapInterval is set and there is a data gap larger than the max
      /// @throw MaxInterval is set and the interval is too wide
      virtual bool getTableInterval(const SatID& sat,
                                    const CommonTime& ttag,
                                    const int& nhalf,
                                    typename DataTable::const_iterator& it1,
                                    typename DataTable::const_iterator& it2,
                                    bool exactReturn=true)
          const throw(InvalidRequest)
      {
          try {
              // find the DataTable for this sat
              typename std::map<SatID, DataTable>::const_iterator satit;
              satit = tables.find(sat);
              if(satit == tables.end()) {
                  InvalidRequest ir("Satellite " + gpstk::StringUtils::asString(sat)
                      + " not found.");
                  GPSTK_THROW(ir);
              }

              // this is the data table for the sat
              const DataTable& dtable(satit->second);
              static const char *fmt=" at time %4Y/%02m/%02d %2H:%02M:%02S";

              // find the timetag in this table
              // NB. throw here if time systems do not match and are not "Any"
              it1 = dtable.find(ttag);
              // is it an exact match?
              bool exactMatch(it1 != dtable.end());

              // user must decide whether to return with exact value; e.g. without
              // velocity data, user needs the interval to compute v from x data
              if(exactMatch && exactReturn) return true;

              // lower_bound points to the first element with key >= ttag
              it1 = it2 = dtable.lower_bound(ttag);

              // Should we allow to predict data?
              if(it1 == dtable.end()) {
                  InvalidRequest ir("No data for the requested time of satellite "
                      + gpstk::StringUtils::asString(sat) + printTime(ttag,fmt));
                  GPSTK_THROW(ir);  
              }
              
              if(dtable.size() < 2*nhalf){
                  InvalidRequest ir("Inadequate data for satellite "
                      + gpstk::StringUtils::asString(sat) + printTime(ttag,fmt));
                  GPSTK_THROW(ir); 
              }

              // check for gap only when exactMatch==false
              if(!exactMatch && checkDataGap){
                typename DataTable::const_iterator lit,rit;
                lit = rit = it2;
               
                if(rit!=dtable.begin()){
                    lit--;
                    if((rit->first-lit->first) > gapInterval) {
                        InvalidRequest ir("Gap at interpolation time for satellite "
                            + gpstk::StringUtils::asString(sat) + printTime(ttag,fmt));
                        GPSTK_THROW(ir);
                    }
                }
                else{
                    if((rit->first - ttag) > gapInterval){
                        InvalidRequest ir("It may lead to bad interpolation precision for satellite "
                            + gpstk::StringUtils::asString(sat) + printTime(ttag,fmt));
                        GPSTK_THROW(ir);
                    }
                }
              }
              
              // try to get the beginning of the window
              int ileft(nhalf);
              for(int i = 0; i < nhalf; i++){
                  if(it1 == dtable.begin()) break;
                  it1--;
                  ileft--;
              }

              // try to get the ending of the window
              int iright = nhalf - 1 + ileft;
              for(int i = 0; i < (nhalf-1+ileft); i++){
                  it2++;
                  if(it2 == dtable.end()){
                      it2--;
                      break;
                  }

                  iright--;
              }

              // adjust the beginning of the window again
              int ileft2 = iright; 
              for(int i = 0; i < iright; i++){
                  if(it1 == dtable.begin()) break;
                  it1--;
                  ileft2--;
              }
              
              // We have checked that there are enough data, so ileft2 should
              // always be zero.
              if(ileft2 > 0){
                  InvalidRequest ir("My god, it should never go here!!!");
                  GPSTK_THROW(ir);
              }

              if(!smartMode) {
                  if(ileft>0){
                    GPSTK_THROW(
                        InvalidRequest("Inadequate data before requested time for satellite"
                            + gpstk::StringUtils::asString(sat) + printTime(ttag,fmt)));
                  }
                  if(iright>0){
                    GPSTK_THROW(
                        InvalidRequest("Inadequate data after requested time for satellite"
                            + gpstk::StringUtils::asString(sat) + printTime(ttag,fmt)));
                  }
              }

              // check that the interval is not too large
              if(checkInterval && (it2->first - it1->first) > maxInterval) {
                  InvalidRequest ir("Interpolation interval too large for satellite "
                      + gpstk::StringUtils::asString(sat) + printTime(ttag,fmt));
                  GPSTK_THROW(ir);
              }

              return exactMatch;
          }
          catch(InvalidRequest& ir) { GPSTK_RETHROW(ir); }
      }


   // interface like that of XvtStore

      /// Dump information about the object to an ostream.
      /// @param[in] os ostream to receive the output; defaults to std::cout
      /// @param[in] detail integer level of detail to provide; allowed values are
      ///    0: number of satellites, time step and time limits, flags, and
      ///                         gap and interval flags and values
      ///    1: number of data/sat
      ///    2: above plus all the data tables
      virtual void dump(std::ostream& os = std::cout, int detail = 0) const throw()
      {
         os << " Dump of TabularSatStore(" << detail << "):" << std::endl;
         if(detail >= 0) {
            static const char *fmt="%4F %w %10.3g %4Y/%02m/%02d %2H:%02M:%02S %P";

            os << "  Data stored for " << nsats() << " satellites\n";
            os << "  Time span of data: ";
            CommonTime initialTime(getInitialTime()), finalTime(getFinalTime());
            if(initialTime == CommonTime::END_OF_TIME ||
               finalTime == CommonTime::BEGINNING_OF_TIME)
                  os << "(there are no time limits)" << std::endl;
            else
               os << " FROM " << printTime(initialTime,fmt) << " TO "
                  << printTime(finalTime,fmt) << std::endl;

            os << "  This store contains:"
               << (havePosition ? "":" not") << " position,"
               << (haveVelocity ? "":" not") << " velocity,"
               << (haveClockBias ? "":" not") << " clock bias, and"
               << (haveClockDrift ? "":" not") << " clock drift data." << std::endl;
            os << "  Checking for data gaps? " << (checkDataGap ? "yes":"no");
            if(checkDataGap) os << "; gap interval is "
               << std::fixed << std::setprecision(2) << gapInterval;
            os << std::endl;
            os << "  Checking data interval? " << (checkInterval ? "yes":"no");
            if(checkInterval) os << "; max interval is "
               << std::fixed << std::setprecision(2) << maxInterval;
            os << std::endl;

            if(detail > 0) {
               typename SatTable::const_iterator it;
               for(it=tables.begin(); it!=tables.end(); it++) {
                  os << "   Sat " << it->first << " : "
                     << it->second.size() << " records.";

                  if(detail == 1) { os << std::endl; continue; }

                  // NB cannot access DataRecord without operator<<(DataRecord)
                  os << "   Data:" << std::endl;
                  typename DataTable::const_iterator jt;
                  for(jt=it->second.begin(); jt!=it->second.end(); jt++)
                     os << " " << printTime(jt->first,fmt)
                        << " " << it->first << " ";
                     os << jt->second // NB requires operator<<(DataRecord)
                        << std::endl;
               }
            }
         }
         os << " End dump of TabularSatStore." << std::endl;
      }

      /// Edit the data tables, removing data outside the indicated time interval.
      /// @param[in] tmin defines the beginning of the time interval
      /// @param[in] tmax defines the end of the time interval
      void edit(const CommonTime& tmin,
                const CommonTime& tmax = CommonTime::END_OF_TIME) throw()
      {
         // loop over satellites
         typename SatTable::iterator it;
         for(it=tables.begin(); it!=tables.end(); it++) {
            //sat = it->first;
            DataTable& dtab(it->second);
            typename DataTable::iterator jt;

            // delete everything above tmax
            jt = dtab.upper_bound(tmax);     // jt points to the earliest time > tmax
            if(jt != dtab.end())
               dtab.erase(jt,dtab.end());

            // delete everything before tmin
            jt = dtab.lower_bound(tmin);     // jt points to the earliest time >= tmin
            if(jt != dtab.begin() && --jt != dtab.begin())
               dtab.erase(dtab.begin(),jt);
         }
      }

      // remaining functions are not virtual

      /// Remove all data and reset time limits
      inline void clear() throw()
         { tables.clear(); }

      /// Return true if the given SatID is present in the store
      virtual bool isPresent(const SatID& sat) const throw()
         { return (tables.find(sat) != tables.end()); }

      /// determine if the input TimeSystem conflicts with the stored TimeSystem
      /// @param ts TimeSystem to compare with stored TimeSystem
      /// @throw if time systems are inconsistent
      void checkTimeSystem(const TimeSystem& ts) const throw(InvalidRequest)
      {
         if(ts != TimeSystem::Any && storeTimeSystem != TimeSystem::Any
                                       && ts != storeTimeSystem)
         {
            InvalidRequest ir("Conflicting time systems: "
               + ts.asString() + " - " + storeTimeSystem.asString());
            GPSTK_THROW(ir);
         }
      }

      /// Get the earliest time of data in the data tables.
      /// @return the earliest time
      /// @throw InvalidRequest if the store is empty.
      CommonTime getInitialTime() const throw(InvalidRequest)
      {
         if(tables.size() == 0) {
            InvalidRequest e("Store is empty");
            GPSTK_THROW(e);
         }

         CommonTime initialTime(CommonTime::END_OF_TIME);

         // loop over satellites
         typename SatTable::const_iterator it;
         for(it=tables.begin(); it!=tables.end(); it++) {
            const DataTable& dtab(it->second);
            typename DataTable::const_iterator jt;
            // update new time limits
            if(dtab.begin() != dtab.end()) {
               jt = dtab.begin();
               if(jt->first < initialTime) initialTime = jt->first;
            }
         }

         return initialTime;
      }

      /// Get the latest time of data in the data tables.
      /// @return the latest time
      /// @throw InvalidRequest if the store is empty.
      CommonTime getFinalTime() const throw(InvalidRequest)
      {
         if(tables.size() == 0) {
            InvalidRequest e("Store is empty");
            GPSTK_THROW(e);
         }

         CommonTime finalTime(CommonTime::BEGINNING_OF_TIME);

         // loop over satellites
         typename SatTable::const_iterator it;
         for(it=tables.begin(); it!=tables.end(); it++) {
            const DataTable& dtab(it->second);
            typename DataTable::const_iterator jt;
            // update new time limits
            if(dtab.begin() != dtab.end()) {
               --(jt = dtab.end());
               if(jt->first > finalTime) finalTime = jt->first;
            }
         }

         return finalTime;
      }

   // end interface like that of XvtStore (also hasVelocity() and isPresent())

      /// Get the earliest time of data in the store for the given satellite.
      /// @return the first time.
      /// @throw InvalidRequest if there is no data in the store for this satellite.
      CommonTime getInitialTime(const SatID& sat) const throw(InvalidRequest)
      {
         if(tables.size() == 0) {
            InvalidRequest e("Store is empty");
            GPSTK_THROW(e);
         }
         typename SatTable::const_iterator it(tables.find(sat));
         if(it == tables.end()) {
            InvalidRequest e("Store has no data for satellite "
                                          + gpstk::StringUtils::asString(sat));
            GPSTK_THROW(e);
         }
         return (it->second.begin())->first;
      }

      /// Get the latest time of data in the store for the given satellite.
      /// @return the last time.
      /// @throw InvalidRequest if there is no data in the store for this satellite.
      CommonTime getFinalTime(const SatID& sat) const throw(InvalidRequest)
      {
         if(tables.size() == 0) {
            InvalidRequest e("Store is empty");
            GPSTK_THROW(e);
         }
         typename SatTable::const_iterator it(tables.find(sat));
         if(it == tables.end()) {
            InvalidRequest e("Store has no data for satellite "
                                          + gpstk::StringUtils::asString(sat));
            GPSTK_THROW(e);
         }
         typename DataTable::const_iterator jt(it->second.end());
         --jt;
         return jt->first;
      }

      /// Dump the data in a subset of the tables as defined by iterators.
      /// Note that the interval includes both it1 and it2.
      void dumpInterval(typename DataTable::const_iterator& it1,
                        typename DataTable::const_iterator& it2,
                        std::ostream& os = std::cout) const throw()
      {
         const char *fmt="%4Y/%02m/%02d %2H:%02M:%02S";
         typename DataTable::const_iterator it(it1);
         while(1) {
            os << " " << printTime(it->first,fmt)
               << " " << it->second << std::endl;
            if(it == it2) break;
            ++it;
         };
      }

      /// Does this store contain position, etc data stored in the tables?
      bool hasPosition() const throw() { return havePosition; }
      bool hasVelocity() const throw() { return haveVelocity; }
      bool hasClockBias() const throw() { return haveClockBias; }
      bool hasClockDrift() const throw() { return haveClockDrift; }

      /// Get number of satellites available
      inline int nsats(void) const throw() { return tables.size(); }

      /// Is the given satellite present?
      bool hasSatellite(const SatID& sat) const throw()
      { return isPresent(sat); }

      /// Get a list (std::vector) of SatIDs present in the store
      std::vector<SatID> getSatList(void) const throw()
      {
         std::vector<SatID> satlist;
         typename SatTable::const_iterator it;
         for(it=tables.begin(); it != tables.end(); ++it)
            if(it->second.size() > 0) satlist.push_back(it->first);
         return satlist;
      }

      /// Get the total number of data records in the store
      inline int ndata(void) const throw()
      {
         int n(0);
         typename SatTable::const_iterator sit;
         for(sit=tables.begin(); sit != tables.end(); ++sit)
            n += sit->second.size();
         return n;
      }

      /// Get the number of data records for the given sat
      inline int ndata(const SatID& sat) const throw()
      {
         typename SatTable::const_iterator it(tables.find(sat));
         if(it == tables.end()) return 0;
         else return it->second.size();
      }

      /// same as ndata()
      inline int size(void) const throw() { return ndata(); }

      /// compute the nominal timestep of the data table for the given satellite
      /// @return 0 if satellite is not found, else the nominal timestep in seconds.
      double nomTimeStep(const SatID& sat) const throw()
      {
         // get the table for this sat
         typename SatTable::const_iterator it(tables.find(sat));

         // not found or empty
         if(it == tables.end() || it->second.size() == 0) return 0.0;

         // save the most frequent N step sizes
         static const int N=3;
         int i,ndt[N]={0,0,0};
         double dt[N],del;

         // loop over the table
         typename DataTable::const_iterator jt(it->second.begin());
         CommonTime prevT(jt->first);
         ++jt;
         while(jt != it->second.end()) {
            del = jt->first - prevT;
            if(del > 1.0e-8) for(i=0; i<N; i++) {
               if(ndt[i] == 0) { dt[i] = del; ndt[i] = 1; break; }
               if(fabs(del-dt[i]) < 1.0e-8) { ndt[i]++; break; }
            }
            prevT = jt->first;
            ++jt;
         }

         // find the most frequent interval
         del = dt[0];
         for(i=1; i<N; i++) if(ndt[i] > ndt[0]) {
            del = dt[i];
            ndt[0] = ndt[i];
         }

         return del;
      }

      /// Is gap checking on?
      bool isDataGapCheck(void) throw() { return checkDataGap; }

      /// Disable checking of data gaps.
      void disableDataGapCheck(void) throw() { checkDataGap = false; }

      /// Get current gap interval.
      double getGapInterval(void) throw() { return gapInterval; }

      /// Set gap interval and turn on gap checking
      void setGapInterval(double interval) throw()
         { checkDataGap = true; gapInterval = interval; }

      /// Is interval checking on?
      bool isIntervalCheck(void) throw() { return checkInterval; }

      /// Disable checking of maximum interval.
      void disableIntervalCheck(void) throw() { checkInterval = false; }

      /// Get current maximum interval.
      double getMaxInterval(void) throw() { return maxInterval; }

      /// Set maximum interval and turn on interval checking
      void setMaxInterval(double interval) throw() {
         checkInterval = true;
         maxInterval = interval;
      }

      /// get the store's time system
      TimeSystem getTimeSystem(void) throw() { return storeTimeSystem; }

      /// set the store's time system
      void setTimeSystem(const TimeSystem& ts) throw() { storeTimeSystem = ts; }

      /// get the smartMode flag to select data from the table for interpolation
      bool getSmartMode(){ return smartMode; }

      /// set the smartMode flag to select data from the table for interpolation
      void setSmartMode(bool smart = true){ smartMode = smart; }
      
   };

      //@}

}  // End of namespace gpstk

#endif // GPSTK_TABULAR_SAT_STORE_INCLUDE
