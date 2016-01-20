#ifndef NAVFILTERKEY_HPP
#define NAVFILTERKEY_HPP

#include <ObsID.hpp>

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** Defines a class that is used by NavFilter and child classes
       * to both identify the source of a navigation message as well
       * as the message itself.  The navigation messages are defined
       * as child classes of NavFilterKey such that the message-format
       * specific data is defined.
       *
       * To put it another way, this is the base class and the child
       * classes define the data storage for the nav message. */
   class NavFilterKey
   {
   public:
         /// Initialize key members to empty defaults
      NavFilterKey();
      std::string stationID;       ///< site/station identifier for data source
      std::string rxID;            ///< receiver identifier for data source
      uint32_t prn;                ///< identifier of broadcasting satellite 
      ObsID::CarrierBand carrier;  ///< carrier band of navigation message
      ObsID::TrackingCode code;    ///< ranging code of navigation message
         // Nav code is not necessary as each filter is unique to a
         // given navigation message structure.

      bool operator<(const NavFilterKey& right) const;
   protected:
         // make this a polymorphic type so dynamic_cast works
      virtual void dummy() {}
   };

      //@}

}

#endif // NAVFILTERKEY_HPP
