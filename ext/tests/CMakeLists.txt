# tests/CMakeLists.txt

# application testing
add_subdirectory (difftools)
add_subdirectory (GNSSEph)
add_subdirectory (mergetools)
add_subdirectory (multipath)
add_subdirectory (NavFilter)
add_subdirectory (Rinextools)
add_subdirectory (time)

# python bindings testings
if (BUILD_PYTHON)
    add_subdirectory (swig)
endif()