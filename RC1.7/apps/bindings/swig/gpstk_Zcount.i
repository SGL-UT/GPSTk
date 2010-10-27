// swig file for GPSZcount

%module gpstk_Zcount

%{
#include "../../../src/GPSZcount.hpp"


using namespace std;
using namespace gpstk;
%}

// Time Group
//
// GPSZcount
%rename (operator_std_string) operator std::string() const;
%include "../../../src/GPSZcount.hpp"
