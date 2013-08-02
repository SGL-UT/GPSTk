#pragma ident "$Id$"
#ifndef GPSTK_RINEXMETDATA_HPP
#define GPSTK_RINEXMETDATA_HPP
#include <map>
#include "CommonTime.hpp"
#include "FFStream.hpp"
#include "RinexMetBase.hpp"
#include "RinexMetHeader.hpp"

namespace gpstk
{
  class RinexMetData : public RinexMetBase
  {

  public:

    RinexMetData()
      : time(gpstk::CommonTime::BEGINNING_OF_TIME)
    {}

    virtual bool isData(void) const { return true; }

    virtual void dump(std::ostream& s) const;

    bool operator<(const RinexMetData& right) const
    { return (time < right.time); }

    typedef std::map<RinexMetHeader::RinexMetType, double> RinexMetMap;

    CommonTime time;
    // RinexMetMap data;

    static const int maxObsPerLine;
    static const int maxObsPerContinuationLine;

  protected:

    void reallyPutRecord(FFStream& s) const
      throw(std::exception, FFStreamError,
            gpstk::StringUtils::StringException);

    virtual void reallyGetRecord(FFStream& s)
      throw(std::exception, FFStreamError,
            gpstk::StringUtils::StringException);

  private:

    void processFirstLine(const std::string& line,
                          const RinexMetHeader& hdr)
      throw(FFStreamError);

    void processContinuationLine(const std::string& line,
                                 const RinexMetHeader& hdr)
      throw(FFStreamError);

    CommonTime parseTime(const std::string& line) const
      throw(FFStreamError);

  };

}

#endif


%extend gpstk::RinexMetData {
   double _lookup_data(int x) {
      return $self->data[static_cast<RinexMetHeader::RinexMetType>(x)];
   }
}

%pythoncode %{
def _RinexMetData_getData(self):
    """
    Returns a dict of the (int->double) data stored.
    The ints are drawn from an enum defined in RinexMetHeader.
    For example, to get the pressure you would use:
        d = myMetData.getData()
        print d[gpstk.RinexMetHeader.PR]
    """
    data = {}
    for i in xrange(RinexMetHeader.HI + 1):
        data[i] = self._lookup_data(i)
    return data

RinexMetData.getData = _RinexMetData_getData
%}
