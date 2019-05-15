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

///@file EPSImage.hpp Vector plotting in Encapsulated Postscript. Class declarations.

#ifndef VDRAW_EPSIMAGE_H
#define VDRAW_EPSIMAGE_H
#include <string>
#include "PSImageBase.hpp"
#include "ViewerManager.hpp"

namespace vdraw
{
   /// @ingroup BasicVectorGraphics  
   //@{

   /**
    * This class generate a vector image in the  Encapsulated Postscript (EPS)
    * file format version 3.0. This is by definition a window on a single page. EPS is a  
    * form of postscript that can be imported by most software programs,
    * from Word to LATEX. The file format is defined by Adobe. The format definition,
    * dated 1 May 1992, is freely available from that company.
    */
   class EPSImage : public PSImageBase {

   public:

     /**
      * Create a EPS output stream attached to an existing stream.
      * @param stream Stream to insert EPS graphic elements
      * @param llx Lower left x coordinate of the EPS canvas in points
      * @param lly Lower left y coordinate of the EPS canvas in points
      * @param urx Upper right x coordinate of the EPS canvas in points
      * @param ury Upper right y coordinate of the EPS canvas in points
      * @param iloc Location of the origin the drawer will use.
      */
      EPSImage(std::ostream& stream, double llx, double lly,
                double urx, double ury,
                ORIGIN_LOCATION iloc=LOWER_LEFT);


     /**
      * Create a EPS output stream attached to an existing stream.
      * @param fname  Name of the output file to be created
      * @param llx Lower left x coordinate of the EPS canvas in points
      * @param lly Lower left y coordinate of the EPS canvas in points
      * @param urx Upper right x coordinate of the EPS canvas in points
      * @param ury Upper right y coordinate of the EPS canvas in points
      * @param iloc Location of the origin the drawer will use.
      */
      EPSImage(const char* fname, double llx, double lly,
                double urx, double ury,
                ORIGIN_LOCATION iloc=LOWER_LEFT);

      /// Destructor.
      ~EPSImage(void);

      /// Launch a viewer
      void view (void) throw (VDrawException);

      /// This member manages how viewer(s) are launched. 
      ViewerManager viewerManager;

   protected:

   private:

      /* Location of the lower left and upper right corners.
       * Units are points. 72 points = one inch. 28 points = 1 cm.
      */
      double llx, lly, urx, ury;

      virtual void outputHeader(void);
      virtual void outputFooter(void);
   }; // class EPSImage

   //@} group BasicVectorGraphics

} // end namespace vdraw

#endif //VDRAW_EPSIMAGE_H
