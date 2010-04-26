#pragma ident "$Id: $"

/**
 * @file RelativityEffect.hpp
 * Calculate Relativity Effect to earth satellite
 */

#ifndef GPSTK_RELATIVITY_EFFECT_HPP
#define GPSTK_RELATIVITY_EFFECT_HPP

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

#include "ForceModel.hpp"


namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{

      /** This class calculate Relativity Effect to earth satellite.
       *  
       *  Reference: Jisheng Li, P109
       *
       *  This Model is checked on Sep 25th,2009, OK!!!
       */
   class RelativityEffect : public ForceModel
   {
   public:
         /// Default constructor
      RelativityEffect(){}

         /// Default destructor
      virtual ~RelativityEffect(){}
      
         // this is the real one
      virtual void doCompute(UTCTime utc, EarthBody& rb, Spacecraft& sc);

         // Return force model name
      virtual std::string modelName()
      { return "RelativityEffect"; }

   protected:


   }; // End of class 'RelativityEffect'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_RELATIVITY_EFFECT_HPP
