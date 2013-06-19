# the section #'s match the numbers used in gpstk.i

%pythoncode %{

################################################################
# Section 2: Time classes
################################################################
	def makeTimeSystem(system='Unknown'):
		return TimeSystem(TimeSystems[system])
	TimeSystems = {
         'Unknown' : 0,
         'Any'     : 1,
         'GPS'     : 2,
         'GLO'     : 3,
         'GAL'     : 4,
         'COM'     : 5,
         'UTC'     : 6,
         'UT1'     : 7,
         'TAI' 	   : 8,
         'TT'      : 9
    }
	
	def str(self):
		return self.asString()
	CommonTime.__str__ = str
	UnixTime.__str__ = str
	SystemTime.__str__ = str
	ANSITime.__str__ = str
	CivilTime.__str__ = str
	GPSWeekSecond.__str__ = str
	GPSWeekZcount.__str__ = str
	JulianDate.__str__ = str
	MJD.__str__ = str
	YDSTime.__str__ = str
	Exception.__str__ = str



################################################################
# Section 3: General/Utils classes
################################################################
	ObsID.__str__ = lambda self: asString(self)

	# ObsID enum (3 of them) values:
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

	# Position class enum:
	CoordinateSystems = {
        'Unknown'    : Position.Unknown,
        'Geodetic'   : Position.Geodetic,
        'Geocentric' : Position.Geocentric,
        'Cartesian'  : Position.Cartesian,
        'Spherical'  : Position.Spherical,
	}
	Position.__str__ = lambda self: self.asString()


	# Triple class extensions:
	# tuple -> triple translation:
	def makeTriple(tuple): 
		return Triple(tuple[0], tuple[1], tuple[2])
	# triple -> tuple translation:
	def makeTuple(self):
		return (self[0], self[1], self[2])
	Triple.makeTuple = makeTuple

	# Xv string method
	Xv.__str__ = lambda self: 'x:'+ self.x.__str__() + ', v:' + self.v.__str__()




################################################################
# Section 4: "XvtStore and friends" (The wild, wild west)
################################################################
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