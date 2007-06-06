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
 * @file RinexEditor.hpp
 * Edit Rinex observation files.
 */

#ifndef RINEX_EDITING_COMMANDS_INCLUDE
#define RINEX_EDITING_COMMANDS_INCLUDE

//------------------------------------------------------------------------------------
#include "Exception.hpp"
#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "DayTime.hpp"
#include "RinexSatID.hpp"

#include <fstream>
#include <string>
#include <vector>
#include <deque>

//------------------------------------------------------------------------------------
namespace gpstk {

   /** @addtogroup rinexutils */
   //@{

   // forward declarations
class RinexEditor;
class REditCmdLessThan;
class TableData;
class TablePRNLessThan;

//------------------------------------------------------------------------------------
/// Class REditCmd encapsulates commands passed to the Rinex Editor
/// (class RinexEditor).
class REditCmd {
   friend class RinexEditor;
   friend class REditCmdLessThan;
   friend bool operator==(const REditCmd& x, const REditCmd& y);
private:
   /// The command types: input file, output file, etc.
   enum TYPE {
         //(order matters)
      INVALID=0,IF,OF,ID,OD,HD,TN,TB,TE,TT,AO,DA,DO,DS,DD,SD,SS,SL,BD,BS,BL,BZ
   };
   /// The type of this command
   TYPE type;
   /// Satellite identifier for data to which this command will apply.
   RinexSatID SV;
   /// Time associated with this command.
   DayTime time;
   /// Sign associated with this command.
   int sign;
   /// String associated with this command.
   std::string field;
   /// integer associated with this command.
   int inOT;
   /// bias associated with this command.
   double bias;
   /// map giving label as a string for each command type
   static std::map<TYPE, std::string> typeLabel;

public:
   /// Default constructor (type is set INVALID).
   REditCmd(void) { type=INVALID; }
   /// Destructor
   ~REditCmd(void);
   /// Constructor from a string which contains the editing command.
   REditCmd(std::string s, std::ostream *oflog=&std::cout) throw(Exception);
   /// Is this a valid command?
   inline bool valid(void) throw() { return (type!=INVALID); }
   /// Print the command on an ostream, with an optional message.
   void Dump(std::ostream& os, std::string msg) throw(Exception);

   class Initialize {
      public: Initialize();
   };
   static Initialize REditCmdInitializer;
};

   /// operator==(REditCmd), defined so algorithm find() can be used to find
   /// the "-" companion to "+" commands.
inline bool operator==(const REditCmd& x, const REditCmd& y)
   { return (x.type == y.type &&
             x.SV == y.SV &&
             //x.time == y.time &&
             x.sign == -y.sign &&         // note the -
             x.field == y.field &&
             //x.bias == y.bias &&
             x.inOT == y.inOT); }

/// class REditCmdLessThan, for use with algorithm sort().
class REditCmdLessThan {      
public:
   /// return true if c1 is less than c2.
   bool operator()(const REditCmd& c1, const REditCmd& c2)
      { return c1.time < c2.time; }
};

//------------------------------------------------------------------------------------
/// class RinexEditor encapsulates the process of editing a Rinex observation dataset
/// (header and observations), including the editing commands that have been input
/// from the user.
class RinexEditor {
private:
   /// input and output file names.
   std::string InputFile,OutputFile;
   /// directories for the input and output files.
   std::string InputDir,OutputDir;
   /// start and stop times, for windowing the data.
   DayTime BegTime,EndTime;
   /// tolerance to be used in comparing time tags.
   double TimeTol;
   /// time step interval if the data is to be decimated.
   double Decimate;
   /// flag to indicate how to handle data with value 'zero'.
   bool BiasZeroData;
   /// flag used when data is omitted from output
   bool Skip;
   /// vector of Rinex observation types in header.
   std::vector<RinexObsHeader::RinexObsType> ObsTypes;
   /// vector of satellites to be deleted.
   std::vector<RinexSatID> DelSV;
   /// input and output Rinex headers.
   RinexObsHeader RHIn,RHOut;
   /// flag for editing the Rinex header.
   bool FillOptionalHeader,HDDeleteOldComments;
   /// flags indicating validity of optional records in the input header.
   bool IVLast,IVInterval,IVTable;
   /// content of the PROGRAM header record.
   std::string HDProgram;
   /// content of the RUN BY header record.
   std::string HDRunBy;
   /// content of the OBSERVER header record.
   std::string HDObserver;
   /// content of the AGENCY header record.
   std::string HDAgency;
   /// content of the POSITION header record, in the form 'x,y,z'
   std::string HDPosition;
   /// content of the MARKER header record.
   std::string HDMarker;
   /// content of the NUMBER header record.
   std::string HDNumber;
   /// comments in the Rinex header.
   std::vector<std::string> HDComments;
   /// times for computing start and stop times, to go in the Rinex header.
   DayTime CurrEpoch,PrevEpoch;
   /// an integer array for computing the time interval, to go in the Rinex header.
   int ndt[9];
   /// a double array for computing the time interval, to go in the Rinex header.
   double bestdt[9];
   /// storage for the PRN/OBS table, to go in the Rinex header.
   std::vector<TableData> table;

      /// Rinex Editing commands that will have to be saved.
   std::deque<REditCmd> Cmds;
      /// Rinex Editing commands for use during processing.
   std::vector<REditCmd> OneTimeCmds;
      /// Rinex Editing commands for use in the current timestep.
   std::vector<REditCmd> CurrentCmds;

public:
      /// flag to control debugging and analysis output.
   bool REVerbose,REDebug;
      /// the output log file stream.
   std::ostream *oflog;

      /// Default constructor. NB. Do not instantiate a RinexEditor outside of
      /// main(), as static initialization order on some OSs (Solaris) mean that
      /// DayTime::END_OF_TIME may not be defined at that point.
   RinexEditor(void);
      /// Destructor
   virtual ~RinexEditor(void);
      /// return string giving the editor version
   std::string getRinexEditVersion(void);
      /// pretty print configuration
   std::ostream& operator<<(const std::ostream& os);
      /// Add the Rinex Editing command structures to the user's command line.
   void AddCommandLine(std::vector<std::string>& args) throw(Exception);
      /// Add a Rinex Editing command to this Editor.
   void AddCommand(std::string cmd) throw(Exception);
      /// Parse the command line for Rinex Editing commands.
   int ParseCommands(void) throw(Exception);
      /// Edit the input header to produce the output header.
   int EditHeader(RinexObsHeader& RHIn, RinexObsHeader& RHOut) throw(Exception);
      /// Edit the input observation to produce the output observation.
   int EditObs(RinexObsData& ROIn, RinexObsData& ROOut) throw(Exception);
      /// Edit a Rinex observation file, using the stored Rinex Editing commands.
   int EditFile(void) throw(Exception);
      /// used to add optional records to the header.
   int FillHeaderAndReplaceFile(std::string& TempFile,std::string& TrueOutputFile) throw(Exception);

   /// This function is called after reading input header and before
   /// calling EditHeader (pass input header).
   virtual int BeforeEditHeader(const RinexObsHeader& rhin) throw(Exception)
      { return 0; }

   /// This function is called after calling EditHeader (pass it the output header).
   virtual int AfterEditHeader(const RinexObsHeader& rhout) throw(Exception)
      { return 0; }

   /// This function is called after reading the input observation and before
   /// calling EditObs (pass it the input observation).
   virtual int BeforeEditObs(const RinexObsData& roin) throw(Exception)
      { return 0; }

   /// This function is called before writing out the header (pass it
   /// the output header).
   virtual int BeforeWritingHeader(RinexObsHeader& rhout) throw(Exception)
      { return 0; }

   /// This function is called before writing out the header that has been
   /// filled with optional records
   virtual int BeforeWritingFilledHeader(RinexObsHeader& rhout) throw(Exception)
      { return 0; }

   /** Callback, just before writing output obs (pass output obs)
   * Return value of BeforeWritingObs determines what is written:
   * if return <0 write nothing and abort
   *            0 write nothing
   *            1 write the obs data roout ONLY (note that the caller may set
   *                roout.epochFlag to determine what is output : 0,1 are data,
   *                while 2,3,4 or 5, are for in-line header (roout.auxHeader)
   *                only -- see the Rinex or RinexObsData documentation)
   *           >1 write BOTH header data (in roout.auxHeader), first setting
   *                roout.epochFlag = the return value), AND the obs data
   *                in roout, using the original value of roout.epochFlag
   */
   virtual int BeforeWritingObs(RinexObsData& roout) throw(Exception)
      { return 0; }

   /// member access of the decimation time interval.
   double Decimation(void) { return Decimate; }
   /// member access of the time comparison tolerance.
   double Tolerance(void) { return TimeTol; }
   /// member access of the start time.
   DayTime BeginTimeLimit(void) { return BegTime; }
   /// member access of the end time.
   DayTime EndTimeLimit(void) { return EndTime; }
   /// member access of the input file name.
   std::string InputFileName(void) { return InputFile; }
   /// member access of the output file name.
   std::string OutputFileName(void) { return OutputFile; }
   /// member access input directory.
   std::string InputDirectory(void) { return InputDir; }
   /// member access output directory.
   std::string OutputDirectory(void) { return OutputDir; }
   /// member access command list, return vector of strings, each with
   /// comma-delimited fields: type,sign,sat,inOT,field,bias,time
   /// e.g. AO,0,G33,-1,SZ,-99.990,-4713/01/01,00:00:0.0000
   ///      IF,0,G33,-1,usno2930.06o.df,-99.990,-4713/01/01,00:00:0.0000
   ///      HD,0,G33,80,ResCor v.3.7 ,-99.990,-4713/01/01,00:00:0.0000
   std::vector<std::string> CommandList(void) throw(Exception);

}; // end class RinexEditor

//------------------------------------------------------------------------------------
/// class TableData is used to store the information in the PRN/Obs table in the
/// Rinex observation header.
class TableData {                      // class used to store PRN/Obs table
public:
   /// satellite identifier.
   RinexSatID prn;
   /// vector of the number of observations, parallel to the obs types in the header.
   std::vector<int> nobs;
   /// constructor, given a satellite id and the number of observation types.
   TableData(const RinexSatID& p, const int& n) { prn=p; nobs=std::vector<int>(n); };
   /// operator==(), needed for find() (compares prn only).
   inline bool operator==(const TableData& d) {return d.prn == prn;}
};

/// class define for use with sort(TableData).
class TablePRNLessThan  {
public:
   /// return true is d1 is less than d2 (compares prn only).
   bool operator()(const TableData& d1, const TableData& d2)
      { return d1.prn < d2.prn; }
};

   //@}

}  // end namespace gpstk

//------------------------------------------------------------------------------------
/// Pretty print the Rinex Editing command syntax, for use by the calling program.
void DisplayRinexEditUsage(std::ostream& os) throw();

//------------------------------------------------------------------------------------
#endif   // nothing below this
