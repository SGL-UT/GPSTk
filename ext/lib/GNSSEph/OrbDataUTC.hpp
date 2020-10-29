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

/**
 *
 *
 *
 */

#ifndef GPSTK_ORBDATAUTC_HPP
#define GPSTK_ORBDATAUTC_HPP

#include <string>
#include <iostream>
#include <math.h>

#include "CommonTime.hpp"
#include "TimeSystemCorr.hpp"    // For class TimeSystemCorrection
#include "Exception.hpp"

namespace gpstk
{
    class OrbDataUTC
    {
    public:
         OrbDataUTC():
           A0(0.0),
           A1(0.0),
           A2(0.0)
           {
           }

            /**
             * The following is only needed for GPS because only
             * the GPS interface specification contains explicit
             * guidance regarding the fit interval of the data.
             *  Therefore, this is implemented as a default that
             *  returns true, which is then overridden in the GPS-related
             * classes that implement OrbDataUTC.
             *
             * For GPS, this method determines if UTC values are valid based
             * on limitations
             * expressed in IS-GPS-200 20.3.3.5.2.4 and Karl Kovach's
             * interpretation of same following UTC Offset Error
             * anomaly of Jan 25-26, 2016
             */
          virtual bool isUtcValid(const CommonTime& ct,
                                  const bool initialXMit=false) const
          {
             return true;
          }

             /**
              * Compute GPS-UTC offset as per IS-GPS-200 20.3.3.5.2.4.
              * The basic method returns the full offset.  The version ending
              * in "ModLeapSec" returns the sub-second offset.
              * NOTE: See preceding method, isUtcValid( ) to determine
              * if provided parameters are OK to use.
              * These methods are implemented in the concrete classes
              * that descend from these classes.  Hence the "= 0" at the
              * end of the signature.  In addition, that makes OrbDataUTC
              * a "pure abstract" class for which no objects may be created.
              */
          virtual double getUtcOffset(const CommonTime& ct) const = 0;
          virtual double getUtcOffsetModLeapSec(const CommonTime& ct) const = 0;

            /**
             * Using the data in the object, form a TimeSystemCorrection
             * object and return that to the caller.
             * @throw InvalidRequest
             */
       virtual TimeSystemCorrection getTSC() const = 0;

             // UTC Parameters
          double A0;
          double A1;
          double A2;
          gpstk::CommonTime ctEpoch;
    };
}

#endif
