#Tests for Math Classes

add_executable(BivarStats_T BivarStats_T.cpp)
target_link_libraries(BivarStats_T gpstk)
add_test(Math_BivarStats BivarStats_T)

add_executable(MathBase_T MathBase_T.cpp)
target_link_libraries(MathBase_T gpstk)
add_test(Math_MathBase MathBase_T)

add_executable(Matrix_Initialization_T Matrix_Initialization_T.cpp)
target_link_libraries(Matrix_Initialization_T gpstk)
add_test(Math_Matrix_Initialization Matrix_Initialization_T)

add_executable(Matrix_Operators_T Matrix_Operators_T.cpp)
target_link_libraries(Matrix_Operators_T gpstk)
add_test(Math_Matrix_Operators Matrix_Operators_T)

add_executable(Matrix_Sizing_T Matrix_Sizing_T.cpp)
target_link_libraries(Matrix_Sizing_T gpstk)
add_test(Math_Matrix_Sizing Matrix_Sizing_T)

add_executable(Matrix_InverseTranspose_T Matrix_InverseTranspose_T.cpp)
target_link_libraries(Matrix_InverseTranspose_T gpstk)
add_test(Math_Matrix_InverseTranspose Matrix_InverseTranspose_T)

add_executable(Matrix_LUDecomp_T Matrix_LUDecomp_T.cpp)
target_link_libraries(Matrix_LUDecomp_T gpstk)
add_test(Math_Matrix_LUDecomp Matrix_LUDecomp_T)

add_executable(Matrix_HouseHolder_T Matrix_HouseHolder_T.cpp)
target_link_libraries(Matrix_HouseHolder_T gpstk)
add_test(Math_Matrix_HouseHolder Matrix_HouseHolder_T)

add_executable(Matrix_Cholesky_T Matrix_Cholesky_T.cpp)
target_link_libraries(Matrix_Cholesky_T gpstk)
add_test(Math_Matrix_Cholesky Matrix_Cholesky_T)

add_executable(Matrix_SVD_T Matrix_SVD_T.cpp)
target_link_libraries(Matrix_SVD_T gpstk)
add_test(Math_Matrix_SVD Matrix_SVD_T)

add_executable(MiscMath_T MiscMath_T.cpp)
target_link_libraries(MiscMath_T gpstk)
add_test(Math_MiscMath MiscMath_T)

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
add_test(Math_Triple Triple_T)

add_executable(Vector_T Vector_T.cpp)
target_link_libraries(Vector_T gpstk)
add_test(Math_Vector Vector_T)
