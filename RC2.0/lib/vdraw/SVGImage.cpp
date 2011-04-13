#pragma ident "$Id$"

///@file SVGImage.cpp Vector plotting in the SVG format. Class definitions.

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
//===========================================================================

#include <sstream>
#include <iomanip>
#include "SVGImage.hpp"
#include "PNG.hpp"
#include "Base64Encoder.hpp"

namespace vdraw
{
  static char VIEWER_ENV_VAR_NAME[]="VDRAW_SVG_VIEWER";

  /**
   * Constructors/Destructors
   */

  SVGImage::SVGImage(std::ostream& stream, double width, double height, ORIGIN_LOCATION iloc):
    VGImage(width, height, iloc),
    viewerManager(VIEWER_ENV_VAR_NAME),
    ostr(stream)
  {
    outputHeader();
  }

  SVGImage::SVGImage(const char* fname, double width, double height, ORIGIN_LOCATION iloc):
    VGImage(width, height, iloc), 
    viewerManager(VIEWER_ENV_VAR_NAME),
    filename(fname), myfstream(fname), ostr(myfstream)
  {
    outputHeader();
  }

  SVGImage::~SVGImage(void)
  {
    if (!footerHasBeenWritten) outputFooter();
  }

  /**
   * Methods
   */

  void SVGImage::outputHeader()
  {
    using namespace std;

    ostr << "<?xml version=\"1.0\"?>" << endl;
    ostr << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"" << endl;
    ostr << " \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">" << endl;
    ostr << endl;
    ostr << "<!-- Created by vplot -->" << endl;
    ostr << endl;
    ostr << "<svg width=\"" << canvasWidth << "pt\" height=\""; 
    ostr << canvasHeight << "pt\"";
    /// The following two are required for firefox support.
    ostr << " xmlns=\"http://www.w3.org/2000/svg\"";
    ostr << " xmlns:xlink=\"http://www.w3.org/1999/xlink\"";
    ostr << " version=\"1.1\">" << endl;

    // Output defaults?
    ostr << "  <defs>" << endl
         << "    <style type=\"text/css\">" << endl
         << "      line    { fill: none; stroke: black; stroke-width: 1pt }" << endl
         << "      polyline{ fill: none; stroke: black; stroke-width: 1pt }" << endl         
         << "      rect    { fill: none; stroke: black; stroke-width: 1pt }" << endl
         << "      circle  { fill: none; stroke: black; stroke-width: 1pt }" << endl
         << "      polygon { fill: none; stroke: black; stroke-width: 1pt }" << endl                  
         << "      path    { fill: none; stroke: black; stroke-width: 1pt }" << endl         
         << "      text    { fill: black; font-size: 12pt; "
         << "font-family:monospace; text-anchor:start }" << endl
         << "    </style>" << endl
         << "  </defs>" << endl;

    // Add a transform to flip y so that the origin is in the bottom left 
    // corner
    if ( ll )
    {
      ostr << endl << "   <g transform=\"translate(0," << canvasHeight;
      ostr << ") scale(1,-1)\">" << endl;
      // Set the indentation
      tab=string("      ");
    } 
    else
      tab=string("   ");
  }

  void SVGImage::outputFooter(void)
  {
    using namespace std;

    // Close our transformation of origin to bottom right
    if ( ll )
      ostr << "   </g>" << endl; 

    ostr << "</svg>" << endl;
    footerHasBeenWritten = true;
  }

  void SVGImage::comment (const Comment& comment)
  {
    using namespace std;

    ostr << tab << "<!-- ";
    string com(comment.str);
    string::size_type loc;
    while( (loc = com.find("-->")) != string::npos)
      com = com.replace(loc+1,1,"=");
    bool hadnl = false;
    if((loc = com.find("\n")) != string::npos)
    {
      hadnl = true;
      ostr << endl;
      string::size_type lastloc = 0;
      while( (loc = com.find("\n",lastloc)) != string::npos )
      {
        ostr << tab << "     " << com.substr(lastloc,loc-lastloc+1);
        lastloc = loc + 1;
      }
      if( lastloc < (com.length()) )
        ostr << tab << "     " << com.substr(lastloc) << endl;
      ostr << tab << "-->" << endl;        
    }
    else
      ostr << com << " -->" << endl;
  }

  void SVGImage::line (const Line& line) 
  {
    using namespace std;

    if ( line.size() == 0 )
    {
      comment("Empty Line ignored...");
      return;
    }

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

    if ( line.size() <= 2 )
    {
      ostr << tab << "<line";
      auto_ptr<Path> abspath = line.asAbsolute();
      Path::const_iterator i=abspath->begin();
      // Since there is no method for drawing a single point and there already
      // is a marker defined...
      if( line.size() == 1 )
      {
        ostr << " x1=\"" << i->first << "\"";
        ostr << " y1=\"" << i->second << "\"";
        ostr << " x2=\"" << i->first << "\"";
        ostr << " y2=\"" << i->second << "\"";
      }
      else
      {
        ostr << " x1=\"" << i->first << "\"";
        ostr << " y1=\"" << i->second << "\"";
        i++;
        ostr << " x2=\"" << i->first << "\"";
        ostr << " y2=\"" << i->second << "\"";
      }

      string tmp = strokeDesc(ss,tss,((tmarker!=NONE)&&(tmarker!=CLEAR)));
      if(tmp.size())
        ostr << " style=\"" << tmp << "\"";

      if( tmarker != NONE && tmarker != CLEAR )
      {
        string mname = cmarker.uniqueName();
        ostr << endl << tab << "   marker-end=\"url(#" 
          << mname << ")\" marker-mid=\"url(#" 
          << mname << ")\" marker-start=\"url(#"
          << mname << ")\"";
      }
      //else if( hasDefaultMarker )
      //  ostr << endl << tab << "   marker-end=\"url(#default)\" marker-mid=\"url(#default)\" marker-start=\"url(#default)\"";

      ostr << "/>" << endl;
    }
    else
    {
      // open line tag and specify x,y coords of two end points
      ostr << tab << "<polyline";

      string tmp = strokeDesc(ss,tss,((tmarker!=NONE)&&(tmarker!=CLEAR)));
      if(tmp.size())
        ostr << " style=\"" << tmp << "\"";

      if( tmarker != NONE && tmarker != CLEAR )
      {
        string mname = cmarker.uniqueName();
        ostr << endl << tab << "   marker-end=\"url(#" 
          << mname << ")\" marker-mid=\"url(#" 
          << mname << ")\" marker-start=\"url(#"
          << mname << ")\"";
      }

      ostr << endl;

      outputPoints(line, ostr);

      // Close xml tag  
      ostr << "/>" << endl;
    }
  }

  /*
     <defs>
     <marker id="mDot1" markerUnits="strokeWidth" markerWidth="2" markerHeight="2" viewBox="0 0 2 2" refX="1" refY="1"
     stroke="red" stroke-width="1pt" fill="red">
     <circle cx="1" cy="1" r=".3"/>
     </marker>
     </defs>
     <polygon points="1,1 72,0 72,72 100,100 150,150 0,72" stroke="black" stroke-opacity="0" fill="none" 
     marker-end="url(#mDot1)" marker-mid="url(#mDot1)" marker-start="url(#mDot1)" />
   */

  void SVGImage::defineMarker (const Marker& marker, int dfltname)
  {
    if( lastMarker == marker ) return;

    using namespace std;

    string name = marker.uniqueName();
    //cout << name << endl;;

    string savedtab=tab;
    string marktab=string("         ");
    string intab=string("            ");

    ostr << tab << "<defs>\n";

    double r = marker.getRange();
    double r2 = 2*r;

    ostr << marktab << "<marker id=\"" << name << "\" markerUnits=\"strokeWidth\" "
      << "markerWidth=\"" <<  r2 << "\" markerHeight=\"" <<  r2 << "\" "
      << "viewBox=\"0 0 " << r2 << " " << r2 << "\" \n"
      << marktab << "        "  << "refX=\"" << r << "\" refY=\"" << r << "\" ";

    string col;
    {
      stringstream ss;
      ss.fill('0');      
      ss << '#' << hex << setw(6) << marker.getColor().getRGB();
      col = ss.str();
    }

    // These are here just in case.  
    ostr.fill('0');
    ostr << "stroke-width=\".2pt\" " << "fill=\"none\">\n"; 

    // Initial marker tab is set up...now to put the shape in...
    if (marker.hasDefaultMark())
    {
      Marker::Mark mark = marker.getMark();
      switch(mark)	
      {
        case Marker::DOT:
          ostr << intab << "<circle cx=\"" << r << "\" cy=\"" << r << "\" "
            <<"r=\"" << r << "\" style=\"fill:" << col << ";stroke:" << col << "\"/>\n";
          break;
        case Marker::PLUS:
          ostr << intab << "<line x1=\"0\" y1=\"" << r << "\" "
            << "x2=\"" << r2 << "\" y2=\"" << r << "\"/>\n";
          ostr << intab << "<line x1=\"" << r << "\" y1=\"0\" "
            << "x2=\"" << r << "\" y2=\"" << r2 << "\"/>\n";
          break;
        case Marker::X:
          ostr << intab << "<line x1=\"0\" y1=\"0\" "
            << "x2=\"" << r2 << "\" y2=\"" << r2 << "\"/>\n";
          ostr << intab << "<line x1=\"" << r2 << "\" y1=\"0\" "
            << "x2=\"0\" y2=\"" << r2 << "\"/>\n";
          break;
      }
    } 
    else
    {
      //This should not be called as BasicShapes were removed...but left 
      // here as a placeholder if it ever comes back.
      //Todo:
      // * Make sure marker color works as necessary (useMarkerColor)
      // * Add some to the tab variable?
      // * Need to fix shape coordinates?
      //ostr << intab << markerShape << endl;
    } 

    ostr << marktab << "</marker>\n";

    ostr << tab << "</defs>\n";

    markerDefined = true;
    lastMarker = marker;
  }



  void SVGImage::rectangle (const Rectangle& rect)
  {
    using namespace std;
    
    StrokeStyle ss;
    StyleType tss = getCorrectStrokeStyle(&ss,rect);
    Color fc;
    StyleType tfc = getCorrectFillColor(&fc,rect);

    if((tss==CLEAR) && (tfc==CLEAR || tfc==NONE))
    {
        comment("invisible rectangle ignored...");
        return;
    }

    ostr << tab;

    // open rect tag and specify x,y coords of the top left corner and 
    // the width and height of the box
    ostr << "<rect x=\"" << rect.x1 << "\" y=\"" << rect.y1
      << "\" width=\"" << rect.x2-rect.x1 << "\" height=\"" << rect.y2-rect.y1 << "\"";

    string f =  fill(fc,tfc);
    string tmp = strokeDesc(ss,tss,((tfc!=NONE)&&(tfc!=CLEAR)));
    if(tmp.size()||f.size())
      ostr << " style=\"" << f << tmp << "\"";

    // Close xml tag  
    ostr << "/>" << endl;
  }

  void SVGImage::circle (const Circle& circle)
  {
    using namespace std;

    StrokeStyle ss;
    StyleType tss = getCorrectStrokeStyle(&ss,circle);
    Color fc;
    StyleType tfc = getCorrectFillColor(&fc,circle);

    if((tss==CLEAR) && (tfc==CLEAR || tfc==NONE))
    {
        comment("invisible circle ignored...");
        return;
    }

    ostr << tab;

    ostr << "<circle cx=\"" << circle.xc << "\" cy=\"" << circle.yc
      << "\" r=\"" << circle.radius << "\"";

    string f =  fill(fc,tfc);
    string tmp = strokeDesc(ss,tss,((tfc!=NONE)&&(tfc!=CLEAR)));
    if(tmp.size()||f.size())
      ostr << " style=\"" << f << tmp << "\"";

    // Close xml tag  
    ostr << "/>" << endl;
  }

  void SVGImage::polygon (const Polygon& polygon) 
  {
    using namespace std;

    StrokeStyle ss;
    StyleType tss = getCorrectStrokeStyle(&ss,polygon);
    Color fc;
    StyleType tfc = getCorrectFillColor(&fc,polygon);

    if((tss==CLEAR) && (tfc==CLEAR || tfc==NONE))
    {
        comment("invisible polygon ignored...");
        return;
    }

    ostr << tab;

    ostr << "<polygon ";

    string f =  fill(fc,tfc);
    string tmp = strokeDesc(ss,tss,((tfc!=NONE)&&(tfc!=CLEAR)));
    if(tmp.size()||f.size())
      ostr << " style=\"" << f << tmp << "\"";

    outputPoints(polygon, ostr);

    // Close xml tag  
    ostr << "/>" << endl;
  }

  void SVGImage::bitmap (const Bitmap& bitmap) 
  {
    using namespace std;

    if(bitmap.boxes)
    {
      comment("Bitmap Boxes");
      rectangles(bitmap);
    }
    else
    {
      comment("Bitmap Image");      
      ostr << tab << "<image"
        << " x=\"" << bitmap.x1 << "\""
        << " y=\"" << bitmap.y1 << "\""
        << " width=\"" << bitmap.x2-bitmap.x1 << "\""
        << " height=\"" << bitmap.y2-bitmap.y1 << "\""
        << " filter=\"none\"" << endl
        << tab << "  image-rendering=\"optimizeSpeed\" preserveAspectRatio=\"none\"" << endl
        << tab << "  xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:show=\"embed\"" << endl;

      if(!ll)
        ostr << " transform=\"scale(1,-1) translate(0,-" << bitmap.y1+bitmap.y2 << ")\"" << endl;

      ostr << tab << "  xlink:href=\"data:image/png;base64,"; 
     
      ostr << Base64Encoder::encode(PNG::png(bitmap));

      ostr << "\"/>" << endl;
    }
  }

  void SVGImage::text (const Text& text) 
  {
    using namespace std;

    TextStyle ts;
    StyleType tts = getCorrectTextStyle(&ts,text);

    if( tts == CLEAR )
    {
      comment("Text with clear font ignored:%s",text.getString().c_str());
      return;
    }

    Color tc = ts.getColor();    
    int angle = text.getAngle();    

    ostr << tab;

    ostr << "<text x=\"" << text.x << "\" y=\"" << ((ll)?(canvasHeight-text.y):text.y) << "\"";

    //For the moment...to correct the transform
    if ( ll )
    {
      ostr << " transform=\"scale(1,-1) translate(0,-" << canvasHeight << ")";
      if ( angle )
        ostr << " rotate(" << setbase(10) << -angle << "," << text.x << "," 
          << ((ll)?(canvasHeight-text.y):text.y) << ")";
      ostr << "\""; 
    }
    else if ( angle )
      ostr << " transform=\"rotate(" << setbase(10) << -angle << "," << text.x << "," 
        << ((ll)?(canvasHeight-text.y):text.y) << ")\"";

    stringstream tmp;

    if(tc!=Color::BLACK)
    {
      tmp.fill('0');
      tmp << "fill:#" << hex << setw(6) << tc.getRGB() << ";";
    }

    if(ts.getPointSize()!=12)
      tmp << "font-size:" << ts.getPointSize() << "pt;";

    if(!ts.isMonospace())
      tmp << "font-family:" << (ts.isSerif()?"serif":(ts.isSansSerif()?"sans-serif":"")) << ";";
    //<< (ts.isMonospace()?"monospace":(ts.isSerif()?"serif":(ts.isSansSerif()?"sans-serif":""))) << ";"

    tmp << (ts.isBold()?"font-weight:bold;":"")
      << (ts.isItalic()?"font-style:italic;":"")
      << (ts.isUnderline()?"text-decoration:underline;":"")
      << (ts.isStrike()?"text-decoration:line-through;":"");

    if(!text.isLeft())
      tmp << "text-anchor:" << (text.isCenter()?"middle":"end") << ";";
    // << (text.isCenter()?"middle":(text.isLeft()?"start":"end")) << ";"

    string t = tmp.str();
    if(t.size()!=0)
      ostr << endl << tab << " style=\"" << t << "\"";

    // Close first xml tag  
    ostr << ">" << endl;

    ostr << tab << text.getString() << endl;

    ostr << tab <<"</text>" << endl;
   }

  void SVGImage::outputPoints(const Path& path, std::ostream& ostr)
  {
    using namespace std;

    // output each point
    ostr << tab << " points=";
    ostr << "\"";
    auto_ptr<Path> temp = path.asAbsolute();
    Path::const_iterator i;
    short count=1, width=5;
    for (i=temp->begin(); i!=temp->end(); i++, count++)
    {
      ostr << i->first << "," << i->second << " ";
      if (count == width )
      {
        count = 0;
        ostr << endl << tab << "         ";
      }
    }
    ostr << "\"";
  }

  void SVGImage::view(void) throw (VDrawException)
  {

    // close up the file's contents
    outputFooter();

    // First flush the file stream.
    ostr.flush();

    // Register viewers in case they haven't been registered.
    viewerManager.registerViewer("rsvg-view -b white");
    viewerManager.registerViewer("ksvg");
    viewerManager.registerViewer("inkscape");
    viewerManager.registerViewer("firefox");

    // Use the viewerManager
    viewerManager.view(filename);

    return;
  }

  std::string SVGImage::fill (Color& fc, StyleType tfc) const
  {
    if( tfc == NONE || tfc == CLEAR )
      return "";
    else
      return convertFillColor( fc );
  }

  std::string SVGImage::convertFillColor (const Color& color) const
  {
    using namespace std;
    stringstream ostr(stringstream::in | stringstream::out); 

    ostr.fill('0');
    ostr << "fill:#" << hex << noshowbase << setw(6) << color.getRGB() << ";";
    ostr.fill(' ');

    return string(ostr.str());
  }

  std::string SVGImage::strokeDesc (StrokeStyle& ss, StyleType tss, bool noneclear) const
  {
    if( tss == NONE )
    {
      if( noneclear )
        return "stroke-opacity:0;";        
      else
        return "";
    }
    else if( tss == CLEAR )
      return "stroke-opacity:0;";
    else
      return convertStrokeStyle(ss);
  }

  std::string SVGImage::convertStrokeStyle (const StrokeStyle& ss) const
  {
    using namespace std;
    stringstream ostr(stringstream::in | stringstream::out); 

    if(ss.getColor().isClear())
    {
      ostr << "stroke-opacity:0;";
      return string(ostr.str());        
    }

    // Color
    if(ss.getColor()!=Color::BLACK)
    {
      ostr << "stroke:rgb(";
      short red, blue, green;
      ss.getColor().getRGBTriplet(red, blue, green);
      ostr << red << "," << blue << "," << green;
      ostr << ");";
    }

    // Width
    if(ss.getWidth()!=1)
      ostr << "stroke-width:" << ss.getWidth() << "pt;";

    if (!ss.getSolid())
    {
      // Dash array list: comma separated variables
      ostr << "stroke-dasharray:";

      StrokeStyle::dashLengthList dll=ss.getDashList();
      StrokeStyle::dashLengthList::iterator i, inext;

      for (i=dll.begin(); i!=dll.end(); i++)
      {
        ostr << (i==dll.begin()?"":",") << *i;
        inext = i; inext++; 
        if (inext!=dll.end())
          cout << ",";
      }
      // Final delimiter
      ostr << ";";   
    }

    return string(ostr.str());
  }


} // namespace vdraw
