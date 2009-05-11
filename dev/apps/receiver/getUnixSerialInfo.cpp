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

#include <iostream>
#include <iomanip>

#include <fcntl.h>   /* File control definitions */
#include <termios.h>
#include <unistd.h>

using namespace std;

int main(void)
{
   struct termios options;  

   int fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);
   tcgetattr(fd, &options);

   cout << "c_iflag " << hex << options.c_iflag << endl;
   cout << "c_oflag " << hex << options.c_oflag << endl;
   cout << "c_cflag " << hex << options.c_cflag << endl;
   cout << "c_cc[VMIN] " << options.c_cc[VMIN] << endl;
   cout << "c_cc[VTIME] " << options.c_cc[VTIME] << endl;

   return 0;
}
