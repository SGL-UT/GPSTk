#pragma ident "$Id$"

/**
 * @file Keeper.hpp
 * This class examines a GNSS Data Structure (GDS) and keeps only specific
 * values according to their TypeIDs.
 */

#ifndef GPSTK_KEEPER_HPP
#define GPSTK_KEEPER_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2009
//
//============================================================================



#include "ProcessingClass.hpp"


namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class examines a GNSS Data Structure (GDS) and keeps only
       *  specific values according to their TypeIDs.
       *
       * A typical way to use this class follows:
       *
       * @code
       *       // Create the input obs file stream
       *    RinexObsStream rin("ebre0300.02o");
       *
       *   gnssRinex gRin;
       *
       *   Keeper keeperObj;
       *   keeperObj.setType( TypeID::C1 );
       *   keeperpObj.addType( TypeID::L1 );
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> keeperObj;
       *   }
       * @endcode
       *
       * The "Keeper" object will visit every satellite in the GNSS Data
       * Structure that is "gRin" and will keep only the information associated
       * with a configured TypeID set, trimming the incoming GDS.
       *
       * \warning If no TypeIDs are specified, then ALL TypeIDs present in
       * the GDS will be kept, and this class would have been ineffective.
       */
   class Keeper : public ProcessingClass
   {
   public:

         /// Default constructor
      Keeper()
      { setIndex(); };


         /** Common constructor
          *
          * @param keepSet       TypeIDSet of data values to be kept.
          */
      Keeper( const TypeIDSet& keepSet )
         : keepTypeSet(keepSet)
      { setIndex(); };


         /** Keeps data from a satTypeValueMap object.
          *
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( satTypeValueMap& gData )
         throw(ProcessingException);


         /** Keeps data from a gnnsSatTypeValue object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process( gnssSatTypeValue& gData )
         throw(ProcessingException)
      { Process(gData.body); return gData; };


         /** Keeps data from a gnnsRinex object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process( gnssRinex& gData )
         throw(ProcessingException)
      { Process(gData.body); return gData; };


         /** Method to set the TypeID to be kept.
          *
          * @param type      TypeID of data values to be kept.
          *
          * \warning The previously set type values will be deleted. If this
          * is not what you want, see method addType.
          *
          * \warning If no TypeIDs are specified, then ALL TypeIDs present in
          * the GDS will be kept, and this class would have been ineffective.
          */
      virtual Keeper& setType( const TypeID& type )
      { keepTypeSet.clear(); keepTypeSet.insert(type); return (*this); };


         /** Method to add a TypeID to be kept.
          *
          * @param type          TypeID of data values to be added to the ones
          *                      being kept.
          */
      virtual Keeper& addType( const TypeID& type )
      { keepTypeSet.insert(type); return (*this); };


         /** Method to establish a set of TypeIDs to be kept.
          *
          * @param keepSet       TypeIDSet of data values to be kept.
          *
          * \warning The previously set type values will be deleted. If this
          * is not what you want, see method addTypeSet.
          */
      virtual Keeper& setTypeSet( const TypeIDSet& keepSet )
      { keepTypeSet.clear(); keepTypeSet = keepSet; return (*this); };


         /** Method to add a set of TypeIDs to be kept.
          *
          * @param keepSet       TypeIDSet of data values to be added to the
          *                      ones being kept.
          */
      virtual Keeper& addTypeSet( const TypeIDSet& keepSet );


         /** Method to clear the set of TypeIDs to be kept. If you do this,
          *  all TypeIDs that are present in GDS will be kept, and this class
          *  would have been ineffective.
          */
      virtual Keeper& clearTypeSet( void )
      { keepTypeSet.clear(); return (*this); };


         /// Method to get the set of TypeIDs to be kept.
      virtual TypeIDSet getTypeSet(void) const
      { return keepTypeSet; };


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~Keeper() {};


   private:


         /// Set of TypeID's to keep.
      TypeIDSet keepTypeSet;


         /// Initial index assigned to this class.
      static int classIndex;


         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


   }; // End of class 'Keeper'


      //@}

}  // End of namespace gpstk

#endif  // GPSTK_KEEPER_HPP
