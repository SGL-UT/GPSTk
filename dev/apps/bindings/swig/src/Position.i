#pragma ident "$Id$"
#ifndef GPSTK_POSITION_HPP
#define GPSTK_POSITION_HPP
#include "Exception.hpp"
#include "StringUtils.hpp"
#include "Triple.hpp"
#include "EllipsoidModel.hpp"
#include "ReferenceFrame.hpp"
#include "Xvt.hpp"

namespace gpstk
{
   class Position;
   // renamed to rangeBetween
   // double range(const Position& A, const Position& B) throw(GeometryException);

   class Position : public Triple
   {
   public:
      enum CoordinateSystem
      {
         Unknown=0,  ///< unknown coordinate system
         Geodetic,   ///< geodetic latitude, longitude, and height above ellipsoid
         Geocentric, ///< geocentric (regular spherical coordinates)
         Cartesian,  ///< cartesian (Earth-centered, Earth-fixed)
         Spherical   ///< spherical coordinates (theta,phi,radius)
      };

      std::string getSystemName()
         throw();

      static const double ONE_MM_TOLERANCE;
      static const double ONE_CM_TOLERANCE;
      static const double ONE_UM_TOLERANCE;

      static double POSITION_TOLERANCE;

      static double setPositionTolerance(const double tol)
         { POSITION_TOLERANCE = tol;  return POSITION_TOLERANCE; }

      static double getPositionTolerance()
         { return POSITION_TOLERANCE; }

      Position& setTolerance(const double tol)
         throw();

      Position()
         throw();

      Position(const double& a,
               const double& b,
               const double& c,
               CoordinateSystem s = Cartesian,
               EllipsoidModel *ell = NULL,
               ReferenceFrame frame = ReferenceFrame::Unknown)
         throw(GeometryException);

      Position(const double ABC[3],
               CoordinateSystem s = Cartesian,
               EllipsoidModel *ell = NULL,
               ReferenceFrame frame = ReferenceFrame::Unknown)
         throw(GeometryException);

      Position(const Triple& ABC,
               CoordinateSystem s = Cartesian,
               EllipsoidModel *ell = NULL,
               ReferenceFrame frame = ReferenceFrame::Unknown)
         throw(GeometryException);

      Position(const Xvt& xvt)
         throw();

      ~Position()
         throw()
         {}

      Position& operator-=(const Position& right)
         throw();

      Position& operator+=(const Position& right)
         throw();

      friend Position operator-(const Position& left,
                                      const Position& right)
         throw();

      friend Position operator+(const Position& left,
                                      const Position& right)
         throw();

      // friend Position operator*(const double& scale,
      //                           const Position& right)
      //    {
      //       Position tmp(right);
      //       tmp.theArray *= scale;
      //       return tmp;
      //    }

      // friend Position operator*(const Position& left,
      //                           const double& scale)
      //    {
      //       return operator*(scale, left);
      //    }

      // friend Position operator*(const int& scale,
      //                           const Position& right)
      //    {
      //       return operator*(double(scale), right);
      //    }

      // friend Position operator*(const Position& left,
      //                           const int& scale)
      //    {
      //       return operator*(double(scale), left);
      //    }

      bool operator==(const Position &right) const
         throw();

      bool operator!=(const Position &right) const
         throw();

      Position transformTo(CoordinateSystem sys)
         throw();

      Position asGeodetic()
         throw()
      { transformTo(Geodetic); return *this; }

      Position asGeodetic(EllipsoidModel *ell)
         throw(GeometryException)
      {
         try { setEllipsoidModel(ell); }
         catch(GeometryException& ge) { GPSTK_RETHROW(ge); }
         transformTo(Geodetic);
         return *this;
      }
      Position asECEF()
         throw()
      { transformTo(Cartesian); return *this; }

      const ReferenceFrame& getReferenceFrame() const
         throw();

      double X() const
         throw();

      double Y() const
         throw();

      double Z() const
         throw();

      double geodeticLatitude() const
         throw();

      double geocentricLatitude() const
         throw();

      double theta() const
         throw();

      double phi() const
         throw();
      double longitude() const
         throw();

      double radius() const
         throw();

      double height() const
         throw();

      CoordinateSystem getCoordinateSystem() const
         throw()
      { return system; };

      double getGeodeticLatitude() const
         throw()
      { return geodeticLatitude(); }

      double getGeocentricLatitude() const
         throw()
      { return geocentricLatitude(); }

      double getLongitude() const
         throw()
      { return longitude(); }

      double getAltitude() const
         throw()
      { return height(); }

      double getHeight() const
         throw()
      { return height(); }

      double getX() const
         throw()
      { return X(); }

      double getY() const
         throw()
      { return Y(); }

      double getZ() const
         throw()
      { return Z(); }

      double getTheta() const
         throw()
      { return theta(); }

      double getPhi() const
         throw()
      { return phi(); }

      double getRadius() const
         throw()
      { return radius(); }


      void setReferenceFrame(const ReferenceFrame& frame)
         throw();

      void setEllipsoidModel(const EllipsoidModel *ell)
         throw(GeometryException);

      Position& setGeodetic(const double lat,
                            const double lon,
                            const double ht,
                            const EllipsoidModel *ell = NULL)
         throw(GeometryException);

      Position& setGeocentric(const double lat,
                              const double lon,
                              const double rad)
         throw(GeometryException);


      Position& setSpherical(const double theta,
                             const double phi,
                             const double rad)
         throw(GeometryException);

      Position& setECEF(const double X,
                        const double Y,
                        const double Z)
         throw();

      Position& setECEF(const double XYZ[3])
         throw()
      { return setECEF(XYZ[0],XYZ[1],XYZ[2]); }

      Position& setECEF(const Triple& XYZ)
         throw()
      { return setECEF(XYZ[0],XYZ[1],XYZ[2]); }


      Position& setToString(const std::string& str,
                            const std::string& fmt)
         throw(GeometryException,
               StringUtils::StringException);

      std::string printf(const char *fmt) const
         throw(StringUtils::StringException);

      std::string printf(const std::string& fmt) const
         throw(StringUtils::StringException)
      { return printf(fmt.c_str()); }

      std::string asString() const
         throw(StringUtils::StringException);

      // static void convertSphericalToCartesian(const Triple& tpr,
      //                                         Triple& xyz)
      //    throw();

      // static void convertCartesianToSpherical(const Triple& xyz,
      //                                         Triple& tpr)
      //    throw();


      // static void convertCartesianToGeodetic(const Triple& xyz,
      //                                        Triple& llh,
      //                                        const double A,
      //                                        const double eccSq)
      //    throw();

      // static void convertGeodeticToCartesian(const Triple& llh,
      //                                        Triple& xyz,
      //                                        const double A,
      //                                        const double eccSq)
      //    throw();

      // static void convertCartesianToGeocentric(const Triple& xyz,
      //                                          Triple& llr)
      //    throw();

      // static void convertGeocentricToCartesian(const Triple& llr,
      //                                          Triple& xyz)
      //    throw();

      // static void convertGeocentricToGeodetic(const Triple& llr,
      //                                         Triple& geodeticllh,
      //                                         const double A,
      //                                         const double eccSq)
      //    throw();


      // static void convertGeodeticToGeocentric(const Triple& geodeticllh,
      //                                         Triple& llr,
      //                                         const double A,
      //                                         const double eccSq)
      //    throw();


      friend std::ostream& operator<<(std::ostream& s,
                                      const Position& p);

      friend double range(const Position& A,
                          const Position& B)
         throw(GeometryException);


      static double radiusEarth(const double geolat,
                                const double A,
                                const double eccSq)
         throw();

      double radiusEarth() const
         throw()
      {
         Position p(*this);
         p.transformTo(Position::Geodetic);
         return Position::radiusEarth(p.theArray[0], p.AEarth, p.eccSquared);
      }

      double elevation(const Position& Target) const
         throw(GeometryException);

      double elevationGeodetic(const Position& Target) const
         throw(GeometryException);

      double azimuth(const Position& Target) const
         throw(GeometryException);

      double azimuthGeodetic(const Position& Target) const
         throw(GeometryException);

      Position getIonosphericPiercePoint(const double elev,
                                         const double azim,
                                         const double ionoht) const
         throw();

      double getCurvMeridian() const
         throw();

      double getCurvPrimeVertical() const
         throw();


   };
}
#endif   // GPSTK_POSITION_HPP







%extend gpstk::Position {
   static gpstk::Triple convertSphericalToCartesian(const gpstk::Triple& tpr) throw() {
      gpstk::Triple xyz;
      gpstk::Position::convertSphericalToCartesian(tpr, xyz);
      return xyz;
   }
   static gpstk::Triple convertCartesianToSpherical(const gpstk::Triple& xyz) throw() {
      gpstk::Triple tpr;
      gpstk::Position::convertCartesianToSpherical(xyz, tpr);
      return tpr;
   }
   static gpstk::Triple convertCartesianToGeodetic( const Triple& xyz,
                                                    const double A,
                                                    const double eccSq) throw() {
      gpstk::Triple llh;
      gpstk::Position::convertCartesianToGeodetic(xyz, llh, A, eccSq);
      return llh;
   }
   static gpstk::Triple convertGeodeticToCartesian( const Triple& llh,
                                                    const double A,
                                                    const double eccSq) throw() {
      gpstk::Triple xyz;
      gpstk::Position::convertGeodeticToCartesian(llh, xyz, A, eccSq);
      return xyz;
   }
   static gpstk::Triple convertCartesianToGeocentric(const Triple& xyz) throw() {
      gpstk::Triple llr;
      gpstk::Position::convertCartesianToGeocentric(xyz, llr);
      return llr;
   }
   static gpstk::Triple convertGeocentricToCartesian(const gpstk::Triple& llr) throw() {
      gpstk::Triple xyz;
      gpstk::Position::convertGeocentricToCartesian(llr, xyz);
      return xyz;
   }
   static gpstk::Triple convertGeocentricToGeodetic( const Triple& llr,
                                                     const double A,
                                                     const double eccSq) throw() {
      gpstk::Triple geodeticllh;
      gpstk::Position::convertGeocentricToGeodetic(llr, geodeticllh, A, eccSq);
      return geodeticllh;
   }
   static gpstk::Triple convertGeodeticToGeocentric( const Triple& geodeticllh,
                                                     const double A,
                                                     const double eccSq) throw() {
      gpstk::Triple llr;
      gpstk::Position::convertGeodeticToGeocentric(geodeticllh, llr, A, eccSq);
      return llr;
   }
};

%inline {
   double rangeBetween(const gpstk::Position& A, const gpstk::Position& B) throw(gpstk::GeometryException) {
      return gpstk::range(A, B);
   }
}


// TODO: figure out how to get these to show up!


%feature("docstring")  rangeBetween "throw()

Computes the range (in meters) between two Positions.

Parameters:
-----------

A:  a Position

B:  another Position


Exceptions:
-----------
GeometryException:  if ellipsoid values differ or if transformTo(Cartesian) fails"



%feature("docstring")  gpstk::Position::convertGeodeticToGeocentric "throw ()

Fundamental routine to convert geodetic to geocentric.

Parameters:
-----------

geodeticllh: Triple consisting of (geodetic latitude (deg N),
                                   longitude (deg E),
                                   height above ellipsoid (meters))

A:  Earth semi-major axis

eccSq:  square of Earth eccentricity";


%feature("docstring")  gpstk::Position::convertSphericalToCartesian "throw ()
Fundamental conversion from spherical to cartesian coordinates.

This is the workhorse for the addition/subtraction operators.

Parameters:
-----------

trp: triple consisting of (theta, phi (degrees), radius)";


%feature("docstring")  gpstk::Position::convertCartesianToSpherical "throw ()
Fundamental routine to convert cartesian to spherical coordinates.
Note that The zero vector is converted to (90,0,0).

Parameters:
-----------

xyz:  Triple consisting of (X,Y,Z)";


%feature("docstring")  gpstk::Position::convertCartesianToGeodetic "throw ()
Fundamental routine to convert ECEF (cartesian) to geodetic coordinates.

Note thatThe zero vector is converted to (90,0,-R(earth)).

Parameters:
-----------

xyz:  Triple of (X,Y,Z) in meters

A:  Earth semi-major axis

eccSq: square of Earth eccentricity";


%feature("docstring")  gpstk::Position::convertGeodeticToCartesian "throw ()
Fundamental routine to convert geodetic to ECEF (cartesian) coordinates.

Parameters:
-----------

llh:  geodetic lat(deg N), lon(deg E), height above ellipsoid (meters)

A:  Earth semi-major axis

eccSq: square of Earth eccentricity";


%feature("docstring")  gpstk::Position::convertCartesianToGeocentric "throw ()
Fundamental routine to convert cartesian (ECEF) to geocentric.

Parameters:
-----------

xyz:  triple consisting of (X,Y,Z)";


%feature("docstring")  gpstk::Position::convertGeocentricToCartesian "throw ()
Fundamental routine to convert geocentric to cartesian (ECEF).

Parameters:
-----------

llr:  Triple consisting of (geocentric lat(deg N), lon(deg E), radius)";


%feature("docstring")  gpstk::Position::convertGeocentricToGeodetic "throw ()
Fundamental routine to convert geocentric to geodetic.

Parameters:
-----------

llr: Triple consisting of (geocentric lat(deg N), lon(deg E), radius(meters))

A:  Earth semi-major axis

eccSq:  square of Earth eccentricity";


%feature("docstring")  gpstk::Position::convertGeodeticToGeocentric "throw ()

Fundamental routine to convert geodetic to geocentric.

Parameters:
-----------

geodeticllh: Triple consisting of (geodetic latitude (deg N),
                                   longitude (deg E),
                                   height above ellipsoid (meters))

A:  Earth semi-major axis

eccSq:  square of Earth eccentricity";
