///////////////////////////////////////////////
//              Python stuff
///////////////////////////////////////////////
%pythoncode %{

def _nlines(fileName):
    """Counts and returns the number of lines in a file, excluding any blank
    lines at the end of the file.
    """
    count = 0
    with open(fileName) as infp:
        for line in infp:
            count += 1
    return count


def now(timeSystem=TimeSystem.Unknown):
    """Returns the current time (defined by what SystemTime() returns)
    in a CommonTime format, in the given TimeSystem.

    Parameters:
            -----------

        timeSystem:  the TimeSystem (enum value) to assign to the output
    """
    t = SystemTime().convertToCommonTime()
    t.setTimeSystem(TimeSystem(timeSystem))
    return t



def commonTime(timeTag):
    """Converts a time to a CommonTime using its convertToCommonTime method."""
    return timeTag.convertToCommonTime()
%}



///////////////////////////////////////////////
//      Macros for string (__str__) stuff
///////////////////////////////////////////////
// Uses the dump method in the class to get string output
%define STR_DUMP_HELPER(name)
%extend gpstk:: ## name {
     std::string __str__() {
        std::ostringstream stream;
        $self->dump(stream);
        return stream.str();
    }
}
%enddef
STR_DUMP_HELPER(AlmOrbit)
STR_DUMP_HELPER(BrcClockCorrection)
STR_DUMP_HELPER(BrcKeplerOrbit)
STR_DUMP_HELPER(EngAlmanac)
STR_DUMP_HELPER(EngEphemeris)
STR_DUMP_HELPER(GalEphemeris)
STR_DUMP_HELPER(GalEphemerisStore)
STR_DUMP_HELPER(GloEphemeris)
STR_DUMP_HELPER(GPSEphemerisStore)
STR_DUMP_HELPER(OrbElem)
STR_DUMP_HELPER(OrbElemStore)
STR_DUMP_HELPER(Rinex3ClockData)
STR_DUMP_HELPER(Rinex3ClockHeader)
STR_DUMP_HELPER(Rinex3EphemerisStore)
STR_DUMP_HELPER(Rinex3NavData)
STR_DUMP_HELPER(Rinex3NavHeader)
STR_DUMP_HELPER(Rinex3ObsData)
STR_DUMP_HELPER(Rinex3ObsHeader)
STR_DUMP_HELPER(RinexMetData)
STR_DUMP_HELPER(RinexMetHeader)
STR_DUMP_HELPER(RinexObsData)
STR_DUMP_HELPER(RinexObsHeader)
STR_DUMP_HELPER(RinexSatID)
STR_DUMP_HELPER(SEMData)
STR_DUMP_HELPER(SEMHeader)
STR_DUMP_HELPER(SP3Data)
STR_DUMP_HELPER(SP3Header)
STR_DUMP_HELPER(SP3SatID)
STR_DUMP_HELPER(YumaData)
STR_DUMP_HELPER(YumaHeader)



// Uses the operator<< in the class to get string output
%define STR_STREAM_HELPER(name)
%extend gpstk:: ##name {
    std::string __str__() {
        std::ostringstream stream;
        stream << *($self);
        return stream.str();
    }
}
%enddef
STR_STREAM_HELPER(ReferenceFrame)
STR_STREAM_HELPER(Xv)
STR_STREAM_HELPER(Xvt)




// Uses gpstk::StringUtils::asString(x) to get string output
%define AS_STRING_HELPER(name)
%extend gpstk:: ##name {
    std::string __str__() {
        return gpstk::StringUtils::asString(*($self));
    }
}
%enddef
AS_STRING_HELPER(ObsID)
AS_STRING_HELPER(SatID)
