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
 * @file DiscCorr.hpp
 * GPS phase discontinuity correction. Given a SatPass object
 * containing dual-frequency pseudorange and phase for an entire satellite pass,
 * and a configuration object (as defined herein), detect discontinuities in
 * the phase and, if possible, estimate their size.
 * Output is in the form of Rinex editing commands (see class RinexEditor).
 */

#ifndef GPSTK_DISCONTINUITY_CORRECTOR_INCLUDE
#define GPSTK_DISCONTINUITY_CORRECTOR_INCLUDE

#include "DayTime.hpp"
#include "GSatID.hpp"
#include "RinexObsHeader.hpp"
#include "SatPass.hpp"
#include "GDCconfiguration.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace gpstk {

   /** @addtogroup rinexutils */
   //@{

   //* const int PrematureEnd=-2;
   /** GPSTK Discontinuity Corrector. Find, and fix if possible, discontinuities
   * in the GPS carrier phase data, given dual-frequency pseudorange and phase
   * data for an entire satellite pass. Input is the SatPass object holding the
   * data, and a GDCconfiguration object giving the parameter values for
   * the corrector. Output is in the form of a list of strings - editing commands
   * - that can be parsed and applied using the GPSTK Rinex Editor (see Prgm
   * EditRinex and the RinexEditor class). Also, the L1 and L2 arrays in the input
   * SatPass are corrected. The routine will mark bad points in the input data
   * using the SatPass flag.
   * @param SP       SatPass object containing the input data.
   * @param config   GDCconfiguration object.
   * @param EditCmds vector<string> (output) containing RinexEditor commands.
   * @return 0 for success, otherwise return an Error code;
   * codes are defined as follows.
   * const int NoData = -4        insufficient input data, or all data is bad
   * const int FatalProblem = -3  DT is not set, or memory problem
   * const int Singularity = -1   polynomial fit fails
   * const int ReturnOK = 0       normal return
   */
   int DiscontinuityCorrector(SatPass&, GDCconfiguration&, std::vector<std::string>&)
      throw(Exception);

   //@}

}  // end namespace gpstk

//------------------------------------------------------------------------------------
#endif
