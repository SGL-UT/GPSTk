#!/usr/bin/perl
use File::stat;
use strict;
use warnings;

if($#ARGV < 1) {
   print "Usage: perl plotall.pl <base> <n1> <n2>\n";
   print "   Plot surface data with contour for all files <base>.N\n";
   print "   where n1 <= N <= n2\n";
   #TD add limits, view, etc
   exit;
}

my $pngout='no';   # set to something other than yes for no .png output

my ($base, $n, $n1, $n2, $dn, $slash, $file);
if($#ARGV > -1) { $base = shift @ARGV; }
if($#ARGV > -1) { $n1 = shift @ARGV; }
$n2 ='';
if($#ARGV > -1) { $n2 = shift @ARGV; }
if($n2 eq '') { $n2=$n1; }
$dn = '';
if($#ARGV > -1) { $dn = shift @ARGV; }
if($dn eq '') { $dn = 1; }

print "Plot all files $base.n for $n1 <= n <= $n2 in steps $dn\n";

open(FPOUT, ">$base.gp") or die "Cant open $base.gp file\n";
print FPOUT "set title \"Ionospheric Vertical TEC\"\n";
print FPOUT "set xlabel \"Latitude\"\n";
print FPOUT "set ylabel \"Longitude\"\n";
print FPOUT "set zlabel \"TEC Units\"\n";
print FPOUT "set xrange [40:60]\n";
print FPOUT "set yrange [250:290]\n";
print FPOUT "set zrange [0:12]\n";
print FPOUT "set cbrange [0:12]\n";
print FPOUT "set pm3d\n";
print FPOUT "unset key\n";
print FPOUT "set data style lines\n";
print FPOUT "set contour base\n";
print FPOUT "set cntrparam levels incremental 0.0,1.5,12.0\n";
print FPOUT "# comment out next two to see only contours\n";
print FPOUT "set hidden3d\n";
print FPOUT "set view 60,45\n";
print FPOUT "# un-comment out next two to see only contours\n";
print FPOUT "#set nosurface\n";
print FPOUT "#set view 0,0\n";

my $N=int(($n2-$n1)/$dn);
foreach my $i (0..$N) {
   my $I=sprintf("%04d",$n1+$i*$dn);
   $file = "$base.$I";
   if(not open(FPTEST, "$base.$I")) {
      print "Could not open $base.$I skip.\n";
      next;
   }
   close(FPTEST);
   # determine time and put up title
   my $min = ($n1+$i*$dn)*0.5;       # 30 second = 0.5 minute data
   my $hr = int($min/60);
   $min = $min - $hr * 60;
   $hr = $hr - 6;                    # local time at 270deg E longitude
   if($hr < 0) { $hr += 24; }
   my $msg = sprintf("LOCAL TIME %02d:%04.1f",$hr,$min);
   if($i > 0) { print FPOUT "unset label 1\n"; }
   print FPOUT "set label 1 \"July 28, 2004 - $msg\" at screen 0.5,0.88 center\n";
   if($pngout eq 'yes') {
      print FPOUT "set terminal png; set output \"$base$I.png\"\n";
   }
   print FPOUT "splot \"$base.$I\" using 1:2:3\n";
   #if($^O eq "MSWin32" && $pngout ne 'yes') {
   if($pngout ne 'yes') {
      print FPOUT "pause -1 \"Surface $I\"\n";
   }
};

close FPOUT;

if ($^O eq "linux") {
   #`/usr/local/bin/gnuplot -persist contour$N.gp`;
   open(GP, "gnuplot -persist $base.gp |");
}
if ($^O eq "MSWin32") {
   #open(GP, "C:\\bat\\gp $base.gp |");
   open(GP, "c:\\gnuplot\\bin\\wgnuplot.exe $base.gp |");
}
close(GP);
