# apps/filetools/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/lib/rxio
)

add_executable(navdmp navdmp.cpp)
target_link_libraries(navdmp gpstk)
install (TARGETS navdmp DESTINATION bin)

add_executable(ficfica ficfica.cpp)
target_link_libraries(ficfica gpstk)
install (TARGETS ficfica DESTINATION bin)

add_executable(ficafic ficafic.cpp)
target_link_libraries(ficafic gpstk)
install (TARGETS ficafic DESTINATION bin)

add_executable(fic2rin fic2rin.cpp)
target_link_libraries(fic2rin gpstk)
install (TARGETS fic2rin DESTINATION bin)

add_executable(ficedit ficedit.cpp)
target_link_libraries(ficedit gpstk)
install (TARGETS ficedit DESTINATION bin)

add_executable(ephemfix ephemfix.cpp)
target_link_libraries(ephemfix rxio)
install (TARGETS ephemfix DESTINATION bin)

add_executable(glodump GloDump.cpp)
target_link_libraries(glodump gpstk)
install (TARGETS glodump DESTINATION bin)

add_executable(rinexelvstrip RinexElvStrip.cpp)
target_link_libraries(rinexelvstrip gpstk)
install (TARGETS rinexelvstrip DESTINATION bin)

add_executable(rinexthin RinexThin.cpp)
target_link_libraries(rinexthin gpstk)
install (TARGETS rinexthin DESTINATION bin)

add_executable(sp3version sp3version.cpp)
target_link_libraries(sp3version gpstk)
install (TARGETS sp3version DESTINATION bin)

add_executable(bc2sp3 bc2sp3.cpp)
target_link_libraries(bc2sp3 gpstk)
install (TARGETS bc2sp3 DESTINATION bin)

add_executable(navsum navsum.cpp)
target_link_libraries(navsum gpstk)
install (TARGETS navsum DESTINATION bin)

add_executable(ephsum EphSum.cpp)
target_link_libraries(ephsum gpstk)
install (TARGETS ephsum DESTINATION bin)

