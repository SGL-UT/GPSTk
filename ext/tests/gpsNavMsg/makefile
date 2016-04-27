CCARGS = -I/home/renfrob/gpstksvn/gpstk/dev/src
LLARGS = -L/home/renfrob/gpstksvn/gpstk/dev/src -lgpstk -lstdc++
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

CNAV2EphClk: CNAV2EphClk.cpp CNAV2EphClk.hpp
	gcc -c $(CCARGS) CNAV2EphClk.cpp

xCNAV2EphClk: xCNAV2EphClk.cpp 
	gcc $(CCARGS) xCNAV2EphClk.cpp $(LLARGS) -o xCNAV2EphClk

CNAVEphemeris: CNAVEphemeris.cpp CNAVEphemeris.hpp
	gcc -c $(CCARGS) CNAVEphemeris.cpp

xCNAVEphemeris: xCNAVEphemeris.cpp CNAVEphemeris EngEphemeris BrcKeplerOrbit PackedNavBits
	gcc $(CCARGS) xCNAVEphemeris.cpp CNAVEphemeris.o EngEphemeris.o BrcKeplerOrbit.o PackedNavBits.o $(LLARGS) -o xCNAVEphemeris

CNAVClock: CNAVClock.cpp CNAVClock.hpp
	gcc -c $(CCARGS) CNAVClock.cpp

xCNAVClock: xCNAVClock.cpp CNAVClock EngEphemeris BrcClockCorrection PackedNavBits
	gcc $(CCARGS) xCNAVClock.cpp CNAVClock.o EngEphemeris.o BrcClockCorrection.o PackedNavBits.o $(LLARGS) -o xCNAVClock

xReadICETestData: xReadICETestData.cpp
	gcc $(CCARGS) xReadICETestData.cpp $(LLARGS) -o xReadICETestData
 	
xCNAVEphemerisPNB: xCNAVEphemerisPNB.cpp PackedNavBits CNAVEphemeris
	gcc $(CCARGS) xCNAVEphemerisPNB.cpp PackedNavBits.o CNAVEphemeris.o $(LLARGS) -o xCNAVEphemerisPNB

xCNAVClockPNB: xCNAVClockPNB.cpp PackedNavBits CNAVClock
	gcc $(CCARGS) xCNAVClockPNB.cpp PackedNavBits.o CNAVClock.o $(LLARGS) -o xCNAVClockPNB

xCNAV2EphClockPNB: xCNAV2EphClockPNB.cpp 
	gcc $(CCARGS) xCNAV2EphClockPNB.cpp $(LLARGS) -o xCNAV2EphClockPNB

EngEphemeris: EngEphemeris.cpp EngEphemeris.hpp
	gcc -c $(CCARGS) EngEphemeris.cpp

GenSyntheticCNAVData: GenSyntheticCNAVData.cpp 
	gcc $(CCARGS) GenSyntheticCNAVData.cpp $(LLARGS) -o GenSyntheticCNAVData

