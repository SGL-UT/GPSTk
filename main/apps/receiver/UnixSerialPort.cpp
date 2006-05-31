#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/receiver/UnixSerialPort.cpp#1 $"

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
//===========================================================================

#include <Exception.hpp>

#include <unistd.h>

#include "UnixSerialPort.hpp"

namespace gpstk 
{
   
   /**
    * Constructors/Destructors
    */

   UnixSerialPort::UnixSerialPort(const char* deviceName, int flags)
      throw(UnixSerialPort::AccessException)
   {
     fd = open(deviceName, O_RDWR | O_NOCTTY | O_NDELAY);

      if (fd<0)
      {
         AccessException ae("Unable to open given port. Do you have access to it?");
         GPSTK_THROW(ae);
      }

      int rtn = fcntl(fd, F_SETFL, 0);

      struct termios options;  


      options.c_iflag = 0x00 | IGNBRK;// | IGNPAR; // 0x1;
      options.c_lflag = 0x00;
      options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
      options.c_oflag = 0x00;
      options.c_cflag =  0x00 | CS8 | CSIZE | CREAD | HUPCL | CLOCAL; // 0x1cb2;
      cfsetispeed(&options, B115200);

         // set timeout to 0.2 sec
         //options.c_cc[VMIN]=0;
         // options.c_cc[VTIME]=2;

      // Final step... apply them
      tcsetattr(fd, TCSANOW, &options);

   }

   UnixSerialPort::~UnixSerialPort(void)
   {
      close(fd);
   }

   int UnixSerialPort::write(char* obuff, int length)
   {
      return ::write(fd, obuff, length);
   }

   int UnixSerialPort::read(char* obuff, int length)
   {
      return ::read(fd, obuff, length);
   }

} //namespace gpstk
