#Tests for Utilities Classes

add_executable(BinUtils_T BinUtils_T.cpp)
target_link_libraries(BinUtils_T gpstk)
add_test(Utilities_BinUtils BinUtils_T)

add_executable(Exception_T Exception_T.cpp)
target_link_libraries(Exception_T gpstk)
add_test(Utilities_Exception Exception_T)

add_executable(StringUtils_T StringUtils_T.cpp)
target_link_libraries(StringUtils_T gpstk)
add_test(Utilities_StringUtils StringUtils_T)

add_executable(Stl_helpers_T Stl_helpers_T.cpp)
target_link_libraries(Stl_helpers_T gpstk)
add_test(Utilities_Stl_helpers Stl_helpers_T)

add_executable(ValidType_T ValidType_T.cpp)
target_link_libraries(ValidType_T gpstk)
add_test(Utilities_ValidType ValidType_T)
