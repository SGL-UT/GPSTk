import sys
import doc
import os
from distutils.core import setup, Extension


core_lib =  ['gpstk.i',
            '../../../src/AlmOrbit.cpp',
            '../../../src/ANSITime.cpp',
            '../../../src/BinUtils.cpp',
            '../../../src/BrcClockCorrection.cpp',
            '../../../src/BrcKeplerOrbit.cpp',
            '../../../src/CivilTime.cpp',
            '../../../src/ClockSatStore.cpp',
            '../../../src/CommonTime.cpp',
            '../../../src/EngAlmanac.cpp',
            '../../../src/EngEphemeris.cpp',
            '../../../src/EngNav.cpp',
            '../../../src/Exception.cpp',
            '../../../src/FFData.cpp',
            '../../../src/FFStream.cpp',
            '../../../src/GalEphemeris.cpp',
            '../../../src/GalEphemerisStore.cpp',
            '../../../src/GPSAlmanacStore.cpp',
            '../../../src/GPSEphemerisStore.cpp',
            '../../../src/GPSWeek.cpp',
            '../../../src/GPSWeekSecond.cpp',
            '../../../src/GPSWeekZcount.cpp',
            '../../../src/GPSZcount.cpp',
            '../../../src/JulianDate.cpp',
            '../../../src/MJD.cpp',
            '../../../src/ObsID.cpp',
            '../../../src/ObsIDInitializer.cpp',
            '../../../src/Position.cpp',
            '../../../src/PositionSatStore.cpp',
            '../../../src/ReferenceFrame.cpp',
            '../../../src/Rinex3ClockData.cpp',
            '../../../src/Rinex3ClockHeader.cpp',
            '../../../src/Rinex3ObsHeader.cpp',
            '../../../src/RinexClockData.cpp',
            '../../../src/RinexClockHeader.cpp',
            '../../../src/RinexObsHeader.cpp',
            '../../../src/RinexObsID.cpp',
            '../../../src/RinexSatID.cpp',
            '../../../src/SEMAlmanacStore.cpp',
            '../../../src/SEMData.cpp',
            '../../../src/SEMHeader.cpp',
            '../../../src/SP3Data.cpp',
            '../../../src/SP3EphemerisStore.cpp',
            '../../../src/SP3Header.cpp',
            '../../../src/SP3SatID.cpp',
            '../../../src/SVNumXRef.cpp',
            '../../../src/SystemTime.cpp',
            '../../../src/TimeConverters.cpp',
            '../../../src/TimeString.cpp',
            '../../../src/TimeSystem.cpp',
            '../../../src/TimeTag.cpp',
            '../../../src/Triple.cpp',
            '../../../src/UnixTime.cpp',
            '../../../src/Xvt.cpp',
            '../../../src/YDSTime.cpp',
            '../../../src/YumaAlmanacStore.cpp',
            '../../../src/YumaData.cpp',
      ]


cpp_flags = ['-std=c++11', '-w']
swig_flags = ['-c++', '-I../include', '-w362,383,384,503']


if not os.path.exists('doc/doc.i'):
      os.makedirs('doc')
      file = open('doc/doc.i', 'w+')
# doc.generate_docs()


setup(name='GPSTk',
      version='2.1',
      description='The GPS Toolkit',
      author='Applied Research Laboratories at the University of Texas at Austin',
      author_email='gpstk@arlut.utexas.edu',
      url='http://www.gpstk.org/',
      ext_modules=[Extension(name='_gpstk',
                             sources=core_lib,
                             include_dirs=['../../../src/'],
                             extra_compile_args=cpp_flags,
                             swig_opts=swig_flags,
                             language='c++')],
      py_modules=['gpstk', 'timeconvert'])
