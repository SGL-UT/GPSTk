#!/bin/bash

source $(dirname "$BASH_SOURCE")/build_setup.sh


#----------------------------------------
# capture the test output
#----------------------------------------

ctest_keyword=GpstkTest
ctest_log_raw=$build_root/Testing/Temporary/LastTest.log
ctest_log_save=$build_root/test.log
cat $ctest_log_raw | grep "$ctest_keyword" > $ctest_log_save

#----------------------------------------
# summary results
#----------------------------------------

test_count=$(cat $ctest_log_save | wc -l)
tests_passed=$(cat $ctest_log_save | grep "failBit=0" | wc -l)
tests_failed=$(cat $ctest_log_save | grep "failBit=1" | wc -l)

echo ""
echo "Build: $build_root"
echo ""
echo "Tests: Summary Results"
echo "------------------------------------------------------------"
echo "Number of tests run    = $test_count"
echo "Number of tests passed = $tests_passed"
echo "Number of tests failed = $tests_failed"
echo "------------------------------------------------------------"
echo ""

#----------------------------------------
# Search test.log for keywords, summarize
#----------------------------------------

keyword_list=()
keyword_list+=("AlmOrbit")
keyword_list+=("ANSITime")
keyword_list+=("Binex_Attrs")
keyword_list+=("Binex_ReadWrite")
keyword_list+=("Binex_Types")
keyword_list+=("Binex_UpdateExtract")
keyword_list+=("BinUtils")
keyword_list+=("BivarStats")
keyword_list+=("CivilTime")
keyword_list+=("ClockModel")
keyword_list+=("CommonTime")
keyword_list+=("Convhelp")
keyword_list+=("FileFilter")
keyword_list+=("FileHunter")
keyword_list+=("FileSpec")
keyword_list+=("FileStore")
keyword_list+=("FileUtils")
keyword_list+=("GPSWeekSecond")
keyword_list+=("GPSWeekZcount")
keyword_list+=("GPSZcount")
keyword_list+=("HelmertTransform")
keyword_list+=("IonoModel")
keyword_list+=("JulianDate")
keyword_list+=("Matrix")
keyword_list+=("MJD")
keyword_list+=("ObsID")
keyword_list+=("PolyFit")
keyword_list+=("Position")
keyword_list+=("RACRotation")
keyword_list+=("ReferenceFrame")
keyword_list+=("RinexEph")
keyword_list+=("RinexMet")
keyword_list+=("RinexNav")
keyword_list+=("RinexObs")
keyword_list+=("Rinex_T")
keyword_list+=("SatID")
keyword_list+=("Sinex_ReadWrite")
keyword_list+=("SP3Eph")
keyword_list+=("SP3SatID")
keyword_list+=("Stats")
keyword_list+=("Stl_helpers")
keyword_list+=("StringUtils")
keyword_list+=("SystemTime")
keyword_list+=("TimeConverters")
keyword_list+=("TimeRange")
keyword_list+=("TimeString")
keyword_list+=("TimeSystem")
keyword_list+=("TimeTag")
keyword_list+=("Triple")
keyword_list+=("UnixTime")
keyword_list+=("ValidType")
keyword_list+=("Vector")
keyword_list+=("WxObsMap")
keyword_list+=("Xvt")
keyword_list+=("YDSTime")


myheader="%-16s, %4s, %4s, %4s, %5s\n"
myformat="%-16s, %4d, %4d, %4d, %5.1f \n"
mydivider="========================================="


# myformat="name = %-16s, tests run = %4d, passed = %4d, failed = %4d, %%passed = %5.1f \n"

echo "------------------------------------------------------------"
echo "Tests: Results by Category"
echo "------------------------------------------------------------"

echo ""
printf "$myheader" "NAME" "RUN" "PASS" "FAIL" "%PASS"
printf "%s\n" "$mydivider"

for keyword in "${keyword_list[@]}"; do
    
    tests_run=$(cat $ctest_log_save | grep -i "$keyword" | wc -l)
    tests_passed=$(cat $ctest_log_save | grep -i "$keyword" | grep "failBit=0" | wc -l)
    tests_failed=$(cat $ctest_log_save | grep -i "$keyword" | grep "failBit=1" | wc -l)
    percent_pass=$(awk -v r=$tests_run -v p=$tests_passed 'BEGIN { print (100*(p/r)) }')
    
    printf "$myformat" "$keyword" "$tests_run" "$tests_passed" "$tests_failed" "$percent_pass"
    
done

echo ""
echo "------------------------------------------------------------"
echo ""

