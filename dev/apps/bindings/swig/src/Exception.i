%include "exception.i"

%rename(__str__) gpstk::Exception::what() const;
%include "../../../src/Exception.hpp"

namespace gpstk {
   namespace StringUtils  {
      NEW_EXCEPTION_CLASS(StringException, Exception);
   }
}


%define CATCHER(NAME)
   catch(const gpstk:: NAME &e) {
      SWIG_Python_Raise(
                        SWIG_NewPointerObj(new gpstk:: NAME (static_cast<const gpstk:: NAME &>(e)),
                                           SWIGTYPE_p_gpstk__##NAME,
                                           SWIG_POINTER_OWN),
                        "NAME",
                        SWIGTYPE_p_gpstk__##NAME);
      SWIG_fail;
   }
%enddef



%exception {
   try {
      $action
   }
   CATCHER(InvalidParameter)
   CATCHER(InvalidRequest)
   CATCHER(AssertionFailure)
   CATCHER(ObjectNotFound)
   CATCHER(AccessError)
   CATCHER(IndexOutOfBoundsException)
   CATCHER(InvalidArgumentException)
   CATCHER(ConfigurationException)
   CATCHER(FileMissingException)
   CATCHER(SystemSemaphoreException)
   CATCHER(SystemPipeException)
   CATCHER(SystemQueueException)
   CATCHER(OutOfMemory)
   CATCHER(NullPointerException)
   CATCHER(UnimplementedException)
   CATCHER(EndOfFile)
   catch (const gpstk::Exception &e) {
      std::string s("GPSTk exception\n"), s2(e.what());
      s = s + s2;
      SWIG_exception(SWIG_RuntimeError, s.c_str());
   }
   catch (const std::exception &e) {
      std::string s("STL exception\n"), s2(e.what());
      s = s + s2;
      SWIG_exception(SWIG_RuntimeError, s.c_str());
   }
   catch (...) {
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
