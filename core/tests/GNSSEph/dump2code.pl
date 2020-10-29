#!/usr/bin/perl
#==============================================================================
#
#  This file is part of GPSTk, the GPS Toolkit.
#
#  The GPSTk is free software; you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published
#  by the Free Software Foundation; either version 3.0 of the License, or
#  any later version.
#
#  The GPSTk is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with GPSTk; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
#
#  This software was developed by Applied Research Laboratories at the
#  University of Texas at Austin.
#  Copyright 2004-2020, The Board of Regents of The University of Texas System
#
#==============================================================================

#==============================================================================
#
#  This software was developed by Applied Research Laboratories at the
#  University of Texas at Austin, under contract to an agency or agencies
#  within the U.S. Department of Defense. The U.S. Government retains all
#  rights to use, duplicate, distribute, disclose, or release this software.
#
#  Pursuant to DoD Directive 523024
#
#  DISTRIBUTION STATEMENT A: This software has been approved for public
#                            release, distribution is unlimited.
#
#==============================================================================

# This script converts the output from rawNavDump to C++ code that can
# be inserted into BDSEphemeris_T::wut()

use warnings;
use strict;

my ($prn, $health, $tocStr, $toeStr, $af0, $af1, $af2, $A, $Adot, $dn, $dndot,
    $ecc, $w, $M0, $OMEGA0, $OMEGAdot, $i0, $idot, $crs, $crc, $cis, $cic,
    $cus, $cuc);
my ($m, $d, $y, $hr, $min, $sec);

while (<>)
{
    chomp;
    my @arr = split(/ +/);
    if (/^PRN : /)
    {
        $prn = $arr[2];
    }
    elsif (/^Health bit/)
    {
        $health = hex($arr[$#arr]);
    }
    elsif (/^Clock Epoch:/)
    {
        $tocStr = $arr[$#arr-1] . " " . $arr[$#arr];
    }
    elsif (/^Eph Epoch:/)
    {
        $toeStr = $arr[$#arr-1] . " " . $arr[$#arr];
    }
    elsif (/^Bias T0:/)
    {
        $af0 = $arr[2];
    }
    elsif (/^Drift:/)
    {
        $af1 = $arr[1];
    }
    elsif (/^Drift rate:/)
    {
        $af2 = $arr[2];
    }
    elsif (/^Semi-major axis:/)
    {
        $A = $arr[2];
        $Adot = $arr[4];
    }
    elsif (/^Motion correction:/)
    {
        $dn = $arr[2];
        $dndot = $arr[4];
    }
    elsif (/^Eccentricity:/)
    {
        $ecc = $arr[1];
    }
    elsif (/^Arg of perigee:/)
    {
        $w = $arr[3];
    }
    elsif (/^Mean anomaly at epoch:/)
    {
        $M0 = $arr[4];
    }
    elsif (/^Right ascension:/)
    {
        $OMEGA0 = $arr[2];
        $OMEGAdot= $arr[4];
    }
    elsif (/^Inclination:/)
    {
        $i0 = $arr[1];
        $idot = $arr[3];
    }
    elsif (/^Radial *Sine:/)
    {
        $crs = $arr[2];
        $crc = $arr[5];
    }
    elsif (/^Inclination *Sine:/)
    {
        $cis = $arr[2];
        $cic = $arr[5];
    }
    elsif (/^In-track *Sine:/)
    {
        $cus = $arr[2];
        $cuc = $arr[5];
        printf("   try\n   {\n");
        printf("      gpstk::BDSEphemeris oe;\n");
        printf("      oe.Cuc      = $cuc;\n");
        printf("      oe.Cus      = $cus;\n");
        printf("      oe.Crc      = $crc;\n");
        printf("      oe.Crs      = $crs;\n");
        printf("      oe.Cic      = $cic;\n");
        printf("      oe.Cis      = $cis;\n");
        printf("      oe.M0       = $M0;\n");
        printf("      oe.dn       = $dn;\n");
        printf("      oe.dndot    = $dndot;\n");
        printf("      oe.ecc      = $ecc;\n");
        printf("      oe.A        = $A;\n");
        printf("      oe.Adot     = $Adot;\n");
        printf("      oe.OMEGA0   = $OMEGA0;\n");
        printf("      oe.i0       = $i0;\n");
        printf("      oe.w        = $w;\n");
        printf("      oe.OMEGAdot = $OMEGAdot;\n");
        printf("      oe.idot     = $idot;\n");
        ($m, $d, $y, $hr, $min, $sec) = split(/[\/ :]/, $tocStr);
        printf("      oe.ctToc    = gpstk::CivilTime(%d,%d,%d,%d,%d,%d,gpstk::TimeSystem::BDT);\n",$y,$m,$d,$hr,$min,$sec);
        printf("      oe.af0      = $af0;\n");
        printf("      oe.af1      = $af1;\n");
        printf("      oe.af2      = $af2;\n");
        printf("      oe.dataLoadedFlag = true;\n");
        printf("      oe.satID = gpstk::SatID($prn, gpstk::SatID::systemBeiDou);\n");
        ($m, $d, $y, $hr, $min, $sec) = split(/[\/ :]/, $toeStr);
        printf("      oe.ctToe    = gpstk::CivilTime(%d,%d,%d,%d,%d,%d,gpstk::TimeSystem::BDT);\n",$y,$m,$d,$hr,$min,$sec);
        printf("      writeVel(oe);\n");
        printf("   }\n");
        printf("   catch(...)\n");
        printf("   {\n");
        printf("      cerr << \"exception\" << endl;\n");
        printf("   }\n");
    }
}
