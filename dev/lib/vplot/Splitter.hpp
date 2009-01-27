#pragma ident "$Id$"

/// @file Splitter.hpp Used to help with splitting sets of points. Class
/// declarations.

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

#ifndef VPLOT_SPLITTER_H
#define VPLOT_SPLITTER_H

#include <vector>
#include <list>
#include <map>
#include <memory>

#include "Path.hpp"

namespace vplot
{
  /**
   * This is a helper class that assists with cutting a set of points along a
   * line.
   */
  class Splitter
  {
  public:
    /**
     * Split the Path p at splitter.
     * @param splitter Where to split the Path
     * @param p Path to split
     * @param top If the top should be returned 
     * @param bottom If the bottom should be returned
     * @param continuous Create interpolated points at the split when necessary
     * @return The top and bottom lists of Path's.  If a side was specified not
     * to return, it will be an empty list.
     */
    static std::pair<std::list<vdraw::Path*>*,std::list<vdraw::Path*>*> 
      splith(double splitter, vdraw::Path* p, bool top, bool bottom, bool continuous=false);

    /**
     * Split the given Path whenever there is a horizontal gap (x2-x1) greater than the given gap
     * between two points.
     * @param gap The distance between which to cut the given path
     * @param p The given path
     * @return A list of Paths split from the original
     */
    static std::list<vdraw::Path*>* splitvgap(double gap, vdraw::Path* p);

    /// Determine if p is within the bounds of the box given by minX, maxX,
    /// minY, and maxY
    inline static bool inBox(const std::pair<double,double>& p, 
        double minX, double maxX, double minY, double maxY)
    {
      return !((p.first<minX || p.first>maxX) || (p.second<minY || p.second>maxY));
    }

    /**
     *   minX   maxX
     * I   | II  | III
     * ----+-----+----- maxY
     * IV  | BOX | V
     * ----+-----+----- minY
     * VI  | VII | VIII
     * The check is performed in this order:
     * - minX 
     *   - Return if valid intersection, otherwise defer to y check
     * - maxX
     *   - Return if valid intersection, otherwise defer to y check
     * - minY
     *   - Return valid intersection
     * - maxY
     *   - Return valid intersection
     * 
     * In other words, if the outside point is in one of the corner regions (I,
     * III, VI, VIII) then there are two lines that the intersection can be on,
     * and only one of the intersection points will be in the box (unless the
     * intersection happens to be the intersection of the three lines).  If the
     * first try with the x boundary fails, the next check with the y boundary
     * must pass.
     *
     * @param inside A point inside of the box
     * @param outside A point outside of the box
     * @param minX The minimum x value of the box
     * @param maxX The maximum x value of the box
     * @param minY The minimum y value of the box
     * @param maxY The maximum y value of the box
     * @return interpolated point on the box
     */
    static std::pair<double,double> intersectBox(
        const std::pair<double,double> inside, const std::pair<double,double> outside, 
        double minX, double maxX, double minY, double maxY);

    /**
     * This method cuts up the given path by interpolating a new point every
     * time the imaginary line would cross the box's border.  This is a linear
     * interpolation between the two points.  
     * @param minX The minimum x value of the box
     * @param maxX The maximum x value of the box
     * @param minY The minimum y value of the box
     * @param maxY The maximum y value of the box
     * @param p The set of points to cut up where it intersects the box
     * @return Sets of lines with interpolated points on the edge of the box
     */
    static std::auto_ptr< std::list< vdraw::Path > > interpToBox(double minX, double maxX, double minY, double maxY, const vdraw::Path& p);

    /**
     * Crop the points to fit the box.  This particular one would be useful for
     * non-line plots, as external points are simply omitted.
     * @param minX The minimum x value of the box
     * @param maxX The maximum x value of the box
     * @param minY The minimum y value of the box
     * @param maxY The maximum y value of the box
     * @param p The set of points to crop
     * @return The set of cropped points
     */
    static std::auto_ptr< vdraw::Path > cropToBox(double minX, double maxX, double minY, double maxY, const vdraw::Path& p);

  private:
    /// Get the x value where a line between the two points p1 and p2 would 
    /// intersect with y
    static inline double intersecth(double y, const std::pair<double,double>& p1, const std::pair<double,double>& p2)
    {
      double ratio = (y - p1.second) / (p2.second - p1.second);
      double xdist = p2.first - p1.first;
      return p1.first + xdist*ratio;
    }

    /// Get the y value where a line between the two points p1 and p2 would 
    /// intersect with x
    static inline double intersectv(double x, const std::pair<double,double>& p1, const std::pair<double,double>& p2)
    {
      double ratio = (x - p1.first) / (p2.first - p1.first);
      double ydist = p2.second - p1.second;
      return p1.second + ydist*ratio;
    }
  };
}

#endif
