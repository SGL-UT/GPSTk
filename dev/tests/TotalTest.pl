#!/usr/bin/perl
#This script iterates recursivly through a directory tree and looks for "bad words" within files
#Script by Eric Hagen
#Last saved Jun 7, 2006

use strict;
use warnings;




print "Please enter the location of the directory you wish to view\n";
print "Example /usr/bin\n";                             #Interogative Portion

chomp(my $dirn = <STDIN>);                              #User Input for Directory
print "\n\n\n\n\n\n\nPLEASE WAIT THIS MAY TAKE SOME TIME\n\n\n\n\n\n\n\n";

system("rm $dirn/TestResults.log");
system("rm $dirn/CovResults.log"); #Gcov results
open (DATA,">>CovResults.log");                         #Open file to which the output will go
chdir "$dirn";
system("jam clean");
print ("Jamming!\n\n");

system("jam");
dirz("$dirn","GoodTests");        #Start Subroutine

close(DATA);                                            #Close the output file


sub dirz {
    my($dir, $oldFname) = @_;
    chdir $dir;	
    opendir D, $dir or die "Unable to open $dir: $!";   #Open the directory or throw exception

    foreach my $fname (readdir D)  {                    #Loop over available files/directories
	next if "$fname" =~ /^\./;			#As long as the file is not a . or .. file
	next if "$dir/$fname" =~ /Checks/;
	next if "$dir/$fname" =~ /Logs/;
	next if "$dir/$fname" =~ /Examples/;
	dirz("$dir/$fname",$fname) if -d "$dir/$fname";        #Recursion to find the files in sub directories
	opendir D, "$dir/$fname" if -d "$dir/$fname";   #Reopen the old directory from before recursion
	if ($fname =~ /x(.*)\.tst$/) {
		print "Executing $fname\n\n";
		system("./$fname >>$dirn/TestResults.log 2>&1");
		gcov($dir , $oldFname);
	}
	
    }
    #system("rm $oldFname.cpp $oldFname.hpp");
    closedir D;                                         #Close the directory
}

sub gcov {
	my($dirn2, $filename) = @_;
	opendir D, $dirn2;
	foreach my $fname (readdir D) {
		next if "$fname" =~ /^\./;
		next unless $fname =~ /^(x?.*$filename.*)\.gcda$/;
		system("gcov $1.gcda >$dirn/GcovStuff");
		print ("gcov $1.gcda >$dirn/GcovStuff\n\n");
		open FILE, "<$dirn/GcovStuff";
		while(defined(my $linez = <FILE>)) { 
			if ($linez =~ /\b$filename.[ch]pp\b/) {
				print DATA "$filename\n";
				print DATA "\t$linez";
				$linez = <FILE>;
				print DATA "\t$linez";
				$linez = <FILE>;
				print DATA "\t$linez";
			}
		}
	}
}
