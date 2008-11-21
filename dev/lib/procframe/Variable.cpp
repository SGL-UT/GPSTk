#pragma ident "$Id$"

/**
 * @file Variable.cpp
 * Class to define and handle GNSS variables.
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


#include "Variable.hpp"


namespace gpstk
{


      // SourceID object representing all sources : type(Unknown),
      // sourceName("").
   SourceID Variable::allSources;


      // SourceID object representing "some" sources : type(Mixed),
      // sourceName("").
   SourceID Variable::someSources(SourceID::Mixed, "");


      // SatID object representing all satellites : id(-1),
      // system(systemGPS).
   SatID Variable::allSats;


      // Default stochastic model to be assigned to variables.
   WhiteNoiseModel Variable::defaultModel;


      // Default constructor for Variable
   Variable::Variable()
   {

      TypeID type;   // Unknown/undefined variable type
      varType = type;

      pVarModel = &defaultModel;

      isSourceIndexed = true;

      isSatIndexed = false;

         // By default, the source associated to this variable is unspecific
      varSource = allSources;

         // By default, the satellite associated to this variable is unspecific
      varSat = allSats;

   }  // End of 'Variable::Variable()'



      /* Common constructor for Variable.
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
   Variable::Variable( const TypeID& type,
                       StochasticModel* pModel,
                       bool sourceIndexed,
                       bool satIndexed )
   {

      varType = type;

      if(pModel == NULL)
      {
         pVarModel = &defaultModel;
      }
      else
      {
         pVarModel = pModel;
      }

      isSourceIndexed = sourceIndexed;

      isSatIndexed = satIndexed;

         // By default, the source associated to this variable is unspecific
      varSource = allSources;

         // By default, the satellite associated to this variable is unspecific
      varSat = allSats;

   }  // End of 'Variable::Variable()'



      /* Constructor for a Variable corresponding to an specific
       * data source and satellite
       *
       * @param type        TypeID of variable.
       * @param pModel      Pointer to StochasticModel associated with
       *                    this variable. By default, it is a white
       *                    noise model.
       * @param source      Data source this variable belongs to.
       * @param satellite   Satellite this variable belongs to.
       */
   Variable::Variable( const TypeID& type,
                       StochasticModel* pModel,
                       const SourceID& source,
                       const SatID& satellite )
   {

      varType = type;

      pVarModel = pModel;

         // Check if the source is unspecific
      if( (source == allSources) || (source == someSources) )
      {
         isSourceIndexed = true;
      }
      else
      {
         isSourceIndexed = false;
      }


         // Check if the satellite is unspecific
      if( satellite == allSats )
      {
         isSatIndexed = true;
      }
      else
      {
         isSatIndexed = false;
      }

         // Set the source associated to this variable
      varSource = source;

         // Set the satellite associated to this variable
      varSat = satellite;

   }  // End of 'Variable::Variable()'



      /* Constructor for a Variable corresponding to an specific
       * data source
       *
       * @param type        TypeID of variable.
       * @param pModel      Pointer to StochasticModel associated with
       *                    this variable. By default, it is a white
       *                    noise model.
       * @param source      Data source this variable belongs to.
       */
   Variable::Variable( const TypeID& type,
                       StochasticModel* pModel,
                       const SourceID& source )
   {

         // Call general constructor setting a generic satellite
      Variable::Variable( type,
                          pModel,
                          source,
                          allSats );

   }  // End of 'Variable::Variable()'



      /* Constructor for a Variable corresponding to an specific
       * satellite
       *
       * @param type        TypeID of variable.
       * @param pModel      Pointer to StochasticModel associated with
       *                    this variable. By default, it is a white
       *                    noise model.
       * @param satellite   Satellite this variable belongs to.
       */
   Variable::Variable( const TypeID& type,
                       StochasticModel* pModel,
                       const SatID& satellite )
   {

         // Call general constructor setting a generic source
      Variable::Variable( type,
                          pModel,
                          allSources,
                          satellite );

   }  // End of 'Variable::Variable()'



      // Equality operator
   bool Variable::operator==(const Variable& right) const
   {

      return ( ( varType == right.getType() )                     &&
               ( pVarModel == right.getModel() )                  &&
               ( isSourceIndexed == right.getSourceIndexed() )    &&
               ( isSatIndexed == right.getSatIndexed() )          &&
               ( varSource == right.getSource() )                 &&
               ( varSat == right.getSatellite() ) );

   }  // End of 'Variable::operator=='



      // This ordering is somewhat arbitrary, but is required to be able
      // to use a Variable as an index to a std::map, or as part of a
      // std::set.
   bool Variable::operator<(const Variable& right) const
   {

      if( varType == right.getType() )
      {
         if( varSource == right.getSource() )
         {
            return (varSat < right.getSatellite());
         }
         else
         {
            return (varSource < right.getSource());
         }
      }
      else
      {
         return (varType < right.getType());
      }

   }  // End of 'Variable::operator<'



      // Assignment operator.
   Variable& Variable::operator=(const Variable& right)
   {

         // First check if these Variables are the same
      if ( this == &right ) return (*this);

      setType(right.getType());

      setModel(right.getModel());

      setSourceIndexed(right.getSourceIndexed());

      setSatIndexed(right.getSatIndexed());

      setSource(right.getSource());

      setSatellite(right.getSatellite());

      return *this;

   }  // End of 'Variable::operator='



}  // End of namespace gpstk
