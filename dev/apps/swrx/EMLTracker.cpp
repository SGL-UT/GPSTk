#pragma ident "$Id$"

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
   pllError(0), pllAlpha(0), pllBeta(0),
   dllError(0), dllAlpha(0), dllBeta(0),
   iadCount(0), nav(false), baseGain(1.0/(0.1767*1.404)),
   inSumSq(0), lrSumSq(0),
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

   iadThreshold = 0.034;

   // Walk the code by the correlator spacing
   searchSize = eplSpacing * localReplica.tickSize/localReplica.codeChipLen;

   // 
   iadCountMax = static_cast<unsigned long>(
      localReplica.codeGenPtr->getSyncIndex() / localReplica.chipsPerTick);
   iadCountDefault = iadCountMax;

   while (iadCountMax * localReplica.tickSize > 2e-3)
      iadCountMax /=2;
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

   snr= 10*log10(pmag*pmag/localReplica.tickSize);

   dllError = lmag - emag;
   pllError = atan(prompt().imag() / prompt().real()) / PI;

   // Do we have any idea where the peak may lie?
   if (min(emag,lmag) > iadThreshold/2 && pmag > max(emag,lmag))
      dllMode = dmOnTop;
   else if (emag > iadThreshold || pmag > iadThreshold || lmag > iadThreshold)
      dllMode = dmClose;
   else
      dllMode = dmFar;

   // At this point all that is left on the inphase is the nav data
   nav = prompt().real() > 0;

   // Close the loop on the dll
   if (dllMode == dmOnTop || dllMode == dmClose)
   {
      localReplica.moveCodePhase(dllAlpha * dllError);
      localReplica.codeFreqOffset = dllBeta * dllError;
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

   if (dllMode == dmOnTop && pllError < 0.25)
      pllMode = pmLocked;
   else
      pllMode = pmUnlocked;

   // Determine how many ticks until we hit the sync index again
   CodeIndex sync = localReplica.codeGenPtr->getSyncIndex();
   CodeIndex indx = localReplica.codeGenPtr->getIndex();
   indx %= sync;
   unsigned chips = sync - indx;
   iadCountMax = static_cast<unsigned long>(chips / localReplica.chipsPerTick);
   while (iadCountMax < iadCountDefault/2)
      iadCountMax += iadCountDefault;
}


double EMLTracker::getPromptPhase() const
{
   double q=prompt().imag();
   double i=prompt().real();
   return atan2(q, i)/PI;
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
        << "#" << endl;
      if (debugLevel>1)
         s << "#h time   pMag   codePO   codeFO    pllErr  carrPO  carrFO  nav code  cp" << endl
           << "#u ms      %       us      mHz       cyc     cyc      Hz     -  chip  --";
   }

   if (detail==0)
   {
      s << left << fixed 
        << setprecision(1) << setw(8) << localReplica.localTime * 1e3
        << setprecision(2) << right
        << " " << setw(5) << getPmag() * 100
        << setprecision(3)
        << " " << setw(8) << localReplica.getCodePhaseOffsetSec() * 1e6
        << " " << setw(8) << localReplica.getCodeFreqOffsetHz() * 1e3
        << "   "
        << setprecision(3)
        << " " << setw(6) << getPllError()
        << setprecision(1)
        << " " << setw(7) << localReplica.carrierPhaseOffset
        << setprecision(2)
        << " " << setw(7) << localReplica.getCarrierFreqOffsetHz()
        << setprecision(0)
        << "   " << getNav()
        << "  " << setw(4) << localReplica.codeGenPtr->getIndex()
        << "  " << dllMode << pllMode;
   }

   if (false)
      s << fixed << right
        << setprecision(4)
        << " " << setw(6) << emag
        << " " << setw(6) << pmag
        << " " << setw(6) << lmag;

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
