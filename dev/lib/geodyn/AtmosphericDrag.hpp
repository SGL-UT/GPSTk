#pragma ident "$Id: $"

/**
 * @file AtmosphericDrag.hpp
 * This class computes the acceleration due to drag on a satellite
 * using an Earth atmosphere model that conforms to the computeDensity 
 * abstract method.
 */

#ifndef GPSTK_ATMOSPHERIC_DRAG_HPP
#define GPSTK_ATMOSPHERIC_DRAG_HPP


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
#include "ASConstant.hpp"
#include "Spacecraft.hpp"

namespace gpstk
{
   
      /** @addtogroup GeoDynamics */
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

