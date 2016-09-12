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
 * @file AtmosphericDrag.hpp
 * This class computes the acceleration due to drag on a satellite
 * using an Earth atmosphere model that conforms to the computeDensity 
 * abstract method.
 */

#ifndef GPSTK_ATMOSPHERIC_DRAG_HPP
#define GPSTK_ATMOSPHERIC_DRAG_HPP

#include "ForceModel.hpp"
#include "ASConstant.hpp"
#include "Spacecraft.hpp"

namespace gpstk
{
   
      /// @ingroup GeoDynamics 
      //@{

      /**
       * This class computes the acceleration due to drag on a satellite
       * using an Earth atmosphere model that conforms to the computeDensity 
       * abstract method.
       *
       *  This Model is checked on Sep 28th,2009, OK!!!
       *
       */
   class AtmosphericDrag : public ForceModel
   {
   public:

         /// Default constructor
      AtmosphericDrag() : dailyF107(150.0), averageF107(150.0), dailyKp(3.0) 
      {};

         /// Default destructor
      virtual ~AtmosphericDrag() {};


         /** Abstract class requires the subclass to compute the atmospheric density.
          * @param ref EarthRef object.
          * @param r Position vector.
          * @param v Velocity vector
          * @return Atmospheric density in kg/m^3
          */
      virtual double computeDensity(UTCTime t, EarthBody& rb, Vector<double> r,Vector<double> v) = 0;
      

         /// Return force model name
      virtual std::string modelName() const
      {return "AtmosphericDrag";}

         /// return the force model index
      virtual int forceIndex() const
      { return FMI_DRAG; }
       
         /// this is the real one
      virtual void doCompute(UTCTime utc, EarthBody& rb, Spacecraft& sc);

      virtual void setSpaceData(double dayF107,double aveF107, double dayKp)
      { dailyF107 = dayF107; averageF107 = aveF107; dailyKp = dayKp; }


   protected:
      double dailyF107;
      double averageF107;
      double dailyKp;

         /// dadcd
      Vector<double> dadcd;

         // Constant
      enum CiraSize{ CIRA_SIZE = 24 };

         /// CIRA Coefficients, this is used to calcute da_dr
      static const double rho_0[CIRA_SIZE];
      static const double H[CIRA_SIZE];
      static const double h0[CIRA_SIZE];
      
   }; // End of class 'gpstk'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_ATMOSPHERIC_DRAG_HPP
