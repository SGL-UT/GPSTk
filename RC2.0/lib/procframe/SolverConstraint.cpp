#pragma ident "$Id$"

/**
 * @file SolverConstraint.hpp
 * Solver Constrain.
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


#include "SolverConstraint.hpp"
#include "SolverGeneral.hpp"
#include <iostream>
#include <iomanip>
#include <string>

namespace gpstk
{
   using namespace std;

   int SolverConstraint::constraint(gnssDataMap& gdsMap)
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
   }

   int SolverConstraint::constraint(gnssSatTypeValue& gData)
   {
      gnssRinex g1;
      g1.header = gData.header;
      g1.body = gData.body;

      int toReturn = constraint(g1);

      gData.body = g1.body;

      return toReturn;
   }

   int SolverConstraint::constraint(gnssRinex& gRin)
   {
      gnssDataMap gdsMap;
      SourceID source( gRin.header.source );
      gdsMap.addGnssRinex( gRin );
      
      int toReturn = constraint(gdsMap);

      return toReturn;
   }

   int SolverConstraint::constraintToSolver( ConstraintSystem& system,
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
   }


   VariableSet SolverConstraint::getVariables()
   {
      EquationSystem eqs = solver.getEquationSystem();
      VariableSet unkSet( eqs.getVarUnknowns() );
      
      return unkSet;
   }

   VariableSet SolverConstraint::getVariables(const SourceID& source)
   {
      VariableSet vset;

      EquationSystem eqs = solver.getEquationSystem();
      VariableSet unkSet( eqs.getVarUnknowns() );

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
   }

   VariableSet SolverConstraint::getVariables(const SourceID& source,
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
   }

   VariableSet SolverConstraint::getVariables(const SourceID& source,
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
   }

   VariableSet SolverConstraint::getVariables(const SourceIDSet& sourceSet)
   {
      VariableSet vset;

      EquationSystem eqs = solver.getEquationSystem();
      VariableSet unkSet( eqs.getVarUnknowns() );

      for( VariableSet::const_iterator itv = unkSet.begin();
         itv != unkSet.end();
         ++itv )
      {
         SourceIDSet::const_iterator it = sourceSet.find( (*itv).getSource() );
         if( it!=sourceSet.end() ) vset.insert( *itv );
      }

      return vset;
   }

   VariableSet SolverConstraint::getVariables(const SourceIDSet& sourceSet,
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
   }

   VariableSet SolverConstraint::getVariables(const SourceIDSet& sourceSet,
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
   }

   VariableSet SolverConstraint::getVariables(const SatID& sat)
   {
      VariableSet vset;
      
      EquationSystem eqs = solver.getEquationSystem();
      VariableSet unkSet( eqs.getVarUnknowns() );

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
   }

   VariableSet SolverConstraint::getVariables(const SatID& sat,const TypeID& type)
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
   }

   VariableSet SolverConstraint::getVariables(const SatID& sat,
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
   }

   VariableSet SolverConstraint::getVariables(const SourceID& source, const SatID& sat, const TypeID& type)
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
   }

   Vector<double> SolverConstraint::getSolution(const VariableSet& varSet)
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
   }

   Matrix<double> SolverConstraint::getCovariance(const VariableSet& varSet)
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
   }


}  // End of namespace 'gpstk'
