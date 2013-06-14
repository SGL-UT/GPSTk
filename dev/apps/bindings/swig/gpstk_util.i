// SWIG interface for the gpstk_util module

%module gpstk_util
%{
	#include "../../../src/MathBase.hpp"
	#include "../../../src/Exception.hpp"
	#include "../../../src/geometry.hpp"
	#include "../../../src/gps_constants.hpp"
	#include "../../../src/SatID.hpp"
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
%template(std_vector_double) std::vector<double>;
%template(std_vector_int) std::vector<int>;

%include "../../../src/geometry.hpp"
%include "../../../src/gps_constants.hpp"
%include "../../../src/SatID.hpp"
%include "../../../src/ObsID.hpp"
%pythoncode %{
	# ObsID enum values:
	ObservationTypes = {
        'otUnknown'   : ObsID.otUnknown,
        'otAny'  	  : ObsID.otAny,
        'otRange' 	  : ObsID.otRange,
        'otPhase' 	  : ObsID.otPhase,
        'otDoppler'   : ObsID.otDoppler,
        'otSNR'       : ObsID.otSNR,
        'otChannel'	  : ObsID.otChannel,
        'otIono' 	  : ObsID.otIono,
        'otSSI' 	  : ObsID.otSSI,
        'otLLI' 	  : ObsID.otLLI,
        'otTrackLen'  : ObsID.otTrackLen,
        'otNavMsg' 	  : ObsID.otNavMsg,
        'otUndefined' : ObsID.otUndefined,
        'otLast' 	  : ObsID.otLast,
	}
	CarrierBands = {
		'cbUnknown'   : ObsID.cbUnknown
		'cbAny' 	  : ObsID.cbAny
		'cbZero' 	  : ObsID.cbZero
		'cbL1' 	  	  : ObsID.cbL1
		'cbL2' 	      : ObsID.cbL2
		'cbL5'        : ObsID.cbL5
		'cbG1'        : ObsID.cbG1
		'cbG2'        : ObsID.cbG2
		'cbE5b'       : ObsID.cbE5b
		'cbE5ab'      : ObsID.cbE5ab
		'cbE1'        : ObsID.cbE1
		'cbE2' 		  : ObsID.cbE2
		'cbE6'        : ObsID.cbE6
		'cbC6'        : ObsID.cbC6
		'cbL1L2'      : ObsID.cbL1L2
		'cbUndefined' : ObsID.cbUndefined
		'cbLast'      : ObsID.cbLast  
	}

	TrackingCodes = {
		'tcUnknown' : ObsID.tcUnknown
		'tcAny' 	: ObsID.tcAny
		'tcCA' 		: ObsID.tcCA
		'tcP' 		: ObsID.tcP
		'tcY'		: ObsID.tcY
		'tcW' 		: ObsID.tcW
		'tcN' 		: ObsID.tcN
		'tcD' 		: ObsID.tcD
		'tcM' 		: ObsID.tcM
		'tcC2M' 	: ObsID.tcC2M
		'tcC2L' 	: ObsID.tcC2L
		'tcC2LM' 	: ObsID.tcC2LM
		'tcI5' 		: ObsID.tcI5
		'tcQ5' 		: ObsID.tcQ5
		'tcIQ5' 	: ObsID.tcIQ5

		'tcGCA' : ObsID.tcGCA
		'tcGP'  : ObsID.tcGP

		'tcA'    : ObsID.tcA
		'tcB'    : ObsID.tcB
		'tcC'    : ObsID.tcC
		'tcBC' 	 : ObsID.tcBC
		'tcABC'  : ObsID.tcABC
		'tcIE5'  : ObsID.tcIE5
		'tcQE5'  : ObsID.tcQE5
		'tcIQE5' : ObsID.tcIQE5

		'tcSCA'       : ObsID.tcSCA
		'tcSI5'       : ObsID.tcSI5
		'tcSQ5'       : ObsID.tcSQ5
		'tcSIQ5'      : ObsID.tcSIQ5
		'tcCCA'       : ObsID.tcCCA     
		'tcCI2'       : ObsID.tcCI2
		'tcCQ2'       : ObsID.tcCQ2
		'tcCIQ2'      : ObsID.tcCIQ2
		'tcCI5'       : ObsID.tcCI5
		'tcCQ5'       : ObsID.tcCQ5
		'tcCIQ5'      : ObsID.tcCIQ5
		'tcCI6'       : ObsID.tcCI6
		'tcCQ6'       : ObsID.tcCQ6
		'tcCIQ6' 	  : ObsID.tcCIQ6
		'tcUndefined' : ObsID.tcUndefined
		'tcLast '     : ObsID.tcLast   
	}
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
%include "../../../src/SpecialFunctions.hpp"
%include "../../../src/Xv.hpp"

%pythoncode %{
	def makeTriple(tuple):
		return Triple(tuple[0], tuple[1], tuple[2])

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
         'systemUnknown'     : SatID.systemUnknown,
	}

%}