#pragma ident "$Id$"


#ifndef MDPNAVSUBFRAME_HPP
#define MDPNAVSUBFRAME_HPP

#include <vector>
#include "miscenum.hpp"
#include "MDPHeader.hpp"

namespace gpstk
{
   /// This class represents a Navigation Subframe Message.
   class MDPNavSubframe : public MDPHeader
   {
   public:
      MDPNavSubframe() throw();
         
      /**  Encode this object to a string.
       * @return this object as a string.
       */
      virtual std::string encode() const throw();
         
      /** Decode this object from a string.
       * @param str the string to read from.
       * @note This is a non-destructive decode.
       */
      virtual void decode(std::string str)
         throw();

      // Return the seconds of week computed from the HOW
      unsigned long getHOWTime() const throw();

      // extract the SFID from the HOW 
      unsigned int getSFID() const throw();

      // extract the SVID from word 3 of an almanac page
      unsigned int getSVID() const throw();

      void fillArray(long out[10]) const
      { for(int i=1; i<=10; i++) out[i-1]=subframe[i];};
      
      virtual std::string getName() const {return "nav";}

      /** Dump some debugging information to the given ostream.
       * @param out the ostream to write to (default: cout)
       * @param level the level of debugging to provide (default: 0)
       */
      void dump(std::ostream& out) const throw();

      int prn;              ///< The SV's PRN.
      CarrierCode carrier;  ///< This NavSubframe's carrier frequency code.
      RangeCode range;      ///< This NavSubframe's range code.
      NavCode nav;          ///< This NavSubframe's nav code.
   
      /** The Navigation Subframe. 10 4-byte words.  There are 11
       * elements to facilitate access to elements 1-10. */
      std::vector<uint32_t> subframe;

      static const unsigned myLength = 44;
      static const unsigned myId = 310;

   }; // class MDPNavSubframe

   // These used to group together a set of nav subframes, indexed by the
   // subframe number. It is used in building up a complete a complete 
   // ephemeris from an SV.
   typedef std::map<short, MDPNavSubframe> EphemerisPages;

   // First element is the page number, as determined from the HOW TOW
   // i.e. page = tow % 750
   typedef std::pair<short, short> SubframePage;
   typedef std::map<SubframePage, MDPNavSubframe> AlmanacPages;

} // namespace gpstk

#endif //  MDPNAVSUBFRAME_HPP
