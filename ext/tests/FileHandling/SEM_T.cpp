//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public
//                            release, distribution is unlimited.
//
//==============================================================================


/*********************************************************************
*
*  Test program to exercise SEMBase, SEMData, and SEMStream.
*
// *********************************************************************/
#include <iostream>
#include <fstream>

#include "SEMData.hpp"
#include "SEMStream.hpp"
#include "SEMHeader.hpp"
#include "SEMBase.hpp"

#include "build_config.h"
#include "TestUtil.hpp"

using namespace std;
using namespace gpstk;

class SEM_T
{
public:
   unsigned roundTripTest();
};

unsigned SEM_T :: roundTripTest()
{
   TUDEF("SEMData", "operator<<");
   try
   {
         // read infn, write what should be identical output to outfn
         // write almanac data to almfn, which should be the same as almexp
      string infn(getPathData() + getFileSep() + "test_input_sem387.txt");
      string outfn(getPathTestTemp() + getFileSep() + "test_output_sem387.out");
      string almfn(getPathTestTemp() + getFileSep() + "test_output_SEM_T.out");
      string almexp(getPathData() + getFileSep() + "test_output_SEM_T.exp");
      SEMStream instr(infn.c_str());
      SEMStream outstr(outfn.c_str(), ios::out);
      SEMStream almstr(almfn.c_str(), ios::out);
      SEMHeader hdr;
      SEMData data;
      TUASSERT(static_cast<bool>(instr));
      TUASSERT(static_cast<bool>(outstr));
      TUCATCH(instr >> hdr);
      TUCATCH(outstr << hdr);
      while (instr >> data)
      {
         outstr << data;
         OrbAlmGen oag = OrbAlmGen(data);
         oag.dump(almstr);
      }
      instr.close();
      outstr.close();
      almstr.close();
      TUCMPFILE(infn, outfn, 0);
      TUCMPFILE(almexp, almfn, 0);
   }
   catch (gpstk::Exception& exc)
   {
      cerr << exc << endl;
      TUFAIL("Unexpected exception");
   }
   catch (std::exception& exc)
   {
      TUFAIL("Unexpected exception: " + string(exc.what()));
   }
   catch (...)
   {
      TUFAIL("Unknown exception");
   }

   TURETURN();
}


int main( int argc, char * argv[] )
{
   unsigned errorTotal = 0;
   SEM_T testClass;

   errorTotal += testClass.roundTripTest();

   std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal
             << std::endl;

   return errorTotal;
}
