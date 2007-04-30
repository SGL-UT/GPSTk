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

#include <math.h>
#include <complex>
#include <iostream>
#include <list>

#include "BasicFramework.hpp"
#include "CommandOption.hpp"
#include "StringUtils.hpp"
#include "icd_200_constants.hpp"

#include "complex_math.h"
#include "SimpleCorrelator.hpp"
#include "CCReplica.hpp"
#include "CACodeGenerator.hpp"
#include "PCodeGenerator.hpp"

#include "IQStream.hpp"

using namespace gpstk;
using namespace std;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class Corltr : public BasicFramework
{
public:
   Corltr() throw();

   bool initialize(int argc, char *argv[]) throw();

private:
   virtual void process();

   double window; // data window length, in ms

   // number of seconds between samples
   double timeStep;

   CCReplica* cc;

   IQStream *input;

   unsigned steps;

   double offset; // Time to offset the local code by
   double doppler;

   double freqErr;  // A receiver frequency error

   int bands;

   bool peakOnly;
};


//-----------------------------------------------------------------------------
Corltr::Corltr() throw() :
   BasicFramework("corltr", "A program to test out local replica generation and correlation."),
   timeStep(50e-9), bands(2), steps(4092), peakOnly(false)
{}


bool Corltr::initialize(int argc, char *argv[]) throw()
{
   using namespace gpstk::StringUtils;

   CommandOptionWithAnyArg
      windowOpt('w', "window",
                "Specifies how wide a window to use, in us. The default "
                "is 1023 us."),

      timeStepOpt('t', "time-step",
                  "Specifies the nominal interval between samples, in us. "
                  "The default is 0.05 us."),

      quantizationOpt('q', "quantization",
                      "They quantization applied to the data. 1, 2 or f. "
                      "The default is f."),

      codeOpt('c', "code",
              "The code/carrier to track. ARG takes the form of "
              "code:carrier:prn:offset:doppler. Code is either c or p. "
              "Carrier is either 1 or 2. Prn is an integer between 1 and 32. "
              "Offset is a number in us, Doppler is a number in Hz. Currently, "
              "only one signal can be specified. For example, to track P code "
              "on L2 for PRN 3, with no initial time or doppler offset, "
              "specify -c p:2:3:0:0"),

      freqErrOpt('f', "freq-err",
                 "Specify the frequency error on the local receiver's "
                 "oscilator, in ppm. The default is 0 ppm"),

      inputOpt('i', "input", 
               "Where to get the IQ samples from. The default is to use "
               "standard input.");

   CommandOptionWithNumberArg 
      stepsOpt('s', "steps",
               "The number of 1/4 chip steps to take. Defaults to 4092."),

      bandsOpt('b', "bands",
               "The number of complex samples per epoch. The default is 2.");

   CommandOptionNoArg
      peakOnlyOpt('p', "peak",
                  "Only output the peak correlation/snr values.");

   if (!BasicFramework::initialize(argc,argv)) 
      return false;

   peakOnly = peakOnlyOpt.getCount()>0;

   if (timeStepOpt.getCount())
      timeStep = asDouble(timeStepOpt.getValue().front()) * 1e-6;
   else

   if (windowOpt.getCount())
      window = asDouble(windowOpt.getValue().front()) * 1e-6;
   else
      window = 1.023e-3;

   if (freqErrOpt.getCount())
      freqErr = StringUtils::asDouble(freqErrOpt.getValue()[0]) * 1e-6;
   else
      freqErr = 0;

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
   if (verboseLevel)
      cout << "# IQStream type: " << input->desc << endl;

   if (bandsOpt.getCount())
      bands = asInt(bandsOpt.getValue()[0]);

   if (stepsOpt.getCount())
      steps = asInt(stepsOpt.getValue()[0]);

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
   int    band =       asInt(word(val, 1, delim));
   int    prn =        asInt(word(val, 2, delim));
          offset =  asDouble(word(val, 3, delim)) * 1e-6;
          doppler = asDouble(word(val, 4, delim));

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
   cc = new CCReplica(timeStep, chipFreq, interFreq, codeGenPtr);

   return true;
}


//-----------------------------------------------------------------------------
void Corltr::process()
{
   const unsigned windowTicks = static_cast<unsigned>(window / timeStep);
   const unsigned maxSamp=windowTicks+1;
   const double stepSize = cc->codeChipLen/4;
   vector< complex<double> > in(maxSamp);
   unsigned numSamp = 0;
   complex<float> s;
   double sumSq;
   while (*input >> s && numSamp < maxSamp)
   {
      in[numSamp] = s;
      sumSq += s.real()*s.real() + s.imag()*s.imag();
      for (int i=1; i<bands; i++)
         *input >> s;
      numSamp++;
   }

   if (numSamp != maxSamp)
   {
      cout << "Insufficient samples for specified window size. Exiting." << endl;
      exit(-1);
   }

   if (verboseLevel)
      cout << "# numSamp:" << numSamp << endl
           << "# timeStep:" << timeStep * 1e9 << " nsec" << endl
           << "# window:" << windowTicks << " samples" << endl
           << "# doppler:" << doppler << " Hz" << endl
           << "# freqErr:" << freqErr * 1e6 << " ppm" << endl
           << "# offset:" << offset*1e6 << " usec" << endl
           << "# Input sumSq: " << sumSq << endl;

   cc->setCodeFreqOffsetHz(doppler);
   cc->setCarrierFreqOffsetHz(doppler);

   if (verboseLevel)
      cc->dump(cout);

   if (verboseLevel)
      cout << "#h delay sum r snr " << endl
           << "#u us cnt cnt dBc-Hz" << endl;

   double maxSnr=0, maxR=0, maxDelay=0;

   for (int i=0; i<steps; i++)
   {
      double delay = i * stepSize + offset;
      cc->reset();
      cc->moveCodePhase(delay / cc->codeChipLen);
      cc->setCodeFreqOffsetHz(doppler);
      cc->setCarrierFreqOffsetHz(doppler);

      SimpleCorrelator<double> sum;
      double mySumSq=0;
      for (int j=0; j<windowTicks; j++)
      {
         cc->tick();
         complex<double> carrier = cc->getCarrier();
         complex<double> m0 = in[j] * conj(carrier);
         complex<double> code = cc->getCode() ? plusOne : minusOne;
         complex<double> cc = conj(carrier) * conj(code);
         mySumSq += cc.real()*cc.real() + cc.imag()*cc.imag();
         sum.process(m0, conj(code));
      }
      double r = abs(sum()) / sqrt(sumSq)/sqrt(mySumSq);
      double snr= 10*log10(r*r/timeStep);
      if (snr>maxSnr)
      {
         maxSnr = snr;
         maxR=r;
         maxDelay=delay;
      }
      if (!peakOnly)
         cout << setprecision(9) << delay*1e6
              << " " << setprecision(4) << abs(sum())
              << " " << r << " " << snr << endl;
   }
   if (peakOnly)
      cout << setprecision(9) << maxDelay*1e6
           << setprecision(4) << " " << maxR << " " << maxSnr << endl;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   try
   {
      Corltr crap;
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
