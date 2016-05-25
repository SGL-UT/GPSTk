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
 * @file RelativityEffect.hpp
 * Calculate Relativity Effect to earth satellite
 */

#ifndef GPSTK_RELATIVITY_EFFECT_HPP
#define GPSTK_RELATIVITY_EFFECT_HPP

#include "ForceModel.hpp"


namespace gpstk
{
      /// @ingroup GeoDynamics 
      //@{

      /** This class calculate Relativity Effect to earth satellite.
       *  
       *  Reference: 'IERS STANDARDS 1996' Jisheng Li, P109
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
      
         /// this is the real one
      virtual void doCompute(UTCTime utc, EarthBody& rb, Spacecraft& sc);

         /// Return force model name
      virtual std::string modelName() const
      { return "RelativityEffect"; }

         /// return the force model index
      virtual int forceIndex() const
      { return FMI_RELATIVE; }

   protected:


   }; // End of class 'RelativityEffect'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_RELATIVITY_EFFECT_HPP
