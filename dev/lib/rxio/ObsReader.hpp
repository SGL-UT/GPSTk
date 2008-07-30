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

#include "Exception.hpp"
#include "ObsEpochMap.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "MDPStream.hpp"
#include "SMODFStream.hpp"
#include "SMODFData.hpp"
#include "FFIdentifier.hpp"
#include "NovatelStream.hpp"
#include "AshtechStream.hpp"

namespace gpstk
{
   class ObsReader
   {
   public:
      const std::string fn;
      FFIdentifier inputType;

      RinexObsStream ros;
      MDPStream mdps;
      SMODFStream smos;
      NovatelStream novas;
      AshtechStream ashs;

      unsigned msid;
      RinexObsHeader roh;
      int debugLevel;
      unsigned long epochCount;

      ObsReader(const std::string& str, int debug=0)
         throw(FileMissingException);
   
      ObsEpoch getObsEpoch();

      operator bool ();

      double estimateObsInterval();
      
      static std::string formatsUnderstood()
      { return "RINEX obs, MDP, smooth, Novatel, and raw Ashtech"; }

   private:
      SMODFData prevSMOD;
      bool usePrevSMOD;
   };

   ObsReader& operator>>(ObsReader& obsReader, ObsEpoch& f)
   throw();

} // end of namespace gpstk
#endif
