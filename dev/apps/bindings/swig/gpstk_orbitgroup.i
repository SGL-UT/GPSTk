// swig file for orbit group

%module gpstk_orbitgroup

%{
#include "../../../src/EphemerisRange.hpp"
#include "../../../src/EngNav.hpp"

using namespace std;
using namespace gpstk;
%}


// Orbit Group
//
%include "../../../src/EphemerisRange.hpp"
%include "../../../src/EngNav.hpp"

