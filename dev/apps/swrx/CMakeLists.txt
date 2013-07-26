# apps/swrx/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
)

SET( LINK_FLAGS "-lpthread")
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${LINK_FLAGS}" )

add_library(simlib STATIC
normal.cpp
CCReplica.cpp
IQStream.cpp
EMLTracker.cpp 
NavFramer.cpp
)
target_link_libraries(simlib gpstk)

add_executable(gpsSim gpsSim.cpp)
target_link_libraries(gpsSim simlib gpstk)

add_executable(tracker tracker.cpp)
target_link_libraries(tracker simlib gpstk)

add_executable(corltr corltr.cpp)
target_link_libraries(corltr simlib gpstk)

add_executable(simpleNav simpleNav.cpp)
target_link_libraries(simpleNav simlib gpstk)

add_executable(iqdump iqdump.cpp)
target_link_libraries(iqdump simlib gpstk)

add_executable(codeDump codeDump.cpp)
target_link_libraries(codeDump simlib gpstk)

add_executable(position position.cpp)
target_link_libraries(position simlib gpstk)

add_executable(trackerMT trackerMT.cpp)
target_link_libraries(trackerMT simlib gpstk pthread)

add_executable(RX RX.cpp)
target_link_libraries(RX simlib gpstk pthread)

