CCARGS = -I/home/mendiola/gpstk/gpstk/RC2.0/src
LLARGS = -L/home/mendiola/gpstk/gpstk/RC2.0/src -lgpstk -lstdc++
OBJ = $(SOURCE:.cpp=.o)
 
clean:
	$(RM) $(EXE) $(OBJ)

BrcKeplerOrbit: BrcKeplerOrbit.cpp BrcKeplerOrbit.hpp 
	gcc -c $(CCARGS) BrcKeplerOrbit.cpp

xBrcKeplerOrbit: xBrcKeplerOrbit.cpp BrcKeplerOrbit EngEphemeris BrcClockCorrection
	gcc $(CCARGS) xBrcKeplerOrbit.cpp BrcKeplerOrbit.o EngEphemeris.o BrcClockCorrection.o $(LLARGS) -o xBrcKeplerOrbit

BrcClockCorrection: BrcClockCorrection.cpp BrcClockCorrection.hpp
	gcc -c $(CCARGS) BrcClockCorrection.cpp

xBrcClockCorrection: xBrcClockCorrection.cpp BrcClockCorrection EngEphemeris BrcKeplerOrbit
	gcc $(CCARGS) xBrcClockCorrection.cpp BrcClockCorrection.o EngEphemeris.o BrcKeplerOrbit.o $(LLARGS) -o xBrcClockCorrection

EngEphemeris: EngEphemeris.cpp EngEphemeris.hpp
	gcc -c $(CCARGS) EngEphemeris.cpp

