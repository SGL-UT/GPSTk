//swig interface for confhelp.hpp

%module gpstk_convhelp

%{
#include "../../../src/GeoidModel.hpp"
#include "../../../src/convhelp.hpp"

using namespace gpstk;
%}

double cycles2meters(double phase, double freq, GeoidModel& geoid);
double meters2cycles(double range, double freq, GeoidModel& geoid);
double cel2far(double c);
double far2cel(double f);
double mb2hg(double mb);
double hg2mb(double hg);



%include "../../../src/GeoidModel.hpp"
