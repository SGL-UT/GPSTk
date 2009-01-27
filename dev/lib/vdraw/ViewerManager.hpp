#pragma ident "$Id$"

/// @file ViewerManager.hpp Launches graphic viewers. Class declarations.

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


#ifndef VDRAW_VIEWER_MANAGER_H
#define VDRAW_VIEWER_MANAGER_H

#include<string>
#include<list>

#include "VDrawException.hpp"
#include "Color.hpp"

namespace vdraw
{
   /** \addtogroup BasicVectorGraphics */ 
   //@{

   /**
    * This class provides a convenient and consistent mechanism by which
    * to launch a graphics file viewer. 
    */
   class ViewerManager {

   public:
 
      /**
       * Constructor. Requires an environment variable to be named
       * that, if defined during runtime, is used to launch a viewer.
       */
      ViewerManager(const std::string& envVar);

      /**
       * Allows client code to register a viewer by name. Multiple viewers
       * can be registered. If a viewer is already registered it will not
       * be registered twice.
       */
     void registerViewer(const std::string& vname);

      /**
       * Launches a viewer for the given filename. First it tries
       * the viewer named by the environment variable (see c'tor).
       * then it tries the "registered" list of viewers. Returns
       * true if a viewer was launched.
       */
     bool view(const std::string& fileName) throw (VDrawException);
    
   protected:

   private:

      std::list<std::string> viewerList;

   }; // class ViewerManager

   //@}

} // namespace vdraw

#endif //VDRAW_VIEWER_MANAGER_H

