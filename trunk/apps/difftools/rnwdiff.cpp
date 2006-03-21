#pragma ident "$Id $"

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






#include "FileFilterFrame.hpp"

#include "RinexNavData.hpp"
#include "RinexNavStream.hpp"
#include "RinexNavFilterOperators.hpp"

#include "DiffFrame.hpp"

using namespace std;
using namespace gpstk;

class RNWDiff : public DiffFrame
{
public:
   RNWDiff(char* arg0)
         : DiffFrame(arg0, 
                     std::string("RINEX Nav"))
   {}

protected:
   virtual void process();
};


void RNWDiff::process()
{
   try
   {
      FileFilterFrame<RinexNavStream, RinexNavData> ff1(inputFileOption.getValue()[0]);
      FileFilterFrame<RinexNavStream, RinexNavData> ff2(inputFileOption.getValue()[1]);
      
      ff1.sort(RinexNavDataOperatorLessThanFull());
      ff2.sort(RinexNavDataOperatorLessThanFull());
      
      pair< list<RinexNavData>, list<RinexNavData> > difflist = 
         ff1.diff(ff2, RinexNavDataOperatorLessThanFull());
      
      if (difflist.first.empty() && difflist.second.empty())
         exit(0);

      list<RinexNavData>::iterator firstitr = difflist.first.begin();
      while (firstitr != difflist.first.end())
      {
         bool matched = false;
         list<RinexNavData>::iterator seconditr = difflist.second.begin();
         while ((!matched) && (seconditr != difflist.second.end()))
         {
               // this will match the exact same nav message in both
               // files, not just the same ephemeris broadcasted at
               // different times.
            if ((firstitr->time == seconditr->time) &&
                (firstitr->PRNID == seconditr->PRNID) &&
                (firstitr->HOWtime == seconditr->HOWtime) )
            {
               cout << fixed << setw(3) << firstitr->time.DOYday() << ' ' 
                    << setw(10) << setprecision(0)
                    << firstitr->time.DOYsecond() << ' ' 
                    << setw(19) << setprecision(12) << scientific
                    << (firstitr->af0      - seconditr->af0) << ' '
                    << (firstitr->af1      - seconditr->af1) << ' '
                    << (firstitr->af2      - seconditr->af2) << ' '
                    << (firstitr->IODE     - seconditr->IODE) << ' '
                    << (firstitr->Crs      - seconditr->Crs) << ' '
                    << (firstitr->dn       - seconditr->dn) << ' '
                    << (firstitr->M0       - seconditr->M0) << ' '
                    << (firstitr->Cuc      - seconditr->Cuc) << ' '
                    << (firstitr->ecc      - seconditr->ecc) << ' '
                    << (firstitr->Cus      - seconditr->Cus) << ' '
                    << (firstitr->Ahalf    - seconditr->Ahalf) << ' '
                    << (firstitr->Toe      - seconditr->Toe) << ' '
                    << (firstitr->Cic      - seconditr->Cic) << ' '
                    << (firstitr->OMEGA0   - seconditr->OMEGA0) << ' '
                    << (firstitr->Cis      - seconditr->Cis) << ' '
                    << (firstitr->i0       - seconditr->i0) << ' '
                    << (firstitr->Crc      - seconditr->Crc) << ' '
                    << (firstitr->w        - seconditr->w) << ' '
                    << (firstitr->OMEGAdot - seconditr->OMEGAdot) << ' '
                    << (firstitr->idot     - seconditr->idot) << ' '
                    << (firstitr->codeflgs - seconditr->codeflgs) << ' '
                    << (firstitr->weeknum  - seconditr->weeknum) << ' '
                    << (firstitr->L2Pdata  - seconditr->L2Pdata) << ' '
                    << (firstitr->accuracy - seconditr->accuracy) << ' '
                    << (firstitr->health   - seconditr->health) << ' '
                    << (firstitr->Tgd      - seconditr->Tgd) << ' '
                    << (firstitr->IODC     - seconditr->IODC) << ' '
                    << (firstitr->HOWtime  - seconditr->HOWtime) << ' '
                    << (firstitr->fitint   - seconditr->fitint)
                    << endl;

               firstitr = difflist.first.erase(firstitr);
               seconditr = difflist.second.erase(seconditr);
               matched = true;
            }
            else
               seconditr++;
         }
         
         if (!matched)
            firstitr++;
      }

      list<RinexNavData>::iterator itr = difflist.first.begin();
      while (itr != difflist.first.end())
      {
         (*itr).dump(cout << '<');
         itr++;
      }

      cout << endl;

      itr = difflist.second.begin();
      while (itr != difflist.second.end())
      {
         (*itr).dump(cout << '>');
         itr++;
      }

   }
   catch(Exception& e)
   {
      cout << e << endl
           << endl
           << "Terminating.." << endl;
   }
   catch(exception& e)
   {
      cout << e.what() << endl
           << endl
           << "Terminating.." << endl;
   }
   catch(...)
   {
      cout << "Unknown exception... terminating..." << endl;
   }
}

int main(int argc, char* argv[])
{
   try
   {
      RNWDiff m(argv[0]);
      if (!m.initialize(argc, argv))
         return 0;
      if (!m.run())
         return 1;
      
      return 0;
   }
   catch(Exception& e)
   {
      cout << e << endl;
   }
   catch(exception& e)
   {
      cout << e.what() << endl;
   }
   catch(...)
   {
      cout << "unknown error" << endl;
   }
   return 1;
}
