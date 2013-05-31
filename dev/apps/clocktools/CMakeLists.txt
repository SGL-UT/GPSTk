# apps/clocktools/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
)

add_executable(dallandev dallandev.cpp)
target_link_libraries(dallandev gpstk)

add_executable(ffp ffp.cpp)
target_link_libraries(ffp gpstk)

add_executable(mallandev mallandev.cpp)
target_link_libraries(mallandev gpstk)

add_executable(nallandev nallandev.cpp)
target_link_libraries(nallandev gpstk)

add_executable(oallandev oallandev.cpp)
target_link_libraries(oallandev gpstk)

add_executable(ohadamarddev ohadamarddev.cpp)
target_link_libraries(ohadamarddev gpstk)

add_executable(ORDPhaseParser ORDPhaseParser.cpp)
target_link_libraries(ORDPhaseParser gpstk)

add_executable(pff pff.cpp)
target_link_libraries(pff gpstk)

add_executable(rmoutlier rmoutlier.cpp)
target_link_libraries(rmoutlier gpstk)

add_executable(scale scale.cpp)
target_link_libraries(scale gpstk)

add_executable(tallandev tallandev.cpp)
target_link_libraries(tallandev gpstk)

add_executable(TIAPhaseParser TIAPhaseParser.cpp)
target_link_libraries(TIAPhaseParser gpstk)

add_executable(trunc trunc.cpp)
target_link_libraries(trunc gpstk)

