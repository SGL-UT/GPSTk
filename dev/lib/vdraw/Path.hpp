#pragma ident "$Id$"

/// @file Path.hpp Class to represent a series of points. Class declarations.

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


#ifndef VDRAW_PATH_H
#define VDRAW_PATH_H

#include<string>
#include<vector>
#include<utility> // std::pair
#include<memory>  // std::auto_ptr

// TODO In the future?
// #include "boost/shared_ptr.hpp" // boost::shared_ptr
// To use shared_ptr instead of auto_ptr

#include "GraphicsConstants.hpp"
#include "VDrawException.hpp"

namespace vdraw
{
  /** \addtogroup BasicVectorGraphics */ 
  //@{

  /**
   * This class describes a point as an (x,y) coordinate.  
   * Previously this had a reference for x and y to first and second, but that
   * seems to be a memory waste.  So those were replaced with methods x() and
   * y().
   */
  struct Point : public std::pair<double, double>
  {
    /// Constructor
    Point(double ix=0.0, double iy=0.0)
      : std::pair<double,double>(ix,iy)
    {
    }

    /// Copy Constructor
    Point(const Point& p)
      : std::pair<double,double>(p.x(),p.y())
    {
    }

    /// Conversion constructor.
    Point(const std::pair<double,double>& o)
      : std::pair<double,double>(o)
    {
    }

    /// Get a reference to x value
    double& x() { return first; }

    /// Get a constant reference to x value
    const double& x() const { return first; }

    /// Get a reference to y value
    double& y() { return second; }

    /// Get a constant reference to y value
    const double& y() const { return second; }

  };

  /**
   * This class defines a sequence of points. The points can be used
   * to specify a polygon, a line, Bezier curve, or anything that
   * requires multiple points as input. The points are relative to
   * an origin point. The origin point is in the page refrence frame. 
   * 
   * Note that this class inherits from std::vector< Point >, 
   * which means that it provides the standard C++ container interface 
   * for a vector of Points. But there are additional  methods for easy 
   * construction and use of this dataset.
   *
   * It is suggested that the users of this class, call a constructor with an
   * estimated size for the vector, as only one allocation will occur
   * (hopefully) and this should signifigantly speed up things.  This number
   * can be overestimated (at the cost of space if not all of it is used. unless
   * followed by a call to tighten()) or underestimated (which will ensure that 
   * space isn't wasted but at a cost of time for reallocation if points are 
   * added over the estimate).
   */
  class Path: public std::vector< Point > 
  {
    public:

      /**
       * Default constructor. An empty path.
       * @param estimated_size An estimate of the number of elements to be
       * placed in this vector.
       */
      Path(unsigned int estimated_size=0) 
        : originX(0.0), originY(0.0)
      {
        if(estimated_size)
          reserve(estimated_size);
      }

      /**
       * Default constructor. An empty path.
       * @param iOriginX absolute X coordinate of the origin in points
       * @param iOriginY absolute Y coordinate of the origin in points
       * @param estimated_size An estimate of the number of elements to be
       * placed in this vector.
       */
      Path(double iOriginX, double iOriginY, unsigned int estimated_size=0) 
        : originX(iOriginX), originY(iOriginY)
      {
        if(estimated_size)
          reserve(estimated_size);
      }

      /**
       * Copy/convert constructor from a vector of pairs.
       * @param v vector of pairs to copy into this Path
       */
      Path(const std::vector< std::pair<double,double> >& v, double iOriginX=0, double iOriginY=0)
      {
        originX = iOriginX;
        originY = iOriginY;
        reserve(v.size());
        std::vector< std::pair<double,double> >::const_iterator i;
        for(i=v.begin(); i!=v.end(); i++)
        {
          if(size()==0 
              || (i->first != (*this)[size()-1].x()) 
              || (i->second != (*this)[size()-1].y()))
            push_back(Point(*i));
        }
        tighten();
      }

      /**
       * If the size was overestimated, then this should deallocate most of the
       * remaining empty space.  Only call this after the Path has been
       * completely filled with points. 
       */
      inline void tighten()
      {
        resize(size());
      }

      /**
       * Sets a new origin point.
       * @param X x coordinate of new origin.
       * @param Y y coordinate of new origin
       */
      inline void setOrigin(double X, double Y)
      { 
        originX = X; 
        originY = Y; 
        return; 
      }

      /**
       * Sets input vaiables equal to the origin values.
       * @param X to be x coordinate of origin
       * @param Y to be y coordinate of origin
       */
      inline void getOrigin(double& X, double& Y) const
      { 
        X = originX; 
        Y = originY; 
        return; 
      }

      /**
       * Mutator. Adds a new point in the absolute frame.
       * @param X X-coordinate of the new point in the absolute frame
       * @param Y Y-coordinate of the new point in the absolute frame
       */ 
      void addPointAbsolute(double X, double Y);

      /**
       * Mutator. Adds a new point, with respect to the path's origin.
       * Note the origin is not the starting point of the path but a special
       * point that is used to simplify rotation and translation of the path
       * as a whole.
       * @param X X-coordinate of the new point in the path's frame
       * @param Y Y-coordinate of the new point in the path's frame
       */ 
      void addPointRelative(double X, double Y);

      /**
       * Mutator. Adds a new pont, relative to the last point added
       * @param DX DX distance along X axis to new point from old one 
       * @param DY DY distance along Y axis to new point from old one 
       */ 
      void addPointDelta(double DX, double DY) throw (VDrawException);

      /**
       * Rotate the set of points about a pivot point.
       * @param angleDegrees Angle of rotation in degrees
       * @param rx X coordinate of point of rotation. Defaults to start point
       * @param ry Y coordinate of point of rotation. Defaults to start point
       */
      void rotate(double angleDegrees, double rx, double ry);

      /**
       * Rotate the set of points about the first point in the series.
       * @param angleDegrees Angle of rotation in degrees
       */
      void rotate(double angleDegrees);

      /**
       * Move the set of points.
       * @param deltaX Increment of x coordinates to move the line
       * @param deltaY Increment of y coordinates to move the line
       */
      void translate(double deltaX, double deltaY);

      /**
       * Scale the set of points with respect to given point.
       * @param factor Dimensionless scaling factor
       * @param sx X coordinate of point of scaling. Defaults to start point
       * @param sy Y coordinate of point of scalling. Defaults to start point
       */
      void scale(double factor, double sx, double sy);

      /**
       * Obtain coordinates to the global frame.  The use of std::auto_ptr here
       * is so that the array created within this method is not immediately
       * copied when the function returns, and that it is deleted at some point
       * so recreating large vectors doesn't cause huge memory leaks. Copying
       * upon return when returning by value is very costly, linear with the
       * size of the underlying vector.  Likewise, instead of just giving
       * a pointer back and hoping that the user of this method uses delete,
       * auto_ptr will guarantee that it is deleted when the auto_ptr object
       * itself is deleted.  
       */
      std::auto_ptr<Path> asAbsolute(void) const;

    protected:

    private:
      /// The x origin for the path.
      double originX; 
      /// The y origin for the path.
      double originY;

  }; // class Path

  //@}

} // namespace vdraw

#endif //VDRAW_PLOT_PATH_H

