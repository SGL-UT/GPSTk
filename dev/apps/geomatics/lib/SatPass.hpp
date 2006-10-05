#pragma ident "$Id: $"

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
 * @file SatPass.hpp
 * Data for one complete satellite pass overhead.
 */

#ifndef GPSTK_SATELLITE_PASS_INCLUDE
#define GPSTK_SATELLITE_PASS_INCLUDE

#include "DayTime.hpp"
#include "RinexSatID.hpp"
#include "RinexObsHeader.hpp"
#include "Exception.hpp"

#include <vector>

namespace gpstk {

   /** @addtogroup rinexutils */
   //@{

/** class SatPassData is used in SatPass to hold the RINEX data.
  */
class SatPassData {
public:
   	/// Constructor
   SatPassData(void);
   	/// Destructor
   ~SatPassData(void);

		/// a flag (cf. SatPass::BAD, etc.) that is set to OK or BAD at creation (by the
      /// caller of SatPass::push_back), then reset by other processing.
   unsigned short flag;
      /// time 'count' : time of data[i] = FirstTime + ndt[i] * dt
   unsigned int ndt;
      /// data arrays for dual frequency pseudorange (m) and carrier phase (cycles)
   double P1,P2,L1,L2;
      /// loss-of-lock and signal-strength indicators (from RINEX) for the P1,P2,L1,L2
      /// data, 'merged' together into a single integer. I.e. if the LLI and SSI for
      /// the data are 2,5,3,6,7,9,3 & 9, then indicators = 25367939.
   unsigned long indicators;

}; // end class SatPassData

/** class SatPass holds all range and phase data for a full satellite pass.
  * Constructed and filled by the calling program, it is used to pass data into
  * and out of the GPSTK discontinuity corrector.
  */
class SatPass {
public:
      /// flag indicating bad data
   static const unsigned short BAD;
      /// flag indicating good data with no phase discontinuity
      /// NB test for 'good' data using (flag != SatPass::BAD), NOT flag==SatPass::OK
   static const unsigned short OK;
      /// flag indicating good data with phase discontinuity on L1 only.
   static const unsigned short LL1;
      /// flag indicating good data with phase discontinuity on L2 only.
   static const unsigned short LL2;
      /// flag indicating good data with phase discontinuity on both L1 and L2.
   static const unsigned short LL3;
      /// size of maximum time gap, in seconds, allowed within SatPass data.
	static double maxGap;
		/// format string, as defined in class DayTime, for output of times
	static std::string outFormat;

   typedef std::vector<SatPassData>::iterator iterator;
   typedef std::vector<SatPassData>::const_iterator const_iterator;

      // member functions ----------------------------------------------
   	/// Default constructor
   SatPass(RinexSatID sat, double dt);

   	/// Destructor
   ~SatPass(void);

      /// Copy constructor
   SatPass(const SatPass& sp) { *this = sp; }

      /// operator =
   SatPass& operator=(const SatPass& sp);

		/// add data to the arrays at timetag tt
      /// Calls must be made in time order
      /// Caller sets the flag to either BAD or OK; other processing (DC) may reset.
   bool push_back(const DayTime tt, SatPassData& spd);

		/// smooth pseudorange and debias phase; call this ONLY after cycleslips
		/// have been removed.
   void smooth(bool smoothPR, bool smoothPH, std::ostream& os);

		/// change the maximum time gap (in seconds) allowed within any SatPass
      /// return the input.
	static double SatPass::setMaxGap(const double gap)
      { maxGap = gap; return maxGap; }

      /// set timetag output format
   void setOutputFormat(std::string fmt) { outFormat = fmt; };

      /// get the max. gap limit size (seconds); for all SatPass objects
	double getMaxGap(void) const throw() { return maxGap; }

      /// get the timetag of the first data in this SatPass
   DayTime getFirstTime(void) const throw() { return firstTime; }

      /// get the timetag of the last data in this SatPass
   DayTime getLastTime(void) const throw() { return lastTime; }

      /// get the satellite of this SatPass
   SatID getSat(void) const throw() { return sat; }

      /// get the time interval of this SatPass
   double getDT(void) const throw() { return dt; }

      /// get the number of good points in this SatPass
   int getNgood(void) const throw() { return ngood; }

      /// get the size of (the arrays in) this SatPass
   unsigned int size(void) const throw() { return data.size(); }

      /// resize the arrays in this SatPass
   void resize(unsigned int n) throw() { data.resize(n); }

      /// get one element of the count array of this SatPass
   unsigned int getCount(unsigned int i) const throw(Exception);

      /// get one element of the data array of this SatPass
   SatPassData getData(unsigned int i) const throw(Exception);

      /// compute the timetag associated with index i in the data array
   DayTime time(unsigned int i) const throw(Exception);

		/// return true if the given timetag is or could be part of this pass
	bool includesTime(const DayTime& tt) const throw();

      /// dump all the data in the pass, one line per timetag;
      /// put message msg1 at beginning of each line,
      /// msg2 at the end of the first (#comment) line.
   void dump(std::ostream& os, std::string msg1, std::string msg2=std::string())
      const throw();

      /// STL iterator begin
   iterator begin() { return data.begin(); }
      /// STL const iterator begin
   const_iterator begin() const
      { return data.begin(); }
      /// STL iterator end
   iterator end() { return data.end(); }
      /// STL const iterator end
   const_iterator end() const
      { return data.end(); }

      // other ---------------------------------------------------------
	friend std::ostream& operator<<(std::ostream& os, const gpstk::SatPass& sp);

      // member data ---------------------------------------------------
      /// status flag for use by DC, caller, etc.; set to 0 by constructor,
      /// but otherwise ignored by class SatPass.
   unsigned int status;

protected:
		/// nominal time spacing of the data.
   double dt;

		/// timetags of the first and last data points.
   DayTime firstTime,lastTime;

   	/// Satellite identifier for this data.
   RinexSatID sat;

		/// number of timetags with good data in the data arrays.
	unsigned int ngood;

		/// All data in the pass (good and bad points), stored in SatPassData,
      /// are stored in this vector.
   std::vector<SatPassData> data;

}; // end class SatPass

   ///Stream output for SatPass.
   ///@param os output stream to write to
   ///@param sp SatPass to write
   ///@return reference to os.
std::ostream& operator<<(std::ostream& os, const gpstk::SatPass& sp);

   //@}

}  // end namespace gpstk

#endif
