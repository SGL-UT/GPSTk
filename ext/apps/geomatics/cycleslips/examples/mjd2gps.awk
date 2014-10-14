# convert MJD in column C to 2 columns: GPS sow and week.
# call awk -f mjd2gps.awk -v C=<column_of_MJD>
# (optional: add -v ROLL=NO) to prevent week rollover in the sow.
# (optional: add -v WEEK=NO) to prevent printing the week (then C -> 1 col)
# (optional: add -v ROUND=NO) to prevent rounding to nearest second
BEGIN {
	weeksave = -1
}
{
	mjd = $C;
	week = int((mjd-44244.0)/7.0)
	if(weeksave == -1) weeksave = week
	if(ROLL=="NO") week = weeksave
	sow = (mjd-44244.0-7*week)*86400.
	if(ROUND=="NO") {
		if(WEEK=="NO") str = sprintf("%11.3lf",sow);
		else           str = sprintf("%11.3lf %4d",sow,week);
	}
	else {
		sow = int(sow+0.5);
		if(WEEK=="NO") str = sprintf("%8.0lf",sow);
		else           str = sprintf("%8.0lf %4d",sow,week);
	}
	$C = str
	print $0
}
