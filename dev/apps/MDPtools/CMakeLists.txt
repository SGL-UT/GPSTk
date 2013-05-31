# apps/MDPtools/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/lib/rxio
)

add_library(mdplib STATIC
BELogEntry.cpp
MDPProcessors.cpp
SummaryProc.cpp
TrackProc.cpp
NavProc.cpp
SubframeProc.cpp
UniqueAlmStore.cpp
)
target_link_libraries(mdplib gpstk rxio)

add_executable(mdptool mdptool.cpp)
target_link_libraries(mdptool mdplib gpstk rxio)

add_executable(mdp2rinex mdp2rinex.cpp)
target_link_libraries(mdp2rinex mdplib gpstk rxio)

add_executable(mdpEdit mdpEdit.cpp)
target_link_libraries(mdpEdit mdplib gpstk rxio)

add_executable(mdpScrubber mdpScrubber.cpp)
target_link_libraries(mdpScrubber mdplib gpstk rxio)

add_executable(mdp2fic mdp2fic.cpp)
target_link_libraries(mdp2fic mdplib gpstk rxio)

if (${UNIX})

# if on Solaris, use curses, otherwise, use ncurses:
#
#add_executable(mdpscreenx mdpscreen.cpp ScreenProc.cpp)
#target_link_libraries(mdpscreenx gpstk rxio ncurses)

endif (${UNIX})