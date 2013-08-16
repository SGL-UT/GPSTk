# lib/vplot/CMakeLists.txt
set (vplot_src
Axis.cpp
LinePlot.cpp
Plot.cpp
ScatterPlot.cpp
SeriesList.cpp
Splitter.cpp
SurfacePlot.cpp
)
   
add_library (vplot ${STADYN} ${vplot_src})
include_directories (${CMAKE_SOURCE_DIR}/lib/vdraw)
install (DIRECTORY . DESTINATION include/gpstk FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
target_link_libraries (vplot vdraw)
install (TARGETS vplot DESTINATION lib)
