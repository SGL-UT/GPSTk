# $Id: Makefile.am 741 2007-09-15 16:14:43Z mvermeer $
# please add in alphabetic order
INCLUDES = -I$(srcdir)/../../src -I$(srcdir)/../vdraw
lib_LTLIBRARIES = libvplot.la
libvplot_la_LDFLAGS = -version-number @GPSTK_SO_VERSION@
libvplot_la_SOURCES = Axis.cpp LinePlot.cpp Plot.cpp ScatterPlot.cpp \
SeriesList.cpp Splitter.cpp SurfacePlot.cpp

incldir = $(includedir)/gpstk
incl_HEADERS = Axis.hpp AxisStyle.hpp LinePlot.hpp Plot.hpp plottypes.hpp \
ScatterPlot.hpp SeriesList.hpp Splitter.hpp SurfacePlot.hpp
