#pragma ident "$Id$"

/**
 * @file Equation.hpp
 * GNSS Data Structure to define and handle 'descriptions' of GNSS equations.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009
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


         /** In case this variable is associated to SOME specific
          *  sources ("Variable::someSources" in "equationSource"),
          *  then the corresponding SourceID set is stored here.
          */
      std::set<SourceID> equationSourceSet;


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
           indTerm(indep), constWeight(1.0) {};


         /// Assignment operator
      virtual equationHeader& operator=(const equationHeader& right);


         /** Assignment operator from a Variable
          *
          * @param indep      Variable representing the independent term.
          */
      virtual equationHeader& operator=(const Variable& indep)
      { indTerm = indep; return (*this); };


         /// Destructor
      virtual ~equationHeader() {};


   }; // End of struct 'equationHeader'



      /** GNSS Data Structure to define and handle 'descriptions' of GNSS
       *  equations.
       */
   struct Equation : gnssData<equationHeader, VariableSet>
   {

         /// Default constructor.
      Equation();


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
          * @param indep     Variable object describing the independent term.
          */
      Equation( const Variable& indep );


         /** Common constructor. It defines an Equation from its independent
          *  term. You must later use other methods to input the variables.
          *
          * @param var     TypeID object describing the independent term.
          */
      Equation( const TypeID& type );


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


         /// Get the value of the constant weight associated to this equation
      virtual double getWeight() const
      { return header.constWeight; };


         /** Set the value of the constant weight associated to this equation
          *
          * @param cweight    Value of constant weight.
          */
      virtual Equation& setWeight(const double& cweight)
      { header.constWeight = cweight; return (*this); };


         /** Add a variable (unknown) to this Equation
          *
          * @param var     Variable object to be added to the unknowns.
          */
      virtual Equation& addVariable(const Variable& var)
      { body.insert(var); return (*this); };


         /** Add a variable (unknown) to this Equation
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
      virtual Equation& addVariable( const TypeID& type,
                                     StochasticModel* pModel   =
                                                      &Variable::defaultModel,
                                     bool sourceIndexed        = true,
                                     bool satIndexed           = false,
                                     double variance           = 4.0e14,
                                     double coef               = 1.0 );


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


         /// Get equation SourceID.
      SourceID getEquationSource() const
      { return header.equationSource; };


         /** Get SourceID set. This is only meaningful if "equationSource" in
          *  header is set to "Variable::someSources".
          */
      std::set<SourceID> getSourceSet() const
      { return header.equationSourceSet; };


         /** Add a source to SourceID set. This is only meaningful if
          * "equationSource" in header is set to "Variable::someSources".
          */
      Equation& addSource2Set( const SourceID& source )
      { header.equationSourceSet.insert(source); return (*this); };


         /** Clear SourceID set. This is only meaningful if "equationSource"
          *  in header is set to "Variable::someSources".
          */
      Equation& clearSourceSet()
      { header.equationSourceSet.clear(); return (*this); };


         /// This ordering is somewhat arbitrary, but is required to be able
         /// to use an Equation as an index to a std::map, or as part of a
         /// std::set.
      virtual bool operator<(const Equation& right) const
      { return (header.indTerm < right.header.indTerm); };


         /// Destructor
      virtual ~Equation() {};


   }; // End of struct 'Equation'

      //@}

}  // End of namespace gpstk
#endif   // EQUATION_HPP
