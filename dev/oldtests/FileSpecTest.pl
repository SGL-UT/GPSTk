#!/usr/bin/perl
# $Id: //depot/sgl/gpstk/dev/tests/FileSpecTest.pl#1 $
#
#
#
#
#
#
#
#

$exedir=$ARGV[0];
system "$exedir/genFileSpecTestDirs";
system "$exedir/FileSpecTest";
system "$exedir/delFileSpecTestDirs";
