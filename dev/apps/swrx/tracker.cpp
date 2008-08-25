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

/*
  The first cut at an object-oriented receiver simulator. This is intended
  to accept the output from gpsSim and produce neat stuff.
*/

#include <math.h>
#include <complex>
#include <iostream>
#include <list>

#include "BasicFramework.hpp"
#include "CommandOption.hpp"
#include "StringUtils.hpp"
#include "icd_200_constants.hpp"

#include "EMLTracker.hpp"
#include "CCReplica.hpp"
#include "CACodeGenerator.hpp"
#include "PCodeGenerator.hpp"
#include "complex_math.h"
#include "IQStream.hpp"
#include "NavFramer.hpp"

using namespace gpstk;
using namespace std;

/* exp10() is a gnu specific extensions */
#ifndef exp10
#define exp10(x) (exp((x)*log(10.)))
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class RxSim : public BasicFramework
{
public:
   RxSim() throw();

   bool initialize(int argc, char *argv[]) throw();

protected:
   virtual void process();

private:
   CCReplica* cc;
   EMLTracker* tr;
   int band;
   double gain;
   bool fakeL2;

   double timeStep; // Time between samples
   double interFreq; // Intermediate frequency from receive

   double timeLimit;
   IQStream *input;
   unsigned iadMax;
};


//-----------------------------------------------------------------------------
RxSim::RxSim() throw() :
   BasicFramework("rxSim", "A simulation of a gps receiver."),
   cc(NULL), tr(NULL), band(1), timeStep(50e-9), interFreq(0.42e6), 
   fakeL2(false), gain(1), timeLimit(9e99),iadMax(20460)
{}


bool RxSim::initialize(int argc, char *argv[]) throw()
{
   using namespace gpstk::StringUtils;

   CommandOptionWithAnyArg
      codeOpt('c', "code",
              "The code/carrier to track. ARG takes the form of "
              "code:carrier:prn:offset:doppler. Code is either c or p. "
              "Carrier is either 1 or 2. Prn is an integer between 1 and 32. "
              "Offset is a number in us, Doppler is a number in Hz. Currently, "
              "only one signal can be specified. For example, to track P code "
              "on L2 for PRN 3, with no initial time or doppler offset, "
              "specify -c p:2:3:0:0"),

      dllAlphaOpt('\0', "dllAlpha",
                  "The gain on the phase update for the code tracker. The "
                  "default is 1e-5 chips/tick"),

      dllBetaOpt('\0', "dllBeta",
                 "The gain on the frequency update for the code tracker. The "
                 "default is 1e-12 chips/tick"),

      pllAlphaOpt('\0', "pllAlpha",
                  "The gain on the phase update for the carrier tracker. The "
                  "default is 0.4 cycles/tick"),

      pllBetaOpt('\0', "pllBeta",
                 "The gain on the frequency update for the carrier tracker. "
                 "The default is 0.1 cycles / iad_period"),

      sampleRateOpt('r',"sample-rate",
                    "Specifies the nominal sample rate, in MHz.  The "
                    "default is 20 MHz."),

      interFreqOpt('x',"inter-freq",
                   "Specifies the intermediate frequency of the receiver,"
                   " in MHz.  Default is 0.42 MHz. If there is no down-"
                   "conversion, the IF should be the L1 or L2 carrier"
                   " frequency" ),

      quantizationOpt('q', "quantization",
                      "They quantization applied to the data. 1, 2 or f. "
                      "The default is f."),

      gainOpt('g', "gain",
              "Gain to apply to the if prior to digitization, in dB. Default is 0."),

      timeLimitOpt('t', "time-limit",
                  "Limit the amount of data to process. Specify time in ms. Defaults to all data."),

      inputOpt('i', "input", 
               "Where to get the IQ samples from. The default is to use stdin.");

   CommandOptionWithNumberArg 
      bandsOpt('b', "bands",
               "The number of complex samples per epoch. The default is 2.");

   if (!BasicFramework::initialize(argc,argv)) 
      return false;

   if (timeLimitOpt.getCount())
      timeLimit = asDouble(timeLimitOpt.getValue()[0]) * 1e-3;

   if (!codeOpt.getCount())
   {
      cout << "Must specify a code/carrier to track. Bye." << endl;
      return false;
   }

   string val=codeOpt.getValue()[0];
   const char delim(':');
   if (numWords(val, delim) != 5)
   {
      cout << "Error in code parameter:" << val << endl;
      return false;
   }

   string code =   lowerCase(word(val, 0, delim));
          band =       asInt(word(val, 1, delim));
   int    prn =        asInt(word(val, 2, delim));
   double offset =  asDouble(word(val, 3, delim)) * 1e-6;
   double doppler = asDouble(word(val, 4, delim));

   CodeGenerator* codeGenPtr;
   double chipFreq;
   switch (code[0])
   {
      case 'c':
         codeGenPtr = new CACodeGenerator(prn);
         chipFreq = CA_CHIP_FREQ;
         break;
      case 'p':
         codeGenPtr = new PCodeGenerator(prn);
         chipFreq = PY_CHIP_FREQ;
         break;
      default:
         cout << "Unsupported code: " << code << endl;
         return false;
   }

   if (sampleRateOpt.getCount())
      timeStep = 1/(asDouble(sampleRateOpt.getValue().front()) * 1e6 );

   if (interFreqOpt.getCount())
      interFreq = asDouble(interFreqOpt.getValue().front()) * 1e6;

   // Note that this object is responsible for destroying
   // the codeGenPtr object
   cc = new CCReplica(timeStep, chipFreq, interFreq, codeGenPtr);

   double chips = offset / cc->codeChipLen;
   cc->moveCodePhase(chips);

   cc->setCodeFreqOffsetHz(doppler);
   cc->setCarrierFreqOffsetHz(doppler);

   double spacing = 0.5 * cc->codeChipLen;
   if (spacing < timeStep)
      spacing = timeStep;

   tr = new EMLTracker(*cc, spacing);

   if (dllAlphaOpt.getCount())
      tr->dllAlpha = asDouble(dllAlphaOpt.getValue()[0]);

   if (dllBetaOpt.getCount())
      tr->dllBeta = asDouble(dllBetaOpt.getValue()[0]);

   if (pllAlphaOpt.getCount())
      tr->pllAlpha = asDouble(pllAlphaOpt.getValue()[0]);

   if (pllBetaOpt.getCount())
      tr->pllBeta = asDouble(pllBetaOpt.getValue()[0]);

   tr->debugLevel = debugLevel;

   char quantization='f';   
   if (quantizationOpt.getCount())
      quantization = quantizationOpt.getValue()[0][0];

   switch (quantization)
   {
      case '1': input = new IQ1Stream(); break;
      case '2': input = new IQ2Stream(); break;
      case 'f':
      default:  input = new IQFloatStream(); break;
   }

   if (inputOpt.getCount())
   {
      input->open(inputOpt.getValue()[0].c_str());
   }
   else
   {
      using std::basic_ios;
      input->copyfmt(std::cin);
      input->clear(std::cin.rdstate());
      input->basic_ios<char>::rdbuf(std::cin.rdbuf());
      input->filename = "<stdin>";
   }

   if (bandsOpt.getCount())
      input->bands = asInt(bandsOpt.getValue()[0]);

   if (gainOpt.getCount())
   {
      double gainDb = StringUtils::asDouble(gainOpt.getValue()[0]);
      gain = exp10(gainDb/10.);
   }

   if (verboseLevel)
   {
      cout << "# Taking input from " << input->filename
           << " (" << input->bands << " samples/epoch)" << endl
           << "# Rx gain level: " << gain << endl;
      tr->dump(cout, 1);
   }

   return true;
}


//-----------------------------------------------------------------------------
void RxSim::process()
{
   NavFramer nf;
   long int dataPoint =0;
   nf.debugLevel = debugLevel;
   nf.dump(cout);

   complex<float> s;
   int b=0;
   while (*input >> s)
   {
      if (b == band-1 || input->bands==1)
      {
         s *= gain;
         if (tr->process(s))
         {
            if (verboseLevel)
               tr->dump(cout);
            nf.process(*tr, dataPoint);
         }
      }
      b++;
      b %= input->bands;

      if (cc->localTime > timeLimit)
         break;

      dataPoint++;
         //if(dataPoint > 16367)
         //dataPoint = 0;
   }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   try
   {
      RxSim crap;
      if (!crap.initialize(argc, argv))
         exit(0);
      crap.run();
   }
   catch (gpstk::Exception &exc)
   { cerr << exc << endl; }
   catch (std::exception &exc)
   { cerr << "Caught std::exception " << exc.what() << endl; }
   catch (...)
   { cerr << "Caught unknown exception" << endl; }
}
