//==============================================================================
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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

#ifndef LNAVEPHMAKER_HPP
#define LNAVEPHMAKER_HPP

#include "NavFilter.hpp"
#include "LNavFilterData.hpp"

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** Sort function for LNavEphMaker.  Orders items by station,
       * rx, prn, carrier, code. */
   struct LNavEphMakerSort
      : std::binary_function<LNavFilterData*,LNavFilterData*,bool>
   {
      inline bool operator()(const LNavFilterData*const& l,
                             const LNavFilterData*const& r)
         const;
   };

      /** "Filter" GPS LNAV subframes, assembling valid ephemerides
       * into groups.
       *
       * @attention Processing depth = 3 epochs.  Unlike other
       * filters, valid data isn't always returned using the
       * validate() or finalize() methods.  In this implementation,
       * validate() only uses the accept mechanism for returning data
       * from validate() calls for subframes that are NOT ephemeris
       * subframes, i.e. not subframe ID 1-3.  Instead, valid
       * ephemeris subframes are stored internally in the ephemerides
       * structure and then copied to completedEphs when the full
       * three subframes are accumulated.  The data in completedEphs
       * should be treated as accepted and should also be deleted when
       * the user is finished with the data.
       */
   class LNavEphMaker : public NavFilter
   {
   public:
         /// Group of 3 subframes making up an ephemeris
      typedef std::vector<LNavFilterData*> EphGroup;
         /// List of complete ephemerides
      typedef std::list<EphGroup*> EphList;
         /// Ephemerides by stn/rx/prn/carrier/code
      typedef std::map<LNavFilterData*, EphGroup, LNavEphMakerSort> EphMap;

      LNavEphMaker();

         /**
          * @note All messages will pass this "filter", however only
          *   valid ephemerides will be made available to the user in
          *   the completeEphs structure after a validate() call.
          * @pre The subframe ID and TOW count are valid
          *   (i.e. LNavFilterData::sf[1] bits 8-10 and 13-29 are
          *   valid).
          * @pre NavFilterKey::stationID is set.
          * @pre NavFilterKey::rxID is set.
          * @pre NavFilterKey::prn is set.
          * @pre NavFilterKey::carrier is set.
          * @pre NavFilterKey::code is set.
          * @pre LNavFilterData::sf is set.
          * @post Valid ephemerides will be stored in completeEphs,
          *   however the contents of completeEphs will be cleared at
          *   the beginning of this function.  As such, only those
          *   ephemerides noted as valid by a given call to validate()
          *   will be present in completeEphs.
          */
      virtual void validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut);

      virtual void finalize(NavMsgList& msgBitsOut);

         /// Internally stores 3 epochs worth of subframe data.
      virtual unsigned processingDepth() const throw()
      { return 3; }

         /// Return the filter name.
      virtual std::string filterName() const throw()
      { return "EphMaker"; }

         /// Storage for the assembly of ephemerides.
      EphMap ephemerides;
         /** Storage of pointers to complete, valid ephemerides.  This
          * data member will contain valid ephemerides identified
          * after the completion of a single validate() call.  There
          * is no carry-over of contents between calls to
          * validate(). */
      EphList completeEphs;
   };

      //@}

   bool LNavEphMakerSort ::
   operator()(const LNavFilterData*const& l, const LNavFilterData*const& r)
      const
   {
      if (l->stationID < r->stationID) return true;
      if (l->stationID > r->stationID) return false;
      if (l->rxID < r->rxID) return true;
      if (l->rxID > r->rxID) return false;
      if (l->prn < r->prn) return true;
      if (l->prn > r->prn) return false;
      if (l->carrier < r->carrier) return true;
      if (l->carrier > r->carrier) return false;
      if (l->code < r->code) return true;
         // the contents of sf won't be unique, nor will timeStamp,
         // but since we ignore those values in the sort function,
         // they are not relevant to the map key.
      return false;
   }

}

#endif // LNAVEPHMAKER_HPP
