#pragma ident "$Id$"


/**
 * @file MDPStream.hpp
 * gpstk::MDPStream - binary MDP file stream container.
 */

//lgpl-license START
//lgpl-license END

//dod-release-statement START
//dod-release-statement END

#ifndef MDPSTREAM_HPP
#define MDPSTREAM_HPP

#include <FFBinaryStream.hpp>
#include "MDPHeader.hpp"

namespace gpstk
{
   /** 
    * This is a stream used to decode data in the MDP format.
    */
   class MDPStream : public FFBinaryStream
   {
   public:
      MDPStream() : streamState(outOfSync)
      {}

         /**
          * @param fn the name of the MDP file to be opened
          * @param mode the ios::openmode to be used on \a fn
          */
      MDPStream(const char* fn,
                std::ios::openmode mode = std::ios::in)
         : FFBinaryStream(fn, mode), 
           streamState(outOfSync)
      {}

      /// destructor per the coding standards
      virtual ~MDPStream()
      {}

      /// overrides open to reset the header
      virtual void open(const char* fn, std::ios::openmode mode = std::ios::in)
      {
         FFBinaryStream::open(fn, mode); 
         streamState = outOfSync;
      }

      /// Used to track what has been retrieved from the stream
      enum {outOfSync, gotHeader, gotBody} streamState;

      /// A copy of the most recent header read
      MDPHeader header;

      /// The raw bytes of the above header
      std::string rawHeader;
   }; // class MDPStream
} // namespace gpstk

#endif
