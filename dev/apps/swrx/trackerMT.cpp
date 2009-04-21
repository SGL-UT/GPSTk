#pragma ident "$Id$"


/*
g++ -c -o trackerMT.o -O -I. -I/.../gpstk/dev/apps/swrx -I/.../gpstk/dev/src trackerMT.cpp

g++ -o trackerMT trackerMT.o /.../gpstk/dev/apps/swrx/simlib.a /.../gpstk/dev/src/libgpstk.a -lm -lstdc++ -lfftw3 -lm -lpthread
*/


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
  The first cut at a parallel tracker for multiple PRNs. 
*/

#include <math.h>
#include <complex>
#include <iostream>
#include <list>
#include <pthread.h>

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

struct Buffer // input buffer
{
   vector< complex<float> > arr;
}; 

struct Par // Parameters to pass to Pthread function.
{
   int dp;
   int bufferSize;
   EMLTracker *tr;
   Buffer *s;
   int *count;
   NavFramer *nf;
   bool v;
};

void *Cfunction(void*); // C-style function to be called with pthreads

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class RxSim : public BasicFramework
{
public:
   RxSim() throw();

   bool initialize(int argc, char *argv[]) throw();
   void function(EMLTracker *tr, int dp, int *count, 
                     complex<double> s, NavFramer *nf);

protected:
   virtual void process();

private:
   CCReplica* cc;
   vector<EMLTracker*> tr;
   int band;
   double gain;
   bool fakeL2;
   int sat; 

   double timeStep; // Time between samples
   double interFreq; // Intermediate frequency from receive

   double timeLimit;
   IQStream *input;
   unsigned iadMax;
   int numTrackers;
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
   if (sampleRateOpt.getCount())
         timeStep = 1/(asDouble(sampleRateOpt.getValue().front()) * 1e6 );

   if (interFreqOpt.getCount())
      interFreq = asDouble(interFreqOpt.getValue().front()) * 1e6;

   numTrackers = codeOpt.getCount();
   for (int i=0; i < codeOpt.getCount(); i++)
   {
      string val=codeOpt.getValue()[i];
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

      tr[i] = new EMLTracker(*cc, spacing);

      if (dllAlphaOpt.getCount())
         tr[i]->dllAlpha = asDouble(dllAlphaOpt.getValue()[0]);

      if (dllBetaOpt.getCount())
         tr[i]->dllBeta = asDouble(dllBetaOpt.getValue()[0]);

      if (pllAlphaOpt.getCount())
         tr[i]->pllAlpha = asDouble(pllAlphaOpt.getValue()[0]);

      if (pllBetaOpt.getCount())
         tr[i]->pllBeta = asDouble(pllBetaOpt.getValue()[0]);

      tr[i]->prn = prn;
      tr[i]->debugLevel = debugLevel;
      
      if(verboseLevel)
         tr[i]->dump(cout, 1);
   }

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
   }

   return true;
}


//-----------------------------------------------------------------------------
void RxSim::process()
{
   pthread_t *thread_id = new pthread_t[numTrackers];
   pthread_attr_t attr;
   int rc;
   void *status;

   vector<Par> p(numTrackers);

   vector<NavFramer> nf(numTrackers);
   long int dataPoint =0;
   vector<int> count(numTrackers); 
   
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   for(int i=0;i<numTrackers;i++)
   {
      nf[i].debugLevel = debugLevel;
      nf[i].dump(cout);
      count[i]=0;
   }

   complex<float> s;
   while (*input >> s)
   {
      Buffer b;
      b.arr.push_back(s);
      dataPoint++;
      int index = 0;
      int bufferSize = 40*16367;
      while(index < bufferSize) // Fill input buffer
      {   
         *input >> s;
         b.arr.push_back(s);   
         index++;
         dataPoint++;
      }
      for(int i = 0; i < numTrackers; i++)
      {
         p[i].dp = dataPoint; // Set parameters for each tracker.
         p[i].bufferSize = bufferSize;
         p[i].s = &b;
         p[i].count = &count[i];
         p[i].tr = tr[i];
         p[i].nf = &nf[i];
         p[i].v = (verboseLevel);
         
   // Split
         rc = pthread_create( &thread_id[i], &attr, Cfunction, &p[i] ) ;
         if (rc)
         {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
         }
      }

   // Join
      for(int i = 0; i < numTrackers; i++)
      {
         rc = pthread_join( thread_id[i], &status) ;
         if (rc)
         {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
         }
      }
      if (cc->localTime > timeLimit)
         break;
   }
   delete thread_id;
   pthread_attr_destroy(&attr);
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

void *Cfunction(void* p)
{
   Par *par = (Par*)p;
   
   EMLTracker *tr = par->tr;
   int *count = par->count;
   NavFramer *nf = par->nf;
   int bufferSize = par->bufferSize;
   int dp = par->dp - bufferSize;
   Buffer *b = par->s;
   bool v = par->v;
   
   int index = 0; 
   
   while(index < bufferSize + 1) // number of data points to track before join.
   {
      if (tr->process(b->arr[index]))
      {
         if(v)
            tr->dump(cout);
         
         if(tr->navChange)
         {
            nf->process(*tr, dp, 
                     (float)tr->localReplica.getCodePhaseOffsetSec()*1e6);
            *count = 0;
         }
         if(*count == 20)
         // The *20* depends on the tracker updating every C/A period.
         {
            *count = 0;
            nf->process(*tr, dp, 
                     (float)tr->localReplica.getCodePhaseOffsetSec()*1e6);
         }
         
         *count = *count + 1;
      }
      index++;
      dp++;
   } 
   pthread_exit(NULL);
   return NULL;
}
