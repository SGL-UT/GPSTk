#!/usr/bin/env python

import argparse
import gpstk
import sys


def main(args=sys.argv[1:]):
    program_description = ('Converts from a given input time specification to '
                           'other time formats. Include the quotation marks. '
                           'All year values are four digit years.')
    parser = argparse.ArgumentParser(description=program_description)

    help_messages = {
        'ansi'  : '\"ANSI-Second\"',
        'civil' : '\"Month(numeric) DayOfMonth Year Hour:Minute:Second\"',
        'rinex' : '\"Year(2-digit) Month(numeric) DayOfMonth Hour Minute Second\"',
        'ews'           : '\"GPSEpoch 10bitGPSweek SecondOfWeek\"',
        'ws'            : '\"FullGPSWeek SecondOfWeek\"',
        'wz'            : '\"FullGPSWeek Zcount\"',
        'z29'           : '\"29bitZcount\"',
        'z32'           : '\"32bitZcount\"',
        'julian'        : '\"JulianDate\"',
        'mjd'           : '\"ModifiedJulianDate\"',
        'unixtime'      : '\"UnixSeconds UnixMicroseconds\"',
        'doy'           : '\"Year DayOfYear SecondsOfDay\"',
        'output_format' : 'Time format to use on output',
        'add_offset'    : 'add NUM seconds to specified time',
        'sub_offset'    : 'subtract NUM seconds to specified time'
    }

    group = parser.add_mutually_exclusive_group()
    group.add_argument('-A', '--ansi',     help=help_messages['ansi'])
    group.add_argument('-c', '--civil',    help=help_messages['civil'])
    group.add_argument('-R', '--rinex',    help=help_messages['rinex'])
    group.add_argument('-o', '--ews',      help=help_messages['ews'])
    group.add_argument('-f', '--ws',       help=help_messages['ws'])
    group.add_argument('-w', '--wz',       help=help_messages['wz'])
    group.add_argument(      '--z29',      help=help_messages['z29'])
    group.add_argument('-Z', '--z32',      help=help_messages['z32'])
    group.add_argument('-j', '--julian',   help=help_messages['julian'])
    group.add_argument('-m', '--mjd',      help=help_messages['mjd'])
    group.add_argument('-u', '--unixtime', help=help_messages['unixtime'])
    group.add_argument('-y', '--doy',      help=help_messages['doy'])

    parser.add_argument('-F', '--output_format', help=help_messages['output_format'])

    parser.add_argument('-a', '--add_offset', help=help_messages['add_offset'],
                        type=int, nargs='+')
    parser.add_argument('-s', '--sub_offset', help=help_messages['sub_offset'],
                        type=int, nargs='+')
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
            time_found = True
            try:
                ct = gpstk.scanTime(input_time, formats[key])
            except gpstk.exceptions.InvalidRequest:
                print ('Input could not be parsed. Check the formatting and '
                       'ensure that the input is both valid and in quotes.')
                sys.exit()

    if not time_found:
        ct = gpstk.SystemTime()

    timeSys = gpstk.TimeSystem(gpstk.TimeSystem.GPS)
    ct.setTimeSystem(timeSys)

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
