#!/usr/bin/perl -w
#
# "$Id$"


#
# This script generates a set of dummy files and directories for
# running FileSpecTest. They are in the form of 
#     fstest/ADMS###/RINEXOBS/S##1###A.##O

for($stn = 401; $stn <= 403; $stn++)
{
    $stnDir = sprintf("fstest/ADMS%03i", $stn);
    `mkdir -p $stnDir`;
    $stnDir .= "/RINEXOBS";
    `mkdir $stnDir`;
    $stnDir .= sprintf("/S%02i1", substr($stn,1,2));
    for ($year = 1; $year <= 2; $year++)
    {
	for ($day = 50; $day <= 53; $day++)
	{
	    $thisFile = $stnDir . sprintf("%03iA.%02iO", $day, $year);
	    print "$thisFile\n";
	    `touch $thisFile`;
	}
    }

#    for ($num = 1; $num <= 5; $num++)
#    {
#	`touch $num`;
#    }
}
