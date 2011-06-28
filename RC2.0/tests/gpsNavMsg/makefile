CCARGS = -I/home/mendiola/gpstk/gpstk/RC2.0/src
LLARGS = -L/home/mendiola/gpstk/gpstk/RC2.0/src -lgpstk -lstdc++
OBJ = $(SOURCE:.cpp=.o)
 
clean:
	$(RM) $(EXE) $(OBJ)

BrcKeplerOrbit: BrcKeplerOrbit.cpp BrcKeplerOrbit.hpp 
	gcc -c $(CCARGS) BrcKeplerOrbit.cpp

xBrcKeplerOrbit: xBrcKeplerOrbit.cpp BrcKeplerOrbit EngEphemeris BrcClockCorrection CNAV2EphClk
	gcc $(CCARGS) xBrcKeplerOrbit.cpp BrcKeplerOrbit.o EngEphemeris.o BrcClockCorrection.o CNAV2EphClk.o $(LLARGS) -o xBrcKeplerOrbit

BrcClockCorrection: BrcClockCorrection.cpp BrcClockCorrection.hpp
	gcc -c $(CCARGS) BrcClockCorrection.cpp

xBrcClockCorrection: xBrcClockCorrection.cpp BrcClockCorrection EngEphemeris BrcKeplerOrbit CNAV2EphClk
	gcc $(CCARGS) xBrcClockCorrection.cpp BrcClockCorrection.o EngEphemeris.o BrcKeplerOrbit.o CNAV2EphClk.o $(LLARGS) -o 		xBrcClockCorrection

CNAV2EphClk: CNAV2EphClk.cpp CNAV2EphClk.hpp
	gcc -c $(CCARGS) CNAV2EphClk.cpp

xCNAV2EphClk: xCNAV2EphClk.cpp CNAV2EphClk EngEphemeris BrcKeplerOrbit BrcClockCorrection
	gcc $(CCARGS) xCNAV2EphClk.cpp CNAV2EphClk.o EngEphemeris.o BrcKeplerOrbit.o BrcClockCorrection.o $(LLARGS) -o xCNAV2EphClk

EngEphemeris: EngEphemeris.cpp EngEphemeris.hpp
	gcc -c $(CCARGS) EngEphemeris.cpp

