# Testing the swig/python bindings

set( SD ${GPSTK_TEST_DATA_DIR} )
set( TD ${GPSTK_TEST_OUTPUT_DIR} )

# Since this requires that there be a valid python runtime, these tests will likely
# be quite different from the C++ library and application tests
add_test(NAME swig_load
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:${SWIG_MODULE}>
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P swig.cmake)

