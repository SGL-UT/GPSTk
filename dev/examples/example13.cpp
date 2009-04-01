// C++ includes
#include <cmath>

// GPSTk includes
#include "EPSImage.hpp"
#include "SurfacePlot.hpp"
#include "Frame.hpp"
#include "BorderLayout.hpp"
#include "HLayout.hpp"
#include "VLayout.hpp"


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

   EPSImage vgwindow("surfacePlotExample.eps",0,0,8.5*PTS_PER_INCH,11*PTS_PER_INCH);

   Frame f(vgwindow);
   BorderLayout bl(f,0.5*PTS_PER_INCH);
   Frame cf = bl.getFrame(0);

   VLayout titleBodySplitter(cf, 0.95);
   Frame ff=titleBodySplitter.getFrame(0);
   Frame titleFrame=titleBodySplitter.getFrame(1);
   TextStyle hdrStyle(0.3*PTS_PER_INCH, TextStyle::BOLD, Color::BLACK, 
                   TextStyle::SANSSERIF);
   titleFrame << Text("Example of Native GPSTk Surface Plotting",titleFrame.cx(),titleFrame.cy(),hdrStyle,Text::CENTER);


   Palette p(Color(Color::GREY),-1,1);
   p.setColor(0,Color(Color::BLUE));
   p.setColor(0.25,Color(Color::GREEN));
   p.setColor(0.5,Color(Color::YELLOW));
   p.setColor(0.75,Color(Color::ORANGE));
   p.setColor(1,Color(Color::RED));

   SurfacePlot sp(100,100,p);
   sp.setColorLabel("Sin value");
   sp.setXAxis(-2*3.14,2*3.14);
   sp.setYAxis(-2*3.14,2*3.14);

   int i,j;
   
   for (i=0; i<100; i++)
     for (j=0; j<100; j++)
   {
     double r = fabs(sqrt((50.-i)*(50.-i)+(50.-j)*(50-j)));
     sp.set(i,j,sin(r/25.*(2*3.14159265)));
   }

   HLayout vl(ff,0.85);
   Frame leftf = vl.getFrame(0), keyf = vl.getFrame(1);
   VLayout mainsplit(leftf,.1);
   Frame mainFrame = mainsplit.getFrame(1),
     lowerFrame = mainsplit.getFrame(0);
   int nPreviews=5;
   HLayout previewsLayout(lowerFrame,nPreviews);

   sp.drawPlot(&mainFrame); 
   sp.drawKey(&keyf);

   StrokeStyle s(Color::CLEAR);
   AxisStyle axisGoneStyle1;
   axisGoneStyle1.line_style = s;
   axisGoneStyle1.tick_style = s;
   axisGoneStyle1.draw_labels = false;

   sp.axis_top    = axisGoneStyle1;
   sp.axis_bottom = axisGoneStyle1;
   sp.axis_left   = axisGoneStyle1;
   sp.axis_right  = axisGoneStyle1;

   for (int k=0; k<nPreviews; k++)
   {
      Frame thisFrame = previewsLayout.getFrame(k);
      sp.drawPlot(&thisFrame);
   }

   vgwindow.view();

   return(0);
}
