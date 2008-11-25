#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/PSImageBase.cpp#4 $"

///@file PSImageBase.cpp Common functionality between EPS and PS formats. Class definitions.

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

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <sstream>
#include "PSImageBase.hpp"

namespace vdraw
{

  /**
   * Constructors/Destructors
   */
  PSImageBase::PSImageBase(std::ostream& stream, 
      double width, 
      double height,
      ORIGIN_LOCATION iloc):
    VGImage(width, height, iloc), 
    ostr(stream), strokeStylePrinted(false),
    textStylePrinted(false),centerDefined(false),
    rightDefined(false)
  {
  }


  PSImageBase::PSImageBase(const char* fname, double width,
      double height,
      ORIGIN_LOCATION iloc):
    VGImage(width, height, iloc), 
    myfstream(fname), ostr(myfstream), filename(fname),
    strokeStylePrinted(false),textStylePrinted(false),
    centerDefined(false), rightDefined(false)
  {
  }



  PSImageBase::~PSImageBase(void)
  {}


  /**
   * Methods
   */

  void PSImageBase::updateStrokeStyle(StrokeStyle& ss)
  {
    if (!strokeStylePrinted || lastUsedStrokeStyle!=ss)
      printStrokeStyle(ss);
  }

  void PSImageBase::comment(const Comment& comment)
  {
    using namespace std;

    string::size_type loc = 0, lastloc = 0;
    while( (loc = comment.str.find("\n",lastloc)) != string::npos )
    {
      ostr << "% " << comment.str.substr(lastloc,loc-lastloc+1);
      lastloc = loc + 1;
    }
    if( lastloc < (comment.str.length()) )
      ostr << "% " << comment.str.substr(lastloc) << endl;

  }

  void PSImageBase::printStrokeStyle(const StrokeStyle& style)
  {
    using namespace std;

    // Color
    double red, blue, green;
    style.getColor().getRGBTripletFractional(red, green, blue);
    ostr << setprecision(5) << red << " " << green << " " << blue;
    ostr << " setrgbcolor " << endl;

    // Width
    ostr << style.getWidth() << " setlinewidth" << endl;

    // Dashing - write out length, space delimited
    ostr << "[ ";

    StrokeStyle::dashLengthList dll=style.getDashList();
    StrokeStyle::dashLengthList::iterator i;
    for (i=dll.begin(); i!=dll.end(); i++) 
      ostr  << (*i) << " ";

    ostr << "] 0 setdash " << endl;

    lastUsedStrokeStyle = style;
    strokeStylePrinted = true;
  }

  void PSImageBase::outputPath(const Path& path)
  {
    using namespace std;

    auto_ptr<Path> abspath = path.asAbsolute();
    Path::const_iterator i;
    bool first=true;

    // Step through each point
    for (i=abspath->begin(); i!=abspath->end(); i++)
    {
      if (first)
      {
        ostr << i->first << " " << ((ll)?i->second:(canvasHeight-i->second)) << " moveto" << endl;
        first=false;
      }
      else
        ostr << i->first << " " << ((ll)?i->second:(canvasHeight-i->second)) << " lineto" << endl;
    } // Step through points in the path

    return;
  }

  void PSImageBase::markPath(const Path& path, const char * name)
  {
    using namespace std;

    auto_ptr<Path> abspath = path.asAbsolute();    
    Path::const_iterator i;

    // Step through each point
    for (i=abspath->begin(); i!=abspath->end(); i++)
      ostr << i->first << " " << ((ll)?i->second:(canvasHeight-i->second)) << " " << name << endl;

    return;
  }


  void PSImageBase::line (const Line& line)
  {
    using namespace std;

    if ( extraComments )
      ostr << "% Line" << endl;

    Marker cmarker;
    StyleType tmarker = getCorrectMarker(&cmarker,line);
    if( tmarker == SHAPE )
    {
      if ( !markerDefined || (lastMarker != line.getMarker()) )
      {
        defineMarker( line.getMarker() );
      }
    }

    StrokeStyle ss;
    StyleType tss = getCorrectStrokeStyle(&ss,line);

    if( tmarker != NONE && tmarker != CLEAR )
    {
      ostr << "% This is the marker run through...\n";

      // Color
      double red, blue, green;
      cmarker.getColor().getRGBTripletFractional(red, green, blue);
      ostr << setprecision(5) << red << " " << green << " " << blue;
      ostr << " setrgbcolor " << endl;
      ostr << ".2 setlinewidth " << endl;

      if( tmarker == SHAPE ) markPath(line);
      else if( tmarker == DEFAULT ) markPath(line,"dflt");
    }

    // The marker messed with color and stuff...so force rewrite...
    if( tmarker != NONE && tmarker != CLEAR ) 
      printStrokeStyle(ss);
    else  
      updateStrokeStyle(ss);

    // If there is neither a line or general stroke style and there is a marker
    // then just leave it at rendering points, not lines.
    if( !(tss == CLEAR) || (tss == NONE && (tmarker == SHAPE || tmarker == DEFAULT)) )
    {
      ostr << "newpath" << endl;
      outputPath(line);
      ostr << "stroke" << endl;
    }
  }

  void PSImageBase::defineMarker (const Marker& marker, const char * markername)
  {
    using namespace std;

    bool dflt = true;
    if( !markername )
    {
      dflt = false;
      markername = "mark";
    }

    if( extraComments )
      ostr << "%   Marker defintion\n";

    ostr << "/" << markername << " { ";

    double r = marker.getRange();
    double r2 = r*2;

    if (marker.hasDefaultMark())
    {
      Marker::Mark mark = marker.getMark();
      switch(mark)
      {
        case Marker::DOT:
          ostr << "newpath " << r << " 0 360 arc fill";
          break;
        case Marker::PLUS:
          ostr << "moveto " 
            << r << " 0 rmoveto "
            << -r2  << " 0 rlineto "
            << r << " " << r << " rmoveto "
            << "0 " << -r2 << " rlineto stroke";
          break;
        case Marker::X:
          ostr << "moveto "
            << r << " " << r << " rmoveto "
            << -r2 << " " << -r2 << " rlineto "
            << r2 << " 0 rmoveto "
            << -r2 << " " << r2 << " rlineto stroke";
          break;
      }
    }
    else
    {
      //Not implemented...see SVG file for more info in same place...
    }

    ostr << " } def\n";

    if( !dflt )
    {
      markerDefined = true;
      lastMarker = marker;
    }
  }

  void PSImageBase::rectangle (const Rectangle& rect)
  {
    using namespace std;

    StrokeStyle ss;
    StyleType tss = getCorrectStrokeStyle(&ss,rect);
    Color fc;
    StyleType tfc = getCorrectFillColor(&fc,rect);

    if( (tss == NONE || tss == CLEAR) && (tfc == NONE || tfc == CLEAR) )
      comment("invisible rectangle ignored...");


    if ( extraComments )
      ostr << "% Rectangle" << endl;

    ostr << "newpath" << endl;

    ostr << rect.x1 << " " << ((ll)?rect.y1:(canvasHeight-rect.y1)) << " moveto\n"
      << rect.x2 << " " << ((ll)?rect.y1:(canvasHeight-rect.y1))  << " lineto\n"
      << rect.x2 << " " << ((ll)?rect.y2:(canvasHeight-rect.y2))  << " lineto\n"
      << rect.x1 << " " << ((ll)?rect.y2:(canvasHeight-rect.y2))  << " lineto\n"
      << "closepath" << endl;

    if( tfc != CLEAR && tfc != NONE )
      ostr << fillWithColor(fc);

    // If the line is clear then don't bother to stroke
    if(!(tss == CLEAR))
    {
      updateStrokeStyle(ss);
      ostr << "stroke" << endl;
    }
  }

  void PSImageBase::circle (const Circle& circle)
  {
    using namespace std;

    StrokeStyle ss;
    StyleType tss = getCorrectStrokeStyle(&ss,circle);
    Color fc;
    StyleType tfc = getCorrectFillColor(&fc,circle);

    if( (tss == NONE || tss == CLEAR) && (tfc == NONE || tfc == CLEAR) )
      comment("invisible circle ignored...");

    if ( extraComments )
      ostr << "% Circle" << endl;

    ostr << "newpath" << endl;

    ostr << circle.xc << " " << ((ll)?circle.yc:(canvasHeight-circle.yc)) << " "
      << circle.radius << " 0 360 arc\n"
      << "closepath" << endl;


    if( tfc != CLEAR && tfc != NONE )
      ostr << fillWithColor(fc);

    // If the line is clear then don't bother to stroke
    if(!(tss == CLEAR))
    {
      updateStrokeStyle(ss);
      ostr << "stroke" << endl;
    }
  }


  void PSImageBase::polygon(const Polygon& polygon)
  {
    using namespace std;

    StrokeStyle ss;
    StyleType tss = getCorrectStrokeStyle(&ss,polygon);
    Color fc;
    StyleType tfc = getCorrectFillColor(&fc,polygon);

    if( (tss == NONE || tss == CLEAR) && (tfc == NONE || tfc == CLEAR) )
      comment("invisible polygon ignored...");

    if ( extraComments )
      ostr << "% Polygon" << endl;

    ostr << "newpath" << endl;
    outputPath(polygon);
    ostr << "closepath" << endl;

    if( tfc != CLEAR && tfc != NONE )
      ostr << fillWithColor(fc);

    // If the line is clear then don't bother to stroke
    if(!(tss == CLEAR))
    {
      updateStrokeStyle(ss);
      ostr << "stroke" << endl;
    }
  }

  void PSImageBase::bitmap (const Bitmap& bitmap) 
  {
    using namespace std;

    if(bitmap.boxes)
    {
      comment("Bitmap boxes");
      rectangles(bitmap);
    }
    else
    {
      comment("Bitmap Image");
      ostr << "gsave" << endl;

      double bw=bitmap.x2-bitmap.x1;
      double bh=(ll?1:-1)*(bitmap.y2-bitmap.y1);

      //ostr << bitmap.x1 << " " << bitmap.y1 << " translate" << endl
      ostr << bitmap.x1 << " " << ((ll)?bitmap.y1:(canvasHeight-bitmap.y1)) << " translate" << endl
        << bw << " " << bh << " scale" << endl;

      Bitmap::CMType which;
      int w, h, osw, osh;
      InterpolatedColorMap icm;
      ColorMap cm;

      if(bitmap.getICM(&icm))
      {
        which = Bitmap::ICM;
        w = icm.getCols();
        h = icm.getRows();
      }
      else if(bitmap.getCM(&cm))
      {
        which = Bitmap::CM;
        w = cm.getCols();
        h = cm.getRows();
      }
      osw = w*bitmap.osc;
      osh = h*bitmap.osr;

      // width height bits/comp matrix datasrc0 … datasrcncomp-1 multi ncomp colorimag
      // matrix = [width 0 0 -height 0 height]
      ostr << osw  << " " << osh << " 8 ["  << osw << " 0 0 " << -osh << " 0 " << osh 
        << "]\n{currentfile " << 3*osw << " string readhexstring pop} bind"
        << "\nfalse 3 colorimage";
      //for(int r=h-1; r>=0; r--) // For ll
      //for(int r=0  ; r<h ; r++) // For !ll
      int count=0;
      for(int r=(ll?h-1:0);(ll?r>=0:r<h);r+=(ll?-1:1))        
      {
        for(int rr=0;rr<bitmap.osr;rr++)
        {
          for(int c=0; c<w; c++)
          {
            stringstream ss;
            ss << hex << noshowbase << setfill('0') << setw(6);
            if(which==Bitmap::CM)
              ss << cm.get(r,c).getRGB();
            else
              ss << icm.get(r,c).getRGB();
            string sss = ss.str();
            for(int cc=0;cc<bitmap.osc;cc++)
            {
              if((count%12)==0) ostr << endl;
              ostr << sss;
              count++;
            }
          }
        }
      }
    }
    ostr << endl << "grestore" << endl;
  }

  void PSImageBase::text(const Text& text)
  {
    using namespace std;
    TextStyle ts;
    StyleType tts = getCorrectTextStyle(&ts,text);

    /*
     * This is to be safe since other things rely upon the last stroke
     * style to see if they need to redefine the style.  Since text changes
     * color this could screw up lines to be this color since the stroke
     * style has not changed.
     */
    StrokeStyle ss(ts.getColor());
    updateStrokeStyle(ss);

    if( tts == CLEAR )
    {
      comment("Text with clear font ignored:%s",text.getString().c_str());
      return;
    }

    if ( extraComments )
      ostr << "% Text" << endl;

    if (text.isCenter() && !centerDefined)
    {
      ostr << "/center { dup stringwidth pop 2 div neg 0 rmoveto } def" << endl;
      centerDefined = true;
    }

    if (text.isRight() && !rightDefined)
    {
      ostr << "/right { dup stringwidth pop neg 0 rmoveto } def" << endl;
      rightDefined = true;
    }

    if( !textStylePrinted || ts != lastUsedTextStyle )
    {
      printTextStyle(ts);
      lastUsedTextStyle = ts;
    }

    double red, blue, green;
    ts.getColor().getRGBTripletFractional(red, green, blue);
    ostr << setprecision(5) << red << " " << green << " " << blue;
    ostr << " setrgbcolor " << endl;

    bool hasAngle = text.getAngle()!=0;

    if( hasAngle )
    {
      ostr << "newpath" << endl
        << "gsave" << endl
        << text.x << " " << ((ll)?text.y:(canvasHeight-text.y)) 
        << " translate" << endl
        << text.getAngle() << " rotate" << endl
        << "0 0 moveto" << endl;
    }
    else
    {
      ostr << "newpath" << endl
        << text.x << " " << ((ll)?text.y:(canvasHeight-text.y)) 
        << " moveto" << endl;
    }

    ostr << "(" << text.getString() << ") " 
      << (text.isRight()?"right":(text.isCenter()?"center":"")) <<" show" << endl;

    if( hasAngle )
      ostr << "grestore" << endl;
  }

  void PSImageBase::printTextStyle(const TextStyle& ts)
  {
    using namespace std;

    if( ts.isMonospace() )
      ostr << "/Courier";
    else if( ts.isSerif() )
      ostr << "/Times";
    else //if( ts.isSansSerif() )
      ostr << "/Helvetica";

    if( ts.isBold() )
      ostr << "-Bold";
    if( ts.isItalic() )
    {
      if( !ts.isBold() ) 
        ostr << "-";
      if( ts.isMonospace() | ts.isSansSerif() )
        ostr << "Oblique";
      else
        ostr << "Italic";
    }

    ostr << " findfont" << endl;

    ostr << ts.getPointSize() << " scalefont setfont" << endl; 

    textStylePrinted = true;
  }

  std::string PSImageBase::fillWithColor(const Color& color) const
  {
    using namespace std;

    if( color.isClear() )
      return "";

    stringstream ostr(stringstream::in | stringstream::out); 

    ostr << "gsave" << endl;
    // Use the fill color
    double red, blue, green;
    color.getRGBTripletFractional(red, green, blue);
    ostr << setprecision(5) << red << " " << green << " " << blue;
    ostr << " setrgbcolor " << endl;
    ostr << "fill" << endl;
    ostr << "grestore" << endl;   

    return string(ostr.str());
  }

} // namespace vdraw

