#pragma ident "$Id$"

#include <math.h>
#include <complex>
#include <map>

#include "BasicFramework.hpp"
#include "CommandOption.hpp"

#include <IQStream.hpp>

using namespace gpstk;
using namespace std;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class IQDump : public BasicFramework
{
public:
   IQDump() throw();

   bool initialize(int argc, char *argv[]) throw();

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
      input->copyfmt(std::cin);
      input->clear(std::cin.rdstate());
      input->std::basic_ios<char>::rdbuf(std::cin.rdbuf());
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
