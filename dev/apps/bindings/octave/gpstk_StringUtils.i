//swig interface for StringUtils.hpp

%module gpstk_StringUtils

%{
#include "../../../src/StringUtils.hpp"

using namespace gpstk;
using namespace StringUtils;
%}

%include "../../../src/StringUtils.hpp"

typedef StringUtils::StringException StringException;