
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

namespace vplot
{

  /**
   * Constructors/Destructors
   */
  PSImageBase::PSImageBase(std::ostream& stream, 
      double width, 
      double height,
      ORIGIN_LOCATION iloc):
    VGImage(width, height, iloc), 
    ostr(stream), centerDefined(false), rightDefined(false)
  {
  }


  PSImageBase::PSImageBase(const char* fname, double width,
      double height,
      ORIGIN_LOCATION iloc):
    VGImage(width, height, iloc), 
    filename(fname), myfstream(fname), ostr(myfstream),
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
    if (lastUsedStrokeStyle!=ss)
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
  }

  void PSImageBase::outputPath(const Path& path)
  {
    using namespace std;

    Path abspath = path.asAbsolute();
    Path::const_iterator i;
    bool first=true;

    // Step through each point
    for (i=abspath.begin(); i!=abspath.end(); i++)
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

    Path abspath = path.asAbsolute();
    Path::const_iterator i;

    // Step through each point
    for (i=abspath.begin(); i!=abspath.end(); i++)
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

    updateStrokeStyle(ss);

    ostr << "stroke" << endl;
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

    updateStrokeStyle(ss);

    ostr << "stroke" << endl;
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

    updateStrokeStyle(ss);

    ostr << "stroke" << endl;
  }

  void PSImageBase::text(const Text& text)
  {
    using namespace std;

    TextStyle ts;
    StyleType tts = getCorrectTextStyle(&ts,text);

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

    if( ts != lastUsedTextStyle )
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

} // namespace vplot

