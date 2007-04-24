#pragma ident "$Id$"

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

   double timeLimit;
   IQStream *input;
   unsigned iadMax;
};


//-----------------------------------------------------------------------------
RxSim::RxSim() throw() :
   BasicFramework("rxSim", "A simulation of a gps receiver."),
   cc(NULL), tr(NULL), band(1), fakeL2(false), gain(1), timeLimit(20e-3),
   iadMax(20460)
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

      quantizationOpt('q', "quantization",
                      "They quantization applied to the data. 1, 2 or f. "
                      "The default is f."),

      gainOpt('g', "gain",
              "Gain to apply to the if prior to digitization, in dB. Default is 0."),

      timeLimitOpt('t', "time-limit",
                  "Limit the amount of data to process. Specify time in ms. Defaults to 20 ms."),

      inputOpt('i', "input", 
               "Where to get the IQ samples from. The default is to use stdin.");

   CommandOptionWithNumberArg 
      bandsOpt('b', "bands",
               "The number of complex samples per epoch. The default is 2.");

   if (!BasicFramework::initialize(argc,argv)) 
      return false;

   // Basic tick of the reciever, assumed to also be the sample rate
   const double timeStep = 1/20e6;

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

   double interFreq;
   switch (band)
   {
      case 1: interFreq = gpstk::L1_FREQ - 1575e6; break;
      case 2: interFreq = gpstk::L2_FREQ - 1228e6; break;
      default: 
         cout << "Unsupported band: " << band << endl;
         return false;
   }

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
   else
      tr->dllAlpha = 2.3;

   if (dllBetaOpt.getCount())
      tr->dllBeta = asDouble(dllBetaOpt.getValue()[0]);
   else
      tr->dllBeta = 2.3e-7;

   if (pllAlphaOpt.getCount())
      tr->pllAlpha = asDouble(pllAlphaOpt.getValue()[0]);
   else
      tr->pllAlpha = 0.4;

   if (pllBetaOpt.getCount())
      tr->pllBeta = asDouble(pllBetaOpt.getValue()[0]);
   else
      tr->pllBeta = 0.1;

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
      input->copyfmt(std::cin);
      input->clear(std::cin.rdstate());
      input->std::basic_ios<char>::rdbuf(std::cin.rdbuf());
      input->filename = "<stdin>";
   }

   if (bandsOpt.getCount())
      input->bands = asInt(bandsOpt.getValue()[0]);

   if (gainOpt.getCount())
   {
      double gainDb = StringUtils::asDouble(gainOpt.getValue()[0]);
      gain = exp10(gainDb/10);
   }

   if (debugLevel>1)
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
   nf.debugLevel = debugLevel;
   nf.dump(cout);

   complex<float> s;
   int b=0;
   while (*input >> s)
   {
      if (b == band-1)
      {
         s *= gain;
         if (tr->process(s))
         {
            if (debugLevel>1)
               tr->dump(cout);
            nf.process(*tr);
         }
      }
      b++;
      b %= input->bands;

      if (cc->localTime > timeLimit)
         break;
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
