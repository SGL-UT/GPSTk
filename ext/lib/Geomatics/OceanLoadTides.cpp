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

/// @file OceanLoadTides.cpp

//------------------------------------------------------------------------------------
// system includes
#include <iostream>
#include <fstream>
#include <algorithm>
// GPSTk
#include "StringUtils.hpp"
#include "MiscMath.hpp"
#include "geometry.hpp"
// geomatics
#include "OceanLoadTides.hpp"
#include "RobustStats.hpp"       // for QSort
#include "CubicSpline.hpp"
//#include "logstream.hpp"         // TEMP

using namespace std;

namespace gpstk {

   using namespace StringUtils;

   // Number of standard (Schwiderski) tides read from BLQ file
   const int OceanLoadTides::NSTD=11;
   // Number of derived tides computed by deriveTides()
   const int OceanLoadTides::NDER=342;

   //---------------------------------------------------------------------------------
   // Open and read the given file, containing ocean loading coefficients, and
   // initialize this object for the sites names in the input list that match a
   // name in the file (case sensitive). Return the number of successfully
   // initialized site names, and remove those sites from the input list.
   // Ocean loading files can be obtained from the web. For example all the ITRF
   // sites are found at ftp://maia.usno.navy.mil/conventions/chapter7/olls25.blq
   // Also, at http://www.oso.chalmers.se/~loading one may submit site label and
   // position for one or more sites, and the resulting ocean loading file will be
   // emailed.
   // @param sites      vector<string> On input contains site labels found in the
   //                   file, on output contains only sites that were NOT found.
   //                   If sites is empty, all sites are loaded.
   // @param filename   string Input ocean loading file name.
   // @return the number of sites successfully initialized.
   // @throw if the file could not be opened.
   int OceanLoadTides::initializeSites(vector<string>& sites, string filename)
      throw(Exception)
   {
   try {
      bool allsites = false;
      if(sites.size() == 0) allsites = true; // return 0;
      int i,n;

      ifstream infile(filename.c_str());
      if(!infile || !infile.is_open()) {
         Exception e("File " + filename + " could not be opened.");
         GPSTK_THROW(e);
      }

      n = 0;         // number of successes
      bool looking=true;                        // true if looking for a site name
      double lat,lon;
      vector<double> coeff;
      string site;
      while(1) {                                // read the file
         int count;
         string line,word;

         // get the next line
         getline(infile,line);
         stripTrailing(line,'\r');

         // process line
         if(!line.empty()) {
            word = firstWord(line);
            //LOG(VERBOSE) << "Word is " << word << " and line is " << line;

            if(word == "$$") {         // NB ignore header - assume column order, etc.
               // pick out the lat/lon
               if(!looking) {
                  while(!line.empty()) {
                     word = stripFirstWord(line);
                     if(word == string("lon/lat:")) {
                        lon = asDouble(stripFirstWord(line));
                        lat = asDouble(stripFirstWord(line));
                        break;
                     }
                  }
               }
            }
            // TD should test be line length <= 21 ? ... what if site name = number
            //else if(looking && !isDecimalString(word)) {
            else if(looking && line.length() <= 21) {
               // site name
               site = line;
               stripTrailing(site,string("\n"));
               stripTrailing(site,string("\r"));
               stripTrailing(site);
               stripLeading(site);
               //LOG(VERBOSE) << "Found site " << site;
               if(allsites) {
                  //LOG(VERBOSE) << "Push back " << site;
                  looking = false;
                  sites.push_back(site);
               }
               else for(i=0; i<sites.size(); i++) {
                  //LOG(VERBOSE) << "Compare " << sites[i];
                  if(site == sites[i]) {
                     looking = false;
                     break;
                  }
               }
               if(!looking) {          // found a site
                  count = 0;
                  coeff.clear();
                  lat = lon = 0.0;
               }
            }
            else if(!looking) {        // not comment and not looking - must be data
               if(numWords(line) != 11) {
                  Exception e("File " + filename + " is corrupted for site " + site
                        + " - offending line follows\n" + line);
                  GPSTK_THROW(e);
               }
               //LOG(VERBOSE) << "Push back line " << line;
               for(i=0; i<11; i++)
                  coeff.push_back(
                     asDouble(stripFirstWord(line)));
               count++;
               if(count == 6) {        // success
                  ostringstream oss;
                  oss << fixed;
                  for(i=0; i<coeff.size(); i++) {
                     if(i < 33) oss << " " << setprecision(5) << setw(7) << coeff[i];
                     else       oss << " " << setprecision(1) << setw(7) << coeff[i];
                     if((i+1)%11 == 0) oss << "\n";
                  }
                  //LOG(VERBOSE) << "  Found site " << site << " with coefficients:";
                  //LOG(VERBOSE) << oss.str();

                  // update coeff map
                  coefficientMap[site] = coeff;
                  n++;
                  // update position map
                  coeff.clear();
                  coeff.push_back(lat);
                  coeff.push_back(lon);
                  positionMap[site] = coeff;

                  // erase a vector element
                  if(!allsites) {
                     vector<string>::iterator pos;
                     pos = find(sites.begin(),sites.end(),site);
                     if(pos != sites.end()) sites.erase(pos);
                  }
                  looking = true;
               }
            }

         }  // end if line not empty

         if(infile.eof() || !infile.good()) break;

      }  // end loop over lines in the file

      return n;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
   catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
   }

   //---------------------------------------------------------------------------------
   // Compute the site displacement vector at the given time for the given site.
   // The site must have been successfully initialized; if not an exception is
   // thrown.
   // @param site  string Input name of the site; must be the same as previously
   //              successfully passed to initializeSites().
   // @param t     EphTime Input time of interest.
   // @return Triple containing the North, East and Up components of the site
   //                displacement in meters.
   // @throw if the site has not been initialized.
   Triple OceanLoadTides::computeDisplacement11(string site, EphTime time)
      throw(Exception)
   {
   try {
      if(!isValid(site)) {
         Exception e("Site " + site + " has not been initialized.");
         GPSTK_THROW(e);
      }

      // get the coefficients for this site
      vector<double> coeff = coefficientMap[site];

      // get the astronomical arguments in radians
      double angles[11];
      //inline this SchwiderskiArg(int(t.year())-1900, t.DOY(), t.secOfDay(), angles);
      {
         double fday(time.secOfDay());
         long jday(static_cast<long>(time.lMJD() + MJD_JDAY + fday/SEC_PER_DAY));
         int iyear,imm,iday;
         convertJDtoCalendar(jday,iyear,imm,iday);
         iyear -= 1900;

         // ordering is: M2, S2, N2, K2, K1, O1, P1, Q1, Mf, Mm, Ssa
         // which are : { semi-diurnal }{   diurnal    }{long-period}
         static const double speed[11] = {
            1.40519E-4, 1.45444E-4, 1.37880E-4, 1.45842E-4,
            0.72921E-4, 0.67598E-4, 0.72523E-4, 0.64959E-4,
            0.053234E-4, 0.026392E-4, 0.003982E-4 };
         static const double angfac[44] =
         {
                                    // sun
            2.0,  0.0,  2.0,  2.0,  //  4 : M2, S2, N2, K2
            1.0,  1.0, -1.0,  1.0,  //  8 : K1, O1, P1, Q1
            0.0,  0.0,  2.0,        // 11 : Mf, Mm, Ssa
                                    // moon
           -2.0,  0.0, -3.0,  0.0,  // 15 : M2, S2, N2, K2
            0.0, -2.0,  0.0, -3.0,  // 19 : K1, O1, P1, Q1
            2.0,  1.0,  0.0,        // 22 : Mf, Mm, Ssa
                                    // lunar perigee
            0.0,  0.0,  1.0,  0.0,  // 26 : M2, S2, N2, K2
            0.0,  0.0,  0.0,  1.0,  // 30 : K1, O1, P1, Q1
            0.0, -1.0,  0.0,        // 33 : Mf, Mm, Ssa
                                    // two pi
            0.0,  0.0,  0.0,  0.0,  // 37 : M2, S2, N2, K2
            0.25,-0.25,-0.25,-0.25, // 41 : K1, O1, P1, Q1
            0.0,  0.0,  0.0         // 44 : Mf, Mm, Ssa
         };

         int icapd = iday + 365*(iyear-75)+((iyear-73)/4);

         //double capt = (27392.500528+1.000000035*double(icapd))/36525.0;
         double capt = 0.74996579132101300 + 2.73785088295687885e-5 * double(icapd);

         // mean longitude of sun at beginning of day
         double H0 = 279.69668+(36000.768930485+0.000303*capt)*capt;

         // mean longitude of moon at beginning of day
         double S0 = ((0.0000019*capt-0.001133)*capt+481267.88314137)*capt+270.434358;

         // mean longitude of lunar perigee at beginning of day
         double P0 = ((-0.000012*capt-0.010325)*capt+4069.0340329577)*capt+334.329653;

         // convert to radians
         //static const double dtr = 0.0174532925199;
         H0 *= DEG_TO_RAD;
         S0 *= DEG_TO_RAD;
         P0 *= DEG_TO_RAD;

         //LOG(INFO) << "Schwiderski " << iday << " " << fixed << setprecision(5)
         //<< setw(11) << fday << " " << icapd << " " << capt
         //<< " " << H0 << " " << S0 << " " << P0;

         static const double twopi = 6.28318530718;
         for(int k=0; k<11; k++) {
            angles[k] = speed[k]*fday + angfac[k]*H0
                                    + angfac[11+k]*S0
                                    + angfac[22+k]*P0
                                    + angfac[33+k]*twopi;
            angles[k] = ::fmod(angles[k],twopi);
            if(angles[k] < 0.0) angles[k] += twopi;
         }
      }  // end SchwiderskiArg()

      // compute the radial, west and south components
      // coefficients are stored by rows: radial, west, south; first amp, then phase
      // column order same as in SchwiderskiArg() [ as in the file ]
      Triple dc;
      for(int i=0; i<3; i++) {         // components
         dc[i] = 0.0;
         for(int j=0; j<11; j++)       // tidal modes
            dc[i] += coeff[i*11+j]*::cos(angles[j]-coeff[33+i*11+j]*DEG_TO_RAD);
      }

      // convert radial,west,south to north,east,up
      double temp=dc[0];
      dc[0] = -dc[2];         // N = -S
      dc[1] = -dc[1];         // E = -W
      dc[2] = temp;           // U = rad

      return dc;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(exception& e) {
      Exception E("std except: " + string(e.what()));
      GPSTK_THROW(E);
   }
   catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
   }

   //---------------------------------------------------------------------------------
   // Compute the site displacement vector at the given time for the given site.
   // The site must have been successfully initialized; if not an exception is
   // thrown.
   // @param site  string Input name of the site; must be the same as previously
   //              successfully passed to initializeSites().
   // @param t     EphTime Input time of interest.
   // @return Triple containing the North, East and Up components of the site
   //                displacement in meters.
   // @throw if the site has not been initialized, if the time system is unknown,
   //                if there is corruption in the static arrays, or .
   Triple OceanLoadTides::computeDisplacement(string site, EphTime time)
      throw(Exception)
   {
      try {
         ostringstream oss;      // TEMP
         int i;

         if(!isValid(site)) {
            Exception e("Site " + site + " has not been initialized.");
            GPSTK_THROW(e);
         }

         // get the coefficients for this site
         vector<double> coeff = coefficientMap[site];

         // Cartwright-Tayler numbers of Scherneck tides
         // ordering is: M2, S2, N2, K2, K1, O1, P1, Q1, Mf, Mm, Ssa

         // standard 11 Scherneck tides:
         static const NVector SchInd[] = {
            { 2, 0, 0, 0, 0, 0 },         // M2
            { 2, 2,-2, 0, 0, 0 },         // S2
            { 2,-1, 0, 1, 0, 0 },         // N2
            { 2, 2, 0, 0, 0, 0 },         // K2
            { 1, 1, 0, 0, 0, 0 },         // K1
            { 1,-1, 0, 0, 0, 0 },         // O1
            { 1, 1,-2, 0, 0, 0 },         // P1
            { 1,-2, 0, 1, 0, 0 },         // Q1
            { 0, 2, 0, 0, 0, 0 },         // Mf
            { 0, 1, 0,-1, 0, 0 },         // Mm
            { 0, 0, 2, 0, 0, 0 },         // Ssa
         };

         // NB there must be 11 std tides in SchInd[]
         if((int)(sizeof(SchInd) / sizeof(NVector)) != NSTD) {
            Exception e("Static SchInd array is corrupted");
            GPSTK_THROW(e);
         }

         // compute time argument
         EphTime ttag(time);
         ttag.convertSystemTo(TimeSystem::UTC);
         double dayfr(ttag.secOfDay()/86400.0);
         ttag.convertSystemTo(TimeSystem::TT);
         // T = EarthOrientation::CoordTransTime()
         double T((ttag.dMJD() - 51544.5)/36525.0);

         // get the Delauney arguments and frequencies at t
         double Del[5], freqDel[5];       // degrees and cycles/day
         Del[0] =    134.9634025100 +     // EarthOrientation::L()
               T*(477198.8675605000 +
               T*(     0.0088553333 +
               T*(     0.0000143431 +
               T*(    -0.0000000680))));
         Del[1] =    357.5291091806 +     // EarthOrientation::Lp()
               T*( 35999.0502911389 +
               T*(    -0.0001536667 +
               T*(     0.0000000378 +
               T*(    -0.0000000032))));
         Del[2] =     93.2720906200 +     // EarthOrientation::F()
               T*(483202.0174577222 +
               T*(    -0.0035420000 +
               T*(    -0.0000002881 +
               T*(     0.0000000012))));
         Del[3] =    297.8501954694 +     // EarthOrientation::D()
               T*(445267.1114469445 +
               T*(    -0.0017696111 +
               T*(     0.0000018314 +
               T*(    -0.0000000088))));
         Del[4] =    125.0445550100 +     // EarthOrientation::Omega2003()
               T*( -1934.1362619722 +
               T*(     0.0020756111 +
               T*(     0.0000021394 +
               T*(    -0.0000000165))));
         for(i=0; i<5; i++) Del[i] = ::fmod(Del[i],360.0);
         freqDel[0] =  0.0362916471 + 0.0000000013*T;
         freqDel[1] =  0.0027377786;
         freqDel[2] =  0.0367481951 - 0.0000000005*T;
         freqDel[3] =  0.0338631920 - 0.0000000003*T;
         freqDel[4] = -0.0001470938 + 0.0000000003*T;

         // convert to Doodson (Darwin) variables
         double Dood[6], freqDood[6];
         Dood[0] = 360.0*dayfr - Del[3];
         Dood[1] = Del[2] + Del[4];
         Dood[2] = Dood[1] - Del[3];
         Dood[3] = Dood[1] - Del[0];
         Dood[4] = -Del[4];
         Dood[5] = Dood[2] - Del[1];
         for(i=0; i<6; i++) Dood[i] = ::fmod(Dood[i],360.0);

         freqDood[0] = 1.0 - freqDel[3];
         freqDood[1] = freqDel[2] + freqDel[4];
         freqDood[2] = freqDood[1] - freqDel[3];
         freqDood[3] = freqDood[1] - freqDel[0];
         freqDood[4] = -freqDel[4];
         freqDood[5] = freqDood[2] - freqDel[1];

         // find amplitudes and phases for vertical, west and south components,
         // for all 342 derived tides, from standard tides
         double amp[NSTD],phs[NSTD];
         double ampS[NDER],ampW[NDER],ampU[NDER];  // south,west,up component amp.s
         double phsS[NDER],phsW[NDER],phsU[NDER];  // south,west,up component phs.s
         double freq[NDER];                        // frequencies (same for S,W,U)

         // vertical
         int nder;         // number returned, may be < NDER
         for(i=0; i<NSTD; i++) {
            amp[i] = coeff[i];
            phs[i] = -coeff[33+i];
         }
         //oss.str(""); oss << fixed << setprecision(5) << "TEST Amp  1 ";
         //for(i=0; i<NSTD; i++) oss << " " << setw(8) << amp[i];
         //LOG(INFO) << oss.str();
         //oss.str(""); oss << fixed << setprecision(1) << "TEST Phs  1 ";
         //for(i=0; i<NSTD; i++) oss << " " << setw(8) << phs[i];
         //LOG(INFO) << oss.str();
         //LOG(INFO) << "TEST T,DAYFR,DELTA" << fixed << setprecision(15)
         //   << setw(25) << T << setw(25) << dayfr << setw(25) << ttag.secOfDay();
         //LOG(INFO) << "TEST Delauneys " << fixed << setprecision(15)
         //   << setw(22) << Del[0] << setw(22) << Del[1] << setw(22) << Del[2]
         //   << setw(22) << Del[3] << setw(22) << Del[4];
         //LOG(INFO) << "TEST Del freqs " << fixed << setprecision(15)
         //   << setw(22) << freqDel[0] << setw(22) << freqDel[1] << setw(22)
         //   << freqDel[2] << setw(22) << freqDel[3] << setw(22) << freqDel[4];
         //LOG(INFO) << "TEST Doods     " << fixed << setprecision(15)
         //   << setw(22) << Dood[0] << setw(22) << Dood[1] << setw(22)
         //   << Dood[2] << setw(22) << Dood[3] << setw(22) << Dood[4]
         //   << setw(22) << Dood[5];
         //LOG(INFO) << "TEST Dood freqs" << fixed << setprecision(15)
         //   << setw(22) << freqDood[0] << setw(22) << freqDood[1] << setw(22)
         //   << freqDood[2] << setw(22) << freqDood[3] << setw(22) << freqDood[4]
         //   << setw(22) << freqDood[5];
         nder = deriveTides(SchInd, amp, phs, Dood, freqDood, ampU, phsU, freq, NSTD);
         //LOG(INFO) << "Vertical returned " << nder << " derived tides";

         // west
         for(i=0; i<NSTD; i++) {
            amp[i] = coeff[11+i];
            phs[i] = -coeff[44+i];
         }
         //oss.str(""); oss << fixed << setprecision(5) << "TEST Amp  2 ";
         //for(i=0; i<NSTD; i++) oss << " " << setw(8) << amp[i];
         //LOG(INFO) << oss.str();
         //oss.str(""); oss << fixed << setprecision(1) << "TEST Phs  2 ";
         //for(i=0; i<NSTD; i++) oss << " " << setw(8) << phs[i];
         //LOG(INFO) << oss.str();
         nder = deriveTides(SchInd, amp, phs, Dood, freqDood, ampW, phsW, freq, NSTD);
         //LOG(INFO) << "West returned " << nder << " derived tides";

         // south
         for(i=0; i<NSTD; i++) {
            amp[i] = coeff[22+i];
            phs[i] = -coeff[55+i];
         }
         //oss.str(""); oss << fixed << setprecision(5) << "TEST Amp  3 ";
         //for(i=0; i<NSTD; i++) oss << " " << setw(8) << amp[i];
         //LOG(INFO) << oss.str();
         //oss.str(""); oss << fixed << setprecision(1) << "TEST Phs  3 ";
         //for(i=0; i<NSTD; i++) oss << " " << setw(8) << phs[i];
         //LOG(INFO) << oss.str();
         nder = deriveTides(SchInd, amp, phs, Dood, freqDood, ampS, phsS, freq, NSTD);
         //LOG(INFO) << "TEST First 40 South amp, phase";
         //for(i=0; i<40; i++) LOG(INFO) << "TEST " << setw(2) << i+1 << fixed
         //   << setprecision(15) << setw(22) << ampS[i] << setw(22) << phsS[i];

         // sum up
         Triple dc(0.0,0.0,0.0);          // U S W
         for(i=0; i<nder; i++) {
            dc[0] += ampU[i] * ::cos(phsU[i]*DEG_TO_RAD);
            //LOG(INFO)<<"TEST LOOP U " << setw(3) << i+1 << fixed << setprecision(15)
            //  << setw(22) << ampU[i]*::cos(phsU[i]*DEG_TO_RAD) << setw(22) << dc[0];
         }
         //LOG(INFO) << "TEST RECURS result U    " << fixed << setprecision(15)
         //      << setw(22) << dc[0];

         for(i=0; i<nder; i++) {
            dc[1] += ampS[i] * ::cos(phsS[i]*DEG_TO_RAD);
            //LOG(INFO)<<"TEST LOOP S " << setw(3) << i+1 << fixed << setprecision(15)
            //  << setw(22) << ampS[i]*::cos(phsS[i]*DEG_TO_RAD) << setw(22) << dc[1];
         }
         //LOG(INFO) << "TEST RECURS result S    " << fixed << setprecision(15)
         //      << setw(22) << dc[1];

         for(i=0; i<nder; i++) {
            dc[2] += ampW[i] * ::cos(phsW[i]*DEG_TO_RAD);
            //LOG(INFO)<<"TEST LOOP W " << setw(3) << i+1 << fixed << setprecision(15)
            //  << setw(22) << ampW[i]*::cos(phsW[i]*DEG_TO_RAD) << setw(22) << dc[2];
         }
         //LOG(INFO) << "TEST RECURS result W    " << fixed << setprecision(15)
         //      << setw(22) << dc[2];

         //LOG(INFO) << "TEST      " << fixed << setprecision(6)
         //   << " " << dc[0] << "     " << dc[1] << "     " << dc[2];

         // convert vertical,south,west to north,east,up
         double temp=dc[0];
         dc[0] = -dc[1];         // N = -S
         dc[1] = -dc[2];         // E = -W
         dc[2] = temp;           // U = U

         return dc;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
      catch(exception& e) {
         Exception E("std except: " + string(e.what()));
         GPSTK_THROW(E);
      }
      catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }

   }  // end Triple OceanLoadTides::computeDisplacement

   //---------------------------------------------------------------------------------
   int OceanLoadTides::deriveTides(const NVector SchInd[],
                                   const double amp[], const double phs[],
                                   const double Dood[], const double freqDood[],
                                   double ampDer[], double phsDer[], double freqDer[],
                                   const int Nin)
      throw(Exception)
   {
      // indexes for std tides: M2, S2, N2, K2, K1,  O1,  P1,  Q1,  Mf,  Mm, Ssa
      static const int stdindex[] = {
         0,  1,  2,  3,109, 110, 111, 112, 263, 264, 265 };

      static const double DerAmp[] = {
          .632208, .294107, .121046, .079915, .023818,-.023589, .022994,
          .019333,-.017871, .017192, .016018, .004671,-.004662,-.004519,
          .004470, .004467, .002589,-.002455,-.002172, .001972, .001947,
          .001914,-.001898, .001802, .001304, .001170, .001130, .001061,
         -.001022,-.001017, .001014, .000901,-.000857, .000855, .000855,
          .000772, .000741, .000741,-.000721, .000698, .000658, .000654,
         -.000653, .000633, .000626,-.000598, .000590, .000544, .000479,
         -.000464, .000413,-.000390, .000373, .000366, .000366,-.000360,
         -.000355, .000354, .000329, .000328, .000319, .000302, .000279,
         -.000274,-.000272, .000248,-.000225, .000224,-.000223,-.000216,
          .000211, .000209, .000194, .000185,-.000174,-.000171, .000159,
          .000131, .000127, .000120, .000118, .000117, .000108, .000107,
          .000105,-.000102, .000102, .000099,-.000096, .000095,-.000089,
         -.000085,-.000084,-.000081,-.000077,-.000072,-.000067, .000066,
          .000064, .000063, .000063, .000063, .000062, .000062,-.000060,
          .000056, .000053, .000051, .000050, .368645,-.262232,-.121995,
         -.050208, .050031,-.049470, .020620, .020613, .011279,-.009530,
         -.009469,-.008012, .007414,-.007300, .007227,-.007131,-.006644,
          .005249, .004137, .004087, .003944, .003943, .003420, .003418,
          .002885, .002884, .002160,-.001936, .001934,-.001798, .001690,
          .001689, .001516, .001514,-.001511, .001383, .001372, .001371,
         -.001253,-.001075, .001020, .000901, .000865,-.000794, .000788,
          .000782,-.000747,-.000745, .000670,-.000603,-.000597, .000542,
          .000542,-.000541,-.000469,-.000440, .000438, .000422, .000410,
         -.000374,-.000365, .000345, .000335,-.000321,-.000319, .000307,
          .000291, .000290,-.000289, .000286, .000275, .000271, .000263,
         -.000245, .000225, .000225, .000221,-.000202,-.000200,-.000199,
          .000192, .000183, .000183, .000183,-.000170, .000169, .000168,
          .000162, .000149,-.000147,-.000141, .000138, .000136, .000136,
          .000127, .000127,-.000126,-.000121,-.000121, .000117,-.000116,
         -.000114,-.000114,-.000114, .000114, .000113, .000109, .000108,
          .000106,-.000106,-.000106, .000105, .000104,-.000103,-.000100,
         -.000100,-.000100, .000099,-.000098, .000093, .000093, .000090,
         -.000088, .000083,-.000083,-.000082,-.000081,-.000079,-.000077,
         -.000075,-.000075,-.000075, .000071, .000071,-.000071, .000068,
          .000068, .000065, .000065, .000064, .000064, .000064,-.000064,
         -.000060, .000056, .000056, .000053, .000053, .000053,-.000053,
          .000053, .000053, .000052, .000050,-.066607,-.035184,-.030988,
          .027929,-.027616,-.012753,-.006728,-.005837,-.005286,-.004921,
         -.002884,-.002583,-.002422, .002310, .002283,-.002037, .001883,
         -.001811,-.001687,-.001004,-.000925,-.000844, .000766, .000766,
         -.000700,-.000495,-.000492, .000491, .000483, .000437,-.000416,
         -.000384, .000374,-.000312,-.000288,-.000273, .000259, .000245,
         -.000232, .000229,-.000216, .000206,-.000204,-.000202, .000200,
          .000195,-.000190, .000187, .000180,-.000179, .000170, .000153,
         -.000137,-.000119,-.000119,-.000112,-.000110,-.000110, .000107,
         -.000095,-.000095,-.000091,-.000090,-.000081,-.000079,-.000079,
          .000077,-.000073, .000069,-.000067,-.000066, .000065, .000064,
         -.000062, .000060, .000059,-.000056, .000055,-.000051 };
   
      static const NVector DerInd[] = {
         { 2, 0, 0, 0, 0, 0 },  { 2, 2,-2, 0, 0, 0 },  { 2,-1, 0, 1, 0, 0 },//M2,S2,N2
         { 2, 2, 0, 0, 0, 0 },  { 2, 2, 0, 0, 1, 0 },  { 2, 0, 0, 0,-1, 0 },//K2,x,x
         { 2,-1, 2,-1, 0, 0 },  { 2,-2, 2, 0, 0, 0 },  { 2, 1, 0,-1, 0, 0 },  
         { 2, 2,-3, 0, 0, 1 },  { 2,-2, 0, 2, 0, 0 },  { 2,-3, 2, 1, 0, 0 },  
         { 2, 1,-2, 1, 0, 0 },  { 2,-1, 0, 1,-1, 0 },  { 2, 3, 0,-1, 0, 0 },  
         { 2, 1, 0, 1, 0, 0 },  { 2, 2, 0, 0, 2, 0 },  { 2, 2,-1, 0, 0,-1 },  
         { 2, 0,-1, 0, 0, 1 },  { 2, 1, 0, 1, 1, 0 },  { 2, 3, 0,-1, 1, 0 },  
         { 2, 0, 1, 0, 0,-1 },  { 2, 0,-2, 2, 0, 0 },  { 2,-3, 0, 3, 0, 0 },  
         { 2,-2, 3, 0, 0,-1 },  { 2, 4, 0, 0, 0, 0 },  { 2,-1, 1, 1, 0,-1 },  
         { 2,-1, 3,-1, 0,-1 },  { 2, 2, 0, 0,-1, 0 },  { 2,-1,-1, 1, 0, 1 },  
         { 2, 4, 0, 0, 1, 0 },  { 2,-3, 4,-1, 0, 0 },  { 2,-1, 2,-1,-1, 0 },  
         { 2, 3,-2, 1, 0, 0 },  { 2, 1, 2,-1, 0, 0 },  { 2,-4, 2, 2, 0, 0 },  
         { 2, 4,-2, 0, 0, 0 },  { 2, 0, 2, 0, 0, 0 },  { 2,-2, 2, 0,-1, 0 },  
         { 2, 2,-4, 0, 0, 2 },  { 2, 2,-2, 0,-1, 0 },  { 2, 1, 0,-1,-1, 0 },  
         { 2,-1, 1, 0, 0, 0 },  { 2, 2,-1, 0, 0, 1 },  { 2, 2, 1, 0, 0,-1 },  
         { 2,-2, 0, 2,-1, 0 },  { 2,-2, 4,-2, 0, 0 },  { 2, 2, 2, 0, 0, 0 },  
         { 2,-4, 4, 0, 0, 0 },  { 2,-1, 0,-1,-2, 0 },  { 2, 1, 2,-1, 1, 0 },  
         { 2,-1,-2, 3, 0, 0 },  { 2, 3,-2, 1, 1, 0 },  { 2, 4, 0,-2, 0, 0 },  
         { 2, 0, 0, 2, 0, 0 },  { 2, 0, 2,-2, 0, 0 },  { 2, 0, 2, 0, 1, 0 },  
         { 2,-3, 3, 1, 0,-1 },  { 2, 0, 0, 0,-2, 0 },  { 2, 4, 0, 0, 2, 0 },  
         { 2, 4,-2, 0, 1, 0 },  { 2, 0, 0, 0, 0, 2 },  { 2, 1, 0, 1, 2, 0 },  
         { 2, 0,-2, 0,-2, 0 },  { 2,-2, 1, 0, 0, 1 },  { 2,-2, 1, 2, 0,-1 },  
         { 2,-1, 1,-1, 0, 1 },  { 2, 5, 0,-1, 0, 0 },  { 2, 1,-3, 1, 0, 1 },  
         { 2,-2,-1, 2, 0, 1 },  { 2, 3, 0,-1, 2, 0 },  { 2, 1,-2, 1,-1, 0 },  
         { 2, 5, 0,-1, 1, 0 },  { 2,-4, 0, 4, 0, 0 },  { 2,-3, 2, 1,-1, 0 },  
         { 2,-2, 1, 1, 0, 0 },  { 2, 4, 0,-2, 1, 0 },  { 2, 0, 0, 2, 1, 0 },  
         { 2,-5, 4, 1, 0, 0 },  { 2, 0, 2, 0, 2, 0 },  { 2,-1, 2, 1, 0, 0 },  
         { 2, 5,-2,-1, 0, 0 },  { 2, 1,-1, 0, 0, 0 },  { 2, 2,-2, 0, 0, 2 },  
         { 2,-5, 2, 3, 0, 0 },  { 2,-1,-2, 1,-2, 0 },  { 2,-3, 5,-1, 0,-1 },  
         { 2,-1, 0, 0, 0, 1 },  { 2,-2, 0, 0,-2, 0 },  { 2, 0,-1, 1, 0, 0 },  
         { 2,-3, 1, 1, 0, 1 },  { 2, 3, 0,-1,-1, 0 },  { 2, 1, 0, 1,-1, 0 },  
         { 2,-1, 2, 1, 1, 0 },  { 2, 0,-3, 2, 0, 1 },  { 2, 1,-1,-1, 0, 1 },  
         { 2,-3, 0, 3,-1, 0 },  { 2, 0,-2, 2,-1, 0 },  { 2,-4, 3, 2, 0,-1 },  
         { 2,-1, 0, 1,-2, 0 },  { 2, 5, 0,-1, 2, 0 },  { 2,-4, 5, 0, 0,-1 },  
         { 2,-2, 4, 0, 0,-2 },  { 2,-1, 0, 1, 0, 2 },  { 2,-2,-2, 4, 0, 0 },  
         { 2, 3,-2,-1,-1, 0 },  { 2,-2, 5,-2, 0,-1 },  { 2, 0,-1, 0,-1, 1 },  
         { 2, 5,-2,-1, 1, 0 },  { 1, 1, 0, 0, 0, 0 },  { 1,-1, 0, 0, 0, 0 },//x,K1,O1
         { 1, 1,-2, 0, 0, 0 },  { 1,-2, 0, 1, 0, 0 },  { 1, 1, 0, 0, 1, 0 },//P1,Q1,x
         { 1,-1, 0, 0,-1, 0 },  { 1, 2, 0,-1, 0, 0 },  { 1, 0, 0, 1, 0, 0 },  
         { 1, 3, 0, 0, 0, 0 },  { 1,-2, 2,-1, 0, 0 },  { 1,-2, 0, 1,-1, 0 },  
         { 1,-3, 2, 0, 0, 0 },  { 1, 0, 0,-1, 0, 0 },  { 1, 1, 0, 0,-1, 0 },  
         { 1, 3, 0, 0, 1, 0 },  { 1, 1,-3, 0, 0, 1 },  { 1,-3, 0, 2, 0, 0 },  
         { 1, 1, 2, 0, 0, 0 },  { 1, 0, 0, 1, 1, 0 },  { 1, 2, 0,-1, 1, 0 },  
         { 1, 0, 2,-1, 0, 0 },  { 1, 2,-2, 1, 0, 0 },  { 1, 3,-2, 0, 0, 0 },  
         { 1,-1, 2, 0, 0, 0 },  { 1, 1, 1, 0, 0,-1 },  { 1, 1,-1, 0, 0, 1 },  
         { 1, 4, 0,-1, 0, 0 },  { 1,-4, 2, 1, 0, 0 },  { 1, 0,-2, 1, 0, 0 },  
         { 1,-2, 2,-1,-1, 0 },  { 1, 3, 0,-2, 0, 0 },  { 1,-1, 0, 2, 0, 0 },  
         { 1,-1, 0, 0,-2, 0 },  { 1, 3, 0, 0, 2, 0 },  { 1,-3, 2, 0,-1, 0 },  
         { 1, 4, 0,-1, 1, 0 },  { 1, 0, 0,-1,-1, 0 },  { 1, 1,-2, 0,-1, 0 },  
         { 1,-3, 0, 2,-1, 0 },  { 1, 1, 0, 0, 2, 0 },  { 1, 1,-1, 0, 0,-1 },  
         { 1,-1,-1, 0, 0, 1 },  { 1, 0, 2,-1, 1, 0 },  { 1,-1, 1, 0, 0,-1 },  
         { 1,-1,-2, 2, 0, 0 },  { 1, 2,-2, 1, 1, 0 },  { 1,-4, 0, 3, 0, 0 },  
         { 1,-1, 2, 0, 1, 0 },  { 1, 3,-2, 0, 1, 0 },  { 1, 2, 0,-1,-1, 0 },  
         { 1, 0, 0, 1,-1, 0 },  { 1,-2, 2, 1, 0, 0 },  { 1, 4,-2,-1, 0, 0 },  
         { 1,-3, 3, 0, 0,-1 },  { 1,-2, 1, 1, 0,-1 },  { 1,-2, 3,-1, 0,-1 },  
         { 1, 0,-2, 1,-1, 0 },  { 1,-2,-1, 1, 0, 1 },  { 1, 4,-2, 1, 0, 0 },  
         { 1,-4, 4,-1, 0, 0 },  { 1,-4, 2, 1,-1, 0 },  { 1, 5,-2, 0, 0, 0 },  
         { 1, 3, 0,-2, 1, 0 },  { 1,-5, 2, 2, 0, 0 },  { 1, 2, 0, 1, 0, 0 },  
         { 1, 1, 3, 0, 0,-1 },  { 1,-2, 0, 1,-2, 0 },  { 1, 4, 0,-1, 2, 0 },  
         { 1, 1,-4, 0, 0, 2 },  { 1, 5, 0,-2, 0, 0 },  { 1,-1, 0, 2, 1, 0 },  
         { 1,-2, 1, 0, 0, 0 },  { 1, 4,-2, 1, 1, 0 },  { 1,-3, 4,-2, 0, 0 },  
         { 1,-1, 3, 0, 0,-1 },  { 1, 3,-3, 0, 0, 1 },  { 1, 5,-2, 0, 1, 0 },  
         { 1, 1, 2, 0, 1, 0 },  { 1, 2, 0, 1, 1, 0 },  { 1,-5, 4, 0, 0, 0 },  
         { 1,-2, 0,-1,-2, 0 },  { 1, 5, 0,-2, 1, 0 },  { 1, 1, 2,-2, 0, 0 },  
         { 1, 1,-2, 2, 0, 0 },  { 1,-2, 2, 1, 1, 0 },  { 1, 0, 3,-1, 0,-1 },  
         { 1, 2,-3, 1, 0, 1 },  { 1,-2,-2, 3, 0, 0 },  { 1,-1, 2,-2, 0, 0 },  
         { 1,-4, 3, 1, 0,-1 },  { 1,-4, 0, 3,-1, 0 },  { 1,-1,-2, 2,-1, 0 },  
         { 1,-2, 0, 3, 0, 0 },  { 1, 4, 0,-3, 0, 0 },  { 1, 0, 1, 1, 0,-1 },  
         { 1, 2,-1,-1, 0, 1 },  { 1, 2,-2, 1,-1, 0 },  { 1, 0, 0,-1,-2, 0 },  
         { 1, 2, 0, 1, 2, 0 },  { 1, 2,-2,-1,-1, 0 },  { 1, 0, 0, 1, 2, 0 },  
         { 1, 0, 1, 0, 0, 0 },  { 1, 2,-1, 0, 0, 0 },  { 1, 0, 2,-1,-1, 0 },  
         { 1,-1,-2, 0,-2, 0 },  { 1,-3, 1, 0, 0, 1 },  { 1, 3,-2, 0,-1, 0 },  
         { 1,-1,-1, 0,-1, 1 },  { 1, 4,-2,-1, 1, 0 },  { 1, 2, 1,-1, 0,-1 },  
         { 1, 0,-1, 1, 0, 1 },  { 1,-2, 4,-1, 0, 0 },  { 1, 4,-4, 1, 0, 0 },  
         { 1,-3, 1, 2, 0,-1 },  { 1,-3, 3, 0,-1,-1 },  { 1, 1, 2, 0, 2, 0 },  
         { 1, 1,-2, 0,-2, 0 },  { 1, 3, 0, 0, 3, 0 },  { 1,-1, 2, 0,-1, 0 },  
         { 1,-2, 1,-1, 0, 1 },  { 1, 0,-3, 1, 0, 1 },  { 1,-3,-1, 2, 0, 1 },  
         { 1, 2, 0,-1, 2, 0 },  { 1, 6,-2,-1, 0, 0 },  { 1, 2, 2,-1, 0, 0 },  
         { 1,-1, 1, 0,-1,-1 },  { 1,-2, 3,-1,-1,-1 },  { 1,-1, 0, 0, 0, 2 },  
         { 1,-5, 0, 4, 0, 0 },  { 1, 1, 0, 0, 0,-2 },  { 1,-2, 1, 1,-1,-1 },  
         { 1, 1,-1, 0, 1, 1 },  { 1, 1, 2, 0, 0,-2 },  { 1,-3, 1, 1, 0, 0 },  
         { 1,-4, 4,-1,-1, 0 },  { 1, 1, 0,-2,-1, 0 },  { 1,-2,-1, 1,-1, 1 },  
         { 1,-3, 2, 2, 0, 0 },  { 1, 5,-2,-2, 0, 0 },  { 1, 3,-4, 2, 0, 0 },  
         { 1, 1,-2, 0, 0, 2 },  { 1,-1, 4,-2, 0, 0 },  { 1, 2, 2,-1, 1, 0 },  
         { 1,-5, 2, 2,-1, 0 },  { 1, 1,-3, 0,-1, 1 },  { 1, 1, 1, 0, 1,-1 },  
         { 1, 6,-2,-1, 1, 0 },  { 1,-2, 2,-1,-2, 0 },  { 1, 4,-2, 1, 2, 0 },  
         { 1,-6, 4, 1, 0, 0 },  { 1, 5,-4, 0, 0, 0 },  { 1,-3, 4, 0, 0, 0 },  
         { 1, 1, 2,-2, 1, 0 },  { 1,-2, 1, 0,-1, 0 },  { 0, 2, 0, 0, 0, 0 },//x,x,Mf
         { 0, 1, 0,-1, 0, 0 },  { 0, 0, 2, 0, 0, 0 },  { 0, 0, 0, 0, 1, 0 },//Mm,SSa
         { 0, 2, 0, 0, 1, 0 },  { 0, 3, 0,-1, 0, 0 },  { 0, 1,-2, 1, 0, 0 },  
         { 0, 2,-2, 0, 0, 0 },  { 0, 3, 0,-1, 1, 0 },  { 0, 0, 1, 0, 0,-1 },  
         { 0, 2, 0,-2, 0, 0 },  { 0, 2, 0, 0, 2, 0 },  { 0, 3,-2, 1, 0, 0 },  
         { 0, 1, 0,-1,-1, 0 },  { 0, 1, 0,-1, 1, 0 },  { 0, 4,-2, 0, 0, 0 },  
         { 0, 1, 0, 1, 0, 0 },  { 0, 0, 3, 0, 0,-1 },  { 0, 4, 0,-2, 0, 0 },  
         { 0, 3,-2, 1, 1, 0 },  { 0, 3,-2,-1, 0, 0 },  { 0, 4,-2, 0, 1, 0 },  
         { 0, 0, 2, 0, 1, 0 },  { 0, 1, 0, 1, 1, 0 },  { 0, 4, 0,-2, 1, 0 },  
         { 0, 3, 0,-1, 2, 0 },  { 0, 5,-2,-1, 0, 0 },  { 0, 1, 2,-1, 0, 0 },  
         { 0, 1,-2, 1,-1, 0 },  { 0, 1,-2, 1, 1, 0 },  { 0, 2,-2, 0,-1, 0 },  
         { 0, 2,-3, 0, 0, 1 },  { 0, 2,-2, 0, 1, 0 },  { 0, 0, 2,-2, 0, 0 },  
         { 0, 1,-3, 1, 0, 1 },  { 0, 0, 0, 0, 2, 0 },  { 0, 0, 1, 0, 0, 1 },  
         { 0, 1, 2,-1, 1, 0 },  { 0, 3, 0,-3, 0, 0 },  { 0, 2, 1, 0, 0,-1 },  
         { 0, 1,-1,-1, 0, 1 },  { 0, 1, 0, 1, 2, 0 },  { 0, 5,-2,-1, 1, 0 },  
         { 0, 2,-1, 0, 0, 1 },  { 0, 2, 2,-2, 0, 0 },  { 0, 1,-1, 0, 0, 0 },  
         { 0, 5, 0,-3, 0, 0 },  { 0, 2, 0,-2, 1, 0 },  { 0, 1, 1,-1, 0,-1 },  
         { 0, 3,-4, 1, 0, 0 },  { 0, 0, 2, 0, 2, 0 },  { 0, 2, 0,-2,-1, 0 },  
         { 0, 4,-3, 0, 0, 1 },  { 0, 3,-1,-1, 0, 1 },  { 0, 0, 2, 0, 0,-2 },  
         { 0, 3,-3, 1, 0, 1 },  { 0, 2,-4, 2, 0, 0 },  { 0, 4,-2,-2, 0, 0 },  
         { 0, 3, 1,-1, 0,-1 },  { 0, 5,-4, 1, 0, 0 },  { 0, 3,-2,-1,-1, 0 },  
         { 0, 3,-2, 1, 2, 0 },  { 0, 4,-4, 0, 0, 0 },  { 0, 6,-2,-2, 0, 0 },  
         { 0, 5, 0,-3, 1, 0 },  { 0, 4,-2, 0, 2, 0 },  { 0, 2, 2,-2, 1, 0 },  
         { 0, 0, 4, 0, 0,-2 },  { 0, 3,-1, 0, 0, 0 },  { 0, 3,-3,-1, 0, 1 },  
         { 0, 4, 0,-2, 2, 0 },  { 0, 1,-2,-1,-1, 0 },  { 0, 2,-1, 0, 0,-1 },  
         { 0, 4,-4, 2, 0, 0 },  { 0, 2, 1, 0, 1,-1 },  { 0, 3,-2,-1, 1, 0 },  
         { 0, 4,-3, 0, 1, 1 },  { 0, 2, 0, 0, 3, 0 },  { 0, 6,-4, 0, 0, 0 },
      };

      if((int)(sizeof(DerAmp) / sizeof(double)) != NDER
            || (int)(sizeof(DerInd) / sizeof(NVector)) != NDER) {
         Exception e("Static arrays are corrupted");
         GPSTK_THROW(e);
      }
   
      int i,j,k,kk;
      static const double dtr(0.01745329252);

      // get amplitude, phase and frequency for each of the standard tides
      double RealAmp[NSTD], ImagAmp[NSTD], Freq[NSTD];
      double phsrad, freq, phas;
      for(i=0; i<Nin; i++) {       // Nin is NSTD
         // first find the index for this tide
         j = stdindex[i];

         // amplitudes
         phsrad = phs[i] * dtr; //DEG_TO_RAD;          // phase in radians
         RealAmp[i] = amp[i] * ::cos(phsrad) / ::fabs(DerAmp[j]);
         ImagAmp[i] = amp[i] * ::sin(phsrad) / ::fabs(DerAmp[j]);
         //LOG(INFO) << "TEST " << setw(2) << i+1 << fixed << setprecision(15)
         //   << setw(19) << amp[i] << setw(19) << phsrad << setw(19) << DerAmp[j];

         // phase and freq
         freq = phas = 0.0;
         for(k=0; k<6; k++) {
            freq += DerInd[j].n[k] * freqDood[k];
            // not used phas += DerInd[j].n[k] * Dood[k];
         }
         Freq[i] = freq;

         // make 0 <= phas < 360  -- why?
         // not used phas = ::fmod(phas,360.0);
         // not used if(phas < 0.0) phas += 360.0;

         //LOG(INFO) << "Dood " << setw(2) << i << " at index " << setw(3) << j
         //   << " (" << DerInd[j].n[0] << "," << setw(2) << DerInd[j].n[1]
         //   << "," << setw(2) << DerInd[j].n[2] << "," << setw(2) << DerInd[j].n[3]
         //   << "," << setw(2) << DerInd[j].n[4] << "," << setw(2) << DerInd[j].n[5]
         //   << ") rA iA F " << fixed << setprecision(10) << setw(13)
         //   << RealAmp[i] << " " << setw(13) << ImagAmp[i]
         //   << " " << setw(12) << Freq[i];
      }

      // sort the frequency, and keep the key
      int key[NSTD];
      for(i=0; i<Nin; ++i) key[i] = i;
      QSort(Freq, key, Nin);

      // use key to sort amplitudes
      double tmpR[NSTD],tmpI[NSTD];
      for(i=0; i<Nin; ++i) {
         tmpR[i] = RealAmp[i];
         tmpI[i] = ImagAmp[i];
      }
      for(i=0; i<Nin; ++i) {
         RealAmp[i] = tmpR[key[i]];
         ImagAmp[i] = tmpI[key[i]];
      }

      // count the shells
      int nl(0),nm(0),nh(0);
      //LOG(INFO) << "TEST Sorted reamp, imamp, freq\n";
      for(i=0; i<Nin; i++) {       // Nin is NSTD
         //LOG(INFO) << "Sorted Dood " << setw(2) << key[i] << " rA iA F P "
         //   << fixed << setprecision(10) << setw(13) << RealAmp[i]
         //   << " " << setw(13) << ImagAmp[i] << " " << setw(12) << Freq[i];
         //LOG(INFO) << "TEST " << setw(2) << i+1 << fixed << setprecision(15)
         // << setw(19)<< RealAmp[i] << setw(19) << ImagAmp[i] << setw(19) << Freq[i];
         if(     Freq[i] < 0.5) nl++;
         else if(Freq[i] < 1.5) nm++;
         else if(Freq[i] < 2.5) nh++;
         // so freq cannot be >= 2.5??
      }
      //LOG(INFO) << "Shells contain " << nl << " " << nm << " " << nh;

      // split arrays into vector<double> for each shell
      vector<double> Flow,Rlow,Ilow,Fmed,Rmed,Imed,Fhi,Rhi,Ihi;
      for(i=0; i<nl; i++) {
         Flow.push_back(Freq[i]);
         Rlow.push_back(RealAmp[i]);
         Ilow.push_back(ImagAmp[i]);
         //LOG(INFO) << "Low shell Dood " << setw(2) << key[i] << " rA iA F "
         //   << fixed << setprecision(10) << setw(13) << RealAmp[i]
         //   << " " << setw(13) << ImagAmp[i] << " " << setw(12) << Freq[i];
      }
      for(i=nl; i<nl+nm; i++) {
         Fmed.push_back(Freq[i]);
         Rmed.push_back(RealAmp[i]);
         Imed.push_back(ImagAmp[i]);
         //LOG(INFO) << "Med shell Dood " << setw(2) << key[i] << " rA iA F "
         //   << fixed << setprecision(10) << setw(13) << RealAmp[i]
         //   << " " << setw(13) << ImagAmp[i] << " " << setw(12) << Freq[i];
      }
      for(i=nl+nm; i<nl+nm+nh; i++) {
         Fhi.push_back(Freq[i]);
         Rhi.push_back(RealAmp[i]);
         Ihi.push_back(ImagAmp[i]);
         //LOG(INFO) << "Hi shell Dood " << setw(2) << key[i] << " rA iA F "
         //   << fixed << setprecision(10) << setw(13) << RealAmp[i]
         //   << " " << setw(13) << ImagAmp[i] << " " << setw(12) << Freq[i];
      }

      // find splines of amp vs frequency in each shell
      CubicSpline<double> csRlow, csIlow, csRmed, csImed, csRhi, csIhi;
      if(nl > 0) {
         csRlow.Initialize(Flow, Rlow);
         csIlow.Initialize(Flow, Ilow);
      }
      csRmed.Initialize(Fmed, Rmed);
      csImed.Initialize(Fmed, Imed);
      csRhi.Initialize(Fhi, Rhi);
      csIhi.Initialize(Fhi, Ihi);

      // evaluate splines at each of the NDER waves; not all will contribute
      int nout(0);
      for(j=0; j<NDER; j++) {       // loop over 342 derived tides
         // this is why nout may be < NDER
         if(DerInd[j].n[0] == 0 && nl == 0) continue;

         // get phase and freq for this tide
         freqDer[nout] = phsDer[nout] = 0.0;
         for(k=0; k<6; k++) {
            freqDer[nout] += DerInd[j].n[k] * freqDood[k];
            phsDer[nout] += DerInd[j].n[k] * Dood[k];
         }
         phsDer[nout] = ::fmod(phsDer[nout],360.0);
         if(phsDer[nout] < 0.0) phsDer[nout] += 360.0;

         //LOG(INFO) << "TEST TDFRPH "
         //   << setw(3) << j+1 << fixed << setprecision(15)
         //   << setw(22) << freqDer[nout] << setw(22) << phsDer[nout]
         //   << setw(3) << DerInd[j].n[0] << setw(3) << DerInd[j].n[1]
         //   << setw(3) << DerInd[j].n[2] << setw(3) << DerInd[j].n[3]
         //   << setw(3) << DerInd[j].n[4] << setw(3) << DerInd[j].n[5];

         if(     DerInd[j].n[0] == 0) phsDer[nout] += 180.0;
         else if(DerInd[j].n[0] == 1) phsDer[nout] += 90.0;

         // get amplitudes at freq
         freq = freqDer[nout];
         double ramp,iamp;
         if(     DerInd[j].n[0] == 0) {
            if(csRlow.testLimits(freq,ramp)) ramp = csRlow.Evaluate(freq);
            if(csIlow.testLimits(freq,iamp)) iamp = csIlow.Evaluate(freq);
         }
         else if(DerInd[j].n[0] == 1) {
            if(csRmed.testLimits(freq,ramp)) ramp = csRmed.Evaluate(freq);
            if(csImed.testLimits(freq,iamp)) iamp = csImed.Evaluate(freq);
         }
         else if(DerInd[j].n[0] == 2) {
            if(csRhi.testLimits(freq,ramp)) ramp = csRhi.Evaluate(freq);
            if(csIhi.testLimits(freq,iamp)) iamp = csIhi.Evaluate(freq);
         }

         ampDer[nout] = DerAmp[j] * RSS(ramp,iamp);
         phsDer[nout] += ::atan2(iamp,ramp)/dtr; //*RAD_TO_DEG;   // TEMP
         if(phsDer[nout] > 180.0) phsDer[nout] -= 360.0;

         //LOG(INFO) << "TEST RE AM  " << setw(3) << j+1 << fixed
         //   << setprecision(15) << setw(22) << ramp << setw(22) << iamp
         //   << setw(22) << ampDer[nout] << setw(22) << phsDer[nout];

         nout++;
      }

      return nout;

   }  // end int OceanLoadTides::deriveTides()

}  // end namespace gpstk
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
