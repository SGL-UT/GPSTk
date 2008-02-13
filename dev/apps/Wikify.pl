#!/usr/bin/perl
#Script by Eric Hagen
#Last saved Jan 10, 2007
#This script is designed to automate capture and Twiki markup of GPSTk application output.

use strict;
use warnings;



#SysCall has the following parameters - GPSTk App to call,
# - parameters for the first example, - parameters for the second example,
# etc.

SysCall("calgps","-3","-Y 1998");
SysCall("ephdiff","-f /home/ehagen/svnclean/gpstk/ref/usersguide/fic06.187 -r /home/ehagen/svnclean/gpstk/ref/usersguide/arl2800.06n");


sub SysCall{
   #Set the application call we are going to make
   my $call = $_[0];
   #Set the file - in the form of AppGPSTk + the application .txt
   my $wikiOutputFile = "AppGPSTk" . $call . ".txt";
   open RESULTS,"> $wikiOutputFile" or die "can't open: $!";
   #Grab the help and put it in a temp file
   system("$call --help > temp.log 2>&1");
   #Print the Wiki enviornmental variables to the file
   print RESULTS "\%META:TOPICINFO{author=\"EricHagen\" date=\"";
   my $time = time();
   print RESULTS "$time\" format=\"1.1\" reprev=\"1.1\" version=\"1.1\"}%\n";
   print RESULTS "This topic is auto generated nightly from the checked in application code in subversion.\n";
   print RESULTS "---\n";
   print RESULTS "\%STARTINCLUDE\%\n";
   print RESULTS "---++ Usage\n";
   #Parse the Help file
   ParseHelp();
   print RESULTS "---++ Examples \n";
   print RESULTS "<verbatim>\n";
   #Grab the arguments for the examples one at a time until all are done.
   for(my $i=1; $i < (@_); $i++) { 
      my $argument = $_[$i];
      print RESULTS ">$call $argument\n";
      system("$call $argument > temp.log");
      #Parse the example
      ParseExample();
   }
   #Finish off writing the file.
   print RESULTS "</verbatim>\n";
   print RESULTS "\%STOPINCLUDE\%\n";
   print RESULTS "   * SET ALLOWTOPICMODIFY = Main.TWikiAdminGroup, Main.EricHagen\n";
   my $date = localtime time();
   print RESULTS "-- Main.Eric Hagen $date";
}

#The subroutine for parsing the GPSTk application help and moving to Twiki markup
sub ParseHelp {
   open TEMP, "< temp.log" or die "can't open datafile: $!";
   while ( <TEMP> ) {
      if ( $_ =~ m/^Required arguments:/) {
         print RESULTS $_;
	 print RESULTS "| Short Arg. | Long Arg. | Description |\n";
      }
      elsif ( $_ =~ m/^Optional arguments:/) {
	 print RESULTS $_;
	 print RESULTS "| Short Arg. | Long Arg. | Description |\n";
      }
      elsif ($_ =~ m/^  -(.*),[ ]*--(.[^ ]*)[ ]*(.*)/) {
         print RESULTS "| -$1 | --$2 | $3 |\n";
      }
   }
}

#The subroutine for parsing the GPSTk application example and moving to Twiki markup
sub ParseExample {
   open TEMP, "< temp.log" or die "can't open datafile: $!";
   my $count = 0;
   while ( <TEMP> ) {
      $count++;
      print RESULTS $_;
      if ($count > 14) {
         print RESULTS ". . .\n\n\n";
	 last;
      } 
   }
}

