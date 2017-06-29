
#include <limits>
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
   cout << "       " << jd.dumpString() << endl;
   //cout << "       " << jd.jday << "," << jd.dday << "," << jd.fday << endl;

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

}
catch(InvalidRequest& ir) { cout << "caught " << ir.what() << endl; }
}

int main(int argc, char **argv)
{
   JulianDate jd;
   CommonTime ct;
   const int prec=-1;

   cout << "Empty ctor " << jd.asString() << endl;

   //                               10        20        30
   //                      123456789012345678901234567890
   compute(string("2457222.5011458123445678809239847234873456789"),prec);

 //compute(string("2457222.5011458333333333"),prec);
   compute(string("2457222.50114583333333333333"),prec);
   jd.fromIntFrac(2457222,.50114583333333333333);        // not as precise as string!
   cout << " FromIntFrac " << jd.asString(prec) << endl;
   cout << "             " << jd.dumpString() << endl;
   ct = jd.convertToCommonTime();
   cout << "Common time " << ct << endl;

   compute(string("2457931.11611111111111111111111111"),prec);
   compute(string("135000"),prec);
   compute(string("2557931.2348702938723479"),prec);

   JulianDate GPS1(1350000);
   GPS1.setTimeSystem(TimeSystem(2)); //TimeSystem::GPS);
   cout << " GPS1 " << GPS1.asString(10) << std::endl;

   JulianDate timecon(2457223,99,0.0,TimeSystem(2)); //TimeSystem::GPS);
   cout << " timecon " << timecon.asString(19) << std::endl;

   //MJD mjd(-2983.2498739,TimeSystem(2));
   MJD mjd;
   mjd.fromString("-2983.2498739");
   cout << " negative mjd dump " << mjd.dumpString() << endl;
   cout << "              str  " << mjd.asString() << endl;

      cout << "\nnum limits ----------------------------" << endl;
      cout << setw(15) << "type :" << setw(6) << "bytes"
           << setw(4) << "rdx" << setw(3) << "dg" << setw(5) << "dg10"
           << " " << setw(20) << "min" << " " << setw(20) << "max"
           << endl;
      cout << setw(15) << "char :"
           << setw(6) << sizeof(char)
           << setw(4) << numeric_limits<char>::radix
           << setw(3) << numeric_limits<char>::digits
           << setw(5) << numeric_limits<char>::digits10
           << " " << setw(20) << (int)numeric_limits<char>::min()
           << " " << setw(20) << (int)numeric_limits<char>::max()
           << endl;
      cout << setw(15) << "unsigned char :"
           << setw(6) << sizeof(unsigned char)
           << setw(4) << numeric_limits<unsigned char>::radix
           << setw(3) << numeric_limits<unsigned char>::digits
           << setw(5) << numeric_limits<unsigned char>::digits10
           << " " << setw(20) << (int)numeric_limits<unsigned char>::min()
           << " " << setw(20) << (int)numeric_limits<unsigned char>::max()
           << endl;
      cout << setw(15) << "int :"
           << setw(6) << sizeof(int)
           << setw(4) << numeric_limits<int>::radix
           << setw(3) << numeric_limits<int>::digits
           << setw(5) << numeric_limits<int>::digits10
           << " " << setw(20) << numeric_limits<int>::min()
           << " " << setw(20) << numeric_limits<int>::max()
           << endl;
      cout << setw(15) << "unsigned int :"
           << setw(6) << sizeof(unsigned int)
           << setw(4) << numeric_limits<unsigned int>::radix
           << setw(3) << numeric_limits<unsigned int>::digits
           << setw(5) << numeric_limits<unsigned int>::digits10
           << " " << setw(20) << numeric_limits<unsigned int>::min()
           << " " << setw(20) << numeric_limits<unsigned int>::max()
           << endl;
      cout << setw(15) << "long :"
           << setw(6) << sizeof(long)
           << setw(4) << numeric_limits<long>::radix
           << setw(3) << numeric_limits<long>::digits
           << setw(5) << numeric_limits<long>::digits10
           << " " << setw(20) << numeric_limits<long>::min()
           << " " << setw(20) << numeric_limits<long>::max()
           << endl;
      cout << setw(15) << "unsigned long :"
           << setw(6) << sizeof(unsigned long)
           << setw(4) << numeric_limits<unsigned long>::radix
           << setw(3) << numeric_limits<unsigned long>::digits
           << setw(5) << numeric_limits<unsigned long>::digits10
           << " " << setw(20) << numeric_limits<unsigned long>::min()
           << " " << setw(20) << numeric_limits<unsigned long>::max()
           << endl;
      cout << setw(15) << "uint64_t :"
           << setw(6) << sizeof(uint64_t)
           << setw(4) << numeric_limits<uint64_t>::radix
           << setw(3) << numeric_limits<uint64_t>::digits
           << setw(5) << numeric_limits<uint64_t>::digits10
           << " " << setw(20) << numeric_limits<uint64_t>::min()
           << " " << setw(20) << numeric_limits<uint64_t>::max()
           << endl;
      cout << setw(15) << "long long :"
           << setw(6) << sizeof(long long)
           << setw(4) << numeric_limits<long long>::radix
           << setw(3) << numeric_limits<long long>::digits
           << setw(5) << numeric_limits<long long>::digits10
           << " " << setw(20) << numeric_limits<long long>::min()
           << " " << setw(20) << numeric_limits<long long>::max()
           << endl;
      cout << setw(15) << "short :"
           << setw(6) << sizeof(short)
           << setw(4) << numeric_limits<short>::radix
           << setw(3) << numeric_limits<short>::digits
           << setw(5) << numeric_limits<short>::digits10
           << " " << setw(20) << numeric_limits<short>::min()
           << " " << setw(20) << numeric_limits<short>::max()
           << setw(13) << "epsilon"
           << setw(8) << "min_exp" << setw(8) << "max_exp"
           << setw(10) << "min_exp10" << setw(10) << "max_exp10"
           << endl;
      cout << setw(15) << "float :"
           << setw(6) << sizeof(float)
           << setw(4) << numeric_limits<float>::radix
           << setw(3) << numeric_limits<float>::digits
           << setw(5) << numeric_limits<float>::digits10
           << " " << setw(20) << numeric_limits<float>::min()
           << " " << setw(20) << numeric_limits<float>::max()
           << setw(13) << numeric_limits<float>::epsilon()
           << setw(8) << numeric_limits<float>::min_exponent
           << setw(8) << numeric_limits<float>::max_exponent
           << setw(10) << numeric_limits<float>::min_exponent10
           << setw(10) << numeric_limits<float>::max_exponent10
           << endl;
      cout << setw(15) << "double :"
           << setw(6) << sizeof(double)
           << setw(4) << numeric_limits<double>::radix
           << setw(3) << numeric_limits<double>::digits
           << setw(5) << numeric_limits<double>::digits10
           << " " << setw(20) << numeric_limits<double>::min()
           << " " << setw(20) << numeric_limits<double>::max()
           << setw(13) << numeric_limits<double>::epsilon()
           << setw(8) << numeric_limits<double>::min_exponent
           << setw(8) << numeric_limits<double>::max_exponent
           << setw(10) << numeric_limits<double>::min_exponent10
           << setw(10) << numeric_limits<double>::max_exponent10
           << endl;
      cout << setw(15) << "long double :"
           << setw(6) << sizeof(long double)
           << setw(4) << numeric_limits<long double>::radix
           << setw(3) << numeric_limits<long double>::digits
           << setw(5) << numeric_limits<long double>::digits10
           << " " << setw(20) << numeric_limits<long double>::min()
           << " " << setw(20) << numeric_limits<long double>::max()
           << setw(13) << numeric_limits<long double>::epsilon()
           << setw(8) << numeric_limits<long double>::min_exponent
           << setw(8) << numeric_limits<long double>::max_exponent
           << setw(10) << numeric_limits<long double>::min_exponent10
           << setw(10) << numeric_limits<long double>::max_exponent10
           << endl;
      cout << "bytes (sizeof) :";
      cout << " char " << sizeof(char);
      cout << " unsigned char " << sizeof(unsigned char);
      //cout << " short " << sizeof(short);
      //cout << ", int " << sizeof(int);
      //cout << ", unsigned int " << sizeof(unsigned int);
      cout << " size_t " << sizeof(size_t);
      cout << ", streampos " << sizeof(ios::streampos);
      //cout << ", long " << sizeof(long);
      //cout << ", unsigned long " << sizeof(unsigned long);
      //cout << ", long long " << sizeof(long long) << endl;
      //cout << "              :";
      //cout << " float " << sizeof(float);
      //cout << ", double " << sizeof(double);
      //cout << ", long double " << sizeof(long double);
      cout << ", void* " << sizeof(void*);
      cout << endl;
 
   return 0;
}
