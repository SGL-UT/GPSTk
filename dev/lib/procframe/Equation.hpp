#pragma ident "$Id$"

/**
 * @file Equation.hpp
 * GNSS Data Structure to define and handle GNSS equations.
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


      // Handy type definition

      /// Set containing Variable objects.
   typedef std::set<Variable> VariableSet;


      /// Defines a header containing basic equation data
   struct equationHeader
   {

         /// Source this equation is related to
      SourceID equationSource;


         /// Satellite this equation is related to
      SatID equationSat;


         /// Independent term
      Variable indTerm;


         /** Constant weight associated to this equation. This is a relative
          *  value that compares with the other Equations. It is 1.0 by
          *  default.
          */
      double constWeight;


         /// Default constructor
      equationHeader()
         : equationSource(Variable::allSources), equationSat(Variable::allSats),
           constWeight(1.0) {};


         /** Explicit constructor
          *
          * @param source     Source this equation is related to.
          * @param sat        Satellite this equation is related to.
          * @param indep      Variable representing the independent term.
          * @param cweight    Constant weight associated to this equation.
          */
      equationHeader( const SourceID& source,
                      const SatID& sat,
                      const Variable indep,
                      const double& cweight )
         : equationSource(source), equationSat(sat), indTerm(indep),
           constWeight(cweight) {};


         /** Explicit constructor from a Variable
          *
          * @param indep      Variable representing the independent term.
          */
      equationHeader(const Variable& indep)
         : equationSource(Variable::allSources), equationSat(Variable::allSats),
           indTerm(indep) {};


         /// Get the value of the constant weight associated to this equation
      virtual double getWeight() const
      { return constWeight; };


         /** Set the value of the constant weight associated to this equation
          *
          * @param cweight    Value of constant weight.
          */
      virtual equationHeader& setWeight(const double& cweight)
      { constWeight = cweight; return (*this); };


         /// Assignment operator
      virtual equationHeader& operator=(const equationHeader& right);


         /** Assignment operator from a Variable
          *
          * @param indep      Variable representing the independent term.
          */
      virtual equationHeader& operator=(const Variable& right)
      { indTerm = right; return (*this); };


         /// Destructor
      virtual ~equationHeader() {};


   }; // End of struct 'equationHeader'



      /// GNSS Data Structure to define and handle GNSS equations.
   struct Equation : gnssData<equationHeader, VariableSet>
   {

         /// Default constructor
      Equation() {};


         /** Common constructor. It defines an Equation from its header. You
          *  must later use other methods to input the variables.
          *
          * @param head     Data structure describing the Equation header.
          */
      Equation( const equationHeader& head )
      { header = head; };


         /** Common constructor. It defines an Equation from its independent
          *  term. You must later use other methods to input the variables.
          *
          * @param var     Variable object describing the independent term.
          */
      Equation( const Variable& var )
      { header.indTerm = var; };


         /** Common constructor. It defines an Equation from its independent
          *  term. You must later use other methods to input the variables.
          *
          * @param var     TypeID object describing the independent term.
          */
      Equation( const TypeID& type )
      { header.indTerm.setType(type); };


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
      { return header.indTerm; };


         /** Set the independent term of this Equation.
          *
          * @param var     Variable object describing the independent term.
          */
      virtual Equation& setIndependentTerm(const Variable& var)
      { header = var; return (*this); };


         /** Add a variable (unknown) to this Equation
          *
          * @param var     Variable object to be added to the unknowns.
          */
      virtual Equation& addVariable(const Variable& var)
      { body.insert(var); return (*this); };


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


         /** Remove a variable (unknown) from this Equation
          *
          * @param var     Variable object to be romoved from the unknowns.
          */
      virtual Equation& removeVariable(const Variable& var)
      { body.erase(var); return (*this); };


         /** Remove ALL variables (unknowns) from this Equation.
          *
          * @warning This method does NOT clear the Equation's independent
          *          term. You MUST take care of it yourself (use method
          *          'setIndependentTerm', for instance).
          */
      virtual Equation& clear()
      { body.clear(); return (*this); };


         /// Destructor
      virtual ~Equation() {};


   }; // End of struct 'Equation'

      //@}

}  // End of namespace gpstk
#endif   // EQUATION_HPP
