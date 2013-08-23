from distutils.core import setup, Extension


script_names = ['timeconvert.py', 'position_difference.py']


core_lib =  ['apps/bindings_installer/gpstk/gpstk_pylibPYTHON_wrap.cxx',
            'src/AlmOrbit.cpp',
            'src/ANSITime.cpp',
            'src/Antenna.cpp',
            'src/AstronomicalFunctions.cpp',
            'src/Bancroft.cpp',
            'src/BDSEphemeris.cpp'
            'src/BDSEphemerisStore.cpp'
            'src/BinUtils.cpp',
            'src/BrcClockCorrection.cpp',
            'src/BrcKeplerOrbit.cpp',
            'src/CivilTime.cpp',
            'src/ClockSatStore.cpp',
            'src/CommonTime.cpp',
            'src/EngAlmanac.cpp',
            'src/EngEphemeris.cpp'
            'src/EngNav.cpp',
            'src/Exception.cpp',
            'src/Expression.cpp',
            'src/ExtractData.cpp',
            'src/FFData.cpp',
            'src/FFStream.cpp',
            'src/FICData.cpp',
            'src/FICHeader.cpp',
            'src/GalEphemeris.cpp'
            'src/GalEphemerisStore.cpp'
            'src/GloEphemeris.cpp',
            'src/GloEphemerisStore.cpp'
            'src/GPSAlmanacStore.cpp',
            'src/GPSEphemeris.cpp'
            'src/GPSEphemerisStore.cpp'
            'src/GPSWeek.cpp',
            'src/GPSWeekZcount.cpp',
            'src/GPSZcount.cpp',
            'src/JulianDate.cpp',
            'src/MJD.cpp',
            'src/MoonPosition.cpp',
            'src/MSCData.cpp',
            'src/MSCStore.cpp',
            'src/ObsEpochMap.cpp',
            'src/ObsID.cpp',
            'src/ObsIDInitializer.cpp',
            'src/OrbElem.cpp',
            'src/OrbElemRinex.cpp',
            'src/OrbitEph.cpp'
            'src/OrbitEphStore.cpp'
            'src/PoleTides.cpp',
            'src/Position.cpp',
            'src/PositionSatStore.cpp',
            'src/PRSolution2.cpp',
            'src/QZSEphemeris.cpp'
            'src/QZSEphemerisStore.cpp'
            'src/ReferenceFrame.cpp',
            'src/Rinex3ClockData.cpp',
            'src/Rinex3ClockHeader.cpp',
            'src/Rinex3EphemerisStore.cpp',
            'src/Rinex3NavData.cpp',
            'src/Rinex3NavHeader.cpp',
            'src/Rinex3ObsData.cpp',
            'src/Rinex3ObsHeader.cpp',
            'src/RinexClockData.cpp',
            'src/RinexClockHeader.cpp',
            'src/RinexEphemerisStore.cpp'
            'src/RinexMetData.cpp',
            'src/RinexMetHeader.cpp',
            'src/RinexNavData.cpp',
            'src/RinexNavHeader.cpp',
            'src/RinexObsData.cpp',
            'src/RinexObsHeader.cpp',
            'src/RinexObsID.cpp',
            'src/RinexSatID.cpp',
            'src/RinexUtilities.cpp',
            'src/SEMAlmanacStore.cpp',
            'src/SEMData.cpp',
            'src/SEMHeader.cpp',
            'src/SolidTides.cpp',
            'src/SP3Data.cpp',
            'src/SP3EphemerisStore.cpp',
            'src/SP3Header.cpp',
            'src/SP3SatID.cpp',
            'src/SunPosition.cpp',
            'src/SVNumXRef.cpp',
            'src/SystemTime.cpp',
            'src/TimeConverters.cpp',
            'src/TimeString.cpp',
            'src/TimeSystem.cpp',
            'src/TimeTag.cpp',
            'src/Triple.cpp',
            'src/TropModel.cpp',
            'src/UnixTime.cpp',
            'src/Week.cpp',
            'src/WeekSecond.cpp',
            'src/WxObsMap.cpp',
            'src/Xvt.cpp',
            'src/YDSTime.cpp',
            'src/YumaAlmanacStore.cpp',
            'src/YumaData.cpp',
            ]


desc = """
The goal of the GPSTk project is to provide an open source library
and suite of applications to the satellite navigation community--to free
researchers to focus on research, not lower level coding.

The GPSTk provides a wide array of functions that solve processing problems
associated with GPS such as processing or using standard formats such as RINEX.
The libraries are the basis for the more advanced applications distributed
as part of the GPSTk suite. The GPSTk is sponsored by Space and Geophysics
Laboratory, within the Applied Research Laboratories at the University of
Texas at Austin (ARL:UT).

GPSTk is the by-product of GPS research conducted at
ARL:UT since before the first satellite launched in 1978; it is the combined
effort of many software engineers and scientists. In 2003, the research staff
at ARL:UT decided to open source much of their basic GPS processing software
as the GPSTk.
"""


def main():
      setup(name='gpstk',
            version='2.2',
            long_description=desc,
            description='The GPS Toolkit',
            author='Applied Research Laboratories at the University of Texas at Austin',
            author_email='gpstk@arlut.utexas.edu',
            url='http://www.gpstk.org/',
            package_dir={'gpstk': 'apps/bindings_installer/gpstk'},
            ext_modules=[Extension(name='gpstk._gpstk_pylib',
                                   sources=core_lib,
                                   extra_compile_args=['-std=c++11', '-w'],
                                   language='c++')],
            packages=['gpstk', 'gpstk.cpp', 'gpstk.constants', 'gpstk.exceptions'],
            scripts=map(lambda x: 'apps/bindings/swig/scripts/' + x, script_names))

if __name__ == '__main__':
      main()
