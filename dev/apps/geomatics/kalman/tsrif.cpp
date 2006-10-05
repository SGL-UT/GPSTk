#pragma ident "$Id: $"

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file tsrif.cpp
 * Test class measurement update of SRIFilter
 */

//------------------------------------------------------------------------------------
#include <iostream>
#include <iomanip>

#include "SRIFilter.hpp"
#include "StringUtils.hpp"
#include "random.hpp"
#include "DayTime.hpp"
#include "Stats.hpp"
#include "PolyFit.hpp"
#include "RobustStats.hpp"

using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// convenience
void LSFunc(Vector<double>& X, Vector<double>& f, Matrix<double>& P);
void doIt(Vector<double>& data, Vector<double>& sol, Matrix<double>& cov);

//------------------------------------------------------------------------------------
// prints inside SRIFilter
bool verbose=false;
// data for the various tests
int test,batch,batchsize,batchlen;
// fake satellites
// (X,Y,Z)(t) = RSV*(st*sp,st*cp,ct)
// where t=Theta+OmegaTheta*t, p=Phi+OmegaPhi*t, s=sin(), c=cos()
double RSV[4]={22.123, 20.984, 21.873, 22.749},
  OmegaPhi[4]={ 0.222,  0.128, -0.209,  0.399},
OmegaTheta[4]={ 0.199, -0.233,  0.212, -0.274},
       Phi[4]={-5.000,  3.209,  0.000, -1.230},
     Theta[4]={ 0.000,  6.123, -1.237,  4.003},
      Bias[4]={18.239, 83.928, -2.489, 45.209},
     Drift[4]={ 0.238, -0.983,  0.703,  0.834};
Matrix<double> P6;          // for test 6,7
Vector<double> D6;          // for test 6,7
Vector<double> D4;          // for test 4,5
// test 8,9
// n is the number of turns
// w is the wire width (microm)
// d is the inner diameter (microm)
// D is the outer diameter (microm)
// L is the measured inductance (nH)
double n8[50] = {
    3.7500, 4.2500, 9.2500, 4.5000, 5.7500, 12.0000,  7.0000, 6.0000, 8.0000, 4.0000,
   11.0000, 8.0000, 6.0000, 3.0000, 5.0000,  9.0000, 16.0000, 6.0000, 3.0000, 4.0000,
    2.0000, 2.0000, 3.0000, 9.2500, 4.0000,  8.0000,  6.0000, 3.5000, 4.5000, 5.5000,
    6.5000, 7.5000, 9.2500, 6.7500, 2.5000,  4.2500,  7.0000, 3.0000, 9.0000, 8.0000,
    5.0000, 9.0000, 3.2500, 4.5000, 3.0000,  5.7500,  3.0000, 4.0000, 6.0000, 8.0000
};
double w8[50] = {
   22.4000, 21.0000,  6.0000, 17.6000, 12.0000,  3.2000, 13.0000, 24.0000,  5.0000,
    5.0000,  9.0000, 14.0000, 19.0000, 19.0000, 24.0000,  6.5000,  5.0000,  9.0000,
    9.0000, 14.0000, 14.0000, 19.0000, 24.0000,  5.2000, 16.0000, 10.0000, 15.0000,
   10.0000, 10.0000, 10.0000, 10.0000, 10.0000,  5.2000, 13.0000, 13.0000, 13.0000,
   15.0000, 20.0000,  6.5000,  6.0000,  7.0000,  7.0000, 25.0000, 23.0000, 18.0000,
    9.5000, 90.0000, 16.0000, 16.0000, 16.0000
};
double d8[50] = {
   165.4500, 132.8500,  64.3500, 221.9000,  82.0500,  57.0000,  34.0000,  42.0000,
   164.0000, 236.0000,  22.0000,  20.0000,  32.0000, 170.0000,  28.0000,  25.0000,
    20.0000, 152.0000, 230.0000, 164.0000, 236.0000, 216.0000, 140.0000,  15.8000,
    38.0000,  80.0000,  75.0000, 177.5000, 115.5000,  75.5000,  44.5000,  20.5000,
    15.8000,  34.0000, 204.0000,  34.0000,  30.0000, 210.0000,   5.0000,  46.0000,
    44.0000,  44.0000, 150.5000,  51.0000, 168.0000,  23.7500, 136.0000,  74.0000,
   100.0000, 136.0000
};
double D8[50] = {
   345, 325, 210, 395, 240, 180, 300, 400, 300, 300,
   300, 300, 300, 300, 300, 230, 300, 300, 300, 300,
   300, 300, 300, 145, 226, 310, 285, 255, 216, 199,
   191, 190, 145, 290, 290, 190, 300, 410, 210, 226,
   154, 250, 340, 300, 300, 190, 700, 262, 392, 532
};
double L8[50] = {
    5.2000,  5.5000, 13.7000, 10.2000,  6.5000, 20.1000,  7.6000,  7.3000, 23.5000,
    9.7000, 17.3000,  9.0000,  5.5000,  3.5000,  3.7000,  9.6000, 36.6000, 12.3000,
    5.2000,  5.9000,  2.4000,  2.1000,  2.8000,  6.4000,  2.1000, 13.9000,  7.2000,
    5.2000,  5.3000,  5.3000,  5.3000,  5.5000,  6.2000,  7.0000,  3.1000,  2.1000,
    7.4000,  4.3000,  7.5000,  9.1000,  2.8000, 12.1000,  3.6000,  3.5000,  3.4000,
    3.4000,  3.9000,  3.1000,  9.8000, 23.8000
};
double x10[5]={ 1.5, 1.5, 1.8, 2.0, 2.5 };
//double x10[5]={ 1.5, 1.5, 1.0, 2.0, 2.5 }; // sats in line -> clear double well
double y10[5]={ 1.5, 2.0, 2.5, 1.75, 1.5 };
// 150 data - actually 149, 2005 is made up
int years11[] = {1856,1857,1858,1859,1860,1861,1862,1863,1864,1865,1866,1867,
   1868,1869,1870,1871,1872,1873,1874,1875,1876,1877,1878,1879,1880,1881,
   1882,1883,1884,1885,1886,1887,1888,1889,1890,1891,1892,1893,1894,1895,
   1896,1897,1898,1899,1900,1901,1902,1903,1904,1905,1906,1907,1908,1909,
   1910,1911,1912,1913,1914,1915,1916,1917,1918,1919,1920,1921,1922,1923,
   1924,1925,1926,1927,1928,1929,1930,1931,1932,1933,1934,1935,1936,1937,
   1938,1939,1940,1941,1942,1943,1944,1945,1946,1947,1948,1949,1950,1951,
   1952,1953,1954,1955,1956,1957,1958,1959,1960,1961,1962,1963,1964,1965,
   1966,1967,1968,1969,1970,1971,1972,1973,1974,1975,1976,1977,1978,1979,
   1980,1981,1982,1983,1984,1985,1986,1987,1988,1989,1990,1991,1992,1993,
   1994,1995,1996,1997,1998,1999,2000,2001,2002,2003,2004,2005
};
double temps11[] = {-0.379,-0.462,-0.416,-0.225,-0.372,-0.404,-0.527,-0.273,
   -0.474,-0.267,-0.223,-0.291,-0.225,-0.305,-0.296,-0.351,-0.266,-0.327,
   -0.376,-0.424,-0.452,-0.211,-0.057,-0.287,-0.296,-0.246,-0.262,-0.318,
   -0.349,-0.350,-0.254,-0.343,-0.311,-0.200,-0.413,-0.353,-0.407,-0.449,
   -0.414,-0.362,-0.199,-0.186,-0.339,-0.250,-0.192,-0.256,-0.349,-0.445,
   -0.443,-0.371,-0.292,-0.505,-0.477,-0.447,-0.442,-0.465,-0.406,-0.393,
   -0.248,-0.159,-0.373,-0.495,-0.410,-0.292,-0.293,-0.217,-0.323,-0.299,
   -0.347,-0.246,-0.116,-0.218,-0.227,-0.361,-0.152,-0.099,-0.137,-0.239,
   -0.139,-0.172,-0.121,-0.024,0.076,-0.038,-0.083,0.028,-0.020,0.001,
   0.158,0.039,-0.120,-0.105,-0.093,-0.097,-0.210,-0.093,-0.025,0.045,
   -0.170,-0.189,-0.272,-0.006,0.061,0.014,-0.028,0.015,0.008,0.039,-0.233,
   -0.166,-0.084,-0.093,-0.105,0.040,-0.031,-0.187,-0.041,0.093,-0.171,
   -0.123,-0.200,0.057,-0.036,0.067,0.102,0.133,0.019,0.226,0.031,0.014,
   0.096,0.253,0.240,0.163,0.310,0.251,0.116,0.179,0.232,0.373,0.227,0.411,
   0.579,0.340,0.289,0.422,0.474,0.474,0.459,0.5
};

const int M151=72;
string msg1(" (from DDBase T202B ASWA CTRA G11 G14)");
int n151[] = {
   213, 214, 215, 216, 217, 218, 228, 229, 237, 238, 239, 240, 241, 242, 243, 244,
   245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260,
   261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276,
   277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291, 292,
   293, 294, 295, 296, 297, 298, 299, 300
};
double y151[] = {
  0.021, -0.012, -0.027, -0.068, -0.024, -0.043, -0.064, -0.023, -0.114, -0.030,
  0.009, 0.095, 0.108, 0.088, 0.126, 0.195, 0.295, 0.305, 0.303, 0.305, 0.310,
  0.312, 0.310, 0.309, 0.316, 0.313, 0.319, 0.321, 0.322, 0.319, 0.320, 0.320,
  0.319, 0.323, 0.326, 0.325, 0.330, 0.328, 0.329, 0.330, 0.339, 0.337, 0.336,
  0.344, 0.343, 0.343, 0.347, 0.346, 0.345, 0.344, 0.339, 0.345, 0.352, 0.349,
  0.355, 0.357, 0.360, 0.360, 0.359, 0.363, 0.361, 0.357, 0.359, 0.362, 0.363,
  0.364, 0.366, 0.366, 0.367, 0.374, 0.374, 0.375
};

const int M152=276;
string msg2(" (from DDBase T202D ASWA CTRA G16 G25)");
int n152[] = {
25, 26, 27, 28, 29, 30, 31, 32, 33, 34,
35, 36, 37, 38, 39, 40, 41, 42, 43, 44,
45, 46, 47, 48, 49, 50, 51, 52, 53, 54,
55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
85, 86, 87, 88, 89, 90, 91, 92, 93, 94,
95, 96, 97, 98, 99, 100, 101, 102, 103, 104,
105, 106, 107, 108, 109, 110, 111, 112, 113, 114,
115, 116, 117, 118, 119, 120, 121, 122, 123, 124,
125, 126, 127, 128, 129, 130, 131, 132, 133, 134,
135, 136, 137, 138, 139, 140, 141, 142, 143, 144,
145, 146, 147, 148, 149, 150, 151, 152, 153, 154,
155, 156, 157, 158, 159, 160, 161, 162, 163, 164,
165, 166, 167, 168, 169, 170, 171, 172, 173, 174,
175, 176, 177, 178, 179, 180, 181, 182, 183, 184,
185, 186, 187, 188, 189, 190, 191, 192, 193, 194,
195, 196, 197, 198, 199, 200, 201, 202, 203, 204,
205, 206, 207, 208, 209, 210, 211, 212, 213, 214,
215, 216, 217, 218, 219, 220, 221, 222, 223, 224,
225, 226, 227, 228, 229, 230, 231, 232, 233, 234,
235, 236, 237, 238, 239, 240, 241, 242, 243, 244,
245, 246, 247, 248, 249, 250, 251, 252, 253, 254,
255, 256, 257, 258, 259, 260, 261, 262, 263, 264,
265, 266, 267, 268, 269, 270, 271, 272, 273, 274,
275, 276, 277, 278, 279, 280, 281, 282, 283, 284,
285, 286, 287, 288, 289, 290, 291, 292, 293, 294,
295, 296, 297, 298, 299, 300
};
double y152[] = {
0.173, 0.189, 0.175, 0.126, 0.044, -0.081, -0.076, -0.079, -0.079, -0.081,
-0.077, -0.079, -0.074, -0.070, -0.073, -0.072, -0.066, -0.067, -0.060, -0.059,
-0.059, -0.054, -0.055, -0.049, -0.052, -0.052, -0.046, -0.044, -0.044, -0.038,
-0.037, -0.033, -0.036, -0.031, -0.040, -0.036, -0.035, -0.031, -0.035, -0.031,
-0.030, -0.031, -0.030, -0.030, -0.028, -0.025, -0.024, -0.026, -0.027, -0.025,
-0.014, -0.014, -0.012, -0.012, -0.011, -0.005, 0.000, -0.004, 0.001, -0.004,
0.002, 0.001, 0.003, 0.004, 0.002, 0.009, 0.012, 0.011, 0.012, 0.012,
0.011, 0.007, 0.009, 0.011, 0.010, 0.012, 0.010, 0.010, 0.017, 0.018,
0.019, 0.026, 0.024, 0.024, 0.031, 0.034, 0.037, 0.038, 0.040, 0.045,
0.047, 0.045, 0.046, 0.046, 0.044, 0.048, 0.048, 0.046, 0.046, 0.049,
0.046, 0.047, 0.050, 0.044, 0.047, 0.049, 0.050, 0.055, 0.057, 0.056,
0.056, 0.064, 0.066, 0.072, 0.072, 0.072, 0.071, 0.073, 0.075, 0.075,
0.077, 0.076, 0.072, 0.071, 0.081, 0.081, 0.082, 0.086, 0.086, 0.088,
0.083, 0.090, 0.089, 0.094, 0.091, 0.092, 0.095, 0.097, 0.093, 0.095,
0.098, 0.098, 0.099, 0.098, 0.098, 0.103, 0.100, 0.102, 0.101, 0.106,
0.104, 0.107, 0.108, 0.112, 0.112, 0.112, 0.116, 0.117, 0.118, 0.119,
0.125, 0.124, 0.127, 0.127, 0.126, 0.127, 0.127, 0.128, 0.128, 0.132,
0.131, 0.135, 0.131, 0.135, 0.130, 0.131, 0.135, 0.134, 0.136, 0.139,
0.140, 0.140, 0.140, 0.144, 0.145, 0.147, 0.147, 0.149, 0.152, 0.150,
0.151, 0.151, 0.154, 0.154, 0.157, 0.161, 0.163, 0.163, 0.161, 0.162,
0.164, 0.165, 0.165, 0.167, 0.166, 0.166, 0.169, 0.170, 0.171, 0.170,
0.173, 0.172, 0.173, 0.174, 0.175, 0.175, 0.176, 0.176, 0.179, 0.182,
0.184, 0.181, 0.180, 0.183, 0.182, 0.188, 0.187, 0.190, 0.192, 0.194,
0.196, 0.195, 0.196, 0.200, 0.196, 0.201, 0.201, 0.198, 0.199, 0.200,
0.199, 0.201, 0.202, 0.203, 0.201, 0.204, 0.206, 0.209, 0.207, 0.208,
0.210, 0.210, 0.209, 0.212, 0.213, 0.215, 0.211, 0.216, 0.215, 0.214,
0.212, 0.217, 0.216, 0.217, 0.218, 0.219, 0.222, 0.222, 0.223, 0.224,
0.227, 0.227, 0.226, 0.224, 0.226, 0.227
};

const int M153=190;
string msg3(" (from DDBase T202D ASWA CTRA G20 G25)");
int n153[] = {
111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
121, 122, 123, 124, 125, 126, 127, 128, 129, 130,
131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
141, 142, 143, 144, 145, 146, 147, 148, 149, 150,
151, 152, 153, 154, 155, 156, 157, 158, 159, 160,
161, 162, 163, 164, 165, 166, 167, 168, 169, 170,
171, 172, 173, 174, 175, 176, 177, 178, 179, 180,
181, 182, 183, 184, 185, 186, 187, 188, 189, 190,
191, 192, 193, 194, 195, 196, 197, 198, 199, 200,
201, 202, 203, 204, 205, 206, 207, 208, 209, 210,
211, 212, 213, 214, 215, 216, 217, 218, 219, 220,
221, 222, 223, 224, 225, 226, 227, 228, 229, 230,
231, 232, 233, 234, 235, 236, 237, 238, 239, 240,
241, 242, 243, 244, 245, 246, 247, 248, 249, 250,
251, 252, 253, 254, 255, 256, 257, 258, 259, 260,
261, 262, 263, 264, 265, 266, 267, 268, 269, 270,
271, 272, 273, 274, 275, 276, 277, 278, 279, 280,
281, 282, 283, 284, 285, 286, 287, 288, 289, 290,
291, 292, 293, 294, 295, 296, 297, 298, 299, 300
};
double y153[] = {
-0.090, -0.100, -0.084, -0.027, 0.070, 0.202, 0.200, 0.204, 0.203, 0.205,
0.207, 0.208, 0.207, 0.217, 0.212, 0.217, 0.219, 0.221, 0.224, 0.224,
0.229, 0.228, 0.238, 0.235, 0.232, 0.237, 0.239, 0.242, 0.246, 0.243,
0.242, 0.248, 0.243, 0.246, 0.248, 0.254, 0.253, 0.253, 0.253, 0.259,
0.256, 0.259, 0.262, 0.265, 0.267, 0.269, 0.272, 0.274, 0.273, 0.274,
0.276, 0.275, 0.282, 0.282, 0.287, 0.281, 0.289, 0.290, 0.293, 0.297,
0.299, 0.301, 0.298, 0.300, 0.302, 0.306, 0.309, 0.310, 0.310, 0.308,
0.317, 0.316, 0.316, 0.323, 0.326, 0.322, 0.329, 0.327, 0.328, 0.338,
0.336, 0.332, 0.339, 0.345, 0.343, 0.347, 0.350, 0.353, 0.354, 0.355,
0.356, 0.361, 0.364, 0.361, 0.364, 0.368, 0.366, 0.370, 0.372, 0.373,
0.368, 0.374, 0.379, 0.376, 0.374, 0.378, 0.380, 0.384, 0.387, 0.388,
0.389, 0.394, 0.396, 0.395, 0.399, 0.399, 0.398, 0.400, 0.404, 0.408,
0.411, 0.413, 0.415, 0.413, 0.418, 0.419, 0.420, 0.424, 0.428, 0.429,
0.429, 0.429, 0.438, 0.435, 0.438, 0.439, 0.441, 0.440, 0.439, 0.447,
0.448, 0.452, 0.452, 0.454, 0.456, 0.458, 0.461, 0.462, 0.461, 0.465,
0.468, 0.470, 0.469, 0.471, 0.471, 0.478, 0.476, 0.480, 0.481, 0.485,
0.482, 0.492, 0.493, 0.495, 0.493, 0.491, 0.496, 0.501, 0.503, 0.504,
0.509, 0.509, 0.510, 0.510, 0.512, 0.515, 0.516, 0.514, 0.521, 0.520,
0.523, 0.524, 0.527, 0.526, 0.532, 0.533, 0.535, 0.540, 0.537, 0.541
};

const int M154=61;
string msg4(" (from DDBase T202B ASWA CTRA G11 G14 after edit-resets)");
int n154[] = {
240, 241, 242, 243, 244, 245, 246, 247, 248, 249,
250, 251, 252, 253, 254, 255, 256, 257, 258, 259,
260, 261, 262, 263, 264, 265, 266, 267, 268, 269,
270, 271, 272, 273, 274, 275, 276, 277, 278, 279,
280, 281, 282, 283, 284, 285, 286, 287, 288, 289,
290, 291, 292, 293, 294, 295, 296, 297, 298, 299,
300
};
double y154[] = {
0.095, 0.108, 0.088, 0.126, 0.195, 0.295, 0.305, 0.303, 0.305, 0.310,
0.312, 0.310, 0.309, 0.316, 0.313, 0.319, 0.321, 0.322, 0.319, 0.320,
0.320, 0.319, 0.323, 0.326, 0.325, 0.330, 0.328, 0.329, 0.330, 0.339,
0.337, 0.336, 0.344, 0.343, 0.343, 0.347, 0.346, 0.345, 0.344, 0.339,
0.345, 0.352, 0.349, 0.355, 0.357, 0.360, 0.360, 0.359, 0.363, 0.361,
0.357, 0.359, 0.362, 0.363, 0.364, 0.366, 0.366, 0.367, 0.374, 0.374,
0.375
};

int *n15,M15;
double *y15,w15[M152];
string msg;

// formats
#include "format.hpp"
format f103(10,3),f103e(10,3,2),f106(10,6),f133(13,3),f63(6,3);

// stuff for doIt
Vector<double> truth;
SRIFilter srif;

//------------------------------------------------------------------------------------
// test least squares using SRIFilter
int main(int argc, char **argv)
{
try {
   int iterationsLimit = 20;
   double convergenceLimit = 1.e-14;
   int i,j,k,M,N,dataset=2;
   double t,inputsig=-1.,inputtau=-1.;
   Vector<double> data;
   Vector<double> sol;
   Matrix<double> cov,partials;
   Namelist NL;

   if(argc > 1) {
      for(i=1; i<argc; i++) {
         //cout << "arg " << i << " is " << argv[i] << endl;
         j = atoi(argv[i]);
         if(j > 0)
            test = j;
         else if(argv[i][0] == '-') {
            if(argv[i][1] == 'v' || argv[i][1]=='V')
               verbose = true;
            else if(argv[i][1] == 't' || argv[i][1] == 'T') {
               i++;
               inputtau = atof(argv[i]);
            }
            else if(argv[i][1] == 's' || argv[i][1] == 'S') {
               i++;
               inputsig = atof(argv[i]);
            }
            else if(argv[i][1] == 'd' || argv[i][1] == 'D') {
               i++;
               dataset = atoi(argv[i]);
            }
         }
         else
            cout << "Unrecognized option: " << argv[i] << endl;
      }
      if(dataset < 1 || dataset > 4) {
         cout << "Error: -d [1234] only. Abort\n";
         return -1;
      }
   }
   else {
      cout << "Test least squares using class SRIFilter\n";
      cout << "  Usage tsrif <n> [-v] [-t tau] [-s sig] [-d n]" << endl;
      cout << "    where -v turns on verbose output inside SRIFilter" << endl;
      cout << "    and -t and -s are for choice 14 (see below)" << endl;
      cout << "    and -d is for choices 15 & 16, n=1,2,3,or4 (see below)" << endl;
      cout << "    and n is:" << endl;
      cout << "  1   simple 1-d linear problem" << endl;
      cout << "  2   simple 1-d linearized problem" << endl;
      cout << "  3   multi-dimensional non-linear problem" << endl;
      cout << "  4   a non-linear batch test; a ranging problem" << endl;
      cout << "  5   test 4 done sequentially" << endl;
      cout << "  6   a batch linear test with random partials and data" << endl;
      cout << "  7   same as 6, but sequential" << endl;
      cout << "  8   a very non-linear equation made linear" << endl;
      cout << "  9   direct linearized solution of test 8" << endl;
      cout << " 10   a 2-d ranging problem with a false minimum" << endl;
      cout << " 11   global warming trends - polynomial fit in batchs" << endl;
      cout << " 12   global warming trends - polynomial fit in one batch" << endl;
      cout << " 13   global warming trends - piecewise fit in one batch" << endl;
      cout << " 14   global warming trends - piecewise fit constrained by "
            << "Gauss-Markov process\n         characterized by sigma and tau "
            << "(input or defaults: tau=50,sig=0.1)." << endl;
      cout << " 15   weighted LS using DD phase data (for dataset n use -d n)"<< endl;
      cout << " 16   robust LS using DD phase data of problem 15" << endl;
      return 0;
   }

   cout << "\n========================= Test " << test
      <<    " =========================" << endl;
   if(test == 1) {
      cout << " linear model : \n    f(X) = [ x0 + x1*t + x2*t*t]" << endl;
      cout << " partials     : \n         = [  1 ,    t ,    t*t]" << endl;
      cout << " add 0.025 gaussian noise to data" << endl;
      M=1; batch=10; N=3;
      data.resize(M*batch);
      truth.resize(N); truth(0) = 1.0; truth(1) = 2.0; truth(2) = 4.0;

      NL += "x0"; NL += "x1"; NL += "x2";
      srif = SRIFilter(NL);
      srif.doLinearize = false;
      sol.resize(N); sol = 1.5;
      for(i=0; i<batch; i++) {
         t = i * 0.32;
         data(M*i) = RandNorm(0.025) + truth(0) + truth(1)*t + truth(2)*t*t;
      }
      doIt(data,sol,cov);
   }
   else if(test == 2) {
      cout << " linearized :\n   f(X) = [ x0*sin(t) + sin(x1)*cos(t) ]" << endl;
      cout << " partials   :\n          [    sin(t),  cos(x1)*cos(t) ]" << endl;
      cout << " add 0.025 gaussian noise to data" << endl;
      M=1; batch=20; N=2;
      data.resize(M*batch);
      truth.resize(N); truth(0) = 1.0; truth(1) = -0.5;

      NL += "x0"; NL += "x1";
      srif = SRIFilter(NL);
      srif.doLinearize = true;
      sol.resize(N); sol = -1.;
      for(i=0; i<batch; i++) {
         t = i * 0.32;
         data(M*i) = RandNorm(0.025) + truth(0)*sin(t) + sin(truth(1))*cos(t);
      }
      doIt(data,sol,cov);
   }
   else if(test == 3) {
      cout << " A multi-dimensional non-linear problem" << endl;
      cout << "        [ cos(x1)*sin(t) - 10*sin(x2)*cos(t)  ]" << endl;
      cout << " f(X) = [ 2*sin(x0)*cos(t) + 4*cos(x3)        ]" << endl;
      cout << "        [ x0*sin(x1)*t*t - x2*cos(x3)*tan(t)  ]" << endl;
      cout << " partials = " << endl;
      cout<<" [0             -sin(x1)*sin(t) -10*cos(x2)*cos(t)  0             ]"
         << endl;
      cout<<" [2*cos(x0)*cos(t) 0            0                -4*sin(x3)       ]"
         << endl;
      cout<<" [sin(x1)*t*t   x0*cos(x1)*t*t  -cos(x3)*tan(t)  x2*sin(x3)*tan(t)]"
         << endl;
      M=3; batch=10; N=4;
      data.resize(M*batch);
      sol.resize(N);
      truth.resize(N);
      truth(0) = 1.0;   sol(0) = 1.1;
      truth(1) = -2.0;  sol(1) = -2.1;
      truth(2) = 3.0;   sol(2) = 3.1;
      truth(3) = -4.0;  sol(3) = -4.1;
         // generate fake data with noise
      for(i=0; i<batch; i++) {
         t = i * 0.32;
         data(M*i)   = RandNorm(.005) + cos(truth(1))*sin(t)
                                    - 10*sin(truth(2))*cos(t);
         data(M*i+1) = RandNorm(.025) + 2*sin(truth(0))*cos(t) + 4*cos(truth(3));
         data(M*i+2) = RandNorm(.015) + truth(0)*sin(truth(1))*t*t
                                    - truth(2)*cos(truth(3))*tan(t);
      }

      NL += "x0"; NL += "x1"; NL += "x2"; NL += "x3";
      srif = SRIFilter(NL);
      srif.doLinearize = true;

      doIt(data,sol,cov);
   }
   else if(test == 4) {
      cout << " a non-linear batch test; a ranging problem" << endl;
      cout << " f(X) = [ sqrt(d0*d0 + d1*d1 + d2*d2) - d3 ]" << endl;
      cout << " where di = XSV[j][i]-xi for each of 4 sats j" << endl;
      cout << endl;
      cout << " partials = [ d0/r  d1/r  d2/r  -1 ]" << endl;
      cout << " where r = 1/sqrt(d0*d0 + d1*d1 + d2*d2)" << endl;
      cout << "" << endl;
      cout << " add gaussian noise at 0.01 to the data" << endl;
      M=4; batch=5; N=4;
      data.resize(M*batch);
      sol.resize(N);
      truth.resize(N);
      partials.resize(M*batch,N);
      truth(0) = 1.0;
      truth(1) = -2.0;
      truth(2) = 3.0;
      truth(3) = -4.0;
      sol = 0.0;
         // generate fake data with noise
      LSFunc(truth,data,partials);
      for(i=0; i<M*batch; i++) data(i) += RandNorm(0.01);

      NL += "X"; NL += "Y"; NL += "Z"; NL += "cdT";
      srif = SRIFilter(NL);
      srif.doLinearize = true;

      doIt(data,sol,cov);
   }
   else if(test == 5) {
      cout << " do test 4 again but sequentially, one batch at a time.\n";
      //cout << " add normal noise at 0.001 to the data\n";
      cout << " this problem is very sensitive to the noise on the data\n";
      cout << endl;
      cout << " NB doing this is very different than the batch solution when\n";
      cout << " doLinearize is set. In this case the solution depends strongly on\n";
      cout << " the initial nominal solution: bad X0 => bad Inf0 => bad X1 => ...\n";
      cout << " A problem like this is best handled with a very good initial X,\n";
      cout << " or a big batch for first sequential step.\n";
      cout << " model\n";
      cout << "       f(X) = [ sqrt(d0*d0 + d1*d1 + d2*d2) - d3 ]\n";
      cout << "                where di = XSV[j][i]-xi for each of 4 sats j\n";
      cout << "       partials = [ d0/r  d1/r  d2/r  -1 ]\n";
      cout << "                where r = 1/sqrt(d0*d0 + d1*d1 + d2*d2)\n";
      M=4; N=4; batch=5;
      data.resize(M);
      sol.resize(N);
      truth.resize(N);
      partials.resize(M,N);
         // true solution
      truth(0) = 1.0;
      truth(1) = -2.0;
      truth(2) = 3.0;
      truth(3) = -4.0;
         // need a good starting point - let it be solution from test 4
      sol(0) = 0.964780;
      sol(1) = -1.974532;
      sol(2) = 2.936646;
      sol(3) = -4.058416;

      NL += "X"; NL += "Y"; NL += "Z"; NL += "cT";
      srif = SRIFilter(NL);
      srif.doLinearize = true;
      srif.doSequential = true;

      for(batch=0; batch<5; batch++) {
         cout << "\n------ Batch = " << batch << " ------" << endl;
            // generate fake data with noise
         LSFunc(truth,data,partials);
         //for(i=0; i<M; i++) data(i) += RandNorm(0.001);
         doIt(data,sol,cov);
      }
   }
   else if(test == 6 || test == 7) {
      batch=5;
      M=4; N=4;
      sol.resize(N);
      truth.resize(N);
         // true solution - arbitrary: it will be used to generate data
      truth(0) = 1.0;
      truth(1) = -2.0;
      truth(2) = 3.0;
      truth(3) = -4.0;
         // P6 is used to generate data
      P6.resize(M,N);
      for(i=0; i<M; i++) for(j=0; j<N; j++) P6(i,j)=RandNorm(10.0);
      cout << " Random partials matrix P6 is\n" << P6 << endl;
         // generate fake data with noise
      D6.resize(batch*M);
      partials.resize(M*batch,N);
      i = test; test = 6; LSFunc(truth,D6,partials); test = i;
      for(i=0; i<M*batch; i++) D6(i) += RandNorm(1.0);

      NL += "A"; NL += "B"; NL += "C"; NL += "D";
      srif = SRIFilter(NL);
      srif.doLinearize = false;
      sol = 0.0;

      if(test == 6) {
         cout << " a batch linear test using random data (same as 7):\n";
         srif.doSequential = false;
         data.resize(M*batch);
         data = D6;
         doIt(data,sol,cov);
      }
      else if(test == 7) {
         cout << " a sequential linear test using random data (same as 6):\n";
         srif.doSequential = true;
         data.resize(M);
         partials.resize(M,N);
         for(batch=0; batch<5; batch++) {
            cout << "\n ------------- Batch = " << batch << " --------------" << endl;
            for(i=0; i<M; i++) data(i) = D6(batch*M+i);
            doIt(data,sol,cov);
         }
      }
   }
   else if(test == 8) {
      cout << " solve this very non-linear equation:\n";
      cout << " L = exp(x0) * n^x1 * w^x2 * d^x3 * D^x4\n";
      cout << "  by making it linear:\n";
      cout << " log(L) = x0 + x1*log(n) + x2*log(w) + x3*log(d) + x4*log(D)\n";
      cout << " (test 9 will solve the non-linear equation.)\n";
      cout << endl;
      M=50; N=5;
      data.resize(M);
      partials.resize(M,N);
      sol.resize(N);
      truth.resize(N);
      for(i=0; i<M; i++) data(i) = log(L8[i]);
      sol = 0.0;
      // I don't have the real truth...
      truth(0) = -7.254122;
      truth(1) = 1.379365;
      truth(2) = -0.480604;
      truth(3) = 0.275616;
      truth(4) = 1.213172;

      NL += "x0"; NL += "x1"; NL += "x2"; NL += "x3"; NL += "x4";
      srif = SRIFilter(NL);
      srif.doLinearize = false;
      srif.doSequential = false;

      doIt(data,sol,cov);
   }
   else if(test == 9) {
      cout << " solve this very non-linear equation:\n";
      cout << " L = exp(x0) * n^x1 * w^x2 * d^x3 * D^x4\n";
      cout << " (test 8 solves the log of this equation (linear).)\n";
      cout << endl;
      M=50; N=5;
      partials.resize(M,N);
      sol.resize(N);
      truth.resize(N);
      data.resize(M);
      data = L8;
      // I don't have the real truth...
      truth(0) = -7.254122;
      truth(1) = 1.379365;
      truth(2) = -0.480604;
      truth(3) = 0.275616;
      truth(4) = 1.213172;
      sol = truth; // 0.0;

      NL += "x0"; NL += "x1"; NL += "x2"; NL += "x3"; NL += "x4";
      srif = SRIFilter(NL);
      srif.doLinearize = true;
      srif.doSequential = false;

      doIt(data,sol,cov);
   }
   else if(test == 10) {
      cout << " a 2-d ranging problem with a false minimum.\n";
      cout << " generate data from truth, adding 0.002 gaussian noise.\n";
      cout << " look at the plot, which is the potential well.\n";
      cout << " it has a minimum at (1,1), which is the true solution,\n";
      cout << " but also another local minimum, at (2.81,2.46).\n";
      cout << " try starting a (0,0), then again starting at (4,4)\n";
      cout << endl;
      M=5; N=2;
      Vector<double> f(M),D(M);
      data.resize(M);
      truth.resize(N);
      sol.resize(N);
      partials.resize(M,N);
      truth(0) = truth(1) = 1.0;
         // generate the data from truth + noise
      LSFunc(truth,data,partials);

         // generate a contour plot of the data
      while(1) {
         double x,y,z,r;
         ofstream ofs("tsrif10.dat",ios::out);
         if(!ofs) {
            cout << " Could not open tsrif10.dat .. abort contour plot\n";
            break;
         }
         for(i=0; i<40; i++) {
            sol(0) = x = i*0.10;
            for(j=0; j<40; j++) {
               sol(1) = y = j*0.10;
               LSFunc(sol,f,partials);
               z = norm(f-data);    // ie RSS
               ofs << f63 << x << " " << f63 << y << " " << f63 << z*z << endl;
            }
            ofs << "  " << endl;    // gnuplot wants a blank line
         }
         ofs.close();
         cout << " Created file tsrif10.dat\n";

         ofs.open("tsrif10.gp",ios::out);
         if(!ofs) {
            cout << " Could not open tsrif10.gp .. abort contour plot\n";
            break;
         }
         ofs << "set title \"tsrif 10 - ranging problem with false minimum\"\n";
         ofs << "set xlabel \"X\"\n";
         ofs << "set ylabel \"Y\"\n";
         ofs << "set zlabel \"Potential = |f-d|^2\"\n";
         ofs << "#set xrange [40:60]\n";
         ofs << "#set yrange [250:290]\n";
         ofs << "#set zrange [0:12]\n";
         ofs << "#set cbrange [0:12]\n";
         ofs << "set pm3d\n";
#ifndef _WIN32
         ofs << "set term x11 enhanced font \"luxi sans,17\"\n";  // linux only
#endif
         ofs << "unset key\n";
         ofs << "set data style lines\n";
         ofs << "set contour base\n";
         ofs << "set cntrparam levels incremental 0.0,0.25,12.0\n";
         ofs << "# comment out next two to see only contours\n";
         ofs << "set hidden3d\n";
         ofs << "set view 60,45\n";
         ofs << "# un-comment out next two to see only contours\n";
         ofs << "#set nosurface\n";
         ofs << "#set view 0,0\n";
         ofs << "set label 1 \"potential well - note true minimum at (1,1) "
             << "and local minimum at (2.81,2.46)\" at screen 0.5,0.88 center\n";
         ofs << "splot \"tsrif10.dat\" using 1:2:3\n";
         ofs.close();
         cout << " Created file tsrif10.gp .. try gnuplot tsrif10.gp\n\n";
         break;
      }

      for(k=0; k<5; k++) data(k) += RandNorm(0.002);
      D = data;      // save

      NL += "X"; NL += "Y";
      srif = SRIFilter(NL);
      srif.doLinearize = true;
      srif.doSequential = false;

      // starting value
      sol(0)   = sol(1)   = 0.0;
      doIt(data,sol,cov);
      cout << endl << " ---------------------------------------------------------\n"
         << " do it again, starting at (4,4) to get the false minimum.\n";
      sol(0)   = sol(1)   = 4.0;
      data = D;
      srif.zeroAll();
      srif.iterationsLimit = 40;
      doIt(data,sol,cov);
   }
   else if(test == 11) {
      //{
      //   ifstream ifs("globaltemp.data");
      //   if(!ifs) {
      //      cout << "could not open globaltemp.data .. abort\n";
      //      return 0;
      //   }
      //   const int BUFF_SIZE=100;
      //   char buffer[BUFF_SIZE];
      //   int year=0;
      //   double temp;
      //   vector<int> years;
      //   vector<double> temps;
      //   while(ifs.getline(buffer,BUFF_SIZE)) {
      //      if(buffer[0]=='#') continue;
      //      string line=buffer;
      //      StringUtils::stripTrailing(buffer,'\r');
      //      string::size_type p;
      //      vector<string> fs;
      //      while(line.size() > 0) {
      //         p = line.find(" ");
      //         if(p==string::npos) p=line.size();
      //         if(p != 0) fs.push_back(line.substr(0,p));
      //         if(p >= line.size()) break;
      //         line.erase(0,p+1);
      //      }
      //      if(StringUtils::asInt(fs[0]) == year) continue;    // percentages line
      //      year = StringUtils::asInt(fs[0]);
      //      temp = StringUtils::asDouble(fs[13]);
      //      years.push_back(year);
      //      temps.push_back(temp);
      //      cout << year << " " << fixed << setprecision(3) << setw(6)
      //         << temp << endl;
      //   }
      //   ifs.close();
      //   for(i=0; i<years.size(); i++) cout << "," << years[i]; cout << endl;
      //   for(i=0; i<temps.size(); i++) cout << "," << temps[i]; cout << endl;
      //}
      cout << " a simple but noisy polynomial fit" << endl;
      cout << " to global temperature anomaly data." << endl;
      cout << " do it in 8 batches (7 of 20, then 1 of 9) points each." << endl;
      cout << " cf. www.cru.uea.ac.uk/cru/data/temperature" << endl;
      NL += "c0"; NL += "c1"; NL += "c2"; NL += "c3"; NL += "c4"; NL += "c5";
      N = NL.size();
      M = batchlen = batchsize = 20;
      Vector<double> f(M);
      Matrix<double> Coef(8,N);  // save the results from each batch
      data.resize(M);
      truth.resize(N);
      sol.resize(N);
      partials.resize(M,N);
      truth = 0.0;         // don't know it

      srif = SRIFilter(NL);
      srif.doLinearize = false;
      srif.doSequential = true;

      sol = 0.0;
      Coef = 0.0;
      for(batch=0; batch<8; batch++) {
         if(batch == 7) {
            batchlen = 9;
            data.resize(batchlen);         // actually not necessary...
         }
         for(i=0; i<batchlen; i++) data(i) = temps11[batch*batchsize+i];
         cout << "\n -------- Batch " << batch+1 << " --------" << endl;
         doIt(data,sol,cov);
         for(i=0; i<N; i++) Coef(batch,i)=sol(i);
      }

      // print out the results, and generate a plot
      cout << "\n Coefficients (batch vs coefficients) :\n" << fixed
         << setw(10) << setprecision(3) << Coef << endl;

      ofstream ofs("tsrif11.dat",ios::out);
      if(!ofs) {
         cout << " Could not open tsrif11.dat .. abort plot\n";
      }
      else {
         // print all the data and all the solutions -- do in one big batch
         batch = 0;         // this necessary for LSFunc to operate correctly
         M = batchlen = batchsize = 149;
         Matrix<double> F(M,8);
         f.resize(M);
         partials.resize(M,N);
         for(j=0; j<8; j++) {
            sol = Coef.rowCopy(j);              // pick out the jth solution
            LSFunc(sol,f,partials);             // evaluate at all 149 times
            //for(i=0; i<M; i++) F(i,j)=f(i);   // use slice instead
            //                    (matrix,col index,slice=(start,length,stride))
            MatrixColSlice<double> Fcol(F,j,std::slice(0,M,1));
            Fcol = f;                           // copy into (the slice of) F
            //MatrixRowSlice<double> Frow(F,17,std::slice(0,7,1));
            //cout << "Col slice is\n" << Fcol << endl;
            //cout << "Row slice is\n" << Frow << endl;
         }
         // print all the evaluated polynomials at all the times
         for(i=0; i<M; i++) {
            // times and data
            ofs << setw(4) << years11[i] << " " << f63 << temps11[i];
            // solutions
            for(j=0; j<8; j++) ofs << " " << f63 << F(i,j);
            ofs << endl;
         }
         ofs.close();
         cout << " Created tsrif11.dat" << endl;
         ofs.open("tsrif11.gp",ios::out);
         if(!ofs) {
            cout << " Could not open tsrif11.gp .. abort plot\n";
         }
         else {
            ofs << "set title \"Global temperature anomaly - "
               << "sequential fits of order " << N-1 << " in 7 batches of 20 points "
               << "and 1 of 9 ... tsrif(11)\"\n";
            ofs << "set xlabel \"Year\"\n";
            ofs << "set ylabel \"Temperature anomaly\"\n";
            ofs << "unset mouse\n";
#ifndef _WIN32
            ofs << "set term x11 enhanced font \"luxi sans,17\"\n";  // linux only
#endif
            ofs << "set key left\n";
            ofs << "#set xrange [40:60]\n";
            ofs << "set yrange [-0.6:0.6]\n";
            ofs << "set style line 1 lt 8 lw 2\n";
            ofs << "plot \"tsrif11.dat\" using 1:2 t \"dT\" with points\n";
            ofs << "replot \"tsrif11.dat\" using 1:3 t \"fit1\" with lines\n";
            ofs << "replot \"tsrif11.dat\" using 1:4 t \"fit2\" with lines\n";
            ofs << "replot \"tsrif11.dat\" using 1:5 t \"fit3\" with lines\n";
            ofs << "replot \"tsrif11.dat\" using 1:6 t \"fit4\" with lines\n";
            ofs << "replot \"tsrif11.dat\" using 1:7 t \"fit5\" with lines\n";
            ofs << "replot \"tsrif11.dat\" using 1:8 t \"fit6\" with lines\n";
            ofs << "replot \"tsrif11.dat\" using 1:9 t \"fit7\" with lines\n";
            ofs << "replot \"tsrif11.dat\" using 1:10 t \"fit8\" with lines ls 1\n";
            ofs.close();
            cout << " Created file tsrif11.gp .. try gnuplot tsrif11.gp\n";
         }
      }
   }
   else if(test == 12) {
      cout << " a simple but noisy polynomial fit";
      cout << " to global temperature anomaly data." << endl;
      cout << " do it in one big batch of 149 points,";
      cout << " and plot the data." << endl;
      cout << " cf. www.cru.uea.ac.uk/cru/data/temperature" << endl;
      NL += "c0"; NL += "c1"; NL += "c2"; NL += "c3"; NL += "c4"; NL += "c5";
      //NL += "c6"; NL += "c7"; NL += "c8";
      //NL += "c9"; NL += "c10"; NL += "c11";
      batchlen=batchsize=M=149; N=NL.size();
      Vector<double> f(M);
      data.resize(M);
      truth.resize(N);
      sol.resize(N);
      partials.resize(M,N);
      truth = 0.0;         // don't know it

      srif = SRIFilter(NL);
      srif.doLinearize = false;
      srif.doSequential = false;

      sol = 0.0;
      batch = 0;     // for LSFunc
      for(i=0; i<M; i++) data(i) = temps11[i];
      doIt(data,sol,cov);

      ofstream ofs("tsrif12.dat",ios::out);
      if(!ofs) {
         cout << " Could not open tsrif12.dat .. abort plot\n";
      }
      else {
         LSFunc(sol,f,partials);
         for(i=0; i<M; i++) {
            ofs << setw(4) << years11[i]
               << " " << f63 << temps11[i]
               << " " << f63 << f[i] << endl;
         }
         ofs.close();
         cout << " Created tsrif12.dat" << endl;
         ofs.open("tsrif12.gp",ios::out);
         if(!ofs) {
            cout << " Could not open tsrif12.gp .. abort plot\n";
         }
         else {
            ofs << "set title \"Global temperature anomaly - fit of order "
               << N-1 << " tsrif(12)\"\n";
            ofs << "set xlabel \"Year\"\n";
            ofs << "set ylabel \"Temperature anomaly\"\n";
            ofs << "unset mouse\n";
#ifndef _WIN32
            ofs << "set term x11 enhanced font \"luxi sans,17\"\n";  // linux only
#endif
            ofs << "set key left\n";
            ofs << "#set xrange [40:60]\n";
            ofs << "#set yrange [250:290]\n";
            ofs << "plot \"tsrif12.dat\" using 1:2 t \"dT\" with points\n";
            ofs << "replot \"tsrif12.dat\" using 1:3 t \"fit\" with lines\n";
            ofs.close();
            cout << "\n Created file tsrif12.gp .. try gnuplot tsrif12.gp\n";
         }
      }
   }
   else if(test == 13 || test == 14) {
         // see discussion  below under test == 14
      double tau=50.0,sig=0.1,B=0.0;       // data residuals are about 0.1
         // input may come from command line
      if(inputsig > 0) sig=inputsig;
      if(inputtau > 0) tau=inputtau;

      cout << "Try fitting the global temperature data of options 11 and 12\n";
      cout << "another way. Divide the timeline into N equal parts. In each part,\n";
      cout << "fit the data to a constant (13). Then do it again (14), but with\n";
      cout << "a priori information which correlates the solution elements\n";
      cout << "using a first order Markov (random walk) process.\n";

      NL += "c0"; NL += "c1"; NL += "c2"; NL += "c3"; NL += "c4"; NL += "c5";
      NL += "c6"; NL += "c7"; NL += "c8"; NL += "c9";
      N = NL.size();

      batchlen = batchsize = M = 150;
      data.resize(M);
      truth.resize(N);
      sol.resize(N);
      partials.resize(M,N);
      for(i=0; i<N; i++) {
         truth(i) = 0.0;
         for(int j=i*(150/N); j<(i+1)*(150/N); j++)
            truth(i) += temps11[j];
         truth(i) /= (150/N);
      }

      srif = SRIFilter(NL);
      srif.doLinearize = false;
      srif.doSequential = false;

      sol = 0.0;
      batch = 0;     // for LSFunc
      for(i=0; i<M; i++) data(i) = temps11[i];

      if(test == 13) cout << " Don't correlate the state elements" << endl;
      if(test == 14) {
         cout << " Add correlation to the state elements" << endl;
         // State = 0
         // Cov = (sig2,B2=const, e=exp(-dt/tau) where dt is
         //                                the time spacing of the state elements....
         // [   1    e    e^2   ]
         // [   e    1    e     ] * sig2 + B2
         // [   e^2  e    1     ]
         // sig2 = variance = (sigma)^2, sigma = uncertainty on state element
         // tau = time constant => stiffer for larger tau
         double d,ex;
         ex = exp(-(M/N)/tau);             // dt = M/N years
         Matrix<double> apCov(N,N);
         Vector<double> apSt(N,0.0);       // apriori state is the 'expected' values
         ident(apCov);
         for(i=0; i<N-1; i++) {            // row i
            d = ex;
            for(j=i+1; j<N; j++) {         // col j
               apCov(i,j) = apCov(j,i) = d;
               d *= ex;
            }
         }
         apCov *= sig*sig;
         //apCov += B*B;

         LabelledMatrix LC(NL,apCov);
         LC.setw(10).setprecision(6);
         LC.message(" apCov");
         cout << LC << endl;

         srif.addAPriori(apCov,apSt);
      }

      doIt(data,sol,cov);

      ofstream ofs("tsrif13.dat",ios::out);
      if(!ofs) {
         cout << " Could not open tsrif13.dat .. abort plot\n";
      }
      else {
         for(i=0; i<M; i++) {
            k = i/(M/N);
            ofs << setw(4) << years11[i]
               << " " << f63 << temps11[i]
               << " " << f63 << sol(k) << endl;
         }
         ofs.close();
         cout << " Created tsrif13.dat" << endl;
         ofs.open("tsrif13.gp",ios::out);
         if(!ofs) {
            cout << " Could not open tsrif13.gp .. abort plot\n";
         }
         else {
            ofs << "set title \"Global temperature anomaly : piecewise";
            if(test==13) ofs << " fit (13)\"\n";
            if(test==14) ofs << " constrained fit, tau="
               << setprecision(1) << tau << "yrs, sig="
               << setprecision(3) << sig << "C"
               << " (14)\"\n";
            ofs << "set xlabel \"Year\"\n";
            ofs << "set ylabel \"Temperature anomaly (deg C)\"\n";
            ofs << "unset mouse\n";
#ifndef _WIN32
            ofs << "set term x11 enhanced font \"luxi sans,17\"\n";  // linux only
#endif
            ofs << "set key left\n";
            ofs << "#set xrange [40:60]\n";
            ofs << "#set yrange [250:290]\n";
            ofs << "plot \"tsrif13.dat\" using 1:2 t \"dT\" with points\n";
            ofs << "replot \"tsrif13.dat\" using 1:3 t \"fit\" with linespoints\n";
            ofs.close();
            cout << "\n Created file tsrif13.gp .. try gnuplot tsrif13.gp\n";
         }
      }
   }
   else if(test == 15 || test == 16) {
      cout << "Dataset is " << dataset << endl;
      if(dataset==1) { msg=msg1; n15 = &n151[0]; y15 = &y151[0]; M15=M151; }
      if(dataset==2) { msg=msg2; n15 = &n152[0]; y15 = &y152[0]; M15=M152; }
      if(dataset==3) { msg=msg3; n15 = &n153[0]; y15 = &y153[0]; M15=M153; }
      if(dataset==4) { msg=msg4; n15 = &n154[0]; y15 = &y154[0]; M15=M154; }
      NL += "Bias"; NL += "Linear"; NL += "Quad";
      N=NL.size();
      batchlen=batchsize=M=M15;
      if(test == 15) cout << " a weighted polynomial fit";
      if(test == 16) cout << " a robust polynomial fit";
      cout << " to " << M  << " double difference phase data points." << endl;
      cout << msg << endl;

      Vector<double> f(M);
      Vector<double> res,wt(M);
      Stats<double> stat;
      PolyFit<double> PF;

      data.resize(M);
      truth.resize(N);
      sol.resize(N);
      partials.resize(M,N);
      truth = 0.0;         // don't know it

      // configure srif
      srif = SRIFilter(NL);
      srif.iterationsLimit = 20;
      srif.doLinearize = false;
      srif.doSequential = false;

      // ---------- do it robust
      if(test == 16) {
         srif.doRobust = true;
         srif.doWeight = false;
         srif.convergenceLimit = 1.e-2;
      }
      // ---------- OR weight it
      if(test == 15) {
         srif.doRobust = false;
         srif.doWeight = true;
         // compute mest of y15 to get weights
         double median,mad,mest;
         mad = Robust::MAD(y15,M,median,true);
         mest = Robust::MEstimate(y15,M,median,mad,w15);
         cov.resize(M,M);
         ident(cov);
         for(i=0; i<M; i++) cov(i,i) = 1.0/(w15[i]*w15[i]); // meas. cov.
         //Robust::StemLeafPlot(cout, y15, M, "raw dd phase data");
      }

      // initialize for doit()
      sol = 0.0;
      batch = 0;     // for LSFunc
      for(i=0; i<M; i++) {
         f(i) = double(n15[i]);     // f used as a temp, for PolyFit PF
         data(i) = y15[i];
      }

      // first compute regular polynomial fit to data, and compute statistics
      PF.Reset(3);
      PF.Add(data,f);
      res = data - PF.Evaluate(f);
      stat.Reset();
      stat.Add(res);
      cout << " Initial raw statistics on residuals of fit:\n "
         << scientific << setprecision(3) << stat << endl;

      // least squares
      doIt(data,sol,cov);

      // compute final weighted stats on residuals of fit
      LSFunc(sol,f,partials);    // f will be the fit evaluted at each point
      if(test == 15) {
         res = data; // leastSquares returns residuals in data Vector
         for(i=0; i<M; i++) {
            data(i)=y15[i];
            wt(i)=w15[i];
         }
      }
      if(test == 16) {
         wt = data;  // leastSquares(robust) returns weights in data Vec
         for(i=0; i<M; i++) data(i)=y15[i];
         res = data-f;
      }

      //cout << "Weights: " << wt << endl;
      stat.Reset();
      stat.Add(res,wt);
      cout << " Final weighted statistics on residuals of fit:\n "
         << scientific << setprecision(3) << stat << endl;

      // plot
      string filename;
      filename = "tsrif" + StringUtils::asString(test) + ".dat";
      ofstream ofs(filename.c_str(),ios::out);
      if(!ofs) {
         cout << " Could not open " << filename << " .. abort plot\n";
      }
      else {
         for(i=0; i<M; i++) {
            ofs << fixed << setw(4) << n15[i]
               << " " << f63 << data[i]
               << " " << f63 << f[i]
               << " " << f63 << res[i]
               << " " << f63 << wt[i]
               << " " << f63 << fabs(res[i])/stat.StdDev()
               << endl;
         }
         ofs.close();
         cout << " Created " << filename << endl;
         filename = "tsrif" + StringUtils::asString(test) + ".gp";
         ofs.open(filename.c_str(),ios::out);
         if(!ofs) {
            cout << " Could not open " << filename << " .. abort plot\n";
         }
         else {
            if(test == 15) ofs << "set title \"DD Phase data - wt'd fit of order "
               << N-1 << " tsrif(15," << dataset << ")\\n(wts from m-est of data)\"\n";
            if(test == 16) ofs << "set title \"DD Phase data - robust fit of order "
               << N-1 << " tsrif(16," << dataset << ")\"\n";
            ofs << "set xlabel \"Count\"\n";
            ofs << "set ylabel \"DDPhase(m)\"\n";
            ofs << "unset mouse\n";
#ifndef _WIN32
            ofs << "set term x11 enhanced font \"luxi sans,17\"\n";  // linux only
#endif
            ofs << "set key bottom right\n";
            ofs << "set autoscale y2\n";
            ofs << "set ytics nomirror\n";
            ofs << "set y2tics\n";
            if(test == 15) ofs << "set y2label \"Residual (m)\"\n";
            if(test == 16) ofs << "set y2label \"Weight\"\n";
            ofs << "#set xrange [40:60]\n";
            ofs << "#set yrange [250:290]\n";
            ofs << "plot \"tsrif" << test
               << ".dat\" using 1:2 t \"DDPh\" with points\n";
            ofs << "replot \"tsrif" << test
               << ".dat\" using 1:3 t \"fit\" with lines\n";
            ofs << "replot \"tsrif" << test
               << ".dat\" using 1:4 axes x1y2 t \"res\" with linespoints\n";
            ofs << "replot \"tsrif" << test
               << ".dat\" using 1:5 axes x1y2 t \"wt\" with linespoints\n";
            ofs.close();
            cout << "\n Created file tsrif" << test
               << ".gp .. try gnuplot tsrif" << test << ".gp\n";
         }
      }

      if(test == 16) {
         QSort(&wt[0],M);
         Robust::StemLeafPlot(cout, &wt[0], M, "weights");
      }
   }
   else {
      cout << " ... not implemented\n";
   }

   return 0;
}
catch(gpstk::Exception& e) {
   cerr << "tsrif caught an exception\n" << e << endl;
   return 0;
}
}

//------------------------------------------------------------------------------------
void doIt(Vector<double>& data, Vector<double>& sol, Matrix<double>& cov)
{
try {
   int i,pre=6,wid=12;
   format fmts(wid,pre,2);

   srif.doVerbose = verbose;
   cout << " Start at x = (" << fixed;
   for(i=0; i<sol.size(); i++) cout << (i==0 ? "":",") << sol[i];
         cout << ")" << endl;
   //cout << " Data is (" << data.size() << "):" << setprecision(pre) << data << endl;

   i = srif.leastSquaresEstimation(data,sol,cov,&LSFunc);
   if(i) cout << " LS failed (" << i << ") "
         << (i==-1 ? "Underdetermined" :
            (i==-2 ? "Singular" : 
            (i==-3 ? "Failed to converge" : "Diverged")))
         << endl;
   cout << " SRIFilter is" << (srif.isValid() ? "":" not") << " valid" << endl;

   Namelist NL=srif.getNames();
   LabelledVector LT(NL,truth);
   LT.setw(wid).setprecision(pre);
   LT.message(" Truth:");
   cout << LT << endl;
   Vector<double> delta(sol-truth);
   LabelledVector LR(NL,delta);
   LR.setw(wid).setprecision(pre);
   LR.message(" Residuals:");
   cout << LR << endl;
   cout << " RMS residuals of fit: " << fmts << RMS(data) << endl;
   LabelledMatrix LC(NL,cov);
   LC.setw(wid).setprecision(pre);
   LC.message(" Covariance:");
   cout << LC << endl;
   cout << " Condition number is " << fmts << srif.ConditionNumber() << endl;
   if(srif.doLinearize || srif.doRobust) {
      cout << " There were " << srif.Iterations() << " iterations,";
      cout << " and convergence was " << fmts << srif.Convergence() << endl;
   }
   //cout << " Data residuals (" << data.size() << ") : " << scientific
      //<< setprecision(pre) << data << endl;
}
catch(gpstk::Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
void LSFunc(Vector<double>& X, Vector<double>& f, Matrix<double>& P)
{
try {
   double t;
   if(test == 1) {
      // f(X) = [ x0 + x1*t + x2*t*t]
      // partials = [ 1  t  t*t ]
      // t = i * 0.32;
      int M=1;
      for(int i=0; i<batch; i++) {
         t = i * 0.32;
         f(M*i) = 0.0;  // its linear X(0) + X(1)*t + X(2)*t*t;
         P(M*i,0)=1.0;  P(M*i,1)=t;  P(M*i,2)=t*t;
      }
   }
   else if(test == 2) {
      // f(X) = [ x0*sin(t) + sin(x1)*cos(t) ]
      // partials = [ sin(t)   cos(x1)*cos(t) ]
      // t = i * 0.32;
      int M=1;
      for(int i=0; i<batch; i++) {
         t = i * 0.32;
         f(M*i) = X(0)*sin(t) + sin(X(1))*cos(t);
         P(M*i,0)=sin(t);  P(M*i,1)=cos(X(1))*cos(t);
      }
   }
   else if(test == 3) {
      //        [ cos(x1)*sin(t) - 10*sin(x2)*cos(t)  ]
      // f(X) = [ 2*sin(x0)*cos(t) + 4*cos(x3)        ]
      //        [ x0*sin(x1)*t*t - x2*cos(x3)*tan(t)  ]
      //
      // partials = 
      // [ 0              -sin(x1)*sin(t)  -10*cos(x2)*cos(t) 0              ]
      // [ 2*cos(x0)*cos(t)  0               0                -4*sin(x3)     ]
      // [ sin(x1)*t*t    cos(x1)*t*t      -cos(x3)*tan(t)    sin(x3)*tan(t) ]
      //
      int M=3;
      for(int i=0; i<batch; i++) {     // loop over batch
         t = i * 0.32;
         f(M*i)   = cos(X(1))*sin(t) - 10*sin(X(2))*cos(t);
         f(M*i+1) = 2*sin(X(0))*cos(t) + 4*cos(X(3));
         f(M*i+2) = X(0)*sin(X(1))*t*t - X(2)*cos(X(3))*tan(t);

         P(M*i+0,0)=0.0;
         P(M*i+0,1)=-sin(X(1))*sin(t);
         P(M*i+0,2)=-10.0*cos(X(2))*cos(t);
         P(M*i+0,3)=0.0;

         P(M*i+1,0)=2*cos(X(0))*cos(t);
         P(M*i+1,1)=0.0;
         P(M*i+1,2)=0.0;
         P(M*i+1,3)=4.0;

         P(M*i+2,0)=sin(X(1))*t*t;
         P(M*i+2,1)=X(0)*cos(X(1))*t*t;
         P(M*i+2,2)=-cos(X(3))*tan(t);
         P(M*i+2,3)=X(2)*sin(X(3))*tan(t);
      }
   }
   else if(test == 4) {
      double x,y,z,r,tb,th,ph;
      for(int i=0; i<batch; i++) {     // loop over batch
         t = i * 0.32;
         for(int j=0; j<4; j++) {      // loop over satellites
            th = Theta[j]+OmegaTheta[j]*t;
            ph = Phi[j]+OmegaPhi[j]*t;
            x = RSV[j]*sin(th)*sin(ph);
            y = RSV[j]*sin(th)*cos(ph);
            z = RSV[j]*cos(th);
            tb = Bias[j];  //+Drift[j]*t;
            r = sqrt( (X(0)-x)*(X(0)-x) + (X(1)-y)*(X(1)-y) + (X(2)-z)*(X(2)-z) );
            f(4*i+j) = r-(tb-X(3));
            P(4*i+j,0) = (X(0)-x)/r;
            P(4*i+j,1) = (X(1)-y)/r;
            P(4*i+j,2) = (X(2)-z)/r;
            P(4*i+j,3) = 1.0;
         }
      }
   }
   else if(test == 5) {
      double x,y,z,r,tb,th,ph;
      t = batch * 0.32;
      for(int j=0; j<4; j++) {      // loop over satellites
         th = Theta[j]+OmegaTheta[j]*t;
         ph = Phi[j]+OmegaPhi[j]*t;
         x = RSV[j]*sin(th)*sin(ph);
         y = RSV[j]*sin(th)*cos(ph);
         z = RSV[j]*cos(th);
         tb = Bias[j];  //+Drift[j]*t;
         r = sqrt( (X(0)-x)*(X(0)-x) + (X(1)-y)*(X(1)-y) + (X(2)-z)*(X(2)-z) );
         f(j) = r-(tb-X(3));
         P(j,0) = (X(0)-x)/r;
         P(j,1) = (X(1)-y)/r;
         P(j,2) = (X(2)-z)/r;
         P(j,3) = 1.0;
      }
   }
   else if(test == 6) {
      // P6 is random but fixed
      Vector<double> f4;
      f4 = P6*X;
      for(int i=0; i<batch; i++) {
         for(int j=0; j<4; j++) {
            f(4*i+j) = f4(j);
            for(int k=0; k<4; k++) P(4*i+j,k) = P6(j,k);
         }
      }
   }
   else if(test == 7) {
      P = P6;
      f = P6*X;
   }
   else if(test == 8) {
         // log(L) = log(x0) + x1*log(n) + x2*log(w) + x3*log(d) + x4*log(D)
      for(int i=0; i<f.size(); i++) {
         f(i) = 0.0;
         P(i,0) = 1.0;
         P(i,1) = log(n8[i]);
         P(i,2) = log(w8[i]);
         P(i,3) = log(d8[i]);
         P(i,4) = log(D8[i]);
      }
   }
   else if(test == 9) {
         // f(x) = exp( x0 + x1*log(n) + x2*log(w) + x3*log(d) + x4*log(D) ) = L
      for(int i=0; i<f.size(); i++) {
         f(i) = exp(X(0) + X(1)*log(n8[i]) + X(2)*log(w8[i]) + X(3)*log(d8[i])
                         + X(4)*log(D8[i]));
         P(i,0) = f(i);
         P(i,1) = f(i)*log(n8[i]);
         P(i,2) = f(i)*log(w8[i]);
         P(i,3) = f(i)*log(d8[i]);
         P(i,4) = f(i)*log(D8[i]);
      }
   }
   else if(test == 10) {
      for(int i=0; i<f.size(); i++) {
         f(i) = sqrt((X(0)-x10[i])*(X(0)-x10[i]) + (X(1)-y10[i])*(X(1)-y10[i]));
         P(i,0) = (X(0)-x10[i])/f(i);
         P(i,1) = (X(1)-y10[i])/f(i);
      }
   }
   else if(test == 11 || test == 12) {
      double t;
      for(int i=0; i<batchlen; i++) {
         t = (years11[batch*batchsize+i] - 1856.0)/144.;
         P(i,0) = 1.0;
         for(int j=1; j<X.size(); j++) P(i,j) = P(i,j-1)*t;
      }
      f = P * X;
   }
   else if(test == 13 || test == 14) {
      P = 0.0;
      int n;
      for(int i=0; i<batchlen; i++) {
         n = (batch*batchsize+i)/(batchsize/X.size());
         P(i,n) = 1.0;
      }
      f = P * X;
   }
   else if(test == 15 || test == 16) {
      for(int i=0; i<batchlen; i++) {
         t = n15[batch*batchsize+i] - n15[0];
         P(i,0) = 1.0;
         for(int j=1; j<X.size(); j++) P(i,j) = P(i,j-1)*t;
      }
      f = P * X;
   }
}
catch(gpstk::Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
