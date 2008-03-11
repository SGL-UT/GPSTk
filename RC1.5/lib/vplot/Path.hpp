
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


#ifndef VPLOT_PATH_H
#define VPLOT_PATH_H

#include<string>
#include<list>
#include<utility> // Provides "pair" definition

#include "VPlotException.hpp"

namespace vplot
{
   /** \addtogroup BasicVectorGraphics */ 
   //@{

   /**
    * This class describes a point as an (x,y) coordinate.
    */
   struct Point {

      /// X coordinate or abscissa
      double x;

      /// Y coordinate or ordinate
      double y;
    
   };

   /**
    * This class defines a sequence of points. The points can be used
    * to specify a polygon, a line, Bezier curve, or anything that
    * requires multiple points as input. The points are relative to
    * an origin point. The origin point is in the page refrence frame. 
    * Note that this class inherits
    * from list<pair<double, double>, which means that it provides the
    * standard C++ container interface for just what it is, a list of points.
    * But there are additional methods for easy construction and use of this
    * dataset.
    * TODO: Consider using another container class for which lookups are O^n.
    */
   class Path: public std::list< std::pair<double,double> > {

   public:
      
      /**
       * Default constructor. An empty path.
       * @param iOriginX absolute X coordinate of the origin in points
       * @param iOriginY absolute Y coordinate of the origin in points
       */
      Path(double iOriginX, double iOriginY) 
            : originX(iOriginX), originY(iOriginY)
         {}

      /**
       * Sets a new origin point.
       * @param X x coordinate of new origin.
       * @param Y y coordinate of new origin
       */
      void setOrigin(double X, double Y)
      { originX = X; originY = Y; return; }

      /**
       * Sets input vaiables equal to the origin values.
       * @param X to be x coordinate of origin
       * @param Y to be y coordinate of origin
       */
      void getOrigin(double& X, double& Y) const
         { X = originX; Y = originY; return; }
      
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
     void addPointDelta(double DX, double DY) throw (VPlotException);

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

        /// Obtain coordinates to the global frame
     Path asAbsolute(void) const;
      
   protected:

   private:
      /// The x origin for the path.
      double originX; 
      /// The y origin for the path.
      double originY;

   }; // class Path

   //@}

} // namespace vplot

#endif //VPLOT_PLOT_PATH_H

