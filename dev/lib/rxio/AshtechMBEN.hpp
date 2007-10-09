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

#ifndef ASHTECHMBEN_HPP
#define ASHTECHMBEN_HPP

/**
 * @file AshtechMBEN.hpp
 * gpstk::AshtechMBEN - class for Ashtech raw measurement data
 */

#include <map>
#include <iostream>

#include "AshtechData.hpp"

namespace gpstk
{
   class AshtechMBEN : public AshtechData
   {
   public:

      AshtechMBEN() {};

      std::string header; // 11 characters exactly
      unsigned seq;     // sow in units of 50 ms, modulo 36,000
                        // (36,000 * 50 ms = 30 minutes)
      unsigned left;
      unsigned svprn; // the PRN of the tracked satellite
      unsigned el;    // degrees
      unsigned az;    // degrees
      unsigned chid;  // 1..12

      struct code_block
      {
         // Warning flag, a bit packed field
         // bits 1&2: 0 same as goodbad=22, 1 same as goodbad=23,
         //           3 same as goodbad=24
         // bit 3: carrier phase questionable
         // bit 4: code phase questionable
         // bit 5: code phase integration not stable
         // bit 6: Z tracking mode
         // bit 7: possible lock of lock
         // bit 8: loss of lock counter reset
         unsigned  warning;
         // Measurement quality
         // 0: measurement not available
         // 22: code and or carrier phase measured
         //     P mode tracking on Z(Y)-12 units
         // 23: 22 + nav msg obtained + obs NOT used in PVT computation
         // 24: 22 + nav msg obtained + obs used in PVT computation
         //     Y mode tracking on Z(Y)-12 units
         // 25: Z mode tracking on Z(Y)-12 units
         unsigned  goodbad;
         unsigned  polarity_known;  // 'spare' in the ashtech docs
         unsigned  ireg;       // SNR in custom units
         unsigned  qa_phase;   // phase quality 0..5 and 95..100 are good
         double    full_phase; // cycles
         double    raw_range;  // seconds
         double    doppler;    // Hz
         double    smoothing;  // meters
         unsigned  smooth_cnt; //

         virtual void decodeASCII(std::stringstream& str)
            throw(std::exception, FFStreamError);
         virtual void decodeBIN(std::string& str)
            throw(std::exception, FFStreamError);
         float snr(float chipRate) const throw();
         void dump(std::ostream& out) const;
      };
         
      // The remaining block is repeated repeated three times for an MPC but
      // only appears once for an MCA
      code_block ca;
      code_block p1;
      code_block p2;
         
      static const char *mpcId, *mcaId;

      virtual std::string getName() const {return "mben";}
      
      virtual bool checkId(std::string hdrId) const
      {return hdrId==mpcId || hdrId==mcaId;}

      void dump(std::ostream& out) const throw();
      virtual void decode(const std::string& data) 
         throw(std::exception, FFStreamError);

   protected:
      virtual void reallyGetRecord(FFStream& ffs)
         throw(std::exception, FFStreamError, EndOfFile);
   };
} // namespace gpstk

#endif
