#!/usr/bin/tclsh

load ../gpstk-tcl.so

DayTime time

puts "Hello world"
puts "  The current GPS week is [time GPSfullweek]"
puts "  The day of the GPS week is [time GPSday]"
puts "  The seconds of the GPS week is [time GPSsecond]"
