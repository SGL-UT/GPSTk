#pragma ident "$Id: RinexObsID.hpp 1161 2008-03-27 17:16:22Z ckiesch $"
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

#ifndef GPSTK_RINEXOBSID_HPP
#define GPSTK_RINEXOBSID_HPP

/**
 * @file RinexObsID.hpp
 * gpstk::RinexObsID - A specialization of ObsID that has Rinex unique
   stuff. The intent is to migrate the RinexObs classes to use this in lieu
   of the RinexObsType subclass of RinexObsHeader. That probably won't happen
   until the code to support rinex 3 is written.
 */
#include "ObsID.hpp"
#include "RinexObsHeader.hpp"

namespace gpstk
{
   class RinexObsID : public ObsID
   {
   public:
      /// empty constructor, creates an invalid object
      RinexObsID()
         : ObsID() {};

      /// Explicit constructior
      RinexObsID(ObservationType ot, CarrierBand cb, TrackingCode tc)
         : ObsID(ot, cb, tc) {};
      
      /// a conversion constructor
      RinexObsID(const RinexObsHeader::RinexObsType& rot);

   };

} // namespace gpstk
#endif
