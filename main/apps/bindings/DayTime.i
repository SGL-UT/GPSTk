%module DayTime
%{
/* Includes the header in the wrapper code */
#include "../../src/DayTime.hpp"
%}

%rename(DayTime_printString) DayTime::printf(const char *fmt) const;

class DayTime {

public:
      enum TimeFrame
      {
         Unknown,    /**< unknown time frame */
         UTC,        /**< Coordinated Universal Time (e.g., from NTP) */
         LocalSystem,/**< time from a local system clock */
         GPS_Tx,     /**< GPS transmit Time (paper clock) (e.g., 15 smooth) */
         GPS_Rx,     /**< GPS receive time (paper clock) */
            // (e.g., rx data if clock bias is applied)
         GPS_SV,     /**< SV time frame (e.g., 211 1.5s/6packs) */
         GPS_Receiver/**< Receiver time (e.g., 30s, raw 1.5s) */
      };

      // ----------- Part  1: constants --------------

      static const long FACTOR;
      static const double JD_TO_MJD;
      static const long MJD_JDAY;
      static const long GPS_EPOCH_JDAY;
      static const long GPS_EPOCH_MJD;
      static const long UNIX_MJD;
      static const long HALFWEEK;
      static const long FULLWEEK;
      static const long SEC_DAY;
      static const long MS_PER_DAY;
      static const double ONE_NSEC_TOLERANCE;
      static const double ONE_USEC_TOLERANCE;
      static const double ONE_MSEC_TOLERANCE;
      static const double ONE_SEC_TOLERANCE;
      static const double ONE_MIN_TOLERANCE;
      static const double ONE_HOUR_TOLERANCE;
      static double DAYTIME_TOLERANCE;


      static const long BEGIN_LIMIT_JDAY;
      static const long END_LIMIT_JDAY;

      static const DayTime BEGINNING_OF_TIME;
      static const DayTime END_OF_TIME;

      static bool DAYTIME_TEST_VALID;

      // ----------- Part  2: member functions: tolerance ------------
      static double setDayTimeTolerance(const double tol)
         throw();
      static double getDayTimeTolerance() 
         throw();

      DayTime& setTolerance(const double tol) throw();

      // --------- Part  3: member functions: constructors ---------
      DayTime();

      DayTime(short GPSWeek,
              double GPSSecond,
              TimeFrame f = Unknown);

      DayTime(unsigned long fullZcount, 
              TimeFrame f = Unknown);

      DayTime(short year,
              short month,
              short day,
              short hour,
              short minute,
              double second,
              TimeFrame f = Unknown);

      DayTime(double MJD, 
              TimeFrame f = Unknown);

      DayTime(short year,
              short doy,
              double sod, 
              TimeFrame f = Unknown);

     // --------- Part  4: member functions: assignment and copy ----------

     DayTime(const DayTime &right);
      
     DayTime& operator=(const DayTime& right);

     // ------------- Part 5: member functions: arithmetic ----------------

     double operator-(const DayTime& right) const;

     DayTime operator+(double sec) const;

     DayTime operator-(double sec) const;

     DayTime& addSeconds(double seconds);

     DayTime& addSeconds(long seconds);

      DayTime& addMilliSeconds(long msec);

      DayTime& addMicroSeconds(long usec);

         // ----------- Part  6: member functions: comparisons ------------

      bool operator==(const DayTime &right) const;

      bool operator!=(const DayTime &right) const;

      bool operator<(const DayTime &right) const;
        
      bool operator>(const DayTime &right) const;

      bool operator<=(const DayTime &right) const;

      bool operator>=(const DayTime &right) const;


         // ----------- Part  7: member functions: time frame ------------

      DayTime& setAllButTimeFrame(const DayTime& right);
      
      DayTime& setTimeFrame(TimeFrame f);
      
      TimeFrame getTimeFrame() const; 

   // ----------- Part  8: member functions: get --------------

   double JD() const
         throw();
   
   double MJD() const
         throw();

   short year() const
         throw();

      short month() const;

      short day() const;

         /// Get day of week
      short dayOfWeek() const;

         /// Get year, month and day of month
      void getYMD(int& yy, int& mm, int& dd) const;

      short hour() const;
      short minute() const;
      double second() const;
      double secOfDay() const;
      short GPS10bitweek() const;
      long GPSzcount() const;
      long GPSzcountFloor() const;
      double GPSsecond() const;
      double GPSsow() const;
      short GPSday() const;
      short GPSfullweek() const;
      short GPSyear() const;
      short DOYyear() const;
      short DOY() const;
      double DOYsecond() const;
      double MJDdate() const;
      long double getMJDasLongDouble() const;
      struct timeval unixTime() const;
      unsigned long fullZcount() const;
      unsigned long fullZcountFloor() const;

         // ----------- Part  9: member functions: set ------------

      DayTime& setYMDHMS(short year,
                         short month,
                         short day, 
                         short hour, 
                         short min,
                         double sec, 
                         TimeFrame f = Unknown);

      DayTime& setGPS(short week,
                      double sow, 
                      TimeFrame f = Unknown);

      DayTime& setGPS(short week, 
                      long zcount, 
                      TimeFrame f = Unknown);

      DayTime& setGPS(short week, 
                      double sow,
                      short year, 
                      TimeFrame f = Unknown);

      DayTime& setGPS(unsigned long Zcount, 
                      TimeFrame f = Unknown);

      DayTime& setGPSfullweek(short fullweek,
                              double sow,
                              TimeFrame f = Unknown);

      DayTime& setGPSZcount(const GPSZcount& z,
                            TimeFrame f = Unknown);

      DayTime& setYDoySod(short year,
                          short day_of_year, 
                          double sec_of_day, 
                          TimeFrame f = Unknown);

      DayTime& setMJD(long double mjd,
                      TimeFrame f = Unknown);

      DayTime& setMJDdate(long double mjd, 
                          TimeFrame f = Unknown);

      DayTime& setMJD(double mjd, 
                      TimeFrame f = Unknown);

      DayTime& setMJDdate(double mjd, 
                          TimeFrame f = Unknown);

      DayTime& setUnix(const struct timeval& t, 
                       TimeFrame f = Unknown);

      DayTime& setANSI(const time_t& t,
                       TimeFrame f = Unknown);

      DayTime& setSystemTime();

      DayTime& setLocalTime();

      DayTime& setYMD(int yy,
                      int mm,
                      int dd,
                      TimeFrame f = Unknown);

      DayTime& setHMS(int hh,
                      int mm,
                      double sec, 
                      TimeFrame f = Unknown);

      DayTime& setSecOfDay(double sod,
                           TimeFrame f = Unknown);

      DayTime& setYDoy(int yy,
                       int doy,
                       TimeFrame f = Unknown);

      DayTime& setToString(const std::string& str, 
                           const std::string& fmt);

      std::string printf(const char *fmt) const;
       
      std::string printf(const std::string& fmt) const;

      std::string asString() const;

      void dump(std::ostream& s) const;

         // ----------- Part 11: functions: fundamental conversions ----------

      static void convertJDtoCalendar(long jd, 
                                      int& iyear, 
                                      int& imonth, 
                                      int& iday);

      static long convertCalendarToJD(int iyear, 
                                      int imonth,
                                      int iday); 

      static void convertSODtoTime(double sod,
                                   int& hh, 
                                   int& mm, 
                                   double& sec);

      static double convertTimeToSOD(int hh,
                                     int mm, 
                                     double sec);
};

%rename(DayTime_printf) DayTime::printf;