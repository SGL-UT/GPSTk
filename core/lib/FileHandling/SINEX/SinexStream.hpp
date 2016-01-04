/**
 * @file SinexStream.hpp
 * File stream for Sinex format files
 */

#ifndef SINEXSTREAM_HPP
#define SINEXSTREAM_HPP

//lgpl-license START
//lgpl-license END

//dod-release-statement START
//dod-release-statement END

#include "FFTextStream.hpp"

namespace gpstk
{
   namespace Sinex
   {
         /**
          * This class provides access to Sinex files.
          *
          * @sa gpstk::SinexHeader and gpstk::SinexData for more information.
          * @sa sinex_test.cpp for an example.
          */
      class Stream : public FFTextStream
      {
      public:
         Stream()
         {}

            /** Constructor
             * Opens file \a fn using ios::openmode \a mode.
             */
         Stream(const char* fn, std::ios::openmode mode=std::ios::in)
               : FFTextStream(fn, mode)
         {}

            /**
             * Destructor
             */
         virtual ~Stream()
         {}
      };

   }  // namespace Sinex

}  // namespace gpstk

#endif  // GPSTK_SINEXSTREAM_HPP
