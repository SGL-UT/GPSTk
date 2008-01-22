#pragma ident "$Id$"

#ifndef FDSTREAMBUFF_HPP
#define FDSTREAMBUFF_HPP

#include <iostream>

namespace gpstk
{

   // This is just a stream buffer that attaches to a file descriptor.
   // All buffering is done by streambuf. This class only does the filling and
   // flushing.
   class FDStreamBuff : public std::streambuf
   {
   public:

      FDStreamBuff() : handle(-1), bufferSize(4096)
      {}

      FDStreamBuff(int fd) : handle(fd), bufferSize(4096)
      {}

      ~FDStreamBuff()
      {close();}

      // These are the functions that actually read/write from the socket to/from
      // a given buffer. They return the number of bytes transmitted (or
      // EOF on error)
      int write(const char * buffer, const int n);
      int read(char * buffer, const int n);
      
      bool is_open() const { return handle >= 0; }
      void close();
      virtual FDStreamBuff* setbuf(char* p, const int len);
  
      // We limit this stream to be sequential
      std::streampos seekoff(std::streamoff, std::ios::seekdir, int)
      { return EOF; }

      void dump(std::ostream& out) const;

      struct Buffer
      {
         char * start;
         char * end;
         bool do_dispose;
         void dispose()
         { 
            if (!do_dispose) return;
            free(start); start = end = 0; do_dispose = false;
         }
         Buffer(void) : start(0), end(0), do_dispose(false) {}
         ~Buffer(void) { dispose(); }
      };

      Buffer buffer;
      int handle;
      unsigned bufferSize;
  
   protected:
      // Standard streambuf functions we have to
      // provide an implementation for
      virtual int overflow(int c = EOF);	// Write out a "put area"
      virtual int underflow(void);		// Fill in a "get area"
      virtual int sync(void);		// Commit all uncommitted writes
      virtual int doallocate(void);	// Allocate a new buffer

      // we use the same buffer for get and put areas
      char * base(void) const { return buffer.start; }
      char * ebuf(void) const { return buffer.end; }
      void setb(char * buffer_beg, char * buffer_end, bool do_dispose)
      { buffer.start = buffer_beg; buffer.end = buffer_end;
      buffer.do_dispose = do_dispose;}
   };

} // end of namespace
#endif
