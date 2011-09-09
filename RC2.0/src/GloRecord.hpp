#pragma ident "$Id$"

/**
 * @file GloRecord.hpp
 * Information encapsulated in a Glonass Nav record.
 */

#ifndef GPSTK_GloRecord_HPP
#define GPSTK_GloRecord_HPP

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


#include <iostream>
#include "Triple.hpp"
#include "Xvt.hpp"

namespace gpstk
{
  /** @addtogroup geodeticgroup */
  //@{

  /// An Earth-Centered, Earth-Fixed position/clock representation.
  /// May also be used for velocity or acceleration in the vector.
  class GloRecord : public Xvt
  {
  public:

    /// Default constructor
    GloRecord()
      : MFtime(0), health(0), freqNum(0), ageOfInfo(0.)
    {};

    /// Destructor.
    virtual ~GloRecord() {};

    Triple getAcc()
      throw()
    { return a; }

    double getTauN()
      throw()
    { return clkbias; }

    double getGammaN()
      throw()
    { return clkdrift; }

    short getMFtime()
      throw()
    { return MFtime; }

    short getHealth()
      throw()
    { return health; }

    short getfreqNum()
      throw()
    { return freqNum; }

    double getAgeOfInfo()
      throw()
    { return ageOfInfo; }

    /// Output the contents of this ephemeris to the given stream.
    void dump(std::ostream& s = std::cout) const
      throw();
    
    GloRecord& setRecord( Triple pos, Triple vel, Triple acc,
                          double clkbias, double clkdrift,
                          short mftime, short h, short freqnum,
                          double ageofinfo                          );

    Triple a;         ///< SV acceleration (x,y,z), Earth-fixed [meters]
    short MFtime;     ///< Message frame time [sec of UTC week]
    short health;     ///< SV health
    short freqNum;    ///< Frequency (channel) number (-7..+12)
    double ageOfInfo; ///< Age of oper. information [days]

    /// Output the contents of this ephemeris to the given stream.
    friend std::ostream& operator<<(std::ostream& s, 
                                    const GloRecord& glo);

  //@}

  };

}

#endif
