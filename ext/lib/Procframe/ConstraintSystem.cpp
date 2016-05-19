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
//  Wei Yan - Chinese Academy of Sciences . 2011
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
 * @file ConstraintSystem.cpp
 * 
 */

#include "ConstraintSystem.hpp"
#include <vector>

namespace gpstk
{
      // Remove a single constraint
   ConstraintSystem& ConstraintSystem::removeConstraint(
                                                  const Constraint& constraint )
   {
      ConstraintList backupList;
      
      for(ConstraintList::iterator it= constraintList.begin();
         it!=constraintList.end();
         ++it)
      {
         bool isEqual(true);

         if(it->header.prefit!=constraint.header.prefit) isEqual=false;
         if(it->header.variance!=constraint.header.variance) isEqual=false;
         if(it->body != constraint.body) isEqual = false;

         if( !isEqual ) backupList.push_back(*it);
      }

      clearConstraint();
      
      for(ConstraintList::iterator it= backupList.begin();
         it!=backupList.end();
         ++it)
      {
         addConstraint(*it);
      }
      
      return (*this);

   }  // End of method 'ConstraintSystem::removeConstraint()'


      // Method to set multi-constraints
   ConstraintSystem& ConstraintSystem::setConstraint(const VariableSet& varSet, 
                                                   const Vector<double>& prefit)
   {
      // Fist, we check the size of inputs
      const size_t size = varSet.size();

      if( prefit.size()!=size )
      {
         Exception e("The input size doesn't match.");
         GPSTK_THROW(e);
      }

      clearConstraint();

      int i(0);
      
      for(VariableSet::const_iterator it = varSet.begin();
         it != varSet.end();
         ++it)
      {
         Constraint constraint;
         constraint.header.prefit = prefit[i];
         constraint.body[*it] = 1.0;

         addConstraint(constraint);
   
         i++;
      }

      return (*this);

   }  // End of method 'ConstraintSystem::setConstraint()'


      // Method to set multi-constraints
   ConstraintSystem& ConstraintSystem::setConstraint(const VariableSet& varSet,
                                                   const Vector<double>& prefit,
                                                   const Matrix<double>& design)
   {
      // Fist, we check the size of inputs
      const size_t size = varSet.size();
      
      if( (prefit.size()!=size) || design.rows()!=size || design.cols()!=size)
      {
         Exception e("The input size doesn't match.");
         GPSTK_THROW(e);
      }
      
      clearConstraint();

      std::vector<Variable> vars;
      for(VariableSet::const_iterator it = varSet.begin();
         it != varSet.end();
         ++it)
      {
         vars.push_back(*it);
      }
      
      for(size_t i=0; i<vars.size(); i++)
      {
         VariableDataMap dataMap;
         for(size_t k=0; k<size; k++)
         {
            if(design[i][k]!=0.0) dataMap[ vars[k] ] = design[i][k];
         }

         Constraint constraint;
         constraint.header.prefit = prefit[i];
         constraint.body = dataMap;

         addConstraint(constraint);
      }
      
      return (*this);

      
   }  // End of method 'ConstraintSystem::setConstraint()'



   ConstraintSystem& ConstraintSystem::constraintMatrix(
                                                    const VariableSet& allVar,
                                                    Vector<double>& prefit,
                                                    Matrix<double>& design,
                                                    Matrix<double>& covariance )
      throw(InvalidConstraintSystem)
   {
      const int rowSize = constraintList.size();
      const int colSize = allVar.size();

      prefit.resize(rowSize,0.0);
      design.resize(rowSize,colSize,0.0);
      covariance.resize(rowSize,rowSize,0.0);
      
      int irow(0);

      for(ConstraintList::iterator it=constraintList.begin();
         it!=constraintList.end();
         ++it)
      {
         prefit[irow] = it->header.prefit;
         covariance[irow][irow] = it->header.variance;
         
         VariableDataMap dataMap = it->body;
         
         
         for(VariableDataMap::iterator itv=dataMap.begin();
            itv!=dataMap.end();
            ++itv)
         {
            /// 
            VariableSet::const_iterator itt = allVar.find(itv->first);
            if(itt==allVar.end())
            {
               InvalidConstraintSystem e("The variable not exist in the input");
               GPSTK_THROW(e);
            }

            int icol(0);
            for(VariableSet::const_iterator itt2=allVar.begin();
               itt2!=allVar.end();
               ++itt2)
            {
               if(itt == itt2) break;
               icol++;
            }
            
            design[irow][icol] = itv->second;
         }


         irow++;
      }

      return (*this);

   }  // End of method 'ConstraintSystem::constraintMatrix()'


      // Add a constraint list
   ConstraintSystem& ConstraintSystem::addConstraintList(
                                             const ConstraintList& equationList)
   {
      for(ConstraintList::const_iterator it = equationList.begin();
         it != equationList.end();
         ++it)
      {
         addConstraint(*it);
      }

      return (*this);
   }

}  // End of namespace gpstk
