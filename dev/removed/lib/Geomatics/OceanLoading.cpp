#pragma ident "$Id: OceanLoading.cpp 208 2009-06-25 14:59:45Z BrianTolman $"

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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================
//============================================================================
//
// This software developed by Applied Research Laboratories at the University
// of Texas at Austin, under contract to an agency or agencies within the U.S. 
// Department of Defense. The U.S. Government retains all rights to use,
// duplicate, distribute, disclose, or release this software. 
//
// Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file OceanLoading.cpp
 */

// system includes
#include <iostream>
#include <fstream>
//#include <iomanip>
#include <algorithm>
// GPSTk
#include "StringUtils.hpp"
#include "geometry.hpp"
// geomatics
//#include "logstream.hpp"  // temp
#include "OceanLoading.hpp"

using namespace std;

namespace gpstk {
//------------------------------------------------------------------------------------
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
int OceanLoading::initializeSites(vector<string>& sites, string filename)
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
   //LOG(VERBOSE) << " OceanLoading::initializeSites() opened file " << filename;
   //if(allsites)
      //LOG(VERBOSE) << " OceanLoading::initializeSites() looking for all sites";
   //else for(i=0; i<sites.size(); i++)
      //LOG(VERBOSE) << " OceanLoading::initializeSites() looking for " << sites[i];

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
      StringUtils::stripTrailing(line,'\r');

      // process line
      if(!line.empty()) {
         word = StringUtils::firstWord(line);
         //LOG(VERBOSE) << "Word is " << word << " and line is " << line;

         if(word == "$$") {         // NB ignore header - assume column order, etc.
            // pick out the lat/lon
            if(!looking) {
               while(!line.empty()) {
                  word = StringUtils::stripFirstWord(line);
                  if(word == string("lon/lat:")) {
                     lon = StringUtils::asDouble(StringUtils::stripFirstWord(line));
                     lat = StringUtils::asDouble(StringUtils::stripFirstWord(line));
                     break;
                  }
               }
            }
         }
         // TD should test be line length <= 21 ? ... what if site name = number
         //else if(looking && !StringUtils::isDecimalString(word)) {
         else if(looking && line.length() <= 21) {
                                    // perhaps a site?
            // NB. site is the first 12 characters - TD isn't it 21?
            site = line; // line.substr(0,12);
            StringUtils::stripTrailing(site,string("\n"));
            StringUtils::stripTrailing(site,string("\r"));
            StringUtils::stripTrailing(site);
            StringUtils::stripLeading(site);
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
            if(StringUtils::numWords(line) != 11) {
               Exception e("File " + filename + " is corrupted for site " + site
                     + " - offending line follows\n" + line);
               GPSTK_THROW(e);
            }
            //LOG(VERBOSE) << "Push back line " << line;
            for(i=0; i<11; i++)
               coeff.push_back(
                  StringUtils::asDouble(StringUtils::stripFirstWord(line)));
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

//------------------------------------------------------------------------------------
// Compute the site displacement vector at the given time for the given site.
// The site must have been successfully initialized; if not an exception is
// thrown.
// @param site  string Input name of the site; must be the same as previously
//              successfully passed to initializeSites().
// @param t     DayTime Input time of interest.
// @return Triple containing the North, East and Up components of the site
//                displacement in meters.
// @throw if the site has not been initialized.
Triple OceanLoading::computeDisplacement(string site, DayTime t) throw(Exception)
{
try {
   if(!isValid(site)) {
      Exception e("Site " + site + " has not been initialized.");
      GPSTK_THROW(e);
   }

   // get the coefficients for this site
   vector<double> coeff = coefficientMap[site];

   // get the astronomical arguments in radians
   // NB DayTime::year() returns short
   double angles[11];
   SchwiderskiArg(int(t.year())-1900, t.DOY(), t.secOfDay(), angles);

   // compute the radial, west and south components
   // coefficients are stored by rows: radial, west, south; first amp, then phase
   // column order same as in SchwiderskiArg() [ as in the file ]
   Triple dc;
   for(int i=0; i<3; i++) {         // components
      dc[i] = 0.0;
      for(int j=0; j<11; j++)       // tidal modes
         dc[i] += coeff[i*11+j]*cos(angles[j]-coeff[33+i*11+j]*DEG_TO_RAD);
   }

   // now convert radial,west,south to north,east,up
   double temp=dc[0];
   dc[0] = -dc[2];         // N = -S
   dc[1] = -dc[1];         // E = -W
   dc[2] = temp;           // U = rad

   return dc;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

// private
// iyear must be YYYY-1900
// return angles in radians
void OceanLoading::SchwiderskiArg(int iyear, int iday, double fday, double* angles)
   throw()
{
   // ordering is: M2, S2, N2, K2, K1, O1, P1, Q1, Mf, Mm, Ssa
   // which are : { semi-diurnal }{   diurnal    }{long-period}
   static const double speed[11] = { 1.40519E-4, 1.45444E-4, 1.37880E-4, 1.45842E-4,
                                     0.72921E-4, 0.67598E-4, 0.72523E-4, 0.64959E-4,
                                     0.053234E-4, 0.026392E-4, 0.003982E-4 };
   //                                 sun
   static const double angfac[44] = { 2.0,  0.0,  2.0,  2.0, //  4 : M2, S2, N2, K2
                                      1.0,  1.0, -1.0,  1.0, //  8 : K1, O1, P1, Q1
                                      0.0,  0.0,  2.0,       // 11 : Mf, Mm, Ssa
   //                                 moon
                                     -2.0,  0.0, -3.0,  0.0, // 15 : M2, S2, N2, K2
                                      0.0, -2.0,  0.0, -3.0, // 19 : K1, O1, P1, Q1
                                      2.0,  1.0,  0.0,       // 22 : Mf, Mm, Ssa
   //                                 lunar perigee
                                      0.0,  0.0,  1.0,  0.0, // 26 : M2, S2, N2, K2
                                      0.0,  0.0,  0.0,  1.0, // 30 : K1, O1, P1, Q1
                                      0.0, -1.0,  0.0,       // 33 : Mf, Mm, Ssa
   //                                 two pi
                                      0.0,  0.0,  0.0,  0.0, // 37 : M2, S2, N2, K2
                                     0.25,-0.25,-0.25,-0.25, // 41 : K1, O1, P1, Q1
                                      0.0,  0.0,  0.0};      // 44 : Mf, Mm, Ssa
   static const double twopi = 6.28318530718;
   static const double dtr = 0.0174532925199;

   // fractional part of the day in seconds
   //int iday = int(day);
   //double fday = (day-double(iday))*86400.0;
   int icapd = iday + 365*(iyear-75)+((iyear-73)/4);
   //double capt = (27392.500528+1.000000035*double(icapd))/36525.0;
   double capt = 0.74996579132101300 + 2.73785088295687885e-5 * double(icapd);
   // mean longitude of sun at beginning of day
   double H0 = (279.69668+(36000.768930485+0.000303*capt)*capt)*dtr;
   // mean longitude of moon at beginning of day
   double S0 = (((0.0000019*capt-0.001133)*capt+481267.88314137)*capt+270.434358)*dtr;
   // mean longitude of lunar perigee at beginning of day
   double P0 = (((-0.000012*capt-0.010325)*capt+4069.0340329577)*capt+334.329653)*dtr;

   //LOG(INFO) << "Schwiderski " << iday << " " << fixed << setprecision(5)
    //<< setw(11) << fday << " " << icapd << " " << capt
    //<< " " << H0 << " " << S0 << " " << P0;

   for(int k=0; k<11; k++) {
      angles[k] = speed[k]*fday + angfac[k]*H0
                                + angfac[11+k]*S0
                                + angfac[22+k]*P0
                                + angfac[33+k]*twopi;
      angles[k] = fmod(angles[k],twopi);
      if(angles[k] < 0.0) angles[k] += twopi;
   }

}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
}  // end namespace gpstk
