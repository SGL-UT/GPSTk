
///@file PSImage.cpp Vector plotting in the Postscript format. Class definitions

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
#include "PSImage.hpp"

namespace vplot 
{
   static char VIEWER_ENV_VAR_NAME[]="VPLOT_PS_VIEWER";

   /**
    * Constructors/Destructors
    */
   PSImage::PSImage(std::ostream& stream, double width, 
				      double height, ORIGIN_LOCATION iloc):
     PSImageBase(stream, width,height,iloc),
     viewerManager(VIEWER_ENV_VAR_NAME)
   {
     outputHeader();
   }


   PSImage::PSImage(const char* fname, double width,
				      double height, ORIGIN_LOCATION iloc):
     PSImageBase(fname, width, height,iloc),
     viewerManager(VIEWER_ENV_VAR_NAME)
   {
     outputHeader();
   }



   PSImage::~PSImage(void)
   {
     if (!footerHasBeenWritten) outputFooter();
   }

   /**
    * Methods
    */

   void PSImage::outputHeader(void)
   {
      using namespace std;

      ostr << "%!" << endl;
      ostr << "%% Created by vplot" << endl;
      ostr << "%%" << endl; 
   }

   void PSImage::outputFooter(void)
   {
      using namespace std;
      ostr << "showpage" << endl;
      footerHasBeenWritten = true;
   }

   /**
    * This function closes the output stream and launches a browser.
    */
   void PSImage::view(void) throw (VPlotException)
   {

      // close up the file's contents
      outputFooter();

      // First flush the file stream.
      ostr.flush();

      // Register viewers in case they haven't been registered.
      viewerManager.registerViewer("ggv");
      viewerManager.registerViewer("ghostview");
      viewerManager.registerViewer("evince");
      viewerManager.registerViewer("kghostview");
      viewerManager.registerViewer("gv");

      // Use the viewerManager
      viewerManager.view(filename);

      return;
   }

} // namespace vplot
