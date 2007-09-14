
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


#ifndef VPLOT_PSIMAGEBASE_H
#define VPLOT_PSIMAGEBASE_H

#include <string>
#include <iostream>
#include <fstream>

#include "VPlotException.hpp"
#include "VGImage.hpp"
#include "StrokeStyle.hpp"

namespace vplot
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

      void text (const Text& text);

   protected:

      /// The stream to write to.
      std::ostream& ostr;
      /// When writing to a file, this is used to set ostr.
      std::ofstream myfstream;

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

} // namespace vplot
#endif //VPLOT_PSIMAGEBASE_H

