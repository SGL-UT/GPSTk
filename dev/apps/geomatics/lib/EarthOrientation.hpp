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
 * @file EarthOrientation.hpp
 * Include file for classes to handle earth orientation data:
 * class gpstk::EarthOrientation encapsulates Earth orientation parameters (EOPs)
 * class gpstk::EOPPrediction encapsulates formulas to predict EOPs; created by
 *  reading NGA 'EOPP###.txt' files.
 * class gpstk::EOPStore encapsulates input, storage and retreval of EOPs.
 */

#ifndef CLASS_EARTHORIENT_INCLUDE
#define CLASS_EARTHORIENT_INCLUDE

//------------------------------------------------------------------------------------
// system includes
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
// GPSTk
#include "Exception.hpp"
#include "DayTime.hpp"

//------------------------------------------------------------------------------------
namespace gpstk
{

   /// Earth orientation parameters
   class EarthOrientation
   {
   public:
      double xp;           // arcseconds
      double yp;           // arcseconds
      double UT1mUTC;      // seconds

      /// Constructor
      EarthOrientation() : xp(0),yp(0),UT1mUTC(0) {}

      /// append to output stream
      friend std::ostream& operator<<(std::ostream& s, const EarthOrientation&);
   };

   //---------------------------------------------------------------------------------
   /** Earth orientation parameter prediction
    *  Read data from EOPP###.txt file, available from NGA. The formulas are:
    *                      2                           2
    * xp(t)= A + B(t-ta) + SUM(Cj sin[2pi(t-ta)/Pj]) + SUM(Dj cos[2pi(t-ta)/Pj]) 
    *                     j=1                         j=1
    *
    *                      2                           2
    * yp(t)= E + F(t-ta) + SUM(Gk sin[2pi(t-ta)/Qk]) + SUM(Hk cos[2pi(t-ta)/Qk]) 
    *                     k=1                         k=1
    *
    *                          4                           4
    * UT1-UTC(t)= I+J(t-tb) + SUM(Km sin[2pi(t-tb)/Rm]) + SUM(Lm cos[2pi(t-tb)/Rm])
    *                         m=1                         m=1
    *  Ref. NGA document 'Earth Orientation Parameter Prediction (EOPP) Description'
    *  Effective date 08 August 2004
    *  http://earth-info.nga.mil/GandG/sathtml/eoppdoc.html
    */
   class EOPPrediction
   {
      /// MJD of beginning of times at which this formula is valid; end time of
      /// validity is tv+7. In EOPP files, this is an integer.
      double tv;
      /// reference times (MJD) used in the formulas
	   double ta,tb;
      /// parameters used in the formulas
	   double A,B,C1,C2,D1,D2,E,F,G1,G2,H1,H2,I,J,K1,K2,K3,K4,L1,L2,L3,L4;
      /// more parameters used in the formulas
	   double P1,P2,Q1,Q2,R1,R2,R3,R4;

   public:
      /// the difference between TAI and UTC in seconds - not used in the computation
	   int TAIUTC;
      /// the number used in the file name 'EOPP<SN>.txt'
      int SerialNo;
      /// information, including the MJD of generation of these parameters.
      std::string Info;
   
      /** access the time (MJD) of validity of these parameters;
       *  the range of validity is 'this' through 'this'+7.
       */
      int getValidTime(void) const
         throw() { return int(tv+0.5); }

      /** load the EOPPrediction in the given file
       *  @param filename Name of file to read, including path.
       *  @return  0 ok, -1 error reading file.
       *  @throw FileMissingException if filename cannot be opened.
       */
      int loadFile(std::string filename)
         throw(FileMissingException);
   
      /** Generate serial number (NGA files are named EOPP<SN>.txt) from epoch.
       *  SN (3 digit) = YWW : year (1 digit), week of year (2 digit)
       *  @param DayTime t Time at which to compute the serial number
       *  @return the serial number.
       */
      static int getSerialNumber(DayTime& t)
         throw(DayTime::DayTimeException);
   
      static int getSerialNumber(int mjd)
         throw(DayTime::DayTimeException)
         {
            DayTime t;
            t.setMJD(double(mjd));
            return getSerialNumber(t);
         }

	   /** Compute and return the Earth orientation parameters at the given MJD.
       *  TD how to warn if input is outside limits of validity?
       *  @param int mjd Integer MJD at which to compute the earth orientation
       *                 parameters.
       *  @return the EarthOrientation.
       */
      EarthOrientation computeEOP(int& mjd) const
         throw(DayTime::DayTimeException);

	   /** Compute and return the Earth orientation parameters at the given epoch.
       *  TD how to warn if input is outside limits of validity?
       *  @param DayTime t Time at which to compute the earth orientation
       *                 parameters.
       *  @return the EarthOrientation.
       */
      EarthOrientation computeEOP(DayTime& t) const
         throw();
   
      /** Stream output for the EOPPrediction, in format of EOPP###.txt files.
       *  @param os stream to append formatted EOPPrediction to.
       *  @return reference to the input stream.
       */
      friend std::ostream& operator<<(std::ostream& s, const EOPPrediction&);
   };

   //---------------------------------------------------------------------------------
   /** Earth orientation parameter store. Store EarthOrientation objects in a map
    *  with key = integer MJD at which the EOPs are computed. Access the store
    *  with any DayTime, linearly interpolating the stored EOPs to the given epoch.
    */ 
   class EOPStore
   {
      /// key is MJD at which the Earth orientation parameters apply
      std::map<int,EarthOrientation> mapMJD_EOP;

      /// first and last times in the store, -1 if store is empty.
      int begMJD,endMJD;
   
   public:
      /// Constructor
      EOPStore() : begMJD(-1), endMJD(-1) {}

      /// Add to the store directly -- not recommended,
      /// use the form that takes EOPPrediction
      void addEOP(int MJD,
                  EarthOrientation& eop)
         throw();

      /** Add to the store by computing using an EOPPrediction,
       *  this is the usual way.
       *  @param MJD integer MJD at which to add EOPs
       *  @return non-0 if MJD is outside range
       */
      int addEOP(int MJD,
                 EOPPrediction& eopp)
         throw(DayTime::DayTimeException);

      /** Add EOPs to the store via an inpu file: either an EOPP file
       *  or a flat file produced by the IERS and available at USNO
       *  (see http://maia.usno.navy.mil/ and get either file
       *  'finals.data' or finals2000A.data').
       *  @param filename Name of file to read, including path.
       */
      void addFile(const std::string& filename)
         throw(FileMissingException);

      /** Add EOPs to the store via an EOPP file:
       *  read the EOPPrediction from the file and then compute EOPs
       *  for all days within the valid range.
       *  @param filename Name of file to read, including path.
       */
      void addEOPPFile(const std::string& filename)
         throw(FileMissingException);

      /** Add EOPs to the store via a flat IERS file.
       *  @param filename Name of file to read, including path.
       */
      void addIERSFile(const std::string& filename)
         throw(FileMissingException);

      /** Edit the store by deleting all entries before(after)
       *  the given min(max) MJDs. If mjdmin is later than mjdmax,
       *  the two are switched.
       *  @param mjdmin integer MJD desired earliest store time.
       *  @param mjdmax integer MJD desired latest store time.
       */
      void edit(int mjdmin,
                int mjdmax)
         throw();

      /** Edit the store by deleting all entries before(after)
       *  the given min(max) times. If tmin is later than tmax,
       *  the two times are switched.
       *  @param tmin DayTime desired earliest store time.
       *  @param tmax DayTime desired latest store time.
       */
      void edit(const DayTime& tmin,
                const DayTime& tmax)
         throw()
         { edit(int(tmin.MJD()+0.5),int(tmax.MJD()+1.5)); }

      /// return the number of entries in the store
      int size(void)
         throw() { return mapMJD_EOP.size(); }

      /// clear the store
      void clear(void)
         throw() { mapMJD_EOP.clear(); begMJD=endMJD=-1; }

      /** Dump the store to cout.
       * @param detail determines how much detail to include in the output
       *   0 start and stop times (MJD), and number of EOPs.
       *   1 list of all times and EOPs.
       */
      void dump(short detail=0,
                std::ostream& s=std::cout) const
         throw();

      /// Return first time (MJD) in the store.
      int getFirstMJD(void) throw()
      { return begMJD; }

      /// Return last time (MJD) in the store.
      int getLastMJD(void) throw()
      { return endMJD; }

      /** Get the EOP at the given epoch and return it.
       *  @param t DayTime at which to compute the EOPs.
       *  @return EarthOrientation EOPs at time t.
       *  @throw InvalidRequest if the (int) MJDs on either side of t
       *     cannot be found in the map.
       */
      EarthOrientation getEOP(DayTime& t) const
         throw(InvalidRequest);

   };

}  // end namespace gpstk

#endif // nothing below this
