Rinex data from June 18, 2016, doy 170, week 1901 

These files were chosen because they have bunch of signals in them.

```
wget -q -O - ftp://cddis.gsfc.nasa.gov/gnss/data/highrate/2016/170/16d/01/FAA100PYF_R_20161700100_15M_01S_MO.crx.gz | zcat | CRX2RNX - | head -n 233 > FAA100PYF_R_20161700100_15M_01S_MO

wget -q -O - ftp://cddis.gsfc.nasa.gov/gnss/data/highrate/2016/170/16d/01/faa1170b00.16d.Z | zcat | CRX2RNX - | head -n 475 > faa1170b00.16o

wget -q -O - ftp://cddis.gsfc.nasa.gov/pub/gps/data/highrate/2016/170/16d/01/nklg170b00.16d.Z  | zcat | CRX2RNX - | head -n 527 >nklg170b00.16o

wget -q -O - ftp://cddis.gsfc.nasa.gov/gnss/products/1901/igs19016.sp3.Z | zcat | head -n 319 > igs19016.sp3
```


R3.03
G: GPS
R: GLONASS
E: Galileo
J: QZSS
C: BDS
I: IRNSS
S: SBAS payload M: Mixed
