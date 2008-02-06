#pragma ident "$Id: $"

/**
 * @file Equation.hpp
 * Class to define and handle GNSS equations.
 */

#ifndef EQUATION_HPP
#define EQUATION_HPP

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



#include "DataStructures.hpp"
#include "StochasticModel.hpp"
#include "Variable.hpp"


namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /// Class to define and handle GNSS equations.
   class Equation
   {
   public:

         /// Default constructor
      Equation() {};


         /** Common constructor. It defines an Equation from its independent
          *  term. You must later use other methods to input the variables.
          *
          * @param var     Variable object describing the independent term.
          */
      Equation( const Variable& var )
      { indTerm = var; };


         /** Common constructor. It defines an Equation from its independent
          *  term. You must later use other methods to input the variables.
          *
          * @param var     TypeID object describing the independent term.
          */
      Equation( const TypeID& type )
      { indTerm.setType(type); };


         /** Common constructor. It takes a simple gnssEquationDefinition
          *  object and creates a  more complex Equation object.
          *
          * @param gnssEq  gnssEquationDefinition object.
          *
          * A gnssEquationDefinition object defines equations as a simple list
          * of TypeID's: The independent term (usually the prefit residual)
          * type in the header, and the variables' types in the body (or
          * 'unknowns').
          * 
          * The resulting Equation object will honor this simple structure,
          * assigning white noise models to all variables, as well as declaring
          * them source-specific and satellite-independent (or 'UN-specific').
          *
          * The former is suitable for simple GNSS data processing strategies
          * like the SPS C1-based positioning, where the variables are
          * TypeID::dx, TypeID::dy, TypeID::dz and TypeID::cdt.
          *
          */
      Equation( const gnssEquationDefinition& gnssEq );


         /// Return the independent term of this equation
      virtual Variable getIndependentTerm() const
      { return indTerm; };


         /** Set the independent term of this Equation.
          *
          * @param var     Variable object describing the independent term.
          */
      virtual Equation& setIndependentTerm(const Variable& var)
      { indTerm = var; return (*this); };


         /** Add a variable (unknown) to this Equation
          *
          * @param var     Variable object to be added to the unknowns.
          */
      virtual Equation& addVariable(const Variable& var)
      { variables.insert(var); return (*this); };


         /** Add a variable (unknown) to this Equation
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
      virtual Equation& addVariable( const TypeID& type,
                                     StochasticModel* pModel = NULL,
                                     bool sourceSpecific = true,
                                     bool satSpecific = false );


         /** Add a Variable to this Equation corresponding to an specific
          *  data source and satellite
          *
          * @param type        TypeID of variable.
          * @param pModel      Pointer to StochasticModel associated with
          *                    this variable. By default, it is a white
          *                    noise model.
          * @param source      Data source this variable belongs to.
          * @param satellite   Satellite this variable belongs to.
          */
      virtual Equation& addVariable( const TypeID& type,
                                     StochasticModel* pModel,
                                     const SourceID& source,
                                     const SatID& satellite );


         /** Add a Variable to this equation corresponding to an specific
          *  data source
          *
          * @param type        TypeID of variable.
          * @param pModel      Pointer to StochasticModel associated with
          *                    this variable. By default, it is a white
          *                    noise model.
          * @param source      Data source this variable belongs to.
          */
      virtual Equation& addVariable( const TypeID& type,
                                     StochasticModel* pModel,
                                     const SourceID& source );


         /** Add a Variable to this Equation corresponding to an specific
          *  satellite
          *
          * @param type        TypeID of variable.
          * @param pModel      Pointer to StochasticModel associated with
          *                    this variable. By default, it is a white
          *                    noise model.
          * @param satellite   Satellite this variable belongs to.
          */
      virtual Equation& addVariable( const TypeID& type,
                                     StochasticModel* pModel,
                                     const SatID& satellite );


         /** Remove a variable (unknown) to this Equation
          *
          * @param var     Variable object to be romoved from the unknowns.
          */
      virtual Equation& removeVariable(const Variable& var)
      { variables.erase(var); return (*this); };


         /// Destructor
      virtual ~Equation() {};


   private:


         // Handy type definitions

         /// Set containing Variable objects.
      typedef std::set<Variable> VariableSet;



         /// Independent term
      Variable indTerm;


         /// Independent term
      VariableSet variables;


   };


      //@}
   
}
#endif // EQUATION_HPP
