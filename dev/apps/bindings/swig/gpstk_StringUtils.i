//swig interface for StringUtils.hpp

%module gpstk_StringUtils

%{
#include "../../../src/StringUtils.hpp"

using namespace gpstk;
using namespace StringUtils;
%}


%rename (Toolkit_StringUtils_stripLeadingI) gpstk::StringUtils::stripLeading(std::string const &);
%rename (Toolkit_StringUtils_stripLeadingII) gpstk::StringUtils::stripLeading(std::string &,std::string const &);
%rename (Toolkit_StringUtils_stripLeadingIII) gpstk::StringUtils::stripLeading(std::string const &,char const *);
%rename (Toolkit_StringUtils_stripLeadingIV) gpstk::StringUtils::stripLeading(std::string const &,char const);
%rename (Toolkit_StringUtils_stripLeadingV) gpstk::StringUtils::stripLeading(std::string const &,std::string::size_type);
%rename (Toolkit_StringUtils_stripLeadingVI) gpstk::StringUtils::stripLeading(std::string const &,std::string const &,std::string::size_type);
%rename (Toolkit_StringUtils_stripLeadingVII) gpstk::StringUtils::stripLeading(std::string const &,char const,std::string::size_type);
%rename (Toolkit_StringUtils_stripLeadingVIII) gpstk::StringUtils::stripLeading(std::string &,char const,std::string::size_type);
%rename (Toolkit_StringUtils_stripLeadingIX) gpstk::StringUtils::stripLeading(std::string const &,char const *,std::string::size_type);

%rename (Toolkit_StringUtils_stripTrailingI) gpstk::StringUtils::stripTrailing(std::string const &);
%rename (Toolkit_StringUtils_stripTrailingII) gpstk::StringUtils::stripTrailing(std::string const &,char const *);
%rename (Toolkit_StringUtils_stripTrailingIII) gpstk::StringUtils::stripTrailing(std::string const &,char const);
%rename (Toolkit_StringUtils_stripTrailingIV) gpstk::StringUtils::stripTrailing(std::string const &,std::string::size_type);
%rename (Toolkit_StringUtils_stripTrailingV) gpstk::StringUtils::stripTrailing(std::string &,std::string const &);
%rename (Toolkit_StringUtils_stripTrailingVI) gpstk::StringUtils::stripTrailing(std::string const &,std::string const &,std::string::size_type);
%rename (Toolkit_StringUtils_stripTrailingVII) gpstk::StringUtils::stripTrailing(std::string const &,char const,std::string::size_type);
%rename (Toolkit_StringUtils_stripTrailingVIII)  gpstk::StringUtils::stripTrailing(std::string &,char const,std::string::size_type);
%rename (Toolkit_StringUtils_stripTrailingIX) gpstk::StringUtils::stripTrailing(std::string &,char const *,std::string::size_type);

%rename (Toolkit_StringUtils_stripI) gpstk::StringUtils::strip(std::string const &);
%rename (Toolkit_StringUtils_stripII) gpstk::StringUtils::strip(std::string const &,char const *);
%rename (Toolkit_StringUtils_stripIII) gpstk::StringUtils::strip(std::string const &,char const);
%rename (Toolkit_StringUtils_stripIV) gpstk::StringUtils::strip(std::string &,char const *);
%rename (Toolkit_StringUtils_stripV) gpstk::StringUtils::strip(std::string &,std::string const &);
%rename (Toolkit_StringUtils_stripVI) gpstk::StringUtils::strip(std::string const &,std::string::size_type);
%rename (Toolkit_StringUtils_stripVII) gpstk::StringUtils::strip(std::string &,std::string const &,std::string::size_type);
%rename (Toolkit_StringUtils_stripVIII) gpstk::StringUtils::strip(std::string const &,char const,std::string::size_type);
%rename (Toolkit_StringUtils_stripIX) gpstk::StringUtils::strip(std::string &,char const,std::string::size_type);
%rename (Toolkit_StringUtils_stripX) gpstk::StringUtils::strip(std::string const &,char const *,std::string::size_type);

%rename (Toolkit_StringUtils_changeI) gpstk::StringUtils::change(std::string &,std::string const &,std::string const &);
%rename (Toolkit_StringUtils_changeII) gpstk::StringUtils::change(std::string const &,std::string const &,std::string const &,std::string::size_type);
%rename (Toolkit_StringUtils_changeIII) gpstk::StringUtils::change(std::string &,std::string const &,std::string const &,std::string::size_type,unsigned int);

%rename (Toolkit_StringUtils_rightJustifyI)  gpstk::StringUtils::rightJustify(std::string const &,std::string::size_type const); 
%rename (Toolkit_StringUtils_rightJustifyII) gpstk::StringUtils::rightJustify(std::string const &,std::string::size_type const,char const);

%rename (Toolkit_StringUtils_leftJustifyI) gpstk::StringUtils::leftJustify(std::string const &,std::string::size_type const);
%rename (Toolkit_StringUtils_leftJustifyII) gpstk::StringUtils::leftJustify(std::string &,std::string::size_type const,char const);

%rename (Toolkit_StringUtils_centerI) gpstk::StringUtils::center(std::string const &,std::string::size_type const);
%rename (Toolkit_StringUtils_centerII) gpstk::StringUtils::center(std::string &,std::string::size_type const,char const);

%rename (Toolkit_StringUtils_d2x) gpstk::StringUtils::d2x(std::string const &);
%rename (Toolkit_StringUtils_x2d) gpstk::StringUtils::x2d(std::string const &);

%rename (Toolkit_StringUtils_lowerCase) gpstk::StringUtils::lowerCase(std::string const &);

%rename (Toolkit_StringUtils_upperCase) gpstk::StringUtils::upperCase(std::string const &);

%rename (Toolkit_StringUtils_prettyPrintI) gpstk::StringUtils::prettyPrint(std::string const &);
%rename (Toolkit_StringUtils_prettyPringII) gpstk::StringUtils::prettyPrint(std::string &,std::string const &);
%rename (Toolkit_StringUtils_prettyPrintIII) gpstk::StringUtils::prettyPrint(std::string const &,std::string const &,std::string const &);
%rename (Toolkit_StringUtils_prettyPrintIV) gpstk::StringUtils::prettyPrint(std::string &,std::string const &,std::string const &,std::string const &); 
%rename (Toolkit_StringUtils_prettyPrintV)  gpstk::StringUtils::prettyPrint(std::string const &,std::string const &,std::string const &,std::string const &,std::string::size_type const);
%rename (Toolkit_StringUtils_prettyPrintVI) gpstk::StringUtils::prettyPrint(std::string &,std::string const &,std::string const &,std::string const &,std::string::size_type const,char const);

%rename (Toolkit_StringUtils_c2x) gpstk::StringUtils::c2x(std::string const &);

%include "../../../src/StringUtils.hpp"


