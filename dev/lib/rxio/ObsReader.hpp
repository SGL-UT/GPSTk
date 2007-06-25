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

/** @file Read various file formats and output a stream of ObsEpoch objects.
*/

#ifndef OBSREADER_HPP
#define OBSREADER_HPP

#include "ObsEpochMap.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "MDPStream.hpp"
#include "SMODFStream.hpp"
#include "SMODFData.hpp"
#include "FFIdentifier.hpp"

namespace gpstk
{
   class ObsReader
   {
   public:
      const std::string fn;
      gpstk::FFIdentifier inputType;

      gpstk::RinexObsStream ros;
      gpstk::MDPStream mdps;
      gpstk::SMODFStream smos;

      unsigned msid;
      gpstk::RinexObsHeader roh;
      int verboseLevel;
      unsigned long epochCount;

      ObsReader(const std::string& str, int verbose=0);
   
      ObsEpoch getObsEpoch();

      bool operator()();

   private:
      gpstk::SMODFData prevSMOD;
      bool usePrevSMOD;
   };
} // end of namespace gpstk
#endif
