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

/// @file RinEdit.cpp
/// Read Rinex observation files (version 2 or 3) and edit them, writing the edited
/// data to a new RINEX file.

// system
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>

// GPSTK
#include "Exception.hpp"
#include "StringUtils.hpp"
#include "GNSSconstants.hpp"

#include "singleton.hpp"
#include "expandtilde.hpp"
#include "logstream.hpp"
#include "CommandLine.hpp"

#include "CommonTime.hpp"
#include "Epoch.hpp"
#include "TimeString.hpp"

#include "RinexSatID.hpp"
#include "RinexObsID.hpp"
#include "Rinex3ObsStream.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsData.hpp"

//------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------
string version(string("2.4 9/23/15 rev"));
// TD
// if reading a R2 file, allow obs types in cmds to be R2 versions (C1,etc)
// option to replace input with output?
// include optional fmt input for t in edit cmds - is this feasible?
// if given a 4-char OT and SV, check their consistency
// OK - test it.  implement DO - how? copy and edit, or clear and copy?
// OK - test it.  edit header when DS (alone) or DO appear ... how?
// how to handle aux header data if its first - OF not yet opened
// END TD

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// class to encapsulate editing commands
class EditCmd
{
public:
   enum CmdType
   {
      invalidCT=0,
      ofCT,
      daCT,
      doCT,
      dsCT,
      ddCT,
      sdCT,
      ssCT,
      slCT,
      bdCT,
      bsCT,
      blCT,
      bzCT,
      countCT
   };
   CmdType type;     // the type of this command
   RinexSatID sat;   // satellite
   RinexObsID obs;   // observation type
   CommonTime ttag;  // associated time tag
   int sign;         // sign +1,0,-1 meaning start, one-time, stop
   int idata;        // integer e.g. SSI or LLI
   double data;      // data e.g. bias value
   string field;     // OF file name

   EditCmd(void) : type(invalidCT) {}       // default constructor
   ~EditCmd(void) {}                      // destructor

      /** constructor from strings, i.e. parser e.g. "DA+","t" or
       * "BDp","SV,OT,t,s" */
   EditCmd(const string typestr, const string arg) throw(Exception);
      /// parse time from string
   bool parseTime(const string arg, CommonTime& ttag) throw();

      /// is it valid?
   inline bool isValid(void) throw()
   { return (type != invalidCT); }

      /// dump, with optional message at front
   string asString(string msg=string())
      throw(Exception);

}; // end class EditCmd


// used for sorting
class EditCmdLessThan
{
public:
   bool operator()(const EditCmd& ec1, const EditCmd& ec2)
   { return (ec1.ttag < ec2.ttag); }
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Object for command line input and global data
class Configuration : public Singleton<Configuration>
{
public:

      // Default and only constructor
   Configuration() throw() { setDefaults(); }

      // Create, parse and process command line options and user input
   int processUserInput(int argc, char **argv) throw();

      // Design the command line
   string buildCommandLine(void) throw();

      // Open the output file, and parse the strings used on the command line
      // return -4 if log file could not be opened
   int extraProcessing(string& errors, string& extras) throw();

      // Parse one of the vector<string> of edit cmd options
   void parseEditCmds(vector<string>& v, const string l, ostringstream& os) throw();

private:

      // Define default values
   void setDefaults(void) throw()
   {
      defaultstartStr = string("[Beginning of dataset]");
      defaultstopStr = string("[End of dataset]");
      beginTime = CivilTime(1980,1,6,0,0,0.0,TimeSystem::GPS).convertToCommonTime();
      endTime = CommonTime::END_OF_TIME;
      decimate = 0.0;
   
      help = verbose = outver2 = false;
      debug = -1;

      messHDdc = messHDda = false;
   }  // end Configuration::SetDefaults()

public:

// member data
   CommandLine opts;             // command line options and syntax page
   static const string prgmName; // program name
   string Title;                 // id line printed to screen and log

      // start command line input
   bool help, verbose, outver2;
   int debug;
   string cfgfile;

   vector<string> messIF, messOF; // RINEX obs file names - IF and OF args
   string InObspath,OutObspath;  // obs file path

      // times derived from --start and --stop
   string defaultstartStr,startStr;
   string defaultstopStr,stopStr;
   CommonTime beginTime,endTime,decTime;

   double decimate,timetol;      // decimate input data
   string logfile;               // output log file

      // added "mess" to each of these variables because they were
      // conflicting with precompiler macro definitions and enum names
      // and I couldn't be bothered to do a more intelligent renaming.

      // editing commands
   bool messHDdc,messHDda,messBZ;
   string messHDp,messHDr,messHDo,messHDa,messHDx,messHDm,messHDn,messHDt,messHDh,messHDj,messHDk,messHDl,messHDs;
   vector<string> messHDc,messDA,messDAm,messDAp,messDO,messDS,messDSp,messDSm,messDD,messDDp,messDDm;
   vector<string> messSD,messSS,messSL,messSLp,messSLm,messBD,messBDp,messBDm,messBS,messBL;

      // end of command line input

   string msg;
   ofstream logstrm;
   static const string calfmt,gpsfmt,longfmt;

      // handle commands
   vector<EditCmd> vecCmds, currCmds;
   Rinex3ObsStream ostrm;        // RINEX output

}; // end class Configuration

//------------------------------------------------------------------------------
// const members of Configuration
const string Configuration::prgmName = string("RinEdit");
const string Configuration::calfmt = string("%04Y/%02m/%02d %02H:%02M:%02S");
const string Configuration::gpsfmt = string("%4F %10.3g");
const string Configuration::longfmt = calfmt + " = " + gpsfmt + " %P";

//------------------------------------------------------------------------------
// prototypes
int initialize(string& errors) throw(Exception);
void fixEditCmdList(void) throw();
int processFiles(void) throw(Exception);
int processOneEpoch(Rinex3ObsHeader& Rhead, Rinex3ObsHeader& RHout,
                    Rinex3ObsData& Rdata, Rinex3ObsData& RDout)
   throw(Exception);
int executeEditCmd(const vector<EditCmd>::iterator& it, Rinex3ObsHeader& Rhead,
                   Rinex3ObsData& Rdata) throw(Exception);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int main(int argc, char **argv)
{
      // get the (single) instance of the configuration
   Configuration& C(Configuration::Instance());

   try {
      int iret;
      clock_t totaltime(clock());
      Epoch wallclkbeg;
      wallclkbeg.setLocalTime();

         // build title = first line of output
      C.Title = "# " + C.prgmName + ", part of the GPS Toolkit, Ver " + version
         + ", Run " + printTime(wallclkbeg,C.calfmt);
   
      for(;;) {
            // get information from the command line
            // iret -2 -3 -4
         if((iret = C.processUserInput(argc, argv)) != 0) break;

            // read stores, check input etc
         string errs;
         if((iret = initialize(errs)) != 0) {
            LOG(ERROR) << "------- Input is not valid: ----------\n" << errs
                       << "\n------- end errors -----------";
            break;
         }
         if(!errs.empty()) LOG(INFO) << errs;         // Warnings are here too

         processFiles();
            // iret = 0 = success
         iret = 0;

         break;      // mandatory
      }

      if(iret == 0) {
            // print elapsed time
         totaltime = clock()-totaltime;
         Epoch wallclkend;
         wallclkend.setLocalTime();
         ostringstream oss;
         oss << C.prgmName << " timing: processing " << fixed << setprecision(3)
             << double(totaltime)/double(CLOCKS_PER_SEC) << " sec, wallclock: "
             << setprecision(0) << (wallclkend-wallclkbeg) << " sec.";
         LOG(INFO) << oss.str();
      }

      return iret;
   }
   catch(FFStreamError& e) { cerr << "FFStreamError: " << e.what(); }
   catch(Exception& e) { cerr << "Exception: " << e.what(); }
   catch (...) { cerr << "Unknown exception.  Abort." << endl; }
   return 1;

}  // end main()

//------------------------------------------------------------------------------
// return -5 if input is not valid
int initialize(string& errors) throw(Exception)
{
   try
   {
      bool isValid(true);
      int j;
      size_t i;
      Configuration& C(Configuration::Instance());
      errors = string("");
      ostringstream ossE;

         // must have an input file and an output file
      if(C.messIF.size() == 0) {
         ossE << "Error : No valid input files have been specified.";
         isValid = false;
      }
      if(C.messOF.size() == 0) {
         ossE << "Error : No valid output files have been specified.";
         isValid = false;
      }

         // add path to filenames, and expand tilde (~)
      include_path(C.InObspath, C.messIF);

         // add path to all OF
         // also if first OF command has a timetag, remove it and make that the start time
      for(j=0, i=0; i<C.vecCmds.size(); ++i) {
         if(C.vecCmds[i].type == EditCmd::ofCT) {
            if(j == 0 && C.vecCmds[i].ttag != CommonTime::BEGINNING_OF_TIME) {
               if(C.beginTime < C.vecCmds[i].ttag)
                  C.beginTime = C.vecCmds[i].ttag;
               C.vecCmds[i].ttag = CommonTime::BEGINNING_OF_TIME;
            }
            j++;
            include_path(C.OutObspath, C.vecCmds[i].field);
               //LOG(VERBOSE) << "Full output file name is " << C.vecCmds[i].field;
         }
      }

         // ------ compute and save a reference time for decimation
      if(C.decimate > 0.0) {
            // TD what if beginTime == BEGINNING_OF_TIME ?
         C.decTime = C.beginTime;
         double s,sow(static_cast<GPSWeekSecond>(C.decTime).sow);
         s = int(C.decimate * int(sow/C.decimate));
         if(::fabs(s-sow) > 1.0) LOG(WARNING) << "Warning : decimation reference time "
                                              << "(--start) is not an even GPS-seconds-of-week mark.";
         C.decTime = static_cast<CommonTime>(
            GPSWeekSecond(static_cast<GPSWeekSecond>(C.decTime).week,0.0));
         LOG(DEBUG) << "Decimate, with final decimate ref time "
                    << printTime(C.decTime,C.longfmt) << " and step " << C.decimate;
      }

         // -------- save errors and output
      errors = ossE.str();
      stripTrailing(errors,'\n');
      replaceAll(errors,"\n","\n# ");

      if(!isValid) return -5;
      return 0;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end initialize()

//------------------------------------------------------------------------------
// Return 0 ok, >0 number of files successfully read, <0 fatal error
int processFiles(void) throw(Exception)
{
   try {
      Configuration& C(Configuration::Instance());
      C.beginTime.setTimeSystem(TimeSystem::GPS);
      C.endTime.setTimeSystem(TimeSystem::GPS);
      int iret,nfiles;
      size_t i,nfile;
      string tag;
      RinexSatID sat;
      ostringstream oss;

      for(nfiles=0,nfile=0; nfile<C.messIF.size(); nfile++) {
         Rinex3ObsStream istrm;
         Rinex3ObsHeader Rhead,RHout;  // use one header for input and output
         Rinex3ObsData Rdata,RDout;
         string filename(C.messIF[nfile]);

            // iret is set to 0 ok, or could not: 1 open file, 2 read header, 3 read data
         iret = 0;

            // open the file ------------------------------------------------
         istrm.open(filename.c_str(),ios::in);
         if(!istrm.is_open()) {
            LOG(WARNING) << "Warning : could not open file " << filename;
            iret = 1;
            continue;
         }
         else
            LOG(DEBUG) << "Opened input file " << filename;
         istrm.exceptions(ios::failbit);

            // read the header ----------------------------------------------
         LOG(INFO) << "Reading header...";
         try { istrm >> Rhead; }
         catch(Exception& e) {
            LOG(WARNING) << "Warning : Failed to read header: " << e.what()
                         << "\n Header dump follows.";
            Rhead.dump(LOGstrm);
            istrm.close();
            iret = 2;
            continue;
         }
         if(C.debug > -1) {
            LOG(DEBUG) << "Input header for RINEX file " << filename;
            Rhead.dump(LOGstrm);
         }
         // dump the obs types
         map<string,vector<RinexObsID> >::const_iterator kt;
         for(kt = Rhead.mapObsTypes.begin(); kt != Rhead.mapObsTypes.end(); kt++) {
            sat.fromString(kt->first);
            oss.str("");
            oss << "# Header ObsIDs " << sat.systemString3() //<< " " << kt->first
               << " (" << kt->second.size() << "):";
            for(i=0; i<kt->second.size(); i++) oss << " " << kt->second[i].asString();
            LOG(INFO) << oss.str();
         }

            // we have to set the time system of all the timetags using ttag from file
         vector<EditCmd>::iterator jt;
         for(jt=C.vecCmds.begin(); jt != C.vecCmds.end(); ++jt)
            jt->ttag.setTimeSystem(Rhead.firstObs.getTimeSystem());

            // -----------------------------------------------------------------
            // generate output header from input header and DO,DS commands
         bool mungeData(false);
         map<string, map<int,int> > mapSysObsIDTranslate;

         RHout = Rhead;
         vector<EditCmd>::iterator it(C.vecCmds.begin());
         while(it != C.vecCmds.end()) {
               // DO delete obs without sign
            if(it->type == EditCmd::doCT) { // no DO+ DO- so far && it->sign == 0)
                  // if the system is defined, delete only for that system
               string sys(asString(it->sat.systemChar()));

                  // loop over systems (short-circuit if sys is defined)
               map<string, vector<RinexObsID> >::iterator jt;
               for(jt=RHout.mapObsTypes.begin(); jt != RHout.mapObsTypes.end(); ++jt)
               {
                  if(sys != string("?") && sys != jt->first)
                     continue;
                     // must put system on it, default is GPS
                  RinexObsID obsid(jt->first + it->obs.asString());

                     // find the OT in the header map, and delete it
                  vector<RinexObsID>::iterator kt;
                  kt = find(jt->second.begin(), jt->second.end(), obsid);
                  if(kt == jt->second.end())                      // ObsID not found
                     continue;      // TD warning?

                     // delete it
                     //RHout.mapObsTypes[jt->first].erase(kt);
                  jt->second.erase(kt);
                  mungeData = true;
               }
            }

               // DS delete sat without sign and without time
            else if(it->type == EditCmd::dsCT && it->sign == 0
                    && it->ttag == CommonTime::BEGINNING_OF_TIME)
            {
               map<RinexSatID, vector<int> >::iterator jt;
               jt = RHout.numObsForSat.find(it->sat);
                  // if its there, delete it
               if(jt != RHout.numObsForSat.end()) RHout.numObsForSat.erase(jt);

               map<RinexSatID, int>::iterator kt;
               kt = RHout.glonassFreqNo.find(it->sat);
                  // if its there, delete it
               if(kt != RHout.glonassFreqNo.end()) RHout.glonassFreqNo.erase(kt);

                  // TD do for sysPhaseShift, ...?
            }

            ++it;                          // go to the next command

         }  // end loop over edit commands

            // if mapObsTypes has changed, must make a map of indexes for translation
            // mapSysObsIDTranslate[sys][input index] = output index
         if(mungeData) {
            map<string, vector<RinexObsID> >::iterator jt;
            for(jt = Rhead.mapObsTypes.begin(); jt != Rhead.mapObsTypes.end(); ++jt) {
               string sys(jt->first);
                  // TD what if entire sys is deleted? RHout[sys] does not exist
               vector<RinexObsID>::iterator kt;
               for(i=0; i < jt->second.size(); i++) {
                  kt = find(RHout.mapObsTypes[sys].begin(),
                            RHout.mapObsTypes[sys].end(), jt->second[i]);
                  mapSysObsIDTranslate[sys][i]
                     = (kt == RHout.mapObsTypes[sys].end()
                        ?  -1                                        // not found
                        : (kt - RHout.mapObsTypes[sys].begin())); // output index
               }
            }

               // dump it
            if(C.debug > -1) {
               for(jt = Rhead.mapObsTypes.begin(); jt != Rhead.mapObsTypes.end(); ++jt) {
                  string sys(jt->first);
                  oss.str("");
                  oss << "Translation map for sys " << sys;
                  for(i=0; i < jt->second.size(); i++)
                     oss << " " << i << ":" << mapSysObsIDTranslate[sys][i];
                  LOG(DEBUG) << oss.str();
               }
            }
         }

            // must use the header defined in input and stored in output stream.
         if(C.outver2)
            RHout.prepareVer2Write();
            // NB. header will be written by executeEditCmd
            // -----------------------------------------------------------------

            // loop over epochs ---------------------------------------------
         LOG(INFO) << "Reading observations...";
         while(1) {
            try { istrm >> Rdata; }
            catch(Exception& e) {
               LOG(WARNING) << " Warning : Failed to read obs data (Exception "
                            << e.getText(0) << "); dump follows.";
               Rdata.dump(LOGstrm,Rhead);
               istrm.close();
               iret = 3;
               break;
            }
            catch(std::exception& e) {
               Exception ge(string("Std excep: ") + e.what());
               GPSTK_THROW(ge);
            }
            catch(...) {
               Exception ue("Unknown exception while reading RINEX data.");
               GPSTK_THROW(ue);
            }

               // normal EOF
            if(!istrm.good() || istrm.eof()) { iret = 0; break; }

            LOG(DEBUG) << "";
            LOG(DEBUG) << " Read RINEX data: flag " << Rdata.epochFlag
                       << ", timetag " << printTime(Rdata.time,C.longfmt);

               // stay within time limits
            if(Rdata.time < C.beginTime) {
               LOG(DEBUG) << " RINEX data timetag " << printTime(C.beginTime,C.longfmt)
                          << " is before begin time.";
               continue;
            }
            if(Rdata.time > C.endTime) {
               LOG(DEBUG) << " RINEX data timetag " << printTime(C.endTime,C.longfmt)
                          << " is after end time.";
               break;
            }

               // decimate
            if(C.decimate > 0.0) {
               double dt(::fabs(Rdata.time - C.decTime));
               dt -= C.decimate * long(0.5 + dt/C.decimate);
               LOG(DEBUG) << "Decimate? dt = " << fixed << setprecision(2) << dt;
               if(::fabs(dt) > 0.25) {
                  LOG(DEBUG) << " Decimation rejects RINEX data timetag "
                             << printTime(Rdata.time,C.longfmt);
                  continue;
               }
            }

               // copy data to output
            RDout = Rdata;
            if(mungeData) {               // must edit RDout.obs
               RDout.obs.clear();
                  // loop over satellites -----------------------------
               Rinex3ObsData::DataMap::const_iterator kt;
               for(kt=Rdata.obs.begin(); kt!=Rdata.obs.end(); ++kt) {
                  sat = kt->first;
                  string sys(string(1,sat.systemChar()));
                  for(i=0; i<Rdata.obs[sat].size(); i++) {     // loop over data
                     if(mapSysObsIDTranslate[sys][i] > -1)
                        RDout.obs[sat].push_back(Rdata.obs[sat][i]);
                  }
               }  // end loop over sats
            }

               // apply editing commands, including open files, write out headers
            iret = processOneEpoch(Rhead, RHout, Rdata, RDout);
            if(iret < 0) break;
            if(iret > 0) continue;

               // write data out
            try { C.ostrm << RDout; }
            catch(Exception& e) { GPSTK_RETHROW(e); }

               // debug: dump the RINEX data objects input and output
            if(C.debug > -1) {
               LOG(DEBUG) << "INPUT data ---------------";
               Rdata.dump(LOGstrm,Rhead);
               LOG(DEBUG) << "OUTPUT data ---------------";
               RDout.dump(LOGstrm,Rhead);
            }

         }  // end while loop over epochs

            // clean up
         istrm.close();

            // failure due to critical error
         if(iret < 0) break;

         if(iret >= 0) nfiles++;

      }  // end loop over files

         // final clean up
      LOG(INFO) << " Close output file.";
      C.ostrm.close();

      if(iret < 0) return iret;

      return nfiles;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end processFiles()

//------------------------------------------------------------------------------
// return <0 fatal; >0 skip this epoch
int processOneEpoch(Rinex3ObsHeader& Rhead, Rinex3ObsHeader& RHout,
                    Rinex3ObsData& Rdata, Rinex3ObsData& RDout) throw(Exception)
{
   try {
      Configuration& C(Configuration::Instance());
      int iret(0);
      RinexSatID sat;
      CommonTime now(Rdata.time);         // TD what if its aux data w/o an epoch?

         // if aux header data, either output or skip
      if(RDout.epochFlag > 1) {           // aux header data
         if(C.messHDda) return 1;
         return 0;
      }

      else {                              // regular data
         vector<EditCmd>::iterator it, jt;
         vector<EditCmd> toCurr;

            // for cmds with ttag <= now either execute and delete, or move to current
         it = C.vecCmds.begin();
         while(it != C.vecCmds.end()) {
            if(it->ttag <= now || ::fabs(it->ttag - now) < C.timetol) {
               LOG(DEBUG) << "Execute vec cmd " << it->asString();
                  // delete one-time cmds, move others to curr and delete
               iret = executeEditCmd(it, RHout, RDout);
               if(iret < 0) return iret;              // fatal error

                  // keep this command on the current list
               if(iret > 0) toCurr.push_back(*it); // C.currCmds.push_back(*it);

                  // if this is a '-' cmd to be deleted, find matching '+' and delete
                  // note fixEditCmdList() forced every - to have a corresponding +
               if(iret == 0 && it->sign == -1) {
                  for(jt = C.currCmds.begin(); jt != C.currCmds.end(); ++jt)
                     if(jt->type==it->type && jt->sat==it->sat && jt->obs==it->obs)
                        break;
                  if(jt == C.currCmds.end()) GPSTK_THROW(Exception(
                    string("Execute failed to find + cmd matching ")+it->asString()));
                  C.currCmds.erase(jt);
               }

                  // remove from vecCmds
               it = C.vecCmds.erase(it);              // erase vector element
            }
            else
               ++it;
         }
      
            // apply current commands, deleting obsolete ones
         it = C.currCmds.begin();
         while(it != C.currCmds.end()) {
            LOG(DEBUG) << "Execute current cmd " << it->asString();
               // execute command; delete obsolete commands
            iret = executeEditCmd(it, RHout, RDout);
            if(iret < 0) return iret;

            if(iret == 0)
               it = C.currCmds.erase(it);              // erase vector element
            else
               ++it;
         }

         for(it = toCurr.begin(); it != toCurr.end(); ++it)
            C.currCmds.push_back(*it);
      }

      return 0;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end processOneEpoch()

//------------------------------------------------------------------------------
// return >0 to put/keep the command on the 'current' queue
// return <0 for fatal error
int executeEditCmd(const vector<EditCmd>::iterator& it, Rinex3ObsHeader& Rhead,
                                                        Rinex3ObsData& Rdata)
   throw(Exception)
{
   Configuration& C(Configuration::Instance());
   size_t i,j;
   string sys;
   vector<string> flds;
   vector<RinexSatID> sats;
   Rinex3ObsData::DataMap::const_iterator kt;
   vector<RinexObsID>::iterator jt;

   try {
      if(it->type == EditCmd::invalidCT) {
         LOG(DEBUG) << " Invalid command " << it->asString();
         return 0;
      }

         // OF output file --------------------------------------------------------
      else if(it->type == EditCmd::ofCT) {
            // close the old file
         if(C.ostrm.is_open()) { C.ostrm.close(); C.ostrm.clear(); }

            // open the new file
         C.ostrm.open(it->field.c_str(),ios::out);
         if(!C.ostrm.is_open()) {
            LOG(ERROR) << "Error : could not open output file " << it->field;
            return -1;
         }
         C.ostrm.exceptions(ios::failbit);

         LOG(INFO) << " Opened output file " << it->field << " at time "
                   << printTime(Rdata.time,C.longfmt);

            // if this is the first file, apply the header commands
         if(it->ttag == CommonTime::BEGINNING_OF_TIME) {
            Rhead.fileProgram = C.prgmName;
            if(!C.messHDp.empty()) Rhead.fileProgram = C.messHDp;
            if(!C.messHDr.empty()) Rhead.fileAgency = C.messHDr;
            if(!C.messHDo.empty()) Rhead.observer = C.messHDo;
            if(!C.messHDa.empty()) Rhead.agency = C.messHDa;
            if(!C.messHDj.empty()) Rhead.recNo = C.messHDj;
            if(!C.messHDk.empty()) Rhead.recType = C.messHDk;
            if(!C.messHDl.empty()) Rhead.recVers = C.messHDl;            
            if(!C.messHDs.empty()) Rhead.antNo = C.messHDs;
            if(!C.messHDx.empty()) {
               flds = split(C.messHDx,',');
                  // TD check n==3,doubles in initialize
               for(i=0; i<3; i++) Rhead.antennaPosition[i] = asDouble(flds[i]);
            }
            if(!C.messHDm.empty()) Rhead.markerName = C.messHDm;
            if(!C.messHDn.empty())
            {
               Rhead.markerNumber = C.messHDn;
               Rhead.valid |= Rinex3ObsHeader::validMarkerNumber;
            }
            if(!C.messHDt.empty()) Rhead.antType = C.messHDt;
            if(!C.messHDh.empty()) {
               flds = split(C.messHDh,',');   // TD check n==3,doubles in initialize
               for(i=0; i<3; i++) Rhead.antennaDeltaHEN[i] = asDouble(flds[i]);
            }
            if(C.messHDdc) {
               Rhead.commentList.clear();
               Rhead.valid ^= Rinex3ObsHeader::validComment;
            }
            if(C.messHDc.size() > 0) {
               for(i=0; i<C.messHDc.size(); i++)
                  Rhead.commentList.push_back(C.messHDc[i]);
               Rhead.valid |= Rinex3ObsHeader::validComment;
            }
         }

         Rhead.firstObs = Rdata.time;
         Rhead.valid &= ~Rinex3ObsHeader::validLastTime;    // turn off

            // write the header
         C.ostrm << Rhead;
         return 0;
      }

         // DA delete all ---------------------------------------------------------------
      else if(it->type == EditCmd::daCT) {
         switch(it->sign) {
            case 1: case 0:
               Rdata.numSVs = 0;                   // clear this data, keep the cmd
               Rdata.obs.clear();
               if(it->sign == 0) return 0;
               break;
            case -1:
               return 0;                           // delete the (-) command
               break;
         }
      }

         // DO delete obs type ----------------------------------------------------------
         // handled above where input is copied into output
      else if(it->type == EditCmd::doCT)
         return 0;

         // DS delete satellite ---------------------------------------------------------
      else if(it->type == EditCmd::dsCT) {
         if(it->sign == -1) return 0;                 // delete the (-) command
            // find the SV
         LOG(DEBUG) << " Delete sat " << it->asString();
         if(it->sat.id > 0) {
            kt = Rdata.obs.find(it->sat);
            if(kt != Rdata.obs.end())                // found the SV
               sats.push_back(kt->first);
            else
               LOG(DEBUG) << " Execute: sat " << it->sat << " not found in data";
         }
         else {
            sats.clear();
            for(kt=Rdata.obs.begin(); kt!=Rdata.obs.end(); ++kt)
               if(kt->first.system == it->sat.system)
                  sats.push_back(kt->first);
         }
         for(j=0; j<sats.size(); j++) {
            Rdata.obs.erase(sats[j]);                 // remove it erase map
            Rdata.numSVs--;                           // don't count it
         }
         if(it->sign == 0) return 0;                  // delete the one-time command
      }

         // -----------------------------------------------------------------------------
         // the rest require that we find satellite and obsid in Rdata.obs
      else {
         if(it->sign == -1) return 0;                 // delete the (-) command

         sys = asString(it->sat.systemChar());        // find the system

            // find the OT in the header map, and get index into vector
         jt = find(Rhead.mapObsTypes[sys].begin(),
                   Rhead.mapObsTypes[sys].end(), it->obs);
         if(jt == Rhead.mapObsTypes[sys].end()) {     // ObsID not found
               // TD message? user error: ask to delete one that's not there
            LOG(DEBUG) << " Execute: obstype " << it->obs << " not found in header";
            return 0;                                 // delete the cmd
         }

         i = (jt - Rhead.mapObsTypes[sys].begin());   // index into vector

            // find the sat
         if(it->sat.id > 0) {
            if(Rdata.obs.find(it->sat)==Rdata.obs.end()) { // sat not found
               LOG(DEBUG) << " Execute: sat " << it->sat << " not found in data";
            }
            else
               sats.push_back(it->sat);
         }
         else {
            for(kt=Rdata.obs.begin(); kt!=Rdata.obs.end(); ++kt) {
               if(kt->first.system == it->sat.system)
                  sats.push_back(kt->first);
            }
         }

         for(j=0; j<sats.size(); j++) {
            switch(it->type) {
                  // DD delete data -----------------------------------------------------
               case EditCmd::ddCT:
                  Rdata.obs[sats[j]][i].data = 0.0;
                  Rdata.obs[sats[j]][i].ssi = 0;
                  Rdata.obs[sats[j]][i].lli = 0;
                  break;
                     // SD set data --------------------------------------------------------
               case EditCmd::sdCT:
                  Rdata.obs[sats[j]][i].data = it->data;
                  break;
                     // SS set SSI ---------------------------------------------------------
               case EditCmd::ssCT:
                  Rdata.obs[sats[j]][i].ssi = it->idata;
                  break;
                     // SL set LLI ---------------------------------------------------------
               case EditCmd::slCT:
                  Rdata.obs[sats[j]][i].lli = it->idata;
                  break;
                     // BD bias data -------------------------------------------------------
               case EditCmd::bdCT:     // do not bias
                  if(Rdata.obs[sats[j]][i].data != 0.0 || C.messBZ)
                     Rdata.obs[sats[j]][i].data += it->data;
                  break;
                     // BS bias SSI --------------------------------------------------------
               case EditCmd::bsCT:
                  Rdata.obs[sats[j]][i].ssi += it->idata;
                  break;
                     // BL bias LLI --------------------------------------------------------
               case EditCmd::blCT:
                  Rdata.obs[sats[j]][i].lli += it->idata;
                  break;
                     // never reached ------------------------------------------------------
               default:
                     // message?
                  break;
            }
         }

         if(it->sign == 0) return 0;                  // delete the one-time command
      }

      return 1;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end executeEditCmd()

//------------------------------------------------------------------------------
int Configuration::processUserInput(int argc, char **argv) throw()
{
   string PrgmDesc,cmdlineUsage, cmdlineErrors, cmdlineExtras;
   vector<string> cmdlineUnrecognized;

      // build the command line
   opts.DefineUsageString(prgmName + " [options]");
   PrgmDesc = buildCommandLine();

      // let CommandLine parse options; write all errors, etc to the passed strings
   int iret = opts.ProcessCommandLine(argc, argv, PrgmDesc,
                                      cmdlineUsage, cmdlineErrors, cmdlineUnrecognized);

      // handle return values
   if(iret == -2) return iret;      // bad alloc
   if(iret == -3) return iret;      // invalid command line

      // help: print syntax page and quit
   if(opts.hasHelp()) {
      LOG(INFO) << cmdlineUsage;
      return 1;
   }

      // extra parsing (perhaps add to cmdlineErrors, cmdlineExtras)
   iret = extraProcessing(cmdlineErrors, cmdlineExtras);
   if(iret == -4) return iret;      // log file could not be opened

      // output warning / error messages
   if(cmdlineUnrecognized.size() > 0) {
      LOG(WARNING) << "Warning - unrecognized arguments:";
      for(size_t i=0; i<cmdlineUnrecognized.size(); i++)
         LOG(WARNING) << "  " << cmdlineUnrecognized[i];
      LOG(WARNING) << "End of unrecognized arguments";
   }

   if(!cmdlineExtras.empty()) {
      stripTrailing(cmdlineExtras,'\n');
      LOG(INFO) << cmdlineExtras;
   }

      // fatal errors
   if(!cmdlineErrors.empty()) {
      stripTrailing(cmdlineErrors,'\n');
      replaceAll(cmdlineErrors,"\n","\n ");
      LOG(ERROR) << "Errors found on command line:\n " << cmdlineErrors
                 << "\nEnd of command line errors.";
      return 1;
   }

      // success: dump configuration summary
   if(debug > -1) {
      ostringstream oss;
      oss << "------ Summary of " << prgmName
          << " command line configuration ------\n";
      opts.DumpConfiguration(oss);
      if(!cmdlineExtras.empty()) oss << "# Extra Processing:\n" << cmdlineExtras;
      oss << "\n------ End configuration summary ------";
      LOG(DEBUG) << oss.str();
   }

   return 0;

}  // end Configuration::CommandLine()

//------------------------------------------------------------------------------
string Configuration::buildCommandLine(void) throw()
{
      // Program description will appear at the top of the syntax page
   string PrgmDesc = " Program " + prgmName +
      " will open and read RINEX observation files(s), apply editing\n"
      " commands, and write out the modified RINEX data to RINEX file(s).\n"
      " Input is on the command line, or of the same form in a file (--file).\n"
      " NB. Minimum required input is one input file (--IF) and one output file (--OF).\n"
      "  Usage: " + prgmName + " [options] [editing commands]\n"
      "  Options:";

      // options to appear on the syntax page, and to be accepted on command line
      //opts.Add(char, opt, arg, repeat?, required?, &target, pre-desc, desc);
      // NB cfgfile is a dummy, but it must exist when cmdline is processed.
   opts.Add(0, "IF", "fn", true, false, &messIF,
            "# RINEX input and output files",
            "Input RINEX observation file name");
   opts.Add(0, "ID", "p", false, false, &InObspath, "",
            "Path of input RINEX observation file(s)");
   opts.Add(0, "OF", "fn", true, false, &messOF, "",
            "Output RINEX obs files [also see --OF <f,t> below]");
   opts.Add(0, "OD", "p", false, false, &OutObspath, "",
            "Path of output RINEX observation file(s)");

   opts.Add('f', "file", "fn", true, false, &cfgfile, "# Other file I/O",
            "Name of file containing more options [#->EOL = comment]");
   opts.Add('l', "log", "fn", false, false, &logfile, "",
            "Output log file name");
   opts.Add(0, "ver2", "", false, false, &outver2, "",
            "Write out RINEX version 2");

   opts.Add(0, "verbose", "", false, false, &verbose, "# Help",
            "Print extra output information");
   opts.Add(0, "debug", "", false, false, &debug, "",
            "Print debug output at level 0 [debug<n> for level n=1-7]");
   opts.Add(0, "help", "", false, false, &help, "",
            "Print this syntax page, and quit");

   opts.Add(0, "HDp", "p", false, false, &messHDp, "# ------ Editing commands ------\n"
            "# RINEX header modifications (arguments with whitespace must be quoted)",
            "Set header 'PROGRAM' field to <p>");
   opts.Add(0, "HDr", "rb", false, false, &messHDr, "",
            "Set header 'RUN BY' field to <rb>");
   opts.Add(0, "HDo", "obs", false, false, &messHDo, "",
            "Set header 'OBSERVER' field to <obs>");
   opts.Add(0, "HDa", "a", false, false, &messHDa, "",
            "Set header 'AGENCY' field to <a>");
   opts.Add(0, "HDx", "x,y,z", false, false, &messHDx, "",
            "Set header 'POSITION' field to <x,y,z> (ECEF, m)");
   opts.Add(0, "HDm", "m", false, false, &messHDm, "",
            "Set header 'MARKER NAME' field to <m>");
   opts.Add(0, "HDn", "n", false, false, &messHDn, "",
            "Set header 'MARKER NUMBER' field to <n>");
   opts.Add(0, "HDj", "n", false, false, &messHDj, "",
            "Set header 'REC #' field to <n>");
   opts.Add(0, "HDk", "t", false, false, &messHDk, "",
            "Set header 'REC TYPE' field to <t>");
   opts.Add(0, "HDl", "v", false, false, &messHDl, "",
            "Set header 'REC VERS' field to <v>");
   opts.Add(0, "HDs", "n", false, false, &messHDs, "",
            "Set header 'ANT #' field to <n>");
   opts.Add(0, "HDt", "t", false, false, &messHDt, "",
            "Set header 'ANT TYPE' field to <t>");
   opts.Add(0, "HDh", "h,e,n", false, false, &messHDh, "",
            "Set header 'ANTENNA OFFSET' field to <h,e,n> (Ht,East,North)");
   opts.Add(0, "HDc", "c", true, false, &messHDc, "",
            "Add 'COMMENT' <c> to the output header");
   opts.Add(0, "HDdc", "", false, false, &messHDdc, "",
            "Delete all comments [not --HDc] from input header");
   opts.Add(0, "HDda", "", false, false, &messHDda, "",
            "Delete all auxiliary header data");

   startStr = defaultstartStr;
   stopStr = defaultstopStr;
   opts.Add(0, "TB", "t[:f]", false, false, &startStr,
            "# Time related [t,f are strings, time t conforms to format f;"
            " cf. gpstk::Epoch.]\n# Default t(f) is 'week,sec-of-week'(%F,%g)"
            " OR 'y,m,d,h,m,s'(%Y,%m,%d,%H,%M,%S)\n"
            " --OF <f,t>        At RINEX time <t>, close output file and open "
            "another named <f> ()",
            "Start time: Reject data before this time");
   opts.Add(0, "TE", "t[:f]", false, false, &stopStr, "",
            "Stop  time: Reject data after this time");
   opts.Add(0, "TT", "dt", false, false, &timetol, "",
            "Tolerance in comparing times, in seconds");
   opts.Add(0, "TN", "dt", false, false, &decimate, "",
            "If dt>0, decimate data to times = TB + N*dt [sec, w/in tol]");

   opts.Add(0, "DA", "t", true, false, &messDA,
            "# In the following <SV> is a RINEX satellite identifier, "
            "e.g. G17 R7 E22 R etc.\n"
            "#              and <OT> is a 3- or 4-char RINEX observation code "
            "e.g. C1C GL2X S2N\n"
            "# Delete cmds; for start(stop) cmds. stop(start) time defaults "
            "to end(begin) of data\n"
            "#     and 'deleting' data for a single OT means it is set to zero "
            "- as RINEX requires.",
            "Delete all data at a single time <t>");
   opts.Add(0, "DA+", "t", true, false, &messDAp, "",
            "Delete all data beginning at time <t>");
   opts.Add(0, "DA-", "t", true, false, &messDAm, "",
            "Stop deleting at time <t>");

   opts.Add(0, "DO", "OT", true, false, &messDO, "",
            "Delete RINEX obs type <OT> entirely (incl. header)");

   opts.Add(0, "DS", "SV,t", true, false, &messDS,
            " --DS <SV>         Delete all data for satellite <SV> [SV may be char]",
            "Delete all data for satellite <SV> at single time <t>");
   opts.Add(0, "DS+", "SV,t", true, false, &messDSp, "",
            "Delete data for satellite <SV> beginning at time <t>");
   opts.Add(0, "DS-", "SV,t", true, false, &messDSm, "",
            "Stop deleting data for sat <SV> beginning at time <t>");

   opts.Add(0, "DD", "SV,OT,t", true, false, &messDD, "",
            "Delete a single RINEX datum(SV,OT) at time <t>");
   opts.Add(0, "DD+", "SV,OT,t", true, false, &messDDp, "",
            "Delete all RINEX data(SV,OT) starting at time <t>");
   opts.Add(0, "DD-", "SV,OT,t", true, false, &messDDm, "",
            "Stop deleting RINEX data(SV,OT) at time <t>");

   opts.Add(0, "SD", "SV,OT,t,d", true, false, &messSD, "",
            "Set data(SV,OT) to value <d> at single time <t>");
   opts.Add(0, "SS", "SV,OT,t,s", true, false, &messSS, "",
            "Set SSI(SV,OT) to value <s> at single time <t>");
   opts.Add(0, "SL", "SV,OT,t,l", true, false, &messSL, "",
            "Set LLI(SV,OT) to value <l> at single time <t>");
   opts.Add(0, "SL+", "SV,OT,t,l", true, false, &messSLp, "",
            "Set all LLI(SV,OT) to value <l> starting at time <t>");
   opts.Add(0, "SL-", "SV,OT,t,l", true, false, &messSLm, "",
            "Stop setting LLI(SV,OT) to value <l> at time <t>");

   opts.Add(0, "BZ", "", false, false, &messBZ,
            "# Bias cmds: (BD cmds apply only when data is non-zero, unless --BZ)",
            "Apply BD command even when data is zero (i.e. 'missing')");
   opts.Add(0, "BS", "SV,OT,t,s", true, false, &messBS, "",
            "Add the value <s> to SSI(SV,OT) at single time <t>");
   opts.Add(0, "BL", "SV,OT,t,l", true, false, &messBL, "",
            "Add the value <l> to LLI(SV,OT) at single time <t>");
   opts.Add(0, "BD", "SV,OT,t,d", true, false, &messBD, "",
            "Add the value <d> to data(SV,OT) at single time <t>");
   opts.Add(0, "BD+", "SV,OT,t,d", true, false, &messBDp, "",
            "Add the value <d> to data(SV,OT) beginning at time <t>");
   opts.Add(0, "BD-", "SV,OT,t,d", true, false, &messBDm, "",
            "Stop adding the value <d> to data(SV,OT) at time <t>");

      // turn off argument expansion for the editing commands
   opts.noExpansion("HDc");
   opts.noExpansion("OF");
   opts.noExpansion("DA");
   opts.noExpansion("DA-");
   opts.noExpansion("DA+");
   opts.noExpansion("DO");
   opts.noExpansion("DS");
   opts.noExpansion("DS+");
   opts.noExpansion("DS-");
   opts.noExpansion("DD");
   opts.noExpansion("DD+");
   opts.noExpansion("DD-");
   opts.noExpansion("SD");
   opts.noExpansion("SS");
   opts.noExpansion("SL");
   opts.noExpansion("SL+");
   opts.noExpansion("SL-");
   opts.noExpansion("BD");
   opts.noExpansion("BD+");
   opts.noExpansion("BD-");
   opts.noExpansion("BS");
   opts.noExpansion("BL");

      // deprecated (old,new)
      //opts.Add_deprecated("--SP3","--eph");

   return PrgmDesc;

}  // end Configuration::buildCommandLine()

//------------------------------------------------------------------------------
int Configuration::extraProcessing(string& errors, string& extras) throw()
{
   int n;
   size_t i;
   vector<string> fld;
   ostringstream oss,ossx;       // oss for Errors, ossx for Warnings and info

      // start and stop times
   for(i=0; i<2; i++) {
      static const string fmtGPS("%F,%g"),fmtCAL("%Y,%m,%d,%H,%M,%S");
      msg = (i==0 ? startStr : stopStr);
      if(msg == (i==0 ? defaultstartStr : defaultstopStr)) continue;

      bool ok(true);
      bool hasfmt(msg.find('%') != string::npos);
      n = numWords(msg,',');
      if(hasfmt) {
         fld = split(msg,':');
         if(fld.size() != 2) { ok = false; }
         else try {
               Epoch ep;
               stripLeading(fld[0]," \t");
               stripLeading(fld[1]," \t");
               ep.scanf(fld[0],fld[1]);
               (i==0 ? beginTime : endTime) = static_cast<CommonTime>(ep);
            }
            catch(Exception& e) { ok = false; LOG(INFO) << "excep " << e.what(); }
      }
      else if(n == 2 || n == 6) {        // try the defaults
         try {
            Epoch ep;
            ep.scanf(msg,(n==2 ? fmtGPS : fmtCAL));
            (i==0 ? beginTime : endTime) = static_cast<CommonTime>(ep);
         }
         catch(Exception& e) { ok = false; LOG(INFO) << "excep " << e.what(); }
      }

      if(ok) {
         msg = printTime((i==0 ? beginTime : endTime),fmtGPS+" = "+fmtCAL);
         if(msg.find("Error") != string::npos) ok = false;
      }

      if(!ok)
         oss << "Error : invalid time or format in --" << (i==0 ? "start" : "stop")
             << " " << (i==0 ? startStr : stopStr) << endl;
      else
         ossx << (i==0 ? "   Begin time --begin" : "   End time --end") << " is "
              << printTime((i==0 ? beginTime : endTime), fmtGPS+" = "+fmtCAL+"\n");
   }

      // parse the editing commands
   parseEditCmds(messOF, "OF", oss);
   parseEditCmds(messDA, "DA", oss);
   parseEditCmds(messDAp, "DA+", oss);
   parseEditCmds(messDAm, "DA-", oss);
   parseEditCmds(messDO, "DO", oss);
   parseEditCmds(messDS, "DS", oss);
   parseEditCmds(messDSp, "DS+", oss);
   parseEditCmds(messDSm, "DS-", oss);
   parseEditCmds(messDD, "DD", oss);
   parseEditCmds(messDDp, "DD+", oss);
   parseEditCmds(messDDm, "DD-", oss);
   parseEditCmds(messSD, "SD", oss);
   parseEditCmds(messSS, "SS", oss);
   parseEditCmds(messSL, "SL", oss);
   parseEditCmds(messSLp, "SL+", oss);
   parseEditCmds(messSLm, "SL-", oss);
   parseEditCmds(messBD, "BD", oss);
   parseEditCmds(messBDp, "BD+", oss);
   parseEditCmds(messBDm, "BD-", oss);
   parseEditCmds(messBS, "BS", oss);
   parseEditCmds(messBL, "BL", oss);

      // dump commands for debug
      //TEMP if(debug > -1)
      //for(i=0; i<vecCmds.size(); i++)
      //LOG(INFO) << vecCmds[i].asString(" Input Edit cmd:");

      // 'fix up' list of edit cmds: sort, add -(+) for unmatched +(-), find + > -
   fixEditCmdList();

      // dump final list of commands
   if(verbose)
      for(i=0; i<vecCmds.size(); i++)
         ossx << vecCmds[i].asString(" Edit cmd:") << endl;

      // open the log file (so warnings, configuration summary, etc can go there) -----
   if(!logfile.empty()) {
      logstrm.open(logfile.c_str(), ios::out);
      if(!logstrm.is_open()) {
         LOG(ERROR) << "Error : Failed to open log file " << logfile;
         return -4;
      }
      LOG(INFO) << "Output redirected to log file " << logfile;
      pLOGstrm = &logstrm;
   }
   LOG(INFO) << Title;

      // add new errors to the list
   msg = oss.str();
   if(!msg.empty()) errors += msg;
   msg = ossx.str();
   if(!msg.empty()) extras += msg;

   return 0;

} // end Configuration::extraProcessing(string& errors) throw()

//------------------------------------------------------------------------------
// little helper routine for extraProcessing
void Configuration::parseEditCmds(vector<string>& vec, const string lab,
                                                           ostringstream& os) throw()
{
   for(size_t i=0; i<vec.size(); i++) {
      EditCmd ec(lab,vec[i]);
      if(ec.isValid()) vecCmds.push_back(ec);
      else os << "Error: invalid argument in " << lab << " cmd: >" << vec[i] << "<\n";
   }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// constructor from strings, i.e. parser e.g. "DA+","t" or "BDp","SV,OT,t,s"
EditCmd::EditCmd(const string intypestr, const string inarg) throw(Exception)
{
   try {
      string tag(upperCase(intypestr)), arg(inarg);
      vector<string> flds;

      type = invalidCT;                                 // defaults
      ttag = CommonTime::BEGINNING_OF_TIME;
      sign = idata = 0;
      data = 0.0;

      if(tag.size() == 2) sign = 0;                   // pull off sign
      else if(tag[2] == '+') sign = 1;
      else if(tag[2] == '-') sign = -1;
      else return;
      tag = tag.substr(0,2);

      flds = split(arg,',');                          // split arg
      const size_t n(flds.size());                       // number of args

      if(tag == "OF") {
         if(n != 1 && n != 3 && n != 7) return;
         field = flds[0];
         if(n != 1) {
            stripLeading(arg,field+",");
            if(!parseTime(arg,ttag)) return;
         }
         type = ofCT;
      }
      else if(tag == "DA") {
         if(!parseTime(arg,ttag)) return;
         type = daCT;
      }
      else if(tag == "DO") {
         if(sign != 0) return;                        // no DO+ or DO-

         if(arg.size() == 4)                          // get sys
            sat.fromString(string(1,arg[0]));
            // else sat sys is unknown

         if(isValidRinexObsID(arg)) obs = RinexObsID(arg);
         else return;

         type = doCT;
      }
      else if(tag == "DS") {
         if(n != 1 && n != 3 && n != 7) return;    // DS DS,w,sow and DS,y,m,d,h,m,s
         try { sat.fromString(flds[0]); } catch(Exception) { return; }
         if(n != 1) {                              // time for DS is BeginTime
            stripLeading(arg,flds[0]+",");
            if(!parseTime(arg,ttag)) return;
         }
         if(sign == 0 && n == 1) sign = 1;
         type = dsCT;
      }
      else {
            // args are SV,OT,t[,d or s or l]
         if(n < 4) return;                            // at minimum SV,OT,week,sow

         stripLeading(arg,flds[0]+","+flds[1]+",");   // remove 'SV,OT,' from arg

         string dat;
         if(tag != "DD") {                            // strip and save last arg (dsl)
            dat = flds[flds.size()-1];
            stripTrailing(arg,string(",")+dat);
         }
         if(!parseTime(arg,ttag)) return;             // get the time

            // parse satellite
         try { sat.fromString(flds[0]); } catch(Exception) { return; }

            // add system char to obs string
         if(flds[1].size() == 3 && sat.systemChar() != '?')
            flds[1] = string(1,sat.systemChar()) + flds[1];
            // parse obs type
         if(isValidRinexObsID(flds[1])) obs = RinexObsID(flds[1]); else return;

         if(tag == "DD") { type = ddCT; return; } // DD is done

         if(n != 5 && n != 9) return;           // rest have SV,OT,t,d = 5 or 9 args

         if(tag == "SD" || tag == "BD") {       // double data
            if(isScientificString(dat)) data = asDouble(dat); else return;
         }
         else {                                 // rest have int data
            if(isDigitString(dat)) idata = asInt(dat); else return;
         }

            // now just set type
         if(tag == "SD") type = sdCT;
         else if(tag == "SS") type = ssCT;
         else if(tag == "SL") type = slCT;
         else if(tag == "BS") type = bsCT;
         else if(tag == "BL") type = blCT;
         else if(tag == "BD") type = bdCT;
      }
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------
bool EditCmd::parseTime(const string arg, CommonTime& ttag) throw()
{
   static const string fmtGPS("%F,%g"),fmtCAL("%Y,%m,%d,%H,%M,%S");
   stripLeading(arg," \t");
   int n(numWords(arg,','));
   if(n == 2 || n == 6) {
      Epoch ep;
      try { ep.scanf(arg, (n==2 ? fmtGPS : fmtCAL)); }
      catch(StringException) { return false; }
      ttag = static_cast<CommonTime>(ep);
   }
   else return false;

   return true;
}

//------------------------------------------------------------------------------
// dump, with optional message
string EditCmd::asString(string msg) throw(Exception)
{
   try {
      Configuration& C(Configuration::Instance());
      static map<CmdType,string> typeLabel;
      if(typeLabel.size() == 0) {
         typeLabel[invalidCT] = string("--invalidCT--   ");
         typeLabel[ofCT] = string("OF_Output_File");
         typeLabel[daCT] = string("DA_Delete_All ");
         typeLabel[doCT] = string("DO_Delete_Obs ");
         typeLabel[dsCT] = string("DS_Delete_Sat ");
         typeLabel[ddCT] = string("DD_Delete_Data");
         typeLabel[sdCT] = string("SD_Set_Data   ");
         typeLabel[ssCT] = string("SS_Set_SSI    ");
         typeLabel[slCT] = string("SL_Set_LLI    ");
         typeLabel[bdCT] = string("BD_Bias_Data  ");
         typeLabel[bsCT] = string("BS_Bias_SSI   ");
         typeLabel[blCT] = string("BL_Bias_LLI   ");
      }

      ostringstream os;

      if(msg.size()) os << msg;
      os << " " << typeLabel[type]
         << " " << (sign==0 ? "0" : (sign<0 ? "-":"+"))
         << " SV:" << sat.toString()
         << " OT:" << obs.asString()
         << " d:" << fixed << setprecision(4) << data
         << " i:" << idata
         << " t:" << (ttag == CommonTime::BEGINNING_OF_TIME
                      ? "BeginTime" : printTime(ttag,C.longfmt))
         << " >" << field << "<";

      return os.str();
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(std::exception& e) {
      Exception E(string("std::except: ") + e.what());
      GPSTK_THROW(E);
   }
}

//------------------------------------------------------------------------------
void fixEditCmdList(void) throw()
{
   Configuration& C(Configuration::Instance());
   vector<EditCmd>::iterator it, jt;
   vector<EditCmd> newCmds;

      // sort on time
   sort(C.vecCmds.begin(), C.vecCmds.end(), EditCmdLessThan());

      // ensure each - command has a corresponding + command
      // (note that + cmds do not need a - cmd: they will just never be turned off)
   for(it = C.vecCmds.begin(); it != C.vecCmds.end(); ++it ) {
      if(it->sign == -1 && it->type != EditCmd::invalidCT) {
         bool havePair(false);
         if(it != C.vecCmds.begin()) {
            jt = it; --jt;  // --(jt = it);
            while(1) {                                // search backwards for match
               if(jt->type == it->type &&
                  jt->sat == it->sat &&
                  jt->obs == it->obs)
               {
                  if(jt->sign == 1) havePair=true;    // its a match
                  else if(jt->sign == -1) {           // this is an error
                     LOG(ERROR) << it->asString("Error: repeat '-'");
                     //LOG(ERROR) << jt->asString("Error: ref here  ");
                     it->type = EditCmd::invalidCT;
                  }
                  break;
               }
               if(jt == C.vecCmds.begin()) break;
               --jt;
            }
         }
         if(!havePair && it->type != EditCmd::invalidCT) {
            EditCmd ec(*it);
            ec.sign = 1;
            ec.ttag = CommonTime::BEGINNING_OF_TIME;
            newCmds.push_back(ec);
            LOG(VERBOSE) << ec.asString(" Add cmd:");
         }
      }
   }

   if(newCmds.size() > 0) {
      for(it = newCmds.begin(); it != newCmds.end(); ++it )
         C.vecCmds.push_back(*it);
      newCmds.clear();
      sort(C.vecCmds.begin(), C.vecCmds.end(), EditCmdLessThan());
   }

      // remove invalidCT commands
   it = C.vecCmds.begin();
   while(it != C.vecCmds.end()) {
      if(it->type == EditCmd::invalidCT)
         it = C.vecCmds.erase(it);              // erase vector element
      else
         ++it;
   }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
