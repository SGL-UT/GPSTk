#pragma ident "$Id$"

/**
 * @file IERS.cpp
 * This class ease handling IERS earth orientation data.
 */

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
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
//  Wei Yan - Chinese Academy of Sciences . 2010
//
//============================================================================

#include "IERS.hpp"
#include <string>
#include <fstream>
#include <cmath>

namespace gpstk
{
      // PI
   const double IERS::PI = 4.0 * std::atan(1.0);

      // Arcseconds to radius convention 
   const double IERS::ARCSEC2RAD =  PI / 180.0 / 3600.0;
      

      // UT1-UTC time difference [s]
   double IERS::UT1mUTC(double mjdUTC)
      throw (InvalidRequest) { return gpstk::UT1mUTC( CommonTime(mjdUTC) ); }


      // Pole coordinate x [arcseconds]
   double IERS::xPole(double mjdUTC)
      throw (InvalidRequest) { return gpstk::PolarMotionX( CommonTime(mjdUTC) ); }


      // Pole coordinate y [arcseconds]
   double IERS::yPole(double mjdUTC)
      throw (InvalidRequest) { return gpstk::PolarMotionY( CommonTime(mjdUTC) );}


      // Nutation dPsi [arcseconds]
      // @param  Modified Julidate in UTC
      // @return dPsi in arcseconds
   double IERS::dPsi(double mjdUTC)
      throw (InvalidRequest) {return gpstk::NutationDPsi( CommonTime(mjdUTC) );}


      // Nutation dEps [arcseconds]
      // @param  Modified Julidate in UTC
      // @return dEps in arcseconds
   double IERS::dEps(double mjdUTC)
      throw (InvalidRequest) {return gpstk::NutationDEps( CommonTime(mjdUTC) );}


      // 'finals.data' from http://maia.usno.navy.mil
   void IERS::loadIERSFile(const std::string& iersFile)
      throw(FileMissingException) { gpstk::LoadIERSFile(iersFile); } 
   

      // ERP data file from IGS 
   void IERS::loadIGSFile(const std::string& igsFile)
      throw(FileMissingException) { gpstk::LoadIGSFile(igsFile); } 


      // ERP data file from STK
   void IERS::loadSTKFile(const std::string& stkFile)
      throw(FileMissingException) { gpstk::loadSTKFile(stkFile); } 


   CommonTime IERS::GPST2UTC(CommonTime GPST)
   { return gpstk::GPST2UTC(GPST); }


   int IERS::TAImUTC(const double& mjdUTC)
   { return gpstk::TAImUTC( CommonTime(mjdUTC) ); } 


}  // End of namespace 'gpstk'
