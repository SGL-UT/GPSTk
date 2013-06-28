%rename(__str__) gpstk::Exception::what() const;
%include "../../../src/Exception.hpp"

%exception {
   try {
      $action
   } catch (Exception &e) {
      std::string s("Backend exception: "), s2(e.what());
      s = s + s2;
      SWIG_exception(SWIG_RuntimeError, s.c_str());
   } catch (...) {
      SWIG_exception(SWIG_RuntimeError, "unknown exception");
   }
}
