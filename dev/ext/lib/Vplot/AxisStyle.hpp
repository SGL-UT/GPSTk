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

/// @file AxisStyle.hpp Used to describe an axis' traits. Class declarations.

#ifndef VPLOT_AXISSTYLE_H
#define VPLOT_AXISSTYLE_H

#include "StrokeStyle.hpp"
#include "TextStyle.hpp"

namespace vplot
{
  /*
   * I probably need a class for the following things:
   *
   * - Axis setup
   *   - This would be in the middle (on 0) or on an edge as a border
   *     of the plot/frame.
   *   * StrokeStyle for everything
   *   * Length of ticks 
   *   * Alignment of ticks
   *     -  --|   Left
   *     -   -|-  Center
   *     -    |-- Right
   *     -   |     |  
   *     -  ---   ---   --- 
   *     -         |     |
   *     - Above Center Below
   *   - Placement of ticks
   *     - Evenly space n across the line (to bounds)
   *     - or evenly space them a given amount to wherever
   *     - Start at 0 or side
   *   - Tick recursion for najor/minor ticks
   *   - Tick recursion for labels
   *   - Label placement
   *   - Label formatting?
   *   * TextStyle for label
   *   - Direction for axis
   *   - Bounds!
   *     - Bound generation from min/max (with buffer area? round?)
   * - If this is a seperate class it needs a set of COPY commands
   *   that copy to a different orientation.  Or a mirror setting that
   *   does so automagically.
   */

  /**
   * This class is used to describe an Axis's style. This includes information
   * about ticks and labels for the Axis. 
   */
  class AxisStyle
  {
    public:
      /**
       * Based on the axis's direction, these describe where the ticks
       * are drawn.
       * \verbatim
       *             |      |
       * Axis: ------+------|------+------ (end)
       *                    |      |
       *            ABOVE CENTER BELOW \endverbatim
       */
      static const int ABOVE  = -1;
      static const int CENTER = 0;
      static const int BELOW  = 1;

      /*
       * Tick settings
       */

      /// Is this logarithmic?
      bool logarithmic;

      /// This is where it appears on the line or through it, which side
      int tick_position;

      /// This is the line style for ticks, if null use line_style
      vdraw::StrokeStyle tick_style;

      /// Max tick length
      double major_tick_length;

      /// Tight bounds (don't make them any bigger than the settings say
      ///   In other words: Force min and max to be at top and bottom 
      bool tight_bounds;

      /// Start labelling ticks from zero?
      // TODO: Start labelling from some specific place?
      bool start_ticks_on_zero;

      /// Ratio for each successive depth of tick marks.  Like .8 or .5
      double tick_depth_multiplier;

      /// Recursive depth to create tick marks
      int tick_recursion_depth;

      /// Number of subticks for every tick
      int subticks_per_tick;

      /*
       * Label settings
       */
      
      /// Draw labels
      bool draw_labels;

      // TODO: Special labels?
      // std::vector<std::string> labels
      // or
      // Map from value to label:
      // std::map<double,std::string> labels
      // for example
      // labels.put(10,"Elevation mask");

      /// The style of the label text
      vdraw::TextStyle label_style;

      // The depth of tick recursion to draw labels
      // NOTE: This isn't really necessary, though maybe for log plots
      //int tick_label_depth;

      /// The position of the label. Valid positions are ABOVE and BELOW
      int label_position;

      /**
       * Format string for the label.  The idea is that this will be "" by
       * default (allowing Axis to figure it out.  Otherwise set it to a format
       * string taking a single double such as "%0.4f" or "%3g" or whatever you
       * wish.
       */
      std::string label_format;

      /*
       * General settings
       */

      /// The style of the line
      vdraw::StrokeStyle line_style;

      /*
       * Constructor/Destructor
       */

      AxisStyle()
      {
        logarithmic = false;
        tick_position = CENTER;
        tick_style = vdraw::StrokeStyle();
        major_tick_length = 10;
        tight_bounds = false;
        start_ticks_on_zero = true;
        tick_depth_multiplier = 0.7;
        tick_recursion_depth = 2;
        subticks_per_tick = 1;
        draw_labels = true;
        label_style = vdraw::TextStyle();
        label_position = ABOVE;
        label_format = "";
        line_style = vdraw::StrokeStyle();
      }

      ~AxisStyle()
      {
      }
  };
}

#endif //VPLOT_AXISSTYLE_H
