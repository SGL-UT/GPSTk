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

/*
   Example of GPSTk's built-in postscript plotting routines (pben).
*/


   // C++ includes
#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()

   // GPSTk includes
#include "StrokeStyle.hpp"
#include "EPSImage.hpp"
#include "LinePlot.hpp"
#include "ScatterPlot.hpp"
#include "Frame.hpp"
#include "BorderLayout.hpp"
#include "HLayout.hpp"
#include "VLayout.hpp"
#include "Marker.hpp"


   //using namespace gpstk;
using namespace std;
using namespace vdraw;
using namespace vplot;

int main()
{

      // First, some housekeeping.
   srand(time(0));  // Initialize random number generator   

      // Declare an EPSImage object, with 'letter' paper size dimensions
      // This object will create an 'Encapsulated Postscript' file
   EPSImage vgwindow( "lineScatterPlotsExample.eps",     // Output file
                      0,                                 // Lower left x
                      0,                                 // Lower left y
                      8.5*PTS_PER_INCH,                  // Upper right x
                      11.0*PTS_PER_INCH );               // Upper right y

      // Create a frame to drawn in
   Frame f(vgwindow);

      // Set up a margin within 'f' frame, 1/2 inch wide
   BorderLayout bl( f, 0.5*PTS_PER_INCH );

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
   titleFrame << Text( "Examples of Native GPSTk Plotting", // Text to display
                       titleFrame.cx(),      // x coordinate of center of frame
                       titleFrame.cy(),      // y coordinate of center of frame
                       hdrStyle,
                       Text::CENTER );



      //// PART ONE: plotting a simple data series ////


      // Generate data series
   vector< pair<double, double> > series1, series2, series3, series4;

   for (double t=0; t<20; t+=0.2)
   {
      double r = 0.4*( ( (rand()%1000)/1000.0 ) - 0.5 );
         // 'Sine' series
      series1.push_back( pair<double, double> ( t, sin(t) ) );
         // 'Cosine' series
      series2.push_back( pair<double, double> ( t, 0.5*cos(t) ) );
         // 'Noisy cosine' series, bigger amplitude
      series3.push_back( pair<double, double> ( t, cos(t) + r ) );
         // 'Low frequency cosine' series
      series4.push_back( pair<double, double> ( t, 0.75*cos(0.5*t) ) );
   }


      // Frame 'ff' will be further divided in two other frames
   VLayout vl( ff, 2 );

      // Create a frame. This goes to the upper part of 'ff'
   Frame tf = vl.getFrame(1);

      // Object to arrange frames in a horizontal stack.  Frame 'tf' will be
      // splited in two new frames, and the first new frame will get 80% of
      // horizontal space (first frame means left frame)
   HLayout h1( tf, 0.8 );

      // Create subframes for plot (left) and key (right)
   Frame plotframe = h1.getFrame(0),                           // Left frame
         keyframe  = h1.getFrame(1);                           // Right frame

      // Object to plot lines
   LinePlot lineplot;
   lineplot.addSeries( string("Sine"), series1 );     // Add "Sine" series
   lineplot.addSeries( string("Cosine"), series2 );   // Add "Cosine" series

      // Don't use the default style series
   StrokeStyle thisStyle(Color::BLACK, 1);

      // Add "noisy" series
   lineplot.addSeries( string("With noise"), series3, thisStyle );

      // Set up a dash pattern
   StrokeStyle::dashLengthList dashPattern;
   dashPattern.push_back(6);              // Dash is 6 pixels long, with
   dashPattern.push_back(2);              // a separation of 2 pixels

      // Change style for 'Low frequency' series and add it
   StrokeStyle thatStyle( Color::MAGENTA, 4, dashPattern );
   lineplot.addSeries( string("Low frequency"), series4, thatStyle );

      // Add axis labels
   lineplot.setXLabel("time (seconds)");
   lineplot.setYLabel("amplitude (m)"); 

      // Let's plot series and key into the corresponding frames
   lineplot.drawPlot(plotframe);
   lineplot.drawKey(keyframe);



      //// PART TWO: a scatter plot ////

      // Generate data series
   vector< pair<double, double> > positions, skewedpos, otherskewedpos;

   for (int i=0; i<1000; i++)
   {
      double x = ((rand() % 1000) + (rand() % 1000) + (rand() % 1000) - 1500.0)
                 / 3000.0;

      double y = ((rand() % 1000) + (rand() % 1000) + (rand() % 1000) - 1500.0)
                 / 3000.0;

      positions.push_back( pair<double, double>( x, y ) );
      skewedpos.push_back( pair<double, double>( 0.4*x, y+x ) );
      otherskewedpos.push_back( pair<double, double>( x, 0.5*y-0.2*x ) );
   }


      // Object to draw a basic scatter plot
   ScatterPlot scatterPlot;

      // Add 'positions' series
   scatterPlot.addSeries(string("Positions"), positions);

      // Define markers which will be drawn at every point
      // 'Skewed solution' series will use big (5), gray 'plus' signs as markers
   Marker markSkew(Marker::PLUS, 5.0, Color::GREY);
      // Add series
   scatterPlot.addSeries( string("Skewed solution"),
                          skewedpos,
                          markSkew );

      // 'Further skewed' series will use small (2), green X's as markers
   Marker markOtherSkew(Marker::X, 2.0, Color::GREEN);
      // Add series
   scatterPlot.addSeries( string("Further skewed"),
                          otherskewedpos,
                          markOtherSkew );

      // Create a frame. This goes to the bottom part of 'ff'
   Frame mf = vl.getFrame(0);

      // Set up a margin within 'mf' frame, 1/2 inch wide
   BorderLayout bl2( mf, 0.5*PTS_PER_INCH );

      // Get a new 'Frame' according to margin
   Frame bf = bl2.getFrame(0);

      // Object to arrange frames in a horizontal stack.  Frame 'bf' will be
      // splited in two new frames, and the first new frame will get 80% of
      // horizontal space (first frame means left frame)
   HLayout bhlayout( bf, 0.8 );

      // First (left) frame
   Frame zoomFrame = bhlayout.getFrame(0);

      // Add axis labels
   scatterPlot.setXLabel("Easting");
   scatterPlot.setYLabel("Northing");

      // Draw series at first (left) frame
   scatterPlot.drawPlot(zoomFrame);

      // Second (right) frame
   Frame rframe = bhlayout.getFrame(1);

      // Draw key at second (right) frame
   scatterPlot.drawKey(rframe);

      // Flush file stream and call a 'ViewerManager' to launch viewer
   vgwindow.view();

   return(0);

}
