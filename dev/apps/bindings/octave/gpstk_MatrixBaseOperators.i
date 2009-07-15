//swig interface for MatrixBaseOperators.hpp

%module gpstk_MatrixBaseOperators

%{
#include "../../../src/MatrixBaseOperators.hpp"

using namespace gpstk;
%}

%inline %{
extern operator<<(std::ostream& s, const ConstMatrixBase<T, E>& a);
extern BaseClass& ident(RefMatrixBase<T, BaseClass>& m) throw (MatrixException);
extern T trace(const ConstMatrixBase<T, BaseClass>& m) throw (MatrixException);
extern T normF(const ConstMatrixBase<T, BaseClass>& m);
extern T normCol(const ConstMatrixBase<T, BaseClass>& m);
extern T slowDet(const ConstMatrixBase<T, BaseClass>& l);
%}

%include "../../../src/MatrixBaseOperators.hpp"

extern operator<<(std::ostream& s, const ConstMatrixBase<T, E>& a);
extern BaseClass& ident(RefMatrixBase<T, BaseClass>& m) throw (MatrixException);
extern T trace(const ConstMatrixBase<T, BaseClass>& m) throw (MatrixException);
extern T normF(const ConstMatrixBase<T, BaseClass>& m);
extern T normCol(const ConstMatrixBase<T, BaseClass>& m);
extern T slowDet(const ConstMatrixBase<T, BaseClass>& l);

