# ext/apps/geomatics/rstats_test/CMakeLists.txt


# Tests are performed in this file (as opposed to tests/geomatics) for the python
# scripts because the python scripts are never written to the build dir, just to
# the install dir


###############################################################################
# Test rstats - robust stats and much more
###############################################################################
find_package(PythonInterp 3)
set(PYTHON_EXE ${PYTHON_EXECUTABLE})
add_test(NAME test_rstats
         COMMAND ${CMAKE_COMMAND}
    	 -DTEST_PROG=${CMAKE_CURRENT_SOURCE_DIR}/rstats_T.py 
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=test_rstats.exp
         -DDIFF_ARGS=-l1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/testsuccexp.cmake)
set_property(TEST test_rstats PROPERTY LABELS Geomatics)

###############################################################################

