#Tests for the TimeHandling Classes

add_executable(ANSITime_T ANSITime_T.cpp)
target_link_libraries(ANSITime_T gpstk)
add_test(TimeHandling_ANSITime ANSITime_T)
set_property(TEST TimeHandling_ANSITime PROPERTY LABELS TimeHandling TimeTag TimeStorage)

add_executable(CivilTime_T CivilTime_T.cpp)
target_link_libraries(CivilTime_T gpstk)
add_test(TimeHandling_CivilTime CivilTime_T)
set_property(TEST TimeHandling_CivilTime PROPERTY LABELS TimeHandling TimeTag TimeStorage)

add_executable(CommonTime_T CommonTime_T.cpp)
target_link_libraries(CommonTime_T gpstk)
add_test(TimeHandling_CommonTime CommonTime_T)
set_property(TEST TimeHandling_CommonTime PROPERTY LABELS TimeHandling TimeStorage)

add_executable(GPSWeekSecond_T GPSWeekSecond_T.cpp)
target_link_libraries(GPSWeekSecond_T gpstk)
add_test(TimeHandling_GPSWeekSecond GPSWeekSecond_T) 
set_property(TEST TimeHandling_ANSITime PROPERTY LABELS TimeHandling TimeTag TimeStorage)

add_executable(GPSWeekZcount_T GPSWeekZcount_T.cpp)
target_link_libraries(GPSWeekZcount_T gpstk)
add_test(TimeHandling_GPSWeekZcount GPSWeekZcount_T)
set_property(TEST TimeHandling_GPSWeekZcount PROPERTY LABELS TimeHandling TimeTag TimeStorage)

add_executable(JulianDate_T JulianDate_T.cpp)
target_link_libraries(JulianDate_T gpstk)
add_test(TimeHandling_JulianDate JulianDate_T)
set_property(TEST TimeHandling_JulianDate PROPERTY LABELS TimeHandling TimeTag TimeStorage)

add_executable(MJD_T MJD_T.cpp)
target_link_libraries(MJD_T gpstk)
add_test(TimeHandling_MJD MJD_T)
set_property(TEST TimeHandling_MJD PROPERTY LABELS TimeHandling TimeTag TimeStorage)

add_executable(SystemTime_T SystemTime_T.cpp)
target_link_libraries(SystemTime_T gpstk)
add_test(TimeHandling_SystemTime SystemTime_T)
set_property(TEST TimeHandling_SystemTime PROPERTY LABELS TimeHandling TimeTag TimeStorage)

add_executable(TimeConverters_T TimeConverters_T.cpp)
target_link_libraries(TimeConverters_T gpstk)
add_test(TimeHandling_TimeConverters TimeConverters_T)
set_property(TEST TimeHandling_TimeConverters PROPERTY LABELS TimeHandling)

add_executable(TimeString_T TimeString_T.cpp)
target_link_libraries(TimeString_T gpstk)
add_test(TimeHandling_TimeString TimeString_T)
set_property(TEST TimeHandling_TimeString PROPERTY LABELS TimeHandling)

add_executable(TimeTag_T TimeTag_T.cpp)
target_link_libraries(TimeTag_T gpstk)
add_test(TimeHandling_TimeTag TimeTag_T)
set_property(TEST TimeHandling_TimeTag PROPERTY LABELS TimeHandling TimeTag TimeStorage)

add_executable(UnixTime_T UnixTime_T.cpp)
target_link_libraries(UnixTime_T gpstk)
add_test(TimeHandling_UnixTime UnixTime_T)
set_property(TEST TimeHandling_UnixTime PROPERTY LABELS TimeHandling TimeTag TimeStorage)

add_executable(YDSTime_T YDSTime_T.cpp)
target_link_libraries(YDSTime_T gpstk)
add_test(TimeHandling_YDSTime YDSTime_T)
set_property(TEST TimeHandling_YDSTime PROPERTY LABELS TimeHandling TimeTag TimeStorage)

add_executable(TimeRange_T TimeRange_T.cpp)
target_link_libraries(TimeRange_T gpstk)
add_test(TimeHandling_TimeRange TimeRange_T)
set_property(TEST TimeHandling_TimeRange PROPERTY LABELS TimeHandling)

add_executable(GPSZcount_T GPSZcount_T.cpp)
target_link_libraries(GPSZcount_T gpstk)
add_test(TimeHandling_GPSZcount GPSZcount_T)
set_property(TEST TimeHandling_GPSZcount PROPERTY LABELS TimeHandling TimeStorage)
