#pragma ident "$Id$"

/*
FFT based acquisition for GPS L1 band.  (Parallel Code Phase Search).


Example usage:

...$ hilbert | acquire -q 2 -x 4.092 -r 16.368 -b 1 -c 21 -p1 > output.txt

      = 2 bit quantization, 4.092 MHz IF, 16.368 MHz sample rate, one band, PRN 21, one period.

...$ gpsSim -x 1.25 -r 5 -c c:1:21:50:13100:0 -t 4 | acquire -x 1.25 -r 5 -c 21 -p 5 > output1.txt

      = float quantization(default), 2 bands (default), 5 periods.



GCC commands: (not added to Jamfile yet, since this links to FFTW)

g++ -c -o acquire.o -O -I. -I/.../gpstk/dev/apps/swrx -I/.../gpstk/dev/src acquire.cpp

g++ -o acquire acquire.o /.../gpstk/dev/apps/swrx/simlib.a /.../gpstk/dev/src/libgpstk.a -lm -lstdc++ -lfftw3 -lm

*/

#include <math.h>
#include <complex>
#include <iostream>
#include <vector>


#include <fftw3.h>

#include "BasicFramework.hpp"
#include "CommandOption.hpp"
#include "StringUtils.hpp"
#include "icd_200_constants.hpp"

#include "CCReplica.hpp"
#include "CACodeGenerator.hpp"
#include "complex_math.h"

#include "IQStream.hpp"

using namespace gpstk;
using namespace std;

class Acquire : public BasicFramework
{
public:
   Acquire() throw();

   bool initialize(int argc, char*argv[]) throw();
   
private:
   virtual void process();
   
   IQStream *input;
   float sampleRate;
   float interFreq;

   float freqSearchWidth;
   float freqBinWidth;

   int numSamples;
   int prn;
   int bands;
   int periods;
   int bins;
   
   CCReplica* cc;
};

Acquire::Acquire() throw() :
   BasicFramework("acquire", "A program for acquisition of C/A code."),
   sampleRate(20e6),
   interFreq(0.42e6),
   numSamples(sampleRate*1e-3*periods),
   prn(1),
   bands(2),
   periods(1),
   freqSearchWidth(20000),
   freqBinWidth(200),
   bins(freqSearchWidth / freqBinWidth + 1)
{}

//-----------------------------------------------------------------------------
bool Acquire::initialize(int argc, char *argv[]) throw()
{
   using namespace gpstk::StringUtils;

   CommandOptionWithAnyArg
      bandsOpt('b',"bands",
               "The number of complex samples per epoch.  The default is 2. "),
      
      periodsOpt('p',"CA-periods",
                 "The number of C/A periods to consider.  Default is one, "
                 "odd values recommended because of possible NAV change."),

      sampleRateOpt('r',"rate",
                    "Specifies the nominal sample rate, in MHz.  The "
                    "default is 20 MHz."),

      interFreqOpt('x',"inter-freq",
                   "Specifies the intermediate frequency of the receiver,"
                   " in MHz.  Default is 0.42 MHz."),

      quantizationOpt('q', "quantization",
                   "The quantization applied to the data. 1, 2 or f. "
                      "The default is f."),
      
      prnOpt('c',"PRN",
             "The PRN of the code to acquire. Default is 1."),

      inputOpt('i', "input", 
               "Where to get the IQ samples from. The default is to use "
               "standard input."),

      searchWidthOpt('w',"search-width",
                     "Width of the doppler search in Hz. "
                     "For example, 20000 would search from -10000 to 10000 Hz. "
                     "Default is 20000."),

      binWidthOpt('f',"bin-width",
                  "Width of the frequency bins in Hz. Default is 200. "
                  "Bin width should be at most 1000Hz/numPeriods, so "
                  "the default is for up to 5 periods.");
   

   if (!BasicFramework::initialize(argc,argv)) 
      return false;

   if (bandsOpt.getCount())
      bands = asInt(bandsOpt.getValue()[0]);

   if (periodsOpt.getCount())
   {
      periods = asInt(periodsOpt.getValue()[0]);
      numSamples = sampleRate*1e-3*periods;
   }
   
   if (sampleRateOpt.getCount())
   {
      sampleRate = asDouble(sampleRateOpt.getValue().front()) * 1e6;
      numSamples = sampleRate*1e-3*periods;
   }

   if (interFreqOpt.getCount())
      interFreq = asDouble(interFreqOpt.getValue().front()) * 1e6;

   if (prnOpt.getCount())
      prn = asInt(prnOpt.getValue()[0]);

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
   
   if(searchWidthOpt.getCount())
   {
      freqSearchWidth = asDouble(searchWidthOpt.getValue().front());
      bins = freqSearchWidth / freqBinWidth + 1;
   }

   if(binWidthOpt.getCount())
   {
      freqBinWidth = asDouble(binWidthOpt.getValue().front());
      bins = freqSearchWidth / freqBinWidth + 1;
   }
   

   return true;
}

//-----------------------------------------------------------------------------
void Acquire::process()
{   
   CACodeGenerator* codeGenPtr = new CACodeGenerator(prn);
   float chipFreq = gpstk::CA_CHIP_FREQ;

   vector< complex<float> > v(numSamples);
   vector< vector <complex<float> > > l(bins, v); // local code bins
   vector< complex<float> >in(numSamples); // input
   
   // Get input code
   int sample = 0;  
   complex<float> s;
   while (*input >> s && sample < numSamples)
   {
      in[sample] = (s); 
      sample ++;

      for(int i = 1; i < bands; i++)
      {*input >> s;} // gpsSim outputs 2 bands (L1 and L2), one after the other.
         // This program currently supports L1 only, this loop throws away
         // the input from L2, or any other bands.
   }
  

   // Create local code replicas for each frequency bin.
   float f = -(freqSearchWidth/2); // initial doppler offset
   for(int i = 0; i < bins; i++)  
   {
      cc = new CCReplica(1/sampleRate, chipFreq, interFreq+f, codeGenPtr);
      cc->reset();
      for(int k = 0; k < numSamples; k++)
      {
         complex<double> carrier = cc->getCarrier();
         complex<double> code = cc->getCode() ? 1 : -1;
         l[i][k] = (code) * (carrier); 
         cc->tick();
      }
      f += freqBinWidth;
   }
   
   // Convert local code replicas to frequency domain.
   vector< complex<float> > v2(numSamples);
   vector< vector <complex<float> > > L(bins, v2); // Transforms of local codes
   fftw_complex *IN;
   fftw_complex *OUT;
   fftw_plan p; 
   fftw_plan p2;
   IN = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * numSamples);
   OUT = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * numSamples);
   p = fftw_plan_dft_1d(numSamples, IN, OUT, FFTW_FORWARD, FFTW_MEASURE);
   for(int i = 0; i < bins; i++)
   {
      for(int k = 0; k < numSamples; k++)
      {
         IN[k][0] = real(l[i][k]);
         IN[k][1] = imag(l[i][k]); 
      }
      fftw_execute(p);
      for(int k = 0; k < numSamples; k++)
      {
         L[i][k] = (complex<float>(OUT[k][0],OUT[k][1]) / (float)sqrt(numSamples));
      }
   }
                                                                         
   p = fftw_plan_dft_1d(numSamples, IN, OUT, FFTW_FORWARD, FFTW_MEASURE);

   // Convert input code to frequency domain. 
   complex<float>X[numSamples];  // Transform of input code.
   for(int i = 0; i < numSamples; i++)
   {
      IN[i][0] = real(in[i]);
      IN[i][1] = imag(in[i]); 
   }
   fftw_execute(p);
   for(int i = 0; i < numSamples; i++)
   {
      X[i] = (complex<float>(OUT[i][0],OUT[i][1]) / (float)sqrt(numSamples));
   }
   
  
   // Multiply conjugate of input frequency samples by 
   // local frequency samples (point by point).
   for(int i = 0; i < bins; i++)
   {
      for(int k = 0; k < numSamples; k++)
      {
         L[i][k] = L[i][k] * (conj(X[k]));
      }
   }

   float max = 0.0;
   int bin, chip;

   // Convert back to time domain and find peak.
   p2 = fftw_plan_dft_1d(numSamples, IN, OUT, 1, FFTW_MEASURE);
   for(int i = 0; i < bins; i++)
   {
      for(int k = 0; k < numSamples; k++)
      {
         IN[k][0] = real(L[i][k]);
         IN[k][1] = imag(L[i][k]);
      }
      fftw_execute(p2);
      for(int k = 0; k < numSamples; k++)
      {
         l[i][k] = abs(complex<float>(OUT[k][0],OUT[k][1]) / 
                       (float)sqrt(numSamples)); 
         if(real(l[i][k]) > max) 
         {
            max = real(l[i][k]);
            chip = k;
            bin = i;
         }
      }     
   }

   // Adjust code phase value for multiple periods in time domain. 
   while(chip > (sampleRate*1e-3))
   {
      chip = chip - (sampleRate*1e-3);
   }
   
   // Dump Information.
   cout << "PRN: " << prn << " Bin: " << bin << " =  Doppler: " 
        << (bin*1e3)/(1000/freqBinWidth) - (freqSearchWidth/2)
        << " Offset: " << chip*1000/(sampleRate*1e-3)
        << " Height: " << max << endl;

   // Output correlation curve for graphing purposes. 
   for(int k = 0; k < numSamples; k++)
   {
      cout << float((k/16.368)*1.023) << " " << real(l[bin][k]) << endl;
   }

   fftw_destroy_plan(p2);fftw_destroy_plan(p);
   fftw_free(IN); fftw_free(OUT); 
}

//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   try
   {
      Acquire go;
      if(!go.initialize(argc, argv))
         exit(0);
      go.run();
   }
   catch (gpstk::Exception &exc)
   { cerr << exc << endl; }
   catch (std::exception &exc)
   { cerr << "Caught std::exception " << exc.what() << endl; }
   catch (...)
   { cerr << "Caught unknown exception" << endl; }
}
