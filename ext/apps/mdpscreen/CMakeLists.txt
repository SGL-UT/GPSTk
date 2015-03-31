# apps/mdpscreen/CMakeLists.txt

if (UNIX)
   if (NOT CYGWIN)
        add_executable(mdpscreenx mdpscreen.cpp ScreenProc.cpp MDPProcessors.cpp)
       # if on Solaris, use curses, otherwise, use ncurses:
       if (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
            target_link_libraries(mdpscreenx gpstk curses)
       else ()
           target_link_libraries(mdpscreenx gpstk ncurses tinfo)
       endif (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
       install (TARGETS mdpscreenx DESTINATION bin)
       install (PROGRAMS mdpscreen DESTINATION bin)
   endif (NOT CYGWIN)
endif (UNIX)

# python application install targets
install( PROGRAMS mdpPhasePlotter.py DESTINATION bin )
