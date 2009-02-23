#pragma ident "$Id$"

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

#ifndef MDPOBSEPOCH_HPP
#define MDPOBSEPOCH_HPP

#include <map>

#include "MDPHeader.hpp"
#include "miscenum.hpp"

namespace gpstk
{
   /// This class represents a set of observations from a single SV at a 
   /// single point in time
   class MDPObsEpoch : public MDPHeader
   {
   public:
      // A helper class to hold a single code/carrier block of observations
      class Observation
      {
      public:
         Observation() throw(): carrier(ccUnknown), range(rcUnknown),bw(0),
            snr(0), lockCount(0), pseudorange(0), phase(0), doppler(0) {}

         void dump(std::ostream& out) const throw();
         std::string encode() const throw();

         // Note that this is a destructive decode, unlike the others in this
         //  set of classes.
         void decode(std::string& str) throw();
  
         CarrierCode carrier;     ///< This observation's carrier frequency.
         RangeCode range;         ///< This observation's range code.
         unsigned bw;             ///< The effective tracking loop bandwidth, Hz
         double snr;              ///< The Signal-to-Noise Ratio. (dB-Hz)
         unsigned long lockCount; ///< The number of consecutive observations, 0
                                  ///< implies a cycle slip or start of track
         double pseudorange;      ///< Pseudorange Measurement (meters)
         double phase;            ///< Phase Measurement (cycles)
         double doppler;          ///< Doppler Measurement, Hz,
                                  ///< positive for approaching satellites
      };

      MDPObsEpoch() throw();

      /**  Encode this object to a string.
       * @return this object as a string.
       */
      virtual std::string encode() const
         throw();
         
      /** Decode this object from a string.
       * @param str the string to read from.
       * @note This is a non-destructive decode.
       */
      virtual void decode(std::string str)
         throw();
         
      virtual std::string getName() const {return "obs";};

      /** Dump some debugging information to the given ostream.
       * @param out the ostream to write to
       */
      void dump(std::ostream& out) const throw();

      typedef std::pair<CarrierCode, RangeCode> ObsKey;
      typedef std::map<ObsKey, Observation> ObsMap;
   
      unsigned numSVs;      ///< The number of SVs in track for this epoch.
      unsigned channel;     ///< The channel this SV is tracked on.
      unsigned prn;         ///< The SV's PRN.
      unsigned status;      ///< SV Health status.
      double elevation;     ///< The SV's elevation in degrees.
      double azimuth;       ///< The SV's azimuth in degrees.

      ObsMap obs;          ///< The list of Observations made at this Epoch.

      bool haveObservation(const CarrierCode cc, const RangeCode rc) const;
      Observation getObservation(const CarrierCode cc, const RangeCode rc) 
         const;

      bool haveObservation(const ObsKey& ok) const
      {return haveObservation(ok.first, ok.second);}

      Observation getObservation(const ObsKey& ok) const
      {return getObservation(ok.first, ok.second);}

      static const unsigned myId = 300;
      // This is just the pre obs_block stuff
      static const unsigned myLength = 8;
      // This is the size of a single obs_block
      static const unsigned myObsLength=32;
   }; // class MDPObsEpoch

   // The key is intended to the PRN of the MDPObsEpoch
   typedef std::multimap<int, gpstk::MDPObsEpoch> MDPEpoch;
   // This is useful when finding all obs from a prn
   // BTW, MCIP stands for MDPEPoch const interator pair
   typedef std::pair<MDPEpoch::const_iterator, MDPEpoch::const_iterator> MCIP;

   void dump(std::ostream& out, const MDPEpoch& me);

   FFStream& operator>>(FFStream& s, MDPEpoch& oe);
   FFStream& operator<<(FFStream& s, const MDPEpoch& oe);

} // namespace gpstk

#endif //MDPOBSEPOCH_HPP
