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
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
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
*  Test program to exercise YumaBase, YumaData, YumaStream.
*
// *********************************************************************/
// System
#include <iostream>
#include <fstream>

#include "YumaData.hpp"
#include "YumaStream.hpp"
#include "YumaBase.hpp"

#include "build_config.h"
#include "TestUtil.hpp"

using namespace std;
using namespace gpstk;

int main( int argc, char * argv[] )
{
   TUDEF("Yuma_T","readData");

   string origFile("test_input_yuma377.txt");
   string testFile("test_output_yuma377.out");
   string testFile2("test_output_Yuma_T.out");

   std::list<OrbAlmGen> oagList;

      // Read an existing Yuma almanac file and write it back out.
   try
   {
      string fs = getFileSep();
      string df(getPathData()+fs);
      string inFile = df + origFile;

      string tf(getPathTestTemp()+fs);
      string outFile1 = tf + testFile; 

      YumaStream In(inFile.c_str());
      YumaStream Out(outFile1.c_str(), ios::out);
      YumaData Data;

      string outFile2 = tf + testFile2; 
      ofstream outAlmDmp;
      outAlmDmp.open(outFile2.c_str(),ios::out);

      while (In >> Data)
      {
         Out << Data;

         OrbAlmGen oag = OrbAlmGen(Data);
         oag.dump(outAlmDmp);
         oagList.push_back(oag);
      }
      In.close();
      Out.close();
   
      TUCSM("RereadData");
      YumaStream In2(outFile1.c_str(), ios::in);
      std::list<OrbAlmGen>::const_iterator cit = oagList.begin(); 
      int index = 0; 
      while (In2 >> Data)
      {
         OrbAlmGen oag2 = OrbAlmGen(Data);
         const OrbAlmGen& oagRef = *cit;
         bool test = oag2.isSameData(&(oagRef));
         TUASSERTE(bool,test,true);
         index++;
         cit++;
      }
      outAlmDmp.close();
    }
   catch(gpstk::Exception& e)
   {
      stringstream ss;
      ss << e;
      TUFAIL(ss.str());
   }
   catch (...)
   {
      stringstream ss;
      ss << "unknown error.  Done.";
      TUFAIL(ss.str());
   }
   TURETURN();
}
