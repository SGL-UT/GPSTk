#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/receiver/UnixSerialPort.hpp#1 $"

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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================


#ifndef UNIXSERIALPORT_H
#define UNIXSERIALPORT_H

#include <string>
#include <iostream>
#include <fstream>

#include "Exception.hpp"

#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

   /**
    * This class provides a simplified interface to serial ports under
    * UNIX.
    */

namespace gpstk 
{

   class UnixSerialPort {

   public:

      NEW_EXCEPTION_CLASS(AccessException, gpstk::Exception);

      UnixSerialPort(const char* deviceName,
                     int flags=O_RDWR | O_NOCTTY | O_NDELAY) 
         throw(AccessException);

      ~UnixSerialPort(void);

      int write(char *obuff, int length);
      int read(char *ibuff, int length);

   private:

      /// File descriptor for the serial port
      int fd;

   };

} //namespace gpstk

#endif //UNIXSERIALPORT_H
