#pragma ident "$Id: //depot/sgl/gpstk/dev/src/AlmOrbit.hpp#2 $"

#ifndef GPSTK_ALMORBIT_HPP
#define GPSTK_ALMORBIT_HPP

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






/**
 * @file AlmOrbit.hpp
 * Encapsulate almanac data, and compute satellite orbit, etc.
 */

#include <map>
#include "DayTime.hpp"
#include "Xvt.hpp"

namespace gpstk
{
   /** @defgroup ephemcalc Ephemeris calculations */
   //@{
 
      /** Encapsulation of almanac SV orbital parameters (page 5,
       * subframes 1-24). */
   class AlmOrbit
   {
   public:
         /// Default constructor, initialize to 0.
      AlmOrbit() throw();

         /// Fill constructor for all fields.
      AlmOrbit(short prn, double aEcc, double ai_offset, double aOMEGAdot,
               double aAhalf, double aOMEGA0, double aw, double aM0,
               double aAF0, double aAF1, long aToa, long axmit_time,
               short aweek, short aSV_health);

      Xvt svXvt(const DayTime& t) const throw();

      short getPRNID() const throw()
      { return PRN; }

         /// returns full week of TRANSMIT TIME
      short getFullWeek() const throw();
      DayTime getTransmitTime() const throw();
      DayTime getToaTime() const throw();
      DayTime getTimestamp() const throw() { return getToaTime(); }
      short getSVHealth() const throw() { return SV_health; }

      void dump(std::ostream& s = std::cout, int verbosity=1) const;

   protected:
      short PRN;              /**< PRN identifier for this SV */
      double ecc;             /**< eccentricity  */
      double i_offset;        /**< i offset from 54 deg. in rads. */
      double OMEGAdot;        /**< Right Ascention rate in rad/sec */
      double Ahalf;           /**< sqrt of semi-major axis in m**1/2 */
      double OMEGA0;          /**< Right Asc. of Ascending node in rads. */
      double w;               /**< arguement of perigee in rads. */
      double M0;              /**< mean anomoly in rads */
      double AF0;             /**< clock model coeff in sec */
      double AF1;             /**< clock model coeff in sec/sec */
      long Toa;               /**< Time of epoch in GPS sec */
      long xmit_time;         /**< Transmission time */
      short week;             /**< week of Toa (full week) */
      short SV_health;        /**< health of SV */

   private:
      friend class EngAlmanac;
   }; // class AlmOrbit

      /// Map from PRN to AlmOrbit.
   typedef std::map<short, AlmOrbit> AlmOrbits;

   std::ostream& operator<<(std::ostream& s, const AlmOrbit& ao);

   //@}
  
} // namespace

#endif
