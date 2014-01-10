#!/usr/bin/perl
#Script by Eric Hagen, Jonathan Vorce
#Last saved Jan 10, 2007
#This script is designed to automate capture and Twiki markup of GPSTk application output.

use strict;
use warnings;



#SysCall has the following parameters - GPSTk App to call,
# - parameters for the first example, - parameters for the second example,
# etc.

#*********** Set local directory variables ***************
#*********************************************************
#location of GPSTk install directory (e.g jam -sPREFIX=############ install)
my $Local_install = "/home/vorce/svn/gpstk/ebony/jam/ebonyInstall/bin/";

#location of subversion ref/ directory to access input files
my $Local_ref = "/home/vorce/svn/gpstk/ebony/jam/ref/usersguide/";

chdir $Local_ref;

#desired location for output
my $Local_out = "/home/vorce/svn/gpstk/ebony/jam/wiki_out/";

#print the whole example as opposed to truncating it at 19 lines
my $truncate_example;

#the following examples should *NOT* be truncated
$truncate_example = 0;   
SysCall("calgps","-3","-Y 1998");
SysCall("ResCor","--REChelp","--ROThelp");
SysCall("RinSum","-i data_set/s081213a.99o --start 1999,08,01,12,00,00");

#the following examples should be truncated
$truncate_example = 1;  
SysCall("ephdiff","-f fic06.187 -r arl2800.06n"); 
SysCall("timeconvert","-R \"85 05 06 13 50 02\"","-o \"01 1379 500\"","-o \"01 1379 500\" -a \"86400\"","-w \"1381 500\" -s \"200\""); 
SysCall("wheresat","-e fic06.187 -p 1 -u \"918129.01 -4346070.45 803.18\" -t 1800");
SysCall("poscvt","--ecef=\"-4346070.69263 4561978.26297 803.498856837\"","-l","--ecef=\"-4346070.69263 4561978.26297 803.498856837\" -F \"%A %L %h\"");
SysCall("PRSolve","-o arl2800.06o -n arl2800.06n","-o arl2800.06o -n arl2800.06n --BeginTime 2006,1,1,00,00,00 --EndTime 2006,1,1,12,00,00");
SysCall("RinexDump","arl2800.06o 3 4 5"); 
SysCall("rinexpvt","-o arl2800.06o -n arl2800.06n","-o arl2800.06o -n arl2800.06n -m arl2800.06m");
SysCall("rinexthin","-f arl2800.06o -s 60 -o arl2800thin.06o");
SysCall("TECMaps");
SysCall("DiscFix"," --dt 1.5 --inputfile arl2800.06o");
SysCall("ficacheck","bronkenfica");
SysCall("ficcheck","fic06.187");



sub SysCall{
   #Set the application call we are going to make
   my $call = $_[0];
   #Set the file - in the form of AppGPSTk + the application .txt
   my $wikiOutputFile = $Local_out . "AppGPSTk" . $call . ".txt";
   open RESULTS,"> $wikiOutputFile" or die "can't open: $!";
   #Grab the help and put it in a temp file
   system($Local_install."./$call --help > " . $Local_out . "temp.log 2>&1");
   #Print the Wiki enviornmental variables to the file
   print RESULTS "\%META:TOPICINFO{author=\"JonathanVorce\" date=\"";
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
      print RESULTS ">$call $argument\n\n";
      system($Local_install."./$call $argument > ". $Local_out ."temp.log");
      #Parse the example
      ParseExample();
   }
   #Finish off writing the file.
   print RESULTS "</verbatim>\n";
   print RESULTS "\%STOPINCLUDE\%\n";
   print RESULTS "   * SET ALLOWTOPICMODIFY = Main.TWikiAdminGroup, Main.JonathanVorce\n";
   my $date = localtime time();
   print RESULTS "-- Main.Jonathan Vorce $date";
}

#Subroutine for parsing the GPSTk application help and moving to Twiki markup
sub ParseHelp {
   open TEMP, "< ". $Local_out ."temp.log" or die "can't open datafile: $!";
   my $test_next = 0;
   while ( <TEMP> ) {
      if ( $_ =~ m/^Required arguments:/) {
         print RESULTS $_;
	 print RESULTS "| Short Arg. | Long Arg. | Description |\n";
	 $test_next = 2;
      }
      elsif ( $_ =~ m/^Optional arguments:/) {
	 print RESULTS $_;
	 print RESULTS "| Short Arg. | Long Arg. | Description |\n";
      }
      #splits line on (-#####) (--#######) and then (anything else)
      elsif ($_ =~ m/^  -(.*),[ ]*--(.[^ ]*)[ ]*(.*)/) {
         print RESULTS "| -$1 | --$2 | $3 |\n";
	 if ($test_next == 2){
	     $test_next = 0;
	 }
	 else{
	     $test_next = 1;
	 }

      }
      #splits line on --####### and then anything else
      elsif ($_ =~ m/^[ ]*--(.[^ ]*)[ ]*(.*)/){
         print RESULTS "|   | --$1 | $2 |\n";
	 if ($test_next == 2){
	     $test_next = 0;
	 }
	 else{
	     $test_next = 1;
	 }
      }
      #if the line starts with some amount of white space AND *-
      elsif ($_ =~ m/^[ ]*(.*)/){
	 # -* is following a line with options we put the line in the last column
	 if ($test_next){
	     print RESULTS "|   |   | $1 |\n";
	 }
      }

   }
}

#The subroutine for parsing the GPSTk application example and moving to Twiki markup
sub ParseExample {
   open TEMP, "< ". $Local_out ."temp.log" or die "can't open datafile: $!";
   my $count = 0;
   while ( <TEMP> ) {
      $count++;
      print RESULTS $_;
      #if the example should be truncated it will be cut off at 19 lines
      if ($truncate_example){
	  if ($count > 19) {
	      print RESULTS ". . .\n\n\n";
	      last;
	  } 
      }
   }
   print RESULTS "\n";
}

