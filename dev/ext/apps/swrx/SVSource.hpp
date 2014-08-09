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

#ifndef SVSOURCE_HPP
#define SVSOURCE_HPP

#include "PCodeGenerator.hpp"
#include "CACodeGenerator.hpp"
#include "NAVCodeGenerator.hpp"

#include <math.h>
#include <complex>
#include <iostream>

#include "complex_math.h"

/*
 *  I normally don't like to put all these meaty functions in an include file,
 *   but I really want the compiler to be able to inline getSample(), handleWrap(), and incrementState()
 *   since they all fall in our inner-most loop. 
 *  How long will it take until compilers can do cross-module optimization?
 */

// A class to generate samples from a single SV
class SVSource
{
public:
   SVSource (int SVPRNID, int bandArg) :
      p_modulation(true),
      ca_modulation(true),
      p_nav(true),
      ca_nav(true),
      ca_amplitude(1),
      p_amplitude(1),
      carrier_amplitude(1),
      doppler(0),
      zchips_per_sample(1),
      zchip_fraction_accum(0),
      carrier_multiplier(0),
      code_only(false),
      p_codegen(SVPRNID),
      ca_codegen(SVPRNID),
      band(bandArg),
      prn(SVPRNID),
      zchip_counter(0)
   {
      switch(band)
      {
         case 1: carrier_multiplier = gpstk::L1_MULT_GPS; break;
         case 2: carrier_multiplier = gpstk::L2_MULT_GPS; break;
      }
   }

   std::complex<double> getSample() const
   {
      double phase = zchip_fraction_accum*carrier_multiplier*2.0*gpstk::PI;
      std::complex<double> carrier = sincos(phase);

      bool nav_bit=(*nav_codegen);
      int p_bit=p_modulation?((*p_codegen)^(p_nav?nav_bit:0)):0;
      int ca_bit=ca_modulation?((*ca_codegen)^(ca_nav?nav_bit:0)):0;

      // Must follow guidelines in Table 3-IV of IS-GPS-200D
      std::complex<double> sample(
         ca_bit?(ca_amplitude):(-ca_amplitude),
         p_bit ?(p_amplitude):(-p_amplitude));
      
      if (!code_only)
         sample *= carrier_amplitude * carrier;

      return sample;
   }

   void incrementState()
   {
      /* Increment internal state to prepare for the next call */
      zchip_fraction_accum += zchips_per_sample + doppler;
      handleWrap();
   }

   void setZChipsPerSample(double val)
   { zchips_per_sample=val; }

   void slewZChipFraction(double val)
   {
      zchip_fraction_accum+=val;
      handleWrap();
   }

   void dump(std::ostream& s) const
   {
      s << "# " << std::setprecision(3)
        << "prn:" << prn << " L" << band
        << " Z:" << zchip_fraction_accum+p_codegen.getIndex() << " chips";
      if (ca_modulation)
         s << " (C/A nav:" << ca_nav << " amp:" << ca_amplitude << ")";
      if (p_modulation)
         s  << " (P nav:" << p_nav << " amp:" << p_amplitude << ")";
      s << std::endl;
      s << "# doppler = " << doppler << " chips/sample" << std::endl
        << "# zchips_per_sample: " << zchips_per_sample << " chips/sample" << std::endl;
   }

private:

   void handleWrap()
   {
      while (zchip_fraction_accum>1.0)
      {
         if(zchip_counter==9)
         {
            if(ca_codegen.isLastInSequence())
            {
               if(ca_epoch_counter==19)
               {
                  ++nav_codegen;
                  ca_epoch_counter=0;            
               } else ++ca_epoch_counter;
            }
            ++ca_codegen;
            zchip_counter=0;
         } 
         else
            ++zchip_counter;

         ++p_codegen; 
         zchip_fraction_accum-=1.0;
      }
   }

public:
   // yea, this is klunky to expose all these but we aren't checking
   // for an invariants so...
   bool p_modulation;
   bool ca_modulation;
   bool p_nav;
   bool ca_nav;

   double ca_amplitude;
   double p_amplitude;
   double carrier_amplitude;

   double doppler;
   double zchips_per_sample;
   double zchip_fraction_accum;

   double carrier_multiplier;
   bool code_only;

   gpstk::PCodeGenerator p_codegen;
   gpstk::CACodeGenerator ca_codegen;
   NAVCodeGenerator nav_codegen;

   int band;
   int prn;
   int zchip_counter;     // Counts 0-9 to tell us when to move to the next C/A chip
   int ca_epoch_counter;  // Counts 0-19 to tell us when to move to the next NAV data bit
};

#endif
