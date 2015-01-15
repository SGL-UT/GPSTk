#Tests for Math Classes

add_executable(BivarStats_T BivarStats_T.cpp)
target_link_libraries(BivarStats_T gpstk)
add_test(Math_BivarStats* BivarStats_T)

add_executable(Matrix_T Matrix_T.cpp)
target_link_libraries(Matrix_T gpstk)
add_test(Math_Matrix Matrix_T)

add_executable(MiscMath_T MiscMath_T.cpp)
target_link_libraries(MiscMath_T gpstk)
add_test(Math_MiscMath* MiscMath_T)

add_executable(PolyFit_T PolyFit_T.cpp)
target_link_libraries(PolyFit_T gpstk)
add_test(Math_PolyFit PolyFit_T)

add_executable(RACRotation_T RACRotation_T.cpp)
target_link_libraries(RACRotation_T gpstk)
add_test(Math_RACRotation RACRotation_T)

add_executable(Stats_T Stats_T.cpp)
target_link_libraries(Stats_T gpstk)
add_test(Math_Stats Stats_T)

add_executable(Stats_TwoSampleStats_T Stats_TwoSampleStats_T.cpp)
target_link_libraries(Stats_TwoSampleStats_T gpstk)
add_test(Math_Stats_TwoSampleStats Stats_TwoSampleStats_T)

add_executable(Triple_T Triple_T.cpp)
target_link_libraries(Triple_T gpstk)
add_test(Math_Triple* Triple_T)

add_executable(Vector_T Vector_T.cpp)
target_link_libraries(Vector_T gpstk)
add_test(Math_Vector Vector_T)