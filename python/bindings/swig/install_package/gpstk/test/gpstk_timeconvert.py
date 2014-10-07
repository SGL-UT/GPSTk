#!/usr/bin/env python

import argparse
import gpstk
import sys

# default args to argv are left in so the script can be run without starting
# a new python process. This is very useful for testing!
def main(args=sys.argv[1:]):
    program_description = ('Converts from a given input time specification to '
                           'other time formats. Include the quotation marks. '
                           'All year values are four digit years. '
                           'Example: $ python gpstk_timeconvert.py -f "158 200" ')
    parser = argparse.ArgumentParser(description=program_description)

    group = parser.add_mutually_exclusive_group()
    group.add_argument('-A', '--ansi', help='\"ANSI-Second\"')
    group.add_argument('-c', '--civil',
                       help='\"Month(numeric) DayOfMonth Year Hour:Minute:Second\"')
    group.add_argument('-R', '--rinex',
                       help='\"Year(2-digit) Month(numeric) DayOfMonth Hour Minute Second\"')
    group.add_argument('-o', '--ews', help='\"GPSEpoch 10bitGPSweek SecondOfWeek\"')
    group.add_argument('-f', '--ws', help='\"FullGPSWeek SecondOfWeek\"')
    group.add_argument('-w', '--wz', help='\"FullGPSWeek Zcount\"')
    group.add_argument(      '--z29', help='\"29bitZcount\"')
    group.add_argument('-Z', '--z32', help='\"32bitZcount\"')
    group.add_argument('-j', '--julian', help='\"JulianDate\"')
    group.add_argument('-m', '--mjd', help='\"ModifiedJulianDate\"')
    group.add_argument('-u', '--unixtime', help='\"UnixSeconds UnixMicroseconds\"')
    group.add_argument('-y', '--doy', help='\"Year DayOfYear SecondsOfDay\"')

    parser.add_argument('-F', '--output_format', help='Time format to use on output')

    parser.add_argument('-a', '--add_offset', type=int, nargs='+',
                        help='add NUM seconds to specified time')
    parser.add_argument('-s', '--sub_offset', type=int, nargs='+',
                        help='subtract NUM seconds to specified time')
    args = parser.parse_args(args)

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
        input_time = getattr(args, key)  # args.ansi, args.civil, etc.
        if input_time is not None:
            try:
                ct = gpstk.scanTime(input_time, formats[key])
                time_found = True
            except gpstk.InvalidRequest:
                raise gpstk.InvalidRequest('Input could not be parsed.'
                     '\nCheck the formatting and ensure that the input is both valid and in quotes.'
                     '\nAlso check if the time is too early/late for these formats.')


    if not time_found:
        ct = gpstk.SystemTime().toCommonTime()

    ct.setTimeSystem(gpstk.TimeSystem('GPS'))

    if args.add_offset is not None:
        for t in args.add_offset:
            ct.addSeconds(float(t))
    if args.sub_offset is not None:
        for t in args.sub_offset:
            ct.addSeconds(-float(t))

    if args.output_format is not None:
        print gpstk.printTime(ct, args.output_format)
    else:
        def left_align(str):
            spacing = ' ' * 8
            return spacing + str.ljust(31)

        print ''  # newline

        print left_align('Month/Day/Year H:M:S'),
        print gpstk.CivilTime(ct)

        print left_align('Modified Julian Date'),
        print gpstk.MJD(ct)

        print left_align('GPSweek DayOfWeek SecOfWeek'),
        print gpstk.GPSWeekSecond(ct).printf('%G %w % 13.6g')

        print left_align('FullGPSweek Zcount'),
        print gpstk.GPSWeekZcount(ct).printf('%F % 6z')

        print left_align('Year DayOfYear SecondOfDay'),
        print gpstk.YDSTime(ct).printf('%Y %03j % 12.6s')

        print left_align('Unix: Second Microsecond'),
        print gpstk.UnixTime(ct).printf('%U % 6u')

        print left_align('Zcount: 29-bit (32-bit)'),
        print gpstk.GPSWeekZcount(ct).printf('%c (%C)')

        print ''  # newline


if __name__ == '__main__':
    main()
