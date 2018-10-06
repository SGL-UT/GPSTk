#!/usr/bin/perl

$exedir=$ARGV[0];
system "$exedir/genFileSpecTestDirs";
system "$exedir/FileSpecTest";
system "$exedir/delFileSpecTestDirs";
