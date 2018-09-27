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

/// @file OceanLoadTides.hpp
/// Include file for classes to handle site displacement due to ocean loading.

#ifndef CLASS_OCEANLOADING_INCLUDE
#define CLASS_OCEANLOADING_INCLUDE

//------------------------------------------------------------------------------------
// system includes
#include <string>
#include <vector>
#include <map>
// GPSTk
#include "Exception.hpp"
#include "EphTime.hpp"
#include "Triple.hpp"

//------------------------------------------------------------------------------------
namespace gpstk {

/// Ocean loading. Computation of displacements of sites on the solid earth surface
/// due to ocean loading.
/// The computation requires a site-specific set of coefficients that are available
/// on the web in a flat file with a specific format. These coefficient files may
/// be obtained from the web. For example all the ITRF sites are found at
/// ftp://maia.usno.navy.mil/conventions/chapter7/olls25.blq
/// Also, at http://www.oso.chalmers.se/~loading one may submit site label and
/// position for one or more sites, and the resulting ocean loading file will be
/// computed and emailed.
/// Once a file is obtained for the site of choice, this object is initialized by
/// calling initializeSites(), passing it the file name an a list of the sites for
/// which computations will later be desired. The function isValid() returns true
/// when a given site has been initialized. The function computeDisplacement() will
/// compute the site displacement vector at any time for any initialized site.
/// 
class OceanLoadTides {
public:
   /// Constructor
   OceanLoadTides() {};

   /// Open and read the given file, containing ocean loading coefficients, and
   /// initialize this object for the sites names in the input list that match a
   /// name in the file (case sensitive, may contain embedded whitespace).
   /// Return the number of successfully initialized site names, and remove those
   /// sites from the input list. Convert coefficients from degrees to radians.
   /// Ocean loading files can be obtained from the web. For example all the ITRF
   /// sites are found at ftp://maia.usno.navy.mil/conventions/chapter7/olls25.blq
   /// Also, at http://www.oso.chalmers.se/~loading one may submit site label and
   /// position for one or more sites, and the resulting ocean loading file will be
   /// emailed.
   /// @param sites      vector<string> On input contains site labels found in the
   ///                   file, on output contains only sites that were NOT found.
   ///                   If empty, all sites are read.
   /// @param filename   string Input ocean loading file name.
   /// @return the number of sites successfully initialized.
   /// @throw if the file could not be opened.
   int initializeSites(std::vector<std::string>& sites, std::string filename)
      throw(Exception);

   /// Return true if the given site name has been initialized, otherwise false.
   bool isValid(std::string site) throw()
   { return (coefficientMap.find(site) != coefficientMap.end()); }

   /// Compute the site displacement vector at the given time for the given site.
   /// Use the 11-tide (simple) model.
   /// The site must have been successfully initialized; if not an exception is
   /// thrown.
   /// @param site  string Input name of the site; must be the same as previously
   ///              successfully passed to initializeSites().
   /// @param t     EphTime Input time of interest.
   /// @return Triple containing the North, East and Up components of the site
   ///                displacement in meters.
   /// @throw if the site has not been initialized.
   Triple computeDisplacement11(std::string site, EphTime t) throw(Exception);

   /// Compute the site displacement vector at the given time for the given site.
   /// The site must have been successfully initialized; if not an exception is
   /// thrown. Based on IERS routine HARDISP.F
   /// @param site  string Input name of the site; must be the same as previously
   ///              successfully passed to initializeSites().
   /// @param t     EphTime Input time of interest.
   /// @return Triple containing the North, East and Up components of the site
   ///                displacement in meters.
   /// @throw if the site has not been initialized, if the time system is unknown,
   ///                if there is corruption in the static arrays, or .
   Triple computeDisplacement(std::string site, EphTime t) throw(Exception);

   /// Return the recorded latitude, longitude and ht(=0) for the given site.
   /// Return value of (0.0,0.0,0.0) probably means the position was not found.
   Triple getPosition(std::string site) throw()
   {
      Triple t(0.0,0.0,0.0);
      std::map<std::string, std::vector<double> >::const_iterator it;
      it = positionMap.find(site);
      if(it != positionMap.end()) {
         t[0] = it->second[0];
         t[1] = it->second[1];
      }
      return t;
   }

private:
   // Compute the astronomical angular arguments for each of the 11 tidal modes.
   // Ref IERS 1996 pg 53.
   //void SchwiderskiArg(int iyear, int doy, double sod, double* angles) throw();

   /// map of (site name, coefficient array), created by call to initializeSites()
   std::map<std::string, std::vector<double> > coefficientMap;

   /// map of (site name,2-element array lat,lon), created by initializeSites()
   std::map<std::string, std::vector<double> > positionMap;

   /// Used for convenience by computeDisplacements
   typedef struct { int n[6]; } NVector;

   /// Number of standard (Schwiderski) tides read from BLQ file
   static const int NSTD;

   /// Number of derived tides computed by deriveTides()
   static const int NDER;

   /// Derive the 342 tides from the standard 11 tides using cubic spline
   /// interpolation. Called by computeDisplacements()
   /// @param SchTides  array of 11 NVectors (int[6]) with for standard tides
   /// @param amp       array of 11 amplitudes from BLQ file
   /// @param phs       array of 11 phases from BLQ file
   /// @param Dood      array of 6 Doodson arguments at time t in degrees
   /// @param freqDood  array of 6 Doodson frequencies at time in cycles/day
   /// @param ampDer    array of nout (up to 342) amplitudes of the derived tides
   /// @param phsDer    array of nout (up to 342) phases of the derived tides
   /// @param freq      array of nout (up to 342) frequencies of the derived tides
   /// @param Nin       number of std tides (11)
   /// @return nout     number of derived tides actually computed, may be < 342
   /// @throw if static arrays are corrupted.
   int deriveTides(const NVector SchTides[], const double amp[], const double phs[],
                   const double Dood[], const double freqDood[],
                   double ampDer[], double phsDer[], double freq[], const int Nin)
      throw(Exception);

};    // end class OceanLoadTides

}  // end namespace gpstk

#endif // nothing below this
