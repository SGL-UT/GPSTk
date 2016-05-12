
namespace gpstk {
   namespace StringUtils  {
      NEW_EXCEPTION_CLASS(StringException, Exception); //Create new class StringException under parent class Exception
   }
}

// This macro is used to help the C++ exceptions be translated to the python
// wraps of the exception class - this is not automatically done by SWIG!
%define CATCHER(NAME)
   catch(const gpstk:: NAME &e)
   {
      SWIG_Python_Raise(
         SWIG_NewPointerObj(new gpstk:: NAME (static_cast<const gpstk:: NAME &>(e)),
                            SWIGTYPE_p_gpstk__##NAME,
                            SWIG_POINTER_OWN),
         "NAME",
         SWIGTYPE_p_gpstk__##NAME);
      SWIG_fail;
   }
%enddef


%exception
{
   try
   {
      $action //Exception special variable 
   }
   // Explicitly handled exceptions:
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
   CATCHER(FFStreamError)


   // Other gpstk exceptions:
   catch (const gpstk::Exception &e)
   {
      std::string s("GPSTk exception\n"), s2(e.what());
      s = s + s2;
      SWIG_exception(SWIG_RuntimeError, s.c_str());
   }

   // STL exceptions:
   catch (const std::exception &e)
   {
      std::string s("STL exception\n"), s2(e.what());
      s = s + s2;
      SWIG_exception(SWIG_RuntimeError, s.c_str());
   }
   
   // any other exception:
   catch (...)
   {
      SWIG_exception(SWIG_RuntimeError, "unknown exception");
   }
}
