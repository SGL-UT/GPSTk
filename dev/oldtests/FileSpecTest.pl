#!/usr/bin/perl
# $Id$
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
