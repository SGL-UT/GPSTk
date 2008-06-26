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

#ifndef CCREPLICA_HPP
#define CCREPLICA_HPP

#include <complex>
#include <iostream>

#include "icd_200_constants.hpp"

#include "CodeGenerator.hpp"

//-----------------------------------------------------------------------------
// This is a intended to generate a local replica of a single code/carrier
//-----------------------------------------------------------------------------
class CCReplica
{
public:

   // Note that this object is responsible for destroying the codeGenPtr
   // object
   CCReplica(double tickSize, double codeFreq, double carrierFreq, gpstk::CodeGenerator* code)
      : tickSize(tickSize),
        codeChipLen(1.0/codeFreq),
        carrierFreq(carrierFreq),
        localTime(0),
        codePhase(0), codePhaseOffset(0), codeFreqOffset(0),
        chipsPerTick(tickSize * codeFreq),
        carrierPhase(0), carrierFreqOffset(0), carrierPhaseOffset(0),
        carrierAccum(0),
        cyclesPerTick(tickSize * carrierFreq),
        codeGenPtr(code)
   {};

   ~CCReplica()
   { if (codeGenPtr) delete codeGenPtr;}

   // This is used to move the local time forward by the specified
   // tick size
   virtual void tick() throw();

   // get the current code & carrier state
   virtual int getCode() {return **codeGenPtr;};  // zero or one
   virtual std::complex<double> getCarrier(); //value between -1 and 1

   // These are used to change the code/carrier by the specified ammount
   virtual void moveCodePhase(double chips) throw();
   virtual void moveCarrierPhase(double cycles) throw();
   virtual void wrapCode();
   virtual void wrapCarrier();

   // Get and set routines that work in engineering units
   virtual void setCodeFreqOffsetHz(double freq);
   virtual double getCodeFreqOffsetHz() const;

   virtual void setCarrierFreqOffsetHz(double freq);
   virtual double getCarrierFreqOffsetHz() const;

   virtual double getCodePhaseOffsetSec() const;

   virtual void reset();

   // These are defining parameters for any code since they are const
   // there is no reason to wrapper them in a call or make them
   // private.
   const double tickSize;       // units: sec
   const double codeChipLen;    // units: sec
   const double carrierFreq;    // units Hz
   const double chipsPerTick;   // units: chips
   const double cyclesPerTick;  // units: cycles

   virtual void dump(std::ostream& s) const;

   // fractional phase of the local code, units are chips
   double codePhase;

   // code time relative to localTime, units of chips
   double codePhaseOffset;

   // The code rate relative to the localTime, units of chips / tick
   double codeFreqOffset;

   // The fractional phase of the local carrier, units are cycles of the IF
   double carrierPhase;

   double carrierPhaseOffset;

   // The carrier frequency offset
   double carrierFreqOffset;

   unsigned long carrierAccum;

   // The local clock, units are in sec
   double localTime;

   gpstk::CodeGenerator* codeGenPtr;
};

#endif
