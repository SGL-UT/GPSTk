#ifndef GPSTK_RINEXOBSHEADER_HPP
#define GPSTK_RINEXOBSHEADER_HPP

#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <iomanip>

#include "CommonTime.hpp"
#include "FFStream.hpp"
#include "RinexObsBase.hpp"
#include "Triple.hpp"
#include "RinexSatID.hpp"

namespace gpstk { 

   typedef struct ClockDataRecord {
      double bias, sig_bias;
      double drift, sig_drift
      double accel, sig_accel;

   std::ostream& operator<<(std::ostream& os, const ClockRecord& rec) throw();

///// This is the only change to the file beyond the original header:
  %template(TabularSatStore_ClockRecord) gpstk::TabularSatStore<gpstk::ClockRecord>; 



   class ClockSatStore : public TabularSatStore<ClockRecord>
   {

   protected:
      bool haveClockAccel;
      int interpType;
      unsigned int interpOrder;
      unsigned int Nhalf;
      bool rejectBadClockFlag;

   public:
      ClockSatStore() throw() : haveClockAccel(false),
                                interpType(2), Nhalf(5),
                                rejectBadClockFlag(true)
      {
         interpOrder = 2*Nhalf;
         haveClockBias = true;
         haveClockDrift = havePosition = haveVelocity = false;
      }

      virtual ~ClockSatStore() {};
      bool hasClockAccel() const throw() { return haveClockAccel; }

      virtual ClockRecord getValue(const SatID& sat, const CommonTime& ttag)
         const throw(InvalidRequest);

      double getClockBias(const SatID& sat, const CommonTime& ttag)
         const throw(InvalidRequest);

      double getClockDrift(const SatID& sat, const CommonTime& ttag)
         const throw(InvalidRequest);

      virtual void dump(std::ostream& os = std::cout, int detail = 0) const throw()
      {
         os << "Dump of ClockSatStore(" << detail << "):\n";
         os << " This store " << (haveClockAccel ? "contains":" does not contain")
            << " clock acceleration data." << std::endl;
         os << " Interpolation is ";
         if(interpType == 2) os << "Lagrange, of order " << interpOrder
            << " (" << Nhalf << " points on each side)" << std::endl;
         else                os << "Linear." << std::endl;
         TabularSatStore<ClockRecord>::dump(os,detail);
         os << "End dump of ClockSatStore.\n";
      }

      void addClockRecord(const SatID& sat, const CommonTime& ttag,
                          const ClockRecord& rec)
         throw(InvalidRequest);

      void addClockBias(const SatID& sat, const CommonTime& ttag,
                        const double& bias, const double& sig=0.0)
         throw(InvalidRequest);

      void addClockDrift(const SatID& sat, const CommonTime& ttag,
                        const double& drift, const double& sig=0.0)
         throw(InvalidRequest);

      void addClockAcceleration(const SatID& sat, const CommonTime& ttag,
                        const double& accel, const double& sig=0.0)
         throw(InvalidRequest);

      unsigned int getInterpolationOrder(void) throw()
         { return interpOrder; }

      void setInterpolationOrder(unsigned int order) throw()
      {
         if(interpType == 2) Nhalf = (order+1)/2;
         else                Nhalf = 1;
         interpOrder = 2*Nhalf;
      }

      void rejectBadClocks(const bool flag)
         { rejectBadClockFlag = flag; }

      void setLagrangeInterp(void) throw()
         { interpType = 2; setInterpolationOrder(10); }

      void setLinearInterp(void) throw()
         { interpType = 1; setInterpolationOrder(1); }

   };

};

#endif
