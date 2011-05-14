#pragma ident "$Id$"

/**
 * @file GeneralConstraint.hpp
 * GeneralConstraint
 */

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


#include "GeneralConstraint.hpp"
#include "SolverGeneral.hpp"
#include <iostream>
#include <iomanip>
#include <string>

namespace gpstk
{
   using namespace std;

      // Feed the  constraint equations to the solver
   int GeneralConstraint::constraint(gnssDataMap& gdsMap)
   {
      try
      {
         realConstraint(gdsMap);
      }
      catch (...)
      {
      	return -1;
      }
  
      return 0;

   }  // End of method 'GeneralConstraint::constraint'
         

      // Feed the  constraint equations to the solver
   int GeneralConstraint::constraint(gnssSatTypeValue& gData)
   {
      gnssRinex g1;
      g1.header = gData.header;
      g1.body = gData.body;

      int toReturn = constraint(g1);

      gData.body = g1.body;

      return toReturn;

   }  // End of method 'GeneralConstraint::constraint('


      // Feed the  constraint equations to the solver
   int GeneralConstraint::constraint(gnssRinex& gRin)
   {
      gnssDataMap gdsMap;
      SourceID source( gRin.header.source );
      gdsMap.addGnssRinex( gRin );
      
      int toReturn = constraint(gdsMap);

      return toReturn;

   }  // End of method 'GeneralConstraint::constraint('
      

      // Low level metod impose a ConstraintSystem object to the solver
   int GeneralConstraint::constraintToSolver( ConstraintSystem& system,
                                              gnssDataMap& gdsMap)
   {
      try
      {
         Vector<double> meas;
         Matrix<double> design;
         Matrix<double> covariance;

         system.constraintMatrix(getVariables(),meas,design,covariance);

         if(meas.size()>0)
         {    
            solver.kFilter.MeasUpdate(meas,design,covariance);

            Vector<double> measVector = solver.getEquationSystem().getPrefitsVector();
            Matrix<double> designMatrix = solver.getEquationSystem().getGeometryMatrix();

            solver.solution = solver.kFilter.xhat;
            solver.covMatrix = solver.kFilter.P;
            solver.postfitResiduals = measVector - (designMatrix * solver.solution);

            solver.postCompute(gdsMap);
         }

         return 0;
      }
      catch (...)
      {
         return -1;
      }

   }  // End of method 'GeneralConstraint::constraint('

      // Methods to parsing data from SolverGeneral

   Variable GeneralConstraint::getVariable(const SourceID& source, const SatID& sat, const TypeID& type)
   {
      VariableSet vset;

      VariableSet varSet = getVariables(source,type);
      for(VariableSet::iterator itv=varSet.begin();
         itv!=varSet.end();
         ++itv)
      {
         if( itv->getType()==type ) return (*itv);
      }

      Exception e("The desirable variable not exist int the solver.");
      GPSTK_THROW(e);

      return Variable();

   }  // End of method 'GeneralConstraint::getVariables(...'


   VariableSet GeneralConstraint::getVariables(const SourceID& source)
   {
      VariableSet vset;

      VariableSet unkSet( getVariables() );

      if(source==Variable::allSources) unkSet; 
      
      for( VariableSet::const_iterator itv = unkSet.begin();
         itv != unkSet.end();
         ++itv )
      {
         if( (itv->getSource() == source) && itv->getSourceIndexed() ) 
         {
            vset.insert( *itv );
         }
      }
      
      return vset;

   }  // End of method 'GeneralConstraint::getVariables(const SourceID& source)'


   VariableSet GeneralConstraint::getVariables(const SourceID& source,
                                               const TypeID& type)
   {
      VariableSet vset;

      VariableSet varSet = getVariables(source);
      for(VariableSet::iterator itv=varSet.begin();
         itv!=varSet.end();
         ++itv)
      {
         if( (itv->getType()==type) && itv->getSourceIndexed() )
         { 
            vset.insert(*itv);
         } 
      }

      return vset;

   }  // End of method 'GeneralConstraint::getVariables(...'


   VariableSet GeneralConstraint::getVariables(const SourceID& source,
                                               const TypeIDSet& typeSet)
   {
      VariableSet vset;

      VariableSet varSet = getVariables(source);
      for(VariableSet::iterator itv=varSet.begin();
         itv!=varSet.end();
         ++itv)
      {
         TypeIDSet::const_iterator it = typeSet.find(itv->getType());
         if( (it!=typeSet.end()) && itv->getSourceIndexed() )
         { 
            vset.insert(*itv);
         } 
      }

      return vset;

   }  // End of method 'GeneralConstraint::getVariables'


   VariableSet GeneralConstraint::getVariables(const SourceIDSet& sourceSet)
   {
      VariableSet vset;

      VariableSet unkSet( getVariables() );

      for( VariableSet::const_iterator itv = unkSet.begin();
         itv != unkSet.end();
         ++itv )
      {
         SourceIDSet::const_iterator it = sourceSet.find( (*itv).getSource() );
         if( it!=sourceSet.end() ) vset.insert( *itv );
      }

      return vset;

   }  // End of method 'GeneralConstraint::getVariables(...'


   VariableSet GeneralConstraint::getVariables(const SourceIDSet& sourceSet,
                                               const TypeID& type)
   {
      VariableSet vset;
      
      VariableSet varSet = getVariables(sourceSet);
      for(VariableSet::iterator itv=varSet.begin();
         itv!=varSet.end();
         ++itv)
      {
         if( (itv->getType()==type) && itv->getSourceIndexed() )
         { 
            vset.insert(*itv);
         } 
      }

      return vset;

   }  // End of method 'GeneralConstraint::getVariables(...'


   VariableSet GeneralConstraint::getVariables(const SourceIDSet& sourceSet,
                                               const TypeIDSet& typeSet)
   {
      VariableSet vset;

      VariableSet varSet = getVariables(sourceSet);
      for(VariableSet::iterator itv=varSet.begin();
         itv!=varSet.end();
         ++itv)
      {
         TypeIDSet::const_iterator it = typeSet.find(itv->getType());
         if( (it!=typeSet.end()) && itv->getSourceIndexed() )
         { 
            vset.insert(*itv);
         } 
      }

      return vset;

   }  // End of method 'GeneralConstraint::getVariables(...'


   VariableSet GeneralConstraint::getVariables(const SatID& sat)
   {
      VariableSet vset;
      
      VariableSet unkSet( getVariables() );

      if(sat==Variable::noSats) return vset;

      for( VariableSet::const_iterator itv = unkSet.begin();
         itv != unkSet.end();
         ++itv )
      {
         if( !(!itv->getSourceIndexed() && itv->getSatIndexed()) ) 
         {
            continue;
         }

         if(sat==Variable::allSats)
         {
            vset.insert(*itv);
         }
         else if(sat==Variable::allGPSSats)
         {
            if(itv->getSatellite().system==SatID::systemGPS) vset.insert(*itv);
         }
         else if(sat==Variable::allGlonassSats)
         {  
            if(itv->getSatellite().system==SatID::systemGlonass) vset.insert(*itv);
         }
         else if(sat==Variable::allGalileoSats)
         {
            if(itv->getSatellite().system==SatID::systemGalileo) vset.insert(*itv);
         }
         else
         {
            if(itv->getSatellite()==sat) vset.insert(*itv);
         }

      }
      
      return vset;

   }  // End of method 'GeneralConstraint::getVariables(const SatID& sat)'


   VariableSet GeneralConstraint::getVariables(const SatID& sat,
                                               const TypeID& type)
   {
      VariableSet vset;

      VariableSet varSet = getVariables(sat);
      for(VariableSet::iterator itv=varSet.begin();
         itv!=varSet.end();
         ++itv)
      {
         if( (itv->getType()==type) ) vset.insert(*itv);
      }

      return vset;

   }  // End of method 'GeneralConstraint::getVariables(const SatID& sat,...)'


   VariableSet GeneralConstraint::getVariables(const SatID& sat,
                                               const TypeIDSet& typeSet)
   {
      VariableSet vset;

      VariableSet varSet = getVariables(sat);
      for(VariableSet::iterator itv=varSet.begin();
         itv!=varSet.end();
         ++itv)
      {
         TypeIDSet::const_iterator it = typeSet.find(itv->getType());
         if( (it!=typeSet.end()) ) vset.insert(*itv);
      }

      return vset;

   }  // End of method 'GeneralConstraint::getVariables(...'


   VariableSet GeneralConstraint::getVariables(const SourceID& source, const SatID& sat, const TypeID& type)
   {
      VariableSet vset;

      VariableSet varSet = getVariables(source,type);
      for(VariableSet::iterator itv=varSet.begin();
         itv!=varSet.end();
         ++itv)
      {
         if( itv->getType()==type ) vset.insert(*itv);
      }

      return vset;

   }  // End of method 'GeneralConstraint::getVariables(...'


   Vector<double> GeneralConstraint::getSolution(const VariableSet& varSet)
   {
      Vector<double> solution(varSet.size(),0.0);
      
      int i(0);
      for(VariableSet::const_iterator it=varSet.begin();
         it!=varSet.end();
         ++it)
      {
         solution[i] = solver.getSolution(*it);

         i++;
      }

      return solution;

   }  // End of method 'GeneralConstraint::getSolution(...'


   Matrix<double> GeneralConstraint::getCovariance(const VariableSet& varSet)
   {
      Matrix<double> covariance(varSet.size(),varSet.size(),0.0);
      
      int i(0);
      for(VariableSet::const_iterator iti=varSet.begin();
         iti!=varSet.end();
         ++iti)
      {
         int j(0);
         
         for(VariableSet::const_iterator itj=varSet.begin();
            itj!=varSet.end();
            ++itj)
         {
            covariance[i][j] = solver.getCovariance(*iti,*itj);
            j++;
         }

         i++;
      }

      return covariance;

   }  // End of method 'GeneralConstraint::getCovariance(...'


   GeneralConstraint& GeneralConstraint::changeState( const VariableList& varList,
                                                      const Matrix<double>& convertMat)
   {
      VariableSet allVariable = getCurrentUnknowns();

      // check input 
      int varNum(0);
      for(VariableList::const_iterator it = varList.begin();
          it!=varList.end();
          ++it)
      {
         if(allVariable.find(*it)==allVariable.end())
         {
            Exception e("The variable doesn't exist in the solver.");
            GPSTK_THROW(e);
         }

         varNum++;
      }
      
      if(varNum!=convertMat.rows() || varNum!=convertMat.cols())
      {
         Exception e("The size of input doesn't match.");
         GPSTK_THROW(e);
      }

      const int numOfVar(varNum);

      Vector<double> vectorOfSolution(numOfVar,0.0);
      Matrix<double> matrixOfCovariance(numOfVar,numOfVar,0.0);

      int i = 0;
      for(VariableList::const_iterator iti = varList.begin();
          iti != varList.end();
          ++iti)
      {
         vectorOfSolution(i) = solver.getSolution(*iti);

         VariableList tempList(varList);

         int j(0);
         for(VariableList::iterator itj = tempList.begin();
            itj!=tempList.end();
            ++itj)
         {
            matrixOfCovariance(i,j) = solver.getCovariance(*iti,*itj);

            j++;
         }

         i++;
      }

      Vector<double> solution = convertMat*vectorOfSolution;
      Matrix<double> covariance = convertMat*matrixOfCovariance*transpose(convertMat);

      i = 0;
      for(VariableList::const_iterator iti=varList.begin();
         iti!=varList.end();
         ++iti)
      {
         setSolution(*iti,solution(i));

         std::list<Variable> tempList(varList);

         int j(0);
         for(std::list<Variable>::iterator itj = tempList.begin();
            itj!=tempList.end();
            ++itj)
         {
            setCovariance(*iti,*itj,covariance(i,j));

            j++;
         }

         i++;
      }
      
      return (*this);

   }  // Ebd if method 'GeneralConstraint::changeState()'


   int GeneralConstraint::findIndexOfSat(const SatIDSet& satSet,const SatID& sat)
   {
      int indexOfSat(-1);

      int i(0);
      for(SatIDSet::const_iterator it=satSet.begin();
          it!=satSet.end();
          ++it)
      {
         if((*it)==sat) indexOfSat = i;

         i++;
      }

      return indexOfSat;

   }  // End of method 'GeneralConstraint::findIndexOfSat()'


   void GeneralConstraint::stackVariables(VariableList& varList,const VariableSet& varSet)
   {
      for(VariableSet::const_iterator it= varSet.begin();
          it!=varSet.end();
          ++it)
      {
         varList.push_back(*it);
      }

   }  // End of method 'GeneralConstraint::stackVariables()'


}  // End of namespace 'gpstk'
