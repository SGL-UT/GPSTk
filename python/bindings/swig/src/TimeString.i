%ignore gpstk::scanTime(TimeTag& btime, const std::string& str, const std::string& fmt); //Ignore the declarations in scanTime of TimeTag and CommonTime and in mixedScanTime of CommonTime
%ignore gpstk::scanTime(CommonTime& btime, const std::string& str, const std::string& fmt);
%ignore gpstk::mixedScanTime(CommonTime& btime, const std::string& str, const std::string& fmt);

%include "../../../../dev/ext/lib/TimeHandling/TimeString.hpp" //Grab header file of TimeString

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
