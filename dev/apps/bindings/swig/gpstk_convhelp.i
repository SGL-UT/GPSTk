//swig interface for confhelp.hpp

%module gpstk_convhelp

%{
#include "../../../src/GeoidModel.hpp"

using namespace gpstk;
%}

%inline %{
extern double cycles2meters(double phase, double freq, GeoidModel& geoid);
extern double meters2cycles(double range, double freq, GeoidModel& geoid);
extern double cel2far(double c);
extern double far2cel(double f);
extern double mb2hg(double mb);
extern double hg2mb(double hg);
%}



%include "../../../src/GeoidModel.hpp"
