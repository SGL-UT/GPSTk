#pragma ident "$Id: //depot/msn/main/code/shared/gpstk/SinexHeader.hpp#3 $"

/**
 * @file SinexHeader.hpp
 * Encapsulate header of SINEX file data, including I/O
 */

#ifndef GPSTK_SINEXHEADER_HPP
#define GPSTK_SINEXHEADER_HPP

//lgpl-license START
//lgpl-license END

//dod-release-statement START
//dod-release-statement END

#include <string>
#include "SinexBase.hpp"

namespace gpstk
{
namespace Sinex
{
      /**
       * This class models the header for a SINEX file.
       *
       * @sa gpstk::SinexStream and gpstk::SinexData for more information.
       */
   class Header
   {
   public:

      static const size_t  MIN_LINE_LEN = 67;
      static const size_t  MAX_LINE_LEN = 79;

         /// Constructors
      Header() : version(VERSION) {};
      Header(const std::string& line) : version(VERSION)
      {
         *this = line;
      };

         /// Destructor
      virtual ~Header() {};

         /// String converters
      operator std::string() const;
      void operator=(const std::string& other);

         /// Debug output operator.
      void dump(std::ostream& s) const;

         ///@name data members
         //@{
      float        version;         ///< Format version of the file ##.##.
      std::string  creationAgency;  ///< Agency creating the file
      Time         creationTime;    ///< Creation time
      std::string  dataAgency;      ///< Agency supplying data for the file
      Time         dataTimeStart;
      Time         dataTimeEnd;
      char         obsCode;
      uint32_t     paramCount;
      char         constraintCode;
      std::string  solutionTypes;   ///< Solution types in this file: S,O,E,T,C,A, or ' '
         //@}

   }; // class Header

}  // namespace Sinex

}  // namespace gpstk

#endif // GPSTK_SINEXHEADER_HPP
