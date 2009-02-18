#pragma ident "$Id$"

/**
 * @file BinexStream.hpp
 * File stream for RINEX meteorological files
 */

#ifndef GPSTK_BINEXSTREAM_HPP
#define GPSTK_BINEXSTREAM_HPP

#include "FFBinaryStream.hpp"

namespace gpstk
{
   /** @addtogroup Binex */
   //@{

      /**
       * This class performs file i/o on a BINEX file for the 
       * BinexData classes.
       *
       * @sa binex_read_write.cpp for an example.
       * @sa binex_test.cpp for an example.
       * @sa BinexData.
       *
       */
   class BinexStream : public FFBinaryStream
   {
   public:
         /// Destructor
      virtual ~BinexStream() {}
      
         /// Default constructor
      BinexStream() {}
      
         /** Constructor 
          * Opens a file named \a fn using ios::openmode \a mode.
          */
      BinexStream(const char* fn,
                  std::ios::openmode mode=std::ios::in | std::ios::binary)
            : FFBinaryStream(fn, mode) {};

         /// Opens a file named \a fn using ios::openmode \a mode.
      virtual void
      open(const char* fn, std::ios::openmode mode)
      { 
         FFBinaryStream::open(fn, mode); 
      }
   };

   //@}

} // namespace gpstk

#endif // GPSTK_BINEXSTREAM_HPP
