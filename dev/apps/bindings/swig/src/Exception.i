%rename(__str__) gpstk::Exception::what() const;
%include "../../../src/Exception.hpp"

%exception {
   try {
      $action
   } catch (gpstk::InvalidRequest &e) {
      std::string s("Invalid Request: "), s2(e.what());
      s = s + s2;
      SWIG_exception(SWIG_ValueError, s.c_str());
   } catch (gpstk::EndOfFile &e) {
      std::string s("End of File: "), s2(e.what());
      s = s + s2;
      SWIG_exception(SWIG_IOError, s.c_str());
   } catch (Exception &e) {
      std::string s("Backend exception: "), s2(e.what());
      s = s + s2;
      SWIG_exception(SWIG_RuntimeError, s.c_str());
   } catch (...) {
      SWIG_exception(SWIG_RuntimeError, "unknown exception");
   }
}
