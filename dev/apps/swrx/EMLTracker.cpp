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

#include "EMLTracker.hpp"

using namespace gpstk;
using namespace std;

/// param localReplica the code/carrier that this object is to track
/// param codeSpacing the correlator spacing (in sec) that will be used for 
/// the code. This class will quantize this value to the closest number
/// of ticks.
EMLTracker::EMLTracker(CCReplica& localReplica, double codeSpacing) :
   GenericTracker(localReplica),
   ticksPerChip(static_cast<unsigned>(1.0/localReplica.chipsPerTick)),
   eplSpacing(static_cast<unsigned>((codeSpacing / localReplica.tickSize))),
   pllError(0), pllAlpha(0.2), pllBeta(0.05),
   dllError(0), dllAlpha(6), dllBeta(0.01),
   iadCount(0), nav(false), baseGain(1.0/(0.1767*1.404)),
   inSumSq(0), lrSumSq(0),iadThreshold(0.02),
   dllMode(dmFar), pllMode(pmUnlocked)
{
   early.setDelay(2*eplSpacing);
   prompt.setDelay(eplSpacing);
   late.setDelay(0);

   // Since our 'prompt' code is really a late code we should really advance 
   // our local replica by this amount but not have it count as part of our
   // code phase offset.
   correlatorBias = eplSpacing * localReplica.chipsPerTick;
   localReplica.moveCodePhase(correlatorBias);
   localReplica.codePhaseOffset -= correlatorBias;

   // Walk the code by the correlator spacing
   searchSize = eplSpacing * localReplica.tickSize/localReplica.codeChipLen;

   iadCountMax = static_cast<unsigned long>(
      localReplica.codeGenPtr->getSyncIndex() / localReplica.chipsPerTick);
   iadCountDefault = iadCountMax;
};


bool EMLTracker::process(complex<double> in)
{
   integrate(in);
   
   if (++iadCount == iadCountMax)
   {
      updateLoop();
      // and dump our accumulators
      early.dump();
      prompt.dump();
      late.dump();
      inSumSq = 0;
      lrSumSq = 0;
      iadCount=0;
      return true;
   }

   return false;
}


void EMLTracker::integrate(complex<double> in)
{
   localReplica.tick();
      
   // First bring the signal level of the input to the same as the
   // local replicas
   in *= baseGain;

   complex<double> carrier = localReplica.getCarrier();
   complex<double> code = localReplica.getCode() ? plusOne : minusOne;

   // mix in the carrier local replica
   complex<double> m0 = in * conj(carrier);

   // and sum them up.. (yea, the conj of the codes should be a NoOp)
   code = conj(code);
   early.process(m0, code);
   prompt.process(m0, code);
   late.process(m0, code);

   // Update our sums for normailzing things...
   complex<double> lr = conj(carrier) * code;
   inSumSq += in.real()*in.real() + in.imag()*in.imag();
   lrSumSq += lr.real()*lr.real() + lr.imag()*lr.imag();
}


void EMLTracker::updateLoop()
{
   sqrtSumSq = sqrt(inSumSq*lrSumSq);

   emag = abs(early()) / sqrtSumSq;
   pmag = abs(prompt()) / sqrtSumSq;
   lmag = abs(late()) / sqrtSumSq;

   pI = prompt().real();
   pQ = prompt().imag();

   snr= 10*log10(pmag*pmag/localReplica.tickSize);

   dllError = lmag - emag;
   pllError = atan(prompt().imag() / prompt().real()) / PI;

   promptPhase =atan2(prompt().imag(), prompt().real()) / PI;

   DllMode oldDllMode=dllMode;
   // Do we have any idea where the peak may lie?
   if (//min(emag,lmag) > iadThreshold/2 &&
      pmag > iadThreshold &&
      pmag > max(emag,lmag))
      dllMode = dmOnTop;
   else if (emag > iadThreshold || pmag > iadThreshold || lmag > iadThreshold)
      dllMode = dmClose;
   else
      dllMode = dmFar;

   if (dllMode != oldDllMode && debugLevel)
      cout << "# t:"
           << fixed << setprecision(1) << localReplica.localTime * 1e3
           << " ms, dll:" << asString(dllMode) << endl;

   // At this point all that is left on the inphase is the nav data
   nav = prompt().real() > 0;

   // Close the loop on the dll
   if (dllMode == dmOnTop || dllMode == dmClose)
   {
      localReplica.moveCodePhase(dllAlpha * dllError);
      localReplica.codeFreqOffset += dllBeta * dllError/iadCount;
   }
   else
   {
      localReplica.moveCodePhase(searchSize);
   }

   // Close up the pll
   if (dllMode == dmOnTop)
   {
      localReplica.moveCarrierPhase(pllAlpha * pllError);
      localReplica.carrierFreqOffset += pllBeta * pllError/iadCount;
   }

   if (dllMode == dmOnTop && pllError < 0.3)
      pllMode = pmLocked;
   else
      pllMode = pmUnlocked;

   // Determine how many ticks until we hit the sync index again
   iadCountPrev = iadCountMax;
   CodeIndex sync = localReplica.codeGenPtr->getSyncIndex();
   CodeIndex indx = localReplica.codeGenPtr->getIndex() % sync;
   unsigned chips =sync - indx;
   iadCountMax = static_cast<unsigned long>(chips / localReplica.chipsPerTick);
   if (iadCountMax < 10000)
      iadCountMax += 20000;
}


void EMLTracker::dump(std::ostream& s, int detail) const
{
   if (detail>0)
   {
      localReplica.dump(s);
      s << "# Tracker:" << endl
        << "# -- bias: " << correlatorBias * localReplica.codeChipLen * 1e6 << " us" << endl
        << "# -- spacing: " << eplSpacing << " tick(s)" << endl
        << "# -- iadThreshold: " << iadThreshold << endl
        << "# -- ticksPerChip: " << ticksPerChip << endl
        << "# -- iadCountMax: " << iadCountMax << endl
        << "# -- searchSize: " << searchSize << " chips" << endl
        << "# -- dll: alpha=" << dllAlpha << " beta=" << dllBeta << endl
        << "# -- pll: alpha=" << pllAlpha <<  " beta=" << pllBeta << endl
        << "#" << endl
        << "#h time  dllErr    codePO    codeFO pllErr   carrPO   carrFO  nav cp   iad   ely   pmt   lat    pmtI   pmtQ"  << endl
        << "#u ms      %         us        Hz    cyc      cyc       Hz     -  --   cnt    %     %     %     cnt    cnt";
   }

   if (detail==0)
   {
      s << left << fixed 
        << setprecision(1) << setw(8) << localReplica.localTime * 1e3
        << setprecision(2) << right
        << " " << setw(6) << getDllError() * 100
        << setprecision(3)
        << " " << setw(11) << localReplica.getCodePhaseOffsetSec() * 1e6
        << setprecision(2)
        << " " << setw(6) << localReplica.getCodeFreqOffsetHz()
        << " "
        << setprecision(3)
        << " " << setw(6) << getPromptPhase()
        << setprecision(1)
        << " " << setw(8) << localReplica.carrierPhaseOffset
        << setprecision(2)
        << " " << setw(8) << localReplica.getCarrierFreqOffsetHz()
        << setprecision(0)
        << "   " << getNav()
        << "  " << dllMode << pllMode
        << "  " << iadCountPrev;
      if (true) 
         s << setprecision(2)
           << " " << setw(5) << emag * 100
           << " " << setw(5) << pmag * 100
           << " " << setw(5) << lmag * 100;
      if (true)
         s << setprecision(0)
           << " " << setw(6) << pI
           << " " << setw(6) << pQ;
   }

   s << endl;
};


string EMLTracker::asString(DllMode mode) const
{
   switch (mode)
   {
      case dmFar: return "f";
      case dmClose: return "n";
      case dmOnTop: return "o";
   }
   return "?";
};
