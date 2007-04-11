#pragma ident "$Id: MDPNavSubframe.hpp 428 2007-03-05 17:25:31Z ocibu $"

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
//  Copyright 2004, The University of Texas at Austin
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

      void fillArray(uint32_t out[10]) const
      { for(int i=1; i<=10; i++) out[i-1]=subframe[i];};

      // This is an experiment. It inverts words based upon the D30
      // of the previous word. Don't do this unless there is some reason
      // to believe that the D30 bits are accurate. Like the subframe
      // has passed its parity check.
      void cookSubframe() throw();
      
      /** Check the parity of the subframe.
       * @return true if the parity check passes
       **/
      bool checkParity() const throw();

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


      // These are not actually encoded in the message but are used
      // in the parity checking
      bool cooked;     ///< true if the bits have been set upright
      bool inverted;   ///< true when entire subframe has been inverted

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
