// SWIG interface for the gpstk_util module

%module gpstk_util
%{
	#include "../../../src/MathBase.hpp"
	#include "../../../src/Exception.hpp"
	#include "../../../src/geometry.hpp"
	#include "../../../src/gps_constants.hpp"
	#include "../../../src/SatID.hpp"
	#include "../../../src/ObsIDInitializer.hpp"
	#include "../../../src/ObsID.hpp"
	#include "../../../src/GNSSconstants.hpp"
	#include "../../../src/Triple.hpp"
	#include "../../../src/ReferenceFrame.hpp"
	#include "../../../src/EllipsoidModel.hpp"
	#include "../../../src/Xvt.hpp"
	#include "../../../src/StringUtils.hpp"
	#include "../../../src/Position.hpp"
	#include "../../../src/SpecialFunctions.hpp"
	#include "../../../src/Xv.hpp"
	using namespace gpstk;
%}
%import "gpstk_time.i"

%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%template(std_vector_double) std::vector<double>;
%template(std_vector_int) std::vector<int>;

%include "../../../src/geometry.hpp"
%include "../../../src/gps_constants.hpp"
%include "../../../src/SatID.hpp"

%include "../../../src/ObsIDInitializer.hpp"
%include "../../../src/ObsID.hpp"
%template(map_int_char) std::map<int, char>;
%template() std::pair<int, char>; 

%template(map_int_string) std::map<int, std::string>;
%template() std::pair<int, std::string>; 

%template(map_char_int) std::map<char, int>;
%template() std::pair<char, int>;

%template(map_string_int) std::map<std::string, int>;
%template() std::pair<std::string, int>; 
%pythoncode %{
	ObsID.__str__ = lambda self: asString(self)

	# ObsID enum values:
	ObservationTypes = {
        'Unknown'   : ObsID.otUnknown,
        'Any'  	    : ObsID.otAny,
        'Range'     : ObsID.otRange,
        'Phase'     : ObsID.otPhase,
        'Doppler'   : ObsID.otDoppler,
        'SNR'       : ObsID.otSNR,
        'Channel'   : ObsID.otChannel,
        'Iono' 	    : ObsID.otIono,
        'SSI' 	    : ObsID.otSSI,
        'LLI' 	    : ObsID.otLLI,
        'TrackLen'  : ObsID.otTrackLen,
        'NavMsg'    : ObsID.otNavMsg,
        'Undefined' : ObsID.otUndefined,
        'Last' 	    : ObsID.otLast
	}
	CarrierBands = {
		'Unknown'   : ObsID.cbUnknown,
		'Any' 	    : ObsID.cbAny,
		'Zero' 	    : ObsID.cbZero,
		'L1' 	    : ObsID.cbL1,
		'L2' 	    : ObsID.cbL2,
		'L5'        : ObsID.cbL5,
		'G1'        : ObsID.cbG1,
		'G2'        : ObsID.cbG2,
		'E5b'       : ObsID.cbE5b,
		'E5ab'      : ObsID.cbE5ab,
		'E1'        : ObsID.cbE1,
		'E2' 		: ObsID.cbE2,
		'E6'        : ObsID.cbE6,
		'C6'        : ObsID.cbC6,
		'L1L2'      : ObsID.cbL1L2,
		'Undefined' : ObsID.cbUndefined,
		'Last'      : ObsID.cbLast, 
	}

	TrackingCodes = {
		'Unknown' : ObsID.tcUnknown,
		'Any' 	  : ObsID.tcAny,
		'CA' 	  : ObsID.tcCA,
		'P' 	  : ObsID.tcP,
		'Y'		  : ObsID.tcY,
		'W' 	  : ObsID.tcW,
		'N' 	  : ObsID.tcN,
		'D' 	  : ObsID.tcD,
		'M' 	  : ObsID.tcM,
		'C2M' 	  : ObsID.tcC2M,
		'C2L' 	  : ObsID.tcC2L,
		'C2LM' 	  : ObsID.tcC2LM,
		'I5' 	  : ObsID.tcI5,
		'Q5' 	  : ObsID.tcQ5,
		'IQ5' 	  : ObsID.tcIQ5,

		'GCA' : ObsID.tcGCA,
		'GP'  : ObsID.tcGP,

		'A'    : ObsID.tcA,
		'B'    : ObsID.tcB,
		'C'    : ObsID.tcC,
		'BC'   : ObsID.tcBC,
		'ABC'  : ObsID.tcABC,
		'IE5'  : ObsID.tcIE5,
		'QE5'  : ObsID.tcQE5,
		'IQE5' : ObsID.tcIQE5,

		'SCA'       : ObsID.tcSCA,
		'SI5'       : ObsID.tcSI5,
		'SQ5'       : ObsID.tcSQ5,
		'SIQ5'      : ObsID.tcSIQ5,
	#	'CCA'       : ObsID.tcCCA, marked as TBD in ObsID.hpp
		'CI2'       : ObsID.tcCI2,
		'CQ2'       : ObsID.tcCQ2,
		'CIQ2'      : ObsID.tcCIQ2,
		'CI5'       : ObsID.tcCI5,
		'CQ5'       : ObsID.tcCQ5,
		'CIQ5'      : ObsID.tcCIQ5,
		'CI6'       : ObsID.tcCI6,
		'CQ6'       : ObsID.tcCQ6,
		'CIQ6' 	    : ObsID.tcCIQ6,
		'Undefined' : ObsID.tcUndefined,
		'Last '     : ObsID.tcLast
	}

	def makeObsID(observationType='Unknown', carrierBand='Unknown', trackingCode='Unknown'):
		return ObsID(ObservationTypes[observationType], CarrierBands[carrierBand], TrackingCodes[trackingCode])


%}

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
	std::string __str__() {
		return "(" + std::to_string($self->theArray[0]) + ", " 
				   + std::to_string($self->theArray[1]) + ", " 
				   + std::to_string($self->theArray[2]) + ")";
	}		
}

%include "../../../src/ReferenceFrame.hpp"
%include "../../../src/EllipsoidModel.hpp"
%include "../../../src/Xvt.hpp"
%include "../../../src/Position.hpp"
%pythoncode %{
	CoordinateSystems = {
        'Unknown'    : Position.Unknown,
        'Geodetic'   : Position.Geodetic,
        'Geocentric' : Position.Geocentric,
        'Cartesian'  : Position.Cartesian,
        'Spherical'  : Position.Spherical,
	}
%}



%include "../../../src/SpecialFunctions.hpp"
%include "../../../src/Xv.hpp"

%pythoncode %{
	def makeTriple(tuple):
		return Triple(tuple[0], tuple[1], tuple[2])
	def makeTuple(self):
		return (self[0], self[1], self[2])
	Triple.makeTuple = makeTuple

	Xv.__str__ = lambda self: 'x:'+ self.x.__str__() + ', v:' + self.v.__str__()
	SatelliteSystems = {
         'systemGPS' 	     : SatID.systemGPS,
         'systemGalileo'     : SatID.systemGalileo,
         'systemGlonass'     : SatID.systemGlonass,
         'systemGeosync'     : SatID.systemGeosync,
         'systemLEO'         : SatID.systemLEO,
         'systemTransit'     : SatID.systemTransit,
         'systemCompass'     : SatID.systemCompass,
         'systemMixed'       : SatID.systemMixed,
         'systemUserDefined' : SatID.systemUserDefined,
         'systemUnknown'     : SatID.systemUnknown
	}
	def makeSatelliteSystem(id=-1, system='systemGPS'):
		return SatelliteSystem(id, SatelliteSystems[system])

%}