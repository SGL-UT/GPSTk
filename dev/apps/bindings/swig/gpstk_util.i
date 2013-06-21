%include "../../../src/geometry.hpp"
%include "../../../src/gps_constants.hpp"
%include "../../../src/SatID.hpp"
%include "../../../src/ObsIDInitializer.hpp"
%include "../../../src/ObsID.hpp"
%include "../../../src/GNSSconstants.hpp"

%include "../../../src/Triple.hpp"
%extend gpstk::Triple {
	double __getitem__(unsigned int i) {
		return $self->theArray[i];
	} 
	gpstk::Triple scale(double scalar) {
		return Triple(scalar * $self->theArray[0], 
				      scalar * $self->theArray[1], 
				      scalar * $self->theArray[2]);
	}
	// std::string __str__() {
	// 	return "(" + std::to_string($self->theArray[0]) + ", " 
	// 			   + std::to_string($self->theArray[1]) + ", " 
	// 			   + std::to_string($self->theArray[2]) + ")";
	// }
}

%include "../../../src/ReferenceFrame.hpp"
%include "../../../src/EllipsoidModel.hpp"
%include "../../../src/Xvt.hpp"
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
	static gpstk::Triple convertCartesianToGeodetic(const Triple& xyz,
                                                    const double A,
                                                    const double eccSq) throw() {
		gpstk::Triple llh;
		gpstk::Position::convertCartesianToGeodetic(xyz, llh, A, eccSq);
		return llh;
	}
	static gpstk::Triple convertGeodeticToCartesian(const Triple& llh,
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
	static gpstk::Triple convertGeocentricToGeodetic(const Triple& llr,
                                                     const double A,
                                                     const double eccSq) throw() {
        gpstk::Triple geodeticllh;
		gpstk::Position::convertGeocentricToGeodetic(llr, geodeticllh, A, eccSq);
		return geodeticllh;
	}
	static gpstk::Triple convertGeodeticToGeocentric(const Triple& geodeticllh,
                                                     const double A,
                                                     const double eccSq) throw() {
		gpstk::Triple llr;
		gpstk::Position::convertGeodeticToGeocentric(geodeticllh, llr, A, eccSq);
		return llr;
	}
}


%include "../../../src/convhelp.hpp"
%include "../../../src/Xv.hpp"


%pythoncode %{
SatID.__str__ = lambda self: asString(self)
ObsID.__str__ = lambda self: asString(self)

def form_dict(class_name, prefix, member_names):
	class_obj = eval(class_name)
	tmp = {}
	for member in member_names:
		tmp[member] = getattr(class_obj, prefix + member)
	return tmp

# ReferenceFrame enum 
ReferenceFrames = form_dict('ReferenceFrame', '', ['Unknown', 'WGS84', 'PZ90'])

def makeReferenceFrame(frame='Unknown'):
	return ReferenceFrame(ReferenceFrames[frame])


# SatID extensions/enum	
SatelliteSystems = {
	'GPS' 	      : SatID.systemGPS,
	'Galileo'     : SatID.systemGalileo,
	'Glonass'     : SatID.systemGlonass,
	'Geosync'     : SatID.systemGeosync,
	'LEO'         : SatID.systemLEO,
	'Transit'     : SatID.systemTransit,
	'Compass'     : SatID.systemCompass,
	'Mixed'       : SatID.systemMixed,
	'UserDefined' : SatID.systemUserDefined,
	'Unknown'     : SatID.systemUnknown
}
def makeSatID(id=-1, system='GPS'):
	return SatID(id, SatelliteSystems[system])



# ObsID enum (3 of them) values:
ObservationTypes = form_dict('ObsID', 'ot', 
	['Unknown', 'Any', 'Range', 'Phase', 'Doppler', 'SNR', 'Channel', 'Iono',
	                 'SSI', 'LLI', 'TrackLen', 'NavMsg', 'Undefined', 'Last'])
# this forms the dict:
# {Unknown : ObsID.otUnknown, ... etc.}

CarrierBands = form_dict('ObsID', 'cb', 
	['Unknown', 'Any' , 'Zero', 'L1', 'L2', 'L5', 'G1', 'G2', 'E5b', 'E5ab', 'E1', 
	                          'E2', 'E6', 'C6', 'L1L2', 'Undefined', 'Last']) 

TrackingCodes = form_dict('ObsID', 'tc', 
	['Unknown', 'Any', 'CA', 'P', 'Y', 'W', 'N', 'D', 'M' , 'C2M', 'C2L', 
	'C2LM', 'I5', 'Q5', 'IQ5', 'GCA', 'GP', 'A', 'B', 'C', 'BC', 'ABC', 'IE5', 
	'QE5', 'IQE5', 'SCA', 'SI5', 'SQ5', 'SIQ5', 'CI2', 'CQ2', 'CIQ2', 'CI5', 
	'CQ5', 'CIQ5', 'CI6', 'CQ6', 'CIQ6' , 'Undefined', 'Last'])

def makeObsID(observationType='Unknown', carrierBand='Unknown', trackingCode='Unknown'):
	return ObsID(ObservationTypes[observationType], 
				 CarrierBands[carrierBand], 
			     TrackingCodes[trackingCode])

# Position class enum:
CoordinateSystems = form_dict('Position', '', 
	['Unknown', 'Geodetic', 'Geocentric', 'Cartesian', 'Spherical'])

# Triple class extensions:
# tuple -> triple translation:
def makeTriple(tuple): 
	return Triple(tuple[0], tuple[1], tuple[2])
# triple -> tuple translation:
def makeTuple(self):
	return (self[0], self[1], self[2])
	
Triple.makeTuple = makeTuple
Triple.__str__ = lambda self: makeTuple(self).__str__()

# Xv string method
Xv.__str__ = lambda self: 'x:'+ self.x.__str__() + ', v:' + self.v.__str__()
%}
