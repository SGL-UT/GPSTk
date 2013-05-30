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
   
add_library (vplot SHARED ${vplot_src})
include_directories (${CMAKE_SOURCE_DIR}/lib/vdraw)
target_link_libraries (vplot vdraw)