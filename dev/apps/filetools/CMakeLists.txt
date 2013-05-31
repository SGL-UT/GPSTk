# apps/filetools/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/lib/rxio
)

add_executable(navdmp navdmp.cpp)
target_link_libraries(navdmp gpstk)

add_executable(ficfica ficfica.cpp)
target_link_libraries(ficfica gpstk)

add_executable(ficafic ficafic.cpp)
target_link_libraries(ficafic gpstk)

add_executable(fic2rin fic2rin.cpp)
target_link_libraries(fic2rin gpstk)

add_executable(ficedit ficedit.cpp)
target_link_libraries(ficedit gpstk)

add_executable(ephemfix ephemfix.cpp)
target_link_libraries(ephemfix gpstk rxio)

add_executable(rinexelvstrip RinexElvStrip.cpp)
target_link_libraries(rinexelvstrip gpstk)

add_executable(rinexthin RinexThin.cpp)
target_link_libraries(rinexthin gpstk)

add_executable(sp3version sp3version.cpp)
target_link_libraries(sp3version gpstk)

add_executable(bc2sp3 bc2sp3.cpp)
target_link_libraries(bc2sp3 gpstk)

add_executable(navsum navsum.cpp)
target_link_libraries(navsum gpstk)

add_executable(ephsum EphSum.cpp)
target_link_libraries(ephsum gpstk)

