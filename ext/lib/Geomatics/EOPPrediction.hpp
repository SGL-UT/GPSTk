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

/// @file EOPPrediction.hpp
/// class EOPPrediction encapsulates formulas to predict EOPs from data in
///  NGA 'EOPP###.txt' files.

#ifndef CLASS_EOPPREDICTION_INCLUDE
#define CLASS_EOPPREDICTION_INCLUDE

//------------------------------------------------------------------------------------
// system includes
#include <iostream>
#include <iomanip>
#include <string>
// GPSTk
#include "Exception.hpp"
#include "EarthOrientation.hpp"

//------------------------------------------------------------------------------------
namespace gpstk {

   //---------------------------------------------------------------------------------
   /// Earth orientation parameter prediction
   /// Read data from EOPP###.txt file, available from NGA. The formulas are:
   ///                       2                           2
   /// xp(t)= A + B(t-ta) + SUM(Cj sin[2pi(t-ta)/Pj]) + SUM(Dj cos[2pi(t-ta)/Pj]) 
   ///                      j=1                         j=1
   ///
   ///                       2                           2
   /// yp(t)= E + F(t-ta) + SUM(Gk sin[2pi(t-ta)/Qk]) + SUM(Hk cos[2pi(t-ta)/Qk]) 
   ///                      k=1                         k=1
   ///
   ///                          4                           4
   /// UT1-UTC(t)= I+J(t-tb) + SUM(Km sin[2pi(t-tb)/Rm]) + SUM(Lm cos[2pi(t-tb)/Rm])
   ///                         m=1                         m=1
   /// Ref. NGA document 'Earth Orientation Parameter Prediction (EOPP) Description'
   /// Effective date 08 August 2004
   /// http://earth-info.nga.mil/GandG/sathtml/eoppdoc.html
   ///
   class EOPPrediction {
   private:
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
   
      /// access the time (MJD) of validity of these parameters;
      /// the range of validity is 'this' through 'this'+7.
      int getValidTime(void) const throw()
         { return int(tv+0.5); }

      /// load the EOPPrediction in the given file
      /// @param filename Name of file to read, including path.
      /// @return  0 ok, -1 error reading file.
      /// @throw FileMissingException if filename cannot be opened.
      int loadFile(std::string filename)
         throw(FileMissingException);
   
      /// Generate serial number (NGA files are named EOPP<SN>.txt) from epoch.
      /// SN (3 digit) = YWW : year (1 digit), week of year (2 digit)
      /// @param mjd Time (MJD) at which to compute the serial number
      /// @return the serial number.
      static int getSerialNumber(int mjd)
         throw(Exception);

	   /// Compute and return the Earth orientation parameters at the given MJD.
      /// TD how to warn if input is outside limits of validity?
      /// @param imjd integer MJD at which to compute the earth orientation parameters
      /// @return the EarthOrientation at imjd.
      EarthOrientation computeEOP(int& imjd) const
         throw(Exception)
      {
         double dmjd(static_cast<double>(imjd));
         return computeEOP(dmjd);
      }

	   /// Compute and return the Earth orientation parameters at the given epoch.
      /// TD how to warn if input is outside limits of validity?
      /// @param mjd Time (MJD) at which to compute the earth orientation parameters.
      /// @return the EarthOrientation object at mjd.
      EarthOrientation computeEOP(double& mjd) const
         throw();
   
      /// Stream output for the EOPPrediction, in format of EOPP###.txt files.
      /// @param os stream to append formatted EOPPrediction to.
      /// @return reference to the input stream.
      friend std::ostream& operator<<(std::ostream& s, const EOPPrediction&);

   }; // end class EOPPrediction

}  // end namespace gpstk

#endif // CLASS_EOPPREDICTION_INCLUDE
