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
* @file ForceModelList.hpp
* ForceModelList is a countainer for force models.
*/

#ifndef GPSTK_FORCE_MODEL_LIST_HPP
#define GPSTK_FORCE_MODEL_LIST_HPP

#include "EarthBody.hpp"
#include "ForceModel.hpp"

#include <list>
#include <set>

namespace gpstk
{
      /// @ingroup GeoDynamics 
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


         /**
         * Remove a generic force to the list
         * @param f Object which implements the ForceModel interface
         */
      void removeForce(ForceModel* pForce)
      { forceList.remove(pForce); }
      

         /// interface implementation for the 'ForceModel'
      virtual Vector<double> getDerivatives(UTCTime utc, EarthBody& bref, Spacecraft& sc);
      

      void setForceModelType(std::set<ForceModel::ForceModelType> fmt);

         /// return the force model name
      virtual std::string modelName() const
      { return "ForceModelList"; };


         /// return the force model index
      virtual int forceIndex() const
      { return FMI_LIST; }

         /// Show the exist force model
      void printForceModel(std::ostream& s);

   protected:

         /// List of forces
      std::list<ForceModel*> forceList;

      std::set<ForceModel::ForceModelType> setFMT;

   }; // End of class 'ForceModelList'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_FORCE_MODEL_LIST_HPP
