#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/PNG.cpp#3 $"

/// @file PNG.cpp Create PNG images.  Class defintions.

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

#include "PNG.hpp"

namespace vdraw
{
  const std::string PNG::header = "\211PNG\r\n\032\n";

  std::string PNG::png(const Bitmap &b)
  {
    InterpolatedColorMap icm;
    ColorMap cm;
    if(b.getICM(&icm))
    {
      return *png(icm,b.osr,b.osc);
    }
    else if(b.getCM(&cm))
    {
      return *png(cm,b.osr,b.osc); 
    }
    else
    { 
      printf("PNG::png(const Bitmap &b); Invalid Bitmap.\n");
      exit(1);
    }
  }

  PNG::string_ptr PNG::png(const ColorMap &c, int osr, int osc)
  {
    std::stringstream s;
    s << header 
      << *ihdr_full(c.getCols()*osc,c.getRows()*osr)
      << *srgb() 
      //<< phys()
      << *idat(c,osr,osc)
      << *iend();
    return string_ptr(new std::string(s.str()));
  }

  PNG::string_ptr PNG::png(const InterpolatedColorMap &c, int osr, int osc)
  {
    std::stringstream s;
    s << header 
      << *ihdr_indexed(c.getCols()*osc,c.getRows()*osr)
      << *srgb() 
      //<< phys()
      << *plte(c)
      << *idat(c,osr,osc)
      << *iend();
    return string_ptr(new std::string(s.str()));
  }

  /*
   * TODO Future ideas for optimization
   * -- Indexed Color Map 
   *    When it is optimal to use the non-indexed version, by these simple
   *    calculations then it would be worthwhile to make an indexed color map
   *    which might end up with far fewer colors to index
   */

  int PNG::cost_indexed(const Bitmap& b)
  {
    InterpolatedColorMap icm;
    if(b.getICM(&icm))
    {
      return cost_indexed(icm.getRows()*b.osr,icm.getCols()*b.osc,256);
    }
    return -1;
  }
  
  int PNG::cost_indexed(int rows, int cols, int numcol)
  {
    int i = 8;          // PNG
    i += 13 + 12;       // IHDR Len + 12 Chunk bytes
    i += 1  + 12;       // sRGB Len + 12 Chunk bytes
    i += numcol*3 + 12; // PLTE Len + 12 Chunk bytes
    i += cost_idat(rows*(cols+1));   // IDAT
    i += 12;            // IEND
    return i;
  }

  int PNG::cost_constant(const Bitmap& b)
  {
    InterpolatedColorMap icm;
    ColorMap cm;
    if(b.getICM(&icm))
    {
      return cost_constant(icm.getRows()*b.osr,icm.getCols()*b.osc);
    }
    else if(b.getCM(&cm))
    {
      return cost_constant(cm.getRows()*b.osr,cm.getCols()*b.osc);
    }
    return -1;
  }

  int PNG::cost_constant(int rows, int cols)
  {
    int i = 8;    // PNG
    i += 13 + 12; // IHDR Len + 12 Chunk bytes
    i += 1  + 12; // sRGB Len + 12 Chunk bytes
    i += cost_idat(rows*(3*cols+1)); // IDAT
    i += 12;      // IEND
    return i;
  }

  int PNG::cost_idat(int stream)
  {
    int i = stream;
    // Huffman bits, 5 per 0xFFFF and 5 for whats left
    i += 5*(stream>>16 + (stream&0xFFFF?1:0));
    i += 4; // Adler-32 checksum
    i += 2; // zlib bytes
    i += 12*(stream/0x2000 + (stream%0x2000?1:0)); //Chunk bits (if split)
    return i;
  }

  PNG::string_ptr PNG::ihdr_full(int width, int height)
  {
    std::stringstream s;
    s << *itos(width)  // width              -- 4 bytes
      << *itos(height) // height             -- 4 bytes
      << btoc(8)      // bit depth          -- 1 byte
      << btoc(2)      // color type         -- 1 byte -- Truecolor
      << btoc(0)      // compression method -- 1 byte
      << btoc(0)      // filter method      -- 1 byte
      << btoc(0);     // interlace method   -- 1 byte
    return chunk("IHDR",s.str());
  }

  PNG::string_ptr PNG::ihdr_indexed(int width, int height)
  {
    std::stringstream s;
    s << *itos(width)  // width              -- 4 bytes
      << *itos(height) // height             -- 4 bytes
      << btoc(8)      // bit depth          -- 1 byte
      << btoc(3)      // color type         -- 1 byte -- Interpolated
      << btoc(0)      // compression method -- 1 byte
      << btoc(0)      // filter method      -- 1 byte
      << btoc(0);     // interlace method   -- 1 byte
    return chunk("IHDR",s.str());
  }


  PNG::string_ptr PNG::srgb()
  {
    std::stringstream s;
    s << btoc(0);     // We want the colors to look good
    return chunk("sRGB",s.str());
  }

  PNG::string_ptr PNG::phys()
  {
    std::stringstream s;
    s << *itos(1)  // Pixels per unit x axis
      << *itos(1)  // Pixels per unit y axis
      << btoc(1); // We know the unit
    return chunk("pHYs",s.str());
  }

  PNG::string_ptr PNG::plte(const InterpolatedColorMap &c)
  {
    // TODO Smaller palette on request?
    Palette p = c.getPalette();
    std::stringstream s;
    for(int i=0;i<256;i++)
      s << *ctos(p.getColor(i/256.0));
    return chunk("PLTE",s.str());
  }

  /*
   * ZLIB flag information
   * First two bytes:
   * +-----+-----+
   * | CMP | FLG |
   * +-----+-----+
   * CMP:
   *  - Bits 0-3 = 0x08 (deflate compression)
   *  - Bits 4-7 = For CM = 8, CINFO is the base-2 logarithm 
   *    of the LZ77 window size, minus eight
   *
   */

  PNG::string_ptr PNG::idat(const InterpolatedColorMap &c, int osr, int osc)
  {
    std::stringstream s;
    string_ptr tmp = data(c,osr,osc);
    unsigned int a = alder(tmp);
    s << *huff(*tmp)
      << *itos(a);
    tmp = string_ptr(new std::string(s.str()));
    return split(*prefix(*tmp));
  }

  PNG::string_ptr PNG::idat(const ColorMap &c, int osr, int osc)
  {
    std::stringstream s;
    string_ptr tmp = data(c,osr,osc);
    unsigned int a = alder(tmp);
    s << *huff(*tmp)
      << *itos(a);
    tmp = string_ptr(new std::string(s.str()));
    return split(*prefix(*tmp));
  }

  PNG::string_ptr PNG::split(const std::string &str)
  {
    std::stringstream s;
    int len = str.size();
    int pos = 0;
    do
    {
      //printf("split() :: len=%08x pos=%08x \n",len,pos);
      int dist=0;
      if(len>0x02000)
        dist = 0x02000;
      else
        dist = len;
      len -= dist;

      s << *chunk("IDAT",str.substr(pos,dist));

      pos += dist;
    } while(len);
    return string_ptr(new std::string(s.str()));            
  }

  PNG::string_ptr PNG::prefix(const std::string &str)
  {
    // TODO There are limits to IDAT length...check and split
    int len = str.size();
    int cmp = (int)(log(len)/log(2))-8;
    if(cmp<0) cmp = 0;
    else if(cmp>7) cmp = 7;
    cmp = (cmp<<4) | 0x08;
    int flg = 0;
    int tmp = (cmp*256+flg)%31;
    if(tmp!=0) flg += 31-tmp;
    std::stringstream s;
    // TODO ZLIB data stream...? Adler-32
    s << btoc(cmp)          // See CMP notes above
      << btoc(flg)          // Flag
      << str;
    return string_ptr(new std::string(s.str()));            
  }

  PNG::string_ptr PNG::huff(const std::string &str)
  {
    // Add huff information
    std::stringstream s;
    int len = str.size();
    int pos = 0;
    do
    {
      //printf("huff() :: len=%08x pos=%08x\n",len,pos);          
      int dist=0;
      if(len>0x0FFFF)
        dist = 0x0FFFF;
      else
        dist = len;
      len -= dist;

      // These go backwards
      s << btoc((len?0x00:0x01)) // Set final bit on last block
        << btoc(dist) 
        << btoc(dist>>8)
        << btoc(~dist)
        << btoc((~dist)>>8)
        << str.substr(pos,dist);

      pos += dist;
    } while(len);
    return string_ptr(new std::string(s.str()));            
  }

  PNG::string_ptr PNG::data(const ColorMap &c, int osr, int osc)
  {
    // For oversampling, we make a row buffer and a column one and repeat as
    // necessary to create the image.
    std::stringstream s;
    for(int row=0; row<c.getRows(); row++)
    {
      std::stringstream r;
      r << btoc(0x00);   // Filter method 0 (identity)
      for(int col=0; col<c.getCols(); col++)
      {
        string_ptr t = ctos(c.get(row,col));
        for(int cc=0; cc<osc; cc++) 
          r << *t;
      }
      std::string rstr = r.str();
      for(int rr=0; rr<osr; rr++) 
        s << rstr;
    }
    return string_ptr(new std::string(s.str()));            
  }

  PNG::string_ptr PNG::data(const InterpolatedColorMap &c, int osr, int osc)
  {
    // For oversampling, we make a row buffer and a column one and repeat as
    // necessary to create the image.
    std::stringstream s;
    for(int row=0; row<c.getRows(); row++)
    {
      std::stringstream r;
      r << btoc(0x00);   // Filter method 0 (no filter)
      for(int col=0; col<c.getCols(); col++)
      {
        char t = btoc((int)(c.getIndex(row,col)*255));
        for(int cc=0; cc<osc; cc++) 
          r << t;
      }
      std::string rstr = r.str();      
      for(int rr=0; rr<osr; rr++) 
        s << rstr;
    }
    return string_ptr(new std::string(s.str()));            
  }


  unsigned int PNG::alder(const PNG::string_ptr &str)
  {
    Adler32 a;
    a.update(str);
    return a.getValue();
  }

  PNG::string_ptr PNG::iend()
  {
    return chunk("IEND","");
  }

  PNG::string_ptr PNG::chunk(const std::string &title, const std::string &text)
  {
    CRC32 c;
    c.update(title);
    c.update(text);
    std::stringstream s;
    s << *itos(text.length())
      << title
      << text
      << *itos(c.getValue());
    return string_ptr(new std::string(s.str()));    
  }

  PNG::string_ptr PNG::itos(int i)
  {
    std::stringstream s;
    s << btoc((i>>24)&0x000000FF)
      << btoc((i>>16)&0x000000FF)
      << btoc((i>>8) &0x000000FF)
      << btoc(i      &0x000000FF);
    return string_ptr(new std::string(s.str()));        
  }

  PNG::string_ptr PNG::ctos(const Color &color)
  {
    unsigned int l = color.getRGB();
    std::stringstream s;
    s << btoc((l>>16)&0x000000FF)
      << btoc((l>>8) &0x000000FF)
      << btoc(l      &0x000000FF);
    return string_ptr(new std::string(s.str()));            
  }


  char PNG::btoc(int b)
  {
    return (char)(b&0x000000FF);
  }

} // namespace vdraw


