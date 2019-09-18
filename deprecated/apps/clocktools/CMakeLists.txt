# apps/clocktools/CMakeLists.txt

add_executable(rmoutlier rmoutlier.cpp)
target_link_libraries(rmoutlier gpstk)
install (TARGETS rmoutlier DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(dallandev dallandev.cpp)
target_link_libraries(dallandev gpstk)
install (TARGETS dallandev DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(ffp ffp.cpp)
target_link_libraries(ffp gpstk)
install (TARGETS ffp DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(mallandev mallandev.cpp)
target_link_libraries(mallandev gpstk)
install (TARGETS mallandev DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(nallandev nallandev.cpp)
target_link_libraries(nallandev gpstk)
install (TARGETS nallandev DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(oallandev oallandev.cpp)
target_link_libraries(oallandev gpstk)
install (TARGETS oallandev DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(ohadamarddev ohadamarddev.cpp)
target_link_libraries(ohadamarddev gpstk)
install (TARGETS ohadamarddev DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(ORDPhaseParser ORDPhaseParser.cpp)
target_link_libraries(ORDPhaseParser gpstk)
install (TARGETS ORDPhaseParser DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(pff pff.cpp)
target_link_libraries(pff gpstk)
install (TARGETS pff DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(scale scale.cpp)
target_link_libraries(scale gpstk)
install (TARGETS scale DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(tallandev tallandev.cpp)
target_link_libraries(tallandev gpstk)
install (TARGETS tallandev DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(TIAPhaseParser TIAPhaseParser.cpp)
target_link_libraries(TIAPhaseParser gpstk)
install (TARGETS TIAPhaseParser DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(trunc trunc.cpp)
target_link_libraries(trunc gpstk)
install (TARGETS trunc DESTINATION "${CMAKE_INSTALL_BINDIR}")

install( PROGRAMS allanplot DESTINATION "${CMAKE_INSTALL_BINDIR}" )
install( FILES allanplot.py DESTINATION "${CMAKE_INSTALL_BINDIR}" )
# install( FILES allanplot.py DESTINATION ${PYTHON_INSTALL_PREFIX} )
# Will resolve at a later time with Python install package

