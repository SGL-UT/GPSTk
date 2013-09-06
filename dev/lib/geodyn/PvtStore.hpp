#pragma ident "$Id$"

/**
* @file PvtStore.hpp
* 
*/

#ifndef GPSTK_PVTSTORE_HPP
#define GPSTK_PVTSTORE_HPP


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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010, 2011
//
//============================================================================


#include "EpochDataStore.hpp"
#include "Triple.hpp"

namespace gpstk
{
      /** @addtogroup Precise Orbit Determination */
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
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreorder"
      PvtStore(std::string desc = "PvtData", ReferenceFrame frame = Unknown)
         : dataDesc(desc), dataFrame(frame) 
      {}
#pragma clang diagnostic pop
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







