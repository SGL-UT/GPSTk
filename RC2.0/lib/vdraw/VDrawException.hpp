#pragma ident "$Id$"

/// @file VDrawException.hpp Declare exception class

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


#ifndef VDRAW_EXCEPTION_H
#define VDRAW_EXCEPTION_H

#include <exception>
#include <string>

namespace vdraw
{
  /** \addtogroup BasicVectorGraphics */ 
  //@{

  /**
   * Exception class unique to this library
   */
  class VDrawException: std::exception {

    public:

      /**
       * Constructor.  Uses default message.
       */
      VDrawException() 
        : myExplanation("Unspecified exception has occured")
      {}

      /**
       * Constructor.  Programmer-defined exception string.
       */
      VDrawException(const std::string& explanation) 
        : myExplanation(explanation)
      {}

      /**
       * Destructor.
       */
      ~VDrawException() throw()
      {}

      /**
       * Accessor to exception string.
       * @return The exception explanation string.
       */
      virtual const char* what() const throw()
      {
        return myExplanation.c_str();
      }

    private:

      // The explanation
      std::string myExplanation;

  }; // class VDrawException

  //@}

} // namespace vdraw

#endif //VDRAW_EXCEPTION_H

