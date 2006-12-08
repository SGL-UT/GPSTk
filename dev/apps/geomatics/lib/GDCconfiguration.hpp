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

/**
 * @file GDCconfiguration.hpp
 * class GDCconfiguration encapsulates the configuration for input to the
 * GPSTK Discontinuity Corrector.
 */

#ifndef GPSTK_DISCONTINUITY_CORRECTOR_CONFIGURATION_INCLUDE
#define GPSTK_DISCONTINUITY_CORRECTOR_CONFIGURATION_INCLUDE

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include "Exception.hpp"

   /// class GDCconfiguration encapsulates the configuration for input to the
   /// GPSTK Discontinuity Corrector.
class GDCconfiguration
{
public:
      /// constructor; this sets a full default set of parameters.
   GDCconfiguration(void) { initialize(); }
      // destructor
   ~GDCconfiguration(void) { CFG.clear(); CFGdescription.clear(); }

      /// Set a parameter in the configuration; the input string 'cmd'
      /// is of the form '[--DC]<id><s><value>' where the separator s is
      /// one of (:=,) and leading '-','--', or '--DC' are optional.
   void setParameter(std::string cmd) throw(gpstk::Exception);

      /// Set a parameter in the configuration using the label and the value,
      /// for booleans use (T,F)=(non-zero,zero).
   void setParameter(std::string label, double value) throw(gpstk::Exception);

      /// Get the parameter in the configuration corresponding to label
   double getParameter(std::string label) throw() { return CFG[label]; }

      /// Tell GDCconfiguration to which stream to send debugging output.
   void setDebugStream(std::ostream& os) { p_oflog = &os; }

      /// Print help page, including descriptions and current values of all
      /// the parameters, to the ostream. If 'advanced' is true, also print
      /// advanced parameters.
   void DisplayParameterUsage(std::ostream& os, bool advanced=false)
      throw(gpstk::Exception);

      /// Return version string
   std::string Version() throw() { return GDCVersion; }

protected:

      /// map containing configuration labels and their values
   std::map <std::string,double> CFG;

      /// map containing configuration labels and their descriptions
   std::map <std::string,std::string> CFGdescription;

      /// Stream on which to write debug output.
   std::ostream *p_oflog;

   void initialize(void);

   static std::string GDCVersion;

}; // end class GDCconfiguration

//------------------------------------------------------------------------------------
#endif
