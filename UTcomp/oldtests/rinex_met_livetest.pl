#!/usr/bin/perl
#
# Live Rinex Test
# 
# >>rinex_met_live <development directory> <met, obs, or nav (m,o,n)> <download directory>
#
#




$dir=$ARGV[0];
$dir =~ s/^-//;
$devdir=$dir."/gpstk/dev/tests";

$L=$ARGV[1];
$L =~ s/^-//;

$dldir=$ARGV[2];
$dldir =~ s/^-//;
if((-e $dldir)==o)
{
   $dldir=$devdir;
}

#print "PRE DL\n";

system "$devdir/Rinex_dl.pl $L $dldir";
print "POST DL\n";

print "Current Directory: ";
system "pwd";

chdir "$dldir/NewRinexData-$L";

print "New Directory: ";
system "pwd";

mkdir "Rinex_out-$L";
$newdir=$dldir."/Rinex_out-".$L;

#print "\n!!$newdir!!\n";

@all_files= glob "*";

#print "!!@all_files\n!!";
#print "BEFORE\n";

chdir "$newdir";

foreach(@all_files)
{
  # print "INSIDE\n";
   $out=$_.".out";
  # print "$out\n";
   system "$devdir/rinex_met_test $_>$out";
   print "Ran $_ Test\n";
}

#print "AFTER\n";
