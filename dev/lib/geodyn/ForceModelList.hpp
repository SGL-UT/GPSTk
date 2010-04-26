#pragma ident "$Id: $"

/**
* @file ForceModelList.hpp
* ForceModelList is a countainer for force models.
*/

#ifndef GPSTK_FORCE_MODEL_LIST_HPP
#define GPSTK_FORCE_MODEL_LIST_HPP

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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
//
//============================================================================


#include "EarthBody.hpp"
#include "ForceModel.hpp"

#include <list>
#include <set>

namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{
      
      /**
       * ForceModelList is a countainer for force models. It allows the simulation to encapsulate
       * a number of different force models and obtain their combined effect on a spacecraft. 
       */
   class ForceModelList : public ForceModel
   {
   public:

         /// Default constructor
      ForceModelList();

         /// Default destructor
      virtual ~ForceModelList()
      {};
         
         /// Clear the force model list
      void clear()
      { setFMT.clear(); }

         /**
         * Adds a generic force to the list
         * @param f Object which implements the ForceModel interface
         */
      void addForce(ForceModel* pForce)
      { forceList.push_back(pForce); };
      
         /// interface implementation for the 'ForceModel'
      virtual Vector<double> getDerivatives(UTCTime utc, EarthBody& bref, Spacecraft& sc);
      

      void setForceModelType(std::set<ForceModel::ForceModelType> fmt);

   protected:

         /// List of forces
      list<ForceModel*> forceList;

      std::set<ForceModel::ForceModelType> setFMT;

   }; // End of class 'ForceModelList'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_FORCE_MODEL_LIST_HPP



