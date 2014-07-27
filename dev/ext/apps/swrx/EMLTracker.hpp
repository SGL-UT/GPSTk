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

#ifndef EMLTRACKER_HPP
#define EMLTRACKER_HPP

#include <math.h>
#include <complex>
#include <iostream>
#include <list>

#include "GNSSconstants.hpp"

#include "CCReplica.hpp"
#include "SimpleCorrelator.hpp"
#include "complex_math.h"


//-----------------------------------------------------------------------------
// Defines the interface to a tracker. A tracker tracks a single code+carrier
// combination as specified with the CCReplica object. Most of the observables
// will be in the CCReplica object, not the tracker.
//-----------------------------------------------------------------------------
class GenericTracker
{
public:
   GenericTracker(CCReplica& localReplica) :
      localReplica(localReplica)
   {};

   // This needs to be called to process every sample
   // It returns true when a dump was performed
   virtual bool process(std::complex<double> s) = 0;

   CCReplica& localReplica;
};


//-----------------------------------------------------------------------------
// This is a textbook early minus late code tracker and a Costas carrier tracker.
// The code localReplica is used as the early version of the code. This class
// generates a prompt and late from this.
//-----------------------------------------------------------------------------
class EMLTracker : public GenericTracker
{
public:

   /// param localReplica the code/carrier that this object is to track
   /// param codeSpacing the correlator spacing (in sec) that will be used for
   /// the code. This class will quantize this value to the closest number
   /// of ticks.
   EMLTracker(CCReplica& localReplica, double codeSpacing);

   virtual bool process(std::complex<double> in);

   void dump(std::ostream& s, int detail=0) const;

   double pllAlpha, pllBeta, dllAlpha, dllBeta;
   bool navChange;
   int periodCount;
   int prn;

   enum DllMode {dmFar, dmClose, dmOnTop};
   enum PllMode {pmUnlocked, pmLocked};

   int debugLevel;

   std::string asString(DllMode mode) const;

   // Accessors to get the current state of this tracker
   DllMode getDllMode() const {return dllMode;}
   PllMode getPllMode() const {return pllMode;}
   bool getNav() const {return nav;}
   double getPllError() const {return pllError;}
   double getDllError() const {return dllError;}
   double getPmag() const {return pmag;}
   double getPromptPhase() const {return promptPhase;}
   double getSNR() const {return snr;}
   double getSqrtSumSq() const {return sqrtSumSq;}
   unsigned getIntegrateCount() const {return iadCount;}

private:
   void integrate(std::complex<double> in);
   void updateLoop();

   double pllError, dllError, promptPhase;

   DllMode dllMode;
   PllMode pllMode;

   bool nav;
   bool prevNav;


   SimpleCorrelator<double> early, prompt, late;
   double emag, pmag, lmag, pI, pQ;

   // These are used to normalize the correlator counts
   double inSumSq, lrSumSq, sqrtSumSq, snr;

   // Integrate and dump count and limit. When the count equals the max
   // its time to close the loop and then dump the sums.
   unsigned iadCount, iadCountMax, iadCountDefault, iadCountPrev;

   // Used to detect whether the peak is close.
   double iadThreshold;

   const unsigned ticksPerChip;

   // correlator spacing, in units of ticks
   unsigned eplSpacing;

   // How much to walk the code when we aren't on a peak
   double searchSize;

   // A constant bias in the code offset due to the way we generate our
   // early, prompt, and late replicas
   double correlatorBias;

   const double baseGain;
};

#endif
