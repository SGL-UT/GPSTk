%ignore gpstk::Position::operator*;
%ignore gpstk::Position::convertSphericalToCartesian(const Triple& tpr, Triple& xyz) throw();
%ignore gpstk::Position::convertCartesianToSpherical(const Triple& xyz, Triple& tpr) throw();
%ignore gpstk::Position::convertCartesianToGeodetic(const Triple& xyz, Triple& llh, const double A, const double eccSq) throw();
%ignore gpstk::Position::convertGeodeticToCartesian(const Triple&, llh, Triple& xyz, const double A, const double eccSq) throw();
%ignore gpstk::Position::convertCartesianToGeocentric(const Triple& xyz, Triple& llr) throw();
%ignore gpstk::Position::convertGeocentricToCartesian(const Triple& llr, Triple& xyz) throw();
%ignore gpstk::Position::convertGeocentricToGeodetic(const Triple& llr, Triple& geodeticllr, const double A, const double eccSq) throw();
%ignore gpstk::Position::convertGeodeticToGeocentric(const Triple& geodeticllh, Triple& llr, const double A, const double eccSq) throw();


%include "../../../src/Position.hpp"

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
