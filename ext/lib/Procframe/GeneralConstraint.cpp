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
 * @file GeneralConstraint.cpp
 * GeneralConstraint
 */

#include "GeneralConstraint.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>

namespace gpstk
{
   using namespace std;

      // Feed the  constraint equations to the solver
   void GeneralConstraint::constraint( gnssDataMap& gdsMap )
      throw(InvalidConstraint)
   {
      try
      {
         realConstraint(gdsMap);
      }
      catch (...)
      {
      	InvalidConstraint e("Invalid constraint.");
         GPSTK_THROW(e);
      }
  
   }  // End of method 'GeneralConstraint::constraint'
         

      // Feed the  constraint equations to the solver
   void GeneralConstraint::constraint( gnssRinex& gRin )
      throw(InvalidConstraint)
   {
      gnssDataMap gdsMap;
      SourceID source( gRin.header.source );
      gdsMap.addGnssRinex( gRin );
      
      constraint(gdsMap);

   }  // End of method 'GeneralConstraint::constraint('
   

      // Feed the  constraint equations to the solver
   void GeneralConstraint::process( gnssRinex& gRin, GeneralEquations* gEquPtr )
   {
      if(gEquPtr)
      {
         
         solver.setEquationSystemConstraints(
                                          gEquPtr->getConstraintSystem(gRin) );

         CommonTime time(gRin.header.epoch);
         updateRefSat( time,
                       gEquPtr->getRefSatSourceMap(),
                       gEquPtr->getSourceRefSatMap() );
         
         solver.Process(gRin); 

         refsatSourceMap = gEquPtr->getRefSatSourceMap();
         sourceRefsatMap = gEquPtr->getSourceRefSatMap();

         constraint(gRin); 
      }
      else
      {
         solver.Process(gRin); 
         constraint(gRin); 
      }

   }  // End of method 'GeneralConstraint::process(...'


      // Feed the  constraint equations to the solver
   void GeneralConstraint::process( gnssDataMap& gdsMap,
                                    GeneralEquations* gEquPtr )
   { 
      if(gEquPtr)
      {
         solver.setEquationSystemConstraints(
                                         gEquPtr->getConstraintSystem(gdsMap) );

         CommonTime time( ( *gdsMap.begin() ).first );
         updateRefSat( time,
                       gEquPtr->getRefSatSourceMap(),
                       gEquPtr->getSourceRefSatMap() );
         
         solver.Process(gdsMap); 

         refsatSourceMap = gEquPtr->getRefSatSourceMap();
         sourceRefsatMap = gEquPtr->getSourceRefSatMap();

         constraint(gdsMap); 
      }
      else
      {
         solver.Process(gdsMap); 
         constraint(gdsMap); 
      }

   }  // End of method 'GeneralConstraint::process(...'

      // Low level metod impose a ConstraintSystem object to the solver
   int GeneralConstraint::constraintToSolver( ConstraintSystem& system,
                                              gnssDataMap& gdsMap )
   {
      try
      {
         Vector<double> meas;
         Matrix<double> design;
         Matrix<double> covariance;

         system.constraintMatrix(getVariables(),meas,design,covariance);
         
         /*
         cout << StringUtils::asString(getVariables()) << endl;

         cout << meas << endl;

         cout << design << endl;

         cout << covariance << endl;
         */

         if(meas.size()>0)
         {    
            solver.kFilter.MeasUpdate(meas,design,covariance);

            Vector<double> measVector = solver.getEquationSystem()
                                              .getPrefitsVector();

            Matrix<double> designMatrix = solver.getEquationSystem()
                                                .getGeometryMatrix();

            solver.solution = solver.kFilter.xhat;
            solver.covMatrix = solver.kFilter.P;
            solver.postfitResiduals = measVector 
                                     -(designMatrix * solver.solution);

            solver.postCompute(gdsMap);
         }

         return 0;
      }
      catch (...)
      {
         return -1;
      }

   }  // End of method 'GeneralConstraint::constraint('


   Matrix<double> GeneralConstraint::convertMatrix(size_t n,size_t oi,size_t ni)
   {
      // Check input
      if( n<1 || oi>=n || ni>=n )
      {
         Exception e("Invalid input, and check it.");
         GPSTK_THROW(e);
      }

      if(oi==ni) return ident<double>(n);
      
      Matrix<double> T(n,n,0.0);
      for( size_t i = 0; i < n; i++ )
      {
         if( i != ni )
         {
            T(i,ni)= -1.0;
            T(i,i) = (i == oi) ? 0.0 : 1.0;
         }
         else
         {  
            T(i,oi) = 1.0;
         }
      }

      return T;
   
   }  // End of method 'GeneralConstraint::convertMatrix()'


   Matrix<double> GeneralConstraint::convertMatrix(size_t n,size_t oi,size_t ni,
                                                   std::vector<int> iv)
   {  
      // Check input
      bool validInput(true);
      
      if( n<1 || oi>=n || ni>=n ) validInput = false;
      
      for(size_t i=0;i<iv.size();i++)
      {
         if(iv[i]<0 || iv[i]>=int(n)) 
         {
            validInput = false;
            break;
         }
      }

      if(validInput==false)
      {
         Exception e("Invalid input, and check it.");
         GPSTK_THROW(e);
      }

      if(oi==ni) return ident<double>(n);

      Matrix<double> T(n,n,0.0);
      for( size_t i = 0; i < n; i++ )
      {
         std::vector<int>::iterator it = find(iv.begin(),iv.end(),i);
         if(it==iv.end()) 
         {
            T(i,i) = 1.0;
            continue;
         }

         if( i != ni )
         {
            T(i,ni)= -1.0;
            T(i,i) = (i == oi) ? 0.0 : 1.0;
         }
         else
         {  
            T(i,oi) = 1.0;
         }
      }

      return T;
      
   }  // End of method 'GeneralConstraint::convertMatrix()'


      // Methods to parsing data from SolverGeneral

   Variable GeneralConstraint::getVariable( const SourceID& source, 
                                            const SatID& sat, 
                                            const TypeID& type )
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


   VariableSet GeneralConstraint::getVariables( const SourceID& source )
   {
      VariableSet vset;

      VariableSet unkSet( getVariables() );

      if(source==Variable::allSources) return unkSet; 
      
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


   VariableSet GeneralConstraint::getVariables( const SourceID& source,
                                                const TypeID& type )
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


   VariableSet GeneralConstraint::getVariables( const SourceID& source,
                                                const TypeIDSet& typeSet )
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


   VariableSet GeneralConstraint::getVariables( const SourceIDSet& sourceSet )
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


   VariableSet GeneralConstraint::getVariables( const SourceIDSet& sourceSet,
                                                const TypeID& type )
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


   VariableSet GeneralConstraint::getVariables( const SourceIDSet& sourceSet,
                                                const TypeIDSet& typeSet )
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


   VariableSet GeneralConstraint::getVariables( const SatID& sat )
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
            if(itv->getSatellite().system==SatID::systemGPS) 
               vset.insert(*itv);
         }
         else if(sat==Variable::allGlonassSats)
         {  
            if(itv->getSatellite().system==SatID::systemGlonass) 
               vset.insert(*itv);
         }
         else if(sat==Variable::allGalileoSats)
         {
            if(itv->getSatellite().system==SatID::systemGalileo) 
               vset.insert(*itv);
         }
         else
         {
            if(itv->getSatellite()==sat) vset.insert(*itv);
         }

      }
      
      return vset;

   }  // End of method 'GeneralConstraint::getVariables(const SatID& sat)'


   VariableSet GeneralConstraint::getVariables( const SatID& sat,
                                                const TypeID& type )
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


   VariableSet GeneralConstraint::getVariables( const SatID& sat,
                                                const TypeIDSet& typeSet )
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


   VariableSet GeneralConstraint::getVariables( const SourceID& source, 
                                                const SatID& sat, 
                                                const TypeID& type )
   {
      VariableSet vset;

      VariableSet varSet = getVariables(source,type);
      for(VariableSet::iterator itv=varSet.begin();
         itv!=varSet.end();
         ++itv)
      {
         if( itv->getSatellite()==sat ) vset.insert(*itv);
      }

      return vset;

   }  // End of method 'GeneralConstraint::getVariables(...'


   VariableSet GeneralConstraint::getVariables( const SourceID& source, 
                                                const SatIDSet& satSet, 
                                                const TypeID& type )
   {
      VariableSet vset;

      VariableSet varSet = getVariables(source,type);
      for(VariableSet::iterator itv=varSet.begin();
         itv!=varSet.end();
         ++itv)
      {
         SatIDSet::const_iterator it = satSet.find(itv->getSatellite());
         if( it != satSet.end() ) vset.insert(*itv);
      }

      return vset;

   }  // End of method 'GeneralConstraint::getVariables(...'


   Vector<double> GeneralConstraint::getSolution( const VariableSet& varSet )
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


   Matrix<double> GeneralConstraint::getCovariance( const VariableSet& varSet )
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


   GeneralConstraint& GeneralConstraint::changeState(
                                               const VariableList& varList,
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
      
      if(varNum != (int)convertMat.rows() || varNum != (int)convertMat.cols())
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
      Matrix<double> covariance = convertMat*matrixOfCovariance
                                 *transpose(convertMat);

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


   int GeneralConstraint::findIndexOfSat( const SatIDSet& satSet,
                                          const SatID& sat )
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


   void GeneralConstraint::stackVariables( VariableList& varList,
                                           const VariableSet& varSet )
   {
      for(VariableSet::const_iterator it= varSet.begin();
          it!=varSet.end();
          ++it)
      {
         varList.push_back(*it);
      }

   }  // End of method 'GeneralConstraint::stackVariables()'


   VariableSet GeneralConstraint::unionVariables( const VariableSet& vs1,
                                                  const VariableSet& vs2 )
   {
      VariableSet tempSet(vs1);
      for(VariableSet::const_iterator it=vs2.begin();
          it!=vs2.end();
          ++it)
      {
         tempSet.insert(*it);
      }

      return tempSet;

   }  // End of method 'GeneralConstraint::unionVariables()'


   VariableSet GeneralConstraint::differenceVariables( const VariableSet& vs1,
                                                       const VariableSet& vs2 )
   {
      VariableSet tempSet;
      for(VariableSet::const_iterator it=vs1.begin();
         it!=vs1.end();
         ++it)
      {
         VariableSet::const_iterator it2 = vs2.find(*it);
         if(it2==vs2.end()) tempSet.insert(*it);
      }
      for(VariableSet::const_iterator it=vs2.begin();
         it!=vs2.end();
         ++it)
      {
         VariableSet::const_iterator it2 = vs1.find(*it);
         if(it2==vs1.end()) tempSet.insert(*it);
      }

      return tempSet;

   }  // End of method 'GeneralConstraint::differenceVariables()'


   VariableSet GeneralConstraint::intersectionVariables(const VariableSet& vs1,
                                                        const VariableSet& vs2 )
   {
      VariableSet tempSet;
      for(VariableSet::const_iterator it=vs1.begin();
         it!=vs1.end();
         ++it)
      {
         VariableSet::const_iterator it2 = vs2.find(*it);
         if(it2!=vs2.end()) tempSet.insert(*it);
      }

      return tempSet;

   }  // End of method 'GeneralConstraint::intersectionVariables()'


      // Check if the satellite is a reference satellite.
   bool GeneralConstraint::isRefSat(const SatID& sat)
   {
      bool isRef(false);

      for(SatSourceMap::iterator it = refsatSourceMap.begin();
         it!=refsatSourceMap.end();
         ++it)
      {
         if(it->first==sat)
         {
            isRef = true;
            break;
         }
      }

      for(SourceSatMap::iterator it = sourceRefsatMap.begin();
         it!=sourceRefsatMap.end();
         ++it)
      {
         if(it->second==sat)
         {
            isRef = true;
            break;
         }
      }

      return isRef;

   }  // End of method 'GeneralConstraint::isRefSat()'



}  // End of namespace 'gpstk'
