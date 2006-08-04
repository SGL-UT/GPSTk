#pragma ident "$Id$"


/**
 * @file MDPHeader.hpp
 * gpstk::MDPHeader - base cass for MDP formatted data. Includes the MDP header.
 */

//lgpl-license START
//lgpl-license END

#ifndef MDPHEADER_HPP
#define MDPHEADER_HPP

#include <map>
#include <iostream>

#include <Exception.hpp>
#include <DayTime.hpp>
#include <StringUtils.hpp>
#include <BinUtils.hpp>

#include <FFData.hpp>
#include <FFStream.hpp>
#include <DataStatus.hpp>

#include <RinexObsData.hpp>

#ifndef __SUNPRO_CC
#include <stdint.h>
#endif

namespace gpstk
{
   /// This class contains header information for all data types
   /// transmitted by the MSN SAASM Receiver.
   class MDPHeader : public FFData, public CRCDataStatus
   {
   public:
      MDPHeader() throw() : id(0) {}

      // Returns whether or not this MDPData is valid.
      bool isValid() const {return good();}

      //! This class is "data" so this function always returns "true".
      virtual bool isData() const {return true;}

      /** Encode this object to a string.
       * @return this object as a string.
       */
      virtual std::string encode() const throw();
         
      /** Decode this object from a string.
       * @param str the string to read from.
       * @note Decoded data is *NOT* removed from the string.
       */
      virtual void decode(std::string str) throw();

      /// Simple accessors for various static thangs.
      virtual std::string getName() const {return "hdr";}

      /** Dump some debugging information to the given ostream.
       * @param out the ostream to write to
       */
      void dump(std::ostream& out) const throw();

      /// Compute the CRC over the string and set the CRC in that
      /// string and in the header.
      void setCRC(std::string& str) const throw();
         
      /// Checks the crc of the message and set crc_err if it fails.
      void checkCRC(std::string str) throw();

      unsigned short id;              ///< Identifies this data's type.
      gpstk::DayTime time;            ///< The time associated with this message
      unsigned short freshnessCount;  ///< A message counter

      // These items need to be mutable so they can be set when an object
      // is sent to a stream.
      mutable unsigned short length;  ///< number of bytes in this message (includes the length of the header)
      mutable unsigned short crc;     ///< the 16 bit CCITT crc covering the header and body of the message

      static const unsigned myLength;  ///< = 16
      static const unsigned frameWord; ///< = 0x9c9c
      static int debugLevel;
      static bool hexDump;

   protected:
      virtual void reallyPutRecord(FFStream& s) const 
         throw(std::exception, StringUtils::StringException, 
               FFStreamError);

      virtual void reallyGetRecord(FFStream& s)
         throw(std::exception, gpstk::StringUtils::StringException, 
               FFStreamError, EndOfFile);
   }; // class MDPHeader

} // namespace gpstk

#endif // MDPHEADER_HPP
