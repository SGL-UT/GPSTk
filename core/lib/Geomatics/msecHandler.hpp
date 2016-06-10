/// @file millisecHandler.hpp  Detect and fix millisecond clock adjusts in data,
///   including pseudoranges, phases and time tags.

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

#ifndef GPSTK_MSHANDLER_INCLUDE
#define GPSTK_MSHANDLER_INCLUDE

#include <string>
#include <vector>
#include <map>

#include "Exception.hpp"
#include "CommonTime.hpp"
#include "SatID.hpp"

namespace gpstk {

// TD bool rmvClk;                  ///< NB currently hardcoded false

// -------------------------------------------------------------------------------
/// Class to detect, validate and remove millisecond clock adjusts from pseudorange
/// and phase data and timetags. Caller passes all the data to the object, in time
/// order, and the object detects where (in time and in observables) there are
/// millisecond adjusts. Caller can then pass data to the object again and have it
/// remove the appropriate adjusts from pseudorange, phase and/or timetags.
/// NB. adjusts should be removed from pseudorange and timetags together.
/// NB. most adjusted data has jumps in pseudorange, but not phase (violates RINEX!)
/// although the opposite (found in phase, but not pseudorange) does exist (job3326).
/// NB. some (Ashtech) usually also have jumps in the time tags; Trimble is variable.
class msecHandler
{
private:
   // member data
   double dt;                             ///< nominal time spacing

   // estimation
   int N;                                 ///< number of obstypes (6)
   std::vector<std::string> obstypes;     ///< obstypes to monitor (L1 L2 C1 C2 P1 P2)
   std::vector<double> wavelengths;       ///< wavelengths of obstypes - 0 for code
   CommonTime prevttag, currttag;         ///< for tracking timetags internally
   // keep the following parallel
   std::vector< std::map<SatID, double> > curr, past;  ///< storing data internally
   std::vector<double> ave;               ///< average step per obstypes
   std::vector<int> npt;                  ///< number of data per obstypes

   // consistency of adjusts
   // record results; typesMap = count adjusts/unique set obs types, should be only 1
   std::map<std::string, int> typesMap;  ///< [string "P1 C2 !L1 !L2"] = number of adj

   // messages to pass back
   std::string findMsg,fixMsg;

   // the adjusts - keep these parallel
   std::vector<CommonTime> times;               ///< time of adjust
   std::vector<int> nms;                        ///< number of ms
   std::vector< std::vector<std::string> > ots; ///< ots that jump
   std::vector<std::string> adjMsgs;            ///< adjust summary (i/o)
   std::vector<std::string> editCmds;           ///< adjust editing cmds
   std::vector<std::string> badMsgs;            ///< bad ones

   // fixing
   bool doPR;           ///< if adjusts appear in pseudorange, fix time tags also
   int ims,ntot;        ///< internal bookkeeping on which adjust, total ms adjust

   // piece-wise linear clock model
   bool rmvClk;                  ///< NB currently hardcoded false
   double intercept, slope;      ///< the piece-wise linear model
   CommonTime tref;              ///< reference time for the linear models

   //static const double wl1, wl2;
   static const double Rfact;//, L1fact, L2fact;      ///< one ms in m,L1cyc,L2cyc

   /// compute average differences and detect adjusts. call after all input at one
   /// time is done, passing it the _next_ timetag.
   /// @param ttag the next time
   void compute(CommonTime ttag);

public:
   /// empty and only constructor - note that setDT() must be called before processing
   msecHandler(void);

   /// Reset the object
   void reset(void);

   /// Re-define obstypes and wavelengths with parallel arrays.
   /// NB must set wavelength(code) = 0.
   /// @param ots vector<string> of obstypes to be passed to add() and fix().
   /// @param waves vector<double> wavelength of phases in ots or zero for codes.
   void setObstypes(const std::vector<std::string>& ots,
                    const std::vector<double>& waves);

   /// Get the obstypes used in the detector
   std::vector<std::string> getObsTypes(void) {
      std::vector<std::string> ots = obstypes;
      return ots;
   }

   /// must set the nominal timestep before any add() or fix()
   void setDT(double dt_in) { dt = dt_in; }

   /// add data at one epoch. May be repeated at the same epoch, but MUST be done in
   /// time order. NB assumes, as in RINEX, that data==0 means it is missing.
   /// @param ttag  time of the data
   /// @param sat   satellite of the data
   /// @param obstype string observation type of the data (L1 L2 C1 C2 P1 P2)
   /// @param data  value of obstype for sat at ttag
   void add(CommonTime ttag, SatID sat, std::string obstype, double data);

   /// After all add() calls, and before calling fix()
   /// @return number of fixes to apply
   int afterAddbeforeFix(void);

   /// edit data by removing the millisecond adjusts, and optionally a piece-wise
   /// linear model of the adjusts. Must be called in time order, as add() was.
   /// NB may call repeatedly with the same ttag, however
   /// NB ttag gets fixed every call, so don't keep calling with same variable ttag.
   /// @param ttag  time of the data
   /// @param sat   satellite of the data
   /// @param obstype string observation type of the data (L1 L2 C1 C2 P1 P2)
   /// @param data  value of obstype for sat at ttag
   void fix(CommonTime& ttag, SatID sat, std::string obstype, double& data);

   /// get messages generated during detection phase
   std::string getFindMessage(bool verbose=false);

   /// get find message in the form of editing commands for EditRinex
   std::vector<std::string> getEditCommands(void) { return editCmds; }

   /// get messages generated during fixing phase
   std::string getFixMessage(bool verbose=false) { return fixMsg; }

   /// get number of valid adjusts found
   int getNMS(void) { return times.size(); }

   /// get the number of invalid adjusts found
   int getNbadMS(void) { return badMsgs.size(); }

   /// get map<CommonTime,int> of valid adjusts; should parallel getFindMessage()
   std::map<gpstk::CommonTime, int> getAdjusts(void);

}; // end class msecHandler

} // end namespace

#endif // GPSTK_MSHANDLER_INCLUDE
