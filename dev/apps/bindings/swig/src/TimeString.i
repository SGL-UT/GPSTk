%ignore gpstk::scanTime(TimeTag& btime, const std::string& str, const std::string& fmt);
%ignore gpstk::scanTime(CommonTime& btime, const std::string& str, const std::string& fmt);
%ignore gpstk::mixedScanTime(CommonTime& btime, const std::string& str, const std::string& fmt);

%include "../../../src/TimeString.hpp"

%inline %{
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
