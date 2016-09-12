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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2009, 2011
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
 * @file Pruner.hpp
 * This class examines a GNSS Data Structure (GDS) and prunes specific
 * values according to their TypeIDs.
 */

#ifndef GPSTK_PRUNER_HPP
#define GPSTK_PRUNER_HPP

#include "ProcessingClass.hpp"


namespace gpstk
{

      /// @ingroup DataStructures
      //@{


      /** This class examines a GNSS Data Structure (GDS) and prunes specific
       * values according to their TypeIDs.
       *
       * A typical way to use this class follows:
       *
       * @code
       *       // Create the input obs file stream
       *    RinexObsStream rin("ebre0300.02o");
       *
       *   gnssRinex gRin;
       *
       *   Pruner prunerObj;
       *   prunerObj.setType( TypeID::C1 );
       *   prunerObj.addType( TypeID::L1 );
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> prunerObj;
       *   }
       * @endcode
       *
       * The "Pruner" object "prunerObj" will visit every satellite in the GNSS
       * Data Structure that is "gRin" and will delete only the information
       * associated with a configured TypeID set, trimming the incoming GDS.
       *
       * \warning If no TypeIDs are specified, then ALL TypeIDs present in
       * the GDS will be kept, and this class would have been ineffective.
       */
   class Pruner : public ProcessingClass
   {
   public:

         /// Default constructor
      Pruner()
      { };


         /** Common constructor
          *
          * @param deleteSet       TypeIDSet of data values to be deleted.
          */
      Pruner( const TypeIDSet& deleteSet )
         : deleteTypeSet(deleteSet)
      { };


         /** Prunes data from a satTypeValueMap object.
          *
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( satTypeValueMap& gData )
         throw(ProcessingException);


         /** Prunes data from a gnnsSatTypeValue object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process( gnssSatTypeValue& gData )
         throw(ProcessingException)
      { Process(gData.body); return gData; };


         /** Prunes data from a gnnsRinex object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process( gnssRinex& gData )
         throw(ProcessingException)
      { Process(gData.body); return gData; };


         /** Method to set the TypeID to be deleted.
          *
          * @param type      TypeID of data values to be deleted.
          *
          * \warning The previously set type values will be deleted. If this
          * is not what you want, see method addType.
          *
          * \warning If no TypeIDs are specified, then ALL TypeIDs present in
          * the GDS will be kept, and this class would have been ineffective.
          */
      virtual Pruner& setType( const TypeID& type )
      { deleteTypeSet.clear(); deleteTypeSet.insert(type); return (*this); };


         /** Method to add a TypeID to be deleted.
          *
          * @param type          TypeID of data values to be added to the ones
          *                      being deleted.
          */
      virtual Pruner& addType( const TypeID& type )
      { deleteTypeSet.insert(type); return (*this); };


         /** Method to specify a set of TypeIDs to be deleted.
          *
          * @param deleteSet       TypeIDSet of data values to be deleted.
          *
          * \warning The previously set type values will be deleted. If this
          * is not what you want, see method addTypeSet.
          */
      virtual Pruner& setTypeSet( const TypeIDSet& deleteSet )
      { deleteTypeSet.clear(); deleteTypeSet = deleteSet; return (*this); };


         /** Method to add a set of TypeIDs to be deleted.
          *
          * @param deleteSet     TypeIDSet of data values to be added to the
          *                      ones being deleted.
          */
      virtual Pruner& addTypeSet( const TypeIDSet& deletepSet );


         /** Method to clear the set of TypeIDs to be deleted. If you do this,
          *  all TypeIDs that are present in GDS will be kept, and this class
          *  would have been ineffective.
          */
      virtual Pruner& clearTypeSet( void )
      { deleteTypeSet.clear(); return (*this); };


         /// Method to get the set of TypeIDs to be deleted.
      virtual TypeIDSet getTypeSet(void) const
      { return deleteTypeSet; };


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~Pruner() {};


   private:


         /// Set of TypeID's to delete.
      TypeIDSet deleteTypeSet;


   }; // End of class 'Pruner'

      //@}

}  // End of namespace gpstk

#endif  // GPSTK_PRUNER_HPP
