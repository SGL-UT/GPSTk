%ignore gpstk::VectorBase::operator[] (size_t i) const;
%ignore gpstk::VectorBase::operator() (size_t i) const;
%ignore gpstk::RefVectorBaseHelper::zeroTolerance;
%ignore gpstk::RefVectorBaseHelper::perator[] (size_t i);
%ignore gpstk::RefVectorBaseHelper::operator() (size_t i);
%ignore gpstk::RefVectorBaseHelper::zeroize();

%include "../../../src/VectorBase.hpp"
