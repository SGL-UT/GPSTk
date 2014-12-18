#Tests for the TimeHandling Classes

add_executable(ANSITime_T ANSITime_T.cpp)
target_link_libraries(ANSITime_T gpstk)
add_test(TimeHandling_ANSITime ANSITime_T)

add_executable(CivilTime_T CivilTime_T.cpp)
target_link_libraries(CivilTime_T gpstk)
add_test(TimeHandling_CivilTime CivilTime_T)

#add_executable(CommonTime_T CommonTime_T.cpp)
#target_link_libraries(CommonTime_T gpstk)
#add_test(TimeHandling_CommonTime CommonTime_T)

add_executable(GPSWeekSecond_T GPSWeekSecond_T.cpp)
target_link_libraries(GPSWeekSecond_T gpstk)
add_test(TimeHandling_GPSWeekSecond GPSWeekSecond_T) 

add_executable(GPSWeekZcount_T GPSWeekZcount_T.cpp)
target_link_libraries(GPSWeekZcount_T gpstk)
add_test(TimeHandling_GPSWeekZcount GPSWeekZcount_T)

add_executable(JulianDate_T JulianDate_T.cpp)
target_link_libraries(JulianDate_T gpstk)
add_test(TimeHandling_JulianDate JulianDate_T)

add_executable(MJD_T MJD_T.cpp)
target_link_libraries(MJD_T gpstk)
add_test(TimeHandling_MJD MJD_T)

add_executable(SystemTime_T SystemTime_T.cpp)
target_link_libraries(SystemTime_T gpstk)
add_test(TimeHandling_SystemTime* SystemTime_T)

#add_executable(TimeConverters_T TimeConverters_T.cpp)
#target_link_libraries(TimeConverters_T gpstk)
#add_test(TimeHandling_TimeConverters TimeConverters_T)

add_executable(TimeString_T TimeString_T.cpp)
target_link_libraries(TimeString_T gpstk)
add_test(TimeHandling_TimeString* TimeString_T)

add_executable(UnixTime_T UnixTime_T.cpp)
target_link_libraries(UnixTime_T gpstk)
add_test(TimeHandling_UnixTime UnixTime_T)

add_executable(YDSTime_T YDSTime_T.cpp)
target_link_libraries(YDSTime_T gpstk)
add_test(TimeHandling_YDSTime YDSTime_T)


