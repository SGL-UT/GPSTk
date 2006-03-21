#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/MDPtools/FormatConversionFunctions.hpp#2 $"

/** @file Translates between various similiar objects */

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

#ifndef FORMATCONVERSIONFUNCTIONS_HPP
#define FORMATCONVERSIONFUNCTIONS_HPP

#include "RinexObsData.hpp"
#include "RinexNavData.hpp"

#include "EngAlmanac.hpp"

#include "MDPNavSubframe.hpp"
#include "MDPObsEpoch.hpp"

namespace gpstk
{
   // A translation from SNR in dB-Hz to the rinex signal strength indicator
   // values were taken from a header written by teqc
   short snr2ssi(float x);

   // generates a single epoch of rinex obs data from mdp data.
   // Updates the rinex obs header as appropriate
   void makeRinexObsData(RinexObsHeader& roh, RinexObsData& rod, 
                         const MDPEpoch& mdp);

   // Try to convert the given pages into an EngAlmanc object. Returns true
   // upon success.
   bool makeEngAlmanac(EngAlmanac& alm,
                       const AlmanacPages& pages);

   // Try to convert the given pages into an EngEphemeris object. Returns true
   // upon success.
   bool makeEngEphemeris(EngEphemeris& eph,
                         const EphemerisPages& pages);
}

#endif
