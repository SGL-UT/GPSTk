#!/bin/bash
#for YEAR in 1983 1984 1985 1986 1987 1988 1989 1990 1991 1992 1993 1994 1995 1996 1997 1998 1999 2000 2001 2002 2003 2004 2005 2006 2007 2008 2009 2010
for YEAR in 1980 1981 1982
do
 echo "Generating calendars for year $YEAR "
 ./calgps -Y $YEAR -p gps-calendar-$YEAR.ps > /dev/null
 ./calgps -Y $YEAR -s gps-calendar-$YEAR.svg   > /dev/null
 convert gps-calendar-$YEAR.ps gps-calendar-$YEAR.pdf > /dev/null
done



