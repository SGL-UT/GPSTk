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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009, 2011
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
 * @file Equation.cpp
 * GNSS Data Structure to define and handle 'descriptions' of GNSS equations.
 */

#include "Equation.hpp"


namespace gpstk
{


      // Assignment operator for equationHeader
   equationHeader& equationHeader::operator=(const equationHeader& right)
   {

      if ( this == &right )
      {
         return (*this);
      }

      equationSource = right.equationSource;
      equationSat = right.equationSat;
      equationSourceSet = right.equationSourceSet;
      equationSatSet = right.equationSatSet;
      indTerm = right.indTerm;
      constWeight = right.constWeight;

      return (*this);

   }  // End of 'equationHeader::operator='



      // Default constructor.
   Equation::Equation()
   {

      header.equationSource = Variable::allSources;
      header.equationSat = Variable::allSats;
      header.constWeight = 1.0;

   }  // End of 'Equation::Equation()'



      /* Common constructor. It defines an Equation from its independent
       * term. You must later use other methods to input the variables.
       *
       * @param indep     Variable object describing the independent term.
       */
   Equation::Equation( const Variable& indep )
   {

      header.equationSource = Variable::allSources;
      header.equationSat = Variable::allSats;
      header.indTerm = indep;
      header.constWeight = 1.0;

   }  // End of 'Equation::Equation()'



      /* Common constructor. It defines an Equation from its independent
       * term. You must later use other methods to input the variables.
       *
       * @param var     TypeID object describing the independent term.
       */
   Equation::Equation( const TypeID& type )
   {

      header.equationSource = Variable::allSources;
      header.equationSat = Variable::allSats;
      header.indTerm.setType(type);
      header.constWeight = 1.0;

   }  // End of 'Equation::Equation()'



      /* Common constructor. It takes a simple gnssEquationDefinition
       * object and creates an Equation.
       *
       * @param gnssEq  gnssEquationDefinition object.
       *
       */
   Equation::Equation( const gnssEquationDefinition& gnssEq )
   {

      header.equationSource = Variable::allSources;
      header.equationSat = Variable::allSats;
      header.constWeight = 1.0;

         // Set the properties of the independent term. Defaults are OK except
         // for type
      header.indTerm.setType(gnssEq.header);

         // Now, get the types of the variables
      for( TypeIDSet::const_iterator pos = gnssEq.body.begin();
           pos != gnssEq.body.end();
           ++pos )
      {

            // Create a default Variable object with this type
         Variable var(*pos);

            // Insert this variable in this Equation's 'body' field
         body.insert(var);

      }  // End of 'for( TypeIDSet::const_iterator pos = gnssEq.body.begin();'

   }  // End of 'Equation::Equation()'



      /* Add a variable (unknown) to this Equation
       *
       * @param type             TypeID of variable.
       * @param pModel           Pointer to StochasticModel associated with
       *                         this variable. By default, it is a white
       *                         noise model.
       * @param sourceIndexed    Whether this variable is SourceID-indexed
       *                         or not. By default, it IS SourceID-indexed.
       * @param satIndexed       Whether this variable is SatID-indexed
       *                         or not. By default, it is NOT.
       * @param variance         Initial variance assigned to this variable.
       * @param coef             Default coefficient assigned.
       */
   Equation& Equation::addVariable( const TypeID& type,
                                    StochasticModel* pModel,
                                    bool sourceIndexed,
                                    bool satIndexed,
                                    double variance,
                                    double coef )
   {

      Variable var( type,
                    pModel,
                    sourceIndexed,
                    satIndexed,
                    variance,
                    coef );

      return (addVariable(var));

   }  // End of method 'Equation::addVariable()'



}  // End of namespace gpstk
