// $Id$
// Edits a FIC file based upon criteria supplied on the command line.
//

#include <iostream>
#include <cmath>
#include <list>

#include "BasicFramework.hpp"
#include "FICStream.hpp"
#include "FICHeader.hpp"
#include "FICData.hpp"
#include "StringUtils.hpp"
#include "CivilTime.hpp"
#include "GPSWeekSecond.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


class FICedit : public BasicFramework
{
public:
   FICedit(const string& applName) throw()
   : BasicFramework(
      applName,
      "Removes specified records from FIC file."),
     inputOpt('i', "input",
              "Input FIC file. Will not be modified.",
              true),
     outputOpt('o', "output",
               "Output FIC file. Will be overwritten if it exists.",
               true),
     killOpt('r', "remove",
             "Specification of what to remove in the form: nnn,ppp,yyyy/ddd/hh:mm:ss. "
             "nnn is the block number, pp is the prn, yyyy/ddd/hh:mm:ss is the HOW time of"
             "the subframe to be removed.",
             true)
   {
      inputOpt.setMaxCount(1);
      outputOpt.setMaxCount(1);
   };
   
   bool initialize(int argc, char *argv[]) throw();
      
protected:
   virtual void process();

private:
   CommandOptionWithAnyArg inputOpt, outputOpt, killOpt;

   struct kill_rec {
      int block;
      int prn;
      CommonTime t;
      bool found;
   };
};


bool FICedit::initialize(int argc, char *argv[]) throw()
{
   if (!BasicFramework::initialize(argc,argv))
      return false;

   return true;
}

void FICedit::process()
{
   list<kill_rec> kills;

   for (int i=0; i<killOpt.getCount(); i++)
   {
      string ks=killOpt.getValue()[i];
      kill_rec kill;
      if (numWords(ks, ',') != 3)
      {
         cout << "Invalid -r specifier:" << ks << endl;
         return;
      }
      
      kill.block = asInt(word(ks, 0, ','));
      kill.prn = asInt(word(ks, 1, ','));
      mixedScanTime(kill.t, word(ks, 2, ','), "%04Y/%03j/%02H:%02M:%02S");
      kill.found = false;
      kills.push_back(kill);
   }

   string ifn = inputOpt.getValue()[0];
   string ofn = outputOpt.getValue()[0];

   FICStream input(ifn.c_str());
   FICStream output(ofn.c_str(), ios::out|ios::binary);

   if (verboseLevel || debugLevel)
   {
      cout << "Reading " << ifn << " writing " << ofn << endl;
      for (list<kill_rec>::const_iterator i=kills.begin(); i != kills.end(); i++)
         cout << "Looking for block " << i->block << " for prn " << i->prn
              << " at " << CivilTime(i->t) << endl;
   }

   FICHeader hdr;
   input >> hdr;
   output << hdr;

   FICData d;
   while (input >> d)
   {
      short week;
      double sow;
      short prn=-1;
      switch (d.blockNum)
      {
         case 9:
            week = (short) d.f[5];
            sow  = d.f[2];
            prn = d.f[19];
            break;
         case 109:
            week = d.i[0];
            sow  = ((d.i[3] & 0x3FFFFFFFL) >> 13) * 6;
            prn =  d.i[1];
            break;
         case 62:
            week = d.i[5];
            sow  = d.i[1];
            break;
         case 162:
            week = d.i[14];
            sow  = ((d.i[2] & 0x3FFFFFFFL) >> 13) * 6;
            break;
         default:
            break;
      }
      CommonTime t(GPSWeekSecond(week,sow));
      t -= 6; // Time in the HOW is for the *next* subframe.
      bool killme=false;
      if (debugLevel>1)
         cout << CivilTime(t) << " " << d.blockNum << " " << prn << endl;
      for (list<kill_rec>::iterator i=kills.begin(); i != kills.end(); i++)
      {
         if (t == i->t && prn == i->prn && d.blockNum == i->block)
         {
            cout << "Found and killed " << CivilTime(t) << " " << d.blockNum << " " << prn << endl;
            i->found = true;
            killme=true;
         }
      }
      if (!killme)
         output << d;
   }

   for (list<kill_rec>::const_iterator i=kills.begin(); i != kills.end(); i++)
      if (!i->found)
         cout << "Did not find " << CivilTime(i->t) << " " << i->block << " " << i->prn << endl;

   input.close();
   output.close();
}

int main(int argc, char *argv[])
{
   FICedit nada(argv[0]);

   if (!nada.initialize(argc, argv))
      exit(0);

   nada.run();
}
