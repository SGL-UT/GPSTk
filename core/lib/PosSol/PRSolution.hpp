//==============================================================================
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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/// @file PRSolution.hpp
/// Pseudorange navigation solution, either a simple solution using all the
/// given data, or a solution including editing via a RAIM algorithm.
 
#ifndef PRS_POSITION_SOLUTION_HPP
#define PRS_POSITION_SOLUTION_HPP

#include <vector>
#include <ostream>
#include "stl_helpers.hpp"
#include "GNSSconstants.hpp"
#include "CommonTime.hpp"
#include "RinexSatID.hpp"
#include "Stats.hpp"
#include "Matrix.hpp"
#include "Namelist.hpp"
#include "XvtStore.hpp"
#include "TropModel.hpp"

namespace gpstk
{
   /** @defgroup GPSsolutions GPS solution algorithms and Tropospheric models */
   //@{

   /// Class WtdAveStats encapsulates statistics on the PR solution and residuals
   /// for a time series of data.
   class WtdAveStats {
   private:
      unsigned int N;
        //unused
      //double APV;
      std::string msg;
      std::string lab[3];
      Stats<double> S[3];
      Matrix<double> sumInfo;
      Vector<double> sumInfoState,Sbias;

   public:

      // ctor
      WtdAveStats(void)
      {
         reset();
         lab[0]="ECEF_X";  lab[1]="ECEF_Y"; lab[2]="ECEF_Z";
      }

      void setMessage(std::string m) { msg = m; }
      std::string getMessage(void) const { return msg; }

      void setLabels(std::string lab1, std::string lab2, std::string lab3)
         { lab[0]=lab1; lab[1]=lab2; lab[2]=lab3; }

      Vector<double> getSol(void) const
      {
         return (getCov()*sumInfoState + Sbias);
      }

      Matrix<double> getCov(void) const { return inverseSVD(sumInfo); }

      Matrix<double> getInfo(void) const { return sumInfo; }

      int getN(void) const { return N; }

      void reset(void)
      {
         N = 0;
         sumInfo = Matrix<double>();
         sumInfoState = Vector<double>();
         Sbias = Vector<double>(3);
         S[0].Reset();
         S[1].Reset();
         S[2].Reset();
      }

      // add to statistics, and to weighted average solution and covariance
      void add(const Vector<double>& Sol, const Matrix<double>& Cov)
      {
         try {
            // add to the statistics
            for(unsigned int i=0; i<3; i++) {
               if(N==0) Sbias(i) = Sol(i);
               S[i].Add(Sol(i)-Sbias(i));
            }

            // NB do NOT include clock(s); this can ruin the position average
            Vector<double> Sol3(Sol);
            Sol3.resize(3);               // assumes position states come first
            Sol3 = Sol3 - Sbias;
            Matrix<double> Cov3(Cov,0,0,3,3);

            // information matrix (position only)
            Matrix<double> Info(inverseSVD(Cov3));
            if(N == 0) {                  // first call: dimension and set to zero
               sumInfo = Matrix<double>(3,3,0.0);
               sumInfoState = Vector<double>(3,0.0);
            }

            // add to the total information
            sumInfo += Info;
            sumInfoState += Info * Sol3;
            ++N;
         }
         catch(Exception& e) { GPSTK_RETHROW(e); }
      }

      // dump statistics and weighted average
      void dump(std::ostream& os, std::string msg="") const
      {
         try {
            os << "Simple statistics on " << msg << std::endl
               << std::fixed << std::setprecision(3);
            if(N > 0) {
               os << "  " << lab[0] << " N: " << S[0].N()
                  << std::fixed << std::setprecision(4)
                  << " Ave: " << S[0].Average()+Sbias[0]
                  << " Std: " << S[0].StdDev()
                  << " Min: " << S[0].Minimum()+Sbias[0]
                  << " Max: " << S[0].Maximum()+Sbias[0]
                  << std::endl;
               os << "  " << lab[1] << " N: " << S[1].N()
                  << std::fixed << std::setprecision(4)
                  << " Ave: " << S[1].Average()+Sbias[1]
                  << " Std: " << S[1].StdDev()
                  << " Min: " << S[1].Minimum()+Sbias[1]
                  << " Max: " << S[1].Maximum()+Sbias[1]
                  << std::endl;
               os << "  " << lab[2] << " N: " << S[2].N()
                  << std::fixed << std::setprecision(4)
                  << " Ave: " << S[2].Average()+Sbias[2]
                  << " Std: " << S[2].StdDev()
                  << " Min: " << S[2].Minimum()+Sbias[2]
                  << " Max: " << S[2].Maximum()+Sbias[2]
                  << std::endl;

               os << "Weighted average " << msg << std::endl;
               Matrix<double> Cov(inverseSVD(sumInfo));
               Vector<double> Sol(Cov * sumInfoState + Sbias);
               os << std::setw(14) << std::setprecision(4) << Sol << "    " << N;
            }
            else os << " No data!";
         }
         catch(Exception& e) { GPSTK_RETHROW(e); }
      }

      friend std::ostream& operator<<(std::ostream& s, const WtdAveStats& as);

   }; // end class WtdAveStats

   /// This class defines an interface to routines which compute a position
   /// and time solution from pseudorange data, with a data editing algorithm
   /// based on Receiver Autonomous Integrity Monitoring (RAIM) concepts.
   /// RAIM ref. "A Baseline GPS RAIM Scheme and a Note on the Equivalence of
   /// Three RAIM Methods," by R. Grover Brown, Journal of the Institute of
   /// Navigation, Vol. 39, No. 3, Fall 1992, pg 301.
   ///
   /// The main point of entry is RAIMCompute(); it will compute a solution given
   /// the pseudoranges from a number of satellites, using a RAIM-based algorithm
   /// to detect and exclude 'bad' data from the solution. Alternately, the user
   /// may compute a straightforward solution using all the input data, without
   /// the RAIM algorithm; this is done by first calling PreparePRSolution()
   /// and then SimplePRSolution().
   ///
   /// The class is able to use satellite data from any GNSS (defined in SatID.hpp).
   /// The user MUST specify allowed GNSS in the vector allowedGNSS before processing.
   /// The algorithm estimates a separate clock bias for each system; the state
   /// vector (Solution) thus has components X,Y,Z,clk1,clk2,clk3... where the clocks
   /// are in the order given by the vector allowedGNSS. The time system of the clocks
   /// will be determined by the input ephemerides; usually IGS SP3 ephemerides use
   /// GPS time for all the systems (but there is still a system time offset relative
   /// to GPS for each other GNSS).
   /// 
   /// Note that at any epoch it may happen that no satellites from some system are
   /// available (either in the data or after the RAIM algorithm), in this case the
   /// clock bias for that system is undefined and set to zero.

   class PRSolution
   {
   public:
         /// Constructor
       PRSolution() : RMSLimit(6.5),
                      SlopeLimit(1000.),
                      NSatsReject(-1),
                      MaxNIterations(10),
                      ConvergenceLimit(3.e-7),
                      hasMemory(true),
                      fixedAPriori(false),
                      nsol(0), ndata(0), APV(0.0),
                      Valid(false)
         {
            was.reset();
            APSolution = Vector<double>(4,0.0);
         }

      /// Return the status of solution
      bool isValid() const { return Valid; }

      // input parameters: -------------------------------------------------

      /// RMS limit (m) on residual of fit
      double RMSLimit;

      /// Slope limit (dimensionless).
      double SlopeLimit;

      /// Maximum number of satellites that may be rejected in the RAIM algorithm;
      /// if this = -1, as many as possible will be rejected (RAIM requires at least 5
      /// satellites). A (single) non-RAIM solution can be obtained by setting this
      /// to 0 before calling RAIMCompute().
      int NSatsReject;

      /// Maximum number of iterations allowed in the linearized least squares
      /// algorithm.
      int MaxNIterations;

      /// Convergence limit (m): continue iteration loop while RSS change in
      /// solution exceeds this.
      double ConvergenceLimit;

      /// vector<SatID::SatelliteSystem> containing the satellite systems allowed
      /// in the solution. **This vector MUST be defined before computing solutions.**
      /// It is used to determine which clock biases are included in the solution,
      /// as well as the apriori state vector (see hasMemory below)
      std::vector<SatID::SatelliteSystem> allowedGNSS;

      /// This determines whether this object will maintain a "memory" of all the
      /// solutions it has computed. This is used for several things, including the
      /// computation of pre-fit residuals, and thus of the aposteriori variance of
      /// unit weight (APV), the number of data, solutions and degrees of freedom
      /// and a combined weighted average solution.
      /// Most importantly, it causes the estimation algorithm at each epoch to be
      /// initialized with an apriori solution, which it 'remembers' from previous
      /// epochs. If multiple GNSS are used in the estimation, at any epoch, then
      /// then setAprioriGNSS() must be called before any processing, otherwise the
      /// system clock of any missing system will not be part of the apriori state.
      bool hasMemory;

      // input and output: -------------------------------------------------

      /// vector<SatID> containing satellite IDs for all the satellites input, with
      /// bad (excluded) ones identified by (Satellite[.] < 0). This vector is saved
      /// after each call to the computation routines (SimplePRSolution and
      /// RAIMCompute) and used for printing.
      std::vector<SatID> SatelliteIDs;

      // output: -------------------------------------------------

      /// Vector<double> containing the computed position solution (3 components,
      /// ECEF in the frame of the ephemeris, meter), the receiver clock bias (m),
      /// and the GPS-GLO time offset (m). In the case of GPS-only or GLO-only
      /// data, the last element is zero and has no meaning. This vector is valid
      /// only when isValid() is true.
      /// If this vector is defined on input, it is used as an apriori position, both
      /// to initialized the iterative algorithm, and to compute position residuals
      /// after a good solution is found.
      Vector<double> Solution;

      /// Matrix<double> containing the computed solution covariance (meter^2);
      /// see doc. for Solution for the components; valid only when isValid() is true.
      Matrix<double> Covariance;

      /// Matrix<double> containing the inverse measurement covariance matrix (m^-2)
      /// that was used in computing the final solution.
      Matrix<double> invMeasCov;

      /// Matrix<double> containing the partials matrix used in the final solution.
      Matrix<double> Partials;

      /// vector<SatID::SatelliteSystem> containing the satellite systems found in
      /// the data at each epoch, after calls to SimplePRSolution and RAIMCompute.
      /// See also allowedGNSS.
      std::vector<SatID::SatelliteSystem> dataGNSS;

      /// The "memory" of this object, used only when hasMemory is true.
      WtdAveStats was;
      double APV;
      int ndata,nsol,ndof;
      /// if true, use the given APriori position instead of the current solution
      /// define by calling void fixAPSolution(X,Y,Z)
      bool fixedAPriori;
      Triple fixedAPrioriPos;
      /// Caller is responsible for setting APSolution before first call, if desired;
      /// after that SimplePRSolution() and RAIMCompute() will update it.
      Vector<double> APSolution;


      /// Prefit residuals; only valid if memory exists b/c it needs apriori solution.
      /// Vector<double> of 'pre-fit' residuals, computed by the solution routines,
      /// but only if APrioriSol is defined; equal to Partials*(Sol-APrioriSol)-Resid
      /// where Resid is the data residual vector on the first iteration.
      Vector<double> PreFitResidual;

      /// Root mean square residual of fit (except when RMSDistanceFlag is set,
      /// then RMS distance from apriori position); in meters.
      double RMSResidual;

      /// Slope computed in the RAIM algorithm (largest of all satellite values)
      /// for the returned solution, dimensionless.
      double MaxSlope;

      /// DOPs computed in a call to DOPCompute() or outputString()
      double TDOP,PDOP,GDOP;

      /// the actual number of iterations used
      int NIterations;

      /// the RSS change in solution at the end of iterations.
      double Convergence;

      /// the number of good satellites used in the final computation
      int Nsvs;

      /// if true, the returned solution may be degraded because the tropospheric
      /// correction was not applied to one or more satellites; applies after calls to
      /// both SimplePRSolution() and RAIMCompute().
      bool TropFlag;

      /// if true, the returned solution may be degraded because the RMS residual or
      /// the slope is large; applies only after calls to RAIMCompute().
      bool RMSFlag, SlopeFlag;

      // member functions -------------------------------------------

      /// Compute the satellite position / corrected range matrix (SVP) which is used
      /// by SimplePRSolution(). SVP is output, dimensioned (N,4) where N is the
      /// number of satellites and the length of both Satellite and Pseudorange.
      /// Data is ignored whenever Sats[i].id is < 0 and when system is not in
      /// allowedGNSS. NB caller should verify that the number of good entries
      /// (Satellite[.] > 0) is > 4 before proceeding.
      /// Even though this is a member function, it changes none of the member data.
      /// @param Tr          input Measured time of reception of the data.
      /// @param Sats        input std::vector<SatID> of satellites; satellites that
      ///                     are to be excluded by the algorithm are marked by a
      ///                     negative 'id' member; this call will mark satellites for
      ///                     which there is no ephemeris.
      /// @param Pseudorange input std::vector<double> of raw pseudoranges (parallel
      ///                     to Sats), in meters
      /// @param pEph        input pointer to gpstk::XvtStore<SatID> to be used
      /// @param SVP         output gpstk::Matrix<double> of dimension (N,4), N is
      ///                     the number of satellites in Sats[] (marked or not),
      ///                     on output this contains the satellite positions at
      ///                     transmit time (cols 0-2), the corrected pseudorange (1).
      /// @return Return values:
      ///  >= 0 number of good satellites found
      /// -4    ephemeris not found for all the satellites
      int PreparePRSolution(const CommonTime& Tr,
                            std::vector<SatID>& Sats,
                            const std::vector<double>& Pseudorange,
                            const XvtStore<SatID> *pEph,
                            Matrix<double>& SVP) const;

      /// Compute a single autonomous pseudorange solution, after calling
      /// PreparePRSolution(). On output, all the member data is filled with results.
      /// 
      /// Input only (first 3 should be just as returned from PreparePRSolution()):
      /// @param Tr          const. Measured time of reception of the data.
      ///                     On output member currTime set to this.
      /// @param Sats        const std::vector<SatID> of satellites. Satellites
      ///                     that are to be excluded by the algorithm are marked by a
      ///                     negative 'id' member. Length N.
      ///                     Also systems not in allowedGNSS are ignored.
      ///                     On output member SatelliteIDs set to this.
      /// @param SVP         const Matrix<double> of dimension (N,5) contains sat.
      ///                     direction cosines and corrected pseudorange data.
      /// @param invMC       const gpstk::Matrix<double> NXN measurement covariance
      ///                     matrix inverse (meter^-2) of the pseudorange data (for N
      ///                     see Sats). If this matrix has dimension 0, no weighting
      ///                     of the data is done.
      /// @param pTropModel  pointer to a gpstk::TropModel for trop correction.
      ///
      /// Input and output:
      /// @param niterLimit  integer limit on the number of iterations. On output,
      ///                     member NIterations = number of iterations actually used.
      /// @param convLimit   double convergence criterion, = RSS change in solution,
      ///                     in meters. On output, member Convergence = final value.
      ///
      /// Output:  (these will be resized within the function)
      /// @param Resids      gpstk::Vector<double> post-fit range residuals for each
      ///                     satellite (m), the length of this Vector is the number
      ///                     of satellites actually used (see Sats).
      /// @param Slopes      gpstk::Vector<double> slope value used in RAIM for each
      ///                     good satellite, length m.
      ///
      /// @return Return values:
      ///  0  ok      (but check TropFlag to see if trop. correction was not applied)
      /// -1  failed to converge
      /// -2  singular problem
      /// -3  not enough good data to form a solution (at least 4 satellites required)
      int SimplePRSolution(const CommonTime& Tr,
                           const std::vector<SatID>& Sats,
                           const Matrix<double>& SVP,
                           const Matrix<double>& invMC,
                           TropModel *pTropModel,
                           const int& niterLimit,
                           const double& convLimit,
                           Vector<double>& Resids,
                           Vector<double>& Slopes);

      /// Compute a RAIM solution without the measurement covariance matrix,
      /// i.e. without measurement weighting.
      int RAIMComputeUnweighted(const CommonTime& Tr,
                                std::vector<SatID>& Satellites,
                                const std::vector<double>& Pseudorange,
                                const XvtStore<SatID> *pEph,
                                TropModel *pTropModel);

      /// Compute a position/time solution, given satellite PRNs and pseudoranges
      /// using a RAIM algorithm. This is the main computation done by this class.
      /// Before this call, allowedGNSS must be defined.
      /// @param Tr          Measured time of reception of the data.
      /// @param Satellites  std::vector<SatID> of satellites; on successful
      ///                    return, satellites that were excluded by the algorithm
      ///                    are marked by a negative 'id' member.
      ///                    Also systems not in allowedGNSS are ignored.
      /// @param Pseudorange std::vector<double> of raw pseudoranges (parallel to
      ///                    Satellite), in meters.
      /// @param invMC       gpstk::Matrix<double> NXN measurement covariance matrix
      ///                    inverse (meter^-2) of the pseudorange data (for N
      ///                    see Sats). If this matrix has dimension 0, no weighting
      ///                    of the data is done.
      /// @param pEph        pointer to gpstk::XvtStore to be used in the algorithm.
      /// @param pTropModel  pointer to gpstk::TropModel for trop correction.
      ///
      /// @return Return values:
      ///  1  solution is ok, but may be degraded; check TropFlag, RMSFlag, SlopeFlag
      ///  0  ok
      /// -1  algorithm failed to converge
      /// -2  singular problem, no solution is possible
      /// -3  not enough good data (> 4) to form a (RAIM) solution
      ///     (the 4 satellite solution might be ok)
      /// -4  ephemeris not found for all the satellites
      int RAIMCompute(const CommonTime& Tr,
                      std::vector<SatID>& Satellites,
                      const std::vector<double>& Pseudorange,
                      const Matrix<double>& invMC,
                      const XvtStore<SatID> *pEph,
                      TropModel *pTropModel);

      /// Compute DOPs using the partials matrix from the last successful solution.
      /// RAIMCompute(), if successful, calls this before returning.
      /// Results stored in PRSolution::TDOP,PDOP,GDOP.
      int DOPCompute(void);

      // output -----------------------------------------------------
      /// conveniences for printing the results of the pseudorange solution algorithm
      /// return string of position, error code and V/NV
      std::string outputPOSString(std::string tag, int iret=-99,
                                    const Vector<double>& Vec=PRSNullVector);

      /// return string of {SYS clock} for all systems, error code and V/NV
      std::string outputCLKString(std::string tag, int iret=-99);

      /// return string of info in POS and CLK
      std::string outputNAVString(std::string tag, int iret=-99,
                                    const Vector<double>& Vec=PRSNullVector);

      /// return string of Nsvs, RMS residual, TDOP, PDOP, GDOP, Slope, niter, conv,
      /// satellites, error code and V/NV
      std::string outputRMSString(std::string tag, int iret=-99);
      std::string outputValidString(int iret=-99);

      /// return string of NAV and RMS strings
      std::string outputString(std::string tag, int iret=-99,
                               const Vector<double>& Vec=PRSNullVector);
      /// return string of the form "#tag label etc" which is header for data strings
      std::string outputStringHeader(std::string tag)
         { return outputString(tag,-999); }

      /// A convenience for printing the error code (return value)
      std::string errorCodeString(int iret);

      /// A convenience for printing the current configuarion
      std::string configString(std::string tag);

      // ------------------------------------------------------------
      /// Fix the apriori solution to the given constant value (XYZ,m)
      /// and initialize the 
      void fixAPSolution(const double& X, const double& Y, const double& Z)
      {
         //fixedAPriori = true;   //TD user input

         fixedAPrioriPos[0] = X;
         fixedAPrioriPos[1] = Y;
         fixedAPrioriPos[2] = Z;

         if(hasMemory) {
            APSolution = Vector<double>(3+allowedGNSS.size(),0.0);
            for(unsigned int i=0; i<3; i++)
               APSolution(i) = fixedAPrioriPos[i];
         }
      }

      /// get the aposteriori variance of unit weight; return zero if not enough
      /// data has been collected.
      inline double getAPV(void) {
         if(ndof > 0) return APV/ndof;
         return 0.0;
      }

      // dump solution, statistics and weighted average
      void dumpSolution(std::ostream& os, std::string msg="PRS")
      {
         try {
            was.setMessage(msg);
            os << was << std::endl;

            if(ndof > 0) {
               // scale covariance
               double sig(::sqrt(APV/ndof));
               Matrix<double> Cov(was.getCov());
               for(size_t i=0; i<Cov.rows(); i++) for(size_t j=i; j<Cov.cols(); j++)
                  Cov(i,j) = Cov(j,i) = Cov(i,j)*sig;
               // print cov as labelled matrix
               Namelist NL;
               NL += "ECEF_X"; NL += "ECEF_Y"; NL += "ECEF_Z";
               LabeledMatrix LM(NL,Cov);
               LM.scientific().setprecision(3).setw(14).symmetric(true);

               os << "Covariance: " << msg << std::endl << LM << std::endl;
               os << "APV: " << msg << std::fixed << std::setprecision(3)
                  << " sigma = " << sig << " meters with "
                  << ndof << " degrees of freedom.\n";
            }
            else os << " Not enough data for covariance.\n";
         }
         catch(Exception& e) { GPSTK_RETHROW(e); }
      }

      /// update apriori solution with a known solution; this is done at the end of
      /// both SimplePRSolution() and RAIMCompute()
      void updateAPSolution(const Vector<double>& Sol)
      {
         int k;
         unsigned int i;

         // first call
         if(APSolution.size() == 0) {
            APSolution = Vector<double>(3+allowedGNSS.size(),0.0);
            for(i=0; i<3; i++)
               APSolution(i) = fixedAPrioriPos[i];
         }

         // must expand Sol to have all allowed clocks
         Vector<double> S(3+allowedGNSS.size(),0.0);
         for(i=0; i<3; i++)
            S(i) = (fixedAPriori ? fixedAPrioriPos[i] : Sol(i));
         for(i=0; i<allowedGNSS.size(); i++) {
            k = vectorindex(dataGNSS,allowedGNSS[i]);
            S(3+i) = (k == -1 ? APSolution[3+i] : Sol[3+k]);
         }

         APSolution = S;

         //APSolution = Sol;
         //if(fixedAPriori)
         //   for(int i=0; i<3; i++) APSolution(i) = fixedAPrioriPos[i];
      }

      /// add newly computed solution (must be valid); update counts, APV and apriori.
      /// input parameters are from PRSolution after computing a solution.
      void addToMemory(const Vector<double>& Sol, const Matrix<double>& Cov,
               const Vector<double>& PreFitResid, const Matrix<double>& Partials,
               const Matrix<double>& invMeasCov)
      {
         was.add(Sol, Cov);

         // first solution: apriori solution has no clock, so PFR bad
         if(was.getN() == 1) return;

         try {
            // consider only the XYZ states, ignore clocks
            Matrix<double> Part(Partials,0,0,Partials.rows(),3);
            Matrix<double> invMC(invMeasCov);
            if(invMC.rows() == 0) {
               invMC=Matrix<double>(Part.rows(),Part.rows());
               ident(invMC);
            }
            Matrix<double> sumInfo(was.getInfo());
            Matrix<double> Ginv(Part*sumInfo*transpose(Part) + invMC);
            Matrix<double> G(inverseSVD(Ginv));
            Vector<double> Gpfr(G*PreFitResid);
            APV += dot(PreFitResid,Gpfr);
            ndata += PreFitResid.size();
            ndof = ndata-sumInfo.rows();
         }
         catch(Exception& e) {
            e.addText("APV failed.");
            GPSTK_RETHROW(e);
         }
      }

   private:

      /// flag: output content is valid.
      bool Valid;

      /// time tag of the current solution
      CommonTime currTime;

      /// time formats used in prints
      static const std::string calfmt,gpsfmt,timfmt;

      /// empty vector used to detect default
      static const Vector<double> PRSNullVector;

   }; // end class PRSolution

   //@}

} // namespace gpstk

#endif
