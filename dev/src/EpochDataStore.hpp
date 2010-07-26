#pragma ident "$Id: $"

/**
* @file EpochDataStore.hpp
* Class to handle interpolatable time serial data 
*/

#ifndef GPSTK_EPOCHDATASTORE_HPP
#define GPSTK_EPOCHDATASTORE_HPP


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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010, 2011
//
//============================================================================


#include <iostream>
#include <string>
#include <map>
#include "DayTime.hpp"


namespace gpstk
{
   
      /** @addtogroup General Data Tool */
      //@{

      /** Class to handle interpolatable time serial data 
       * 
       */
   class EpochDataStore
   {
   public:

         /// Handy type definition
      typedef std::map<DayTime, std::vector<double>> EpochData;

         /// Default constructor
      EpochDataStore()
         : initialTime(DayTime::END_OF_TIME),
           finalTime(DayTime::BEGINNING_OF_TIME),
           interPoints(10)
      {}

         /// Default deconstructor
      virtual ~EpochDataStore()
      { allData.clear(); }
      
      
         /// clear the all the data
      void clear()
      { allData.clear(); }

         /** Edit the dataset, removing data outside the indicated time
          *  interval.
          *
          * @param[in] tmin defines the beginning of the time interval
          * @param[in] tmax defines the end of the time interval
          */
      virtual void edit(DayTime tmin, DayTime tmax = DayTime(DayTime::END_OF_TIME))
         throw();

         /// return the number of entries in the store
      size_t size(void)
      { return allData.size(); }

         /** Determine the earliest time stored in the object 
          *
          * @return The initial time
          *
          */
      virtual DayTime getInitialTime()
      { return initialTime; };


         /** Determine the latest time stored in the object
          *
          * @return The final time
          *
          */
      virtual DayTime getFinalTime()
      { return finalTime; };


         /// Add to the store directly
      void addData(const DayTime& time,const std::vector<double>& data)
         throw();


         /** Get the Data at the given epoch and return it.
          *  @param t DayTime at which to compute the EOPs.
          *  @return EarthOrientation EOPs at time t.
          *  @throw InvalidRequest if the epoch on either side of t
          *     cannot be found in the map.
          */
      std::vector<double> getData(DayTime& t) const
         throw(InvalidRequest);


   protected:
      
         /// Object holding all the data for the vehicle
      EpochData allData;
         
         /// These give the overall span of time for which this object
         ///  contains data.
      DayTime initialTime;
      DayTime finalTime;
      
         /// Number of points to do Lagrange Interpolation, default is 10
      int interPoints;

   }; // End of class 'EpochDataStore'

      // @}

}  // End of namespace gpstk


#endif   // GPSTK_EPOCH_DATA_STORE_HPP






