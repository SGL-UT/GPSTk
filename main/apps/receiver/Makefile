lall: rtAshtech

RTASH_OBJS = rtAshtech.o UnixSerialPort.o AshtechMessage.o \
	ScreenControl.o RollingFile.o RinexObsRoller.o \
	RinexNavRoller.o

rtAshtech: $(RTASH_OBJS)
#	g++ -o rtAshtech $(RTASH_OBJS) /usr/local/lib/libgpstk.a
	g++ -o rtAshtech $(RTASH_OBJS) -lgpstk

clean:
	rm -f *.o
	rm -f *.a
	rm -f *~
	rm -f *.so
	rm -f rtAshtech 
