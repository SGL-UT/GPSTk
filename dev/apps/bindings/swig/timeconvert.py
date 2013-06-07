#============================================================================
#
#  This file is part of GPSTk, the GPS Toolkit.
#
#  The GPSTk is free software; you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published
#  by the Free Software Foundation; either version 2.1 of the License, or
#  any later version.
#
#  The GPSTk is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with GPSTk; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
#
#  Copyright 2009, The University of Texas at Austin
#
#============================================================================

#============================================================================
#
#This software developed by Applied Research Laboratories at the University of
#Texas at Austin, under contract to an agency or agencies within the U.S. 
#Department of Defense. The U.S. Government retains all rights to use,
#duplicate, distribute, disclose, or release this software. 
#
#Pursuant to DoD Directive 523024 
#
# DISTRIBUTION STATEMENT A: This software has been approved for public 
#                           release, distribution is unlimited.
#
#=============================================================================

import argparse
from gpstk_time import *

parser = argparse.ArgumentParser(description='Converts from a given input time specification'
                      			  ' to other time formats.  Include the quotation marks.'
                        		 ' All year values are four digit years.')


parser.add_argument('-A', '--ansi',     help='\"ANSI-Second\"')
parser.add_argument('-c', '--civil',    help='\"Month(numeric) DayOfMonth Year Hour:Minute:Second\"')
parser.add_argument('-R', '--rinex',    help='\"Year(2-digit) Month(numeric) DayOfMonth Hour Minute Second\"')
parser.add_argument('-o', '--ews',      help='\"GPSEpoch 10bitGPSweek SecondOfWeek\"')
parser.add_argument('-f', '--ws',       help='\"FullGPSWeek SecondOfWeek\"')
parser.add_argument('-w', '--wz',       help='\"FullGPSWeek Zcount\"')
parser.add_argument(      '--z29',      help='\"29bitZcount\"')
parser.add_argument('-Z', '--z32',      help='\"32bitZcount\"')
parser.add_argument('-j', '--julian',   help='\"JulianDate\"')
parser.add_argument('-m', '--mjd',      help='\"ModifiedJulianDate\"')
parser.add_argument('-u', '--unixtime', help='\"UnixSeconds UnixMicroseconds\"')
parser.add_argument('-y', '--doy',      help='\"Year DayOfYear SecondsOfDay\"')

# unimplemented args:
# parser.add_argument('-F', '--input_format', help='')
# parser.add_argument('-R', '--input_time', help='')
# parser.add_argument('-R', '--rinex', help='')

parser.add_argument('-a', '--add_offset', help='add NUM seconds to specified time')
parser.add_argument('-s', '--sub_offset', help='subtract NUM seconds to specified time')
args = parser.parse_args()


ct = SystemTime()
# ct.setTimeSystem() # some enum GPS value goes here?

# we SHOULDN'T need this mess, the scanning methods should take care of parsing the type for us
if args.ansi:
	print 'ansi input'
elif args.civil:
	print 'civil input'
elif args.rinex:
	print 'rinex input'
elif args.ews:
	pass
elif args.ws:
	pass
elif args.wz:
	pass
elif args.z29:
	pass
elif args.z32:
	pass
elif args.julian:
	pass
elif args.mjd:
	pass
elif args.unixtime:
	pass
elif args.doy:
	pass
else:
	pass

	"01 1379 500"


spacing = '        '
def left_align(str):
	return spacing + str.ljust(32, ' ')


wz = GPSWeekZcount(ct)
civ = CivilTime(ct)

print '' # newline
print left_align('Month/Day/Year H:M:S'),        CivilTime(ct).asString()
print left_align('ModifiedJulianDate'),          MJD(ct).asString()
print left_align('GPSweek DayOfWeek SecOfWeek'), GPSWeekSecond(ct).printf('%G %w % 13.6g')
print left_align('FullGPSweek Zcount'),          wz.printf('%F % 6z')
print left_align('Year DayOfYear SecondOfDay'),  YDSTime(ct).printf('%Y %03j % 12.6s')
print left_align('Unix: Second Microsecond'),    UnixTime(ct).printf('%U % 6u') 
print left_align('Zcount: 29-bit (32-bit)'),     wz.printf('%c (%C)')
print '' # newline

