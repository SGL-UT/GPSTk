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
* @file ForceModelList.cpp
* ForceModelList is a countainer for force models.
*/

#include "ForceModelList.hpp"


namespace gpstk
{

   ForceModelList::ForceModelList()
   {
         //setFMT.insert(ForceModel::Cd);
         //setFMT.insert(ForceModel::Cr);
      setFMT.clear();
      clear();
   }

      // interface implementation for the 'ForceModel'
   Vector<double> ForceModelList::getDerivatives(UTCTime utc, EarthBody& bref, Spacecraft& sc)
   {
      const int np = setFMT.size(); //getNP();

      a.resize(3,0.0);
      da_dr.resize(3,3,0.0);
      da_dv.resize(3,3,0.0);
      da_dp.resize(3,np,0.0);

      da_dcd.resize(3,1,0.0);
      da_dcr.resize(3,1,0.0);
      
      for(std::list<ForceModel*>::iterator it = forceList.begin();
          it != forceList.end();
          ++it)
      {
         (*it)->doCompute(utc,bref,sc);

         a      += (*it)->getAccel();
         da_dr   += (*it)->partialR();
         da_dv   += (*it)->partialV();
            //da_dp   += (*it)->partialP();

            //cout<<(*it)->modelName()<<endl;
         
         da_dcd   += (*it)->partialCd();
         da_dcr   += (*it)->partialCr();
         
      }
      
         // declare a counter
      int i = 0;  

      for(std::set<ForceModel::ForceModelType>::iterator it = setFMT.begin();
          it!=setFMT.end();
          it++)
      {
         if((*it)==ForceModel::Cd)
         {
            da_dp(0,i) = da_dcd(0,0);
            da_dp(1,i) = da_dcd(1,0);
            da_dp(2,i) = da_dcd(2,0);
         }
         else if((*it)==ForceModel::Cr)
         {
            da_dp(0,i) = da_dcr(0,0);
            da_dp(1,i) = da_dcr(1,0);
            da_dp(2,i) = da_dcr(2,0);
         }
         else
         {
            Exception e("Error in ForceModelList::getDerivatives():Unexpect ForceModelType");
            GPSTK_THROW(e);
         }

         i++;
      }  

         /* Transition Matrix (6+np)*(6+np)
            |                          |
            | dr_dr0   dr_dv0   dr_dp0  |
            |                          |
            phi= | dv_dr0   dv_dv0   dv_dp0  |
            |                          |
            | 0         0          I      |
            |                          |
         */
      Matrix<double> phi = sc.getTransitionMatrix();

         /* A Matrix (6+np)*(6+np)
            |                       |
            | 0         I      0      |
            |                       |
            A = | da_dr      da_dv   da_dp  |
            |                       |
            | 0         0      0      |
            |                       |
         */
      Matrix<double> A = getAMatrix();
      
         /* dphi Matrix
            |                          |
            | dv_dr0   dv_dv0   dv_dp0 |
            |                          |
            dphi = | da_dr0   da_dv0   da_dp0 |
            |                          |
            | 0         0         0    |
            |                          |

            da_dr0 = da_dr*dr_dr0 + da_dv*dv_dr0

            da_dv0 = da_dr*dr_dv0 + da_dv*dv_dv0

            da_dp0 = da_dr*dr_dp0 + da_dv*dv_dp0 + da_dp0;
         */
      Matrix<double> dphi = A * phi;


      Vector<double> r = sc.R();
      Vector<double> v = sc.V();
      
      gpstk::Vector<double> dy(42+6*np,0.0);   //////////////////////////////////////////////////////////////////////////

      dy(0) = v(0);      // v
      dy(1) = v(1);
      dy(2) = v(2);
      dy(3) = a(0);      // a
      dy(4) = a(1);
      dy(5) = a(2);
      
      for(int i=0;i<3;i++)
      {
         for(int j=0;j<3;j++)
         {
            dy(6+i*3+j) = dphi(i,j);            // dv_dr0
            dy(15+i*3+j) = dphi(i,j+3);         // dv_dv0
            dy(24+3*np+i*3+j) = dphi(i+3,j);    // da_dr0
            dy(33+3*np+i*3+j) = dphi(i+3,j+3);  // da_dv0   
         }
         for(int k=0;k<np;k++)
         {
            dy(24+i*np+k) = dphi(i,i*np+k);         // dv_dp0
            dy(42+3*np+i*np+k) = dphi(i+3,i*np+k);  // da_dp0
         }
      }

      return dy;

   }  // End of method 'ForceModelList::getDerivatives()'


   void ForceModelList::printForceModel(std::ostream& s)
   {
         // a counter
      int i(1);

      for(std::list<ForceModel*>::iterator it = forceList.begin();
          it != forceList.end();
          ++it)
      {
         s << std::setw(3) << i << " "
           << (*it)->forceIndex() << " "
           << (*it)->modelName() << std::endl;

         i++;
      }

   }  // End of method 'ForceModelList::printForceModel()'

   void ForceModelList::setForceModelType(std::set<ForceModel::ForceModelType> fmt)
   {
      setFMT.clear();
      for(std::set<ForceModel::ForceModelType>::iterator it = fmt.begin();
          it != fmt.end();
          ++it)
      {
         setFMT.insert(*it);
      }

   }  // End of method ''



}  // End of namespace 'gpstk'
