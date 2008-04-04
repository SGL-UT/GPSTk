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
 * @file CommandInput.hpp
 * Include file for command line input, including defaults and
 * validation for program DDBase.
 */

//------------------------------------------------------------------------------------
// testing - not for release
//#define StochasticModelTest 1

//------------------------------------------------------------------------------------
#ifndef CLASS_DDBASE_COMMANDINPUT_INCLUDE
#define CLASS_DDBASE_COMMANDINPUT_INCLUDE

//------------------------------------------------------------------------------------
// includes
// system
// GPSTk
// DDBase
// put '#include this file' at bottom of DDBase.hpp #include "DDBase.hpp"

//------------------------------------------------------------------------------------
/// Class CommandInput encapsulates all the data input from command line.
class CommandInput {
public:
   // functions
   int GetCmdInput(int argc, char **argv) throw(gpstk::Exception);
   int ValidateCmdInput(void) throw(gpstk::Exception);
   void Dump(std::ostream& s=std::cout) const throw(gpstk::Exception);

   // member data
   bool Debug;
   bool Verbose;
   bool Screen;
   bool Validate;
   std::string LogFile;
   std::string InputPath;
   std::string NavPath;
   std::string EOPPath;
   std::string OutPath;
   std::vector<std::string> NavFileNames;
   std::vector<std::string> EOPFileNames;
   std::string TimeTableFile;
   gpstk::DayTime BegTime;
   gpstk::DayTime EndTime;
   int Frequency;
      // stochastic models
   std::string StochasticModel;
#ifdef StochasticModelTest
   double SNRmax,SNRatt;
#endif // StochasticModelTest
      // for configuration of pseudorange solution
   double PRSrmsLimit;
   //double PRSslopeLimit;   // no
   bool PRSalgebra;
   //bool PRSresidCrit;      // no
   //bool PRSreturnatonce    // no
   int PRSnIter;
   double PRSconverge;
      // for use by pseudorange solution only
   double PRSMinElevation;
   gpstk::TropModel *pTropModel;          // Station has another for Estimation
   bool noRAIM;                           // not implemented right now
      // for modeling the residual zenith delay (RZD) of the troposphere
   int NRZDintervals;                     // 0 for no RZD estimation
   double RZDtimeconst;                   // hours
   double RZDsigma;                       // meters
      // 
   double DataInterval;
      // editing
   double MinElevation;
   double RotatedAntennaElevation;
   double RotatedAntennaAzimuth;
   int MaxGap;
   int MinDDSeg;
   int PhaseBiasReset;
   std::vector<gpstk::GSatID> ExSV;
      // timetable
   gpstk::GSatID RefSat;
      // Estimation
   bool noEstimate;
   int nIter;
   double convergence;
   bool FixBiases;
   double TightConstraint,LooseConstraint;// in ppm (of baseline)
   double DefaultTemp,DefaultPress,DefaultRHumid;
      // output
   std::vector<std::string> OutputBaselines;
   std::vector<gpstk::Triple> OutputBaselineOffsets;
   std::string OutputClkFile;
   std::string OutputRawDDFile;
   std::string OutputDDDFile;
   std::string OutputTDDFile;
   std::string OutputRawFile;
   std::string OutputPRSFile;
   std::string OutputDDRFile;

private:
   bool help;
   void SetDefaults() throw(gpstk::Exception);
   void PreProcessArgs(const char *arg, std::vector<std::string>& Args)
      throw(gpstk::Exception);
};    // end class CommandInput

//------------------------------------------------------------------------------------
extern CommandInput CI;

#endif
// nothing below this
//------------------------------------------------------------------------------------
