#pragma ident "$Id: //depot/msn/r5.3/wonky/gpstkplot/lib/draw/Comment.cpp#2 $"

/// @file Comment.cpp Defines a simple comment. Class definitions.

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

#include "Comment.hpp"
#include <string>
#include <stdarg.h>
#include <stdio.h>

/// This is the default buffer size to avoid unnecessary allocation.  This is
/// meant to be a generally small size that will fit most comments.
#define BUFFER_SIZE 100

namespace vplot
{

   Comment::Comment(const char *format, ...)
   {
      va_list ap;
      va_start(ap,format);
      comment_init(format,ap);
      va_end(ap);
   }

   Comment::Comment(const char *format, va_list ap)
   {
      comment_init(format,ap);
   }

   void Comment::comment_init(const char *format, va_list ap)
   {
      char buffer[BUFFER_SIZE];
      size_t count = 0;
      count = my_vsnprintf(buffer, BUFFER_SIZE, format, ap);
      if( count > BUFFER_SIZE )
      {
         //allocate larger
        char *newbuff = new char[count+1];
        size_t newcount = my_vsnprintf(newbuff,count+1,format,ap);
        str = std::string(newbuff);
        delete[] newbuff;
      }
      else
      {
        str = std::string(buffer);
      }
   }

   //int win_snprintf(char* str, size_t size, const char* format, ...)
   int Comment::my_vsnprintf(char * str, size_t size, const char* format, va_list ap)
   {
     size_t count;
#if defined(WINDOWS) || defined(WIN32) || defined(WIN64) || defined(windows)
     count = vscprintf(format, ap);
     vsnprintf(str, size, _TRUNCATE, format, ap);
#else
     count = vsnprintf(str, size, format, ap);
#endif
     return count;
   }

} // namespace vplot

