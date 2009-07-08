// swig file for orbit group

%module gpstk_orbitgroup

%{
#include "../../../src/EphemerisRange.hpp"
#include "../../../src/EngNav.hpp"
//#include "../../../src/EngAlmanac.hpp"
//#include "../../../src/EngEphemeris.hpp"

using namespace std;
using namespace gpstk;
%}

//typedef EngAlmanac::SVNotPresentException SVNotPresentException;
//%rename (operator_EngEphemeris) operator EngEphemeris;


// Orbit Group
//
%include "../../../src/EphemerisRange.hpp"
%include "../../../src/EngNav.hpp"
// get6bitHealth() is declared in hpp file, but not implemented in cpp file
//%ignore get6bitHealth;
//%include "../../../src/EngAlmanac.hpp"
// same situation with getEphkey()
//%ignore getEphkey;
//%include "../../../src/EngEphemeris.hpp"
