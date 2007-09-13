#pragma ident "$Id: //depot/msn/r5.3/wonky/gpstkplot/lib/draw/ViewerManager.cpp#3 $"

/// @file ViewerManager.cpp Launches graphic viewers. Class definitions.

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

#include <iostream>
#include <stdlib.h>
#include "ViewerManager.hpp"

namespace vplot
{
   ViewerManager::ViewerManager(const std::string& envVar)
   {
      const char* envVarValue = getenv(envVar.c_str());
      if (envVarValue!=NULL)
          viewerList.push_back(envVarValue);
   }

   void ViewerManager::registerViewer(const std::string& viewer)
   {
      using namespace std;
     
      list<string>::iterator it=find(viewerList.begin(), viewerList.end(), viewer);
      if (it==viewerList.end())
          viewerList.push_back(viewer);      
   }

   bool ViewerManager::view(const std::string& fileName) throw (VPlotException)
   {
      using namespace std;
      bool worked=false;

      list<string>::iterator viewer;
      for (viewer=viewerList.begin(); viewer!=viewerList.end(); viewer++)
      {
         std::cout << "Going to launch " << *viewer << std::endl;
         string command = (*viewer) + " " + fileName;
         if (system(command.c_str())==0)
	 {
	    worked=true;
	    break;
         }
         std::cout << "... couldn't execute: " << command << std::endl;
      }

      return worked;
   }
  

} // namespace vplot

