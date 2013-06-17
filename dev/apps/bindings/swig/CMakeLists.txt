# this file is not added to parent cmakelists.txt files yet
cmake_minimum_required(VERSION 2.8)

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# warning disables:
# 362: operator= ignored
# 383: operator++ ignored
# 384: operator-- ignored
# 503: can't wrap 'identifer' unless renamed to a valid identifier
set(CMAKE_SWIG_FLAGS "-w362,383,384,503")
set(CMAKE_CXX_FLAGS "-std=c++11")

# time module
set_source_files_properties(gpstk_time.i PROPERTIES CPLUSPLUS ON) 
swig_add_module(gpstk_time python gpstk_time.i 
					../../../src/ANSITime.cpp
					../../../src/CivilTime.cpp
					../../../src/CommonTime.cpp
					../../../src/Exception.cpp
					../../../src/GPSWeek.cpp
					../../../src/GPSWeekSecond.cpp
					../../../src/GPSWeekZcount.cpp
					../../../src/GPSZcount.cpp
					../../../src/JulianDate.cpp
					../../../src/MJD.cpp
					../../../src/SystemTime.cpp
					../../../src/TimeConstants.hpp
					../../../src/TimeConverters.cpp
					../../../src/TimeString.cpp
					../../../src/TimeSystem.cpp
					../../../src/TimeTag.cpp
					../../../src/TimeTag.hpp
					../../../src/UnixTime.cpp
					../../../src/YDSTime.cpp)
swig_link_libraries(gpstk_time ${PYTHON_LIBRARIES})



# util module
set_source_files_properties(gpstk_util.i PROPERTIES CPLUSPLUS ON)
swig_add_module(gpstk_util python gpstk_util.i 
					../../../src/EllipsoidModel.hpp
					../../../src/geometry.hpp
					../../../src/GNSSconstants.hpp
					../../../src/gps_constants.hpp
					../../../src/MathBase.hpp
					../../../src/ObsID.cpp
					../../../src/ObsID.hpp
					../../../src/ObsIDInitializer.cpp
					../../../src/ReferenceFrame.cpp
					../../../src/SatID.hpp
					../../../src/SpecialFunctions.cpp
					../../../src/Triple.cpp
					../../../src/Xv.hpp
					../../../src/Xvt.cpp
	                ../../../src/Exception.cpp
	                ../../../src/Position.cpp
	                ../../../src/StringUtils.hpp
					)
swig_link_libraries(gpstk_util ${PYTHON_LIBRARIES})
