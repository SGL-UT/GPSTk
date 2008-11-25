#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/Comment.hpp#1 $"

/// @file Comment.hpp Defines a simple comment. Class declarations.

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


#ifndef VDRAW_COMMENT_H
#define VDRAW_COMMENT_H

#include <string>
#include <stdarg.h>

namespace vdraw
{
   /** \addtogroup BasicVectorGraphics */ 
   //@{

   /**
    * This class defines a simple comment.  These can be useful
    * for developers to make comments in the output file for 
    * any extra information that might need to be included.
    */
   class Comment {

   public:

      /// Created string
      std::string str;

     /**
      * Constructor. Defines the comment using printf style
      * @param format format string
      * @param ... other optional arguments
      */
      Comment(const char *format,...);

     /**
      * Constructor. Defines the comment using printf style
      * @param format format string
      * @param ap va_list of other optional arguments
      */
      Comment(const char *format, va_list ap);


   protected:

   private:

   void comment_init(const char * format, va_list ap);

   int my_vsnprintf(char * str, size_t size, const char* format, va_list ap);

            
   }; // class Comment

   //@}

} // namespace vdraw

#endif //VDRAW_COMMENT_H


