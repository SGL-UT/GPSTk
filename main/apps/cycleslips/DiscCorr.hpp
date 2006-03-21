//------------------------------------------------------------------------------------
// DiscCorr.hpp  GPS phase discontinuity correction. Given arrays containing
//    dual-frequency pseudorange and phase (see class SVPass herein),
//    detect discontinuities in the phase and if possible estimate their size.
//    Output is in the form of Rinex editing commands (see class RinexEditor).
//
// DiscCorr is part of the GPS Tool Kit (GPSTK) developed in the
// Satellite Geophysics Group at Applied Research Laboratories,
// The University of Texas at Austin (ARL:UT), and was written by Dr. Brian Tolman.
//------------------------------------------------------------------------------------
#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/cycleslips/DiscCorr.hpp#1 $"

/**
 * @file DiscCorr.hpp
 * GPS phase discontinuity correction.
 */

//------------------------------------------------------------------------------------
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

#ifndef GPSTK_DISCONTINUITY_CORRECTOR_INCLUDE
#define GPSTK_DISCONTINUITY_CORRECTOR_INCLUDE

#include "DayTime.hpp"
#include "RinexObsHeader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace gpstk {

   /** @addtogroup rinexutils */
   //@{

   /// class GDCConfig encapsulates the configuration for input to the
   /// GPSTK Discontinuity Corrector.
class GDCConfig
{
public:
//Misc
      /// If true, print the syntax message and quit.
   bool help;
      /// Control amount of debugging output to log file, as follows.
      ///  0   nothing
      ///  1   Summary of input
      ///  2   High level actions, change in GDC config, results summary, timing
      ///  3   Actual results (Rinex Editor commands), stats on SVPass smoothing
      ///  4   All SVPass information - defined, filled, processed, done, etc.
      ///  5   List of GDC segments, dump data: before,linear combo, WL, GF, after
      ///      processing.
      ///  6   Dump WL statistical, and GF range fit, data
      ///  7   Editing actions, outliers, GFR fit and slip fixing stats and decisions.
      /// Output for Debug > 2 is quite large.
   int Debug;
      /// Stream on which to write debug output.
   std::ostream *oflog;
      /// nominal data time step (seconds); this MUST be set by caller.
   double DT;
//Editing
      /// Minimum allowed pseudorange, (meters)
   double MinRange;
      /// Maximum allowed pseudorange, (meters)
   double MaxRange;
//Segment definition
      /// Largest acceptable time gap within a segment (seconds)
   double MaxGap;
      /// Smallest acceptable length of a phase segment (seconds)
   double MinSeg;
      /// Smallest acceptable number of points in a phase segment ()
   int MinPts;
//Wide-lane bias slip detection
      /// N sigma: WL R-Ph is compared to N*sigma(WL R-Ph) ()
   double WLNSigma;
      /** Width of sliding statistics window when WL small slips are detected;
         (multiples of minimum width, which comes from MinSeg parameter). */
   int WLSSWindowWidth;
      /** Minimum value of difference in average WL bias which triggers a test
         of WL small slips. */
   double WLSSTestMin;
      /** Minimum value of peak in delta-WL-average which detects a WL
         small slip. */
   double WLSSDetectMin;
//Wide-lane bias slip estimation
      /// Minimum value for N*Sig(WL) (1 WL cycle)
      /// Minimum number of points in each segment needed for slip fixing
   unsigned WLFixNpts;
      /// Maximum gap between segments allowed for WL slip fixing (sec)
   double WLFixMaxGap;
      /** Maximum total error (StdDev/sqrt(N)) on WL R-Ph allowed for slip
         fixing (WL cycles) */
   double WLFixSigma;
//Geometry-free slip detection 
      /// Maximum degree of polynomial fit to geometry-free range ()
   int GFPolyMaxDegree;
      /// Maximum allowed RMS residual of polynomal fit to geometry-free range ().
   double GFRMaxRMSFit;
      /// Maximum allowed error in GF residual for detection of GF slips ().
   double GFDetectMaxSigma;
      /// Tolerance on GF slips when no WL slip detected (cycles of WL1-WL2)
   double GFOnlySlipTol;
      /// Tolerance on GF slips when a WL slip was detected (cycles of WL1-WL2)
   double GFSlipTol;
      /// Maximum # of consecutive outliers in GF allowed, else slip
   int GFNOutliers;
//Geometry-free phase slip estimation
      /// Maximum gap between segments allowed for GF slip fixing (sec)
   double GFFixMaxGap;
      /// Time period of data used in fit of GFPhase when fixing slips (sec)
   int GFFitTime;
      /// Minimum number of good data points used in GFPhase fit (); NB may
      /// be doubled, then halved, if the algorithm has trouble.
   int GFFitNMin;
      /// Maximum RMS residual of fit allowed in slip fixing (GF cycles)
   double GFFixSigma;
      /// Maximum value of fractional part of slip in slip fixing
//Output 
      /// Flag for output format of time-tags: GPS (T) or YMDHMS (F).
   bool OutputGPSTime;
      /// Flag to include deleting outliers in the editing command output.
   bool FixOutput;


      /// constructor; this sets a full default set of parameters.
   GDCConfig(void);
      // destructor
   //~GDCConfig(void);
      /// Set a parameter in the configuration;
      /// The input string 'Pcmd' is of the form '<id>S<value>' where the
      /// separator S is either ',' '=' or ':' ;
      /// to see a list of possible choices, call with Pcmd='help'.
      /// An optional leading '--DC' is permitted.
   void SetParameter(std::string Pcmd);
      /// Print current values of all parameters to the given ostream.
   void PrintParameters(std::ostream& os);
      /// Print help page to the ostream.
   void DisplayParameterUsage(std::ostream& os);
};

//------------------------------------------------------------------------------------
/** class SVPass holds all range and phase data for a full satellite pass.
  * Constructed and filled by the calling program, it is used to pass data into
  * and out of the GPSTK discontinuity corrector.
  */
class SVPass {
public:
   /// Values for the Flag array, marking good data.
   static const int OK;
   /// Values for the Flag array, marking bad data.
   static const int BAD;
   /// Values for the Flag array, marking bad data, used internally.
   static const int SETBAD,GFBAD;
   /// Values for the Flag array, marking slips found.
   static const int SLIPWL,SLIPGF,SLIP;
   /// Values for the Flag array, marking slips fixed.
   static const int FIXWL,FIXGF,FIX;

   /// flag used internally to indicated extra arrays (A1,A2) are needed.
   bool Extra;
   /// Length of data arrays.
   int Npts,Length;
   /// biases removed from the data.
   double bias1,bias2;
   /// Satellite identifier for this data.
   RinexPrn SV;
   /// Start time of this data.
   DayTime BegTime;
   /// End time of this data.
   DayTime EndTime;
   /// Array used to mark the data.
   int *Flag;
   /// Arrays holding dual frequency pseudorange (meters) and phase (cycles).
   double *P1,*P2,*L1,*L2;
   /// Extra arrays used internally.
   double *A1,*A2;
   /// Extra arrays used internally.
   double *E1,*E2;

   /// Default constructor
   SVPass(void);
   /// Destructor
   ~SVPass(void);
   /// Resize the array to length n.
   void Resize(int n);
};

/// class SVPLessThan is for use by the sort() function.
class SVPLessThan {     
public:
   bool operator()(const SVPass& c1, const SVPass& c2)
      { return c1.BegTime < c2.BegTime; }
};

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
/** GPSTK Discontinuity Corrector. Find, and fix if possible, discontinuities
 * in the GPS carrier phase data, given dual-frequency pseudorange and phase
 * data for an entire satellite pass. Input is the SVPass object holding the
 * data (with Npts=number of good data points), and a GDCConfig configuration
 * object giving the parameter values for the corrector. Output is in the form
 * of a list of strings giving editing commands which can be parsed and
 * applied using the GPSTK Rinex Editor (see Prgm EditRinex and the RinexEditor
 * class). Also, the L1 and L2 arrays in the input SVPass are corrected
 * (the P1 and P2 arrays are necessarily trashed). The routine will also mark
 * outliers in the input data.
 * @param SP SVPass object containing the input data.
 * @param GDC GDCConfig configuration object.
 * @param EC vector<string> (output) containing RinexEditor commands.
 * @return 0 for success, otherwise return an Error code;
 * codes are defined as follows.
 * const int FatalProblem=-3;
 * const int PrematureEnd=-2;
 * const int Singular=-1;
 * const int ReturnOK=0;
 */
int GPSTKDiscontinuityCorrector(SVPass& SP, GDCConfig& GDC,
   std::vector<std::string>& EC);

   //@}

}  // end namespace gpstk

//------------------------------------------------------------------------------------
#endif
