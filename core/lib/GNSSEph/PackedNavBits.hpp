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
 * @file PackedNavBits.hpp
 * Engineering units navigation message abstraction.
 */

#ifndef GPSTK_PACKEDNAVBITS_HPP
#define GPSTK_PACKEDNAVBITS_HPP

#include <vector>
#include <cstddef>
#include "gpstkplatform.h" //#include <stdint.h>
#include <string>

#include "ObsID.hpp"
#include "SatID.hpp"
#include "CommonTime.hpp"
#include "Exception.hpp"

namespace gpstk
{
   /// @ingroup ephemcalc 
   //@{

   class PackedNavBits
   {
   public:
      /// empty constructor
      PackedNavBits();

      /// explicit constructor
      PackedNavBits(const SatID& satSysArg, 
                    const ObsID& obsIDArg,
                    const CommonTime& transmitTimeArg);

      /// explicit constructor
      PackedNavBits(const SatID& satSysArg, 
                    const ObsID& obsIDArg,
                    const std::string rxString,
                    const CommonTime& transmitTimeArg);

      PackedNavBits(const PackedNavBits& right);             // Copy constructor
      //PackedNavBits& operator=(const PackedNavBits& right); // Copy assignment

      PackedNavBits* clone() const;

      void setSatID(const SatID& satSysArg);
      void setObsID(const ObsID& obsIDArg);
      void setRxID(const std::string rxString); 
      void setTime(const CommonTime& transmitTimeArg);
      void clearBits();

         /* Returnst the satellite system ID for a particular SV */
      SatID getsatSys() const;

         /* Returns Observation type, Carrier, and Tracking Code */
      ObsID getobsID() const;

         /* Returns string defining the receiver that collected the data. 
            NOTE: This was a late addition to PackedNavBits and may not  
            be present in all applications */ 
      std::string getRxID() const; 

         /* Returns time of transmission from SV */
      CommonTime getTransmitTime() const;

         /* Returns the number of bits */
      size_t getNumBits() const;

         /* Output the contents of this class to the given stream. */
      void dump(std::ostream& s = std::cout) const throw();
      
         /***    UNPACKING FUNCTIONS *********************************/
         /* Unpack an unsigned long integer */
      unsigned long asUnsignedLong(const int startBit, 
                                   const int numBits, 
                                   const int scale ) const;

         /* Unpack a signed long integer */
      long asLong(const int startBit, 
                  const int numBits, 
                  const int scale ) const;

         /* Unpack an unsigned double */
      double asUnsignedDouble( const int startBit, 
                               const int numBits, 
                               const int power2) const;

         /* Unpack a signed double */
      double asSignedDouble( const int startBit, 
                             const int numBits, 
                             const int power2) const;

         /* Unpack a double with units of semicircles */
      double asDoubleSemiCircles( const int startBit, 
                                  const int numBits, 
                                  const int power2) const;
     
         /* Unpack a string */
      std::string asString(const int startBit, 
                           const int numChars) const;

         // The following three methods were added to support
         // GLONASS sign/magnitude real values.
         //
         // Since GLONASS has no disjoint fields (at least not
         // up through ICD Edition 5.1) there are no methods
         // for unpacking disjoint-field sign/mag quantities. 
         /* Unpack a sign/mag long */
      long asSignMagLong(const int startBit, 
                  const int numBits, 
                  const int scale) const;
                  
         /* Unpack a sign/mag double */
      double asSignMagDouble( const int startBit, 
                             const int numBits, 
                             const int power2) const;
                             
         /* Unpack a sign/mag double with units of semi-circles */
      double asSignMagDoubleSemiCircles( const int startBit, 
                                  const int numBits, 
                                  const int power2) const;

         /* Unpack mehthods that join multiple disjoint 
            navigation message areas as a single field
            NOTE: startBit1 is associated with the most significant section
                  startBit2 is associated with the least significant section
         */
         /* Unpack a split unsigned long integer */
      unsigned long asUnsignedLong(const unsigned startBits[],
                                   const unsigned numBits[],
                                   const unsigned len,
                                   const int scale ) const;

         /* Unpack a signed long integer */
      long asLong(const unsigned startBits[],
                  const unsigned numBits[],
                  const unsigned len, 
                  const int scale ) const;

         /* Unpack a split unsigned double */
      double asUnsignedDouble( const unsigned startBits[],
                               const unsigned numBits[],
                               const unsigned len,
                               const int power2) const;

         /* Unpack a split signed double */
      double asSignedDouble( const unsigned startBits[],
                             const unsigned numBits[],
                             const unsigned len,
                             const int power2) const;

         /* Unpack a split double with units of semicircles */
      double asDoubleSemiCircles( const unsigned startBits[],
                                  const unsigned numBits[],
                                  const unsigned len,
                                  const int power2) const;      

         /***    PACKING FUNCTIONS *********************************/
         /* Pack an unsigned long integer */
      void addUnsignedLong( const unsigned long value, 
                            const int numBits,  
                            const int scale ) 
         throw(InvalidParameter);
        
         /* Pack a signed long integer */                     
      void addLong( const long value, 
                    const int numBits, 
                    const int scale )
         throw(InvalidParameter);

         /* Pack an unsigned double */
      void addUnsignedDouble( const double value, 
                              const int numBits, 
                              const int power2)
         throw(InvalidParameter);

         /* Pack a signed double */
      void addSignedDouble( const double value, 
                            const int numBits, 
                            const int power2)
         throw(InvalidParameter);

         /* Pack a double with units of semicircles */
      void addDoubleSemiCircles( const double radians, 
                                 const int numBits, 
                                 const int power2)
         throw(InvalidParameter);

         /**
          * Pack a string.
          * Characters in String limited to those defined in IS-GPS-200 Section 20.3.3.5.1.8
          * numChars represents number of chars (8 bits each) to add to PackedBits.
          * If numChars < length of String only, chars 1..numChars will be added.
          * If numChars > length of String, blanks will be added at the end. */
      void addString(const std::string String, 
                     const int numChars)
         throw(InvalidParameter);
      
      void addPackedNavBits( const PackedNavBits &pnb)
         throw(InvalidParameter);
   
         /*
          * Output the packed bits as a set of 32 bit
          * hex values, four per line, without any
          * additional information. 
          * Returns the number of bits in the object.
          */
      int outputPackedBits(std::ostream& s = std::cout, 
		           const short numPerLine=4,
		     	     const char delimiter = ' ',
               const short numBitsPerWord=32 ) const;

         /* 
          * The equality operator insists that ALL the metadata
          * and the complete bit patterns must match.   
          * However, there are frequently occaisions when only 
          * a subset of the metadata need be checked, and sometimes
          * only certain set of bits.  Therefore, operator==( ) is
          * supplemented by matchBits( ) and matchMetaData( )
          */
      bool operator==(const PackedNavBits& right) const;

         /*
          * There are frequently cases in which we want to know
          * if a pair of PackedNavBits objects are from the same
          * SV, but we might want to allow for different receivers
          * and/or different ObsIDs.  Therefore, matchMetaData( )
          * allows specification of the particular metadata items
          * that are to be checked using a bit-flag system.
          */ 
      static const unsigned int mmTIME = 0x0001;  // Check transmitTime
      static const unsigned int mmSAT  = 0x0002;  // Check SatID
      static const unsigned int mmOBS  = 0x0004;  // Check ObsID
      static const unsigned int mmRX   = 0x0008;  // Check Receiver ID
      static const unsigned int mmALL  = 0xFFFF;  // Check ALL metadata
      static const unsigned int mmNONE = 0x0000;  // NO metadata checks
      bool matchMetaData(const PackedNavBits& right,
                         const unsigned flagBits=mmALL) const;
         /*
          * Return true if all bits between start and end are identical
          * between this object and right.  Default is to compare all
          * bits.  
          *
          * This method allows comparison of the "unchanging" data in 
          * nav messages while avoiding the time tags.
          */
      bool matchBits(const PackedNavBits& right, 
                     const short startBit=0, 
                     const short endBit=-1) const;

          /*
           *  This is the most flexible of the matching methods.
           *  A default of match(right) will yield the same 
           *  result as operator==( ).
           *  However, the arguments provide the means to 
           *  specifically check bits sequences and/or
           *  selectively check the metadata. 
           */
      bool match(const PackedNavBits& right, 
                 const short startBit=0, 
                 const short endBit=-1,
                 const unsigned flagBits=mmALL) const;
          /*
          * This version was the original equality checker.  As
          * first implemented, it checks ONLY SAT and OBS for
          * equality.  Therefore, it is maintained with that
          * default functionality.  That is to say, when 
          * checkOverhead==true, the result is the same as a call
          * to matchBits(right,startBit,endBit, (mmSAT|mmOBS)).
          *
          * For clarity, it is suggested that new code use
          *  operator==(),
          *  matchMetaData(), and/or 
          *  matchBits( ) using explicit flags. 
          *
          * This version was REMOVED because of ambiguity
          * in the signature. 
          *
          * The checkOverhead option allows the user to ignore
          * the associated metadata.  E.g. ObsID, SatID. 
          *
      bool matchBits(const PackedNavBits& right, 
                     const short startBit=0, 
                     const short endBit=-1, 
                     const bool checkOverhead) const;
          */

         /** 
          * The less than operator is defined in order to support use
          *   with the NavFilter classes.  The idea is to provide a
          *   "sort" for bits contained in the class.  Matching strings
          *   will fail both  a < b and b < a; however, in the process
          *   all matching strings can be sorted into sets and the 
          *   "winner" determined. 
          */
      bool operator<(const PackedNavBits& right) const; 

         /* Resize the vector holding the packed data. */
      void trimsize();

         /**
          * Raw bit input
          * This function is intended as a test-support function.
          * It assumes a string of the form
          *    ###  0xABCDABCD 0xABCDABCD 0xABCDABCD
          * where
          *    ### is the number of bits to expect in the remainder
          *        of the line.
          *    0xABCDABCD are each 32-bit unsigned hex numbers, left 
          *        justified.  The number of bits needs to match or
          *        exceed ###
          * The function returns if the read is succeessful.
          * Otherwise,the function throws an exception */
       void rawBitInput(const std::string inString )
          throw(InvalidParameter);       

       void setXmitCoerced(bool tf=true) {xMitCoerced=tf;}
       bool isXmitCoerced() const {return xMitCoerced;}

   private:
      SatID satSys;            /**< System ID (based on RINEX defintions */
      ObsID obsID;             /**< Defines carrier and code tracked */
      std::string rxID;        /**< Defines the receiver that collected the data */
      CommonTime transmitTime; /**< Time nav message is transmitted */
      std::vector<bool> bits;  /**< Holds the packed data */
      int bits_used;
      
      bool xMitCoerced;        /**< Used to indicate that the transmit
                                    time is NOT directly derived from
                                    the SOW in the message */

         /** Unpack the bits */
      uint64_t asUint64_t(const int startBit, const int numBits ) const 
         throw(InvalidParameter);

         /** Pack the bits */
      void addUint64_t( const uint64_t value, const int numBits );

         /** Extend the sign bit for signed values */
      int64_t SignExtend( const int startBit, const int numBits ) const;
   
         /** Scales doubles by their corresponding scale factor */
      double ScaleValue( const double value, const int power2) const;

   }; // class PackedNavBits

   //@}
   std::ostream& operator<<(std::ostream& s, const PackedNavBits& pnb);

} // namespace

#endif
