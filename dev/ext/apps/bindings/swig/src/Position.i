// This file:
//  - ignores operators that can't be wrapped (eliminates a warning)
//  - ignores functions with void return types and take a non-const parameter for output
//  - adds wrappers on the functions mentioned above so that they are purely functional

%ignore gpstk::Position::convertSphericalToCartesian(const Triple& tpr, Triple& xyz) throw();
%ignore gpstk::Position::convertCartesianToSpherical(const Triple& xyz, Triple& tpr) throw();
%ignore gpstk::Position::convertCartesianToGeodetic(const Triple& xyz, Triple& llh, const double A, const double eccSq) throw();
%ignore gpstk::Position::convertGeodeticToCartesian(const Triple&, llh, Triple& xyz, const double A, const double eccSq) throw();
%ignore gpstk::Position::convertCartesianToGeocentric(const Triple& xyz, Triple& llr) throw();
%ignore gpstk::Position::convertGeocentricToCartesian(const Triple& llr, Triple& xyz) throw();
%ignore gpstk::Position::convertGeocentricToGeodetic(const Triple& llr, Triple& geodeticllr, const double A, const double eccSq) throw();
%ignore gpstk::Position::convertGeodeticToGeocentric(const Triple& geodeticllh, Triple& llr, const double A, const double eccSq) throw();

%include "../../../lib/GNSSCore/Position.hpp"

%extend gpstk::Position {
   static gpstk::Triple convertSphericalToCartesian(const gpstk::Triple& tpr) {
      gpstk::Triple xyz;
      gpstk::Position::convertSphericalToCartesian(tpr, xyz);
      return xyz;
   }
   static gpstk::Triple convertCartesianToSpherical(const gpstk::Triple& xyz) {
      gpstk::Triple tpr;
      gpstk::Position::convertCartesianToSpherical(xyz, tpr);
      return tpr;
   }
   static gpstk::Triple convertCartesianToGeodetic( const Triple& xyz,
                                                    const double A,
                                                    const double eccSq) {
      gpstk::Triple llh;
      gpstk::Position::convertCartesianToGeodetic(xyz, llh, A, eccSq);
      return llh;
   }
   static gpstk::Triple convertGeodeticToCartesian( const Triple& llh,
                                                    const double A,
                                                    const double eccSq) {
      gpstk::Triple xyz;
      gpstk::Position::convertGeodeticToCartesian(llh, xyz, A, eccSq);
      return xyz;
   }
   static gpstk::Triple convertCartesianToGeocentric(const Triple& xyz) {
      gpstk::Triple llr;
      gpstk::Position::convertCartesianToGeocentric(xyz, llr);
      return llr;
   }
   static gpstk::Triple convertGeocentricToCartesian(const gpstk::Triple& llr) {
      gpstk::Triple xyz;
      gpstk::Position::convertGeocentricToCartesian(llr, xyz);
      return xyz;
   }
   static gpstk::Triple convertGeocentricToGeodetic( const Triple& llr,
                                                     const double A,
                                                     const double eccSq) {
      gpstk::Triple geodeticllh;
      gpstk::Position::convertGeocentricToGeodetic(llr, geodeticllh, A, eccSq);
      return geodeticllh;
   }
   static gpstk::Triple convertGeodeticToGeocentric( const Triple& geodeticllh,
                                                     const double A,
                                                     const double eccSq) {
      gpstk::Triple llr;
      gpstk::Position::convertGeodeticToGeocentric(geodeticllh, llr, A, eccSq);
      return llr;
   }
};
