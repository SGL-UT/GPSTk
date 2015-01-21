#pragma ident "$Id: //depot/msn/main/code/shared/gpstk/SinexData.hpp#3 $"

/**
 * @file SinexData.hpp
 * Encapsulate SINEX file data, including I/O
 */

#ifndef GPSTK_SINEXDATA_HPP
#define GPSTK_SINEXDATA_HPP

//lgpl-license START
//lgpl-license END

//dod-release-statement START
//dod-release-statement END

#include <vector>
#include <map>
#include "SinexBase.hpp"
#include "SinexHeader.hpp"
#include "SinexBlock.hpp"

namespace gpstk
{
namespace Sinex
{
      /**
       * This class encapsulates all data in a SINEX file.  The data
       * in this class is organized into a list of Sinex::Block objects
       * which contain individual lines of SINEX data.  This class
       * derives from FFData and thus can be read from or written to
       * an FFStream.
       */
   class Data : public FFData
   {
   public:

         /// Constructor.
      Data() { initBlockFactory(); };

         /// Destructor
      virtual ~Data();

         // The next four lines is our common interface
         /// Data is "data" so this function always returns true.
      virtual bool isData() const { return true; };

         /// Debug output function.
      virtual void dump(std::ostream& s) const;

         /// Verifies that the specified block title is valid.
      static bool isValidBlockTitle(const std::string& block);

         /// Header
      Header  header;

         /// Block storage
      Blocks  blocks;

   protected:

         /// Mappings from block titles to create functions
      static BlockFactory   blockFactory;

         /**
          * Initializes the block factory with mappings from block titles
          * to create functions.
          */
      static void  initBlockFactory();

         /**
          * Writes the formatted record to the FFStream.
          */
      void reallyPutRecord(FFStream& s) const
         throw(std::exception, FFStreamError, StringUtils::StringException);

         /**
          * This function reads a record from the given FFStream.
          * If an error is encountered in retrieving the record, the
          * stream is reset to its original position and its fail-bit is set.
          * @throws StringException when a StringUtils function fails
          * @throws FFStreamError when exceptions(failbit) is set and
          *  a read or formatting error occurs.  This also resets the
          *  stream to its pre-read position.
          */
      void reallyGetRecord(FFStream& s)
         throw(std::exception, FFStreamError, StringUtils::StringException);

   }; // class Data

}  // namespace Sinex

}  // namespace gpstk

#endif // GPSTK_SINEXDATA_HPP
