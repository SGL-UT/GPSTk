#!/usr/bin/perl
# $Id: //depot/sgl/gpstk/dev/tests/testscript.pl#6 $

#            Ryan Mire ARL SGL 
#                June 2005 
# last saved: July 25 1:50pm
# 
#
# Test Script created for the purpose of automating the testing of the GPStk Toolkit
# Proper Command Line Syntax:
#   >>test script terminated\n testscript <home directory> <configfile> <output directory> [<html file and directory path>]
#


$homedir=$ARGV[0];
die "\nThe given output directory, $homedir, is  not found or does not exist.\n Try......>>test script terminated\n testscript <home directory> <configfile> <output directory> [<html file name>]\n"
 if((-e $homedir)==0);

$configfile=$ARGV[1];

$outdir=$ARGV[2];
mkdir "$outdir"
  if((-e $outdir)==0);

$html=$ARGV[3];

$exedir=$homedir."/gpstk/dev/tests";
die "\nThe test directory, $exedir, is not found or does not exist.......test script terminated"
 if((-e $exedir)==0);

$datdir=$exedir."/data";
die "\nThe data directory, $datdir, is not found or does not exist......test script terminated"
 if((-e $datdir)==0);

$passfail="All Tests Passed";


$n=0;

open(CONFIG, $configfile);
$testspec="";
$mainlog="$outdir/testscript.log";
open(LOG,">".$mainlog);
open(HTML,">".$html);

if($html)          
{
   chomp($kernel =`uname -s`);
   chomp($processor = `uname -p`);
   print HTML "<tr class=\"alt\">\n";
   print HTML "<td> $kernel</td>\n";                   #Name of Kernel
   print HTML "<td> $processor</td>\n";                #Name of Processor
}

while(<CONFIG>)
{
  # print "N = $n\n";
   next if /^(#|!)/;                         #Skips lines beginning with "!" 
   s/^\s+//;                                 #Substitute any space at the beginning of the line with nothing
   
   if (/\\$/)                                #If the line contains a "\" at the end
   {
      s/\\$//;                               #Substitute the endline character "\" with nothing
      s/\n/ /;                               #Substitute the newline character with a space
      $testspec=$testspec.$_;
      next;
   }
   else
   {
      $testspec=$testspec.$_;
   }

      $n+=1;                                 #Counts up through all the executed tests

      $testspec=~ s/;\s*$//;                 #Substitue the colon and space endline character with nothing
      $testspec=~ s/\$outdir/$outdir/;       #Replace the word $outdir with the true local value of $outdir
      $testspec=~ s/\$datdir/$datdir/g;      #Replace the word $datdir with the true local value of $datdir
     # print "\nOriginal: $testspec\n";

      @Cmdline=split /,/, $testspec;         #Split the line at any commas and create the command line array
      @Cmdname=split /(\s+|,|;)/, $testspec; #Split the line at any comas or whitespace or colons and create the command name array

      $Cmdline="$exedir/@Cmdline[0]";        #Add on the proper local directory $exedir to begin the building of the command line
      $Cmdname="@Cmdname[0]";                
      $scrfile="$outdir/$Cmdname.scr";       #Creates the generic screen output file in the proper $outdir local directory
          
     # print "Command Line: $Cmdline\n";
      print "Executing: $Cmdname\n";
     # print "File Spec for Screen Capture: $scrfile\n";
 
      $ret_val="Success";
     
      if (system "$Cmdline>$scrfile")
      {
         $ret_val="Failure";
      }
   
      $diff_val="Success";
      $diffline=@Cmdline[1];                   #If there are any pieces after the first space/comma/colon diffline gets them

     # print "DIFFLINE:     $diffline\n";

      if ($diffline)                            
      {
	
        
         $diffline=~ s/^\s//;                   #Takes off the beginning whitespace 
         @diffline=split /:/, $diffline;        #Splits at any colon
         $output=@diffline[0];                  #Text before the colon
         $golden=@diffline[1];                  #Text after the colon
         $diff_file="$outdir/$Cmdname.dif";     
                  
         system "sdiff -s -I File $outdir/$output $datdir/$golden>$diff_file";      #executes Unix Bash Shell command

      
         if (!-z $diff_file)                    #If the diff file is non-empty
         {
            $diff_val="False";
         }
      }
   
      if(-z $diff_file)
      {
        system "rm $diff_file"
      }

      print "\t\tTest Run:     $ret_val\n";
      print "\t\tDiff Value:   $diff_val\n";
#     print "$kernel \t $processor \n";


      if ($ret_val eq 'Success' && $diff_val eq 'Success')
      {
         print LOG "$Cmdname......Success\n";
      }
      else
      {
          print LOG "$Cmdname......Failure   -see $Cmdname.dif and $Cmdname.scr\n";        
          $passfail="One or More Tests Failed";
          chomp($n);
          chomp($Cmdname);
          $failed[$n]="\t $n \t $Cmdname \n";      #Adds failed test to Failed Test list
      }
      
      if ($html)
      {
          if($ret_val eq 'Success' && $diff_val eq 'Success')
          {
	      $colorcode="pass";
          }
          else
          {
	      $colorcode="fail";
	  }
           print HTML "<td> class=\"$colorcode\"> $ret_val </td>\n";     
      }

      $testspec="";
}
   print LOG "\n Failed Test List :\n";
 print LOG "\n @failed";

if($html)
{
   print HTML "</tr>\n";
}

   close(HTML);
   close(LOG);
   close(CONFIG);
   
 print "\n    Main Directory:       $homedir\n";
 print "    Configuration File:   $configfile\n";
 print "    Output Directory:     $outdir\n";
 print "    Test Directory:       $exedir\n";
 print "    Data directory:       $datdir\n";
 print "\nRan $n tests\n"; 
 print "$passfail\n";
   
if(@failed!=0)
{   
    print "Failed Test List:\n";
    print "@failed";
}
