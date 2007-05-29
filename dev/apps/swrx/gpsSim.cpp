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

/**
   A simple GPS signal simulator. Based upon code from Johnathan York.
  
   See http://sglwiki/cgi-bin/twiki/view/SGL/SwRxSignalPathDesign for the
   derivation of the several of the values below.

   Needs to support the following modes to support receiver development:
 	carrier w/o noise
	carrier w noise
	C/A w/o noise
	C/A w noise
	C/A + nav + noise
	P + nav + noise
	P + C/A + nav + noise
	L1 + L2 P, C/A + nav + noise
*/

#include <math.h>
#include <complex>
#include <iostream>
#include <list>

#include "BasicFramework.hpp"
#include "CommandOption.hpp"
#include "StringUtils.hpp"
#include "icd_200_constants.hpp"

#include "SVSource.hpp"
#include "normal.hpp"
#include "IQStream.hpp"

using namespace gpstk;
using namespace std;

class GpsSim : public BasicFramework
{
public:
   GpsSim() throw();

   bool initialize(int argc, char *argv[]) throw();

   // Base frequency of receiver (in Hz)
   const double RX_BASE_FREQ;

   // How many samples are taken in one base period
   const int RX_SAMPLE_RATE_MULTIPLIER;

   // The multipliers applied to the base frequency to get the local
   // oscilator for the mixers.
   const int RX_L1_LO_MULTIPLIER;
   const int RX_L2_LO_MULTIPLIER;

   // Number of local oscilators
   const int LO_COUNT;

   // The units of these are counts of the ADC
   double noise_amplitude, p_amplitude, ca_amplitude;

   // A gain to apply to the 'final' signal
   double gain;

   // number of seconds between samples
   double time_step;

   // One period is a tick of the RX_BASE_FREQ clock
   unsigned long periods_to_generate;

   bool codeOnly;

   list<SVSource*> sv_sources;

   vector<double> omega_lo; 

   double freqErr;

   IQStream *output;
   
protected:
   virtual void process();
};


GpsSim::GpsSim() throw() :
   BasicFramework("gpsSim", "A simple simulation of a the GPS signal."),
   periods_to_generate(4096),
   LO_COUNT(2),
   RX_BASE_FREQ(1.0e6),
   RX_SAMPLE_RATE_MULTIPLIER(20),
   RX_L1_LO_MULTIPLIER(1575),
   RX_L2_LO_MULTIPLIER(1228),
   time_step(1.0/(RX_BASE_FREQ*RX_SAMPLE_RATE_MULTIPLIER)),
   p_amplitude(0.1250*M_SQRT2),
   ca_amplitude(0.1767*M_SQRT2),
   noise_amplitude(2.805),
   codeOnly(false),
   gain(1)
{}


bool GpsSim::initialize(int argc, char *argv[]) throw()
{
   CommandOptionWithAnyArg
      codeOpt('c', "code",
              "Generate the signal defined by the argument. ARG takes "
              "the form of code:carrier:prn:offset:doppler:nav. Code is either "
              "c, p, or cp. Carrier is either 1 or 2. Prn is an integer between 1 "
              "and 32. Offset is a number in microseconds. Doppler is a number "
              "in Hz. Nav is 0, c, p, or cp. For example, to generate C/A on L1 for "
              "PRN 15 with zero time-offset, zero doppler, a dummy nav signal "
              "on both C/A and P, then specify -c c:1:15:0:p.  This option "
              "may be repeated to simulate multiple signals.");

   CommandOptionNoArg
      codeOnlyOpt('\0', "code-only",
                  "Only generate the codes. No carrier, no hetrodyning.");

   CommandOptionWithAnyArg
      quantizationOpt('q', "quantization",
                      "What type of IQ stream; 1, 2 or f. The default is f.");

   CommandOptionWithAnyArg
      gainOpt('g', "gain",
              "Gain to apply to the if prior to digitization, in dB. Default is 0.");

   CommandOptionWithAnyArg
      noiseGainOpt('n', "noise",
                   "Specify the gain (in dB) for the receiver's noise. The "
                   "default is 0 dB");

   CommandOptionWithAnyArg
      freqErrOpt('f', "freq-err",
                 "Specify the frequency error on the local receiver's "
                 "oscilator, in ppm. The default is 0 ppm");

   CommandOptionWithAnyArg
      runTimeOpt('t', "run-time",
                 "How long (in ms) to run the simulation for. The default is 20 ms");

   CommandOptionWithAnyArg
      outputOpt('o', "output",
                 "Where to write the output. The default is stdout");

   if (!BasicFramework::initialize(argc,argv)) 
      return false;

   using namespace gpstk::StringUtils;

   char quantization='f';   
   if (quantizationOpt.getCount())
      quantization = quantizationOpt.getValue()[0][0];

   switch (quantization)
   {
      case '1': output = new IQ1Stream(); break;
      case '2': output = new IQ2Stream(); break;
      case 'f':
      default:  output = new IQFloatStream(); break;
   }

   if (outputOpt.getCount())
   {
      output->open(outputOpt.getValue()[0].c_str(), ios::out);
   }
   else
   {
      output->copyfmt(std::cout);
      output->clear(std::cout.rdstate());
      output->std::basic_ios<char>::rdbuf(std::cout.rdbuf());
      output->filename = "<stdout>";
   }
   if (debugLevel)
      cout << "Writing data to " << output->filename << endl;

   if (gainOpt.getCount())
   {
      double gainDb = StringUtils::asDouble(gainOpt.getValue()[0]);
      gain = exp10(gainDb/10);
      if (debugLevel)
         cout << "# Rx gain level: " << gainDb << " db ("
              << gain << ")" << endl;
   }

   if (noiseGainOpt.getCount())
   {
      double gain = StringUtils::asDouble(noiseGainOpt.getValue()[0]);
      noise_amplitude *= exp10(gain/10);
      if (debugLevel)
         cout << "# Noise level: " << gain << " db" << endl;
   }

   if (codeOnlyOpt.getCount())
      codeOnly = true;

   if (freqErrOpt.getCount())
      freqErr = StringUtils::asDouble(freqErrOpt.getValue()[0]) * 1e-6;
   else
      freqErr = 0;

   if (debugLevel)
      cout << "# Receiver frequency error: " << freqErr * 1e6 << " ppm" << endl;

   if (runTimeOpt.getCount())
   {
      double rt = asDouble(runTimeOpt.getValue()[0]);
      periods_to_generate = static_cast<long unsigned>(rt*RX_BASE_FREQ);
   }

   if (debugLevel)
      cout << "# Running for : " << periods_to_generate
           << " periods (" << 1e3 * periods_to_generate/RX_BASE_FREQ
           << " msec)" << endl;

   // Compute the local oscilator frequencies, units seem to be radians/sample
   omega_lo.resize(LO_COUNT);
   omega_lo[0] = 2.0*PI*RX_L1_LO_MULTIPLIER/RX_SAMPLE_RATE_MULTIPLIER;
   omega_lo[1] = 2.0*PI*RX_L2_LO_MULTIPLIER/RX_SAMPLE_RATE_MULTIPLIER;
   
   vector<double> lo(LO_COUNT);
   lo[0] = omega_lo[0]/time_step / 2 /PI;
   lo[1] = omega_lo[1]/time_step / 2 /PI;

   if (debugLevel)
      cout << "# LO 1: " << setw(7) << lo[0] * 1e-6<< " MHz"
           <<    "  2: " << setw(7) << lo[1] * 1e-6<< " MHz"
           <<  endl
           << "# IF 1: " << setw(7) << (L1_FREQ - lo[0]) * 1e-3<< " kHz"
           <<    "  2: " << setw(7) << (L2_FREQ - lo[1]) * 1e-3<< " kHz"
           <<  endl;

   for (int i=0; i < codeOpt.getCount(); i++)
   {
      string val=codeOpt.getValue()[i];
      const char delim(':');
      if (numWords(val, delim) != 6)
      {
         cout << "Error in code parameter:" << val << endl;
         exit(-1);
      }

      string code =   lowerCase(word(val, 0, delim));
      int    band =       asInt(word(val, 1, delim));
      int    prn =        asInt(word(val, 2, delim));
      double offset =  asDouble(word(val, 3, delim));
      double doppler = asDouble(word(val, 4, delim));
      string nav =    lowerCase(word(val, 5, delim));

      if (code.size() > 2 || nav.size() > 2 || band <1 || band > LO_COUNT ||
          prn < 1 || prn >32)
      {
         cout << "code:" << code << " band:" << band << " prn:" << prn
              << " offset:" << offset << " nav:" << nav << endl
              << "Error in specifying code. Ignoring this code." << endl;
         exit(-1);
      }

      // This is the number of P code chips in one sample. If this
      // is not around or less than 1/2, we have a problem
      double sampleRate = 1/(RX_SAMPLE_RATE_MULTIPLIER*RX_BASE_FREQ); //sec
      double chips_per_sample_base = gpstk::PY_CHIP_FREQ * sampleRate;
      
      switch(band)
      {
         case 1: doppler *= sampleRate / L1_MULT; break;
         case 2: doppler *= sampleRate / L2_MULT; break;
      }

      doppler *= 1-freqErr;
      chips_per_sample_base *= 1-freqErr;

      // Create and configure the SV source
      SVSource* src = new SVSource(prn, band);
      src->zchips_per_sample = chips_per_sample_base;
      src->doppler = doppler;
      src->ca_modulation = isLike(code, "c");
      src->p_modulation = isLike(code, "p");
      src->ca_nav = isLike(nav, "c") && src->ca_modulation;
      src->p_nav = isLike(nav, "p") && src->p_modulation;
      src->p_amplitude = p_amplitude;
      src->ca_amplitude = ca_amplitude;
      // offset needs to be provided to the SVSource in units of P chips
      src->slewZChipFraction(offset * gpstk::PY_CHIP_FREQ * 1e-6);

      if (codeOnlyOpt.getCount())
         src->code_only = true;

      if (debugLevel)
         src->dump(cout);
         
      sv_sources.push_back(src);
   }

   return true;
}

void GpsSim::process()
{
   // Used to accumulate up the samples
   complex<float> accum[LO_COUNT];

   // simulation time, in seconds
   double rx_time = 0;

   for(int period=0; period<periods_to_generate; ++period)
   {
      for(int sample=0; sample<RX_SAMPLE_RATE_MULTIPLIER; ++sample)
      {
         rx_time += time_step;

         // Clear out our accumulators
         for (int i=0; i < LO_COUNT; i++)
            accum[i] = (0,0);

         // Sum the signals from each SV
         list<SVSource*>::iterator i;
         for(i = sv_sources.begin(); i != sv_sources.end(); i++)
         {
            SVSource* src = *i;
            int band=src->band-1;
            accum[band] += src->getSample();
            src->incrementState();
         }

         // For each local oscilator frequency...
         for (int i=0; i<LO_COUNT; i++)
         {
            // Compute the local oscilator and the sample noise
            complex<double> lo = sincos(omega_lo[i] * sample);
            complex<double> noise(generate_normal_rv()*noise_amplitude,
                                  generate_normal_rv()*noise_amplitude);
            
            // Heterodyne the signals
            if (!codeOnly)
               accum[i] *= conj(lo);

            // and add the noise
            accum[i] += noise;

            // Apply receiver gain
            accum[i] *= gain;

            // And output the samples
            *output << accum[i];
         }         
      }
   }
}

int main(int argc, char *argv[])
{
   try
   {
      GpsSim crap;
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
