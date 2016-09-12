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

/// @file Text.hpp Class to represent text. Class declarations.

#ifndef VDRAW_TEXT_H
#define VDRAW_TEXT_H

#include <string>
#include <stdio.h>

#include "VDrawException.hpp"
#include "TextStyle.hpp"

namespace vdraw
{
      /// @ingroup BasicVectorGraphics
      //@{


      /**
       * This class defines a text object that is to be displayed in an image.
       */
   class Text
   {

   public:

         /// X coordinate of the text.
      double x; 

         /// Y coordinate of the text.
      double y;

         /**
          * Enumeration of alignment constants
          */
      enum ALIGNMENT {LEFT, CENTER, RIGHT};

         /**
          * Constructs text at some coordinate.
          * @param str string to appear as text
          * @param ix x coordinate of the first point
          * @param iy y coordinate of the first point
          * @param align enumerated alignment constant, for which direction
          *   text is aligned with respect to the (x,y) point
          * @param angle angle in degrees the text is to be pointing, starting
          *   at zero, counter-clockwise from ---> (3:00)
          */
      Text(const char* str, double ix, double iy, ALIGNMENT align=LEFT,
           int angle=0);

         /**
          * Constructs text at some coordinate.
          * @param str string to appear as text
          * @param ix x coordinate of the first point
          * @param iy y coordinate of the first point
          * @param its textStyle describing how this text is supposed to look
          * @param align enumerated alignment constant, for which direction
          *   text is aligned with respect to the (x,y) point
          * @param angle angle in degrees the text is to be pointing, starting
          *   at zero, counter-clockwise from ---> (3:00)
          */
      Text(const char* str, double ix, double iy, const TextStyle& its,
           ALIGNMENT align=LEFT, int angle=0);

         /**
          * Constructs text at some coordinate.
          * @param num number to be converted to text
          * @param ix x coordinate of the first point
          * @param iy y coordinate of the first point
          * @param align enumerated alignment constant, for which direction
          *   text is aligned with respect to the (x,y) point
          * @param angle angle in degrees the text is to be pointing, starting
          *   at zero, counter-clockwise from ---> (3:00)
          */
      Text(int num, double ix, double iy, ALIGNMENT align=LEFT, int angle=0);

         /**
          * Constructs text at some coordinate.
          * @param num number to be converted to text
          * @param ix x coordinate of the first point
          * @param iy y coordinate of the first point
          * @param its textStyle describing how this text is supposed to look
          * @param align enumerated alignment constant, for which direction
          *   text is aligned with respect to the (x,y) point
          * @param angle angle in degrees the text is to be pointing, starting
          *   at zero, counter-clockwise from ---> (3:00)
          */
      Text(int num, double ix, double iy, const TextStyle& its,
           ALIGNMENT align=LEFT, int angle=0);


         /**
          * Modifier for the output string of the text.
          * @param str string to appear as text
          */
      inline void setText(const std::string& str)
      { textString = str; }

         /**
          * Modifier for the style of the text.
          * @param its textStyle describing how this text is supposed to look
          */
      inline void setStyle(TextStyle& its)
      { hasOwnStyle = true; textStyle = its; }

         /**
          * Remove the text style
          */
      inline void removeStyle()
      { hasOwnStyle = false; }

         /**
          * Modifier for the position on the image of the text.
          * @param ix x coordinate of the first point
          * @param iy y coordinate of the first point
          */
      inline void setPosition(double ix, double iy)
      { x = ix; y = iy; }

         /**
          * Modifier for the angle of the text.
          * @param align new enumerated alignment constant, for which direction
          *   text is aligned with respect to the (x,y) point
          */
      inline void setAlignment(ALIGNMENT align)
      { textAlign = align; }

         /**
          * Modifier for the angle of the text.
          * @param angle angle in degrees the text is to be pointing, starting
          *   at zero, counter-clockwise from ---> (3:00)
          */
      inline void setAngle(int angle)
      { textAngle = angle; }


         /*
          * Accessors
          */

         /// Accessor to get the text string.
      inline std::string getString(void) const
      { return textString; }

         /// Accessor to see if text as its own style.
      inline bool hasOwnTextStyle(void) const
      { return hasOwnStyle; }

         /// Accessor to get the text style.
      inline TextStyle getStyle(void) const
      { return textStyle; }

         /// Accessor to see if text is centered.
      inline bool isCenter(void) const
      { return textAlign == CENTER; }

         /// Accessor to see if text is aligned anchored left.
      inline bool isLeft(void) const
      { return textAlign == LEFT; }

         /// Accessor to see if text is aligned anchored right.
      inline bool isRight(void) const
      { return textAlign == RIGHT; }

         /// Accessor to get the angle of the text.
      inline int getAngle(void) const
      { return textAngle; }

   protected:

   private:

      std::string textString;
      bool hasOwnStyle;
      TextStyle textStyle;
      ALIGNMENT textAlign;
      int textAngle;

   }; // class Text

      //@}

} // namespace vdraw

#endif //VDRAW_TEXT_H
