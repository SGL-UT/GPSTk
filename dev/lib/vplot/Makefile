CPPFLAGS= -O2

all: library

VPLOTPLOT_OBJS=VGImage.o SVGImage.o \
	PSImage.o PSImageBase.o EPSImage.o Frame.o \
	StrokeStyle.o Color.o ViewerManager.o Path.o \
	GraphicsConstants.o Line.o Polygon.o \
	Rectangle.o Circle.o TextStyle.o Text.o \
	Marker.o Comment.o

library: $(VPLOTPLOT_OBJS)
	ar cru libvplot.a $(VPLOTPLOT_OBJS)
	ranlib libvplot.a

clean:
	rm -f *.o
	rm -f *.a
	rm -f *~
	rm -f *.so
