#pragma ident "$Id: $"

/**
* @file Spacecraft.hpp
* The Spacecraft class encapsulates physical parameters.
*/

#ifndef GPSTK_SPACECRAFT_HPP
#define GPSTK_SPACECRAFT_HPP

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

#include <iostream>
#include <string>
#include "Vector.hpp"
#include "Matrix.hpp"
#include "UTCTime.hpp"

namespace gpstk
{
   using namespace std;

      /** @addtogroup GeoDynamics */
      //@{

      /**
       * The Spacecraft class encapsulates physical parameters
       * 
       */
   class Spacecraft
   {
   public:

         /// Default constructor
      Spacecraft(std::string name = "spacecraft");

         /// Default destructor
      ~Spacecraft() {};

         /// SC position(R), velocity(V) and dynamic parameters(P)
      Vector<double> R() {return r;}
      Vector<double> V() {return v;}
      Vector<double> P() {return p;}
      
         /// SC derivatives
      Vector<double> dR_dR0() {return dr_dr0;}
      Vector<double> dR_dV0() {return dr_dv0;}
      Vector<double> dR_dP0() {return dr_dp0;}
      Vector<double> dV_dR0() {return dv_dr0;}
      Vector<double> dV_dV0() {return dv_dv0;}
      Vector<double> dV_dP0() {return dv_dp0;}

         /// Get number of force model parameters
      int getNumOfP()
      { return p.size(); }
      
         /// initialize the state vector with position and velocity and force model parameters 
      void initStateVector(Vector<double> rv, Vector<double> dp = Vector<double>(0,0.0));
      
         /// Methods to handle SC state vector
      Vector<double> getStateVector();
      void setStateVector(Vector<double> y);

         /// Methods to handle SC transition matrix
      Matrix<double> getTransitionMatrix();
      void setTransitionMatrix(Matrix<double> phiMatrix);
      
         /// Method to get SC state transition matrix 6*6
      Matrix<double> getStateTransitionMatrix();

         /// Method to get SC sensitivity matrix 6*np
      Matrix<double> getSensitivityMatrix();


         // Methods to handle SC physical parameters
      
      std::string getName()
      { return scName; }


      Spacecraft& setName(std::string satName)
      { scName = satName; return (*this);}


      double getReflectCoeff()
      { return reflectCoeff; }


      Spacecraft& setReflectCoeff(double Cr)
      { reflectCoeff = Cr; return (*this); }

      double getDragCoeff()
      { return dragCoeff; }


      Spacecraft& setDragCoeff(double Cd)
      { dragCoeff = Cd; return (*this);}
      

      double getDragArea()
      { return crossArea; }

      Spacecraft& setDragArea(double satArea)
      { crossArea = satArea; return (*this);}
      

      double getSRPArea()
      { return crossArea; }


      Spacecraft& setSRPArea(double satArea)
      { crossArea = satArea; return (*this);}
      

      double getDryMass()
      {return dryMass;}


      Spacecraft& setDryMass(double satMass)
      { dryMass = satMass; return (*this);}
      


         /// some basic test
      void test();


   protected:

      void resetState();
      
         /// name: The name of the spacecraft (e.g. "NCC-1701-D") 
      string scName;
      
         /// Object to hold epoch in UTC
      UTCTime utc;

         /// state vector     6*n + 42
      Vector<double> r;         // 3 Position
      Vector<double> v;         // 3 Velocity
      Vector<double> p;         // n dynamical parameters [this is important]

      Vector<double> dr_dr0;      // 3*3  I
      Vector<double> dr_dv0;      // 3*3  0
      Vector<double> dr_dp0;      // 3*n  0
      Vector<double> dv_dr0;      // 3*3  0
      Vector<double> dv_dv0;      // 3*3  I
      Vector<double> dv_dp0;      // 3*n  0

         /// Coefficient of Reflectivity
      double reflectCoeff;

         /// Coefficient of drag
      double dragCoeff;

         /// Cross sectional (reflective) area [m^2]
      double crossArea;

         /// mass [kg]
      double dryMass;

   }; // End of class 'Spacecraft'


      /**
       * Stream output for DayTime objects.  Typically used for debugging.
       * @param s stream to append formatted DayTime to.
       * @param t DayTime to append to stream \c s.
       * @return reference to \c s.
       */
   std::ostream& operator<<( std::ostream& s,
                             const gpstk::Spacecraft& sc );
      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_SPACECRAFT_HPP

