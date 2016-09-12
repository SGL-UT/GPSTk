//============================================================================
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
//  Copyright 2004, The University of Texas at Austin
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
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
* @file PvtStore.hpp
* 
*/

#ifndef GPSTK_PVTSTORE_HPP
#define GPSTK_PVTSTORE_HPP

#include "EpochDataStore.hpp"
#include "Triple.hpp"

namespace gpstk
{
      /// @ingroup Precise Orbit Determination 
      //@{

      /**
       * Class to store and manage vehicle position velocity and clock.
       *
       */
   class PvtStore : public EpochDataStore
   {
   public:
      enum ReferenceFrame
      {
         Unknown    =0,   ///< unknown reference system
         ITRF,            ///< International Terrestial Reference Frame
         ICRF            ///< International Celestial Reference Frame
      };

      typedef struct PvtData
      {
         Triple position;  ///< vehicle position (x,y,z), meters
         Triple velocity;  ///< vehicle velocity (vx vy vz), meters/sec
         double dtime;     ///< vehicle clock correction in seconds
         double ddtime;    ///< vehicle clock drift in sec/sec
         
         PvtData() : dtime(0.0), ddtime(0.0){}

         PvtData(Triple pos, Triple vel, double dt = 0.0, double ddt = 0.0) 
            : position(pos), velocity(vel), dtime(dt), ddtime(ddt)
         {}

      } Pvt;

         /// Default constructor
      PvtStore(std::string desc = "PvtData", ReferenceFrame frame = Unknown)
         : dataFrame(frame), dataDesc(desc)
      {}
         /// Default deconstructor
      virtual ~PvtStore() {}
      
         /// Add to the store directly
      void addPvt(const CommonTime& time,const Pvt& vd)
         throw();

         /** Get the Pvt data at the given epoch and return it.
          *  @param t CommonTime at which to compute the Pvt.
          *  @return Pvt data at time t.
          *  @throw InvalidRequest if the epoch on either side of t
          *     cannot be found in the map.
          */
      Pvt getPvt(const CommonTime& t) const
         throw(InvalidRequest);

         /// Get a string 
      ReferenceFrame referenceFrame()
      { return dataFrame; }

   protected:

         /// Reference frame of the data
      ReferenceFrame  dataFrame;

         /// Description of the data
      std::string dataDesc;

   }; // End of class 'PvtStore'

   
   std::ostream& operator<<(std::ostream& s, const PvtStore::Pvt& d);

      // @}

}  // End of namespace 'gpstk'


#endif   // GPSTK_PVTSTORE_HPP
