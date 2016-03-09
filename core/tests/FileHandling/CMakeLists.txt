#Tests for FileHandling Classes

add_executable(Binex_Types_T Binex_Types_T.cpp)
target_link_libraries(Binex_Types_T gpstk)
add_test(FileHandling_Binex_Types Binex_Types_T)

add_executable(Binex_Attrs_T Binex_Attrs_T.cpp)
target_link_libraries(Binex_Attrs_T gpstk)
add_test(FileHandling_Binex_Attrs Binex_Attrs_T)

add_executable(Binex_UpdateExtract_T Binex_UpdateExtract_T.cpp)
target_link_libraries(Binex_UpdateExtract_T gpstk)
add_test(FileHandling_Binex_UpdateExtract Binex_UpdateExtract_T)

add_executable(Binex_ReadWrite_T Binex_ReadWrite_T.cpp)
target_link_libraries(Binex_ReadWrite_T gpstk)
add_test(FileHandling_Binex_ReadWrite Binex_ReadWrite_T)

add_executable(Rinex_T Rinex_T.cpp)
target_link_libraries(Rinex_T gpstk)
add_test(FileHandling_Rinex_T Rinex_T)

add_executable(RinexClock_T RinexClock_T.cpp)
target_link_libraries(RinexClock_T gpstk)
add_test(FileHandling_RinexClock_T RinexClock_T)

add_executable(RinexMet_T RinexMet_T.cpp)
target_link_libraries(RinexMet_T gpstk)
add_test(FileHandling_RinexMet_T RinexMet_T)

add_executable(RinexNav_T RinexNav_T.cpp)
target_link_libraries(RinexNav_T gpstk)
add_test(FileHandling_RinexNav_T RinexNav_T)

add_executable(RinexObs_T RinexObs_T.cpp)
target_link_libraries(RinexObs_T gpstk)
add_test(FileHandling_RinexObs_T RinexObs_T)

add_executable(Rinex3Obs_T Rinex3Obs_T.cpp)
target_link_libraries(Rinex3Obs_T gpstk)
add_test(FileHandling_Rinex3Obs_T Rinex3Obs_T)

add_executable(Rinex3Nav_T Rinex3Nav_T.cpp)
target_link_libraries(Rinex3Nav_T gpstk)
add_test(FileHandling_Rinex3Nav_T Rinex3Nav_T)

add_executable(Rinex3Clock_T Rinex3Clock_T.cpp)
target_link_libraries(Rinex3Clock_T gpstk)
add_test(FileHandling_Rinex3Clock Rinex3Clock_T)

add_executable(SP3_T SP3_T.cpp)
target_link_libraries(SP3_T gpstk)
add_test(FileHandling_SP3 SP3_T)

add_executable(FFBinaryStream_T FFBinaryStream_T.cpp)
target_link_libraries(FFBinaryStream_T gpstk)
add_test(FileHandling_FFBinaryStream FFBinaryStream_T)
