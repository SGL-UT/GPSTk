#Tests for GNSSCore Classes

add_executable(Convhelp_T Convhelp_T.cpp)
target_link_libraries(Convhelp_T gpstk)
add_test(GNSSCore_Convhelp* Convhelp_T)

#add_executable(IonoModel_T IonoModel_T.cpp ftest.cpp)
#target_link_libraries(IonoModel_T gpstk)
#add_test(GNSSCore_IonoModel IonoModel_T)

add_executable(IonoModelStore_T IonoModelStore_T.cpp)
target_link_libraries(IonoModelStore_T gpstk)
add_test(GNSSCore_IonoModelStore* IonoModelStore_T)

add_executable(Position_T Position_T.cpp)
target_link_libraries(Position_T gpstk)
add_test(GNSSCore_Position Position_T)

add_executable(TropModel_T TropModel_T.cpp)
target_link_libraries(TropModel_T gpstk)
add_test(GNSSCore_TropModel* TropModel_T)

add_executable(Xvt_T Xvt_T.cpp)
target_link_libraries(Xvt_T gpstk)
add_test(GNSSCore_Xvt* Xvt_T)
