#pragma ident "$Id: IonoModelStore.cpp 70 2006-08-01 18:36:21Z ehagen $"



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
 * @file IonoModelStore.cpp
 * Store GPS Navigation Message based ionospheric models
 */

#include "IonoModelStore.hpp"

using namespace std;

namespace gpstk
{
   bool IonoModelStore::addIonoModel(const DayTime& mt, const IonoModel& im)
      throw()
   {
      if (!im.isValid())
         return false;
      
      IonoModelMap::const_iterator i = ims.upper_bound(mt);
      if (!ims.empty() && i != ims.begin())
      {
            // compare to previous stored model and if they have the
            // the same alpha and beta parameters don't store it
         i--;
         if (im == i->second)
            return false;
      }
      ims[mt] = im;
      return true;
   }
      
   double IonoModelStore::getCorrection(const DayTime& time,
                                        const Geodetic& rxgeo,
                                        double svel,
                                        double svaz,
                                        IonoModel::Frequency freq) const
      throw(IonoModelStore::NoIonoModelFound)
   {
      IonoModelMap::const_iterator i = ims.upper_bound(time);
      if (!ims.empty() && i != ims.begin())
      {
         i--;
         return i->second.getCorrection(time, rxgeo, svel, svaz, freq);
      }
      else
      {
         NoIonoModelFound e;
         GPSTK_THROW(e);
      }
   }
}
