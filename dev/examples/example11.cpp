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

/**
 * @file RungeKuttaTest.cpp
 *
 */

//using namespace gpstk;
using namespace std;
using namespace vdraw;
using namespace vplot;

int  main ()
{
   // First, some housekeeping.
   srand(time(0));  // Initialize random number generator   

   EPSImage vgwindow("lineScatterPlotsExample.eps",0,0,8.5*PTS_PER_INCH,11*PTS_PER_INCH);

   Frame f(vgwindow);
   BorderLayout bl(f,0.5*PTS_PER_INCH);
   Frame cf = bl.getFrame(0);

   VLayout titleBodySplitter(cf, 0.95);
   Frame ff=titleBodySplitter.getFrame(0);
   Frame titleFrame=titleBodySplitter.getFrame(1);
   TextStyle hdrStyle(0.3*PTS_PER_INCH, TextStyle::BOLD, Color::BLACK, 
                   TextStyle::SANSSERIF);
   titleFrame << Text("Examples of Native GPSTk Plotting",titleFrame.cx(),titleFrame.cy(),hdrStyle,Text::CENTER);


   // Part one: plotting a simple data series.
   vector< pair<double, double> > series1, series2, series3, series4;

   for (double t=0; t<20; t+=0.2)
   {
     double r = 0.4*(((rand()%1000)/1000.)-.5);
      series1.push_back(pair <double, double> (t, sin(t)));
      series2.push_back(pair <double, double> (t, 0.5*cos(t)));
      series3.push_back(pair <double, double> (t, cos(t) + r));
      series4.push_back(pair <double, double> (t, 0.75*cos(0.5*t)));
   }



   VLayout vl(ff,2);
   Frame tf = vl.getFrame(1);
   HLayout h1(tf,0.8);
   Frame plotframe = h1.getFrame(0),
     keyframe = h1.getFrame(1);
 
   LinePlot lineplot;
   lineplot.addSeries(string("Sine"),series1);
   lineplot.addSeries(string("Cosine"),series2);

   // Don't use the default style series
   StrokeStyle thisStyle(Color::BLACK, 1);
   lineplot.addSeries(string("With noise"),series3, thisStyle);

   StrokeStyle::dashLengthList dashPattern;
   dashPattern.push_back(6);
   dashPattern.push_back(2);
   StrokeStyle thatStyle(Color::MAGENTA, 4, dashPattern);
   lineplot.addSeries(string("Low frequency"),series4, thatStyle);

   lineplot.setXLabel("time (seconds)");
   lineplot.setYLabel("amplitude (m)"); 

   lineplot.drawPlot(plotframe);
   lineplot.drawKey(keyframe);

   // Part one: a scatter plot

   vector< pair<double, double> > positions, skewedpos, otherskewedpos;

   for (int i=0; i<1000; i++)
   {
     double x = ((rand() % 1000) + (rand() % 1000) + (rand() % 1000) - 1500.) / 3000.;
     double y = ((rand() % 1000) + (rand() % 1000) + (rand() % 1000) - 1500.) / 3000.;
     positions.push_back(pair<double, double>(x,y));
     skewedpos.push_back(pair<double, double>(0.4*x,y+x));
     otherskewedpos.push_back(pair<double, double>(x,.5*y-.2*x));
   }

   ScatterPlot scatterPlot;
   scatterPlot.addSeries(string("Positions"), positions);

   Marker markSkew(Marker::PLUS, 5.0, Color::GREY);
   scatterPlot.addSeries(string("Skewed solution"), skewedpos, markSkew);

   Marker markOtherSkew(Marker::X, 2.0, Color::GREEN);
   scatterPlot.addSeries(string("Further skewed"), 
			 otherskewedpos, markOtherSkew);

   Frame mf = vl.getFrame(0);
   BorderLayout bl2(mf, 0.5*PTS_PER_INCH);
   Frame bf = bl2.getFrame(0);
   HLayout bhlayout(bf, 0.8);
   Frame zoomFrame = bhlayout.getFrame(0);

   scatterPlot.setXLabel("Easting");
   scatterPlot.setYLabel("Northing");
   scatterPlot.drawPlot(zoomFrame);
   Frame rframe = bhlayout.getFrame(1);
   scatterPlot.drawKey(rframe);

   vgwindow.view();

   return(0);
}
