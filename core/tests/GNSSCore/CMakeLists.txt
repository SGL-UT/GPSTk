#Tests for GNSSCore Classes

add_executable(Convhelp_T Convhelp_T.cpp)
target_link_libraries(Convhelp_T gpstk)
add_test(GNSSCore_Convhelp Convhelp_T)

add_executable(IonoModel_T IonoModel_T.cpp)
target_link_libraries(IonoModel_T gpstk)
add_test(GNSSCore_IonoModel IonoModel_T)

add_executable(IonoModelStore_T IonoModelStore_T.cpp)
target_link_libraries(IonoModelStore_T gpstk)
add_test(GNSSCore_IonoModelStore IonoModelStore_T)

add_executable(ObsID_T ObsID_T.cpp)
target_link_libraries(ObsID_T gpstk)
add_test(GNSSCore_ObsID ObsID_T)

add_executable(ObsIDInitializer_T ObsIDInitializer_T.cpp)
target_link_libraries(ObsIDInitializer_T gpstk)
add_test(GNSSCore_ObsIDInitializer ObsIDInitializer_T)

add_executable(Position_T Position_T.cpp)
target_link_libraries(Position_T gpstk)
add_test(GNSSCore_Position Position_T)

add_executable(TropModel_T TropModel_T.cpp)
target_link_libraries(TropModel_T gpstk)
add_test(GNSSCore_TropModel TropModel_T)

add_executable(WxObsMap_T WxObsMap_T.cpp)
target_link_libraries(WxObsMap_T gpstk)
add_test(GNSSCore_WxObsMap WxObsMap_T)

add_executable(Xvt_T Xvt_T.cpp)
target_link_libraries(Xvt_T gpstk)
add_test(GNSSCore_Xvt Xvt_T)

add_executable(SVNumXRef_T  SVNumXRef_T.cpp)
target_link_libraries(SVNumXRef_T gpstk)
add_test(GNSSCore_SVNumXRef SVNumXRef_T)

###############################################################################
add_executable(Tropdump Tropdump.cpp)
target_link_libraries(Tropdump gpstk)

add_test(NAME TropModel_Global
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:Tropdump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=TropModel_Global
         -DARGS=--trop\ Global
         -DDIFF_ARGS=-l1\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST TropModel_Global PROPERTY LABELS Tropmodel)

add_test(NAME TropModel_Saas
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:Tropdump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=TropModel_Saas
         -DARGS=--trop\ Saas
         -DDIFF_ARGS=-l1\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST TropModel_Saas PROPERTY LABELS Tropmodel)

add_test(NAME TropModel_GCAT
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:Tropdump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=TropModel_GCAT
         -DARGS=--trop\ GCAT
         -DDIFF_ARGS=-l1\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST TropModel_GCAT PROPERTY LABELS Tropmodel)

add_test(NAME TropModel_GG
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:Tropdump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=TropModel_GG
         -DARGS=--trop\ GG
         -DDIFF_ARGS=-l1\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST TropModel_GG PROPERTY LABELS Tropmodel)

add_test(NAME TropModel_GGHeight
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:Tropdump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=TropModel_GGHeight
         -DARGS=--trop\ GGHeight
         -DDIFF_ARGS=-l1\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST TropModel_GGHeight PROPERTY LABELS Tropmodel)

add_test(NAME TropModel_MOPS
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:Tropdump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=TropModel_MOPS
         -DARGS=--trop\ MOPS
         -DDIFF_ARGS=-l1\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST TropModel_MOPS PROPERTY LABELS Tropmodel)

add_test(NAME TropModel_NB
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:Tropdump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=TropModel_NB
         -DARGS=--trop\ NB
         -DDIFF_ARGS=-l1\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST TropModel_NB PROPERTY LABELS Tropmodel)

add_test(NAME TropModel_Simple
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:Tropdump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=TropModel_Simple
         -DARGS=--trop\ Simple
         -DDIFF_ARGS=-l1\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST TropModel_Simple PROPERTY LABELS Tropmodel)

add_test(NAME TropModel_Zero
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:Tropdump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=TropModel_Zero
         -DARGS=--trop\ Zero
         -DDIFF_ARGS=-l1\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST TropModel_Zero PROPERTY LABELS Tropmodel)

###############################################################################
###############################################################################
