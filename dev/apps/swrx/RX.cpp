#pragma ident "$Id$"


/*
  First shot at integrating position solutions into the tracking algorithm. 
*/

/*

COMPILE:
g++ -c -o trackerN.o -O -I. -I/home/mdavis/svn/clau/gpstk/dev/apps/swrx -I/home/mdavis/svn/clau/gpstk/dev/src trackerN.cpp

g++ -o trackerN trackerN.o /home/mdavis/svn/clau/gpstk/dev/apps/swrx/simlib.a /home/mdavis/svn/clau/gpstk/dev/src/libgpstk.a -lm -lstdc++ -lfftw3 -lm -lpthread

SAMPLE USAGE:
hilbert -i /home/mdavis/docs/CarData/CarData.bin | ./trackerN -b 1 -q 2 -x 4.13 -r 8.184 -c c:1:30:416.789:-8800 -c c:1:2:410.191:-8300 -c c:1:10:836.144:-9000 -c c:1:15:174.609:-4000 -c c:1:18:255.254:-3500 -c c:1:24:183.284:-5700 -c c:1:26:907.747:-4300 -c c:1:29:355.327:-5400 -p 1 -e rin269.08n -w 1498  
  
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


#include <math.h>
#include <complex>
#include <iostream>
#include <list>
#include <pthread.h>
#include "BasicFramework.hpp"
#include "CommandOption.hpp"
#include "StringUtils.hpp"
#include "icd_200_constants.hpp"
#include "EngNav.hpp"
#include "EMLTracker.hpp"
#include "CCReplica.hpp"
#include "CACodeGenerator.hpp"
#include "PCodeGenerator.hpp"
#include "complex_math.h"
#include "IQStream.hpp"
#include "NavFramer.hpp"
#include <DayTime.hpp>
#include <GPSEphemerisStore.hpp>
#include <RinexNavStream.hpp>
#include <RinexNavData.hpp>
#include <TropModel.hpp>
#include <IonoModel.hpp>
#include <GPSGeoid.hpp>
#include <PRSolution.hpp>

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
   bool v;       // verbose
   int prn;
};

void *Cfunction(void*); // C-style function to be called with pthreads

pthread_mutex_t mutexVec = PTHREAD_MUTEX_INITIALIZER;
int dataPoints[32]; // variables needed by position algorithm
bool edgeFound;
int ZCount;

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

   bool solvePos; // If false, no position solutions will be performed (default)
   string ephFile; // Parameters for position solution.
   int gpsWeek;   // (we don't currently generate our own eph file).
};


//-----------------------------------------------------------------------------
RxSim::RxSim() throw() :
   BasicFramework("rxSim", "A simulation of a gps receiver."),
   cc(NULL), tr(NULL), band(1), timeStep(50e-9), interFreq(0.42e6), 
   fakeL2(false), gain(1), timeLimit(9e99), iadMax(20460), solvePos(false)
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
              "specify -c p:2:3:0:0", true),

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
               "Where to get the IQ samples from. The default is to use stdin."),
      posOpt('p',"prsolve", "Solve for position solution every subframe." 
                  " An ephemeris file and gpsWeek number must also be "
                  "supplied if this option is selected."
             " Any value can be entered, e.g. '-p 7'"),
      ephFileOption('e', "ephemeris", "Rinex Ephemeris data file name "
                    "for position solution."),

      gpsWeekOpt('w',"gps-week",
                "The GPSWeek, used for position solution.")
;

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

   if (posOpt.getCount())
   {
      solvePos = true;
      if(!gpsWeekOpt.getCount() || !ephFileOption.getCount())
      {
         cout << "If solving for position (-p option), you must specify both "
              << "an ephemeris file and the gpsWeek (-e and -w options).  "
              << "Exiting." << endl;
         return false;
      }
      
   }

   if(gpsWeekOpt.getCount())
   {
      gpsWeek = asInt(gpsWeekOpt.getValue().front());
   }   
   if(ephFileOption.getCount())
   {
      ephFile = ephFileOption.getValue()[0];
   }

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
   pthread_t thread_id[numTrackers];
   pthread_attr_t attr;
   int rc;
   void *status;

   vector<Par> p(numTrackers);

   vector<NavFramer> nf(numTrackers);
   long int dataPoint =0;
   vector<int> count(numTrackers); 
   
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

      pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
      edgeFound = false;
      for(int i = 0; i < numTrackers; i++)
      {
         p[i].dp = dataPoint; // Set parameters for each tracker.
         p[i].bufferSize = bufferSize;
         p[i].s = &b;
         p[i].count = &count[i];
         p[i].tr = tr[i];
         p[i].nf = &nf[i];
         p[i].v = (verboseLevel);
         p[i].prn = tr[i]->prn;
         
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
      if(edgeFound == true)
      {         
//---------------------------------------------------------------------------
// Position Solution
         if(solvePos == true)
         {
            GPSEphemerisStore bce;
            IonoModel iono;
            DayTime time;
            double zCount = (double)ZCount - 6.0;
            double sampleRate = 1/timeStep;
            GPSGeoid gm;
            vector<SatID> svVec;
            vector<double> ionoVec;
            Triple antennaPos;

            DayTime t(gpsWeek,zCount);
            time = t; 

            RinexNavStream rns(ephFile.c_str(), ios::in);
            rns.exceptions(ifstream::failbit);
            RinexNavHeader hdr;
            rns >> hdr;
            iono = IonoModel(hdr.ionAlpha, hdr.ionBeta);
            RinexNavData rnd;
            while (rns >> rnd)
               bce.addEphemeris(rnd); // PUT INPUT STUFF UP AT BEGINNING
            if (time < bce.getInitialTime() || time > bce.getFinalTime())
               cout << "Warning: Initial time does not appear to be "
                    << "within the provided ephemeris data." << endl;

            for (int i=1; i < 33; i++)
            {
               SatID sv(i, SatID::systemGPS);
               svVec.push_back(sv);
            }
            float refDataPoint;

            long int total = 0;
            int numberSVs = 0;

            for(int i=0; i<32;i++)
            {
               total += dataPoints[i];
               if(dataPoints[i] != 0)
                  numberSVs++;
            }
            refDataPoint = total/numberSVs;
            vector<double> obsVec(32);


            for(int i=0; i<32; i++)
            {
               if(dataPoints[i] != 0)
               {
                     // 0.073 is an arbitrary guessed time of flight
                  obsVec[i] = gpstk::C_GPS_M*(0.073 - (refDataPoint - 
                       dataPoints[i])/(sampleRate)); //*2 because of hilbert
               }  
               else
               {
                  SatID temp(0, SatID::systemGPS); 
                  svVec[i] = temp; // set SatID equal to 0, 
                                   //the SV won't be considered
               }
            }
// Calculate initial position solution.

            GGTropModel gg;
            gg.setWeather(30., 1000., 50.);    
            PRSolution prSolver;
            prSolver.RMSLimit = 400;
            prSolver.RAIMCompute(time, svVec, obsVec, bce, &gg); 
            Vector<double> sol = prSolver.Solution;
            cout << endl << "Position (ECEF): " << fixed << sol[0] 
                 << " " << sol[1] 
                 << " " << sol[2] << endl 
                 << "Clock Error (includes that caused by guess): " 
                 << sol[3]*1000/gpstk::C_GPS_M << " ms" << endl;
            cout << "# good SV's: " << prSolver.Nsvs << endl
                 << "RMSResidual: " << prSolver.RMSResidual << " meters" 
                 << endl << endl;

            for(int i = 0; i < 32; i++)
               dataPoints[i] = 0;

// Calculate Ionosphere correction.
/*            antennaPos[0] = sol[0];
            antennaPos[1] = sol[1];
            antennaPos[2] = sol[2];
            ECEF ecef(antennaPos);
            for (int i=1; i<=32; i++)
            {
               SatID sv(i, SatID::systemGPS);
               try 
               {
                  Xvt svpos = bce.getXvt(sv, time);
                  double el = antennaPos.elvAngle(svpos.x);
                  double az = antennaPos.azAngle(svpos.x);
                  double ic = iono.getCorrection(time, ecef, el, az); // in meters
                  ionoVec.push_back(ic);
               }
               catch (Exception& e)
               {}
            }
            if(verboseLevel)
            {
               for(int i = 0; i < 32; i++)
               {
                  cout << svVec[i] << " "  << obsVec[i] << " " << ionoVec[i] << endl;
         
               }
            }
            for(int i=0;i<32;i++)
            {
               obsVec[i] -= sol[3]; // convert pseudoranges to ranges
               obsVec[i] += ionoVec[i]; // make iono correction to ranges.
               }*/
         }
      }
//---------------------------------------------------------------------------

      if (cc->localTime > timeLimit)
         break;
   }
   pthread_attr_destroy(&attr);
   pthread_exit(NULL);
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
   int prn = par->prn;
   
   int index = 0;
   pthread_mutex_lock (&mutexVec);
   edgeFound = false;
   pthread_mutex_unlock (&mutexVec);

   while(index < bufferSize + 1) // number of data points to track before join.
   {
      if (tr->process(b->arr[index]))
      {
         if(v)
            tr->dump(cout);
         
         if(tr->navChange)
         {
            if(nf->process(*tr, dp, 
                           (float)tr->localReplica.getCodePhaseOffsetSec()*1e6))
            {
               pthread_mutex_lock (&mutexVec);
               dataPoints[prn-1] = nf->subframes.back().dataPoint;
               edgeFound = true;
               ZCount = EngNav::getHOWTime(nf->subframes.back().words[1]);
               pthread_mutex_unlock (&mutexVec);
            }
            
            *count = 0;
         }
         if(*count == 20)
         // The *20* depends on the EMLtracker updating every C/A period.
         {
            *count = 0;
            if(nf->process(*tr, dp, 
                           (float)tr->localReplica.getCodePhaseOffsetSec()*1e6))
            {
               pthread_mutex_lock (&mutexVec);
               dataPoints[prn-1] = nf->subframes.back().dataPoint;
               edgeFound = true;
               ZCount = EngNav::getHOWTime(nf->subframes.back().words[1]);
               pthread_mutex_unlock (&mutexVec);
            }
         }
         *count = *count + 1;
      }
      index++;
      dp++;
   }

   pthread_exit((void*) 0);
}
