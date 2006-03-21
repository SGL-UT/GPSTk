#!/usr/local/bin/perl
#   
# preprocess.pl  Preprocess Rinex data for SatBiasIonoModel
#                Use ResCor to compute EL AZ LA LO SR,
#                add station id to header as MARKER NAME,
#                put station position in header,
#                and decimate to 30 seconds.
#                Assumes ephemeris file is in same directory,
#                and has a name of the same form (with "brdc"),
#                as data files: <dir>/zwenXXXX.NNo and <dir>/brdcXXXX.NNn
#
use strict;
use warnings;
use File::Basename;

my ($stationID,$filename,$slash,$prgmdir,$option,$ephfile,$rawfile,$obsfile,$sumfile);
my ($x,$y,$z)=(0,0,0);
my @opt=();

if($#ARGV < 0) {
   print "Usage: perl preprocess.pl <filename>\n";
   print "   where <filename> is the complete file name,\n";
   print "   and <filename> is found in dir ./alldata.\n";
   print "   Output (.obs and .sum) are placed in ./processed.\n";
   exit;
}
$filename = shift @ARGV;
$stationID = substr($filename,length($filename)-12,4);

if ($^O eq "linux") {
   $slash = "/";
   # GPSTK prgms found here
   $prgmdir = "/home/btolman/gpstk/dev/apps/Rinextools";
}
if ($^O eq "MSWin32") {
   $slash = "\\";
   # GPSTK prgms found here
   $prgmdir = "C:\\Code\\GPSLIB\\Working\\dev\\apps\\Rinextools";
}

print "Process station $stationID in file $filename\n";

$rawfile = "alldata$slash$filename";
$obsfile = "processed$slash$stationID.obs";
$sumfile = "processed$slash$stationID.sum";

# make up ResCor options
#decimate to 30 second data, fill output header
push @opt, "-IF$rawfile -OF$obsfile";
push @opt, "-TN30.0 -HDf -HDm$stationID --Callow --IonoHt 350";
push @opt, "-DOL1 -DOL2";
#push @opt, "-DOC1 -DOP1 -DOP2";
push @opt, "-DOD1 -DOD2 -DOS1 -DOS2";
push @opt, "-AOEL -AOAZ -AOLA -AOLO -AOSR";

# Run RinSum and gather information for ResCor options.
$option = $prgmdir . $slash . "RinSum -i$rawfile -o$sumfile -g";
#print "Run $option\n";
`$option`;

open FILE, "$sumfile" or die "Couldn't open RinSum output $sumfile\n";
while(<FILE>) {
   chomp;
   if(/could not be opened./) {
      print "Data file $rawfile could not be opened - abort.\n";
      exit;
   }
   if(/Position \(XYZ,m\)/) {
      $_ =~ s/Position \(XYZ,m\) : \(([0-9\.\-]+), ([0-9\.\-]+), ([0-9\.\-]+)\)\./$1 $2 $3/;
      #print "Found position $1 $2 $3\n";
      $x = $1;
      $y = $2;
      $z = $3;
      if($x != 0) {
         push @opt, "--RxXYZ " . sprintf("%.4f",$x) . "," . sprintf("%.4f",$y) . "," . sprintf("%.4f",$z);
      }
   }
   if(/WARNING: ObsType P1 should be deleted from header./) {
      $option = "--Cforce";
      push @opt, $option;
      last;
   }
}
close(FILE);

# if position was not found in the header, compute and add it
if($x == 0) {
   push @opt, "--RAIM --RAIMhead";
}

# add ephemeris file
$ephfile = $rawfile;
$ephfile =~ s/$stationID/brdc/;
$ephfile =~ s/\.([90][0-9])o/.$1n/;
push @opt, "--nav $ephfile";

$option = $prgmdir . $slash . "ResCor @opt";
print "Run $option\n";
`$option`;

#re-run RinSum
$option = $prgmdir . $slash . "RinSum -i$obsfile -o$sumfile -g";
print "Run $option\n";
`$option`;
