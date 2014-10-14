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

///@file EPSImage.cpp Vector plotting in Encapsulated Postscript. Class definitions.

#include <iostream>
#include "EPSImage.hpp"

namespace vdraw
{
   static char VIEWER_ENV_VAR_NAME[]="VDRAW_EPS_VIEWER";

   EPSImage::EPSImage(std::ostream& stream, 
       double illx, double illy, 
       double iurx, double iury,
       ORIGIN_LOCATION iloc):
     PSImageBase(stream, iurx-illx, iury-illy, iloc),
     viewerManager(VIEWER_ENV_VAR_NAME),   
     llx(illx), lly(illy), 
     urx(iurx), ury(iury)
  {
    outputHeader();
  }


   EPSImage::EPSImage(const char* fname, 
       double illx, double illy, 
       double iurx, double iury,
       ORIGIN_LOCATION iloc):
     PSImageBase(fname, iurx-illx, iury-illy, iloc),
     viewerManager(VIEWER_ENV_VAR_NAME),        
     llx(illx), lly(illy), 
     urx(iurx), ury(iury)
  {
    outputHeader();
  }


   EPSImage::~EPSImage(void) 
   {
     // No operations. Not yet.
   }

   /**
    * Methods
    */


   void EPSImage::outputHeader(void)
   {
     using namespace std;
     ostr << "%!PS-Adobe EPSF-3.0" << endl;
     ostr << "%%BoundingBox: " ;
     ostr << llx << " " << lly << " " << urx << " " << ury << endl;
     ostr << "%% Created by vdraw" << endl;
     ostr << "%%" << endl; 
   } 

   void EPSImage::outputFooter(void)
   {
   }

   void EPSImage::view(void) throw (VDrawException)
   {

     // close up the file's contents
     outputFooter();

     // First flush the file stream.
     ostr.flush();

     // Register viewers in case they haven't been registered.
     viewerManager.registerViewer("ggv");
     viewerManager.registerViewer("kghostview --portrait");
     viewerManager.registerViewer("ghostview");
     viewerManager.registerViewer("gv");
     viewerManager.registerViewer("evince");
     viewerManager.registerViewer("gsview32");

     // Use the viewerManager
     viewerManager.view(filename);

     return;
   }


} // namespace vdraw
