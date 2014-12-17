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

add_executable(Rinex_Clock_T Rinex_Clock_T.cpp)
target_link_libraries(Rinex_Clock_T gpstk)
add_test(FileHandling_Rinex_Clock* Rinex_Clock_T)

#add_executable(Rinex_Met_T Rinex_Met_T.cpp ftest.cpp)
#target_link_libraries(Rinex_Met_T gpstk)
#add_test(FileHandling_Rinex_Met Rinex_Met_T)

#add_executable(Rinex_Nav_T Rinex_Nav_T.cpp ftest.cpp)
#target_link_libraries(Rinex_Nav_T gpstk)
#add_test(FileHandling_Rinex_Nav Rinex_Nav_T)

#add_executable(Rinex_Obs_T Rinex_Obs_T.cpp ftest.cpp)
#target_link_libraries(Rinex_Obs_T gpstk)
#add_test(FileHandling_Rinex_Obs Rinex_Obs_T)

add_executable(SP3_T SP3_T.cpp)
target_link_libraries(SP3_T gpstk)
add_test(FileHandling_SP3* SP3_T)

