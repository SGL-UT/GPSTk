#include "GPSEphemerisStore.hpp"
#include "CivilTime.hpp"
#include "TimeString.hpp"
#include "TestUtil.hpp"

using namespace std;

class GPSEphemerisStore_T
{
public:
   unsigned doGetPrnXvtTests()
   {
      TUDEF("GPSEphemerisStore","getPrnXvt");
      try
      {
            // make sure that getPrnXvt is getting the correct
            // ephemeris.  Implicitly checks the findUserEphemeris
            // method
         gpstk::GPSEphemerisStore store;
         gpstk::GPSEphemeris eph1, eph2, eph3;
/* GPSEphemeris has no bit conversion.  Fantastic.
         uint32_t eph1A[] =
            {
               0x22C37D35, 0x2E4A29A4, 0x37D48009, 0x04F4E198, 0x1D41EC15,
               0x320E27A1, 0x002579BA, 0x1523280D, 0x003FFDA6, 0x2A29ABD4,
               0x22C37D35, 0x2E4A4A34, 0x153DCC28, 0x102DD458, 0x16E3D199,
               0x3E06421D, 0x2712797D, 0x0449A87B, 0x035D9E95, 0x23281F08,
               0x22C37D35, 0x2E4A6BBC, 0x00210DF1, 0x0B2D283A, 0x3FE1892B,
               0x2541587B, 0x05C7D01B, 0x234D0E14, 0x3FE8D0D5, 0x15019354
            };
         uint32_t eph2A[] =
            {
               0x22C37D35, 0x2E7049A4, 0x37D40016, 0x00F4E1BB, 0x1D41EC2A,
               0x320E279E, 0x00257985, 0x04A327F5, 0x003FFDF5, 0x2A29BA24,
               0x22C37D35, 0x2E6FCADC, 0x04BDCC31, 0x102DD44E, 0x10CDEE29,
               0x3E06421D, 0x271281C4, 0x0449687C, 0x035D9EF9, 0x2327C738,
               0x22C37D35, 0x2E6FEB54, 0x00210DF1, 0x0B2D4100, 0x3FE18902,
               0x25415966, 0x05C7D00D, 0x234CFF0D, 0x3FE8D090, 0x04819308
            };
         uint32_t eph3A[] =
            {
               0x22C37D35, 0x2EE02910, 0x37D40016, 0x06E83CBF, 0x1D41EC2A,
               0x320E279E, 0x00257985, 0x0523985E, 0x003FFDCA, 0x2A29ABD4,
               0x22C37D35, 0x2EE04A80, 0x053E3CEC, 0x0FE91F3A, 0x03128E40,
               0x3E64C223, 0x2704E56A, 0x045DA85F, 0x035F8430, 0x23984F1C,
               0x22C37D35, 0x2EE06B08, 0x00264DC4, 0x0B04B772, 0x3FFA492B,
               0x25420F75, 0x05A0501C, 0x237C7C98, 0x3FE8EF4C, 0x0501DC50
            };

         eph1.addSubframe(&eph1A[ 0], 1919, 11, 1);
         eph1.addSubframe(&eph1A[10], 1919, 11, 1);
         eph1.addSubframe(&eph1A[20], 1919, 11, 1);

         eph2.addSubframe(&eph2A[ 0], 1919, 11, 1);
         eph2.addSubframe(&eph2A[10], 1919, 11, 1);
         eph2.addSubframe(&eph2A[20], 1919, 11, 1);

         eph3.addSubframe(&eph3A[ 0], 1919, 11, 1);
         eph3.addSubframe(&eph3A[10], 1919, 11, 1);
         eph3.addSubframe(&eph3A[20], 1919, 11, 1);
*/

         gpstk::SatID sat(11, gpstk::SatID::systemGPS);
         gpstk::ObsID obsID(gpstk::ObsID::otNavMsg,
                            gpstk::ObsID::cbL1,
                            gpstk::ObsID::tcCA);

         eph1.transmitTime = gpstk::GPSWeekSecond(1917, 568800);
         eph1.HOWtime = 568806;
         eph1.IODE = eph1.IODC = 84;
         eph1.health = 0;
         eph1.accuracyFlag = 2;
         eph1.accuracy = 4.85;
         eph1.Tgd = -1.21071935E-08;
         eph1.codeflags = 1;
         eph1.L2Pdata = 1;
            // what units? What significance? Who knows, poorly documented.
         eph1.fitDuration = 4;
         eph1.fitint = 0;
         eph1.dataLoadedFlag = true;
         eph1.satID = sat;
         eph1.obsID = obsID;
         eph1.ctToe = gpstk::GPSWeekSecond(1917, 576000);
         eph1.ctToc = gpstk::GPSWeekSecond(1917, 576000);
         eph1.af0 = -6.66419510E-04;
         eph1.af1 = -1.13686838E-12;
         eph1.af2 = 0.00000000E+00;
         eph1.M0 = 1.99681727E+00;
         eph1.dn = 5.91703218E-09;
         eph1.ecc = 1.68173878E-02;
         eph1.A = 5.15368285E+03 * 5.15368285E+03;
         eph1.OMEGA0 = 1.35418919E+00;
         eph1.i0 = 8.97860144E-01;
         eph1.w = 1.58433409E+00;
         eph1.OMEGAdot = -8.47928177E-09;
         eph1.idot = 1.43934567E-10;
         eph1.dndot = 0;
         eph1.Adot = 0;
         eph1.Cuc = -3.76813114E-06;
         eph1.Cus = 8.17701221E-06;
         eph1.Crc = 1.84968750E+02;
         eph1.Crs = -7.05000000E+01;
         eph1.Cic = 2.45869160E-07;
         eph1.Cis = -2.27242708E-07;
         eph1.adjustValidity();

         eph2.IODE = eph2.IODC = 18;
         eph2.transmitTime   = gpstk::GPSWeekSecond(1917, 570630);
         eph2.HOWtime        = 570606; //570636;
         eph2.health         = 0;
         eph2.accuracyFlag   = 0;
         eph2.accuracy       = 2.4;
         eph2.Tgd            = -1.21071935E-08;
         eph2.codeflags      = 1;
         eph2.L2Pdata        = 1;
            // what units? What significance? Who knows, poorly documented.
         eph2.fitDuration    = 4;
         eph2.fitint         = 0;
         eph2.dataLoadedFlag = true;
         eph2.satID          = sat;
         eph2.obsID          = obsID;
         eph2.ctToe          = gpstk::GPSWeekSecond(1917, 575984);
         eph2.ctToc          = gpstk::GPSWeekSecond(1917, 575984);
         eph2.af0            = -6.66412525E-04;
         eph2.af1            = -1.02318154E-12;
         eph2.af2            = 0.00000000E+00;
         eph2.M0             = 1.99448351E+00;
         eph2.dn             = 5.91703218E-09;
         eph2.ecc            = 1.68173917E-02;
         eph2.A              = 5.15368285E+03 * 5.15368285E+03;
         eph2.OMEGA0         = 1.35418933E+00;
         eph2.i0             = 8.97860150E-01;
         eph2.w              = 1.58433400E+00;
         eph2.OMEGAdot       = -8.47963893E-09;
         eph2.idot           = 1.43934567E-10;
         eph2.dndot          = 0;
         eph2.Adot           = 0;
         eph2.Cuc            = -3.76813114E-06;
         eph2.Cus            = 8.17514956E-06;
         eph2.Crc            = 1.84968750E+02;
         eph2.Crs            = -7.05000000E+01;
         eph2.Cic            = 2.45869160E-07;
         eph2.Cis            = -2.27242708E-07;
         eph2.adjustValidity();

         eph3.IODE = eph3.IODC = 20;
         eph3.transmitTime   = gpstk::GPSWeekSecond(1917, 576000);
         eph3.HOWtime        = 576006;
         eph3.health         = 0;
         eph3.accuracyFlag   = 0;
         eph3.accuracy       = 2.4;
         eph3.Tgd            = -1.21071935E-08;
         eph3.codeflags      = 1;
         eph3.L2Pdata        = 1;
            // what units? What significance? Who knows, poorly documented.
         eph3.fitDuration    = 4;
         eph3.fitint         = 0;
         eph3.dataLoadedFlag = true;
         eph3.satID          = sat;
         eph3.obsID          = obsID;
         eph3.ctToe          = gpstk::GPSWeekSecond(1917, 583184);
         eph3.ctToc          = gpstk::GPSWeekSecond(1917, 583184);
         eph3.af0            = -6.66419510E-04;
         eph3.af1            = -1.02318154E-12;
         eph3.af2            = 0.00000000E+00;
         eph3.M0             = 3.04459617E+00;
         eph3.dn             = 5.81881381E-09;
         eph3.ecc            = 1.68157691E-02;
         eph3.A              = 5.15368655E+03 * 5.15368655E+03;
         eph3.OMEGA0         = 1.35412860E+00;
         eph3.i0             = 8.97861215E-01;
         eph3.w              = 1.58440514E+00;
         eph3.OMEGAdot       = -8.43570852E-09;
         eph3.idot           = 1.70007081E-10;
         eph3.dndot          = 0;
         eph3.Adot           = 0;
         eph3.Cuc            = -3.06405127E-06;
         eph3.Cus            = 8.32602382E-06;
         eph3.Crc            = 1.80031250E+02;
         eph3.Crs            = -5.64062500E+01;
         eph3.Cic            = 2.84984708E-07;
         eph3.Cis            = -4.28408384E-08;
         eph3.adjustValidity();

         store.addEphemeris(eph1);
         store.addEphemeris(eph2);
         store.addEphemeris(eph3);

         gpstk::CommonTime start = gpstk::CivilTime(2016, 10, 8, 13, 50, 0);
         start.setTimeSystem(gpstk::TimeSystem::Any);
            // test from 13:50:00 to 18:10:00 at 5min increments
            // 2 + 1400H + 1500H + 1600H + 1700H + 3
            // 5 + 4xH = 5 + 4*12 = 53
         for (unsigned long epCount = 0; epCount < 53; epCount++)
         {
            gpstk::CommonTime what(start + (epCount * 300));
            short iodc, expected;
               // this will change if "start" changes
            if (epCount < 2)
               expected = -666;
            else if (epCount < 8)
               expected = 84;
            else if (epCount < 26)
               expected = 18;
            else
               expected = 20;
            try
            {
                  //gpstk::Xvt meh = store.getXvt(11, what, iodc);
               gpstk::GPSEphemeris eph = store.findEphemeris(sat, what);
               iodc = eph.IODC;
            }
            catch (gpstk::InvalidRequest &exc)
            {
               iodc = -666;
            }
            TUASSERTE(short, expected, iodc);
               /*
            cout << setw(2) << epCount << " "
                 << gpstk::printTime(what, "%Y/%02m/%02d %02H:%02M:%02S")
                 << " " << iodc << " expect " << expected << endl;
               */
         }
      }
      catch (gpstk::Exception &exc)
      {
         cerr << exc << endl;
         TUFAIL("Unexpected exception");
      }
      catch (...)
      {
         TUFAIL("Unexpected exception");
      }

      TURETURN();
   }
};


int main(int argc, char *argv[])
{
   unsigned total = 0;
   GPSEphemerisStore_T testClass;
   total += testClass.doGetPrnXvtTests();

   cout << "Total Failures for " << __FILE__ << ": " << total << endl;
   return total;
}
