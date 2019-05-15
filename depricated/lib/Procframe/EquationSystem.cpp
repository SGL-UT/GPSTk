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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2009
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
 * @file EquationSystem.cpp
 * Class to define and handle complex equation systems for solvers.
 */

#include "EquationSystem.hpp"
#include <iterator>

namespace gpstk
{



      // General white noise stochastic model
   WhiteNoiseModel EquationSystem::whiteNoiseModel;



      /* Add a new equation to be managed.
       *
       * @param equation   Equation object to be added.
       *
       */
   EquationSystem& EquationSystem::addEquation( const Equation& equation )
   {

         // Add "equation" to "equationDescriptionList"
      equationDescriptionList.push_back(equation);

         // We must "Prepare()" this EquationSystem
      isPrepared = false;

      return (*this);

   }  // End of method 'EquationSystem::addEquation()'



      /* Remove an Equation being managed. In this case the equation is
       * identified by its independent term.
       *
       * @param indterm  Variable object of the equation independent term
       *                 (measurement type).
       *
       * \warning All Equations with the same independent term will be
       *          erased.
       */
   EquationSystem& EquationSystem::removeEquation( const Variable& indterm )
   {

         // Create a backup list
      std::list<Equation> backupList;

         // Visit each "Equation" in "equationDescriptionList"
      for( std::list<Equation>::const_iterator itEq =
                                                equationDescriptionList.begin();
           itEq != equationDescriptionList.end();
           ++itEq )
      {

            // If current equation has a different independent term, save it
         if ( (*itEq).getIndependentTerm() != indterm )
         {
            backupList.push_back(*itEq);
         }

      }

         // Clear the full contents of this object
      clearEquations();

         // Add each "Equation" in the backup equation list
      for( std::list<Equation>::const_iterator itEq = backupList.begin();
           itEq != backupList.end();
           ++itEq )
      {
         addEquation(*itEq);
      }

         // We must "Prepare()" this EquationSystem again
      isPrepared = false;

      return (*this);

   }  // End of method 'EquationSystem::removeEquation()'



      // Remove all Equation objects from this EquationSystem.
   EquationSystem& EquationSystem::clearEquations()
   {
         // First, clear the "equationDescriptionList"
      equationDescriptionList.clear();

      isPrepared = false;

      return (*this);

   }  // End of method 'EquationSystem::clearEquations()'



      /* Prepare this object to carry out its work.
       *
       * @param gData   GNSS data structure (GDS).
       *
       */
   EquationSystem& EquationSystem::Prepare( gnssRinex& gData )
   {

         // First, create a temporary gnssDataMap
      gnssDataMap myGDSMap;

         // Get gData into myGDSMap
      myGDSMap.addGnssRinex( gData );

         // Call the map-enabled method, and return the result
      return (Prepare(myGDSMap));

   }  // End of method 'EquationSystem::Prepare()'



      /* Prepare this object to carry out its work.
       *
       * @param gdsMap     Map of GNSS data structures (GDS), indexed
       *                   by SourceID.
       *
       */
   EquationSystem& EquationSystem::Prepare( gnssDataMap& gdsMap )
   {

         // Let's start storing 'current' unknowns set from 'previous' epoch
      oldUnknowns = currentUnknowns;

         // Former currentUnknowns will belong to global unknowns set
      varUnknowns = currentUnknowns;

         // Prepare set of current unknowns and list of current equations
      currentUnknowns = prepareCurrentUnknownsAndEquations(gdsMap);

        // Backup all unknowns and delete not type indexed variable in the 'currentUnknowns'
      allUnknowns.clear();
      for(VariableSet::const_iterator it = currentUnknowns.begin();
          it != currentUnknowns.end();
          it++)
      { allUnknowns.push_back(*it); }
      
      currentUnknowns.clear();
      rejectUnknowns.clear();
      for(std::list<Variable>::const_iterator it = allUnknowns.begin();
          it != allUnknowns.end();
          it++)
      {
           if((*it).getTypeIndexed())
           {
               currentUnknowns.insert(*it);
           }
           else
           {
               rejectUnknowns.insert(*it);
           }
      }
      

         // Now, let's update the global set of unknowns with current unknowns
      varUnknowns.insert( currentUnknowns.begin(), currentUnknowns.end() );

         // Compute phiMatrix and qMatrix
      getPhiQ(gdsMap);

         // Build prefit residuals vector
      getPrefit(gdsMap);

         // Get geometry and weights matrices
      getGeometryWeights(gdsMap);

         // Handling the ConstraintSystem
      imposeConstraints();

      /*
      ofstream debugstrm("unknows.debug");
      debugstrm << StringUtils::asString(varUnknowns) << endl;
      debugstrm.close();

      debugstrm.open("phiMatrix.debug");
      debugstrm << phiMatrix << endl;
      debugstrm.close();

      debugstrm.open("qMatrix.debug");
      debugstrm << qMatrix << endl;
      debugstrm.close();

      debugstrm.open("rMatrix.debug");
      debugstrm << rMatrix << endl;
      debugstrm.close();

      debugstrm.open("measVector.debug");
      debugstrm<<measVector<<endl;
      debugstrm.close();

      debugstrm.open("hMatrix.debug");
      debugstrm<<hMatrix<<endl;
      debugstrm.close();
      */


         // Set this object as "prepared"
      isPrepared = true;

      return (*this);

   }  // End of method 'EquationSystem::Prepare()'



      // Get current sources (SourceID's) and satellites (SatID's)
   void EquationSystem::prepareCurrentSourceSat( gnssDataMap& gdsMap )
   {

         // Clear "currentSatSet" and "currentSourceSet"
      currentSatSet.clear();
      currentSourceSet.clear();

         // Insert the corresponding SatID's in "currentSatSet"
      currentSatSet = gdsMap.getSatIDSet();

         // Insert the corresponding SourceID's in "currentSourceSet"
      currentSourceSet = gdsMap.getSourceIDSet();

         // Let's return
      return;

   }  // End of method 'EquationSystem::prepareCurrentSourceSat()'



      // Prepare set of current unknowns and list of current equations
   VariableSet EquationSystem::prepareCurrentUnknownsAndEquations(
                                                         gnssDataMap& gdsMap )
   {

         // Let's clear the current equations list
      currentEquationsList.clear();

         // Let's create 'currentUnkSet' set
      VariableSet currentUnkSet;

         // Get "currentSatSet" and "currentSourceSet"
         // and stored in currentSourceSet and currentSatSet
      prepareCurrentSourceSat( gdsMap );


         // Visit each "Equation" in "equationDescriptionList"
      for( std::list<Equation>::const_iterator itEq =
                                                equationDescriptionList.begin();
           itEq != equationDescriptionList.end();
           ++itEq )
      {

            // First, get the SourceID set for this equation description
         SourceIDSet equSourceSet;

            // Check if current equation description is valid for all sources
         if ( (*itEq).getEquationSource() == Variable::allSources )
         {
            equSourceSet = currentSourceSet;
         }
         else
         {

               // Check if equation description is valid for some sources
            if ( (*itEq).getEquationSource() == Variable::someSources )
            {

                  // We have to find the intersection between equation
                  // description SourceID's and available SourceID's.
               SourceIDSet tempSourceSet( (*itEq).getSourceSet() );

                  // Declare an 'insert_iterator' to be used by
                  // 'set_intersection' algorithm (provided by STL)
               std::insert_iterator< SourceIDSet >
                                 itOut( equSourceSet, equSourceSet.begin() );

                  // Let's intersect both sets
               set_intersection( tempSourceSet.begin(), tempSourceSet.end(),
                              currentSourceSet.begin(), currentSourceSet.end(),
                              itOut );

            }
            else
            {
                  // In this case, we take directly the source into the
                  // equation source set
               equSourceSet.insert( (*itEq).getEquationSource() );
            }

         }  // End of 'if ( (*itEq).getEquationSource() == ...'
         
            // Second, get the SatID set for this equation description
         SatIDSet equSatSet = (*itEq).getSatSet();
         

            // We have the SourceID set that is applicable to this
            // equation description.

            // Now we must get the satellites visible from each
            // particular SourceID
         for( SourceIDSet::const_iterator itSource = equSourceSet.begin();
              itSource != equSourceSet.end();
              ++itSource )
         {

               // Get visible satellites from this SourceID
            SatIDSet visibleSatSet;

               // Iterate through all items in the gnssDataMap
            for( gnssDataMap::const_iterator it = gdsMap.begin();
                 it != gdsMap.end();
                 ++it )
            {

                  // Look for current SourceID
               sourceDataMap::const_iterator sdmIter(
                                             (*it).second.find( (*itSource) ) );

                  // If SourceID was found, then look for satellites
               if( sdmIter != (*it).second.end() )
               {

                     // Iterate through corresponding 'satTypeValueMap'
                  for( satTypeValueMap::const_iterator stvmIter =
                                                      (*sdmIter).second.begin();
                       stvmIter != (*sdmIter).second.end();
                       stvmIter++ )
                  {
                        // for some sat   
                     if((equSatSet.size() > 0)                           &&
                        (equSatSet.find((*stvmIter).first) == equSatSet.end()))
                     {
                        continue;
                     }

                        // Add current SatID to 'visibleSatSet'
                     visibleSatSet.insert( (*stvmIter).first );

                  }  // End of 'for( satTypeValueMap::const_iterator ...'

               }  // End of 'for( sourceDataMap::const_iterator sdmIter = ...'

            }  // End of 'for( gnssDataMap::const_iterator it = ...'

               // We have the satellites visible from this SourceID

               
               // We need a copy of current Equation object description
            Equation tempEquation( (*itEq) );

               // Remove all variables from current equation
            tempEquation.clear();

               // Update equation independent term with SourceID information
            tempEquation.header.equationSource = (*itSource);

               // Now, let's visit all Variables in this equation description
            for( VariableSet::const_iterator itVar = (*itEq).body.begin();
                 itVar != (*itEq).body.end();
                 ++itVar )
            {

                  // We will work with a copy of current Variable
               Variable var( (*itVar) );

                  // Check what type of variable we are working on

                  // If variable is source-indexed, set SourceID
               if( var.getSourceIndexed() )
               {
                  var.setSource( (*itSource) );
               }

                  // Add this variable to current equation description. Please
                  // be aware that satellite-indexed variables inside current
                  // equations will be handled later
               tempEquation.addVariable(var);

                  // If variable is not satellite-indexed, we just need to
                  // add it to "currentUnkSet
               if( !var.getSatIndexed() )
               {
                     // Insert the result in "currentUnkSet" and
                     // current equation
                  currentUnkSet.insert(var);
                  //tempEquation.addVariable(var);
               }
               else
               {
                     // If variable IS satellite-indexed, we have to visit all
                     // visible satellites (from current SourceID) and set the
                     // satellite before adding variable to "currentUnkSet
                  for( SatIDSet::const_iterator itSat = visibleSatSet.begin();
                       itSat != visibleSatSet.end();
                       ++itSat )
                  {

                        // Set satellite
                     var.setSatellite( (*itSat) );

                        // Insert the result in "currentUnkSet" and
                        // current equation
                     currentUnkSet.insert(var);
                  }

               }  // End of 'if( !var.getSatIndexed() )...'

            }  // End of 'for( VariableSet::const_iterator itVar = ...'


               // Let's generate the current equations starting from this
               // equation description. Therefore, we update equation
               // independent term with SatID information and add each instance
               // to 'currentEquationsList'.
            for( SatIDSet::const_iterator itSat = visibleSatSet.begin();
                 itSat != visibleSatSet.end();
                 ++itSat )
            {
               tempEquation.header.equationSat = (*itSat);

                  // New equation is complete: Add it to 'currentEquationsList'
               currentEquationsList.push_back( tempEquation );
            }

         }  // End of 'for( SourceIDSet::const_iterator itSource = ...'

      }  // End of 'for( std::list<Equation>::const_iterator itEq = ...'


         // Now we will take care of satellite-indexed variables inside each
         // specific "Equation" in "currentEquationsList"
      size_t eqListSize( currentEquationsList.size() );
      for( int i = 0; i < eqListSize; ++i )
      {

            // Get a copy of first equation on 'currentEquationsList'
         Equation tempEqu( currentEquationsList.front() );

            // Remove the original equation at the beginning of the list.
         currentEquationsList.pop_front();

            // Get a copy of variables inside this equation
         VariableSet varSet( tempEqu.body );

            // Clear the variables from this equation
         tempEqu.clear();

            // Visit each variable inside 'varSet', check if it is
            // satellite-indexed, and add it to equation
         for( VariableSet::iterator itVar = varSet.begin();
              itVar != varSet.end();
              ++itVar )
         {

               // Check if it is satellite-indexed
            if( !(*itVar).getSatIndexed() )
            {
                  // If not satellite-indexed, just add it back
               tempEqu.addVariable( (*itVar) );
            }
            else
            {
               // If 'itVar' is satellite-indexed, let's index a copy of it
               // and add it to equation
               Variable var( (*itVar) );
               var.setSatellite( tempEqu.header.equationSat );

               tempEqu.addVariable( var );
            }

         }  // End of 'for( VariableSet::iterator itVar = varSet.begin(); ...'

            // Our equation is ready, let's add it to the end of the list
         currentEquationsList.push_back( tempEqu );

      }  // End of 'for( int i = 0; i < eqListSize; ++i ) ...'


         // Return set of current unknowns
      return currentUnkSet;

   }  // End of method 'EquationSystem::prepareCurrentUnknownsAndEquations()'



      // Compute PhiMatrix
   void EquationSystem::getPhiQ( const gnssDataMap& gdsMap )
   {

      const size_t numVar( varUnknowns.size() );

         // Resize phiMatrix and qMatrix
      phiMatrix.resize( numVar, numVar, 0.0);
      qMatrix.resize( numVar, numVar, 0.0);

         // Set a counter
      int i(0);

         // Visit each "Variable" inside "varUnknowns"
      for( VariableSet::const_iterator itVar  = varUnknowns.begin();
           itVar != varUnknowns.end();
           ++itVar )
      {

            // Check if (*itVar) is inside 'currentUnknowns'
         if( currentUnknowns.find( (*itVar) ) != currentUnknowns.end() )
         {

               // Get a 'gnssRinex' data structure
            gnssRinex gRin( gdsMap.getGnssRinex( (*itVar).getSource() ) );

               // Prepare variable's stochastic model
            (*itVar).getModel()->Prepare( (*itVar).getSatellite(),
                                          gRin );

               // Now, check if this is an 'old' variable
            if( oldUnknowns.find( (*itVar) ) != oldUnknowns.end() )
            {
                  // This variable is 'old'; compute its phi and q values
               phiMatrix(i,i) = (*itVar).getModel()->getPhi();
               qMatrix(i,i)   = (*itVar).getModel()->getQ();
            }
            else
            {
                  // This variable is 'new', so let's use its initial variance
                  // instead of its stochastic model
               phiMatrix(i,i) = 0.0;
               qMatrix(i,i)   = (*itVar).getInitialVariance();
            }

         }
         else
         {
               // If (*itVar) is NOT inside 'currentUnknowns', then apply it
               // a white noise stochastic model to decorrelate it
            phiMatrix(i,i) = whiteNoiseModel.getPhi();
            qMatrix(i,i)   = whiteNoiseModel.getQ();
         }

            // Increment counter
         ++i;
      }


      return;

   }  // End of method 'EquationSystem::getPhiQ()'



      // Compute prefit residuals vector
   void EquationSystem::getPrefit( gnssDataMap& gdsMap )
   {

         // Declare temporal storage for values
      std::vector<double> tempPrefit;

         // Visit each Equation in "currentEquationsList"
      for( std::list<Equation>::const_iterator itEq =
                                                   currentEquationsList.begin();
           itEq != currentEquationsList.end();
           ++itEq )
      {

            // Store SourceID, SatID and TypeID of current equation
         tempPrefit.push_back( gdsMap.getValue( (*itEq).header.equationSource,
                                          (*itEq).header.equationSat,
                                          (*itEq).header.indTerm.getType() ) );


      }  // End of 'for( std::list<Equation>::const_iterator itEq = ...'

         // Then, finally get prefit residuals into appropriate gpstk::Vector
      measVector = tempPrefit;

      return;

   }  // End of method 'EquationSystem::getPrefit()'



      // Compute hMatrix and rMatrix
   void EquationSystem::getGeometryWeights( gnssDataMap& gdsMap )
   {

         // Resize hMatrix and rMatrix
      hMatrix.resize( measVector.size(), varUnknowns.size(), 0.0);
      rMatrix.resize( measVector.size(),  measVector.size(), 0.0);

         // Let's work with the first element of the data structure
      gnssDataMap gds2( gdsMap.frontEpoch() );

         // Let's fill weights and geometry matrices
      int row(0);                      // Declare a counter for row number
      for( std::list<Equation>::const_iterator itRow =
                                                   currentEquationsList.begin();
           itRow != currentEquationsList.end();
           ++itRow )
      {

            // Create temporal GDS objects
         SourceID source( (*itRow).header.equationSource );
         SatID sat( (*itRow).header.equationSat );

            // Get a TypeIDSet with all the data types present in current GDS
            // Declare an appropriate object
         TypeIDSet typeSet;

            // We need a flag
         bool found( false );

            // Iterate through data structure
         for( gnssDataMap::const_iterator itGDS = gds2.begin();
              itGDS != gds2.end() && !found;
              ++itGDS )
         {
               // Look for source
            sourceDataMap::const_iterator itSDM = (*itGDS).second.find(source);
            if( itSDM != (*itGDS).second.end() )
            {
                  // Get the types
               typeSet = (*itSDM).second.getTypeID();
               found = true;
            }
         }


            // First, fill weights matrix
            // Check if current GDS has weight info. If you don't want those
            // weights to get into equations, please don't put them in GDS
         if( typeSet.find(TypeID::weight) != typeSet.end() )
         {
               // Weights matrix = Equation weight * observation weight
            rMatrix(row,row) = (*itRow).header.constWeight
                               * gds2.getValue(source, sat, TypeID::weight);
         }
         else
         {
               // Weights matrix = Equation weight
            rMatrix(row,row) = (*itRow).header.constWeight;
         }

            // Second, fill geometry matrix: Look for equation coefficients
         int col(0);                   // Declare a counter for column number
         for( VariableSet::const_iterator itCol = varUnknowns.begin();
              itCol != varUnknowns.end();
              ++itCol )
         {

               // Check if unknown is in current equation and also is marked
               // as a current unknown
            if( (*itRow).body.find( (*itCol) ) != (*itRow).body.end() &&
                currentUnknowns.find( (*itCol) ) != currentUnknowns.end() )
            {

                  // Check if '(*itCol)' unknown variable enforces a specific
                  // coefficient
               if( (*itCol).isDefaultForced() )
               {
                     // Use default coefficient
                  hMatrix(row,col) = (*itCol).getDefaultCoefficient();
               }
               else
               {
                     // Look the coefficient in provided data

                     // Get type of current varUnknown
                  TypeID type( (*itCol).getType() );

                     // Check if this type has an entry in current GDS type set
                  if( typeSet.find(type) != typeSet.end() )
                  {
                        // If type was found, insert value into hMatrix
                     hMatrix(row,col) = gds2.getValue(source, sat, type);
                  }
                  else
                  {
                        // If value for current type is not in gdsMap, then
                        // insert default coefficient for this variable
                     hMatrix(row,col) = (*itCol).getDefaultCoefficient();
                  }

               }  // End of 'if( (*itCol).isDefaultForced() ) ...'

            }  // End of 'if( (*itRow).body.find( (*itCol) ) != ...'

               // Increment column counter
            ++col;

         }  // End of 'for( VariableSet::const_iterator itCol = ...'

            // Handle type index variable
         for( VariableSet::const_iterator itCol = (*itRow).body.begin();
             itCol != (*itRow).body.end();
             ++itCol )
         {

            VariableSet::const_iterator itr = rejectUnknowns.find( (*itCol) );
            if( itr == rejectUnknowns.end() || (*itr).getTypeIndexed()) continue;

            Variable var(*itr);

            col = 0;            
            for( VariableSet::const_iterator it = varUnknowns.begin(); it != varUnknowns.end(); it++)
            {
                if(((*itCol).getType() == (*it).getType())                  &&
                   ((*itCol).getModel() == (*it).getModel())                &&
                   ((*itCol).getSourceIndexed() == (*it).getSourceIndexed())&&
                   ((*itCol).getSatIndexed() == (*it).getSatIndexed())      &&
                   ((*itCol).getSource() == (*it).getSource())              &&
                   ((*itCol).getSatellite() == (*it).getSatellite())        
                   )
                {
                    break;
                }

                col++;    
            }

            
            // Check if '(*itCol)' unknown variable enforces a specific
            // coefficient
            if( (*itCol).isDefaultForced() )
            {
                   // Use default coefficient
                hMatrix(row,col) = (*itCol).getDefaultCoefficient();
            }
            else
            {
                // Look the coefficient in provided data

                   // Get type of current varUnknown
                TypeID type( (*itCol).getType() );

                   // Check if this type has an entry in current GDS type set
                if( typeSet.find(type) != typeSet.end() )
                {
                       // If type was found, insert value into hMatrix
                    hMatrix(row,col) = gds2.getValue(source, sat, type);
                }
                else
                {
                      // If value for current type is not in gdsMap, then
                      // insert default coefficient for this variable
                    hMatrix(row,col) = (*itCol).getDefaultCoefficient();
                }

            }  // End of 'if( (*itCol).isDefaultForced() ) ...'
            
         }

            // Increment row number
         ++row;

      }  // End of 'std::list<Equation>::const_iterator itRow = ...'


      return;

   }  // End of method 'EquationSystem::getGeometryWeights()'


      // Impose the constraints system to the equation system
      // the prefit residuals vector, hMatrix and rMatrix will be appended.
   void EquationSystem::imposeConstraints()
   {
      if(!equationConstraints.hasConstraints()) return;

      ConstraintList destList;

      ConstraintList tempList = equationConstraints.getConstraintList();
      for(ConstraintList::iterator it = tempList.begin();
         it != tempList.end();
         ++it )
      {
         VariableDataMap dataMapOk;

         bool validConstraint(true);

         try
         {
            VariableDataMap dataMap = it->body;
            for(VariableDataMap::iterator itv = dataMap.begin();
               itv != dataMap.end();
               ++itv )
            {
               bool isFound(false);

               VariableSet::iterator itv2 = varUnknowns.find(itv->first);
               if(itv2!=varUnknowns.end())
               {
                  isFound = true;
                  dataMapOk[*itv2] = dataMap[itv->first];
               }
               else
               {
                  for(itv2 = varUnknowns.begin();
                     itv2 != varUnknowns.end();
                     ++itv2 )
                  {
                     if( (itv->first.getType() == itv2->getType()) &&
                        (itv->first.getSource() == itv2->getSource()) &&
                        (itv->first.getSatellite() == itv2->getSatellite()) )
                     {
                        isFound = true;
                        dataMapOk[*itv2] = dataMap[itv->first];
                        break;
                     }
                  }
               }

               if( !isFound ) validConstraint = false;
            }
         }
         catch(...)
         {
            validConstraint = false;
         }

         if(validConstraint)
         {
            destList.push_back(Constraint(it->header,dataMapOk));
         }
         else
         {
            // we discard all constraints
            return;
         }

      }
         // Update the equation system
      equationConstraints.setConstraintList(destList);


         // Now, we can append the matrix(prefit design and weight)
      try
      {
         Vector<double> meas;
         Matrix<double> design;
         Matrix<double> cov;

         equationConstraints.constraintMatrix(varUnknowns,
            meas, design, cov);

         const int oldSize = measVector.size();
         const int newSize = oldSize + meas.size();
         const int colSize = hMatrix.cols();

         Vector<double> tempPrefit(newSize,0.0);
         Matrix<double> tempGeometry(newSize,colSize,0.0);
         Matrix<double> tempWeight(newSize,newSize,0.0);

         for(int i=0; i< newSize; i++)
         {
               // prefit
            if(i<oldSize) tempPrefit(i) = measVector(i);
            else tempPrefit(i) = meas(i-oldSize);

               // geometry
            for(int j=0;j<colSize;j++)
            {
               if(i<oldSize) tempGeometry(i,j) = hMatrix(i,j);
               else tempGeometry(i,j) = design(i-oldSize,j);
            }

               // weight
            if(i<oldSize) tempWeight(i,i) = rMatrix(i,i);
            else tempWeight(i,i) = 1.0/cov(i-oldSize,i-oldSize);

         }
            // Update these matrix
         measVector = tempPrefit;
         hMatrix = tempGeometry;
         rMatrix = tempWeight;
      }
      catch(...)
      {
         return;
      }

   }  // End of method 'EquationSystem::imposeConstraints()'


      /* Return the TOTAL number of variables being processed.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   int EquationSystem::getTotalNumVariables() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return varUnknowns.size();

   }  // End of method 'EquationSystem::getTotalNumVariables()'



      /* Return the set containing all variables being processed.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   VariableSet EquationSystem::getVarUnknowns() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return varUnknowns;

   }  // End of method 'EquationSystem::getVarUnknowns()'



      /* Return the CURRENT number of variables, given the current equation
       * system definition and the GDS's involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   int EquationSystem::getCurrentNumVariables() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return currentUnknowns.size();

   }  // End of method 'EquationSystem::getCurrentNumVariables()'



      /* Return the set containing variables being currently processed.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   VariableSet EquationSystem::getCurrentUnknowns() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return currentUnknowns;

   }  // End of method 'EquationSystem::getCurrentUnknowns()'



      /* Return the CURRENT number of sources, given the current equation
       * system definition and the GDS's involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   int EquationSystem::getCurrentNumSources() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return currentSourceSet.size();

   }  // End of method 'EquationSystem::getCurrentNumSources()'



      /* Return the set containing sources being currently processed.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   SourceIDSet EquationSystem::getCurrentSources() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return currentSourceSet;

   }  // End of method 'EquationSystem::getCurrentSources()'



      /* Return the CURRENT number of satellites, given the current equation
       * system definition and the GDS's involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   int EquationSystem::getCurrentNumSats() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return currentSatSet.size();

   }  // End of method 'EquationSystem::getCurrentNumSats()'



      /* Return the set containing satellites being currently processed.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   SatIDSet EquationSystem::getCurrentSats() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return currentSatSet;

   }  // End of method 'EquationSystem::getCurrentSats()'



      /* Get prefit residuals GPSTk Vector, given the current equation
       *  system definition and the GDS' involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   Vector<double> EquationSystem::getPrefitsVector() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return measVector;

   }  // End of method 'EquationSystem::getPrefitsVector()'



      /* Get geometry matrix, given the current equation system definition
       *  and the GDS' involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   Matrix<double> EquationSystem::getGeometryMatrix() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return hMatrix;

   }  // End of method 'EquationSystem::getGeometryMatrix()'



      /* Get weights matrix, given the current equation system definition
       *  and the GDS' involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   Matrix<double> EquationSystem::getWeightsMatrix() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return rMatrix;

   }  // End of method 'EquationSystem::getWeightsMatrix()'


      /* Get the State Transition Matrix (PhiMatrix), given the current
       * equation system definition and the GDS' involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   Matrix<double> EquationSystem::getPhiMatrix() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return phiMatrix;

   }  // End of method 'EquationSystem::getPhiMatrix()'



      /* Get the Process Noise Covariance Matrix (QMatrix), given the
       * current equation system definition and the GDS' involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   Matrix<double> EquationSystem::getQMatrix() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return qMatrix;

   }  // End of method 'EquationSystem::getQMatrix()'



}  // End of namespace gpstk
