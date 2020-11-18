/// @file AtmLoadTides.hpp
/// Include file for classes to handle site displacement due to atmospheric loading.

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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

#ifndef CLASS_ATMOSPHERICLOADING_INCLUDE
#define CLASS_ATMOSPHERICLOADING_INCLUDE

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

/// Atmospheric loading. Computation of displacements of sites on the solid earth
/// surface due to atmospheric loading.
/// The computation requires a site-specific set of coefficients that are available
/// via the program grdinterp.f
/// Once a file is obtained for the sites of choice, this object is initialized by
/// calling initializeSites(), passing it the file name an a list of the sites for
/// which computations will later be desired. The function isValid() returns true
/// when a given site has been initialized. The function computeDisplacement() will
/// compute the site displacement vector at any time for any initialized site.
/// 
class AtmLoadTides {
public:
   /// Constructor
   AtmLoadTides() {};

   /// Open and read the given file, containing atmospheric loading coefficients, and
   /// initialize this object for the sites names in the input list that match a
   /// name in the file (case sensitive, may contain embedded whitespace).
   /// Return the number of successfully initialized site names, and remove those
   /// sites from the input list. Convert coefficients from degrees to radians.
   /// @param sites      vector<string> On input contains site labels found in the
   ///                   file, on output contains only sites that were NOT found.
   ///                   If empty, all sites are read.
   /// @param filename   string Input atmospheric loading file name.
   /// @return the number of sites successfully initialized.
   /// @throw if the file could not be opened.
   int initializeSites(std::vector<std::string>& sites, std::string filename);

   /// Return true if the given site name has been initialized, otherwise false.
   bool isValid(std::string site)
   { return (coefficientMap.find(site) != coefficientMap.end()); }

   /// Compute the site displacement vector at the given time for the given site.
   /// The site must have been successfully initialized; if not an exception is
   /// thrown. Based on IERS routine HARDISP.F
   /// @param site  string Input name of the site; must be the same as previously
   ///              successfully passed to initializeSites().
   /// @param t     EphTime Input time of interest.
   /// @param UT1mUTC Difference of UT1 and UTC, a very small correction to t.
   /// @return Triple containing the North, East and Up components of the site
   ///                displacement in meters.
   /// @throw if the site has not been initialized, if the time system is unknown,
   ///                if there is corruption in the static arrays
   Triple computeDisplacement(std::string site, EphTime t, double UT1mUTC=0);

   /// Return the recorded latitude, longitude and ht(=0) for the given site.
   /// Return value of (0.0,0.0,0.0) probably means the position was not found.
   Triple getPosition(std::string site)
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
   /// map of (site name, coefficient array), created by call to initializeSites()
   std::map<std::string, std::vector<double> > coefficientMap;

   /// map of (site name,2-element array lat,lon), created by initializeSites()
   std::map<std::string, std::vector<double> > positionMap;

};    // end class AtmLoadTides

}  // end namespace gpstk

#endif   // end ifdef CLASS_ATMOSPHERICLOADING_INCLUDE
// nothing below this
