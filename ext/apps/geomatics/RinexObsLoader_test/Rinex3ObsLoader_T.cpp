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

/// @file Rinex3ObsLoader_T.cpp
/// Read Rinex observation files (version 2 or 3)
/// and dump data observations and passes.

// system
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include "StringUtils.hpp"
#include "Exception.hpp"
#include "logstream.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsFileLoader.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
try {
   int iret;

   if(argc <= 1) {
      cout << "Usage: Rinex3ObsLoader_T <rinexfile>" << endl;
      return 1;
   }

   string filename(argv[1]), msg, str;
   cout << "Load file " << filename << endl;

   Rinex3ObsFileLoader rofl(filename);

   if(! rofl.loadObsID("GC1C")) cout << "Can't load GC1C" << endl;
   if(! rofl.loadObsID("GL1C")) cout << "Can't load GL1C" << endl;
   if(! rofl.loadObsID("GC2W")) cout << "Can't load GC2W" << endl;
   if(! rofl.loadObsID("GL2W")) cout << "Can't load GL2W" << endl;
   rofl.saveTheData(true);

   iret = rofl.loadFiles(str,msg);
   if(iret < 0 || !str.empty()) {
      LOG(ERROR) << " Error - Loader failed: returned " << iret
                     << " with message " << str;
   }

   iret = rofl.getStoreSize();
   cout << "Store size is " << iret << endl;

   Rinex3ObsHeader rhead = rofl.getFullHeader(0);
   rhead.dump(cout);

   // have to remove file name - paths conflict
   ostringstream oss;
   oss << rofl.asString();
   str = oss.str();
   str = StringUtils::replaceAll(str,filename,"filename removed, see above");
   cout << str << endl;

   return 0;
}
catch(FFStreamError& e) { cerr << "FFStreamError: " << e.what(); }
catch(Exception& e) { cerr << "Exception: " << e.what(); }
catch (...) { cerr << "Unknown exception.  Abort." << endl; }
   return 9;

}  // end main()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
