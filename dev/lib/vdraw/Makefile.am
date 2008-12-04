# $Id: Makefile.am 741 2007-09-15 16:14:43Z mvermeer $
# please add in alphabetic order
INCLUDES = -I$(srcdir)/../../src
lib_LTLIBRARIES = libvdraw.la
libvdraw_la_LDFLAGS = -version-number @GPSTK_SO_VERSION@
libvdraw_la_SOURCES = Adler32.cpp Base64Encoder.cpp Bitmap.cpp \
BorderLayout.cpp Canvas.cpp Color.cpp ColorMap.cpp Comment.cpp CRC32.cpp \
EPSImage.cpp Frame.cpp GraphicsConstants.cpp GridLayout.cpp HLayout.cpp \
InterpolatedColorMap.cpp Line.cpp Marker.cpp Palette.cpp PNG.cpp PSImage.cpp \
PSImageBase.cpp Path.cpp Rectangle.cpp StrokeStyle.cpp SVGImage.cpp Text.cpp \
TextStyle.cpp VGImage.cpp VLayout.cpp ViewerManager.cpp

incldir = $(includedir)/gpstk
incl_HEADERS = Adler32.hpp Base64Encoder.hpp BasicShape.hpp Bitmap.hpp \
BorderLayout.hpp Canvas.hpp Circle.hpp Color.hpp ColorMap.hpp Comment.hpp \
CRC32.hpp EPSImage.hpp Fillable.hpp Frame.hpp GraphicsConstants.hpp \
GridLayout.hpp HLayout.hpp Helper.hpp InterpolatedColorMap.hpp Layout.hpp \
Line.hpp Markable.hpp Marker.hpp Palette.hpp PNG.hpp PSImage.hpp \
PSImageBase.hpp Path.hpp Polygon.hpp Rectangle.hpp StrokeStyle.hpp \
SVGImage.hpp Text.hpp TextStyle.hpp VGImage.hpp VLayout.hpp VDrawException.hpp \
VGState.hpp ViewerManager.hpp
