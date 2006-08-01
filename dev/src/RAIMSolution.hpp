#pragma ident "$Id$"


/**
 * @file RAIMSolution.hpp
 * Autonomous pseudorange navigation solution, including RAIM algorithm
 */
 
#ifndef RAIM_POSITION_SOLUTION_HPP
#define RAIM_POSITION_SOLUTION_HPP

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

#include <vector>
#include <ostream>
#include "icd_200_constants.hpp"
#include "DayTime.hpp"
#include "Matrix.hpp"
#include "RinexObsHeader.hpp"
#include "EphemerisStore.hpp"
#include "TropModel.hpp"

using namespace std;
using namespace gpstk;

namespace gpstk
{
   /** @defgroup GPSsolutions GPS solution algorithms and Tropospheric models */
   //@{
 
   /** This class define an interface to routines which compute a position
    * and time solution from pseudorange data, with a data editing algorithm
    * based on Receiver Autonomous Integrity Monitoring (RAIM) concepts.
    */
   class RAIMSolution
   {
   public:
         /// Constructor
      RAIMSolution() throw(Exception) : Valid(false), Algebraic(false),
         ResidualCriterion(true), RMSLimit(6.5), SlopeLimit(100.),
         NSatsReject(-1), ReturnAtOnce(false), MaxNIterations(10),
         ConvergenceLimit(3.e-7), Debug(false), pDebugStream(&cout) {};

      /** Compute the position/time solution, given satellite PRNs and pseudoranges
       * 'Tr'          Measured time of reception of the data.
       * 'Satellite'   Vector of satellites; on successful return, satellites that
       *               were excluded by the algorithm are marked by a negative
       *               Satellite[i].prn
       * 'Pseudorange' Vector of raw pseudoranges (parallel to satellite), in meters.
       *               On successful return, contains residual of fit (m)
       *               (for unmarked satellites).
       * 'Eph'         EphemerisStore to be used.
       *
       * Return values:
       *  2  solution found, but it is not good (RMS residual exceed limits)
       *  1  solution found, but it is suspect (slope is large)
       *  0  ok
       * -1  failed to converge
       * -2  singular problem
       * -3  not enough good data to form a RAIM solution
       *     (the 4 satellite solution might be returned - check isValid())
       * -4  ephemeris not found for one or more satellites
       */
      int Compute(const DayTime& Tr, vector<RinexPrn>& Satellite,
         vector<double>& Pseudorange, const EphemerisStore& Eph,
         TropModel *pTropModel) throw(Exception);

         /// Return status of solution (output) components of the class
      bool isValid() const throw() { return Valid; }

      // input:

      /// RMS limit - either residual of fit or distance (see ResidualCriterion).
      double RMSLimit;

      /// Slope limit.
      double SlopeLimit;

      /// Use an algebraic (if true) or linearized least squares (if false) algorithm.
      bool Algebraic;

      /** Use a rejection criterion based on RMS residual of fit (true)
       * or RMS distance from an a priori position. If false, Solution
       * must be defined as this a priori position when Compute() is called.
       */
      bool ResidualCriterion;

      /** Return as soon as a solution meeting the limit requirements is found
       * (this makes it a non-RAIM algorithm).
       */
      bool ReturnAtOnce;

      /** Maximum number of satellites that may be rejected; if -1, as many as
       * possible (at least 5 are required). A (single) non-RAIM solution can be
       * obtained by setting this to 0 before calling compute().
       */
      int NSatsReject;

      /// If true, Compute() will output solution information to *pDebugStream.
      bool Debug;

      /// Pointer to an ostream, default &cout; if Debug is true, Compute() will print
      /// all preliminary solutions, along with debug information, to it.
      ostream *pDebugStream;

      // TD optional: measurement covariance matrix

      /// Maximum number of iterations allowed in the linearized least squares
      /// algorithm.
      int MaxNIterations;

      /// Convergence limit (m): continue iteration loop while RSS change in
      /// solution exceeds this.
      double ConvergenceLimit;

      // output:

      /// flag: output content is valid.
      bool Valid;

      /** Vector<double> containing the computed position solution (ECEF, meter);
       * valid only when isValid() is true.
       */
      Vector<double> Solution;

      /** 4x4 Matrix<double> containing the computed solution covariance (meter);
       * valid only when isValid() is true.
       */
      Matrix<double> Covariance;

      /** Root mean square residual of fit (except when RMSDistanceFlag is set,
       * then RMS distance from apriori 4-position); in meters.
       */
      double RMSResidual;

      /** Slope computed in the RAIM algorithm (largest of all satellite values)
       * for the returned solution, dimensionless ??.
       */
      double MaxSlope;

      /// the actual number of iterations used (linearized least squares algorithm)
      int NIterations;

      /// the RSS change in solution at the end of iterations.
      double Convergence;

      /// the number of good satellites used in the final computation
      int Nsvs;

   private:

      /** Matrix, dimensioned Nx4, where N data are input, containing satellite
       * positions at transmit time (0,1,2) and raw pseudorange+clk+relativity (3).
       */
      Matrix<double> SVP;

      /// Save the input solution (for use in rejection when ResidualCriterion is
      /// false).
      Vector<double> APrioriSolution;

      /// fuction used by RAIM algorithm to cycle through all combinations of
      /// satellites
      int IncrementMarkedIndexes(Vector<int>& I, int& n,
         Vector<bool>& V, int& Vd, int& st);

   }; // end class RAIMSolution

   /** Compute the satellite position / corrected range matrix (SVP); used by
    * AutonomousPRSolution(). SVP is output, dimensioned (N,4) where N is the number
    * of satellites and the length of both Satellite and Pseudorange. Data is ignored
    * whenever Satellite[i].prn is < 0.
    * Return values:
    *  0  ok
    * -4  ephemeris not found for one or more satellites
    */
   int PrepareAutonomousSolution(const DayTime& Tr, vector<RinexPrn>& Satellite,
      vector<double>& Pseudorange, const EphemerisStore& Eph, Matrix<double>& SVP);

   /** Compute a single autonomous pseudorange solution.
    * Input:
    *   DayTime Tr           data time tag (for use by some trop models)
    *   Vector<bool> Use     of length N, the number of satellites; if value is
    *                           false, do not include it in the computation.
    *   Matrix<double> SVP   of dimension (N,4).  This Matrix must have been
    *                           computed by calling PrepareAutonomousPRSolution().
    *   bool Algebraic       flag indicating algebraic (true) algorithm, or
    *                           linearized least squares (false).
    *   pTropModel           pointer to TropModel for use within the algorithm
    *
    *   Weight matrix TD......
    *
    * Input and output (for least squares only; ignored if Algebraic==true):
    *   int n_iterate         limit on iterations. On output, the number of iterations
    *                            used.
    *   double converge       convergence criterion (RSS change in solution, meters).
    *                            On output, the final value.
    * Output:  (these will be resized within the function)
    *   Vector<double> Sol    solution (ECEF & time; all in meters) length 4
    *   Matrix<double> Cov    covariance matrix (meter*meter) dimension 4x4
    *   Vector<double> Resid  range residuals for each satellite (m),
    *                            length N-(number of satellites with Use=true).
    *   Vector<double> Slope  slope value used in RAIM for each good satellite,
    *                            length N
    * Return values:
    *  0  ok
    * -1  failed to converge
    * -2  singular problem
    * -3  not enough good data to form a solution (at least 4 satellites required)
    * -4  ephemeris not found for one or more satellites
    */
   int AutonomousPRSolution(const DayTime& Tr, const Vector<bool>& Use,
      const Matrix<double> SVP, TropModel *pTropModel, const bool Algebraic,
      int& n_iterate, double& converge, Vector<double>& Sol, Matrix<double>& Cov,
      Vector<double>& Resid, Vector<double>& Slope)
         throw(Exception);

   //@}

} // namespace gpstk

#endif
