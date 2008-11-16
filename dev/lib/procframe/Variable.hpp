#pragma ident "$Id$"

/**
 * @file Variable.hpp
 * Class to define and handle GNSS variables.
 */

#ifndef VARIABLE_HPP
#define VARIABLE_HPP

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



namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /// Class to define and handle GNSS variables.
   class Variable
   {
   public:

         /// Default constructor for Variable
      Variable();


         /** Common constructor for Variable.
          *  By default, it is indexed by SourceID.
          *
          * @param type             TypeID of variable.
          * @param pModel           Pointer to StochasticModel associated with
          *                         this variable. By default, it is a white
          *                         noise model.
          * @param sourceIndexed    Whether this variable is SourceID-indexed
          *                         or not. By default, it IS SourceID-indexed.
          * @param satIndexed       Whether this variable is SatID-indexed
          *                         or not. By default, it is NOT.
          */
      Variable( const TypeID& type,
                StochasticModel* pModel = NULL,
                bool sourceIndexed = true,
                bool satIndexed    = false );


         /** Constructor for a Variable corresponding to a specific
          *  data source and satellite
          *
          * @param type        TypeID of variable.
          * @param pModel      Pointer to StochasticModel associated with
          *                    this variable. By default, it is a white
          *                    noise model.
          * @param source      Data source this variable belongs to.
          * @param satellite   Satellite this variable belongs to.
          */
      Variable( const TypeID& type,
                StochasticModel* pModel,
                const SourceID& source,
                const SatID& satellite );


         /** Constructor for a Variable corresponding to a specific
          *  data source
          *
          * @param type        TypeID of variable.
          * @param pModel      Pointer to StochasticModel associated with
          *                    this variable. By default, it is a white
          *                    noise model.
          * @param source      Data source this variable belongs to.
          */
      Variable( const TypeID& type,
                StochasticModel* pModel,
                const SourceID& source );


         /** Constructor for a Variable corresponding to a specific
          *  satellite
          *
          * @param type        TypeID of variable.
          * @param pModel      Pointer to StochasticModel associated with
          *                    this variable. By default, it is a white
          *                    noise model.
          * @param satellite   Satellite this variable belongs to.
          */
      Variable( const TypeID& type,
                StochasticModel* pModel,
                const SatID& satellite );


         /// Get variable type
      TypeID getType() const
      { return varType; };


         /** Set variable type
          *
          * @param type        New TypeID of variable.
          */
      Variable& setType(const TypeID& type)
      { varType = type; return (*this); };


         /// Get variable model pointer
      StochasticModel* getModel() const
      { return pVarModel; };


         /** Set variable model
          *
          * @param pModel      Pointer to StochasticModel associated with
          *                    this variable. By default, it is a white
          *                    noise model.
          */
      Variable& setModel(StochasticModel* pModel)
      { pVarModel = pModel; return (*this); };


         /// Get if this variable is SourceID-indexed
      bool getSourceIndexed() const
      { return isSourceIndexed; };


         /** Set if this variable is SourceID-indexed.
          *
          * @param sourceIndexed    Whether this variable is SourceID-indexed
          *                         or not. By default, it IS SourceID-indexed.
          */
      Variable& setSourceIndexed(bool sourceIndexed)
      { isSourceIndexed = sourceIndexed; return (*this); };


         /// Get if this variable is SatID-indexed.
      bool getSatIndexed() const
      { return isSatIndexed; };


         /** Set if this variable is SatID-indexed.
          *
          * @param satIndexed       Whether this variable is SatID-indexed
          *                         or not. By default, it is NOT.
          */
      Variable& setSatIndexed(bool satIndexed)
      { isSatIndexed = satIndexed; return (*this); };


         /// Get the SourceID
      SourceID getSource() const
      { return varSource; };


         /** Set this variable's SourceID
          *
          * @param source     Specific SourceID of variable.
          */
      Variable& setSource(const SourceID& source)
      { varSource = source; setSourceIndexed(true); return (*this); };



         /// Get variable-specific satellite
      SatID getSatellite() const
      { return varSat; };


         /** Set variable-specific satellite
          *
          * @param satellite  Specific SatID of variable.
          */
      Variable& setSatellite(const SatID& satellite)
      { varSat = satellite; setSatIndexed(true); return (*this); };


         /// Equality operator
      virtual bool operator==(const Variable& right) const;


         /// This ordering is somewhat arbitrary, but is required to be able
         /// to use a Variable as an index to a std::map, or as part of a
         /// std::set.
      virtual bool operator<(const Variable& right) const;


         /// Inequality operator
      bool operator!=(const Variable& right) const
      { return !(operator==(right)); }


         /// Assignment operator.
      virtual Variable& operator=(const Variable& right);


         /// SourceID object representing all sources : type(Unknown),
         /// sourceName("").
      static SourceID allSources;


         /// SatID object representing all satellites : type(Unknown),
         /// sourceName("").
      static SatID allSats;


         /// Destructor
      virtual ~Variable() {};


   private:


         /// Type of the variable
      TypeID varType;


         /// Pointer stochastic model applying to variable
      StochasticModel* pVarModel;


         /** Whether this variable is or not source-specific. By default
          *  it is true, which means that this variable is different for
          *  each source (p.e., the TypeID::dx position variable of a given
          *  station).
          */
      bool isSourceIndexed;


         /** Whether this variable is or not satellite-specific. By default
          *  it is false, which means that this variable is the same
          *  independently from the satellites involved (p.e., the TypeID::dx
          *  position variable of a given station).
          *  On the other hand, if this variable is satellite-specific (i.e.,
          *  this field is true), this means that this variable is different
          *  for each satellite on view; for instance, the TypeID::dtSat
          *  satellite clock offset variable.
          */
      bool isSatIndexed;


         /** In case the value of this variable belongs to a specific
          *  source, the corresponding SourceID is stored here.
          */
      SourceID varSource;


         /** In case the value of this variable belongs to a specific
          *  satellite, the corresponding SatID is stored here.
          */
      SatID varSat;


         /// Default stochastic model to be assigned to variables.
      static WhiteNoiseModel defaultModel;


   }; // End of class 'Variable'

      //@}

}  // End of namespace gpstk
#endif   // VARIABLE_HPP
