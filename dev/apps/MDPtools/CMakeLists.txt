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
target_link_libraries(mdplib rxio)

add_executable(mdptool mdptool.cpp CNAVMessageAsm.cpp)
target_link_libraries(mdptool mdplib)
install (TARGETS mdptool DESTINATION bin)

add_executable(mdp2rinex mdp2rinex.cpp)
target_link_libraries(mdp2rinex mdplib)
install (TARGETS mdp2rinex DESTINATION bin)

add_executable(mdpEdit mdpEdit.cpp)
target_link_libraries(mdpEdit mdplib)
install (TARGETS mdpEdit DESTINATION bin)

add_executable(mdpScrubber mdpScrubber.cpp)
target_link_libraries(mdpScrubber mdplib)
install (TARGETS mdpScrubber DESTINATION bin)

add_executable(mdp2fic mdp2fic.cpp)
target_link_libraries(mdp2fic mdplib)
install (TARGETS mdp2fic DESTINATION bin)

if (UNIX)
	if (NOT CYGWIN)
		add_executable(mdpscreenx mdpscreen.cpp ScreenProc.cpp)
		# if on Solaris, use curses, otherwise, use ncurses:
		if (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
			target_link_libraries(mdpscreenx mdplib curses)
		else ()
			target_link_libraries(mdpscreenx mdplib ncurses)
		endif (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")


		install (TARGETS mdpscreenx DESTINATION bin)
	endif (NOT CYGWIN)
endif (UNIX)
