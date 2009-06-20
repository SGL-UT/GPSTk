#pragma ident "$Id$"

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

/*
   Example of native surface plots (pben).
*/


   // C++ includes
#include <cmath>

   // GPSTk includes
#include "EPSImage.hpp"
#include "SurfacePlot.hpp"
#include "Frame.hpp"
#include "BorderLayout.hpp"
#include "HLayout.hpp"
#include "VLayout.hpp"

   // Define PLOTSIZE
const int PLOTSIZE(100);

   //using namespace gpstk;
using namespace std;
using namespace vdraw;
using namespace vplot;

int  main ()
{

      // Declare an EPSImage object, with 'letter' paper size dimensions
      // This object will create an 'Encapsulated Postscript' file
   EPSImage vgwindow( "surfacePlotExample.eps",          // Output file
                      0,                                 // Lower left x
                      0,                                 // Lower left y
                      8.5*PTS_PER_INCH,                  // Upper right x
                      11.0*PTS_PER_INCH );               // Upper right y

      // Create a frame to drawn in
   Frame f(vgwindow);

      // Set up a margin within 'f' frame, 1/2 inch wide
   BorderLayout bl(f,0.5*PTS_PER_INCH);

      // Get a new 'Frame' according to margin
   Frame cf = bl.getFrame(0);

      // Object to arrange frames in a vertical stack.  Frame 'cf' will be
      // splited in two new frames, and the first new frame will get 95% of
      // vertical space (first frame means bottom frame)
   VLayout titleBodySplitter( cf, 0.95 );

      // 'ff' is the first vertical frame of 'titleBodySplitter', with 95%
      // In method "VLayout::getFrame()", a smaller number means more at the
      // bottom of the original frame
   Frame ff=titleBodySplitter.getFrame(0);

      // 'titleFrame' is the second vertical frame, with 5% of vertical space
   Frame titleFrame=titleBodySplitter.getFrame(1);

      // Text style for main title
   TextStyle hdrStyle( 0.3*PTS_PER_INCH,
                       TextStyle::BOLD,
                       Color::BLACK,
                       TextStyle::SANSSERIF );

      // Fill frame with "Text" object.
   titleFrame << Text( "Example of Native GPSTk Surface Plotting",      // Text
                       titleFrame.cx(),      // x coordinate of center of frame
                       titleFrame.cy(),      // y coordinate of center of frame
                       hdrStyle,
                       Text::CENTER );

      // Let's define a simple interpolated palette of colors
   Palette p( Color::GREY, -1, 1);           // Grey will be at the extremes

      // Add colors to the palette, with matching indexes
   p.setColor( 0.00, Color(Color::BLUE)   );
   p.setColor( 0.25, Color(Color::GREEN)  );
   p.setColor( 0.50, Color(Color::YELLOW) );
   p.setColor( 0.75, Color(Color::ORANGE) );
   p.setColor( 1.00, Color(Color::RED)    );

      // Set up a simple Surface Plot, 100x100 size and with palette 'p'
   SurfacePlot sp( PLOTSIZE, PLOTSIZE, p);

   sp.setColorLabel( "Sin value" );       // Label for color axis
   sp.setXAxis( -2*3.14, 2*3.14 );        // Minimum and maximum for x axis
   sp.setYAxis( -2*3.14, 2*3.14 );        // Minimum and maximum for y axis

   int i, j;

      // Create data series
   for (i=0; i<PLOTSIZE; i++)
   {
      for (j=0; j<PLOTSIZE; j++)
      {
            // Compute series values
         double r = fabs( sqrt( (50.0-i)*(50.0-i) + (50.0-j)*(50.0-j) ) );

            // Set color at (i,j) to matching color on the palette
         sp.set( i, j, sin( (r/25.0)*(2*3.14159265) ) );
      }
   }

      // Object to arrange frames in a horizontal stack.  Frame 'ff' will be
      // splited in two new frames, and the first new frame will get 85% of
      // horizontal space (first frame means left frame)
   HLayout vl( ff, 0.85 );

      // Create subframes for plot (left) and key (right)
   Frame leftf = vl.getFrame(0), keyf = vl.getFrame(1);

      // Left frame 'leftf' will be further divided in a vertical stack.
      // The first new frame (at bottom) will get 10% of vertical space.
   VLayout mainsplit( leftf, 0.1 );

      // Create new subframes
   Frame mainFrame = mainsplit.getFrame(1),                    // Upper frame
         lowerFrame = mainsplit.getFrame(0);                   // Lower frame

   int nPreviews = 5;

      // Lower subframe will be divided horizontally in 5 new subframes
   HLayout previewsLayout( lowerFrame, nPreviews);

      // Let's dray main plot and its key
   sp.drawPlot(&mainFrame);
   sp.drawKey(&keyf);

      // Declare a StrokeStyle object with an 'invisible' color
   StrokeStyle s(Color::CLEAR);

      // Define an Axis' style. This style is 'invisible'
   AxisStyle axisGoneStyle1;
   axisGoneStyle1.line_style = s;
   axisGoneStyle1.tick_style = s;
   axisGoneStyle1.draw_labels = false;

      // Adding the 'invisible' style to all sides will disable axis plotting
   sp.axis_top    = axisGoneStyle1;
   sp.axis_bottom = axisGoneStyle1;
   sp.axis_left   = axisGoneStyle1;
   sp.axis_right  = axisGoneStyle1;

      // Let's plot 'nPreviews' times the SurfacePlot, using the lower,
      // subdivided subframe.
   for (int k=0; k<nPreviews; k++)
   {
      Frame thisFrame = previewsLayout.getFrame(k);
      sp.drawPlot(&thisFrame);
   }

      // Flush file stream and call a 'ViewerManager' to launch viewer
   vgwindow.view();

   return(0);

}
