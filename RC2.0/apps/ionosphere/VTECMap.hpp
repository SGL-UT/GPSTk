#pragma ident "$Id$"

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

/**
 * @file VTECMap.hpp
 * VTECMap implements a simple ionospheric model using least squares
 * and slant TEC values from multiple stations.
 */

#ifndef GPSTK_VTECMAP_INCLUDE
#define GPSTK_VTECMAP_INCLUDE

//------------------------------------------------------------------------------------
#include "Exception.hpp"
#include "StringUtils.hpp"
#include "DayTime.hpp"

#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"

#include "WGS84Geoid.hpp"
#include "Position.hpp"
#include "icd_200_constants.hpp"     // for TWO_PI
#include "geometry.hpp"              // for DEG_TO_RAD and RAD_TO_DEG
#include "MiscMath.hpp"              // for RSS

#include <iostream>
#include <string>
#include <vector>
#include <map>

//------------------------------------------------------------------------------------
namespace gpstk
{

//------------------------------------------------------------------------------------
/// class Station TD document
class Station {
public:
   //string id;  use header.markerName
   int nfile;                 // this object is Stations[nfile]
   std::string filename;
   //RinexObsStream instream; // cannot make vector<Station> for lack of copy c'tor
   RinexObsHeader header;
   RinexObsData robs;
   bool getNext;      // if true, ready to read next epoch
   int nread;         // number of obs records read or (-1 = dead, 0=header read)
   //int inC1,inL1,inL2,inP1,inP2,inD1,inD2,inS1,inS2;        // indexes in header
   Position xyz;                         // cartesian position (ECEF)
   Position llr;                         // geocentric position in LLR
   std::map<SatID,DayTime> InitTime;     // time of start of this (cont) phase seg

   friend std::ostream& operator<<(std::ostream& os, const Station& f);
};

/// Data collected from each station/satellite pair, used to construct the TEC map
class ObsData {
public:
   double AcqTime;   ///< acquisition time: time since start of continuous phase (s)
   double VTEC;      ///< vertical TEC (TECU?)
   double VTECerror; ///< computed error in the VTEC
   double elevation; ///< elevation angle (degrees)
   double azimuth;   ///< azimuth angle (degrees)
   double latitude;  ///< latitude of the ionospheric pierce point (degrees)
   double longitude; ///< longitude of the ionospheric pierce point (degrees E)
};

/// Data defining the map at each grid point: grid location and TEC value
class GridData {
public:
   Position XYZ;    ///< ECEF position (x,y,z) in meters
   Position LLR;    ///< geocentric position (lat,lon,radius) in deg,degE,meters
   double value;    ///< computed map value at this grid point (TECU?)
};

//------------------------------------------------------------------------------------
/// class VTECMap stores and computes a grid in latitude and longitude, then given
/// VTEC data over a network of ground stations, computes the value of VTEC on
/// the grid.
class VTECMap {
public:
      /// Supported grid types
   enum GridType
   {
      UniformLatLon=1,       ///< Grid is uniform in (geocentric) latitude, longitude
      UniformSpace           ///< Grid is uniform in space (constant physical spacing)
   };

      /// Supported fit types
   enum FitType
   {
      Constant,         ///< Model ionospheric TEC as constant
      Linear            ///< Model ionospheric TEC as linear function of lat,lon
   };

      /// default constructor
   VTECMap() { grid=NULL; SetDefaults(); }

      /// destructor
   virtual ~VTECMap() { if(grid) delete[] grid; }

      /// copy input data
   void CopyInputData(VTECMap &right);

      /// default values of input data
   void SetDefaults();

      /// build the 2D grid itself
      /// @param refStation Station object for the reference station
   virtual void MakeGrid(Station& refStation) throw(Exception)
      { reallyMakeGrid(refStation,1); }

      /// write grid locations to a file
      /// @param ostream on which to write
   void OutputGrid(std::ostream& os);

      /// populate the 2D grid
      /// @param epoch time of interest
      /// @param data vector of ObsData structures for all observed data
      /// @param bias overall bias to add to vertical TEC data
   virtual void ComputeMap(DayTime& epoch, std::vector<ObsData>& data, double bias);

      /// write the computed grid values to a file
      /// @param ostream on which to write
      /// @param bool gnuplotFormat if true, output for gnuplot,
      ///    otherwise (default) for Matlab (TD right?)
   void OutputMap(std::ostream& os, bool gnuplotFormat=false);

      // input data
   double Decorrelation;      ///< decorellation error rate in TECU/1000km
   double MinElevation;       ///< minimum elevation in degrees
   double IonoHeight;         ///< Height of the ionosphere in meters
   GridType gridtype;         ///< uniform in space or uniform in lat/lon
   FitType fittype;           ///< constant or linear
   double BeginLat;           ///< beginning latitude (deg)
   double DeltaLat;           ///< step in latitude (deg)
   int NumLat;                ///< number of latitude grids
   double BeginLon;           ///< beginning longitude (deg E)
   double DeltaLon;           ///< step in longitude (deg)
   int NumLon;                ///< number of longitude grids
   Station RefStation;        ///< reference station, input by MakeGrid()

      // grid and map data
   GridData *grid;
   double ave;

      // constants needed in computation
   static const double VTECErrorMultipath; ///< TD doc
   static const double VTECErrorSat;       ///< TD doc
   static const double VTECErrorCFC[4];    ///< TD doc
   static const double ObliqCoef[4];       ///< Parameters used in obliquity calc.

      // computation

      /// Convert vertical TEC to foF2 using one of two methods.
      /// @param method flag giving methods:
      ///   0 Constant slab of thickness tau=280 km
      ///   1 Cubic fit to tau from ARL data
      /// @param vtec Vertical TEC
      /// @param epoch TD doc
      /// @param lon Longitude of TD doc
   double VTECtoF0F2(int method, double vtec, DayTime& epoch, double lon);

      /// Compute the error on the VTEC
      /// @param t acquisition time in sec ?
      /// @param el elevation in degrees
      /// @param vtec vertical TEC (TECU)
      /// @return error on the VTEC
   double VTECError(double t, double el, double vtec);

      /// Compute the obliquity at a given elevation
      /// @param el elevation in degrees
      /// @return obliquity factor
   double Obliquity(double el);

protected:
      /// Allocate the grid array and fill it.
      /// @param refStation reference station
      /// @param factor : 1 for VTEC maps, 2 for MUF maps
   void reallyMakeGrid(Station& refStation, int factor)
      throw(Exception);

      /// Compute one grid value, using all the data. Add a bias b to all the data.
      /// Called by ComputeMap.
   void ComputeGridValue(GridData& gridpt, std::vector<ObsData>& data, double b);

      /// Chi squared calculation ... TD
   double ChiSqPlane(std::vector<double>& vtec, std::vector<double>& x,
     std::vector<double>& y, std::vector<double>& sigma);

}; // end class VTECMap

/// class MUFMap is a VTECMap that computes MUF on the grid points.
class MUFMap : public VTECMap {
public:
      /// build the 2D grid itself
      /// @param refStation Station object for the reference station
   void MakeGrid(Station& refStation) throw(Exception)
         { reallyMakeGrid(refStation,2); }

      /// populate the 2D grid
      /// @param epoch time of interest
      /// @param data vector of ObsData structures for all observed data
      /// @param bias overall bias to add to vertical TEC data
   void ComputeMap(DayTime& epoch, std::vector<ObsData>& data, double bias);
};

/// class MUFMap is a VTECMap that computes F0F2 on the grid points.
class F0F2Map : public VTECMap {
public:
      /// populate the 2D grid
      /// @param epoch time of interest
      /// @param data vector of ObsData structures for all observed data
      /// @param bias overall bias to add to vertical TEC data
   void ComputeMap(DayTime& epoch, std::vector<ObsData>& data, double bias);
};

//------------------------------------------------------------------------------------
// prototypes

/// Compute the coefficients [a,b,c] in the equation z = a*x + b*y + c that describes
/// a plane, given the cartesian coordinates [x,y,z] of three points p1,p2,p3 that
/// define the plane.
/// @param cof double[3] of coefficients (output)
/// @param p1 double[3] of coordinates
/// @param p2 double[3] of coordinates
/// @param p3 double[3] of coordinates
void PlaneCoefficients(double cof[3], double p1[3], double p2[3], double p3[3])
   throw(Exception);

//------------------------------------------------------------------------------------
} // namespace gpstk

//------------------------------------------------------------------------------------
#endif   // GPSTK_VTECMAP_INCLUDE
