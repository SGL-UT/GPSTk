#pragma ident "$Id$"

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//============================================================================

#include <sstream>
#include <errno.h>
#include <stdlib.h>

#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/file.h>

#include "FDStreamBuff.hpp"

#define BSIZE 4096

using namespace std;

namespace gpstk
{
   // Close the file descriptor
   void FDStreamBuff::close(void)
   {
      if (is_open())
      {
         ::close(handle);
         handle = -1;
      }
   }

   // Write characters to the stream, giving time. Return the number of
   // characters actually written (which is always n, or EOF in case of error).
   int FDStreamBuff::write(const char * buffer, const int n)
   {
      if( !is_open() )
         return EOF;
      if( n == 0 )
         return 0;
  
      const char * const buffer_end = buffer + n;
      while( buffer < buffer_end )
      {
         const int char_written = ::write(handle, buffer, buffer_end-buffer);
         if( char_written > 0 )
            buffer += char_written;
         else
            if (char_written < 0 && !(errno == EAGAIN || errno == EINTR ))
               return EOF;
            else
               cout << "to be implimented" << endl;
      }
      return n;
   }

   // Read characters from the stream into a given buffer (of given size n)
   // If there is nothing to read, yield and keep trying. Return the number
   // of characters actually read, or 0 (in the case of EOF) or EOF on error.
   int FDStreamBuff::read(char * buffer, const int n)
   {
      if( !is_open() )
         return EOF;
      if( n == 0 )
         return 0;
   
      for(;;)
      {
         const int char_read = ::read(handle, buffer, n);
         if( char_read >= 0 )
            return char_read;
         if( errno == EAGAIN || errno == EINTR )
            cout << "to be implimented" << endl;
         else
            return EOF;
      }
   }


   // See libg++-2.7.2/libio/iostream.info-1 for some nice words on how this
   // stuff should work.
   // Flush (write out) the put area, resetting pptr if the write was successful
   // Return 0, or EOF on error
   int FDStreamBuff::sync(void)
   {
      const int n = pptr() - pbase();
      if( n == 0 )
         return 0;
      return write(pbase(), n) == n ? (pbump(-n), 0) : EOF;
   }


   // Write out the buffer into the communication channel After that, put a
   // character c (unless it's EOF) Return 0, or EOF on error. This method
   // allocates a buffer if there wasn't any, and switches it to the "put mode"
   // (discarding all data that may have been in the buffer)
   int FDStreamBuff::overflow(int ch)
   {
      if (sync() == EOF)
         return EOF;

      if (base() == 0)			// If there wasn't any buffer,
         doallocate();			// ... make one
   
      setg(base(),base(),base());		// Make the get area completely empty
      setp(base(),ebuf());			// Give all the buffer to the put area

      if (pptr() == 0)
         cout << "Throw an exception here or something bad." << endl;

      if (ch != EOF)
         *pptr() = ch, pbump(1);

      return 0;
   }


   // Fill in the get area, and return its first character. Actually read into
   // the main buffer from the beginning (syncing pending output if was any)
   // and set the get area to that part of the buffer. Also set pptr() = eptr()
   // so that the first write would call overflow() (which would discard the
   // read data)
   int FDStreamBuff::underflow(void)
   {
      if( gptr() < egptr() )
         return *(unsigned char*)gptr();

      if( sync() == EOF )	// commit all pending output first
         return EOF;	// libg++ uses switch_to_get_mode() in here

      if( base() == 0 )	// If there wasn't any buffer, make one first
         doallocate();
  
      //assert( base() );
      base();
      const int count = read(base(),ebuf() - base());
      setg(base(),base(),base() + (count <= 0 ? 0 : count));
      setp(base(),base());		// no put area - do overflow on the first put
      return count <= 0 ? EOF : *(unsigned char*)gptr();
   }


   // Allocate a new buffer
   int FDStreamBuff::doallocate(void)
   {
      const int size = BSIZE;
      // have to do malloc() as ~streambuf() does free() on the buffer
      char *p = (char *)malloc(size);

      if (p == NULL)
         cout << "throw an exception here?" << endl;

      setb(p, p+size, true);
      return 1;
   }


   // Associate a user buffer with the FDStreamBuff
   FDStreamBuff* FDStreamBuff::setbuf(char* p, const int len)
   {
      if( streambuf::setbuf(p,len) == 0 )
         return 0;

      setp(base(),base());
      setg(base(),base(),base());
      return this;
   }


   void FDStreamBuff::dump(std::ostream& out) const
   {
      std::ostringstream ost;
      ost << "FDStreamBuff: " 
          << " H:" << handle << std::endl
          << " put: " << std::hex << pbase() << " - " << epptr()
          << " curr:" << pptr() - pbase() << std::endl
          << " get:"  << eback() << " - " << egptr()
          << " curr:" << gptr() - eback() << std::endl
          << " buff:" << base() << " - " << ebuf()
          << std::endl;
      out << ost.str();
   }

} // end of namespace
