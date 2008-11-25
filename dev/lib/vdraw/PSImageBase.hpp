#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/PSImageBase.hpp#2 $"

///@file PSImageBase.hpp Common functionality between EPS and PS formats. Class declarations.

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


#ifndef VDRAW_PSIMAGEBASE_H
#define VDRAW_PSIMAGEBASE_H

#include <string>
#include <iostream>
#include <fstream>

#include "VDrawException.hpp"
#include "VGImage.hpp"
#include "StrokeStyle.hpp"

namespace vdraw
{
   /** \addtogroup BasicVectorGraphics */ 
   //@{

   /**
    *  This class implements the functionality common between the EPS and
    *  Postscript formats. 
    */
   class PSImageBase : public VGImage {

   public:

     /**
      * Create a Postscript output stream attached to an existing stream.
      * @param stream Stream to insert Postscript graphic elements
      * @param width  Width of the Postscript canvas in points
      * @param height Height of the Postscript canvas in points
      * @param iloc Location of the origin the drawer will use.
      */
      PSImageBase(std::ostream& stream, 
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
      PSImageBase(const char* fname, 
                           double width=US_LETTER_WIDTH_PTS, 
                           double height=US_LETTER_HEIGHT_PTS, 
                           ORIGIN_LOCATION iloc=LOWER_LEFT);

      /// Destructor.
      ~PSImageBase(void);

      void setMarker( const Marker& newMarker ) 
        {VGImage::setMarker(newMarker);defineMarker(newMarker,"dflt");}

      void comment(const Comment& comment);

      using VGImage::comment;

      void line(const Line& line);

      using VGImage::line;

      void rectangle (const Rectangle& rect);

      void circle (const Circle& circle);

      void polygon (const Polygon& polygon);

      void bitmap(const Bitmap& bitmap);      

      void text (const Text& text);

   protected:

      /// When writing to a file, this is used to set ostr.
      std::ofstream myfstream;

      /// The stream to write to.
      std::ostream& ostr;

      /// Remember the output file name
      std::string filename;

      /**
       * Postscript uses one color at a time. It doesn't
       * use separate colors for edges, lines or shapes.
       */
      StrokeStyle lastUsedStrokeStyle;

      /// Last text style used
      TextStyle lastUsedTextStyle;

      using VGImage::StyleType;
      using VGImage::getCorrectStrokeStyle;
      using VGImage::getCorrectMarker;
      using VGImage::getCorrectTextStyle;
      using VGImage::getCorrectFillColor;

   private:

      /// Outputs a new stroke specification if necessary
      void updateStrokeStyle(StrokeStyle& ss);

      /// Prints a stroke specification
      void printStrokeStyle(void);

      /// Prints a text specification
      void printTextStyle(const TextStyle& ts);

      /// Outputs a stroke specification 
      void printStrokeStyle(const StrokeStyle& style);

      /// Writes a Path 
      void outputPath(const Path& path);

      /// Fills in a shape with color
      std::string fillWithColor(const Color& color) const;

      /// For debug purposes
      static const bool extraComments=true;

      /**
       * Make a PS specification of a marker.
       */
      void defineMarker(const Marker& mark, const char * markername=0);

      void markPath(const Path& path, const char * name="mark");


      // This determines if a marker of some sort has been defined yet.
      bool markerDefined;

      // This contains the last defined marker.
      Marker lastMarker;

      /**
       * On initilization., make sure all styles are printed as necessary.
       */
      
      /// Have we output any strokestyle
      bool strokeStylePrinted;

      /// Have we output any textStyle
      bool textStylePrinted;

      /**
       * The following are booleans describing whether or not a definition 
       * has been written to the output file.  Definitions need only be 
       * written once.
       */

       /// Centered text justification definition.
       bool centerDefined;

       /// Right-aligned text justification definition.
       bool rightDefined;


   }; // class PSImageBase

   //@} group BasicVectorGraphics

} // namespace vdraw
#endif //VDRAW_PSIMAGEBASE_H

