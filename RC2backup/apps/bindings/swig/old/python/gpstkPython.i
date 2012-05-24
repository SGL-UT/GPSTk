// GPSTk wrap for Python through SWIG 1.3.25


%module gpstkPython
%{
#include <iostream>
#include <fstream>
#include <valarray>
#include <string>
#include <map>
#include <ios>

#include "../../../src/DayTime.hpp"
#include "../../../src/FFStream.hpp"
#include "../../../src/RinexObsBase.hpp"
#include "../../../src/Triple.hpp"
#include "../../../src/RinexObsHeader.hpp"
#include "../../../src/Exception.hpp"
#include "../../../src/GPSZcount.hpp"
#include "../../../src/RinexObsStream.hpp"
#include "../../../src/ECEF.hpp"
#include "../../../src/Geodetic.hpp"
#include "../../../src/geometry.hpp"
#include "../../../src/Xvt.hpp"
#include "../../../src/RinexObsData.hpp"
#include "../../../src/Position.hpp"
#include "../../../src/RAIMSolution.hpp"
#include "../../../src/AlmanacStore.hpp"
#include "../../../src/AlmOrbit.hpp"
#include "../../../src/ValidType.hpp"
#include "../../../src/X1Sequence.hpp"
// mergePCodeWords.h must be present in directory for X2Sequence
#include "../../../src/X2Sequence.hpp"
#include "../../../src/CodeBuffer.hpp"
#include "../../../src/SatID.hpp"
#include "../../../src/GenXSequence.hpp"
#include "../../../src/SVPCodeGen.hpp"
#include "../../../src/EphemerisRange.hpp"
#include "../../../src/EngEphemeris.hpp"
#include "../../../src/EngAlmanac.hpp"
#include "../../../src/EngNav.hpp"
#include "../../../src/ObservationStore.hpp"
#include "../../../src/IonoModel.hpp"
#include "../../../src/IonoModelStore.hpp"
#include "../../../src/TropModel.hpp"
#include "../../../src/ClockModel.hpp"
#include "../../../src/ObsClockModel.hpp"
#include "../../../src/EpochClockModel.hpp"
#include "../../../src/LinearClockModel.hpp"
#include "../../../src/ObsRngDev.hpp"
#include "../../../src/ORDEpoch.hpp"
#include "../../../src/RungeKutta4.hpp"
#include "../../../src/Stats.hpp"
#include "../../../src/RinexEphemerisStore.hpp"
#include "../../../src/SP3EphemerisStore.hpp"
#include "../../../src/FileHunter.hpp"
//#include "../../../src/FileStore.hpp"
#include "../../../src/RinexNavHeader.hpp"
//#include "../../../src/TabularEphemerisStore.hpp"
//#include "../../../src/EphemerisStore.hpp"
#include "../../../src/BCEphemerisStore.hpp"
#include "../../../src/SP3Header.hpp"
//#include "../../../src/SP3Base.hpp"
//#include "../../../src/RinexNavBase.hpp"
#include "../../../src/FFData.hpp"
#include "../../../src/FICStream.hpp"
#include "../../../src/FICAStream.hpp"
#include "../../../src/MSCStream.hpp"
#include "../../../src/RinexMetStream.hpp"
#include "../../../src/RinexNavStream.hpp"
#include "../../../src/SMODFStream.hpp"
#include "../../../src/SP3Stream.hpp"
#include "../../../src/FFTextStream.hpp"

#include "../../../src/FICData.hpp"
#include "../../../src/FICHeader.hpp"
#include "../../../src/RinexMetData.hpp"
#include "../../../src/RinexMetHeader.hpp"
#include "../../../src/RinexNavData.hpp"
#include "../../../src/SP3Data.hpp"
#include "../../../src/icd_200_constants.hpp"

#include "gpstkPythonUtils.cpp"
#include "streamRead.cpp"

using namespace std;
using namespace gpstk;

// RinexObsType and ExtraWaveFact are defined under scope of RinexObsHeader
// SWIG doesn't support nested C++ structs so the wrap file will call
//   RinexObsType in global space
// This is to define RinexObsType and ExtraWaveFact in the global space
typedef RinexObsHeader::RinexObsType RinexObsType;
typedef RinexObsHeader::ExtraWaveFact ExtraWaveFact;

// Same technique for nested classes in DayTime.hpp
typedef DayTime::DayTimeException DayTimeException;
typedef DayTime::FormatException FormatException;

typedef RinexObsData::RinexDatum RinexDatum;

// Originally from gpstkPython2.i
typedef AlmanacStore::NoAlmanacFound NoAlmanacFound;
typedef EngAlmanac::SVNotPresentException SVNotPresentException;
typedef TropModel::InvalidTropModel InvalidTropModel;
typedef IonoModelStore::NoIonoModelFound NoIonoModelFound;
typedef IonoModel::InvalidIonoModel InvalidIonoModel;

typedef RinexMetHeader::RinexMetType RinexMetType;
typedef RinexMetHeader::sensorType sensorType;
typedef RinexMetHeader::sensorPosType sensorPosType;
typedef EphemerisStore::NoEphemerisFound NoEphemerisFound;
typedef RinexObsData::RinexPrnMap RinexPrnMap;
%}



// Include some .i files that help parsing
%include "common.i"
%include "std_string.i"
%include "std_vector.i"
%include "std_ios.i"
%include "std_iostream.i"
//%include "std_map.i"
%include "../../../src/icd_200_constants.hpp"
%include "gpstkPythonUtils.i"



// FFData, FFStream, FFTextStream are all included up here to make the import's easier
%include "../../../src/FFData.hpp"
%include "../../../src/FFStream.hpp"
%include "../../../src/FFTextStream.hpp"



// Import .hpp files for SWIG's reference
// Order matters for some of these files
// Ordered through trial and error
// If you import a class, you can't use it in Python
// but this provides the class definitions for the other
// classes.
%import "../../../src/FFStreamError.hpp"
%import "../../../src/FFBinaryStream.hpp"
%import "../../../src/FICStreamBase.hpp"
%import "../../../src/EphemerisStore.hpp"
%import "../../../src/TabularEphemerisStore.hpp"
%import "../../../src/FileStore.hpp"
%import "../../../src/RinexNavBase.hpp"
%import "../../../src/SP3Base.hpp"
%import "../../../src/FICBase.hpp"
%import "../../../src/RinexMetBase.hpp"


// Include the files for SWIG to parse
// Order matters to some

// Exception
// Redefinition problem, similar to Triple & Xvt, also at operator<<
// Exception+DayTimeException and Triple+Xvt pairs don't
//   redefine each other's operator<<'s
// Fixing method, see Xvt
%rename (Exception_streamRead) operator<<;
%include "../../../src/Exception.hpp"
%rename (streamRead) operator<<;

//
// Code Generation Group
//
%include "../../../src/ValidType.hpp"
// X1Sequence() constructor throws an exception
%include "../../../src/X1Sequence.hpp"
// X2Sequence() constructor throws an exception
%include "../../../src/X2Sequence.hpp"
// CodeBuffer has two operator[]
%rename (operator_bracket_const) gpstk::CodeBuffer::operator[](int 
const) const;
%include "../../../src/CodeBuffer.hpp"
%include "../../../src/SatID.hpp"
%include "../../../src/GenXSequence.hpp"
%include "../../../src/SVPCodeGen.hpp"
%include "../../../src/ObservationStore.hpp"

//
// Ephemeris Group
//
%include "../../../src/AlmanacStore.hpp"
%include "../../../src/AlmOrbit.hpp"

//
// Error Modelling Group
//
%include "../../../src/IonoModel.hpp"
%include "../../../src/IonoModelStore.hpp"
%include "../../../src/TropModel.hpp"
%include "../../../src/ClockModel.hpp"
%include "../../../src/ObsClockModel.hpp"
%include "../../../src/EpochClockModel.hpp"
%include "../../../src/LinearClockModel.hpp"

//
// File Group
//
// SP3EphemerisStore.hpp contains classes that base off template classes
%include "../../../src/BCEphemerisStore.hpp"
%template (FileStore_RinexNavHeader) gpstk::FileStore<RinexNavHeader >;
%template(FileStore_SP3Header) gpstk::FileStore<SP3Header >;
%include "../../../src/RinexEphemerisStore.hpp"
%include "../../../src/SP3EphemerisStore.hpp"
%include "../../../src/FileHunter.hpp"
%include "../../../src/FICStream.hpp"
%include "../../../src/FICAStream.hpp"
%include "../../../src/MSCStream.hpp"
%include "../../../src/RinexMetStream.hpp"
%include "../../../src/RinexNavStream.hpp"
%include "../../../src/SMODFStream.hpp"
%include "../../../src/SP3Stream.hpp"

//
// File Support Group
//
%include "../../../src/RinexObsStream.hpp"
%include "../../../src/RinexObsBase.hpp"
%include "RinexObsType.i"
%include "ExtraWaveFact.i"
%include "../../../src/RinexObsHeader.hpp"
%include "RinexDatum.i"

// Template declarations for RinexObsTypeMap and RinexPrnMap
// They are typedefs in RinexObsData.hpp
// A third template, BOGUS_TEMPLATE has to be there so...
//   the RinexPrnMap declaration will compile.
// I do not know why that is.
%include "../../../src/RinexObsData.hpp"
%template() std::pair<RinexObsType, RinexDatum>;
%template (RinexObsTypeMap) std::map<RinexObsType, RinexDatum >;
%template() std::pair<RinexPrn, RinexObsData::RinexObsTypeMap>;
%template (RinexPrnMap) std::map<RinexPrn, RinexObsData::RinexObsTypeMap>;
%template() std::pair<RinexPrn, int>;
%template (BOGUS_TEMPLATE) std::map<RinexPrn, int>;

%include "../../../src/SP3Header.hpp"
%include "../../../src/RinexNavHeader.hpp"
%rename (operator_EngEphemeris) operator EngEphemeris;
%rename (operator_AlmOrbit) operator gpstk::AlmOrbit;
%include "../../../src/FICData.hpp"
%include "../../../src/FICHeader.hpp"
%include "../../../src/RinexMetData.hpp"
%include "../../../src/RinexMetHeader.hpp"
%include "sensorType.i"
%include "../../../src/RinexNavData.hpp"
%include "../../../src/SP3Data.hpp"

//
// Math Group
//
%include "../../../src/RungeKutta4.hpp"

//
// Position Group
//
// Triple
// Rename below to solve "shadowed by..." warning
%rename (operator_bracket_const) gpstk::Triple::operator[](size_t const) const;
// Under Triple.hpp, friend std::ostream operator<< will
//    clash with Xvt's def of operator<<, result in redefine
%rename (Triple_streamRead) operator<<;
%include "../../../src/Triple.hpp"
%rename (streamRead) operator<<;
// Position
%rename (printf_const) printf(std::string const &) const;
%include "../../../src/Position.hpp"
// ECEF
%include "../../../src/ECEF.hpp"
// GeoidModel
%include "../../../src/GeoidModel.hpp"
// Xvt
// The %rename directive below solves the redefinition conflict with Triple.hpp
%rename (Xvt_streamRead) operator<<;
%include "../../../src/Xvt.hpp"
// The %rename directive below restores the usual naming scheme
%rename (streamRead) operator<<;
// Geodetic
%include "../../../src/Geodetic.hpp"
// RAIM Solution
%include "../../../src/RAIMSolution.hpp"

//
// Orbit Group
//
%include "../../../src/EphemerisRange.hpp"
%include "../../../src/EngNav.hpp"
// get6bitHealth() is declared in hpp file, but not implemented in cpp file
%ignore get6bitHealth;
%include "../../../src/EngAlmanac.hpp"
// same situation with getEphkey()
%ignore getEphkey;
%include "../../../src/EngEphemeris.hpp"

//
// Time Group
//
// GPSZcount
%rename (operator_std_string) operator std::string() const;
%include "../../../src/GPSZcount.hpp"
// DayTime
%rename (Operator_GPSZcount) operator gpstk::GPSZcount() const;
%rename (printf_std_string) printf(std::string const &) const;
%rename (DayTimeException_streamRead) operator<<;
%include "DayTimeException.i"
%rename (streamRead) operator<<;
%include "../../../src/DayTime.hpp"

//
// QA Group
//
// QA that is not in ObservationStore.hpp
%include "../../../src/ObsRngDev.hpp"
%include "../../../src/ORDEpoch.hpp"

%include "streamRead.i"
