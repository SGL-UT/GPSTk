///////////////////////////////////////////////
//              Python stuff
///////////////////////////////////////////////
%pythoncode %{

def nlines(fileName):
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
//           C++ extension stuff
///////////////////////////////////////////////
%extend gpstk::ReferenceFrame {
	std::string gpstk::ReferenceFrame::__str__() {
        int f = static_cast<int>($self->getFrame());
        if(f == 1) return "WGS84";
        if(f == 2) return "PZ90";
        else return "Unknown";
	}
}



///////////////////////////////////////////////
//            macro string stuff
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
STR_DUMP_HELPER(Rinex3EphemerisStore)
STR_DUMP_HELPER(Rinex3ObsHeader)
STR_DUMP_HELPER(Rinex3ObsData)
STR_DUMP_HELPER(Rinex3NavData)
STR_DUMP_HELPER(Rinex3NavHeader)
STR_DUMP_HELPER(SEMData)
STR_DUMP_HELPER(SEMHeader)



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
