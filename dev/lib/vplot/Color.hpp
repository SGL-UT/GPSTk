
/// @file Color.cpp Defines colors. Class declarations.

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


#ifndef VPLOT_COLOR_H
#define VPLOT_COLOR_H

#include<string>
#include<list>

namespace vplot
{
   /** \addtogroup BasicVectorGraphics */ 
   //@{

   /**
    * This class defines a color. The color can be defined using a variety
    * of constructors.
    */
   class Color {

   public:
 
     /**
      * @defgroup PredefinedColors Predefined Color Constants
      * These are hexadecimal constants representing common
      * and some uncommon colors.
      */
      //@{
      ///\ct FF0000\endct
      static const long RED          = 0xFF0000; 
      ///\ct FFA500\endct
      static const long ORANGE       = 0xFFA500;
      ///\ct FFFF00\endct
      static const long YELLOW       = 0xFFFF00;
      ///\ct 00FF00\endct
      static const long GREEN        = 0x00FF00;
      ///\ct 0000FF\endct
      static const long BLUE         = 0x0000FF;
      ///\ct EE82EE\endct
      static const long VIOLET       = 0xEE82EE;
      ///\ct FFFFFF\endct
      static const long WHITE        = 0xFFFFFF;
      ///\ct A5252A\endct
      static const long BROWN        = 0xA5252A;
      ///\ct 000000\endct
      static const long BLACK        = 0x000000;
      ///\ct BEBEBE\endct
      static const long GREY         = 0xBEBEBE;
      ///\ct BEBEBE\endct
      static const long GRAY         = 0xBEBEBE;
      ///\ct FFC0CB\endct
      static const long PINK         = 0xFFC0CB;
      ///\ct 00FFFF\endct
      static const long CYAN         = 0x00FFFF;
      ///\ct 6BBE23\endct
      static const long OLIVE        = 0x6B8E23;
      ///\ct BDB76B\endct
      static const long KHAKI        = 0xBDB76B;
      ///\ct 87CEEB\endct
      static const long SKY_BLUE     = 0x87CEEB;
      ///\ct 40E0D0\endct
      static const long TURQUOISE    = 0x40E0D0;
      ///\ct 7FFF00\endct
      static const long CHARTREUSE   = 0x7FFF00; 
      ///\ct FF00FF\endct
      static const long MAGENTA      = 0xFF00FF;
      ///\ct B03060\endct
      static const long MAROON       = 0xB03060;
      ///\ct CD5500\endct
      static const long BURNT_ORANGE = 0xCD5500;
      ///\ct 9C0001\endct
      static const long CARDINAL     = 0x9C0001; // The color, NOT the bird...
      ///\ct 000080\endct
      static const long NAVY         = 0x000080;
      ///\ct 228B22\endct
      static const long FOREST_GREEN = 0x228B22;
      ///\ct 800080\endct
      static const long DARK_PURPLE  = 0x800080;
      //@}

      /**
       * This constant can be used to declare something as "invisible."  Its primary
       * use is for creating a type of line that shall only be displayed as points
       * using a marker.  Thus these points are each separated, not connected by a line.
       */
      static const long CLEAR        = -1;

      /**
       * Color default is black.
       */
      Color(void) : rgb(BLACK) {};

      /**
       * Color initialized using a single value in three bytes, 0xRRGGBB. The first
       * byte is red, the next is green, the last is blue. For example, 
       * 0x00FF00 is pure green.
       */
      Color(long rgbDef)
      {
        if(rgbDef != CLEAR)
          rgbDef &= 0xFFFFFF;
        rgb = rgbDef;
      };

      /**
       * Color initialized using 3 integers describing (from 0-255) the red, green, and
       * blue components of the color.
       */
      Color(int red, int green, int blue) 
      {
         if( red < 0 ) red = 0;
         else if( red > 0xFF ) red = 0xFF;
         if( green < 0 ) green = 0;
         else if( green > 0xFF ) green = 0xFF;
         if( blue < 0 ) blue = 0;
         else if( blue > 0xFF ) blue = 0xFF;
         rgb = (red << 16) | (green << 8) | blue;
      };

      /**
       * Color initialized using a single string. 
       * \see Color::setToString for a list of valid strings. 
       * @param colorName String description of what this color should end up
       * as.
       */
      Color(const std::string& colorName) { setToString(colorName); }; 

      /// Operator ==
      bool operator==(const Color& rhs) {return (rgb==rhs.rgb);};

      /// Operator !=
      bool operator!=(const Color& rhs) {return (rgb!=rhs.rgb);};

     /**
      *  Uses a string to set the value of the color. Here are the values
      *  \li "red" or "r" 
      *  \li "orange" or "o" 
      *  \li "yellow" or "y"
      *  \li "green" or "g"
      *  \li "blue" or "b" 
      *  \li "violet" or "v"
      *  \li "white" or "w" 
      *  \li "brown" or "k" 
      *  \li "black" or "n"
      *  \li "grey" or "gray" 
      *  \li "forest green" or "forest"
      *  \li "pink" 
      *  \li "cyan"
      *  \li "olive"
      *  \li "khaki"
      *  \li "sky blue"
      *  \li "turquoise"
      *  \li "magenta"
      *  \li "maroon"
      *  \li "burnt orange"
      *  \li "cardinal" 
      *  \li "clear"
      *  \li "navy"
      *  \li "dark purple"
      */
     void setToString( const std::string& str);

     /**
      * Returns a long that can be manipulated to display the hex equivalent of a color.
      * @return A long containing the 0xRRGGBB equivalent of the color.
      */
     long getRGB( void ) const { return rgb; };

     /** 
      * Sets input shorts to their appropriate color value (0-255).
      * @param[out] red short to contain the red color value
      * @param[out] green short to contain the green color value
      * @param[out] blue short to contain the blue color value
      */
     void getRGBTriplet( short& red, short& green, short& blue ) const;

     /**
      * Sets input doubles to their appropriate color value (0-1).  
      * Where 1 is equivalent to 255 or the max amount of that color.
      * @param[out] red double to contain the red color value
      * @param[out] green double to contain the green color value
      * @param[out] blue double to contain the blue color value
      */
     void getRGBTripletFractional( double& red, double& green, 
				   double& blue ) const;

     /**
      * Sets the color to the appropriate color value given by input variables.
      * @param red short containing the red color value (0-255)
      * @param green short containing the green color value (0-255)
      * @param blue short containing the blue color value (0-255)
      */
     void setRGBTriplet( short red, short green, short blue );

     /**
      * Sets the color to the appropriate color value given by input variables.
      * @param red short containing the red color value (0-1)
      * @param green short containing the green color value (0-1)
      * @param blue short containing the blue color value (0-1)
      */
     void setRGBTripletFractional( double red, double green, double blue );

     /**
      * Used to determine if this color is clear
      * @return A boolean denoting that this color is clear(true) or not clear(false).
      */
     bool isClear( void ) const { return rgb == CLEAR; }
    
   protected:

      /// Color as an RGB value
      long rgb;

   private:

   }; // class Color

   //@}

} // namespace vplot

#endif //VPLOT_COLOR_H

