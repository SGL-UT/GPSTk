# this file is not added to parent cmakelists.txt files yet

cmake_minimum_required(VERSION 2.8)

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

FIND_PACKAGE(PythonLibs)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(CMAKE_SWIG_FLAGS "")

SET_SOURCE_FILES_PROPERTIES(gpstk_time.i PROPERTIES CPLUSPLUS ON) 


SWIG_ADD_MODULE(gpstk_time python gpstk_time.i 
					../../../src/ANSITime.cpp
					../../../src/CivilTime.cpp
					../../../src/CommonTime.cpp
					../../../src/Exception.cpp
					../../../src/GPSZcount.cpp
					../../../src/GPSWeek.cpp
					../../../src/GPSWeekSecond.cpp
					../../../src/GPSWeekZcount.cpp
					../../../src/JulianDate.cpp
					../../../src/MJD.cpp
					../../../src/SystemTime.cpp
					../../../src/TimeConstants.hpp
					../../../src/TimeConverters.cpp
					../../../src/TimeString.cpp
					../../../src/TimeSystem.cpp
					../../../src/TimeTag.hpp
					../../../src/TimeTag.cpp
					../../../src/UnixTime.cpp
					../../../src/YDSTime.cpp)


SWIG_LINK_LIBRARIES(gpstk_time ${PYTHON_LIBRARIES})
