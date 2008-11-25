#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/VGState.hpp#1 $"

///@file VGState.hpp Maintains style state. Class declarations.

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

#ifndef VDRAW_VGSTATE_H
#define VDRAW_VGSTATE_H

#include "StrokeStyle.hpp"
#include "Marker.hpp"
#include "TextStyle.hpp"
#include "Color.hpp"


namespace vdraw
{
  /** \addtogroup BasicVectorGraphics */ 
  //@{

  /**
   * This class will simply maintain state about text and line styles, fill
   * colors, and markers.  Encapsulated for use with a stack of states.
   */

  class VGState 
  {
    public:

      /// The default StrokeStyle (if one is set)
      StrokeStyle strokeStyle;
      /// The default Marker (if one is set)
      Marker marker;
      /// The default TextStyle (if one is set)
      TextStyle textStyle;
      /// The default fill Color (if one is set)
      Color fillColor;

      /// Whether or not to use the default StrokeStyle
      bool useSS;
      /// Whether or not to use the default Marker
      bool useM;
      /// Whether or not to use the default TextStyle
      bool useTS;
      /// Whether or not to use the default fill Color      
      bool useFC;

      /// Make sure the defaults are with nothing set.
      VGState()
      {
        useSS = useM = useTS = useFC = false;
      }

      /// Remove the default StrokeStyle
      inline void setSS()                      { useSS=false; }
      /// Set the default StrokeStyle
      inline void setSS(const StrokeStyle &ss) { useSS=true; strokeStyle=ss; }
      /// Remove the default Marker
      inline void setM()                       { useM=false; }
      /// Set the default Marker
      inline void setM(const Marker &m)        { useM=true; marker=m; }
      /// Remove the default TextStyle
      inline void setTS()                      { useTS=false; }
      /// Set the default TextStyle
      inline void setTS(const TextStyle &ts)   { useTS=true; textStyle=ts; }
      /// Remove the default fill Color
      inline void setFC()                      { useFC=false; }
      /// Set the default fill Color
      inline void setFC(const Color &fc)       { useFC=true; fillColor=fc; }
  }; // class VGState

  //@} group BasicVectorGraphics

} // namespace vdraw

#endif //VDRAW_VGSTATE_H

