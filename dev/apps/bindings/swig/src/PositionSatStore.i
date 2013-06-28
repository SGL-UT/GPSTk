#ifndef GPSTK_POSITION_SAT_STORE_INCLUDE
#define GPSTK_POSITION_SAT_STORE_INCLUDE

#include <map>
#include <iostream>

#include "TabularSatStore.hpp"
#include "Exception.hpp"
#include "SatID.hpp"
#include "CommonTime.hpp"
#include "Triple.hpp"
#include "SP3Data.hpp"

namespace gpstk
{

   typedef struct PositionStoreDataRecord {
      Triple Pos, sigPos;
      Triple Vel, sigVel;
      Triple Acc, sigAcc;
   } PositionRecord;

   std::ostream& operator<<(std::ostream& os, const PositionRecord& cdr) throw();

/////// This is the only change from the original headaer file:
  %template(TabularSatStore_ClockRecord) gpstk::TabularSatStore<gpstk::PositionRecord>;
   class PositionSatStore : public TabularSatStore<PositionRecord>
   {

   // member data
   protected:

      bool haveAcceleration;
      bool rejectBadPosFlag;
      unsigned int interpOrder;
      unsigned int Nhalf;

   public:

      PositionSatStore() throw() : haveAcceleration(false), rejectBadPosFlag(true),
                                   Nhalf(5)
      {
         interpOrder = 2*Nhalf;
         havePosition = true;
         haveVelocity = false;
         haveClockBias = false;
         haveClockDrift = false;
      }

      ~PositionSatStore() {};

      bool hasAccleration() const throw() { return haveAcceleration; }

      PositionRecord getValue(const SatID& sat, const CommonTime& ttag)
         const throw(InvalidRequest);

      Triple getPosition(const SatID& sat, const CommonTime& ttag)
         const throw(InvalidRequest);

      Triple getVelocity(const SatID& sat, const CommonTime& ttag)
         const throw(InvalidRequest);

      Triple getAcceleration(const SatID& sat, const CommonTime& ttag)
         const throw(InvalidRequest);

      virtual void dump(std::ostream& os = std::cout, int detail = 0) const throw()
      {
         os << "Dump of PositionSatStore(" << detail << "):\n";
         os << " This store " << (haveAcceleration ? "contains":"does not contain")
            << " acceleration data." << std::endl;
         os << " Interpolation is Lagrange, of order " << interpOrder
            << " (" << Nhalf << " points on each side)" << std::endl;
         TabularSatStore<PositionRecord>::dump(os,detail);
         os << "End dump of PositionSatStore.\n";
      }

      void addPositionRecord(const SatID& sat, const CommonTime& ttag,
                             const PositionRecord& rec)
         throw(InvalidRequest);

      void addPositionData(const SatID& sat, const CommonTime& ttag,
                           const Triple& Pos, const Triple& Sig=Triple())
         throw(InvalidRequest);

      void addVelocityData(const SatID& sat, const CommonTime& ttag,
                           const Triple& Vel, const Triple& Sig=Triple())
         throw(InvalidRequest);

      void addAccelerationData(const SatID& sat, const CommonTime& ttag,
                               const Triple& Acc, const Triple& Sig=Triple())
         throw(InvalidRequest);

      unsigned int getInterpolationOrder(void) throw()
         { return interpOrder; }

      void setInterpolationOrder(unsigned int order) throw()
         { Nhalf = (order+1)/2; interpOrder = 2*Nhalf; }

      void rejectBadPositions(const bool flag)
         { rejectBadPosFlag=flag; }

   };

}

#endif // GPSTK_POSITION_SAT_STORE_INCLUDE
