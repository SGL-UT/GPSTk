IGS Rinex data from June 18, 2016, doy 170, week 1901 

```
# Configure the proxy for http and ftp traffic
export ftp_proxy=http://webproxy.arlut.utexas.edu:3128
export http_proxy=http://webproxy.arlut.utexas.edu:3128

# Rinex 3.00 Mixed w BDS
wget -q -O - ftp://cddis.gsfc.nasa.gov/pub/gps/data/campaign/mgex/daily/rinex3/2016/015/16o/sptu0150.16o.Z | zcat > sptu1050.16o
# Manually trimmed for epochs (2016  1 15  1 22 30.0000000) - (2016  1 15  1 24 30.0000000)

# Rinex 3.02 Mixed w BDS
wget -q -O - ftp://cddis.gsfc.nasa.gov/pub/gps/data/campaign/mgex/daily/rinex3/2016/015/16o/nrmg0150.16o.Z | zcat | head -n 279 > nrmg1050.16o

# Rinex 3.03 Mixed w BDS
wget -q -O - ftp://cddis.gsfc.nasa.gov/pub/gps/data/campaign/mgex/daily/rinex3/2016/015/16o/solo0150.16o.Z | zcat | head -n 303 > solo1050.16o

# Rinex 3.03
wget -q -O - ftp://cddis.gsfc.nasa.gov/gnss/data/highrate/2016/170/16d/01/UCAL00CAN_S_20161700100_15M_01S_MO.crx.gz | zcat | CRX2RNX - | head -n 216 > UCAL00CAN_S_20161700100_15M_01S_MO

# Rinex 3.02
wget -q -O - ftp://cddis.gsfc.nasa.gov/gnss/data/highrate/2016/170/16d/01/FAA100PYF_R_20161700100_15M_01S_MO.crx.gz | zcat | CRX2RNX - | head -n 233 > FAA100PYF_R_20161700100_15M_01S_MO

# Rinex 2.10 w just GPS
wget -q -O - ftp://cddis.gsfc.nasa.gov/pub/gnss/data/daily/2016/170/16d/osn31700.16d.Z  | zcat | CRX2RNX - | head -n 129 >osn31700.16o

# Rinex 2.11 w just GPS
wget -q -O - ftp://cddis.gsfc.nasa.gov/pub/gnss/data/daily/2016/170/16d/cags1700.16d.Z  | zcat | CRX2RNX - | head -n 135 >cags1700.16o

# Rinex 2.11 w GPS and GLONASS
wget -q -O - ftp://cddis.gsfc.nasa.gov/gnss/data/highrate/2016/170/16d/01/faa1170b00.16d.Z | zcat | CRX2RNX - | head -n 475 >faa1170b00.16o

# Rinex 2.11 w GPS and GLONASS and more obs than faa1
wget -q -O - ftp://cddis.gsfc.nasa.gov/pub/gps/data/highrate/2016/170/16d/01/nklg170b00.16d.Z  | zcat | CRX2RNX - | head -n >527 >nklg170b00.16o

# Rinex 2.11 w GPS, GLONASS, Galileo, and Geosync
wget -q -O - ftp://cddis.gsfc.nasa.gov/pub/gnss/data/daily/2016/170/16d/kerg1700.16d.Z  | zcat | CRX2RNX - | head -n 727 >kerg1700.16o

# SP3 w just GPS
wget -q -O - ftp://cddis.gsfc.nasa.gov/gnss/products/1901/igs19016.sp3.Z | zcat | head -n 319 > igs19016.sp3
```
