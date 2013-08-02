%include "exception.i"

%rename(__str__) gpstk::Exception::what() const;
%include "../../../src/Exception.hpp"

namespace gpstk {
   namespace StringUtils  {
      NEW_EXCEPTION_CLASS(StringException, Exception);
   }
}

%exception {
   try {
      $action
   } catch (const gpstk::InvalidRequest &e) {
      std::string s("Invalid Request: "), s2(e.what());
      s = s + s2;
      SWIG_exception(SWIG_RuntimeError, s.c_str());
   } catch (const gpstk::StringUtils::StringException &e) {
      std::string s("String Exception: "), s2(e.what());
      s = s + s2;
      SWIG_exception(SWIG_RuntimeError, s.c_str());
   } catch (const gpstk::EndOfFile &e) {
      std::string s("End of File: "), s2(e.what());
      s = s + s2;
      SWIG_exception(SWIG_IOError, s.c_str());
   } catch (const gpstk::Exception &e) {
      std::string s("GPSTk exception: "), s2(e.what());
      s = s + s2;
      SWIG_exception(SWIG_RuntimeError, s.c_str());
   } catch (const std::exception &e) {
      std::string s("STL exception: "), s2(e.what());
      s = s + s2;
      SWIG_exception(SWIG_RuntimeError, s.c_str());
   } catch (...) {
      SWIG_exception(SWIG_RuntimeError, "unknown exception");
   }
}

%feature("director:except") {
    if( $error != NULL ) {
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch( &ptype, &pvalue, &ptraceback );
        PyErr_Restore( ptype, pvalue, ptraceback );
        PyErr_Print();
        Py_Exit(1);
    }
}
