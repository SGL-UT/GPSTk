#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/TextStyle.hpp#2 $"

/// @file TextStyle.hpp Defines how text appears. Class declarations.

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

#ifndef VDRAW_TEXTSTYLE_H
#define VDRAW_TEXTSTYLE_H

#include <string>
#include <iostream>
#include "Color.hpp"

namespace vdraw
{
  /** \addtogroup BasicVectorGraphics */ 
  //@{

  /**
   * This class defines a style of text.  Attributes include the following:
   *   - Font Point Size
   *   - Font Color
   *   - Font Type (Monospace, Sans-serif, or Serif)
   *   - Font style (any combination of the following)
   *      - Bold
   *      - Italic
   *      - Underline
   *      - Strike
   */
  class TextStyle {
    /*
     * Public stuff
     */
    public:
      /*
       * Fields
       */
      /// Constant style defining a normal font.  If you really don't want to
      /// type "0"
      static const short NORMAL    = 0;

      /// Constant style defining bold.
      static const short BOLD      = 1;

      /// Constant style defining italic.
      static const short ITALIC    = 2;

      /// Constant style defining underline.
      static const short UNDERLINE = 4;

      /// Constant style defining strike.
      static const short STRIKE    = 8;

      /// This enumeration contains the simple choices of font.
      enum Font{MONOSPACE, SANSSERIF, SERIF};

      /*
       * Constructors
       */

      /**
       * Constructor. Defines a text style.
       */
      TextStyle(void);

      /**
       * Constructor. Defines a text style.
       * @param ips Font size in points
       * @param istyle Binary representation of text attributes (bold, italics, underline, strike)
       * @see setStyle
       */
      TextStyle(double ips, short istyle=0);

      /**
       * Constructor. Defines a text style.
       * @param ips Font size in points
       * @param istyle Binary representation of text attributes (bold, italics, underline, strike)
       * @param icolor Color of the text
       * @param ifont Font desired
       * @see setStyle
       */
      TextStyle(double ips, short istyle, const Color& icolor, Font ifont=MONOSPACE);

      /**
       * Constructor. Defines a text style.
       * @param ips Font size in points
       * @param icolor Color of the text
       * @param ifont Font desired
       * @see setStyle
       */
      TextStyle(double ips, const Color& icolor, Font ifont=MONOSPACE);

      /*
       * Modifiers
       */

      /**
       * This method does a static conversion from a string to a short for use
       * with the TextStyle class constructors and methods.
       * This string is created using the characters "b" for bold "i" for italics "u" for underline
       * and "s" for strike.  Neither order nor case matters, other characters are ignored. For example:
       * Bold and Italic = "bi" or "Ib" or "BI" or "iB" ...
       * @param str Character representation of text attributes (bold, italics, underline, strike)
       */
      static short style(const char* str);

      /**
       * Modifier. Modifies text attributes vina binary representation in an integer.
       * This is created using the defined BOLD, ITALIC, UNDERLINE, and STRIKE variables and 
       * using a binary OR to put them together.  For example: 
       * Bold and Italic -> int bandi = (TextStyle::BOLD | TextStyle::ITALIC)
       * @param istyle Binary representation of text attributes (bold, italics, underline, strike)
       */
      void setStyle(short istyle);

      /// Set color
      inline void setColor(const Color &c) { color = c; }

      /// Set point size
      inline void setPointSize(double s) { pointSize = s; }

      /*
       * Accessor Methods
       */

      /// Color accessor
      inline Color getColor(void) const { return color; }

      /// Point Size accessor
      inline double getPointSize(void) const { return pointSize; }

      /// Style accessor
      inline bool isBold(void) const { return bold; }

      /// Style accessor
      inline bool isItalic(void) const { return italic; }

      /// Style accessor
      inline bool isUnderline(void) const { return underline; }

      /// Style accessor
      inline bool isStrike(void) const { return strike; }

      /// Font checker
      inline bool isMonospace(void) const { return font == MONOSPACE; }

      /// Font checker
      inline bool isSansSerif(void) const { return font == SANSSERIF; }

      /// Font checker
      inline bool isSerif(void) const { return font == SERIF; }

      /// Style accessor
      inline short getStyle(void) const 
      { return (bold?BOLD:0) & (italic?ITALIC:0) & (underline?UNDERLINE:0) & (strike?STRIKE:0); }

      /// Font accessor
      inline Font getFont(void) const { return font; }

      /// Equals.
      inline bool equals(const TextStyle& other) const
      { 
        return (pointSize == other.getPointSize()) 
          && (getStyle() == other.getStyle()) 
          && (font == other.getFont()); 
      }

      /// Operator ==
      bool operator==(const TextStyle& rhs) const {return this->equals(rhs);};

      /// Operator !=
      bool operator!=(const TextStyle& rhs) const {return !this->equals(rhs);};

      /*  
       * Protected stuff
       */
    protected:


      /*
       * Private stuff
       */
    private:

      /// Enumeration of the Font
      Font font;

      /// Text color as an RGB value
      Color color;

      /// Text point size
      double pointSize;

      /// Bold
      bool bold;

      /// Italics
      bool italic;

      /// Underline
      bool underline;

      /// Strike
      bool strike;

  };

  //@}

}
#endif //VDRAW_TEXTSTYLE_H

