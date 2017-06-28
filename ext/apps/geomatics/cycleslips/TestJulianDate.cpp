#include "JulianDate.hpp"
#include "CommonTime.hpp"
#include "CivilTime.hpp"
#include "GPSWeekSecond.hpp"
#include "YDSTime.hpp"
#include "UnixTime.hpp"
#include "MJD.hpp"
#include "GPSWeekZcount.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;

void compute(std::string str, const int prec=-1)
{
try {
   CommonTime ct;
   JulianDate jd;

   cout << "\n=========================\nTry    " << str << endl;
   jd.fromString(str);
   cout << "       " << jd.asString(prec) << endl;

   jd.setTimeSystem(TimeSystem::GPS);
   ct = jd.convertToCommonTime();
   cout << "Common time " << ct << endl;

   cout << "Convert" << endl;
   cout << JulianDate(ct).printf("JD     %.19J %P") << endl;
   cout << JulianDate(ct).printf("JD0802 %08J %02P") << endl;
   cout << "JDstr  " << JulianDate(ct).asString(prec) << endl;
   cout << MJD(ct).printf("MJD      %.16Q") << endl;
   cout << CivilTime(ct).printf("Civil  %02m/%02d/%04Y %02H:%02M:%02S") << endl;
   cout << YDSTime(ct).printf("YDS    %Y %03j % 12.6s") << endl;
   if(jd.JD() > 2444239.5) {
      //cout << UnixTime(ct).printf("Unix   %U % 6u") << endl;
      cout << GPSWeekSecond(ct).printf("GWDS   %G %w % 13.6g") << endl;
      GPSWeekZcount wz(ct);
      cout << wz.printf("GWZ    %F % 6z") << endl;
   }

   jd.convertFromCommonTime(ct);
   cout << "Circle " << jd.asString(prec) << endl;
   cout << " comp  " << str << endl;
} catch(InvalidRequest& ir) { cout << "caught " << ir.what() << endl; }
}

int main(int argc, char **argv)
{
   JulianDate jd;
   const int prec=-1;

   cout << "Empty ctor " << jd.asString() << endl;

   //                               10        20        30
   //                      123456789012345678901234567890
   compute(string("2457222.5011458123445678809239847234873456789"),prec);
 //compute(string("2457222.5011458333333333"),prec);
   compute(string("2457222.50114583333333333333"),prec);
   compute(string("2457931.11611111111111111111111111"),prec);
   compute(string("135000"),prec);
   compute(string("2557931.2348702938723479"),prec);

   JulianDate GPS1(1350000,TimeSystem(2)); //TimeSystem::GPS);
   cout << " GPS1 " << GPS1.asString(10) << std::endl;

   JulianDate timecon;
   timecon.fromJDaySOD(2457223,99.0,TimeSystem(2)); //TimeSystem::GPS);
   cout << " timecon " << timecon.asString(19) << std::endl;

   return 0;
}
