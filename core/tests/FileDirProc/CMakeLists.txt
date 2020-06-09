#Tests for FileDirProc Classes

add_executable(FileFilter_T FileFilter_T.cpp)
target_link_libraries(FileFilter_T gpstk)
add_test(FileDirProc_FileFilter FileFilter_T)

if ( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
  message(STATUS "Cowardly refusing to test FileHunter under Windows")
else()
  add_executable(FileHunter_T FileHunter_T.cpp)
  target_link_libraries(FileHunter_T gpstk)
  add_test(FileDirProc_FileHunter FileHunter_T)
endif()

add_executable(FileSpecFind_T FileSpecFind_T.cpp)
if( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
  target_link_libraries(FileSpecFind_T gpstk shlwapi)
else()
  target_link_libraries(FileSpecFind_T gpstk)
endif()
add_test(NAME FileDirProc_FileSpecFind
  COMMAND FileSpecFind_T
  WORKING_DIRECTORY ${GPSTK_TEST_DATA_DIR})

add_executable(FileSpec_T FileSpec_T.cpp)
target_link_libraries(FileSpec_T gpstk)
add_test(FileDirProc_FileSpec FileSpec_T)

add_executable(FileStore_T FileStore_T.cpp)
target_link_libraries(FileStore_T gpstk)
add_test(FileDirProc_FileStore FileStore_T)

add_executable(FileUtils_T FileUtils_T.cpp)
target_link_libraries(FileUtils_T gpstk)
add_test(FileDirProc_FileUtils FileUtils_T)

#add_executable(RTFileFrame_T RTFileFrame_T.cpp)
#target_link_libraries(RTFileFrame_T gpstk)
#add_test(FileDirProc_RTFileFrame RTFileFrame_T)
