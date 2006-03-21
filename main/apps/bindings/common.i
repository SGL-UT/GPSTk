/* Directives common to all interfaces. */

/* Math operators */
%rename(__add__) operator+;
%rename(__sub__) operator-;
%rename(__mul__) operator*;
%rename(operator_assignment) operator=;

/* Logical operators */
%rename(operator_equal_to) operator==;
%rename(operator_not_equal_to) operator!=;
%rename(operator_less_than) operator<;
%rename(operator_greater_than) operator>;
%rename(operator_less_than_equal_to) operator <=;
%rename(operator_greater_than_equal_to) operator >=;

/* Stream operators */
%rename(istream) operator<<;
%rename(ostream) operator>>;

/* Increment/decrement operators */
%ignore operator++;
%ignore operator--;
%ignore operator+=;
%ignore operator-=;

/* stdio support */
%include "cstring.i"

/* Standadard ANSI C++ library support (e.g., STL) support */
%rename(std_string) std::string;
%include "std_common.i"
%include "std_map.i"
%include "std_pair.i"
%include "std_string.i"
%include "std_vector.i"



namespace std {
   %template(vectori) vector<int>;
   %template(vectord) vector<double>;
};






