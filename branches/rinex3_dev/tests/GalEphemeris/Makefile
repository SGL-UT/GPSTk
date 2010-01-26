# The main bojangle!
PROGRAMS = GalEphemerisTest

SRCS = GalEphemerisTest.cpp
OBJS = $(SRCS:.cpp=.o)

# Give me compilers or give me death!
FLAGS = -ansi -pedantic -Wno-unknown-pragmas #-Wall

CC = g++ -c -g $(FLAGS)
CFLAGS = -I ../../src/

LD = g++ $(FLAGS)
LIBS = ../../src/libgpstk-R3.a

# Revolution!
.SUFFIXES: .cpp .o
.cpp.o:
	$(CC) $< -o $@ $(CFLAGS)
	@echo

# Targets!
all: GalEphemerisTest

help:
	@echo
	@echo "This makefile supports the following:"
	@echo
	@echo "    make clean   Cleans all output files."
	@echo "    make help    Prints out this help."
	@echo "    make         Builds the program to the executable, GalEphemerisTest."
	@echo
	@echo "NB. Inside of this Makefile, the proper directories need to be supplied"
	@echo "    so that g++ can find the header files and the gpstk-R3 library."
	@echo
	@echo "    The defaults should work fine, but in case of errors, check these first."
	@echo

GalEphemerisTest: $(OBJS)
	$(LD) $(OBJS) -o $@ $(LIBS)
	@echo

# Source file dependencies.
#main.o: GalileoTest.o
#GalileoTest.o: GalileoTest.h++

# Clean up, clean up, everybody cleanup!
clean:
	@echo "INFO Cleaning directory..."
	rm -f *.o
	rm -f $(PROGRAMS)
	rm -f *.dSYM
	@echo