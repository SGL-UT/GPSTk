#pragma ident "$Id$"

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

/// @file Comment.hpp Defines a simple comment. Class declarations.

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


