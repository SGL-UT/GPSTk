%pythoncode %{

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