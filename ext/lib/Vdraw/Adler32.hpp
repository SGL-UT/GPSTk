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

/// @file Adler32.hpp Used to calculate an Adler-32 checksum. Class declarations.

#ifndef VDRAW_ALDER32_H
#define VDRAW_ALDER32_H

#include <string>
#include <memory>

//http://en.wikipedia.org/wiki/Adler-32

namespace vdraw
{
      /// @ingroup BasicVectorGraphics
      //@{

      /**
       * This class is used to generate an Alder-32 checksum for a
       * sequence of bytes.
       */
   class Adler32
   {
   protected:
         /// Largest prime less that 2^16
      static const unsigned int mod = 65521;

         /// The current sum.
      unsigned int a;

         /// The current a sum.
      unsigned int b;

   public:
         /**
          * Default constructor.
          */
      Adler32()
      {
         reset();
      }


         /**
          * Resets the Adler32 to initial value.
          */
      inline void reset()
      {
         a=1; b=0;
      }

         /**
          * Returns the Adler32 value.
          *
          * @return The current checksum value.
          */
      inline unsigned int getValue() const
      {
         return (b << 16) | a;
      }

         /**
          * Updates the Adler32 with specified array of bytes.
          *
          * @param  str  The string to update the Adler32 with.
          */
      void update(const std::string &str)
      {
         update(str.c_str(),str.length());
      }

         /**
          * Updates the Adler32 with specified array of bytes.
          *
          * @param  str  The string to update the Adler32 with.
          */
      void update(const std::auto_ptr<std::string> &str)
      {
         update(str->c_str(),str->length());
      }

         /**
          * Updates the Adler32 with specified array of bytes.
          *
          * @param  buf  The byte array to update the Adler32 with.
          * @param  len  The number of bytes to use for the update.
          */
      void update(const char* buf, unsigned int len);

   }; // class Adler32

      //@}

} // namespace vdraw

#endif //VDRAW_ALDER32_H
