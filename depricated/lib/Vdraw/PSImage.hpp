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

///@file PSImage.hpp Vector plotting in the Postscript format. Class declarations.

#ifndef VDRAW_PSIMAGE_H
#define VDRAW_PSIMAGE_H

#include <string>
#include <iostream>
#include <fstream>
#include "ViewerManager.hpp"
#include "PSImageBase.hpp"

namespace vdraw
{
      /// @ingroup BasicVectorGraphics
      //@{

      /**
       * This class generates vector graphics conformant with
       * Postscript Level 2.  Such graphics can be sent directly to a
       * printer.
       */
   class PSImage : public PSImageBase
   {

   public:

         /**
          * Create a Postscript output stream attached to an existing stream.
          * @param stream Stream to insert Postscript graphic elements
          * @param width  Width of the Postscript canvas in points
          * @param height Height of the Postscript canvas in points
          * @param iloc Location of the origin the drawer will use.
          */
      PSImage(std::ostream& stream, 
              double width=US_LETTER_WIDTH_PTS,
              double height=US_LETTER_HEIGHT_PTS,
              ORIGIN_LOCATION iloc=LOWER_LEFT);

         /**
          * Create a Postscript output stream attached to an existing stream.
          * @param fname  Name of the output file to be created
          * @param width  Width of the Postscript canvas in points
          * @param height Height of the Postscript canvas in points
          * @param iloc Location of the origin the drawer will use.
          */
      PSImage(const char* fname, double width=US_LETTER_WIDTH_PTS, 
              double height=US_LETTER_HEIGHT_PTS,
              ORIGIN_LOCATION iloc=LOWER_LEFT);


         /// Destructor.
      ~PSImage(void);

         /// Launch a viewer.
      void view (void) throw (VDrawException);

         /// This member manages how viewer(s) are launched. 
      ViewerManager viewerManager;

   protected:

      virtual void outputHeader(void);
      virtual void outputFooter(void);

   private:



   }; // class PSImage

      //@}

} // namespace vdraw

#endif //VDRAW_PSIMAGE_H
