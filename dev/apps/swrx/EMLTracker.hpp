#pragma ident "$Id$"


#ifndef EMLTRACKER_HPP
#define EMLTRACKER_HPP

#include <math.h>
#include <complex>
#include <iostream>
#include <list>

#include "icd_200_constants.hpp"

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

   SimpleCorrelator<double> early, prompt, late;
   double emag, pmag, lmag;

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
