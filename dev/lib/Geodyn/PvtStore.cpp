#pragma ident "$Id$"

/**
* @file PvtStore.cpp
* 
*/

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


#include "PvtStore.hpp"
#include "MiscMath.hpp"

namespace gpstk
{
   using namespace std;
  
      // Add to the store directly
   void PvtStore::addPvt(const CommonTime& time,const Pvt& d)
    throw()
   {
      std::vector<double> data(8,0.0);
      
      data[0] = d.position[0];
      data[1] = d.position[1];
      data[2] = d.position[2];
      
      data[3] = d.velocity[0];
      data[4] = d.velocity[1];
      data[5] = d.velocity[2];

      data[6] = d.dtime;
      data[7] = d.ddtime;

      addData(time, data);

   }  // End of 'VehiclePVStore::addPVData()'

   

      /* Get the Pvt at the given epoch and return it.
       *  @param t CommonTime at which to compute the Pvt.
       *  @return Pvt data at time t.
       *  @throw InvalidRequest if the epoch on either side of t
       *     cannot be found in the map.
       */
   PvtStore::Pvt PvtStore::getPvt(const CommonTime& t) const
         throw(InvalidRequest)
   {
      std::vector<double> data = getData(t);

      Pvt d;
      
      d.position[0] = data[0];
      d.position[1] = data[1]; 
      d.position[2] = data[2]; 
                    
      d.velocity[0] = data[3]; 
      d.velocity[1] = data[4]; 
      d.velocity[2] = data[5]; 
                    
      d.dtime = data[6]; 
      d.ddtime = data[7]; 
     
      return d;
   
   }  // End of method 'PvtStore::getPvt()'


   ostream& operator<<(std::ostream& os, const PvtStore::Pvt& d)
   {
      os << " " << setw(18) << setprecision(8) << d.position[0]
         << " " << setw(18) << setprecision(8) << d.position[1]
         << " " << setw(18) << setprecision(8) << d.position[2]
         << " " << setw(18) << setprecision(8) << d.velocity[0]
         << " " << setw(18) << setprecision(8) << d.velocity[1]
         << " " << setw(18) << setprecision(8) << d.velocity[2]
         << " " << setw(18) << setprecision(8) << d.dtime
         << " " << setw(18) << setprecision(8) << d.ddtime;


      return os;
   }

}  // End of namespace 'gpstk'






