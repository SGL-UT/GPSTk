# The main bojangle!
PROGRAMS = main

SRCS = main.c++ # GalileoTest.c++
OBJS = $(SRCS:.c++=.o)

# Give me compilers or give me death!
FLAGS = -ansi -pedantic -Wno-unknown-pragmas #-Wall

CC = g++ -c -g $(FLAGS)
CFLAGS = -I ../../src/

LD = g++ $(FLAGS)
LIBS = ../../src/libgpstk-R3.a

# Revolution!
.SUFFIXES: .c++ .o
.c++.o:
	$(CC) $< -o $@ $(CFLAGS)
	@echo

# Targets!
all: main

help:
	@echo
	@echo "This makefile supports the following:"
	@echo
	@echo "    make clean   Cleans all output files."
	@echo "    make help    Prints out this help."
	@echo "    make         Builds the program to the executable, main."
	@echo
	@echo "NB. Inside of this Makefile, the proper directories need to be supplied"
	@echo "    so that g++ can find the header files and the gpstk-R3 library."
	@echo
	@echo "    The defaults should work fine, but in case of errors, check these first."
	@echo

main: $(OBJS)
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