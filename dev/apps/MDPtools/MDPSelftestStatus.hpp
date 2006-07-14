#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/MDPtools/MDPSelftestStatus.hpp#4 $"

#ifndef MDPSELFTESTSTATUS_HPP
#define MDPSELFTESTSTATUS_HPP

#include "MDPHeader.hpp"
#include <Xvt.hpp>

namespace gpstk
{
   /// This class represents a runtime selftest status message
   class MDPSelftestStatus : public MDPHeader
   {
   public:
      MDPSelftestStatus() throw();

      /**  Encode this object to a string.
       * @return this object as a string.
       */
      virtual std::string encode() const
         throw();
         
      /** Decode this object from a string.
       * @param str the string to read from.
       * @note This is a non-destructive decode.
       */
      virtual void decode(std::string str)
         throw();
         
      virtual std::string getName() const {return "sts";};

      /** Dump some debugging information to the given ostream.
       * @param out the ostream to write to
       */
      void dump(std::ostream& out) const throw();

      float antennaTemp;
      float receiverTemp;
      long status;
      float cpuLoad;
      gpstk::DayTime selfTestTime; ///< SelfTest SOW & week
      gpstk::DayTime firstPVTTime; ///< First PVT SOW & week
      short extFreqStatus;
      short saasmStatusWord;

      static const unsigned myLength = 32;
      static const unsigned myId = 400;

   }; // class MDPSelftestStatus
      
} // namespace gpstk

#endif //MDPSELFTESTSTATUS_HPP
