# lib/vdraw/CMakeLists.txt
set (vdraw_src
Adler32.cpp
Base64Encoder.cpp
Bitmap.cpp
BorderLayout.cpp
Canvas.cpp
Color.cpp 
ColorMap.cpp
Comment.cpp
CRC32.cpp 
EPSImage.cpp
Frame.cpp
GraphicsConstants.cpp 
GridLayout.cpp
HLayout.cpp 
InterpolatedColorMap.cpp
Line.cpp 
Marker.cpp
Palette.cpp
PNG.cpp
PSImage.cpp
PSImageBase.cpp
Path.cpp
Rectangle.cpp
StrokeStyle.cpp
SVGImage.cpp
Text.cpp
TextStyle.cpp
VGImage.cpp
VLayout.cpp
ViewerManager.cpp
)
    
add_library (vdraw ${STADYN} ${vdraw_src})
include_directories (${CMAKE_SOURCE_DIR}/lib/geomatics)
install (DIRECTORY . DESTINATION include/gpstk FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
install (TARGETS vdraw DESTINATION lib)
