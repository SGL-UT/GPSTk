# ext/apps/geomatics/rstats_test/CMakeLists.txt


# Tests are performed in this file (as opposed to tests/geomatics) for the python
# scripts because the python scripts are never written to the build dir, just to
# the install dir


###############################################################################
# Test rstats - robust stats and much more
###############################################################################
add_test(NAME test_rstats
         COMMAND ${CMAKE_COMMAND}
    	 -DTEST_PROG=${CMAKE_CURRENT_SOURCE_DIR}/rstats_T.py 
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=test_rstats
         -DARGS=--pathexe\ ${CMAKE_CURRENT_BINARY_DIR}/../robust\ --path\ ${CMAKE_CURRENT_SOURCE_DIR}
         -DDIFF_ARGS=-l1\ -e\ 1.e-5
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/testsuccexp.cmake)
set_property(TEST test_rstats PROPERTY LABELS Geomatics)

###############################################################################

