#pragma ident "$Id$"

/// @file SP3EphemerisStore.cpp
/// Store a tabular list of position and clock bias (perhaps also velocity and clock
/// drift) data from SP3 file(s) for several satellites; access the tables to compute
/// values at any timetag, within the limits of the data, from this table via
/// interpolation.
/// An option allows assigning the clock store to RINEX clock files, with separate
/// timestep and interpolation algorithm.

#include <iostream>

#include "Exception.hpp"
#include "SatID.hpp"
#include "CommonTime.hpp"
#include "TimeString.hpp"
#include "StringUtils.hpp"

#include "SP3Stream.hpp"
#include "SP3Header.hpp"
#include "SP3Data.hpp"

#include "RinexClockStream.hpp"
#include "RinexClockHeader.hpp"
#include "RinexClockData.hpp"

#include "FileStore.hpp"
#include "ClockSatStore.hpp"
#include "PositionSatStore.hpp"

#include "SP3EphemerisStore.hpp"

using namespace std;

namespace gpstk
{
   using namespace StringUtils;

   /** @addtogroup ephemstore */
   //@{

   // XvtStore interface:

   // Returns the position and clock offset of the indicated
   // object in ECEF coordinates (meters) at the indicated time.
   // @param[in] sat the satellite of interest
   // @param[in] ttag the time to look up
   // @return the Xt of the object at the indicated time
   // @throw InvalidRequest If the request can not be completed for any
   //    reason, this is thrown. The text may have additional
   //    information as to why the request failed.
   Xt SP3EphemerisStore::getXt(const SatID& sat, const CommonTime& ttag)
      const throw(InvalidRequest)
   {
      try {
         PositionRecord prec(posStore.getValue(sat,ttag));
         ClockRecord crec(clkStore.getValue(sat,ttag));

         Xt retXt;
         for(int i=0; i<3; i++)
            retXt.x[i] = prec.Pos[i] * 1000.0;    // km -> m

         if(useSP3clock)                          // SP3
            retXt.clkbias = crec.bias * 1.e-6;    // microsec -> sec
         else                                     // RINEX clock
            retXt.clkbias = crec.bias;            // sec

         return retXt;
      }
      catch(InvalidRequest& e) { GPSTK_RETHROW(e); }
   }

   // Returns the position, velocity, and clock offset of the indicated
   // object in ECEF coordinates (meters) at the indicated time.
   // @param[in] sat the satellite of interest
   // @param[in] ttag the time to look up
   // @return the Xvt of the object at the indicated time
   // @throw InvalidRequest If the request can not be completed for any
   //    reason, this is thrown. The text may have additional
   //    information as to why the request failed.
   Xvt SP3EphemerisStore::getXvt(const SatID& sat, const CommonTime& ttag)
      const throw(InvalidRequest)
   {
      PositionRecord prec;
      ClockRecord crec;
      try { prec = posStore.getValue(sat,ttag); }
      catch(InvalidRequest& e) { GPSTK_RETHROW(e); }
      try { crec = clkStore.getValue(sat,ttag); }
      catch(InvalidRequest& e) { GPSTK_RETHROW(e); }

      try {
         Xvt retXvt;
         for(int i=0; i<3; i++) {
            retXvt.x[i] = prec.Pos[i] * 1000.0;    // km -> m
            retXvt.v[i] = prec.Vel[i] * 0.1;       // dm/s -> m/s
         }
         if(useSP3clock) {                            // SP3
            retXvt.clkbias = crec.bias * 1.e-6;       // microsec -> sec
            retXvt.clkdrift = crec.drift * 1.e-6;     // microsec/sec -> sec/sec
         }
         else {                                       // RINEX clock
            retXvt.clkbias = crec.bias;               // sec
            retXvt.clkdrift = crec.drift;             // sec/sec
         }

         // compute relativity correction, in seconds
         retXvt.computeRelativityCorrection();

         return retXvt;
      }
      catch(InvalidRequest& e) { GPSTK_RETHROW(e); }
   }

   // Determine the earliest time for which this object can successfully 
   // determine the Xvt for any object.
   // @return the earliest time in the table
   // @throw InvalidRequest if the object has no data.
   CommonTime SP3EphemerisStore::getInitialTime() const throw(InvalidRequest)
   {
      try {
         if(useSP3clock) return posStore.getInitialTime();

         CommonTime tc(clkStore.getInitialTime()), tp(posStore.getInitialTime());
         return (tc > tp ? tc : tp);
      }
      catch(InvalidRequest& e) { GPSTK_RETHROW(e); }
   }

   // Determine the latest time for which this object can successfully 
   // determine the Xvt for any object.
   // @return the latest time in the table
   // @throw InvalidRequest if the object has no data.
   CommonTime SP3EphemerisStore::getFinalTime() const throw(InvalidRequest)
   {
      try {
         if(useSP3clock) return posStore.getFinalTime();

         CommonTime tc(clkStore.getFinalTime()), tp(posStore.getFinalTime());
         return (tc > tp ? tp : tc);
      }
      catch(InvalidRequest& e) { GPSTK_RETHROW(e); }
   }

// end of XvtStore interface

   // Return the position for the given satellite at the given time
   // @param[in] sat the SatID of the satellite of interest
   // @param[in] ttag the time (CommonTime) of interest
   // @return Triple containing the position ECEF XYZ meters
   // @throw InvalidRequest if result cannot be computed, for example because
   //  a) the time t does not lie within the time limits of the data table
   //  b) checkDataGap is true and there is a data gap
   //  c) checkInterval is true and the interval is larger than maxInterval
   Triple SP3EphemerisStore::getPosition(const SatID sat, const CommonTime ttag)
      const throw(InvalidRequest)
   {
      try {
         PositionRecord prec;
         prec = posStore.getValue(sat,ttag);
         for(int i=0; i<3; i++)
            prec.Pos[i] *= 1000.0;    // km -> m
         return prec.Pos;
      }
      catch(InvalidRequest& e) { GPSTK_RETHROW(e); }
   }

   // Return the velocity for the given satellite at the given time
   // @param[in] sat the SatID of the satellite of interest
   // @param[in] ttag the time (CommonTime) of interest
   // @return Triple containing the velocity ECEF XYZ meters/second
   // @throw InvalidRequest if result cannot be computed, for example because
   //  a) the time t does not lie within the time limits of the data table
   //  b) checkDataGap is true and there is a data gap
   //  c) checkInterval is true and the interval is larger than maxInterval
   Triple SP3EphemerisStore::getVelocity(const SatID sat, const CommonTime ttag)
      const throw(InvalidRequest)
   {
      try {
         PositionRecord prec;
         prec = posStore.getValue(sat,ttag);
         for(int i=0; i<3; i++)
            prec.Vel[i] *= 0.1;    // dm/s -> m/s
         return prec.Vel;
      }
      catch(InvalidRequest& e) { GPSTK_RETHROW(e); }
   }

   // Get the earliest time of data in the store for the given satellite.
   // Return the first time
   // Throw InvalidRequest if there is no data
   CommonTime SP3EphemerisStore::getInitialTime(const SatID& sat)
      const throw(InvalidRequest)
   {
      try {
         if(useSP3clock) return posStore.getInitialTime(sat);

         CommonTime tc(clkStore.getInitialTime(sat)),tp(posStore.getInitialTime(sat));
         return (tc > tp ? tc : tp);
      }
      catch(InvalidRequest& e) { GPSTK_RETHROW(e); }
   }

   // Get the latest time of data in the store for the given satellite.
   // Return the latest time
   // Throw InvalidRequest if there is no data
   CommonTime SP3EphemerisStore::getFinalTime(const SatID& sat)
      const throw(InvalidRequest)
   {
      try {
         if(useSP3clock) return posStore.getFinalTime(sat);

         CommonTime tc(clkStore.getFinalTime(sat)), tp(posStore.getFinalTime(sat));
         return (tc > tp ? tp : tc);
      }
      catch(InvalidRequest& e) { GPSTK_RETHROW(e); }
   }


   // This is a utility routine used by the loadFile and loadSP3File routines.
   // Store position (velocity) and clock data from SP3 files in clock and position
   // stores. Also update the FileStore with the filename and SP3 header.
   void loadSP3Store(const string& filename,
                     FileStore<SP3Header>& fileStore,
                     ClockSatStore& clkStore,
                     PositionSatStore& posStore,
                     bool rejectBadPos, bool rejectBadClk,
                     bool rejectPredPos, bool rejectPredClk,
                     bool fillClockStore)
      throw(Exception)
   {
      try {
         // open the input stream
         SP3Stream strm(filename.c_str());
         if(!strm.is_open()) {
            Exception e("File " + filename + " could not be opened");
            GPSTK_THROW(e);
         }
         strm.exceptions(ios::failbit);
         //cout << "Opened file " << filename << endl;

         // declare header and data
         SP3Header head;

         // read the SP3 ephemeris header
         try {
            strm >> head;
         }
         catch(Exception& e) {
            e.addText("Error reading header of file " + filename);
            GPSTK_RETHROW(e);
         }
         //cout << "Read header" << endl; head.dump();

         // save in FileStore
         fileStore.addFile(filename, head);

         // posStore.addPositionRecord will set haveVelocity
         //posStore.haveVelocity = head.containsVelocity;

         // must define TabularSatStore::nominalTimeStep
         if(posStore.getTimeStep() == -1.0) {
            posStore.setTimeStep(head.epochInterval);
            if(fillClockStore) clkStore.setTimeStep(head.epochInterval);
         }
         else {
            // check consistency of multiple files.
            // NB ESA GLO clks are 5 minute while IGS GPS clks are 30 sec
            double DT(posStore.getTimeStep()),dt(head.epochInterval);
            if(DT != dt) {
               Exception e("Time step (" + asString(dt,2)+") of load file " + filename
                  + " is inconsistent with existing position data("
                  + asString(DT,2)+")");
               GPSTK_THROW(e);
            }
            if(fillClockStore) {
               DT = clkStore.getTimeStep();
               if(DT != dt) {
                  Exception e("Time step (" + asString(dt,2)+") of load file "
                     + filename + " is inconsistent with existing clock data("
                     + asString(DT,2)+")");
                  GPSTK_THROW(e);
               }
            }
         }

         // read data
         bool isC(head.version==SP3Header::SP3c);
         bool haveRec,goNext,haveP,haveV,haveEP,haveEV,predP,predC;
         int i;
         CommonTime ttag;
         SatID sat;
         SP3Data data;
         PositionRecord prec;
         ClockRecord crec;

         prec.Pos = prec.sigPos = prec.Vel = prec.sigVel = prec.Acc = prec.sigAcc
            = Triple(0,0,0);
         if(fillClockStore) {
            crec.bias = crec.drift = crec.sig_bias = crec.sig_drift = 0.0;
            crec.accel = crec.sig_accel = 0.0;
         }

         try {
            haveP = haveV = haveEP = haveEV = predP = predC = false;
            goNext = true;

            while(strm >> data) {
            //cout << "Read data " << data.RecType
            //<< " at " << printTime(data.time,"%Y %m %d %H %M %S") << endl;

               // The SP3 doc says that records will be in order....
               // use while to loop twice, if necessary: as soon as a RecType is
               // repeated, the current records are output, then the loop
               // returns to start filling the records again.
               while(1) {
                  if(data.RecType == '*') {                                // epoch
                     if(haveP || haveV) goNext = false;
                     else {
                        ttag = data.time;
                        goNext = true;
                     }
                  }
                  else if(data.RecType == 'P' && !data.correlationFlag) {  // P
                     if(haveP) goNext = false;
                     else {
               //cout << "P record: "; data.dump(cout);
                        sat = data.sat;
                        for(i=0; i<3; i++) {
                           prec.Pos[i] = data.x[i];                        // km
                           if(isC && data.sig[i]>=0)
                              prec.sigPos[i] = ::pow(head.basePV,data.sig[i]); // mm
                           else
                              prec.sigPos[i] = 0.0;
                        }

                        if(fillClockStore) {
                           crec.bias = data.clk;                           // microsec
                           if(isC && data.sig[3]>=0)                // picosec -> msec
                              crec.sig_bias = ::pow(head.baseClk,data.sig[3]) * 1.e-6;
                        }

                        if(data.orbitPredFlag) predP = true;
                        if(data.clockPredFlag) predC = true;

                        haveP = true;
                     }
                  }
                  else if(data.RecType == 'V' && !data.correlationFlag) {  // V
                     if(haveV) goNext = false;
                     else {
               //cout << "V record: "; data.dump(cout);
                        for(i=0; i<3; i++) {
                           prec.Vel[i] = data.x[i];                        // dm/s
                           if(isC && data.sig[i]>=0) prec.sigVel[i] =
                                          ::pow(head.basePV,data.sig[i]);  // 10-4mm/s
                           else
                              prec.sigVel[i] = 0.0;
                        }

                        if(fillClockStore) {
                           crec.drift = data.clk * 1.e-4; // 10-4micros/s -> micors/s
                           if(isC && data.sig[3]>=0)      // 10-4picos/s  -> micros/s
                              crec.sig_drift = ::pow(head.baseClk,data.sig[3])*1.e-10;
                        }

                        if(data.orbitPredFlag) predP = true;
                        if(data.clockPredFlag) predC = true;

                        haveV = true;
                     }
                  }
                  else if(data.RecType == 'P' && data.correlationFlag) {   // EP
                     if(haveEP) goNext = false;
                     else {
               //cout << "EP record: "; data.dump(cout);
                        for(i=0; i<3; i++)
                           prec.sigPos[i] = data.sdev[i];
                        if(fillClockStore)
                           crec.sig_bias = data.sdev[3] * 1.e-6;// picosec -> microsec

                        if(data.orbitPredFlag) predP = true;
                        if(data.clockPredFlag) predC = true;

                        haveEP = true;
                     }
                  }
                  else if(data.RecType == 'V' && data.correlationFlag) {   // EV
                     if(haveEV) goNext = false;
                     else {
               //cout << "EV record: "; data.dump(cout);
                        for(i=0; i<3; i++)
                           prec.sigVel[i] = data.sdev[i];                  // 10-4mm/s

                        if(fillClockStore)
                           crec.sig_drift = data.sdev[3]*1.0e-10;// 10-4ps/s->micros/s

                        if(data.orbitPredFlag) predP = true;
                        if(data.clockPredFlag) predC = true;

                        haveEV = true;
                     }
                  }
                  else {
               //cout << "other record (" << data.RecType << "):\n";
                     //data.dump(cout);
                     //throw?
                     goNext = true;
                  }

                  //cout << "goNext is " << (goNext ? "T":"F") << endl;
                  if(goNext) break;

                  if(rejectBadPos && (prec.Pos[0]==0.0 ||
                                      prec.Pos[1]==0.0 ||
                                      prec.Pos[2]==0.)) {
                     //cout << "Bad position" << endl;
                     haveP = false; // bad position record
                  }
                  else if(rejectBadClk && crec.bias >= 999999.) {
                     //cout << "Bad clock" << endl;
                     haveP = false; // bad clock record
                  }
                  else {
                     //cout << "Add rec: " << sat << " " << ttag << " " << prec<<endl;
                     if(!rejectPredPos || !predP)
                        posStore.addPositionRecord(sat,ttag,prec);
                     if(fillClockStore && (!rejectPredClk || !predC))
                        clkStore.addClockRecord(sat,ttag,crec);

                     // prepare for next
                     haveP = haveV = haveEP = haveEV = predP = predC = false;
                     prec.Pos = prec.Vel = prec.sigPos = prec.sigVel = Triple(0,0,0);
                     crec.bias = crec.drift = crec.sig_bias = crec.sig_drift = 0.0;
                  }

                  goNext = true;

               }  // end while loop (loop twice)
            }  // end read loop

            if(haveP || haveV) {
               if( rejectBadPos && (prec.Pos[0]==0.0 ||
                                    prec.Pos[1]==0.0 ||
                                    prec.Pos[2]==0.0) ) {
                  //cout << "Bad last rec: position" << endl;
                  haveP = false;  // bad position record
               }
               else if(rejectBadClk && crec.bias >= 999999.) {
                  //cout << "Bad last rec: clock" << endl;
                  haveP = false;  // bad clock record
               }
               else {
                  //cout << "Add last rec: "<< sat <<" "<< ttag <<" "<< prec << endl;
                  if(!rejectPredPos || !predP)
                     posStore.addPositionRecord(sat,ttag,prec);
                  if(fillClockStore && (!rejectPredClk || !predC))
                     clkStore.addClockRecord(sat,ttag,crec);
                  haveP = haveV = predP = predC = false;
               }
            }
         }
         catch(Exception& e) {
            e.addText("Error reading data of file " + filename);
            GPSTK_RETHROW(e);
         }

         // close
         strm.close();

      }
      catch(Exception& e) { GPSTK_RETHROW(e); }

   }

   // Load an SP3 ephemeris file; if the clock store uses RINEX clock files,
   // this routine will also accept that file type and load the data into the
   // clock store. This routine will may set the velocity, acceleration, bias
   // or drift 'have' flags.
   void SP3EphemerisStore::loadFile(const string& filename) throw(Exception)
   {
      try {
         // if using only SP3, simply read the SP3
         if(useSP3clock) {
            loadSP3Store(filename, SP3Files, clkStore, posStore,
                  rejectBadPosFlag, rejectBadClockFlag,
                  rejectPredPosFlag, rejectPredClockFlag, true);
            return;
         }

         // must determine what kind of file it is
         bool isSP3(true);
         {                    // decide if the file is SP3
            SP3Stream strm;
            // open
            try {
               strm.open(filename.c_str(),std::ios::in);
               if(!strm.is_open()) isSP3 = false;
               strm.exceptions(std::fstream::failbit);
            }
            catch(Exception& e) { isSP3 = false; }
            catch(std::exception& e) { isSP3 = false; }

            // read the header
            SP3Header header;
            try { strm >> header; }
            catch(Exception& e) { isSP3 = false; }
            catch(std::exception& e) { isSP3 = false; }

            strm.close();
         }

         // call the appropriate load routine
         if(isSP3) loadSP3File(filename);
         else      loadRinexClockFile(filename);

      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   // Load an SP3 ephemeris file; may set the velocity and acceleration flags.
   // If the clock store uses RINEX clock files, this ignores the clock data.
   void SP3EphemerisStore::loadSP3File(const std::string& filename)
      throw(Exception)
   {
      try {
         loadSP3Store(filename, SP3Files, clkStore, posStore,
               rejectBadPosFlag, rejectBadClockFlag,
               rejectPredPosFlag, rejectPredClockFlag, useSP3clock);
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   // Load a RINEX clock file; may set the 'have' bias and drift flags
   void SP3EphemerisStore::loadRinexClockFile(const std::string& filename)
      throw(Exception)
   {
      try {
         if(useSP3clock) useRinexClockData();

         // open the input stream
         RinexClockStream strm(filename.c_str());
         if(!strm.is_open()) {
            Exception e("File " + filename + " could not be opened");
            GPSTK_THROW(e);
         }
         strm.exceptions(std::ios::failbit);
         //cout << "Opened file " << filename << endl;

         // declare header and data
         RinexClockHeader head;
         RinexClockData data;

         // read the RINEX clock header
         try {
            strm >> head;
         }
         catch(Exception& e) {
            e.addText("Error reading header of file " + filename);
            GPSTK_RETHROW(e);
         }
         //cout << "Read header" << endl; head.dump();

         // save in FileStore
         clkFiles.addFile(filename, head);

         // must define time step
         int i, ndt[3]={-1,-1,-1};
         double dt[3], del;
         CommonTime prevTime(CommonTime::BEGINNING_OF_TIME);

         // read data
         try {
            while(strm >> data) {
               //data.dump(cout);

               if(data.datatype == std::string("AS")) {
                  // add this data
                  ClockRecord rec;
                  rec.bias = data.bias; rec.sig_bias = data.sig_bias,
                  rec.drift = data.drift; rec.sig_drift = data.sig_drift,
                  rec.accel = data.accel; rec.sig_accel = data.sig_accel;
                  clkStore.addClockRecord(data.sat, data.time, rec);

                  // count the timestep
                  if(prevTime == CommonTime::BEGINNING_OF_TIME)
                     prevTime = data.time;
                  del = data.time - prevTime;
                  if(del > 1.0e-6) {
                     for(i=0; i<3; ++i) {
                        if(ndt[i] < 0) { dt[i] = del; ndt[i]=1; break; }
                        if(fabs(del-dt[i]) < 1.e-6) { ndt[i]++; break; }
                     }
                  }
                  prevTime = data.time;
               }
            }
         }
         catch(Exception& e) {
            e.addText("Error reading data of file " + filename);
            GPSTK_RETHROW(e);
         }

         // compute the nominal timestep
         double nomTimeStep(dt[0]);
         for(i=1; i<3; i++) if(ndt[i] > ndt[0]) {     // find the most frequent dt
            nomTimeStep = dt[i];
            ndt[0] = ndt[i];
         }

         // set the timestep, unless it has already been set
         if(clkStore.getTimeStep() == -1.0) {
            clkStore.setTimeStep(nomTimeStep);
            //cout << "Set clock time step to " << nomTimeStep << endl;
         }
         else {
            // check consistency of multiple files.
            // NB ESA GLO clks are 5 minute while IGS GPS clks are 30 sec
            double DT(clkStore.getTimeStep());
            if(DT != nomTimeStep) {
               Exception e("Time step (" + asString(nomTimeStep,2)+") of load file "
                  + filename + " is inconsistent with existing clock data("
                  + asString(DT,2)+")");
               GPSTK_THROW(e);
            }
         }

         // close
         strm.close();

      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //@}

}  // End of namespace gpstk
