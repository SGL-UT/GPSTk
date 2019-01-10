//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

#include <math.h>
#include <complex>
#include <map>

#include "BasicFramework.hpp"
#include "CommandOption.hpp"

#include "IQStream.hpp"

using namespace gpstk;
using namespace std;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class IQDump : public BasicFramework
{
public:
   IQDump() throw();
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
   bool initialize(int argc, char *argv[]) throw();
#pragma clang diagnostic pop
protected:
   virtual void process();

private:
   IQStream *input;
   int bands;
   bool stats;
};


//-----------------------------------------------------------------------------
IQDump::IQDump() throw() :
   BasicFramework("rxSim", "A simulation of a gps receiver."),
   bands(2), stats(false)
{}


bool IQDump::initialize(int argc, char *argv[]) throw()
{
   using namespace gpstk::StringUtils;

   CommandOptionWithAnyArg
      inputOpt('i', "input",
               "Where to get the IQ input from. The default is stdin."),

      quantizationOpt('q', "quantization",
                      "What type of IQ stream; 1, 2 or f. The default is f."),

      bandsOpt('b', "bands",
               "The number of complex samples per epoch. The default is 2.");

   CommandOptionNoArg
      statsOpt('s', "stats", "Compute stats on values.");

   if (!BasicFramework::initialize(argc,argv)) 
      return false;

   stats = statsOpt.getCount()>0;

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
   if (debugLevel)
      cout << "Taking input from: " << input->filename << endl
           << "IQStream type: " << input->desc << endl;

   input->debugLevel = debugLevel;

   if (bandsOpt.getCount())
      bands = asInt(bandsOpt.getValue()[0]);

   return true;
}


//-----------------------------------------------------------------------------
void IQDump::process()
{
   complex<float> s;
   int band=1;
   unsigned long N=0;
   typedef map<float, unsigned long> HistMap;
   HistMap hist;
   while (*input >> s)
   {
      if (!stats)
         cout << s.real() << " " << s.imag() << " ";
      else
      {
         if (band==1)
         {
            N++;
            ++hist[s.real()];
            ++hist[s.imag()];
         }
      }
      if (band == bands)
      {
         band=1;
         if (!stats)
            cout << endl;
      }
      else
         band++;
   }

   if (stats)
   {
      N*=2; // I & Q each count as a separate sample
      cout << "# " << N << " samples" << endl
           << "# value  frac" << endl;
      for (HistMap::const_iterator i = hist.begin(); i != hist.end(); i++)
         cout << "# " << setw(4) <<  i->first << " : " << (float)i->second/N << endl;

      unsigned long pos(0),neg(0);
      for (HistMap::const_iterator i = hist.begin(); i != hist.end(); i++)
         if (i->first > 0)
            pos += i->second;
         else
            neg += i->second;

      cout << endl
           << "#   >0 : " << (float)pos/N << endl
           << "#  <=0 : " << (float)neg/N << endl;
   }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   try
   {
      IQDump crap;
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
