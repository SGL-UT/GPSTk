#pragma ident "$Id$"

/**
 * @file NablaOp.hpp
 * This is a class to apply the Nabla operator (differences on
 * satellite-related data) to GNSS data structures.
 */

#ifndef GPSTK_NABLAOP_HPP
#define GPSTK_NABLAOP_HPP

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



#include "ProcessingClass.hpp"


namespace gpstk
{

      /** @addtogroup GPSsolutions */
      //@{

      /** This class applies the Nabla operator (differences on
       *  satellite-related data) to GNSS data structures.
       *
       * A typical way to use this class follows:
       *
       * @code
       *      // Input observation file stream
       *   RinexObsStream rin("ebre0300.02o");
       *      // Reference position of receiver station
       *   Position nominalPos(4833520.2269, 41537.00768, 4147461.489);
       *
       *      // Some more code and definitions here...
       *
       *      // GNSS data structure for rover data
       *   gnssRinex gRin;
       *
       *      // Set model defaults. A typical C1-based modeling is used
       *   ModeledPR model( nominalPos,
       *                    ionoStore,
       *                    mopsTM,
       *                    bceStore,
       *                    TypeID::C1,
       *                    true );
       *
       *      // Create an object to compute single differences on
       *      // satellite-related data. The highest satellite will be used as
       *      // reference satellite and prefit residuals and geometric
       *      // coefficients dx, dy, dz will be differenced (this is the
       *      // default behaviour).
       *   NablaOp nabla;
       *
       *   while(rin >> gRin)
       *   {
       *
       *         // By default, difference is applied on code prefit residuals
       *         // and and geometric coefficients dx, dy, dz; therefore, you
       *         // must use a modeler object before 'nabla'
       *      gRin >> model >> nabla;
       *   }
       *
       * @endcode
       *
       * The "NablaOp" object will visit every satellite in the GNSS data
       * structure that is "gRin" and, unless told otherwise with 'setRefSat()'
       * method, it will pick up as reference satellite the satellite with the
       * highest elevation.
       *
       * Afterwards, it will REMOVE reference satellite data from the GNSS data
       * structure and will substract the specified type or types (code prefit
       * residuals, dx, dy, dz by default) from the corresponding data in the
       * rest of the "gRef" data structure.
       *
       * Take notice that in the default case the types to be differenced were
       * computed by "ModeledPR" or "ModeledReferencePR" objects (among others),
       * so those steps are mandatory.
       *
       * Be warned that, by default, if a given satellite does not have
       * in "gRin" the data required to be differenced, it will be summarily
       * deleted from the data structure.
       *
       * @warning Using method "setRefSat()" may be desirable in order to ensure
       * that different receivers use the same reference satellite. However, you
       * must be aware that method "setRefSat()" will also disable the automatic
       * lookup of the satellite with the highest elevation. That may cause
       * troubles if the reference satellite goes out of view at a later epoch.
       * Therefore, you should later re-enable automatic lookup with the method
       * "useHighestSat(void)".
       *
       * @sa DeltaOp.hpp for differences on ground-related data.
       *
       */
   class NablaOp : public ProcessingClass
   {
   public:


         /** Default constructor.
          *
          * By default it will difference prefitC, dx, dy, and dz data and will
          * take as reference satellite the one with the highest elevation.
          */
      NablaOp();


         /** Common constructor taking as input the reference satellite
          *  to be used.
          *
          * @param rSat    SatID of satellite to be used as reference.
          */
      NablaOp(const SatID& rSat);


         /** Common constructor taking as input the type of data values
          *  to be differenced.
          *
          * @param difftype   TypeID of data values to be differenced.
          */
      NablaOp(const TypeID& difftype)
         : lookReferenceSat(true)
      { setDiffType(difftype); setIndex(); };


         /** Common constructor taking as input the type of data values
          *  to be differenced and the reference satellite.
          *
          * @param rSat       SatID of satellite to be used as reference.
          * @param difftype   TypeID of data values to be differenced.
          */
      NablaOp( const SatID& rSat,
               const TypeID& difftype )
         : refSat(rSat), lookReferenceSat(false)
      { setDiffType(difftype); setIndex(); };


         /** Common constructor taking as input a set of types of data values
          *  to be differenced, as wll as the reference satellite.
          *
          * @param rSat       SatID of satellite to be used as reference.
          * @param diffSet    TypeIDSet of data values to be differenced.
          */
      NablaOp( const SatID& rSat,
               const TypeIDSet& diffSet )
         : refSat(rSat), lookReferenceSat(false), diffTypes(diffSet)
      { setIndex(); };


         /** Method to set the reference satellite to be used. It is not
          *  common to use this method.
          *
          * @param rSat       SatID of reference satellite to be used.
          *
          * @warning Using this method will disable the automatic lookup of
          * the satellite with the highest elevation. That may cause troubles
          * if the reference satellite goes out of view at a later epoch.
          * Therefore, you should later re-enable automatic lookup with the
          * method "useHighestSat(void)".
          */
      virtual NablaOp& setRefSat(const SatID& rSat)
      { refSat = rSat; lookReferenceSat = false; return (*this); };


         /// Method to indicate that the reference satellite will be the one
         /// with highest elevation. This is the default behaviour.
      virtual NablaOp& useHighestSat(void)
      { lookReferenceSat = true; return (*this); };


         /// Method to get the reference satellite being used.
      virtual SatID getRefSat(void) const
      { return refSat; };


         /** Method to set data type values to be differenced.
          *
          * @param difftype      TypeID of data values to be differenced.
          *
          * \warning  Previously set data type values will be deleted. If this
          * is not what you want, see method 'addDiffType()'.
          */
      virtual NablaOp& setDiffType(const TypeID& difftype)
      { diffTypes.clear(); diffTypes.insert(difftype); return (*this); };


         /** Method to add a data value type to be differenced.
          *
          * @param difftype      TypeID of data values to be added to the ones
          *                      being differenced.
          */
      virtual NablaOp& addDiffType(const TypeID& difftype)
      { diffTypes.insert(difftype); return (*this); };


         /** Method to establish a set of data values to be differenced.
          *
          * @param diffSet       TypeIDSet of data values to be differenced.
          *
          * \warning  Previously set data type values will be deleted. If this
          * is not what you want, see method 'addDiffTypeSet()'.
          */
      virtual NablaOp& setDiffTypeSet(const TypeIDSet& diffSet)
      { diffTypes.clear(); diffTypes = diffSet; return (*this); };


         /** Method to add a set of data value types to be differenced.
          *
          * @param diffSet    TypeIDSet of data values to be added to the
          *                   ones being differenced.
          */
      virtual NablaOp& addDiffTypeSet(const TypeIDSet& diffSet);


         /// Method to get the set of data value types to be differenced.
      virtual TypeIDSet getDiffTypeSet(void) const
      { return diffTypes; };


         /** Returns a reference to a satTypeValueMap object after differencing
          *  data type values given in 'diffTypes' field with respect to
          *  reference station data in 'refData' field.
          *
          * @param gData      Data object holding the data.
          */
      virtual satTypeValueMap& Process(satTypeValueMap& gData)
         throw(ProcessingException);


         /** Returns a reference to a gnssSatTypeValue object after
          *  differencing data type values given in 'diffTypes' field with
          *  respect to reference station data in 'refData' field.
          *
          * @param gData      Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException)
      { Process(gData.body); return gData; };


         /** Returns a reference to a gnnsRinex object after differencing
          *  data type values given in 'diffTypes' field with respect to
          *  reference station data in 'refData' field.
          *
          * @param gData      Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException)
      { Process(gData.body); return gData; };


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor.
      virtual ~NablaOp() {};


   private:


         /// SatID of reference satellite.
      SatID refSat;


         /// Flag indicating if we have to look for a reference satellite.
      bool lookReferenceSat;


         /// Set (TypeIDSet) containing the types of data to be differenced.
      TypeIDSet diffTypes;


         /// Initial index assigned to this class.
      static int classIndex;


         /// Index belonging to this object.
      int index;


         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };



   }; // End of class 'NablaOp'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_NABLAOP_HPP
