
///@file SVGImage.hpp Vector plotting in the SVG format. Class declarations.

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


#ifndef VPLOT_SVGIMAGE_H
#define VPLOT_SVGIMAGE_H

#include <string>
#include <iostream>
#include <fstream>
#include "VGImage.hpp"
#include "ViewerManager.hpp"

namespace vplot
{
   /** \addtogroup BasicVectorGraphics */ 
   //@{

   /**
    * This class generates graphics in Scalable Vector Graphics format.
    * Much of the code is based on examples and explanations from
    * the text "SVG Essentials" by J. David Eisenberg, published by
    * O'Reilly.
    */
   class SVGImage : public VGImage
   {
   public:

     /**
      * Constructors and Destructor
      */

     /**
      * Create an SVG output stream attached to an existing stream.
      * @param stream Stream to insert SVG graphic elements
      * @param width  Width of the SVG canvas in points
      * @param height Height of the SVG canvas in points
      * @param iloc Location of the origin the drawer will use
      */
      SVGImage(std::ostream& stream, double width=US_LETTER_WIDTH_PTS, 
                double height=US_LETTER_HEIGHT_PTS, 
                ORIGIN_LOCATION iloc=LOWER_LEFT);

     /**
      * Create an SVG output file given the file name
      * @param fname  Name of the output file to be created
      * @param width  Width of the SVG canvas in points
      * @param height Height of the SVG canvas in points
      * @param iloc Location of the origin the drawer will use
      */
      SVGImage(const char* fname, double width=US_LETTER_WIDTH_PTS, 
                double height=US_LETTER_HEIGHT_PTS, 
                ORIGIN_LOCATION iloc=LOWER_LEFT);

      /// Destructor.
      ~SVGImage(void);
 

      /**
       * Accessor Methods
       */

      /**
       * Operations
       */
      
      void setMarker( const Marker& newMarker ) 
      {
        VGImage::setMarker(newMarker);
        defineMarker(newMarker);
      }

      void comment(const Comment& comment);

      using VGImage::comment;
      
      void line (const Line& line);

      using VGImage::line;

      void rectangle (const Rectangle& rect);

      void circle (const Circle& circle);

      void text (const Text& text);

      void polygon(const Polygon& polygon);

      void view (void) throw (VPlotException);

      /// Used to define default programs to open this type of file for a
      /// preview.
      ViewerManager viewerManager;

   protected:
     /**
      * Data members
      */
      /// The stream to write to.
      std::ostream& ostr;

      /// When writing to a file, this is used to set ostr.
      std::ofstream myfstream;

      /// Remember the output file name
      std::string filename;

      //using VGImage::StyleType;
      //using VGImage::getCorrectStrokeStyle;
      //using VGImage::getCorrectMarker;
      //using VGImage::getCorrectTextStyle;
      //using VGImage::getCorrectFillColor;

   private:

      /**
       * Datamembers
       */

      // Most drawing elements are in the same group a simple list.
      // This element is used to align them or indent them consistently.
      std::string tab;

      // This determines if a marker of some sort has been defined yet.
      bool markerDefined;

      // This contains the last defined marker.
      Marker lastMarker;

      /**
       * Operations
       */
   
      /// This outputs a consistent SVG header.
      void outputHeader(void);

      /// Close up the markers and grouping
      virtual void outputFooter(void);

      /**
       * Make an SVG style property from a strokestyle
       * @param ss Stroke Style to convert to a string
       * @return An SVG formatted string describing that strokestyle
       */
      std::string convertStrokeStyle (const StrokeStyle& ss) const;

      /**
       * Generates the fill tag for a Fillable object.
       * @param fc Fill Color to use if necessary
       * @param tfc StyleType specification for choosing ret value
       * @return An SVG formatted string describing the fill color
       */
      std::string fill (Color& fc, StyleType tfc) const;

      /**
       * Generates the stroke description.
       * @param ss StrokeStyle to use if necessary
       * @param tss StyleType specification for choosing ret value
       * @param noneclear If tss == NONE then make stroke clear
       * @return An SVG formatted string describing the stroke
       */
      std::string strokeDesc (StrokeStyle& ss, StyleType tss, bool noneclear=false) const;

      /**
       * Make an SVG style property from a fillColor
       * @param color Color to fill
       * @return An SVG formatted string describing the input fill color
       */
      std::string convertFillColor (const Color& color) const;

      /**
       * Make a SVG specification of a marker.
       */
      void defineMarker(const Marker& mark, int dfltnum=0);

      /// Takes the elements of a Path and writes them to an output stream.
      void outputPoints(const Path& path, std::ostream& ostr); 

   }; // class SVGImage

   //@} group BasicVectorGraphics

} // namespace GPSTK

#endif //VPLOT_SVGIMAGE_H
