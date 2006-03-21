#!/usr/bin/perl
# $Id: //depot/sgl/gpstk/dev/tests/Rinex_dl.pl#9 $

#                                         Ryan Mire ARL SGL 
#                                           July 2005
# last saved: July 27 10:00am
#
# The purpose of this script is to have as close to a live test as possible of the rinex_met read and write functions
# the live quality of this test will be created through the downloading of all the IGS gathered and posted data from 
# "yesterday"
#
# >>Rinex_dl.pl <first letter of met obs or nav> [<download directory>]
#
#

use Net::FTP;
#use strict;
#use warnings;

$filetype=$ARGV[0];
$filetype =~ s/^-//;
chomp($filetype);
#print "!".$filetype."!\n";
#chomp($filetype);
die "\nThe Given File Type is invalid, obs, met and nav are the three filetypes accepted. Try using lowercase 'o','m' or 'n' respectively. \n >>Rinex.dl <firstletter of filetype> [<download directory>]\n"
  if(($filetype ne 'n') && ($filetype ne 'o') && ($filetype ne 'm'));


$filedir="05".$filetype;

if($ARGV[1])
{
$downdir=$ARGV[1];
die "\nThe given download directory does not exist or is invalid, try again\n"
    if((-e $downdir)==0);
}

$date = `date`;
chomp($day = `date +"%j"`);
$day-=1;
print "HELLO \t $day\n";
@date=split /\s/, $date;

chomp($year=@date[5]);


my $host="cddis.gsfc.nasa.gov";
my $directory="/gps/data/daily/$year/$day/$filedir";

$ftp=Net::FTP->new($host,Timeout=>240) or $newerr=1;
  push @ERRORS, "Can't ftp to $host: $!\n" if $newerr;
  myerr() if $newerr;
print "Connected\n";

$ftp->login("Anonymous","GPSTK@arlut.utexas.edu") or $newerr=1;
print "Getting file list\n";
  push @ERRORS, "Can't login to $host: $!\n" if $newerr;
  $ftp->quit if $newerr;
  myerr() if $newerr; 
print "Logged in\n";

$ftp->cwd($directory) or $newerr=1; 
  push @ERRORS, "Can't cd  $!\n" if $newerr;
  myerr() if $newerr;
  $ftp->quit if $newerr;

@files=$ftp->dir or $newerr=1;
  push @ERRORS, "Can't get file list $!\n" if $newerr;
  myerr() if $newerr;
print "Got  file list\n";   

$n=0;

if($ARGV[1])
{
   $newdir=$downdir."/NewRinexData-".$filetype;
}
else
{
   $newdir="/NewRinexData-".$filetype;
}
die "Oops!!, $newdir already exist \t  Try Again "
    if((-e $newdir)!=0);

mkdir "$newdir";
chdir "$newdir";


foreach(@files)
{
    @line=split/\s/, @files[$n];
    $file=pop(@line);
    $n+=1;
    $ftp->get("/gps/data/daily/$year/$day/$filedir/$file")  or $newerr=1;
    if($newerr!=1)   
    {
        print "Downloaded $file\n";
    }
    push @ERRORS, "Can't get $_ $!\n" if $newerr; 
}
print " $directory\n ";
$ftp->quit;

print "Youre now in the new subdirectory $newdir,\n filled with your data \n";

sub myerr 
{
  print "Error: \n";
  print @ERRORS;
  exit 0;
}
