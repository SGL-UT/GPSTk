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

/// @file ViewerManager.cpp Launches graphic viewers. Class definitions.

#include <iostream>
#include <stdlib.h>
#include <algorithm>

#include "ViewerManager.hpp"

namespace vdraw
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

   bool ViewerManager::view(const std::string& fileName) throw (VDrawException)
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
  

} // namespace vdraw
