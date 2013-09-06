/// @file RinEdit.cpp
/// Read Rinex observation files (version 2 or 3) and edit them, writing the edited
/// data to a new RINEX file.

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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

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

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
string Version(string("1.0 8/1/11 rev"));
// TD
// option to replace input with output?
// include optional fmt input for t in edit cmds - is this feasible?
// if given a 4-char OT and SV, check their consistency
// OK - test it.  implement DO - how? copy and edit, or clear and copy?
// OK - test it.  edit header when DS (alone) or DO appear ... how?
// how to handle aux header data if its first - OF not yet opened
// END TD

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// class to encapsulate editing commands
class EditCmd {
public:
   enum CmdType {
      INVALID=0,OF,DA,DO,DS,DD,SD,SS,SL,BD,BS,BL,BZ,count
   };
   CmdType type;     // the type of this command
   RinexSatID sat;   // satellite
   RinexObsID obs;   // observation type
   CommonTime ttag;  // associated time tag
   int sign;         // sign +1,0,-1
   int idata;        // integer e.g. SSI or LLI
   double data;      // data e.g. bias value
   string field;     // OF file name

   EditCmd(void) : type(INVALID) {}       // default constructor
   ~EditCmd(void) {}                      // destructor

   // constructor from strings, i.e. parser e.g. "DA+","t" or "BDp","SV,OT,t,s"
   EditCmd(const string typestr, const string arg) throw(Exception);
   // parse time from string
   bool parseTime(const string arg, CommonTime& ttag) throw();

   inline bool isValid(void) throw()      // is it valid?
      { return (type != INVALID); }

   string asString(string msg=string())   // dump, with optional message at front
      throw(Exception);

}; // end class EditCmd

// used for sorting
class EditCmdLessThan {
public:
   bool operator()(const EditCmd& ec1, const EditCmd& ec2)
      { return (ec1.ttag < ec2.ttag); }
};

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// Object for command line input and global data
class Configuration : public Singleton<Configuration> {

public:

   // Default and only constructor
   Configuration() throw() { SetDefaults(); }

   // Create, parse and process command line options and user input
   int ProcessUserInput(int argc, char **argv) throw();

   // Design the command line
   string BuildCommandLine(void) throw();

   // Open the output file, and parse the strings used on the command line
   // return -4 if log file could not be opened
   int ExtraProcessing(string& errors, string& extras) throw();

   // Parse one of the vector<string> of edit cmd options
   void parseEditCmds(vector<string>& v, const string l, ostringstream& os) throw();

private:

   // Define default values
   void SetDefaults(void) throw()
   {
      defaultstartStr = string("[Beginning of dataset]");
      defaultstopStr = string("[End of dataset]");
      beginTime = CommonTime::BEGINNING_OF_TIME;
      endTime = CommonTime::END_OF_TIME;
      decimate = 0.0;
   
      help = verbose = outver2 = false;
      debug = -1;

      HDdc = HDda = false;
   }  // end Configuration::SetDefaults()

public:

// member data
   CommandLine opts;             // command line options and syntax page
   static const string PrgmName; // program name
   string Title;                 // id line printed to screen and log

   // start command line input
   bool help, verbose, outver2;
   int debug;
   string cfgfile;

   vector<string> IF, OF;        // RINEX obs file names - IF and OF args
   string InObspath,OutObspath;  // obs file path

   // times derived from --start and --stop
   string defaultstartStr,startStr;
   string defaultstopStr,stopStr;
   CommonTime beginTime,endTime,decTime;

   double decimate,timetol;      // decimate input data
   string logfile;               // output log file

   // editing commands
   bool HDdc,HDda,BZ;
   string HDp,HDr,HDo,HDa,HDx,HDm,HDn,HDt,HDh;
   vector<string> HDc,DA,DAm,DAp,DO,DS,DSp,DSm,DD,DDp,DDm;
   vector<string> SD,SS,SL,SLp,SLm,BD,BDp,BDm,BS,BL;

   // end of command line input

   string msg;
   ofstream logstrm;
   static const string calfmt,gpsfmt,longfmt;

   // handle commands
   vector<EditCmd> vecCmds, currCmds;
   Rinex3ObsStream ostrm;        // RINEX output

}; // end class Configuration

//------------------------------------------------------------------------------------
// const members of Configuration
const string Configuration::PrgmName = string("RinEdit");
const string Configuration::calfmt = string("%04Y/%02m/%02d %02H:%02M:%02S");
const string Configuration::gpsfmt = string("%4F %10.3g");
const string Configuration::longfmt = calfmt + " = " + gpsfmt + " %P";

//------------------------------------------------------------------------------------
// prototypes
int Initialize(string& errors) throw(Exception);
void FixEditCmdList(void) throw();
int ProcessFiles(void) throw(Exception);
int ProcessOneEpoch(Rinex3ObsHeader& Rhead, Rinex3ObsHeader& RHout,
                    Rinex3ObsData& Rdata, Rinex3ObsData& RDout) throw(Exception);
int ExecuteEditCmd(const vector<EditCmd>::iterator& it, Rinex3ObsHeader& Rhead,
                                            Rinex3ObsData& Rdata) throw(Exception);

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
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
   C.Title = "# " + C.PrgmName + ", part of the GPS Toolkit, Ver " + Version
      + ", Run " + printTime(wallclkbeg,C.calfmt);
   
   for(;;) {
      // get information from the command line
      // iret -2 -3 -4
      if((iret = C.ProcessUserInput(argc, argv)) != 0) break;

      // read stores, check input etc
      string errs;
      if((iret = Initialize(errs)) != 0) {
         LOG(ERROR) << "------- Input is not valid: ----------\n" << errs
                    << "\n------- end errors -----------";
         break;
      }
      if(!errs.empty()) LOG(INFO) << errs;         // Warnings are here too

      iret = ProcessFiles();                       // iret == number of files

      break;      // mandatory
   }

   if(iret == 0) {
      // print elapsed time
      totaltime = clock()-totaltime;
      Epoch wallclkend;
      wallclkend.setLocalTime();
      ostringstream oss;
      oss << C.PrgmName << " timing: processing " << fixed << setprecision(3)
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

//------------------------------------------------------------------------------------
// return -5 if input is not valid
int Initialize(string& errors) throw(Exception)
{
try {
   bool isValid(true);
   int j;
   size_t i;
   Configuration& C(Configuration::Instance());
   errors = string("");
   ostringstream ossE;

   // must have an input file and an output file
   if(C.IF.size() == 0) {
      ossE << "Error : No valid input files have been specified.";
      isValid = false;
   }
   if(C.OF.size() == 0) {
      ossE << "Error : No valid output files have been specified.";
      isValid = false;
   }

   // add path to filenames, and expand tilde (~)
   include_path(C.InObspath, C.IF);

   // add path to all OF
   // also if first OF command has a timetag, remove it and make that the start time
   for(j=0, i=0; i<C.vecCmds.size(); ++i) {
      if(C.vecCmds[i].type == EditCmd::OF) {
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
   }

   // -------- save errors and output
   errors = ossE.str();
   stripTrailing(errors,'\n');
   replaceAll(errors,"\n","\n# ");

   if(!isValid) return -5;
   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end Initialize()

//------------------------------------------------------------------------------------
// Return 0 ok, >0 number of files successfully read, <0 fatal error
int ProcessFiles(void) throw(Exception)
{
try {
   Configuration& C(Configuration::Instance());
   static const int width=13;
#pragma unused(width)
   int iret,nfiles;
   size_t i,nfile;
   string tag;
   RinexSatID sat;
   ostringstream oss;

   for(nfiles=0,nfile=0; nfile<C.IF.size(); nfile++) {
      Rinex3ObsStream istrm;
      Rinex3ObsHeader Rhead,RHout;  // use one header for input and output
      Rinex3ObsData Rdata,RDout;
      string filename(C.IF[nfile]);

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

      // we have to set the time system of all the timetags using ttag from file !!
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
         if(it->type == EditCmd::DO) { // no DO+ DO- so far && it->sign == 0)
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
         else if(it->type == EditCmd::DS && it->sign == 0
                  && it->ttag == CommonTime::BEGINNING_OF_TIME)
         {
            map<RinexSatID, vector<int> >::iterator jt;
            jt = RHout.numObsForSat.find(it->sat);
            // if its there, delete it
            if(jt != RHout.numObsForSat.end()) RHout.numObsForSat.erase(jt);

            map<RinexSatID, int>::iterator kt;
            kt = RHout.GlonassFreqNo.find(it->sat);
            // if its there, delete it
            if(kt != RHout.GlonassFreqNo.end()) RHout.GlonassFreqNo.erase(kt);

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
      if(C.outver2) RHout.PrepareVer2Write();
      // NB. header will be written by ExecuteEditCmd
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
         catch(exception& e) {
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
         iret = ProcessOneEpoch(Rhead, RHout, Rdata, RDout);
         if(iret < 0) break;
         if(iret > 0) continue;

         // write data out
         try { C.ostrm << RDout; }
         catch(Exception& e) { GPSTK_RETHROW(e); }

         // debug: dump the RINEX data object
         if(C.debug > -1) Rdata.dump(LOGstrm,Rhead);

      }  // end while loop over epochs

      // clean up
      istrm.close();

      // failure due to critical error
      if(iret < 0) break;

      if(iret >= 0) nfiles++;

   }  // end loop over files

   // final clean up

   if(iret < 0) return iret;

   return nfiles;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end ProcessFiles()

//------------------------------------------------------------------------------------
// return <0 fatal; >0 skip this epoch
int ProcessOneEpoch(Rinex3ObsHeader& Rhead, Rinex3ObsHeader& RHout,
                    Rinex3ObsData& Rdata, Rinex3ObsData& RDout) throw(Exception)
{
   try {
      Configuration& C(Configuration::Instance());
      int iret(0);
      RinexSatID sat;
      CommonTime now(Rdata.time);         // TD what if its aux data w/o an epoch?

      // if aux header data, either output or skip
      if(RDout.epochFlag > 1) {           // aux header data
         if(C.HDda) return 1;
         return 0;
      }

      else {                              // regular data
         vector<EditCmd>::iterator it, jt;

         // for cmds with ttag <= now either execute and delete, or move to current
         it = C.vecCmds.begin();
         while(it != C.vecCmds.end()) {
            if(it->ttag <= now || ::fabs(it->ttag - now) < C.timetol) {
	       // execute command;
               // delete one-time cmds, move others to curr and delete
               iret = ExecuteEditCmd(it, RHout, RDout);
               if(iret < 0) return iret;

               // keep this command on the current list
               if(iret > 0) C.currCmds.push_back(*it);

               // remove from vecCmds
               it = C.vecCmds.erase(it);              // erase vector element
            }
            else
               ++it;
         }
      
         // apply current commands, deleting obsolete ones
         it = C.currCmds.begin();
         while(it != C.currCmds.end()) {
            // execute command; delete obsolete commands
            iret = ExecuteEditCmd(it, RHout, RDout);
            if(iret < 0) return iret;

            if(iret == 0)
               it = C.currCmds.erase(it);              // erase vector element
            else
               ++it;
         }
      }

      return 0;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end ProcessOneEpoch()

//------------------------------------------------------------------------------------
// return >0 to put/keep the command on the 'current' queue
// return <0 for fatal error
int ExecuteEditCmd(const vector<EditCmd>::iterator& it, Rinex3ObsHeader& Rhead,
                                                        Rinex3ObsData& Rdata)
   throw(Exception)
{
   Configuration& C(Configuration::Instance());
   int iret;
#pragma unused(iret)
   size_t i;
   string sys;
   vector<string> flds;
   Rinex3ObsData::DataMap::iterator kt;
   vector<RinexObsID>::iterator jt;

   try {
      if(it->type == EditCmd::INVALID) {
         // message?
         return 0;
      }

      // OF output file --------------------------------------------------------------
      else if(it->type == EditCmd::OF) {
         // close the old file
         if(C.ostrm.is_open()) { C.ostrm.close(); C.ostrm.clear(); }

         // open the new file
         C.ostrm.open(it->field.c_str(),ios::out);
         if(!C.ostrm.is_open()) {
            LOG(ERROR) << "Error : could not open output file " << it->field;
            return 1;
         }
         C.ostrm.exceptions(ios::failbit);

         LOG(INFO) << " Opened output file " << it->field << " at time "
                        << printTime(Rdata.time,C.longfmt);

         // if this is the first file, apply the header commands
         if(it->ttag == CommonTime::BEGINNING_OF_TIME) {
                               Rhead.fileProgram = C.PrgmName;
            if(!C.HDp.empty()) Rhead.fileProgram = C.HDp;
            if(!C.HDr.empty()) Rhead.fileAgency = C.HDr;
            if(!C.HDo.empty()) Rhead.observer = C.HDo;
            if(!C.HDa.empty()) Rhead.agency = C.HDa;
            if(!C.HDx.empty()) {
               flds = split(C.HDx,',');   // TD check n==3,doubles in Initialize
               for(i=0; i<3; i++) Rhead.antennaPosition[i] = asDouble(flds[i]);
            }
            if(!C.HDm.empty()) Rhead.markerName = C.HDm;
            if(!C.HDn.empty()) Rhead.markerNumber = C.HDx;
            if(!C.HDt.empty()) Rhead.antType = C.HDt;
            if(!C.HDh.empty()) {
               flds = split(C.HDh,',');   // TD check n==3,doubles in Initialize
               for(i=0; i<3; i++) Rhead.antennaDeltaHEN[i] = asDouble(flds[i]);
            }
            if(C.HDdc) {
               Rhead.commentList.clear();
               Rhead.valid ^= Rinex3ObsHeader::validComment;
            }
            if(C.HDc.size() > 0) {
               for(i=0; i<C.HDc.size(); i++)
                  Rhead.commentList.push_back(C.HDc[i]);
               Rhead.valid |= Rinex3ObsHeader::validComment;
            }
         }

         Rhead.firstObs = Rdata.time;
         Rhead.valid &= ~Rinex3ObsHeader::validLastTime;    // turn off

         // write the header
         C.ostrm << Rhead;
      }

      // DA delete all ---------------------------------------------------------------
      else if(it->type == EditCmd::DA) {
         switch(it->sign) {
            case 1: case 0:
               Rdata.numSVs = 0;                   // clear this data, return 0
               Rdata.obs.clear();
               break;
            case -1: return 1;                     // delete the (-) command
               break;
         }
      }

      // DO delete obs type ----------------------------------------------------------
      // handled above where input is copied into output
      else if(it->type == EditCmd::DO)
         return 0;

      // DS delete satellite ---------------------------------------------------------
      // TD what if entire system is deleted
      else if(it->type == EditCmd::DS) {
         if(it->sign == -1) return 1;                 // delete the (-) command

         if(it->sign == 0 && it->ttag == CommonTime::BEGINNING_OF_TIME)
               return 0;                              // handled above where I->0

         // find the SV
         kt = Rdata.obs.find(it->sat);
         if(kt != Rdata.obs.end()) {                  // found the SV
            Rdata.obs.erase(kt);                      // remove it
            Rdata.numSVs--;                           // don't count it
         }
         if(it->sign == 0) return 1;                  // delete the one-time command
      }

      // -----------------------------------------------------------------------------
      // the rest require that we find satellite and obsid in Rdata.obs
      else {
         if(it->sign == -1) return 1;                 // delete the (-) command

         kt = Rdata.obs.find(it->sat);                // find the sat
         if(kt == Rdata.obs.end())                    // sat not found
            return 0;                                 // (this may be normal)

         sys = asString(it->sat.systemChar());        // find the system

         // find the OT in the header map, and get index into vector
         jt = find(Rhead.mapObsTypes[sys].begin(),
                   Rhead.mapObsTypes[sys].end(), it->obs);
         if(jt == Rhead.mapObsTypes[sys].end())       // ObsID not found
            // TD message? user error: ask to delete one that's not there
            return 0;

         i = (jt - Rhead.mapObsTypes[sys].begin());   // index into vector

         switch(it->type) {
            // DD delete data --------------------------------------------------------
            case EditCmd::DD:
               Rdata.obs[it->sat][i].data = 0.0;
               Rdata.obs[it->sat][i].ssi = 0;
               Rdata.obs[it->sat][i].lli = 0;
               break;
            // SD set data -----------------------------------------------------------
            case EditCmd::SD:
               Rdata.obs[it->sat][i].data = it->data;
               break;
            // SS set SSI ------------------------------------------------------------
            case EditCmd::SS:
               Rdata.obs[it->sat][i].ssi = it->idata;
               break;
            // SL set LLI ------------------------------------------------------------
            case EditCmd::SL:
               Rdata.obs[it->sat][i].lli = it->idata;
               break;
            // BD bias data ----------------------------------------------------------
            case EditCmd::BD:
               Rdata.obs[it->sat][i].data += it->data;
               break;
            // BS bias SSI -----------------------------------------------------------
            case EditCmd::BS:
               Rdata.obs[it->sat][i].ssi += it->idata;
               break;
            // BL bias LLI -----------------------------------------------------------
            case EditCmd::BL:
               Rdata.obs[it->sat][i].lli += it->idata;
               break;
            // never reached ---------------------------------------------------------
            default:
               // message?
               break;
         }

         if(it->sign == 0) return 1;                  // delete the one-time command
      }

      return 0;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end ExecuteEditCmd()

//------------------------------------------------------------------------------------
int Configuration::ProcessUserInput(int argc, char **argv) throw()
{
   string PrgmDesc,cmdlineUsage, cmdlineErrors, cmdlineExtras;
   vector<string> cmdlineUnrecognized;

   // build the command line
   opts.DefineUsageString(PrgmName + " [options]");
   PrgmDesc = BuildCommandLine();

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
   iret = ExtraProcessing(cmdlineErrors, cmdlineExtras);
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
      oss << "------ Summary of " << PrgmName
         << " command line configuration ------\n";
      opts.DumpConfiguration(oss);
      if(!cmdlineExtras.empty()) oss << "# Extra Processing:\n" << cmdlineExtras;
      oss << "------ End configuration summary ------";
      LOG(DEBUG) << oss.str();
   }

   return 0;

}  // end Configuration::CommandLine()

//------------------------------------------------------------------------------------
string Configuration::BuildCommandLine(void) throw()
{
   // Program description will appear at the top of the syntax page
   string PrgmDesc = " Program " + PrgmName +
" will open and read RINEX observation files(s), apply editing\n"
" commands, and write out the modified RINEX data to RINEX file(s).\n"
" Input is on the command line, or of the same form in a file (--file).\n"
" NB. Minimum required input is one input file (--IF) and one output file (--OF).\n"
"  Usage: " + PrgmName + " [options] [editing commands]\n"
"  Options:";

   // options to appear on the syntax page, and to be accepted on command line
   //opts.Add(char, opt, arg, repeat?, required?, &target, pre-desc, desc);
   // NB cfgfile is a dummy, but it must exist when cmdline is processed.
   opts.Add(0, "IF", "fn", true, false, &IF,
            "# RINEX input and output files",
            "Input RINEX observation file name");
   opts.Add(0, "ID", "p", false, false, &InObspath, "",
            "Path of input RINEX observation file(s)");
   opts.Add(0, "OF", "fn", true, false, &OF, "",
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

   opts.Add(0, "HDp", "p", false, false, &HDp, "# ------ Editing commands ------\n"
            "# RINEX header modifications (arguments with whitespace must be quoted)",
            "Set header 'PROGRAM' field to <p>");
   opts.Add(0, "HDr", "rb", false, false, &HDr, "",
            "Set header 'RUN BY' field to <rb>");
   opts.Add(0, "HDo", "obs", false, false, &HDo, "",
            "Set header 'OBSERVER' field to <obs>");
   opts.Add(0, "HDa", "a", false, false, &HDa, "",
            "Set header 'AGENCY' field to <a>");
   opts.Add(0, "HDx", "x,y,z", false, false, &HDx, "",
            "Set header 'POSITION' field to <x,y,z> (ECEF, m)");
   opts.Add(0, "HDm", "m", false, false, &HDm, "",
            "Set header 'MARKER' field to <m>");
   opts.Add(0, "HDn", "n", false, false, &HDn, "",
            "Set header 'NUMBER' field to <n>");
   opts.Add(0, "HDt", "t", false, false, &HDt, "",
            "Set header 'ANTENNA TYPE' field to <t>");
   opts.Add(0, "HDh", "h,e,n", false, false, &HDh, "",
            "Set header 'ANTENNA OFFSET' field to <h,e,n> (Ht,East,North)");
   opts.Add(0, "HDc", "c", true, false, &HDc, "",
            "Add 'COMMENT' <c> to the output header");
   opts.Add(0, "HDdc", "", false, false, &HDdc, "",
            "Delete all comments [not --HDc] from input header");
   opts.Add(0, "HDda", "", false, false, &HDda, "",
            "Delete all auxiliary header data");

   startStr = defaultstartStr;
   stopStr = defaultstopStr;
   opts.Add(0, "TB", "t[:f]", false, false, &startStr,
            "# Time related [t,f are strings, time t conforms to format f;"
            " cf. gpstk::Epoch.]\n# Default t(f) is 'week,sec-of-week'(%F,%g)"
            " OR 'y,m,d,h,m,s'(%Y,%m,%d,%H,%M,%s)\n"
            " --OF <f,t>        At RINEX time <t>, close output file and open "
               "another named <f> ()",
            "Start time: Reject data before this time");
   opts.Add(0, "TE", "t[:f]", false, false, &stopStr, "",
            "Stop  time: Reject data after this time");
   opts.Add(0, "TT", "dt", false, false, &timetol, "",
            "Tolerance in comparing times, in seconds");
   opts.Add(0, "TN", "dt", false, false, &decimate, "",
            "If dt>0, decimate data to times = TB + N*dt [sec, w/in tol]");

   opts.Add(0, "DA", "t", true, false, &DA,
            "# In the following <SV> is a RINEX satellite identifier, "
               "e.g. G17 R7 E22 R etc.\n"
            "#              and <OT> is a 3- or 4-char RINEX observation code "
               "e.g. C1C GL2X S2N\n"
            "# Delete cmds; for start(stop) cmds. stop(start) time defaults "
               "to end(begin) of data\n"
            "#     and 'deleting' data for a single OT means it is set to zero "
               "- as RINEX requires.",
            "Delete all data at a single time <t>");
   opts.Add(0, "DA+", "t", true, false, &DAp, "",
            "Delete all data beginning at time <t>");
   opts.Add(0, "DA-", "t", true, false, &DAm, "",
            "Stop deleting at time <t>");

   opts.Add(0, "DO", "OT", true, false, &DO, "",
            "Delete RINEX obs type <OT> entirely (incl. header)");

   opts.Add(0, "DS", "SV,t", true, false, &DS,
            " --DS <SV>         Delete all data for satellite <SV> [SV may be char]",
            "Delete all data for satellite <SV> at single time <t>");
   opts.Add(0, "DS+", "SV,t", true, false, &DSp, "",
            "Delete data for satellite <SV> beginning at time <t>");
   opts.Add(0, "DS-", "SV,t", true, false, &DSm, "",
            "Stop deleting data for sat <SV> beginning at time <t>");

   opts.Add(0, "DD", "SV,OT,t", true, false, &DD, "",
            "Delete a single RINEX datum(SV,OT) at time <t>");
   opts.Add(0, "DD+", "SV,OT,t", true, false, &DDp, "",
            "Delete all RINEX data(SV,OT) starting at time <t>");
   opts.Add(0, "DD-", "SV,OT,t", true, false, &DDm, "",
            "Stop deleting RINEX data(SV,OT) at time <t>");

   opts.Add(0, "SD", "SV,OT,t,d", true, false, &SD, "",
            "Set data(SV,OT) to value <d> at single time <t>");
   opts.Add(0, "SS", "SV,OT,t,s", true, false, &SS, "",
            "Set SSI(SV,OT) to value <s> at single time <t>");
   opts.Add(0, "SL", "SV,OT,t,l", true, false, &SL, "",
            "Set LLI(SV,OT) to value <l> at single time <t>");
   opts.Add(0, "SL+", "SV,OT,t,l", true, false, &SLp, "",
            "Set all LLI(SV,OT) to value <l> starting at time <t>");
   opts.Add(0, "SL-", "SV,OT,t,l", true, false, &SLm, "",
            "Stop setting LLI(SV,OT) to value <l> at time <t>");

   opts.Add(0, "BZ", "", false, false, &BZ,
            "# Bias cmds: (BD cmds apply only when data is non-zero, unless --BZ)",
            "Apply BD command even when data is zero (i.e. 'missing')");
   opts.Add(0, "BS", "SV,OT,t,s", true, false, &BS, "",
            "Add the value <s> to SSI(SV,OT) at single time <t>");
   opts.Add(0, "BL", "SV,OT,t,l", true, false, &BL, "",
            "Add the value <l> to LLI(SV,OT) at single time <t>");
   opts.Add(0, "BD", "SV,OT,t,d", true, false, &BD, "",
            "Add the value <d> to data(SV,OT) at single time <t>");
   opts.Add(0, "BD+", "SV,OT,t,d", true, false, &BDp, "",
            "Add the value <d> to data(SV,OT) beginning at time <t>");
   opts.Add(0, "BD-", "SV,OT,t,d", true, false, &BDm, "",
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

}  // end Configuration::BuildCommandLine()

//------------------------------------------------------------------------------------
int Configuration::ExtraProcessing(string& errors, string& extras) throw()
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
   parseEditCmds(OF, "OF", oss);
   parseEditCmds(DA, "DA", oss);
   parseEditCmds(DAp, "DA+", oss);
   parseEditCmds(DAm, "DA-", oss);
   parseEditCmds(DO, "DO", oss);
   parseEditCmds(DS, "DS", oss);
   parseEditCmds(DSp, "DS+", oss);
   parseEditCmds(DSm, "DS-", oss);
   parseEditCmds(DD, "DD", oss);
   parseEditCmds(DDp, "DD+", oss);
   parseEditCmds(DDm, "DD-", oss);
   parseEditCmds(SD, "SD", oss);
   parseEditCmds(SS, "SS", oss);
   parseEditCmds(SL, "SL", oss);
   parseEditCmds(SLp, "SL+", oss);
   parseEditCmds(SLm, "SL-", oss);
   parseEditCmds(BD, "BD", oss);
   parseEditCmds(BDp, "BD+", oss);
   parseEditCmds(BDm, "BD-", oss);
   parseEditCmds(BS, "BS", oss);
   parseEditCmds(BL, "BL", oss);

   // dump commands for debug
   //TEMP if(debug > -1)
      //for(i=0; i<vecCmds.size(); i++)
         //LOG(INFO) << vecCmds[i].asString(" Input Edit cmd:");

   // 'fix up' list of edit cmds: sort, add -(+) for unmatched +(-), find + > -
   FixEditCmdList();

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

} // end Configuration::ExtraProcessing(string& errors) throw()

//------------------------------------------------------------------------------------
// little helper routine for ExtraProcessing
void Configuration::parseEditCmds(vector<string>& vec, const string lab,
                                                           ostringstream& os) throw()
{
   for(size_t i=0; i<vec.size(); i++) {
      EditCmd ec(lab,vec[i]);
      if(ec.isValid()) vecCmds.push_back(ec);
      else os << "Error: invalid argument in " << lab << " cmd: >" << vec[i] << "<\n";
   }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// constructor from strings, i.e. parser e.g. "DA+","t" or "BDp","SV,OT,t,s"
EditCmd::EditCmd(const string intypestr, const string inarg) throw(Exception)
{
   try {
      string tag(upperCase(intypestr)), arg(inarg);
      vector<string> flds;

      type = INVALID;                                 // defaults
      ttag = CommonTime::BEGINNING_OF_TIME;
      sign = idata = 0;
      data = 0.0;

      if(tag.size() == 2) sign = 0;                   // pull off sign
      else if(tag[2] == '+') sign = 1;
      else if(tag[2] == '-') sign = -1;
      else return;
      tag = tag.substr(0,2);

      flds = split(arg,',');                          // split arg
      const int n(flds.size());                       // number of args

      if(tag == "OF") {
         if(n != 1 && n != 3 && n != 7) return;
         field = flds[0];
         if(n != 1) {
            stripLeading(arg,field+",");
            if(!parseTime(arg,ttag)) return;
         }
         type = OF;
      }
      else if(tag == "DA") {
         if(!parseTime(arg,ttag)) return;
         type = DA;
      }
      else if(tag == "DO") {
         if(sign != 0) return;                        // no DO+ or DO-

         if(arg.size() == 4)                          // get sys
            sat.fromString(string(1,arg[0]));
         // else sat sys is unknown

         if(isValidRinexObsID(arg)) obs = RinexObsID(arg);
         else return;

         type = DO;
      }
      else if(tag == "DS") {
         if(n != 1 && n != 3 && n != 7) return;    // DS DS,w,sow and DS,y,m,d,h,m,s
         try { sat.fromString(flds[0]); } catch(Exception) { return; }
         if(n != 1) {                              // time for DS is BeginTime
            stripLeading(arg,flds[0]+",");
            if(!parseTime(arg,ttag)) return;
         }
         type = DS;
      }
      else {
         // args are SV,OT,t[,d or s or l]
         if(n < 4) return;                            // at minimum SV,OT,week,sow

         stripLeading(arg,flds[0]+","+flds[1]+",");   // remove 'SV,OT,' from arg

         string dat;
         if(tag != "DD") {                            // strip and save last arg (dsl)
            dat = flds[flds.size()-1];
            stripTrailing(arg,","+dat);
         }
         if(!parseTime(arg,ttag)) return;             // get the time

         // parse satellite
         try { sat.fromString(flds[0]); } catch(Exception) { return; }

         // add system char to obs string
         if(flds[1].size() == 3 && sat.systemChar() != '?')
            flds[1] = string(1,sat.systemChar()) + flds[1];
         // parse obs type
         if(isValidRinexObsID(flds[1])) obs = RinexObsID(flds[1]); else return;

         if(tag == "DD") { type = DD; return; } // DD is done

         if(n != 5 && n != 9) return;           // rest have SV,OT,t,d = 5 or 9 args

         if(tag == "SD" || tag == "BD") {       // double data
            if(isScientificString(dat)) data = asDouble(dat); else return;
         }
         else {                                 // rest have int data
            if(isDigitString(dat)) idata = asInt(dat); else return;
         }

         // now just set type
         if(tag == "SD") type = SD;
         else if(tag == "SS") type = SS;
         else if(tag == "SL") type = SL;
         else if(tag == "BS") type = BS;
         else if(tag == "BL") type = BL;
         else if(tag == "BD") type = BD;
      }
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------
// dump, with optional message
string EditCmd::asString(string msg) throw(Exception)
{
   try {
      Configuration& C(Configuration::Instance());
      static map<CmdType,string> typeLabel;
      if(typeLabel.size() == 0) {
         typeLabel[INVALID] = string("--INVALID--   ");
         typeLabel[OF] = string("OF_Output_File");
         typeLabel[DA] = string("DA_Delete_All ");
         typeLabel[DO] = string("DO_Delete_Obs ");
         typeLabel[DS] = string("DS_Delete_Sat ");
         typeLabel[DD] = string("DD_Delete_Data");
         typeLabel[SD] = string("SD_Set_Data   ");
         typeLabel[SS] = string("SS_Set_SSI    ");
         typeLabel[SL] = string("SL_Set_LLI    ");
         typeLabel[BD] = string("BD_Bias_Data  ");
         typeLabel[BS] = string("BS_Bias_SSI   ");
         typeLabel[BL] = string("BL_Bias_LLI   ");
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
   catch(exception& e) {
      Exception E(string("std::except: ") + e.what());
      GPSTK_THROW(E);
   }
}

//------------------------------------------------------------------------------------
void FixEditCmdList(void) throw()
{
   Configuration& C(Configuration::Instance());
   vector<EditCmd>::iterator it, jt;
   vector<EditCmd> newCmds;

   // sort on time
   sort(C.vecCmds.begin(), C.vecCmds.end(), EditCmdLessThan());

   // ensure each - command has a corresponding + command
   // (note that + cmds do not need a - cmd: they will just never be turned off)
   for(it = C.vecCmds.begin(); it != C.vecCmds.end(); ++it ) {
      if(it->sign == -1 && it->type != EditCmd::INVALID) {
         bool havePair(false);
         if(it != C.vecCmds.begin()) {
            --(jt = it);
            while(1) {                                // search backwards for match
               if(jt->type == it->type && jt->sat == it->sat) {
                  if(jt->sign == 1) havePair=true;    // its a match
                  else if(jt->sign == -1) {           // this is an error
                     LOG(ERROR) << it->asString("Error: repeat '-'");
                     it->type = EditCmd::INVALID;
                     break;
                  }
               }
               if(jt == C.vecCmds.begin()) break;
               --jt;
            }
         }
         if(!havePair && it->type != EditCmd::INVALID) {
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

   // remove INVALID commands
   it = C.vecCmds.begin();
   while(it != C.vecCmds.end()) {
      if(it->type == EditCmd::INVALID)
         it = C.vecCmds.erase(it);              // erase vector element
      else
         ++it;
   }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
