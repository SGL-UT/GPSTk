import argparse, sys
from gpstk import *

parser = argparse.ArgumentParser(description='Converts from a given input time '
								   'specification to other time formats.'  
								   'Include the quotation marks. All year values' 
								   ' are four digit years.')

group = parser.add_mutually_exclusive_group()
group.add_argument('-A', '--ansi',     help='\"ANSI-Second\"')
group.add_argument('-c', '--civil',    help='\"Month(numeric) DayOfMonth Year Hour:Minute:Second\"')
group.add_argument('-R', '--rinex',    help='\"Year(2-digit) Month(numeric) DayOfMonth Hour Minute Second\"')
group.add_argument('-o', '--ews',      help='\"GPSEpoch 10bitGPSweek SecondOfWeek\"')
group.add_argument('-f', '--ws',       help='\"FullGPSWeek SecondOfWeek\"')
group.add_argument('-w', '--wz',       help='\"FullGPSWeek Zcount\"')
group.add_argument(      '--z29',      help='\"29bitZcount\"')
group.add_argument('-Z', '--z32',      help='\"32bitZcount\"')
group.add_argument('-j', '--julian',   help='\"JulianDate\"')
group.add_argument('-m', '--mjd',      help='\"ModifiedJulianDate\"')
group.add_argument('-u', '--unixtime', help='\"UnixSeconds UnixMicroseconds\"')
group.add_argument('-y', '--doy',      help='\"Year DayOfYear SecondsOfDay\"')

parser.add_argument('-F', '--output_format', help='Time format to use on output')

parser.add_argument('-a', '--add_offset', help='add NUM seconds to specified time',      type=int, nargs='+')
parser.add_argument('-s', '--sub_offset', help='subtract NUM seconds to specified time', type=int, nargs='+')
args = parser.parse_args()

# these format keys must match the long arg names
formats = {
	'ansi'     : '%K',
	'civil'    : '%m %d %Y %H:%M:%f', 
	'rinex'    : '%y %m %d %H %M %S',
	'ews'      : '%E %G %g',
	'ws'       : '%F %g',
	'wz'       : '%F %Z',
	'z29'      : '%E %c',
	'z32'      : '%C',
	'julian'   : '%J',
	'mjd'      : '%Q',
	'unixtime' : '%U',
	'doy'      : '%Y %j %s'
}

time_found = False
for key in formats:
	input_time = getattr(args, key) # args.ansi, args.civl, etc.
	if input_time:
		time_found = True
		try:
			ct = getTime(input_time, formats[key])
		except InvalidRequest:
			print 'Input could not be parsed. Check the formatting and '
			'ensure that the input is both valid and in quotes.'
			sys.exit()


if not time_found:
	ct = SystemTime()
 
ct.setTimeSystem(makeTimeSystem('GPS'))

if args.add_offset:
	for t in args.add_offset:
		ct.addSeconds(float(t))		
if args.sub_offset:
	for t in args.sub_offset:
		ct.addSeconds(-float(t))


if args.output_format:
    print printTime(ct, args.output_format)
else:
	spacing = ' ' * 8
	def left_align(str):
		return spacing + str.ljust(31)
	wz = GPSWeekZcount(ct)
	print '' # newline
	print left_align('Month/Day/Year H:M:S'),        CivilTime(ct)
	print left_align('Modified Julian Date'),        MJD(ct)
	print left_align('GPSweek DayOfWeek SecOfWeek'), GPSWeekSecond(ct).printf('%G %w % 13.6g')
	print left_align('FullGPSweek Zcount'),          wz.printf('%F % 6z')
	print left_align('Year DayOfYear SecondOfDay'),  YDSTime(ct).printf('%Y %03j % 12.6s')
	print left_align('Unix: Second Microsecond'),    UnixTime(ct).printf('%U % 6u') 
	print left_align('Zcount: 29-bit (32-bit)'),     wz.printf('%c (%C)')
	print '' # newline
