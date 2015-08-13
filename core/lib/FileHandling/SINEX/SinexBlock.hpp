#pragma ident "$Id: //depot/msn/main/code/shared/gpstk/SinexBlock.hpp#5 $"

/**
 * @file SinexBlock.hpp
 * Encapsulate SINEX block data, including I/O
 */

#ifndef GPSTK_SINEXBLOCK_HPP
#define GPSTK_SINEXBLOCK_HPP

//lgpl-license START
//lgpl-license END

//dod-release-statement START
//dod-release-statement END

#include <vector>
#include <map>
#include "SinexBase.hpp"
#include "SinexTypes.hpp"

namespace gpstk
{
namespace Sinex
{
      /** Base class for all SINEX blocks; the common base allows blocks
       * of varying types to be stored in a vector.  Specific block types
       * are realized by the derived Block class defined below.
       */
   class BlockBase
   {
         /// Sinex:Data calls the protected putBlock() and getBlock() methods.
      friend class Data;

   public:

      virtual ~BlockBase() {}

         /// Comments that apply to an entire block
      std::string  comment;

         /**Returns the block title
          */
      virtual std::string  getTitle() const = 0;

         /** Returns the number of lines in the block
          */
      virtual size_t  getSize() const = 0;

   protected:

         /**
          * Writes the formatted block to the SinexStream.
          * @param s Stream to which to write
          * @returns Number of lines written
          */
      virtual size_t putBlock(Sinex::Stream& s) const
         throw(std::exception, FFStreamError, StringUtils::StringException) = 0;

         /**
          * Reads a record from the given SinexStream; if an error
          * is encountered in retrieving the record, the stream is reset
          * to its original position and its fail-bit is set.
          * @param s Stream from which to read
          * @returns Number of lines read
          * @throws StringException when a StringUtils function fails
          * @throws FFStreamError when exceptions(failbit) is set and
          *  a read or formatting error occurs.  This also resets the
          *  stream to its pre-read position.
          */
      virtual size_t getBlock(Sinex::Stream& s)
         throw(std::exception, FFStreamError, StringUtils::StringException) = 0;

   }; // class BlockBase


      /** SINEX block parameterized by the type of data it contains.
       * Each element in the block is a data line of the specified type.
       */
   template<class T>
   class Block : public BlockBase
   {
   public:

      virtual ~Block() {}

      static BlockBase* create() { return new Block<T>; }

      std::string  getTitle() const { return T::BLOCK_TITLE; }

         /** Returns the number of data items in the block
          */
      size_t  getSize() const { return dataVec.size(); }

         /** Appends data to the block
          */
      void push_back(const T& x) { dataVec.push_back(x); }

      std::vector<T>& getData() { return dataVec; }

   protected:

      std::vector<T>  dataVec;  // Data storage

         /** Writes all data in the block to the specified stream
          */
      virtual size_t putBlock(Sinex::Stream& s) const
         throw(std::exception, FFStreamError)
      {
         size_t  lineNum = 0;
         typename std::vector<T>::const_iterator  i = dataVec.begin();
         for ( ; i != dataVec.end(); ++i, ++lineNum)
         {
            try
            {
               const Sinex::DataType&  d = *i;
               s << (std::string)d << std::endl;
            }
            catch (Exception& exc)
            {
               FFStreamError  err(exc);
               GPSTK_THROW(err);
            }
         }
         return lineNum;
      };

         /** Reads all data in a block from the specified stream
          */
      virtual size_t getBlock(Sinex::Stream& s)
         throw(std::exception, FFStreamError, StringUtils::StringException)
      {
         size_t  lineNum = 0;
         char    c;
         while (s.good() )
         {
            c = s.get();
            if (s.good() )
            {
               if (c == DATA_START)
               {
                     /// More data
                  std::string  line;
                  s.formattedGetLine(line);
                  try
                  {
                     dataVec.push_back(
                        T(line.insert( (unsigned)0, (unsigned)1, c),
                          lineNum) );
                  }
                  catch (Exception& exc)
                  {
                     FFStreamError  err(exc);
                     GPSTK_THROW(err);
                  }
                  ++lineNum;
               }
               else
               {
                     /// End of data
                  s.putback(c);
                  break;
               }
            }
         }
         return lineNum;
      };

   }; // class Block<T>


      /// Block storage type
   typedef std::vector<const BlockBase*>  Blocks;

      /// Block iterator
   typedef Blocks::iterator  BlockIter;

      /// Function pointer for invoking create methods for blocks
   typedef BlockBase* (*BlockCreateFunc)();

      /// Mapping from block titles to block create functions
   typedef std::map<std::string, BlockCreateFunc>  BlockFactory;

}  // namespace Sinex

}  // namespace gpstk

#endif // GPSTK_SINEXBLOCK_HPP
