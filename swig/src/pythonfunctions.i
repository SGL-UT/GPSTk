///////////////////////////////////////////////
//              Python stuff
///////////////////////////////////////////////
%pythoncode %{
def now(timeSystem=TimeSystem('UTC')):
    """
    Returns the current time (defined by what SystemTime() returns)
    in a CommonTime format, in the given TimeSystem.

    Parameters:
            -----------

        timeSystem:  the TimeSystem to assign to the output
    """
    t = SystemTime().toCommonTime()
    t.setTimeSystem(timeSystem)
    return t


def times(starttime, endtime, seconds=0.0, days=0):
    """
    Returns a generator expression of CommonTime objects between (or equal to)
    starttime and endtime.

    You may specify a timestep in seconds (floating/integral type)
    and/or days (integral type). Not specifying a timestep will
    return a generator that yields the starttime and endtime parameters.
    The timestep must be positive, or a gpstk.exceptions.InvalidRequest
    will be raised.
    """
    if (seconds < 0.0) or (days < 0):
        raise InvalidRequest('Negative time steps may not be used.')
        return

    if (seconds == 0.0) and (days == 0):
        # empty generator:
        yield starttime
        yield endtime
        return

    t = CommonTime(starttime)
    while t <= endtime:
        yield CommonTime(t)
        t.addSeconds(seconds)
        t.addDays(days)


def moonPosition(time):
    """
    Returns the current position (A gpstk.Triple) of the moon.
    This is a functional wrapper on the moonPosition class.
    """
    return MoonPosition().getPosition(time)


def sunPosition(time):
    """
    Returns the current position (A gpstk.Triple) of the moon.
    This is a functional wrapper on the MoonPosition class.
    """
    return SunPosition().getPosition(time)


def poleTides(time, position, x, y):
    """
    Returns the effect (a gpstk.Triple) of pole tides (meters)
    on the given position, in the Up-East-North (UEN) reference frame.
    This is a functional wrapper on the (hidden) PoleTides class.
    """
    return PoleTides().getPoleTide(time, position, x, y)

def solidTides(time, position):
    """
    Returns the effect (a gpstk.Triple) of solid Earth tides (meters)
    at the given position and epoch, in the Up-East-North (UEN) reference frame.
    This is a functional wrapper on the (hidden) SolidTides class.
    """
    return SolidTides().getSolidTide(time, position)


def cartesian(x=0.0, y=0.0, z=0.0,
              model=None, frame=ReferenceFrame('Unknown')):
    "Returns a Position in the Cartesian coordinate system."
    return Position(x, y, z, Position.Cartesian, model, frame)


def geodetic(latitude=0.0, longitude=0.0, height=0.0,
             model=None, frame=ReferenceFrame('Unknown')):
    "Returns a Position in the Geodetic coordinate system."
    return Position(latitude, longitude, height, Position.Geodetic, model, frame)


def spherical(theta=0.0, phi=0.0, radius=0.0,
              model=None, frame=ReferenceFrame('Unknown')):
    "Returns a Position in the Spherical coordinate system."
    return Position(theta, phi, radius, Position.Spherical, model, frame)


def geocentric(latitude=0.0, longitude=0.0, radius=0.0,
               model=None, frame=ReferenceFrame('Unknown')):
    "Returns a Position in the Geocentric coordinate system."
    return Position(latitude, longitude, radius, Position.Geocentric, model, frame)


def eval(expr, **kwargs):
    e = Expression(expr)
    e.setGPSConstants()
    for key in kwargs:
        e.set(key, kwargs[key])
    return e.evaluate()


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
STR_DUMP_HELPER(EngNav)
STR_DUMP_HELPER(GPSEphemeris)
STR_DUMP_HELPER(BDSEphemeris)
STR_DUMP_HELPER(QZSEphemeris)
STR_DUMP_HELPER(GalEphemeris)
STR_DUMP_HELPER(GloEphemeris)
STR_DUMP_HELPER(OrbElem)
STR_DUMP_HELPER(OrbElemStore)
STR_DUMP_HELPER(Rinex3ClockData)
STR_DUMP_HELPER(Rinex3ClockHeader)
STR_DUMP_HELPER(Rinex3NavData)
STR_DUMP_HELPER(Rinex3NavHeader)
STR_DUMP_HELPER(Rinex3ObsData)
STR_DUMP_HELPER(Rinex3ObsHeader)
STR_DUMP_HELPER(RinexMetData)
STR_DUMP_HELPER(RinexMetHeader)
STR_DUMP_HELPER(RinexNavData)
STR_DUMP_HELPER(RinexNavHeader)
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


// Uses the print method in the class to get string output
%define STR_PRINT_HELPER(name)
%extend gpstk:: ## name {
   std::string __str__() {
      std::ostringstream stream;
      $self->print(stream);
      return stream.str();
   }
}
%enddef
STR_PRINT_HELPER(Expression)



// Uses the dump method in the class to get string output
// for dump methods that have a detail parameter
%define STR_DUMP_DETAIL_HELPER(name)
%extend gpstk:: ## name {
   std::string __str__() {
      std::ostringstream stream;
      $self->dump(stream, 1);
      return stream.str();
   }
}
%enddef
STR_DUMP_DETAIL_HELPER(ClockSatStore)
STR_DUMP_DETAIL_HELPER(GalEphemerisStore)
STR_DUMP_DETAIL_HELPER(GloEphemerisStore)
STR_DUMP_DETAIL_HELPER(GPSEphemerisStore)
STR_DUMP_DETAIL_HELPER(Rinex3EphemerisStore)
STR_DUMP_DETAIL_HELPER(SP3EphemerisStore)



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
STR_STREAM_HELPER(GPSZcount)
STR_STREAM_HELPER(Position)
// STR_STREAM_HELPER(ReferenceFrame)
STR_STREAM_HELPER(Xv)
STR_STREAM_HELPER(SvObsEpoch)
STR_STREAM_HELPER(ObsEpoch)


// STR_STREAM_HELPER(Xvt)
// Q: Why is this (below) here instead of the macro for Xvt?
// A: There is an ambiguity issue for the operator<< for Xvt,
// see the end of TabularSatStore.hpp for a conflicting defintion
// of the Xvt out stream operator and Xvt.hpp+Xvt.cpp.
%extend gpstk::Xvt {
   std::string __str__() {
      std::ostringstream os;
         os << "x:" << $self->x
            << ", v:" << $self->v
            << ", clk bias:" << $self->clkbias
            << ", clk drift:" << $self->clkdrift
            << ", relcorr:" << $self->relcorr;
      return os.str();
   }
}


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
