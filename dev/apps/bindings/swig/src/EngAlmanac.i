#pragma ident "$Id$"
#ifndef GPSTK_ENGALMANAC_HPP
#define GPSTK_ENGALMANAC_HPP
#include "Exception.hpp"
#include "EngNav.hpp"
#include "AlmOrbit.hpp"
#include "Xvt.hpp"
#include "StringUtils.hpp"

namespace gpstk
{

   class EngAlmanac : public EngNav
   {
   public:

      NEW_EXCEPTION_CLASS(SVNotPresentException, gpstk::InvalidRequest);

      typedef std::map<short, unsigned char, std::less<short> > SVBitsMap;

      EngAlmanac() throw();

      virtual ~EngAlmanac() {}

      bool addSubframe(const long subframe[10], const int gpsWeek)
         throw(gpstk::InvalidParameter);

      bool isData(SatID sat) const throw();

      double getEcc(SatID sat) const throw(SVNotPresentException);

      double getIOffset(SatID sat) const throw(SVNotPresentException);

      double getOmegadot(SatID sat) const throw(SVNotPresentException);

      short get6bitHealth(SatID sat) const throw(SVNotPresentException);

      short getSVHealth(SatID sat) const throw(SVNotPresentException);

      short getSVConfig(SatID sat) const throw(SVNotPresentException);

      double getAhalf(SatID sat) const throw(SVNotPresentException);

      double getA(SatID sat) const throw(SVNotPresentException);

      double getOmega0(SatID sat) const throw(SVNotPresentException);

      double getW(SatID sat) const throw(SVNotPresentException);

      double getM0(SatID sat) const throw(SVNotPresentException);

      double getAf0(SatID sat) const throw(SVNotPresentException);

      double getAf1(SatID sat) const throw(SVNotPresentException);

      double getToa() const throw();

      double getToa(SatID sat) const throw(SVNotPresentException);

      double getXmitTime(SatID sat) const throw(SVNotPresentException);

      short getFullWeek(SatID sat) const throw(SVNotPresentException);

      void getIon(double a[4], double b[4]) const
         throw(InvalidRequest);

      void getUTC(double& a0, double& a1, double& deltaTLS, long& tot,
                  int& WNt, int& WNLSF, int& DN, double& deltaTLSF) const
         throw(InvalidRequest);

      short getAlmWeek() const throw();

      AlmOrbit getAlmOrbElem(SatID sat) const
         throw(SVNotPresentException);

      AlmOrbits getAlmOrbElems() const
      { return almPRN; }

      Xvt svXvt(SatID sat, const CommonTime& t) const
         throw(SVNotPresentException);

      Xvt svXvt(short prn, const CommonTime& t) const
         throw(SVNotPresentException)
         { SatID sat(prn,SatID::systemGPS); return svXvt(sat,t); }

      void dump(std::ostream& s = std::cout, bool checkFlag=true) const;

      bool check(std::ostream& s) const;

   // protected:
   //    void checkSVHere(SatID sat) const throw(SVNotPresentException);
   //    double alpha[4];
   //    double beta[4];
   //    double A0;                   /**< Bias term of difference polynomial */
   //    double A1;                   /**< Drift term of difference polynomial */
   //    double dt_ls;                /**< time increment due to leap seconds */
   //    double dt_lsf;               /**< scheduled future time increment due to
   //                                    leap seconds */
   //    long t_ot;                   /**< reference time */
   //    long t_oa;                   /**< Toa from page id 51 (subframe 5,
   //                                    pg 25) */
   //    int wn_t;                    /**< reference week of current leap
   //                                    second */
   //    int wn_lsf;                  /**< week number of last/next leap
   //                                    second */
   //    short alm_wk;                /**< GPS Week of the Almanac from the last
   //                                    page of orbital data */
   //    unsigned char dn;            /**< reference day # of future leap
   //                                    second */
   //    SVBitsMap health;            /**< satellite health array */
   //    std::string special_msg;     /**< Special message from GPS */

   //    SVBitsMap SV_config;         /**< 4 bit anti-spoofing/SV config sats. */
   //       //@}

   //    AlmOrbits almPRN;
   //    bool haveUTC;

   // private:
   //    bool operator==(const EngAlmanac&);
   //    bool operator!=(const EngAlmanac&);
   //    bool operator<(const EngAlmanac&);
   //    bool operator>(const EngAlmanac&);
   };

   std::ostream& operator<<(std::ostream& s, const EngAlmanac& alm);

}
#endif


%{
typedef gpstk::EngAlmanac::SVNotPresentException SVNotPresentException;
%}