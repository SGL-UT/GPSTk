#pragma ident "$Id: $"

/**
 * @file Equation.cpp
 * Class to define and handle GNSS equations.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008
//
//============================================================================


#include "Equation.hpp"


namespace gpstk
{


      /* Common constructor. It takes a simple gnssEquationDefinition
       *  object and creates an Equation.
       *
       * @param gnssEq  gnssEquationDefinition object.
       *
       */
   Equation::Equation( const gnssEquationDefinition& gnssEq )
   {
         // Set the properties of the independent term. Defaults are OK except
         // for type
      indTerm.setType(gnssEq.header);

         // Now, get the types of the variables
      TypeIDSet::const_iterator pos;
      for( pos = gnssEq.body.begin(); pos != gnssEq.body.end(); ++pos )
      {
            // Create a default Variable object with this type
         Variable var(*pos);
            // Insert this variable in this Equation's 'variables' field
         variables.insert(var);
      }

   }


      /* Add a variable (unknown) to this Equation
       *
       * @param type        TypeID of variable.
       * @param pModel      Pointer to StochasticModel associated with
       *                    this variable. By default, it is a white
       *                    noise model.
       * @param sourceSpecific Whether this variable is source-specific
       *                    or not. By default, it IS source-specific.
       * @param satSpecific Whether this variable is satellite-specific
       *                    or not. By default, it is NOT.
       */
   Equation& Equation::addVariable( const TypeID& type,
                                    StochasticModel* pModel,
                                    bool sourceSpecific,
                                    bool satSpecific )
   {
      Variable var(type, pModel, sourceSpecific, satSpecific);

      return (addVariable(var));
   }


      /* Add a Variable to this Equation corresponding to an specific
       *  data source and satellite
       *
       * @param type        TypeID of variable.
       * @param pModel      Pointer to StochasticModel associated with
       *                    this variable. By default, it is a white
       *                    noise model.
       * @param source      Data source this variable belongs to.
       * @param satellite   Satellite this variable belongs to.
       */
   Equation& Equation::addVariable( const TypeID& type,
                                    StochasticModel* pModel,
                                    const SourceID& source,
                                    const SatID& satellite )
   {
      Variable var(type, pModel, source, satellite);

      return (addVariable(var));
   }


      /* Add a Variable to this equation corresponding to an specific
       *  data source
       *
       * @param type        TypeID of variable.
       * @param pModel      Pointer to StochasticModel associated with
       *                    this variable. By default, it is a white
       *                    noise model.
       * @param source      Data source this variable belongs to.
       */
   Equation& Equation::addVariable( const TypeID& type,
                                    StochasticModel* pModel,
                                   const SourceID& source )
   {
      Variable var(type, pModel, source);

      return (addVariable(var));
   }


      /* Add a Variable to this Equation corresponding to an specific
       *  satellite
       *
       * @param type        TypeID of variable.
       * @param pModel      Pointer to StochasticModel associated with
       *                    this variable. By default, it is a white
       *                    noise model.
       * @param satellite   Satellite this variable belongs to.
       */
   Equation& Equation::addVariable( const TypeID& type,
                                    StochasticModel* pModel,
                                    const SatID& satellite )
   {
      Variable var(type, pModel, satellite);

      return (addVariable(var));
   }


      /* Add a new equation to be managed. Any given equation is identified
       * by its the TypeID of its independent term (measurement type) AND
       * the specific SourceID it applies to.
       *
       * @param type    TypeID of equation independent term (measurement
       *                type).
       * @param source  Specific SourceID this equation applies to.
       *
       */
/*   Equation& Equation::addEquation( const TypeID& type,
                                                  const SourceID& source )
   {
         // Look for equation definition corresponding to "source", and insert
         // "type" in its corresponding TypeIDSet.
      equationDefinitions[source].insert(type);

      return (*this);
   }
*/

      /* Add a new equation to be managed. In this case the equation will
       * be identified just by its the TypeID of its independent term
       * (measurement type) because it will apply to ALL available
       * SourceID's.
       *
       * @param type    TypeID of equation independent term (measurement
       *                type).
       *
       */
/*   Equation& Equation::addEquation( const TypeID& type )
   {
         // Look for equation definition corresponding to "allSources", and
         // insert "type" in its corresponding TypeIDSet.
      equationDefinitions[allSources].insert(type);

      return (*this);
   }
*/

      /* Remove an equation being managed. Any given equation is identified
       * by its the TypeID of its independent term (measurement type) AND
       * the specific SourceID it applies to.
       *
       * @param type    TypeID of equation independent term (measurement
       *                type).
       * @param source  Specific SourceID this equation applies to.
       *
       */
/*   Equation& Equation::removeEquation( const TypeID& type,
                                                     const SourceID& source )
   {
         // Look for equation definition corresponding to "source", and erase
         // "type" from its corresponding TypeIDSet.
      equationDefinitions[source].erase(type);

      return (*this);
   }
*/

      /* Remove an equation being managed. In this case the equation is
       * identified just by its the TypeID of its independent term
       * (measurement type) because it will be removed from ALL
       * available SourceID's.
       *
       * @param type    TypeID of equation independent term (measurement
       *                type).
       *
       */
/*   Equation& Equation::removeEquation( const TypeID& type )
   {
         // Look for equation definitions corresponding to all available
         // sources, and erase "type" from corresponding TypeIDSet.
      sourceTypeIDSetMap::iterator it;
      for(  it = equationDefinitions.begin();
            it!= equationDefinitions.end(); 
            it++ )
      {
         (*it).second.erase(type);
      }

      return (*this);
   }
*/




} // end namespace gpstk
