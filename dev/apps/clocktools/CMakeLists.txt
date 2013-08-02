# apps/clocktools/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
)

add_executable(dallandev dallandev.cpp)
target_link_libraries(dallandev gpstk)
install (TARGETS dallandev DESTINATION bin)

add_executable(ffp ffp.cpp)
target_link_libraries(ffp gpstk)
install (TARGETS ffp DESTINATION bin)

add_executable(mallandev mallandev.cpp)
target_link_libraries(mallandev gpstk)
install (TARGETS mallandev DESTINATION bin)

add_executable(nallandev nallandev.cpp)
target_link_libraries(nallandev gpstk)
install (TARGETS nallandev DESTINATION bin)

add_executable(oallandev oallandev.cpp)
target_link_libraries(oallandev gpstk)
install (TARGETS oallandev DESTINATION bin)

add_executable(ohadamarddev ohadamarddev.cpp)
target_link_libraries(ohadamarddev gpstk)
install (TARGETS ohadamarddev DESTINATION bin)

add_executable(ORDPhaseParser ORDPhaseParser.cpp)
target_link_libraries(ORDPhaseParser gpstk)
install (TARGETS ORDPhaseParser DESTINATION bin)

add_executable(pff pff.cpp)
target_link_libraries(pff gpstk)
install (TARGETS pff DESTINATION bin)

add_executable(rmoutlier rmoutlier.cpp)
target_link_libraries(rmoutlier gpstk)
install (TARGETS rmoutlier DESTINATION bin)

add_executable(scale scale.cpp)
target_link_libraries(scale gpstk)
install (TARGETS scale DESTINATION bin)

add_executable(tallandev tallandev.cpp)
target_link_libraries(tallandev gpstk)
install (TARGETS tallandev DESTINATION bin)

add_executable(TIAPhaseParser TIAPhaseParser.cpp)
target_link_libraries(TIAPhaseParser gpstk)
install (TARGETS TIAPhaseParser DESTINATION bin)

add_executable(trunc trunc.cpp)
target_link_libraries(trunc gpstk)
install (TARGETS trunc DESTINATION bin)

