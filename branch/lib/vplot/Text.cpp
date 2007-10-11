
/// @file Text.cpp Class to represent text. Class definitions.

#include "Text.hpp"

namespace vplot
{
   /**
    * Constructor
    */
   Text::Text(const char* str, double ix, double iy, ALIGNMENT align, int angle)
             :hasOwnStyle(false)
   { 
      textString = std::string(str);
      x = ix;
      y = iy;
      textAlign = align;
      textAngle = angle;
   }

   Text::Text(const char* str, double ix, double iy, TextStyle& its, ALIGNMENT align, int angle)
             :hasOwnStyle(true), textStyle(its)
   { 
      textString = std::string(str);
      x = ix;
      y = iy;
      textAlign = align;
      textAngle = angle;
   }

   Text::Text(int num, double ix, double iy, ALIGNMENT align, int angle)
             :hasOwnStyle(false)
   { 
      char buffer [33];
      sprintf(buffer,"%d",num);
      textString = std::string(buffer);
      x = ix;
      y = iy;
      textAlign = align;
      textAngle = angle;
   }

   Text::Text(int num, double ix, double iy, TextStyle& its, ALIGNMENT align, int angle)
             :hasOwnStyle(true), textStyle(its)
   { 
      char buffer [33];
      sprintf(buffer,"%d",num);
      textString = std::string(buffer);
      x = ix;
      y = iy;
      textAlign = align;
      textAngle = angle;
   }
   
   /**
    * Modifiers
    */
   void Text::setAngle(int angle)
   {
      textAngle = angle;
   }
   
   void Text::setPosition(double ix, double iy)
   {
      x = ix;
      y = iy;
   }
   
   void Text::setAlignment(ALIGNMENT align)
   {
      textAlign = align;
   }
   
   void Text::setText(const std::string& str)
   {
      textString = str;
   }

   void Text::setStyle(TextStyle& its)
   {
      hasOwnStyle = true;
      textStyle = its;
   }

} // namespace vplot
