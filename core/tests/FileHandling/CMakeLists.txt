#Tests for FileHandling Classes

add_executable(Binex_ReadWrite_T Binex_ReadWrite_T.cpp)
target_link_libraries(Binex_ReadWrite_T gpstk)
add_test(FileHandling_Binex_ReadWrite Binex_ReadWrite_T)

add_executable(Binex_Types_T Binex_Types_T.cpp)
target_link_libraries(Binex_Types_T gpstk)
add_test(FileHandling_Binex_Types Binex_Types_T)

add_executable(Rinex_T Rinex_T.cpp)
target_link_libraries(Rinex_T gpstk)
add_test(FileHandling_Rinex_T Rinex_T)

add_executable(RinexClock_T RinexClock_T.cpp)
target_link_libraries(RinexClock_T gpstk)
add_test(FileHandling_RinexClock* RinexClock_T)

add_executable(RinexMet_T RinexMet_T.cpp)
target_link_libraries(RinexMet_T gpstk)
add_test(FileHandling_RinexMet_T RinexMet_T)

add_executable(RinexNav_T RinexNav_T.cpp)
target_link_libraries(RinexNav_T gpstk)
add_test(FileHandling_RinexNav_T RinexNav_T)

add_executable(RinexObs_T RinexObs_T.cpp)
target_link_libraries(RinexObs_T gpstk)
add_test(FileHandling_RinexObs_T RinexObs_T)

add_executable(SP3_T SP3_T.cpp)
target_link_libraries(SP3_T gpstk)
add_test(FileHandling_SP3* SP3_T)

