/// @file AtmLoadTides.cpp
/// Classes to handle site displacement due to atmospheric loading.

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

//------------------------------------------------------------------------------------
// system includes
#include <iostream>
#include <fstream>
#include <algorithm>
// GPSTk
#include "StringUtils.hpp"
#include "MiscMath.hpp"
#include "GNSSconstants.hpp"
// geomatics
#include "AtmLoadTides.hpp"
//#include "logstream.hpp"      // TEMP

using namespace std;

namespace gpstk {

   using namespace StringUtils;

   //---------------------------------------------------------------------------------
   // Open and read the given file, containing atmospheric loading coefficients, and
   // initialize this object for the sites names in the input list that match a
   // name in the file (case sensitive). Return the number of successfully
   // initialized site names, and remove those sites from the input list.
   // Atmospheric loading files can be obtained by running the program grdinterp.f
   // param sites      vector<string> On input contains site labels found in the
   //                   file, on output contains only sites that were NOT found.
   //                   If sites is empty, all sites are loaded.
   // param filename   string Input atmospheric loading file name.
   // return the number of sites successfully initialized.
   // throw if the file could not be opened.
   int AtmLoadTides::initializeSites(vector<string>& sites, string filename)
   {
   try {
      bool allsites = false;
      if(sites.size() == 0) allsites = true;
      int i,nwant,nfound;

      ifstream infile(filename.c_str());
      if(!infile || !infile.is_open()) {
         Exception e("File " + filename + " could not be opened.");
         GPSTK_THROW(e);
      }

      nwant = sites.size();
      nfound = 0;                               // number of successes
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
         stripLeading(line);

         // process line
         if(!line.empty()) {
            word = firstWord(line);
            //LOG(INFO) << "Word is " << word << " and line is " << line;

            if(word == "$$") {         // NB ignore header - assume column order, etc.
               // pick out the lat/lon
               while(!line.empty()) {
                  word = stripFirstWord(line);
                  if(word == string("station")) {
                     site = stripFirstWord(line);
                     stripTrailing(site,';');
                  }
                  if(word == string("coord.(long,lat)")) {
                     lon = asDouble(stripFirstWord(line));
                     lat = asDouble(stripFirstWord(line));
                     //LOG(INFO) << "Found coords for site " << site << " "
                     //      << fixed << setprecision(6) << lon << " " << lat;
                     break;
                  }
               }
            }
            else if(looking && line.length() <= 40) {          // site
               // site name
               site = line;
               //LOG(INFO) << "Found site " << site;
               if(allsites) {
                  //LOG(INFO) << "Push back " << site;
                  looking = false;
                  sites.push_back(site);
               }
               else for(i=0; i<sites.size(); i++) {
                  //LOG(INFO) << "Compare " << sites[i];
                  if(site == sites[i]) {
                     looking = false;
                     break;
                  }
               }
               if(!looking) {          // found a site
                  count = 0;
                  coeff.clear();
               }
            }
            else if(!looking) {        // not comment and not looking - must be data
               if(numWords(line) != 4) {
                  Exception e("File " + filename + " is corrupted for site " + site
                        + " - offending line follows\n" + line);
                  GPSTK_THROW(e);
               }
               //LOG(INFO) << "Push back line " << line;
               for(i=0; i<4; i++)
                  coeff.push_back(asDouble(stripFirstWord(line)));
               count++;
               if(count == 3) {        // success
                  ostringstream oss;
                  oss << fixed;
                  for(i=0; i<coeff.size(); i++) {
                     oss << " " << setprecision(4) << setw(7) << coeff[i];
                     if((i+1)%4 == 0) oss << "\n";
                  }
                  //LOG(INFO) << "  Found site " << site << " with coefficients:";
                  //LOG(INFO) << oss.str();

                  // update coeff map
                  coefficientMap[site] = coeff;
                  nfound++;
                  // update position map
                  coeff.clear();
                  coeff.push_back(lat);
                  coeff.push_back(lon);
                  positionMap[site] = coeff;
                  //LOG(INFO) << "pushback coords for site " << site << " "
                  //         << fixed << setprecision(6) << lon << " " << lat;

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

         if(!allsites && nfound >= nwant) break;

         if(infile.eof() || !infile.good()) break;

      }  // end loop over lines in the file

      return nfound;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(exception& e) {
      Exception E("std except: "+string(e.what()));
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
   Triple AtmLoadTides::computeDisplacement(string site, EphTime time)
   {
      try {

         // get the coefficients for this site
         if(coefficientMap.find(site) == coefficientMap.end())
            GPSTK_THROW(Exception("Site not found in atmospheric loading store"));
         vector<double> coeff = coefficientMap[site];

         // compute time argument
         EphTime ttag(time);
         ttag.convertSystemTo(TimeSystem::UTC);    // ignore UT1-UTC
         double dayfr(ttag.secOfDay()/86400.0);    // fraction of day
         static const double w1(2*PI), w2(4*PI);
         const double cos1(::cos(w1*dayfr));
         const double sin1(::sin(w1*dayfr));
         const double cos2(::cos(w2*dayfr));
         const double sin2(::sin(w2*dayfr));

         // NB Displacement is defined positive up, north
         // and east directions, and is in millimeters

         // total d(t) = d(1)*cos(t*w1) + d(2)*sin(t*w1)
         //            + d(3)*cos(t*w2) + d(4)*sin(t*w2)
         // If t is fractions of a UT1 day,
         // then w1=2pi radians/day and w2=4pi radians/day.

         // Column order coss1 sins1 coss2 sins2
         // Row (coeff) order: RADIAL NS EW

         Triple dc;     // dc is NEU, so must RAD,N,E -> NEU
         dc[2] = coeff[0]*cos1 + coeff[1]*sin1 + coeff[2]*cos2 + coeff[3]*sin2;
         dc[0] = coeff[4]*cos1 + coeff[5]*sin1 + coeff[6]*cos2 + coeff[7]*sin2;
         dc[1] = coeff[8]*cos1 + coeff[9]*sin1 + coeff[10]*cos2 + coeff[11]*sin2;
         // convert to meters
         dc[0] /= 1000.0;
         dc[1] /= 1000.0;
         dc[2] /= 1000.0;

         return dc;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
      catch(exception& e) {
         Exception E("std except: " + string(e.what()));
         GPSTK_THROW(E);
      }
      catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }

   }  // end Triple AtmLoadTides::computeDisplacement

}  // end namespace gpstk
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
/*
https://geophy.uni.lu/displacementgrids/

Method 2: Using the Displacement grids
Based on the Proposed IERS Conventions... (PRELIMINARY UNTIL CONVENTIONS ARE APPROVED)

   T. M. van Dam, University of Luxembourg
   R. Ray, Space Geodesy Branch, NASA Goddard Space Flight Center
Anyone using this data set directly or products derived from this page is requested
   to use the following citation:
van Dam, T. and R. Ray, 2010, Updated October 2010.
   "S1 and S2 Atmospheric Tide Loading Effects for Geodetic Applications."

The displacement grids are determined at every 1.0 degree of latitude and longitude
They have been calculated by convolving 1.125 deg x 1.125 deg S1 and S2 annual mean
atmospheric tides [Ray and Ponte, 2003]
with Farrell's Green's Functions. We assume no ocean response to pressure

To use the grids and interpolate yourself you must:
   Download at least one of the following files
   CE (s1_s2_def_ce.dat)
   CM (s1_s2_def_cm.dat)

If you do not already have a program for interpolating evenly spaced grid points,
   you may download, grdintrp.f
CE, and CM designate the reference frame in which the displacements are determined.
   See Blewitt [2003] for further information
The ascii grids contain the up (dr), north (vt) and east (vl) components of
   the sine and cosine amplitudes for the S1 and S2 tides
The deformations are in mm
Each displacement component has 4 parameters: cosS1,sinS1,cosS2,sinS2 written
   in that order in the file

To read the grids:
   do i=1,nlon (nlon=361)
   do j=1,nlat (nlat=181)
   read(iun,*) rlon,rlat,(dr(k),k=1,4),(vt(k),k=1,4),(vl(k),k=1,4)
   end do
   end do

total dr(t) = dr(1)*cos(t*ω1) + dr(2)*sin(t*ω1) + dr(3)*cos(t*ω2) + dr(4)*sin(t*ω2)
   If t is in fractions of a UT1 day, then ω1=2π radians/day and ω2=4π radians/day

If you will use your own routine to interpolate the grids, you do not need to
   muddle through the remainder of this document;

If you need information on running the supplied interpolation routine, grdintrp.f,
   please continue

To run grdintrp.f:
   You will need to specify the input file to read from in grdintrp.f (variable=iref)
   You will need to specify the output format in grdintrp.f (variable=iout)
   The program reads 3 variables: STA, longitude, latitude from a file that you
   create called in.grdintrp
      e.g.
      bjfs 115.892487 39.6086006
      blyt 245.285156 33.6104164
      bogt 285.919067 4.64007235
      bor1 17.0734558 52.2769585
      bran 241.722961 34.1848946
      bras 11.1130829 44.1221657
      braz 312.122131 -15.9474754

Note: currently grdintrp.f expects STA to be 4 characters in length,
   you (of course) must change this specification if you use longer character names
Customize the program to address your own particular needs
Compile the program using your favorite fortran compiler;
   on UNIX or LINUX: f77 grdinterp.f -o grdinterp
Run program (Type grdintrp at the system prompt)
The output file is named grdintrp.dat

Data in these files can be used to generate a time series of the surface
   displacement at the site
   total dr(t) = dr(1)*cos(t*ω1) + dr(2)*sin(t*ω1) + dr(3)*cos(t*ω2) + dr(4)*sin(t*ω2)
   If t is in fractions of a UT1 day, then ω1=2π radians/day and ω2=4π radians/day.

Center of Mass Corrections:
   As with ocean loading, it may be necessary to compute the crust-frame translation
   (geocenter motion) due to the atmospheric tidal mass, dX(t),dY(t), and dZ(t).
   These values may be computed according to the method given by Scherneck
   at http://www.oso.chalmers.se/âˆ¼loading/cmc.html. For example,
      dX(t)=A1*cos(t*ω1)+ B1*sin(t*ω1) A2*cos(t*ω2) +B2*sin(t*ω2)
      If t is in fractions of a UT1 day, then ω1=πradians/day and ω2=2πradians/day

Download the COM corrections here
   com.dat (com_table.pdf)

As with ocean tidal loading, this correction should be applied in transforming
   GPS orbits from the CM frame to the CF frame expected in the sp3 orbit.

References
Blewitt, G. (2003), Self-consistency in reference frames, geocenter definition,
   and srface loading of the solid Earth, J. Geophys. Res., ,108, 2103,
   doi:10.1029/2002JB002082.

Farrell, W. E. (1972), Deformation of the Earth by surface loads,
   Rev. Geophys., 10, 761-797.

Ray, R.D. and R.M. Ponte (2003), Barometeric tides from ECMWF operational analyses,
   Annales Geophysicae, 21, 1897-1910.

Ray, R.D. and G. Egbert (2004), The global S_1 tide, J. Phys. Oceanogr.,
   34, 1922-1935.
*/
