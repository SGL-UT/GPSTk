
%inline %{ //Insert into header portion of TimeString.hpp
   gpstk::CommonTime scanTime(const std::string& str, const std::string& fmt)
   {
      gpstk::CommonTime m;
      gpstk::scanTime(m, str, fmt);
      return m;
   }
   gpstk::CommonTime mixedScanTime(const std::string& str, const std::string& fmt)
   {
      gpstk::CommonTime m;
      gpstk::mixedScanTime(m, str, fmt);
      return m;
   }
%}
