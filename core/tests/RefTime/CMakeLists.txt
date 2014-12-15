#Tests for RefTime Classes

#add_executable(HelmertTransform_T HelmertTransform_T.cpp ftest.cpp)
#target_link_libraries(HelmertTransform_T gpstk)
#add_test(RefTime_HelmertTransform HelmertTransform_T)

add_executable(ReferenceFrame_T ReferenceFrame_T.cpp ftest.cpp)
target_link_libraries(ReferenceFrame_T gpstk)
add_test(RefTime_ReferenceFrame ReferenceFrame_T)