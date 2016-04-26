#ifndef LNAVEPHMAKER_HPP
#define LNAVEPHMAKER_HPP

#include <NavFilter.hpp>
#include <LNavFilterData.hpp>

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
       * @attention Processing depth = 3 epochs.
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
