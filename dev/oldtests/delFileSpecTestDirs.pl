#!/usr/bin/perl -w
# $Id$
#
#
# This script eliminates the set of dummy files and directories created
# by genFileSpecTestDirs. The dummy files/directories are used while
# running FileSpecTest but are unnecessary afterwards.
#

for ($stn = 401; $stn <= 403; $stn++)
{
    $stnDir = sprintf("fstest/ADMS%03i/RINEXOBS", $stn);
    
    for ($year = 1; $year <= 2; $year++)
    {

        for ($day = 50; $day <= 53; $day++)
        {
            $stnFile = $stnDir . sprintf("/S%02i1%03iA.%02iO", substr($stn,1,2), $day, $year);
            # print "$stnFile\n";
             `rm $stnFile`;
        }
    }
 `rmdir -p $stnDir`;
}
