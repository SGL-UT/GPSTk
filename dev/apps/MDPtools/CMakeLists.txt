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

add_executable(mdptool mdptool.cpp CNAVMessageAsm.cpp)
target_link_libraries(mdptool mdplib gpstk rxio)
install (TARGETS mdptool DESTINATION bin)

add_executable(mdp2rinex mdp2rinex.cpp)
target_link_libraries(mdp2rinex mdplib gpstk rxio)
install (TARGETS mdp2rinex DESTINATION bin)

add_executable(mdpEdit mdpEdit.cpp)
target_link_libraries(mdpEdit mdplib gpstk rxio)
install (TARGETS mdpEdit DESTINATION bin)

add_executable(mdpScrubber mdpScrubber.cpp)
target_link_libraries(mdpScrubber mdplib gpstk rxio)
install (TARGETS mdpScrubber DESTINATION bin)

add_executable(mdp2fic mdp2fic.cpp)
target_link_libraries(mdp2fic mdplib gpstk rxio)
install (TARGETS mdp2fic DESTINATION bin)

if (UNIX)
	add_executable(mdpscreenx mdpscreen.cpp ScreenProc.cpp)
	# if on Solaris, use curses, otherwise, use ncurses:
	if (${CMAKE_SYSTEM_NAME} MATCHES "Solaris")
		target_link_libraries(mdpscreenx mdplib gpstk rxio curses)
	else ()
		target_link_libraries(mdpscreenx mdplib gpstk rxio ncurses)
	endif (${CMAKE_SYSTEM_NAME} MATCHES "Solaris")

	if (NOT CYGWIN)
		install (TARGETS mdpscreenx DESTINATION bin)
	endif (NOT CYGWIN)
endif (UNIX)
