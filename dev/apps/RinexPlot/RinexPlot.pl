#!/usr/bin/perl
#
# RinexPlot - plot Rinex data
# 
# $Id$
#
#============================================================================
#
#  This file is part of GPSTk, the GPS Toolkit.
#
#  The GPSTk is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  any later version.
#
#  The GPSTk is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with GPSTk; if not, write to the Free Software Foundation, Inc.,
#  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#  
#  Copyright 2004, The University of Texas at Austin
#
#============================================================================
#-------------------------------------------------------------------------------------
# TD
# (option?) don't plot points with value 0
# it doesn't plot a symbol for the first point.
# need to figure out the height and width of a character in pixels.  **
# you can open a file in a re-entrant sub .. see the Perl books .. use for --load
# make it safe to run two copies at once .. use unique name for $CFG{'datafile'}
# Right axis
# Time limits
# Gnuplot output
# postscript output?
# RAIM -- how to plot?
# Dataset/Configure ? ... all ResCor options

#-------------------------------------------------------------------------------------
use strict;
use Tk;
use Tk::Dialog;
use Tk::DialogBox;
use Tk::ROText;
use Tk::BrowseEntry;
use File::stat;
use File::Basename;
use IO::Handle;

#-------------------------------------------------------------------------------------
# Version number
my $VERSION = "1.2 (10/1/2004)";
# Message box description of this program
my $ABOUT_TEXT = "\nRinexPlot is a GUI for utility programs developed with\n"
               . "the GPS Toolkit (GPSTk) that will read, manipulate and plot\n"
               . "data in a Rinex file.\n";
my $AUTHOR_TEXT = "RinexPlot is written in Perl/Tk by Dr. Brian W. Tolman.\n";

#-------------------------------------------------------------------------------------
# configuration - most things are stored in a hash - see Defaults()
my (%CFG,%OPT); # CFG has one value per key, OPT can have many values per key
my ($key,$val);
my %Options;    # valid options for command line -- see Defaults()
my %Grow;       # valid options that can have many values (OPT) -- see Defaults()

my $SLASH="/";
if ($^O eq "MSWin32") {
   $SLASH="\\";
}
my $menutear = 1;

#-------------------------------------------------------------------------------------
# drawing area TD add to CFG
my $BCOLOR       = 'white';        # background color for canvas
my $FCOLOR       = 'blue';         # foreground color for canvas
my $MCOLOR       = 'black';        # color of mouse rectangles
my $firstcanvas = 0;               # mark very first resizing of canvas

# cursors -- see
# Win32 : \perl\site\lib\Tk\X11\cursorfont.h
# Linux : /usr/X11R6/include/X11/cursorfont.h
my ($cursor,$waitcursor,$crosshair)=('crosshair','watch','crosshair');

#my @items = ();

#-------------------------------------------------------------------------------------
# Widgets
my $w_top;                         # top level window
my $w_canvas;                      # canvas
my $statusbar;                     # status bar
my $menubar;                       # menu bar

#-------------------------------------------------------------------------------------
# Plot configuration
my ($Xrate, $Yrate);
my $FirstAutoscale = 1;            # flag showing need to call autoscale()
my $LimitsSet = 0;                 # flag indicating LoadConfig has set limits
my $UsingDefaults = 1;             # flag indicating all labels are defaults
my $LogOpen = 0;                   # flag saying when LOG is open
my $MainUp = 0;                    # flag saying main window is up
my $ScaleDefined = 0;              # flag saying scales have been defined (Rates())
my ($begW,$begS)=(0,0);            # used in DataTime
my ($endW,$endS);

#-------------------------------------------------------------------------------------
# Axes
# Call NiceScale(datamin,datamax,$ScaleN,totalpixels);
# Labels are ($ScaleMin + ($i-1) * $ScaleStep) foreach $i (1..$ScaleN).
# Multiply labels by 10**ScaleExp to make them all (minimum size) integers.
# There are $ScalePPL pixels per label.
my ($ScaleMin,$ScaleStep,$ScaleExp,$ScaleN,$ScalePPL)=(0,0,0,0,0);
my @NiceUnits=(0,12,15,20,25,30,40,50,60,80,100,120,150);
my ($YScaleMin,$YScaleStep,$YScaleExp)=(0,0,0);
my ($XScaleMin,$XScaleStep,$XScaleExp)=(0,0,0);
# if plot limits (XMin,etc) are fixed by user, set this =1, else (autoscale) set =0
my $ScaleFixed=0;
# tics
my ($XTicLen,$YTicLen)=(5,5);
# use these as input to NewScale
my ($XMin,$XMax,$YMin,$YMax);
# save previous values for 'unzoom'
my ($OldXMin,$OldXMax,$OldYMin,$OldYMax)=('','','','');

#-------------------------------------------------------------------------------------
# Curves to plot (ncurv of them)
# each curve defined by Sat, OT, column in file, color, symbol type
my $Reconfigure;       # flag that says call ConfigureCurves()
my $ncurv;             # number of curves defined
my @curvSV = ();       # satellite for this curve
my @curvOT = ();       # obs type (from @obslist)
my @curvON = ();       # switch to turn off curve, on is non-zero
my @curvCol = ();      # column in $CFG{'datafile'} where data is found
my @curvLines = ();    # plot with lines or not (0)
my @curvSymbs = ();    # plot with symbols
my @symbols     = ('none','cross','plus','diamond','square','circle','del','delta');
my @curvColor = ();    # color to plot
my @colors = (
      '#0000ff', # bright blue
      '#ff0000', # bright red
      '#00ff00', # bright green
      '#ff00ff', # magenta
      '#00ffff', # cyan
      '#80ff00', # electric green
      '#ff0080', # pink
      '#8000ff', # purple
      '#ffff00', # yellow
      '#ff8000', # orange
      '#00ff80', # sea green
      '#0080ff', # sky blue
      '#000080', # dark blue
      '#800000', # dark red almost brown
      '#008000', # dark green
);
#'#000000', # black
#'#ffffff', # white
#'#a0b7ce', # "MSWin blue"

#-------------------------------------------------------------------------------------
# Mouse rectangle
my $MOUSE_RECT;                              # the rectangle id
my $MAKE_RECT=0;                             # state, initially off
my ($RECT_X0,$RECT_X1,$RECT_Y0,$RECT_Y1);    # rectangle limits
my $CLICK = 0;                               # is the mouse button down?

#-------------------------------------------------------------------------------------
# Data file(s)
#my $AutoView=1;           # flag that says view RinSum output when created
my ($filename,$ConfigFile,$ResCorfilename);
my ($RinexInSummary,$RinexRCSummary);
my @filelist=();
my ($numobs, $numsvs);    # number of obs types and SVs in Rinex file
my ($nobssel, $nsvssel);  # number of obs types and SVs (above) that are selected
my @obslist=();           # list of obs types in Rinex header
my @obsdesc=();           # descriptions of obs types from Rinex header dump
my @obsselect=();         # list of switches (0,1) parallel to obslist
my @svlist=();            # list of SVs in Rinex file
my @svselect=();          # list of switches (0,1) parallel to svlist
my @svbegin=();           # list of SV begin times (W,SOW)
my @svend=();             # list of SV end times (W,SOW)

#-------------------------------------------------------------------------------------
# Computation using ResCor (create file $ResCorfilename)
# list of extended obs types that ResCor can compute, with desc., units, dependence
my (@ExtOT, @ExtDesc, @ExtDep, @ExtSelect);     # these kept parallel
# hash: ExtUnit{$ExtOT[$i]} = units of that OT
my %ExtUnit=('L1','cycles','L2','cycles','C1','meters','P1','meters','P2','meters',
   'D1','Hz','D2','Hz','S1','dB-Hz','S2','dB-Hz');
# flags giving dependence of the Ext obs types on standard obs types
my ($DepL1,$DepL2,$DepC1,$DepP1,$DepP2,$DepEP,$DepPS) = (1,2,4,8,16,32,64);
my $ETdb;    # dialog box that selects extended obs types ... create only once
# for use as input to ResCor -- receiver label, position
my ($RxLabel,$RxX,$RxY,$RxZ,$RxComment) = ('','','','','');
# types that ResCor will debias: SP VP L3 L4 L5 MP M1 M2 M3 M4 M5 XR XI X1 X2
# (XR should not be debiased by ResCor)
# TD add config option to turn this on/off
my %MayDebias=('SP',1,'VP',1,'L3',1,'L4',1,'L5',1,
               'MP',1,'M1',1,'M2',1,'M3',1,'M4',1,'M5',1,
               'XR',0,'XI',1,'X1',1,'X2',1);
#
my $MyPrgmDir='unknown';
if ($^O eq 'MSWin32') {
   $MyPrgmDir="C:\\Code\\GPSLIB\\Working\\dev\\apps\\Rinextools";
}
elsif ($^O eq 'linux') {
   $MyPrgmDir="/home/btolman/mybin";
}
#-------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------
# This lists and defines(!) all valid options for CFG and OPT. This string is written
# in the Help/Topics window, and is printed to the screen when '--help' is the only
# cmdline option.
# It is also parsed by Defaults() into the default CFG and OPT settings, with rules:
# 0. one key/value pair per line.
# 1. the key follows '--' and does not contain whitespace.
# 2. the value is in parentheses at the end : ($val)\n$
# 3. the option is repeatable (cmdline only) if the string '(can repeat)' appears.
# 4. everything after 'Example:' is ignored.
# 5. ignore any line that starts (col 1) with '#'.
my $CmdText =
"# RinexPlot, part of the GPSTk : Plot Rinex data\n" .
"# Usage: perl RinexPlot.pl [--option <arg>]\n" .
"# Input options (put on command line or in config file) :\n" .
"# Rinex tools directory:\n" .
" --prgmdir <dir>     directory of GPSTK programs (". $MyPrgmDir . ")\n" .
"# input:\n" .
" --load <file>       load options file, but don't nest (can repeat) ()\n" .
" --Rinex <file>      load and summarize this Rinex observation file ()\n" .
" --nav <file>        load this Rinex navigation file ()\n" .
"# output:\n" .
" --log <file>        send all diagnostic output to <file> (SCREEN)\n" .
" --datafile <file>   store data to be plotted in flat <file> (RP.dat)\n" .
"# create new obs types:\n" .
" --AO <ExOT>         add extended obs types (can repeat) ()\n" .
" --create [on|off]   run ResCor to create any new (AO) obs types (on)\n" .
"# plot datasets\n" .
" --sat <sat>         select satellite for plot (can repeat) ()\n" .
" --obs <OT>          select obs type for plot (can repeat) ()\n" .
" --refresh [on|off]  draw the screen (only if --sat and --obs) (on)\n" .
" --begin <wk,sow>    begin GPS time -- do not read data before this time (0,0)\n" .
" --end <wk,sow>      end GPS time -- do not read data after this time (9999,0)\n" .
"# plot configuration:\n" .
" --width <pixels>    width of plot surface, L-axis to R-axis (640)\n" .
" --height <pixels>   height of plot surface, B-axis to T-axis (480)\n" .
" --lines [on|off]    draw a line when drawing curves (on)\n" .
" --points [on|off]   draw points when drawing curves (off)\n" .
" --XMin <x>          set minimum value on X axis, omit to autoscale ()\n" .
" --XMax <x>          set maximum value on X axis, omit to autoscale ()\n" .
" --YMin <y>          set minimum value on Y axis, omit to autoscale ()\n" .
" --YMax <y>          set maximum value on Y axis, omit to autoscale ()\n" .
" --Week <w>          week number associated with XMin/Max (for Blabel) ()\n" .
" --Bmargin <pixels>  distance between graph and window bottom (30)\n" .
" --Tmargin <pixels>  distance between graph and window top (30)\n" .
" --Lmargin <pixels>  distance between graph and window left (40)\n" .
" --Rmargin <pixels>  distance between graph and window right (30)\n" .
" --BticN <n>         number of tics on bottom axis (5)\n" .
" --TticN <n>         number of tics on top axis (5)\n" .
" --LticN <n>         number of tics on left axis (5)\n" .
" --RticN <n>         number of tics on right axis (5)\n" .
"# plot labels:\n" .
" --Blabel <string>   label below the bottom axis (GPS Seconds of Week)\n" .
" --Tlabel <string>   label above the top axis, i.e title (Title)\n" .
" --Llabel <string>   label above the left axis ()\n" .
" --Rlabel <string>   label above the right axis ()\n" .
"# switches:\n" .
" --verbose [on|off]  output more information to log file (off)\n" .
" --keepdata [on|off] on exit, do not delete the data file RP.dat (off)\n" .
" --autoview [on|off] automatically display data file summary (on)\n" .
" --Cforce [on|off]   force C1 to replace P1 (off)\n" .
" --Callow [on|off]   allow C1 to replace a missing P1 (on)\n" .
"# other:\n" .
" --zoomX <frac>      zoom commands expand(contract) by fraction (0.1)\n" .
" --help [on|off]     show the help window (off)\n" .
"#\n" .
" Example:\natom>./RinexPlot --Rinex ../gdms/data/04032/alic0320.04o --autoview off" .
"\n  --AO L4 --AO M5 --sat G08 --sat G30 --obs L4 --obs M5 --refresh\n";

#-------------------------------------------------------------------------------------
# Temp data
my (@opt, $file, $dummy, $i, $j, $ans, $msg);
my ($cmd,$buffer,$len);

#-------------------------------------------------------------------------------------
# computation of scales and coordinate transformations
#-------------------------------------------------------------------------------------
# compute a new 'nice' scale, using $XMin,$XMax,$YMin,$YMax.
# call with first arg 'fixed' if plot limits are fixed, else 'auto' to "autoscale"
# second arg is a label printed on LOG and Status
sub NewScale {
   my ($flag,$str)=@_;

   ($OldXMin,    $OldXMax,    $OldYMin,    $OldYMax) = 
   ($CFG{'XMin'},$CFG{'XMax'},$CFG{'YMin'},$CFG{'YMax'});

   if($YMin > $YMax) { $ans=$YMax; $YMax=$YMin; $YMin=$ans; }
   if($XMin > $XMax) { $ans=$XMax; $XMax=$XMin; $XMin=$ans; }

   my $tics = $CFG{'BticN'};
   if($flag eq 'fixed') { $tics = $CFG{'BticN'} + 2; }
   NiceScale($XMin,$XMax,$tics,$CFG{'width'});
   if($flag eq 'fixed') { $XScaleMin = $ScaleMin+$ScaleStep; }
   if($flag eq 'auto')  { $XScaleMin = $ScaleMin; }
   $XScaleStep = $ScaleStep;
   $XScaleExp = $ScaleExp;

   $tics = $CFG{'LticN'};
   if($flag eq 'fixed') { $tics = $CFG{'LticN'} + 2; }
   NiceScale($YMin,$YMax,$tics,$CFG{'height'});
   if($flag eq 'fixed') { $YScaleMin = $ScaleMin+$ScaleStep; }
   if($flag eq 'auto')  { $YScaleMin = $ScaleMin; }
   $YScaleStep = $ScaleStep;
   $YScaleExp = $ScaleExp;

   if($flag eq 'auto') {
      # let NiceScale determine the plot limits
      $YMin = $YScaleMin;
      $YMax = $YScaleMin+($CFG{'LticN'}-1)*$YScaleStep;
      $XMin = $XScaleMin;
      $XMax = $XScaleMin+($CFG{'BticN'}-1)*$XScaleStep;
   }

   ($CFG{'XMin'},$CFG{'XMax'},$CFG{'YMin'},$CFG{'YMax'})=($XMin,$XMax,$YMin,$YMax);
   Rates();  # Rates() can change XMin, etc.

   print LOG "$str: new limits are $YMin, $YMax, $XMin, $XMax\n";
   Status("$str: new limits are X: "
   . sprintf("%10.3f",$XMin) . ", "
   . sprintf("%10.3f",$XMax) .  ", Y: "
   . sprintf("%.3f",$YMin) . ", "
   . sprintf("%.3f",$YMax) .  ".");

   if($flag eq 'fixed') { $ScaleFixed=1; } else { $ScaleFixed=0; }
   $FirstAutoscale = 0;
}

# compute the 'rates' needed for coordinate transformations
sub Rates {
   # check for division by 0
   if($CFG{'XMin'} == $CFG{'XMax'}) {
      if($CFG{'XMax'} == 0) { $CFG{'XMax'} = 1; }
      else { $CFG{'XMax'} += 1.5 * $CFG{'XMax'}; }
   }
   if($CFG{'YMin'} == $CFG{'YMax'}) {
      if($CFG{'YMax'} == 0) { $CFG{'YMax'} = 1; }
      else { $CFG{'YMax'} += 1.5 * $CFG{'YMax'}; }
   }
   $Xrate = $CFG{'width'}/($CFG{'XMax'}-$CFG{'XMin'});
   $Yrate = -$CFG{'height'}/($CFG{'YMax'}-$CFG{'YMin'});

	$ScaleDefined = 1;
}

# convert data X units,    ($CFG{'XMin'} <= $rawx <= $CFG{'XMax'} )
# into screen coordinates  (0     <= $x    <= $CFG{'width'})
sub Xdata2scr {
   my $rawx = shift;
   my $x = $CFG{'Lmargin'} + ($rawx-$CFG{'XMin'}) * $Xrate;
   return int($x);
}

# convert data Y units,    ($CFG{'YMin'}   <= $rawy <= $CFG{'YMax'})
# into screen coordinates  ($CFG{'height'} <= $y    <= 0    )
sub Ydata2scr {
   my $rawy = shift;
   my $y = $CFG{'Tmargin'} + $CFG{'height'} + ($rawy-$CFG{'YMin'}) * $Yrate;
   return int($y);
}

# convert screen coordinates  (0     <= $x    <= $CFG{'width'})
# into data units,            ($CFG{'XMin'} <= $rawx <= $CFG{'XMax'} )
sub scr2Xdata {
   my $x = shift;
   my $rawx = $CFG{'XMin'} + ($x - $CFG{'Lmargin'})/$Xrate;
   return $rawx;
}

# convert screen coordinates  ($CFG{'height'} <= $y    <= 0    )
# into data units,            ($CFG{'YMin'}   <= $rawy <= $CFG{'YMax'})
sub scr2Ydata {
   my $y = shift;
   my $rawy = ($y - $CFG{'Tmargin'} - $CFG{'height'})/$Yrate + $CFG{'YMin'};
   return $rawy;
}

#-------------------------------------------------------------------------------------
# menu callbacks
#-------------------------------------------------------------------------------------
sub FileLoad {
   $filename = $w_top->getOpenFile(-title => 'Load a configuration file',
      -initialfile => basename($ConfigFile),
      #-initialdir => $dir,
      #-defaultextension => $ext,
   );
   if($filename eq '') { return; }

   $LimitsSet = 0;                      # LoadConfig will notice if limits are set
   $ans = LoadConfig($filename);
   if($ans ne 'fail') {
      ProcessConfig();
   }
}

sub FileSave {
   print LOG "Save the configuration file $ConfigFile\n";
   $ans = SaveConfig($ConfigFile);
   if($ans eq 'fail') {
      print LOG "Could not save configuration file $ConfigFile\n";
   }
}

sub FileSaAs {
   my $Savefilename = $w_top->getSaveFile(-title => 'Save configuration in a file');
   $ans = SaveConfig($Savefilename);
   if($ans eq 'fail') {
      print LOG "Could not save configuration file $Savefilename\n";
   }
}

#-------------------------------------------------------------------------------------
sub FileExit {
   Status("Cleaning up...");
   print LOG "Clean up and exit.\n";
   if($CFG{'keepdata'} ne 'on') {
      $dummy = `rm -f $CFG{'datafile'}`;
      if($CFG{'verbose'} eq 'on') { print LOG $dummy; }
   }
   if($CFG{'log'} ne '') { close LOG; }
   exit;
}

#-------------------------------------------------------------------------------------
sub FileInpu {
   Status("Get the name of a Rinex observation file...");
   $filename = $w_top->getOpenFile(-title => 'Choose a Rinex obs file');
   if($filename eq '') { return; }
   DataInputProcess($filename);
}

#-------------------------------------------------------------------------------------
sub FileSumm {
   $file = $RinexInSummary;
   if($ResCorfilename ne '') { $file=$RinexRCSummary; }
   if($file eq '') {
      PopNotice("Error: no file","No input Rinex observation file has been chosen\n"
            . "Go to File/Rinex Obs");
      #$ans = $w_top->messageBox(-title => 'Error: no file',
      #-message => "No input observation file has been chosen\n"
      #. "Go to File/Rinex Obs",
      #-type => 'OK', -icon => 'info');
      return;
   }
   Status("Press the Close button to return to the main window...");
   #print LOG "View data from $file\n";
   $buffer = '';
   my $len=0;
   if (not open(F, "<$file")) {
      $w_top->Dialog(
         -title => 'File not found',
         -text => "Could not open the file $file",
         -bitmap => 'error',
      )->Show;
      return;
   }
   $len = read(F, $buffer, 100000);
   #print LOG "Read $len bytes\n";
   my @list = split("\n", $buffer);
   my $width=0; foreach $i (@list) { if(length($i)>$width) { $width=length($i); } }
   my $TWdb = $w_top->DialogBox(
      -title => "View RinSum output ($len bytes): $file",
      -buttons => ['Close'],
      -popover => $w_top,
      -overanchor => 'w',
      -popanchor => 'w',
   );
   my $text_win = $TWdb->Scrolled('Text',
      -setgrid => 'true',
      -scrollbars => 'e',
      -height => '40',   #lines
      -width => $width,  #characters
      )->pack(-expand => 'yes', -fill => 'both');
   $text_win->insert('end', $buffer);  # add text at end of (empty) widget
   $text_win->see('end');              # scroll to end of text
   $TWdb->Show();
   close F;
   Status("...");
}

#-------------------------------------------------------------------------------------
sub FileSele {
   Status('');

   # figure out the width of the box
   my $len = length($CFG{'Rinex'});
   if($len < length($CFG{'nav'})) { $len = length($CFG{'nav'}); }
   if($RxLabel ne '' && $len < 60) { $len = 60; }
   if($ResCorfilename ne '' && $len < length($ResCorfilename)) {
      $len = length($ResCorfilename);
   }
   if($len < 20) { $len = 20; }

   # are there extended obs types defined?
   $dummy = 0;
   $msg = '';
   foreach $i (0..$#ExtOT) {
      if($ExtSelect[$i] != 0) {
         $dummy++;
         $msg = $msg . $ExtOT[$i] . " ";
      }
   }

   my $FSdb = $w_top->DialogBox(
      -title =>  "Current Selections",
      -buttons => ['Ok','Clear'],
      -default_button => 'Ok',
      -popover => $w_top,
      -overanchor => 'c',
      -popanchor => 'c',
   );
   $FSdb->add('Label',-text => "Rinex Observation File:")->pack(-anchor => 'w');
   my $f1=$FSdb->Frame(-borderwidth => 2, -relief => 'groove',
      )->pack(-anchor => 'w');
   $FSdb->add('Label',-text => "Rinex Navigation File:")->pack(-anchor => 'w');
   my $f2=$FSdb->Frame(-borderwidth => 2, -relief => 'groove',
      )->pack(-anchor => 'w');
   my ($f3,$f4,$f5,$f6);
   if($RxLabel ne '') {
      $FSdb->add('Label',-text => "Receiver Position:")->pack(-anchor => 'w');
      $f3=$FSdb->Frame(-borderwidth => 2, -relief => 'groove',
         )->pack(-anchor => 'w');
   }
   if($ResCorfilename ne '') {
      $FSdb->add('Label',-text => "Computed Observations File")->pack(-anchor => 'w');
      $f4=$FSdb->Frame(-borderwidth => 2, -relief => 'groove',
         )->pack(-anchor => 'w');
   }
   if($dummy > 0) {
      $FSdb->add('Label',-text => "Computed Observation Types")->pack(-anchor => 'w');
      $f5=$FSdb->Frame(-borderwidth => 2, -relief => 'groove',
         )->pack(-anchor => 'w');
   }
   $FSdb->add('Label',-text => "Current configuration file")->pack(-anchor => 'w');
   $f6=$FSdb->Frame(-borderwidth => 2, -relief => 'groove')->pack(-anchor => 'w');


   my $e1=$f1->Entry(
      -textvariable => \$CFG{'Rinex'}, -width => $len, -justify => 'left',
      )->pack(-side => 'top', -anchor => 'w');
   $f1->Label(-text => 'Summary file:')->pack(-side => 'left');
   $f1->Label(-text => $RinexInSummary, -justify => 'left')->pack(-side => 'left');
   my $e2=$f2->Entry(
      -textvariable => \$CFG{'nav'}, -width => $len)->pack(-side => 'left');
   if($RxLabel ne '') {
      my $f31=$f3->Frame(-borderwidth => 2, -relief => 'flat')->pack(-anchor => 'w');
      $f31->Label(-text => "Label : ")->pack(-side => 'left');
      $f31->Entry(-textvariable => \$RxLabel, -width => 5)->pack(-side => 'left');
      my $f32=$f3->Frame(-borderwidth => 2, -relief => 'flat')->pack(-anchor => 'w');
      $f32->Label(-text => "X : ")->pack(-side => 'left');
      $f32->Entry(-textvariable => \$RxX, -width => 15)->pack(-side => 'left');
      $f32->Label(-text => "  Y : ")->pack(-side => 'left');
      $f32->Entry(-textvariable => \$RxY, -width => 15)->pack(-side => 'left');
      $f32->Label(-text => "  Z : ")->pack(-side => 'left');
      $f32->Entry(-textvariable => \$RxZ, -width => 15)->pack(-side => 'left');
      my $f33=$f3->Frame(-borderwidth => 2, -relief => 'flat')->pack(-anchor => 'w');
      $f33->Label(-text => "Comment: ")->pack(-side => 'left');
      $f33->Entry(-textvariable => \$RxComment, -width => 50)->pack(-side => 'left');
   }
   if($ResCorfilename ne '') {
      $f4->Label(-text => $ResCorfilename, -justify => 'left',
         )->pack(-side => 'top', -anchor => 'w');
      $f4->Label(-text => 'Summary file:'
         )->pack(-side => 'left', -anchor => 'w');
      $f4->Label(-text => $RinexRCSummary, -justify => 'left',
         )->pack(-side => 'left', -anchor => 'w');
   }

   #TD add ExtOT, Selected sats/obs ? 
   if($dummy > 0) {
      $f5->Label(-text => $msg, -justify => 'left',
         )->pack(-side => 'top', -anchor => 'w');
   }

   $f6->Label(-text => $ConfigFile, -justify => 'left',
      )->pack(-side => 'top', -anchor => 'w');

   $ans = $FSdb->Show();
   $e1->focus; $e1->icursor('end');
   if($ans eq 'Clear') {
      FileClea();
   }
}

#-------------------------------------------------------------------------------------
sub FileClea {
   $ans = $w_top->messageBox(
      -title => "Clear all selections",
      -message => "Are you sure you want to clear\n"
         . "all file names and all data selections?     ",
      -type => 'YesNo',
      -icon => 'question',
      #no popover for messageBox
   );
   if($ans eq 'yes') { ClearAll(); }
}

#-------------------------------------------------------------------------------------
sub ClearAll {
   Status("All the input files and data selection have been cleared.");
   $CFG{'nav'} = '';
   $CFG{'Rinex'} = '';
   $ResCorfilename = '';
   $RinexInSummary = '';
   $RinexRCSummary = '';
   $RxLabel = '';
   @obslist = ();
   @obsdesc = ();
   @obsselect = ();
   @svlist = ();
   @svselect = ();
   $dummy = `rm -f $CFG{'datafile'}`;
   if($CFG{'verbose'} eq 'on') { print LOG $dummy; }
   @svbegin = ();
   @svend = ();
   $numobs = 0;
   $numsvs = 0;
   $nobssel = 0;
   $nsvssel = 0;
   $FirstAutoscale = 1;
   ClearExot();
}

#-------------------------------------------------------------------------------------
sub DataInputProcess {
   ($filename) = @_;
   # first see that it exists
   if (not open(F, "<$filename")) {
      PopNotice("Empty file","File $filename        \ndoes not exist or is empty.");
      return;
   }
   print LOG "Selected input Rinex obs file $filename\n";
   ClearAll();
   WaitCursor();

	# get extended obs type information here (once) from ResCor
   ExtDialogInit();

   Status("Summarizing obs file $filename...please wait...");
   $CFG{'Rinex'} = $filename;
   $ResCorfilename = '';
   $RinexRCSummary = '';
   $RinexInSummary = basename($filename) . ".sum";
   $cmd = $CFG{'prgmdir'} . $SLASH . "RinSum -g -i" . $CFG{'Rinex'}
      . " > $RinexInSummary";
   print LOG "Summarize obs file: Execute $cmd\n";
   $dummy = `$cmd`; if($CFG{'verbose'} eq 'on') { print LOG $dummy; }

   # pull out obs types from $RinexInSummary
   $ans = DataGather($RinexInSummary);
   if($ans == -1) {
      Status("Choose another observation file");
      PopNotice("Error: wrong file type",
         $CFG{'Rinex'} . "         \nis NOT a Rinex observation file.\n");
      $CFG{'Rinex'} = '';
      $ResCorfilename = '';
      $RinexInSummary = '';
   }
   else {
      if($CFG{'autoview'} eq 'on') { FileSumm(); }
      $FirstAutoscale = 1;
   }
   Status("Summary complete for $filename.");
   NormalCursor();
}

#-------------------------------------------------------------------------------------
sub DataGather {
   ($file) = @_;
   open FILE, "$file" or die "Error: DataGather could not open file $file\n";
   @obslist = ();
   @obsdesc = ();
   @obsselect = ();
   @svlist = ();
   @svselect = ();
   $dummy = `rm -f $CFG{'datafile'}`; if($CFG{'verbose'} eq 'on') { print LOG $dummy; }
   @svbegin = ();
   @svend = ();
   $nobssel = 0;
   $nsvssel = 0;
   my $EOH = 0;
   while(<FILE>) {
      chomp;
      $_ =~ s/^\s+//;
      if(m/is not a Rinex observation file/) {
         return -1;
      }
      if(m/END OF HEADER/) { $EOH=1; }

      @opt = split (/\s+/, $_);
      if($opt[0] eq "Type") {
         push @obslist, $opt[3];
         push @obsdesc, substr($_,13,length($_)-13);
         push @obsselect, 0;
      }
      # following condition (..eq "G") means only GPS satellites get picked up
      if($EOH==1 && $opt[0] eq "PRN" && substr($opt[1], 0, 1) eq "G") {
         $numobs = $#obslist + 1;
         push @svlist, $opt[1];
         push @svselect, 0;
         push @svbegin, [ $opt[$numobs+3], $opt[$numobs+4] ];
         push @svend, [ $opt[$numobs+6], $opt[$numobs+7] ];
      }
      if($opt[0] eq "WARNING:" && $opt[1] eq "ObsType") {
         $dummy = $opt[2];
         foreach $i (0..$numobs-1) {
            if($obslist[$i] eq $dummy) {
               splice(@obslist, $i, 1);
               splice(@obsdesc, $i, 1);
               splice(@obsselect, $i, 1);
               $numobs--;
               last;
            }
         }
      }
   }
   close FILE;
   $numsvs = $#svlist + 1;

   #print LOG "Obs types ($numobs) found: @obslist\n";
   #print LOG "Found $numsvs SVs, here are start and stop times:\n";
   #foreach $i (0..$numsvs-1) {
   #   print LOG "$svlist[$i] ($svbegin[$i][0],$svbegin[$i][1])"
   #   . " - ($svend[$i][0],$svend[$i][1])\n";
   #}
   return 0;
}

#-------------------------------------------------------------------------------------
sub DataSats {
   if($numsvs <= 0) {
      $msg = "There are no satellites to select!"
         . "\nGo to File/Input to load a new Rinex data file";
      PopNotice("Error: no satellites",$msg);
      return;
   }

   Status("Select satellites to be plotted.");
   my $SLdb = $w_top->DialogBox(
      -title => 'Select satellite(s) for plot',
      -buttons => ['Ok','Cancel'],
      -default_button => 'Ok',
      -popover => $w_top,
      -overanchor=> 'n',
      -popanchor => 'n',
   );
   my (@cb);
   my $rows = 6;
   my ($r,$c,$cols)=(1,0,int 1+$numsvs/$rows);

   # label and frame for satellites
   $SLdb->add('Label',
      -text => "Select Satellite(s)",
      -justify => 'center')->grid(-columnspan => "$cols");
   my $f1=$SLdb->Frame(-borderwidth => 2, -relief => 'groove')->grid();
   foreach $i (0..$numsvs-1) {
      $cb[$i] = $f1->Checkbutton(
         -text => $svlist[$i],
         -variable => \$svselect[$i],
         -relief => 'flat')->grid(-row => $r, -column => $c, -ipadx => '2');
      $r++;
      if($r > $rows) { $r = 1; $c++; }
   }
   # frame for two buttons
   $dummy = $rows+1;
   my $f2 = $f1->Frame(-borderwidth => 2, -relief => 'flat'
      )->grid(-row => $dummy, -columnspan => "$cols");
   $dummy = int($cols/2);
   if($dummy == 0) { $dummy=1; }
   my $cb2 = $f2->Button(
      -text => 'All',
      -width => '15',
      -relief => 'groove',
      -command => \&SetAllSats)->grid(-row => 1, -column => 1);
   my $cb1 = $f2->Button(
      -text => 'Clear',
      -width => '15',
      -relief => 'groove',
      -command => \&ClearSats)->grid(-row => 1, -column => 2);

   # save a copy to see when sats are un-selected
   my @saveselect = @svselect;
   $ans = $SLdb->Show();
   if($ans eq "Cancel") {
      @svselect = @saveselect;
      return;
   }

   $dummy = 0;
   $filename = "$CFG{'datafile'}";
   foreach $i (0..$numsvs-1) {
      if($saveselect[$i] != $svselect[$i]) {
         if($svselect[$i] == 0) {
            if($dummy == 0) { print LOG "DataSet/Sats Selected Sats:"; $dummy=1; }
            print LOG " -$svlist[$i]";
         }
         else {
            if(-e $filename) {
               $dummy = `rm -f $filename`;
               if($CFG{'verbose'} eq 'on') { print LOG $dummy; }
            }
         }
         $Reconfigure = 1;
      }
      if($svselect[$i] != 0) {
         if($dummy == 0) { print LOG "DataSet/Sats Selected Sats:"; $dummy=1; }
         print LOG " $svlist[$i]";
         $nsvssel++;
      }
   }
   if($dummy == 1) { print LOG "\n"; }
   Status("Satellites selected ... go to DataSet/Obs to select obs?");
}

#-------------------------------------------------------------------------------------
sub DataObst {
   if($numobs <= 0) {
      $msg = "There are no observation types selected!"
         . "\nGo to File/Input to load a new Rinex data file";
      PopNotice("Error: no obs types",$msg);
      return;
   }

   Status("Select observations to be plotted.");
   my $SLdb = $w_top->DialogBox(
      -title => 'Select observation type(s) for plotting',
      -buttons => ['Ok','Cancel'],
      -default_button => 'Ok',
      -popover => $w_top,
      -overanchor=> 'n',
      -popanchor => 'n',
   );
   my (@cb);
   my $rows = 6;
   my ($r,$c,$cols)=(1,0,int 1+$numobs/$rows);

   # label and frame for obs types
   $SLdb->add('Label',
      -text => "\nSelect Obs Type(s)",
      -justify => 'center')->grid(-columnspan => "$cols");
   my $f2=$SLdb->Frame(-borderwidth => 2, -relief => 'groove')->grid();
   foreach $i (0..$numobs-1) {
      $cb[$i] = $f2->Checkbutton(
         -text => $obslist[$i] . " = " . $obsdesc[$i],
         -variable => \$obsselect[$i],
         -relief => 'flat')->grid(-sticky => 'w');
   }
   # frame for two buttons
   $dummy = $numobs+1;
   my $f1 = $f2->Frame(-borderwidth => 2, -relief => 'flat'
      )->grid(-row => $dummy, -columnspan => "$cols");
   my $cb2 = $f1->Button(
      -text => 'Clear',
      -width => '15',
      -relief => 'groove',
      -command => \&ClearObs);
   my $cb1 = $f1->Button(
      -text => 'All',
      -width => '15',
      -relief => 'groove',
      -command => \&SetAllObs)->grid($cb2);

   # save a copy to see when sats are un-selected
   my @saveselect = @obsselect;
   $ans = $SLdb->Show();
   if($ans eq "Cancel") {
      @obsselect = @saveselect;
      return;
   }

   $dummy = 0;
   foreach $i (0..$numobs-1) {
      if($saveselect[$i] != $obsselect[$i]) {
         $Reconfigure = 1;
         $ans = `rm -f $CFG{'datafile'}`; if($CFG{'verbose'} eq 'on') { print LOG $ans; }
      }
      if($obsselect[$i] != 0) {
         if($dummy == 0) { print LOG "DataSet/ObsTypes Selected Obs:"; $dummy=1; }
         print LOG "  $obslist[$i]";
         $nobssel++;
      }
   }
   if($dummy == 1) { print LOG "\n"; }
   Status("Observations selected ... Refresh?");
}

#-------------------------------------------------------------------------------------
sub DataTime {
   # put up a dialog box with number entry box
   Status("Dataset/Times Enter new time limits");
   my $DTdb = $w_top->DialogBox(
      -title => 'Time Limits',
      -buttons => ['Ok','Cancel'],
      -default_button => 'Ok',
      -popover => $w_top,
      -overanchor => 'c',
      -popanchor => 'c',
   );

   ($begW,$begS) = split(',',$CFG{'begin'});
   ($endW,$endS) = split(',',$CFG{'end'});

   $DTdb->add('Label',-text => "  GPS   Week  Seconds of week")->pack(-anchor => 'w');
   my $f1=$DTdb->Frame(-borderwidth => 2, -relief => 'flat')->pack(-anchor => 'w');
   #$DTdb->add('Label')->pack(-anchor => 'w');
   my $f2=$DTdb->Frame(-borderwidth => 2, -relief => 'flat')->pack(-anchor => 'w');

   $f1->Label(-text => "Begin : ")->pack(-side => 'left');
   my $e1=$f1->Entry(-textvariable => \$begW, -width => 5)->pack(-side => 'left');
   $f1->Entry(-textvariable => \$begS, -width => 15)->pack(-side => 'left');
   $f1->Button(-text => 'Clear', -width => '10', -relief => 'groove',
      -command => \&ClearDTL)->pack(-side => 'right', -padx => '10');

   $f2->Label(-text => "End    : ")->pack(-side => 'left');
   $f2->Entry(-textvariable => \$endW, -width => 5)->pack(-side => 'left');
   $f2->Entry(-textvariable => \$endS, -width => 15)->pack(-side => 'left');
   $f2->Button(-text => 'Set to Data', -width => '10', -relief => 'groove',
      -command => \&SetDTL)->pack(-side => 'right', -padx => '10');

   $e1->focus;
   $e1->icursor('end');
   $ans = $DTdb->Show();
   if($ans eq 'Ok') {
      print LOG "Dataset/Time limits are ($begW,$begS)-($endW,$endS)\n";
      $CFG{'begin'} = "$begW,$begS";
      $CFG{'end'} = "$endW,$endS";
   }
   Status('');
}
sub ClearDTL {
   $begW = 0;
   $begS = 0;
   $endW = 9999;
   $endS = 0;
}
sub SetDTL {
   ClearDTL();
   if($numsvs <= 0) { return; }
   my $firstime = 1;
   foreach $i (0..$numsvs-1) {
      if($svselect[$i] != 0) {
         if($firstime) {
            $begW = $svbegin[$i][0];
            $begS = $svbegin[$i][1];
            $endW = $svend[$i][0];
            $endS = $svend[$i][1];
            $firstime = 0;
         }
         else {
            if( $svbegin[$i][0] <  $begW ||
                   ($svbegin[$i][0] == $begW && $svbegin[$i][1] < $begS)) {
               $begW = $svbegin[$i][0];
               $begS = $svbegin[$i][1];
            }
            if( $svend[$i][0] >  $endW ||
                   ($svend[$i][0] == $endW && $svend[$i][1] > $endS)) {
               $endW = $svend[$i][0];
               $endS = $svend[$i][1];
            }
         }
      }
   }
}

#-------------------------------------------------------------------------------------
sub DataConf {
   PopNotice("DataSet/Configure","DataSet/Configure is not yet implemented");
   # Iono height
   # debiasing
}

#-------------------------------------------------------------------------------------
sub SetAllSats {
   if($numsvs <= 0) { return; }
   foreach $i (0..$numsvs-1) { $svselect[$i] = 1; }
   $nsvssel = $numsvs;
   $FirstAutoscale = 1;
}

#-------------------------------------------------------------------------------------
sub ClearSats {
   if($numsvs <= 0) { return; }
   foreach $i (0..$numsvs-1) {
      if($svselect[$i] != 0) {
         $filename = "$CFG{'datafile'}";
         if(-e $filename) {
            $dummy = `rm -f $filename`;
            if($CFG{'verbose'} eq 'on') { print LOG $dummy; }
         }
      }
      $svselect[$i] = 0;
   }
   $nsvssel = 0;
   $FirstAutoscale = 1;
}

#-------------------------------------------------------------------------------------
sub SetAllObs {
   if($numobs <= 0) { return; }
   foreach $i (0..$numobs-1) { $obsselect[$i] = 1; }
   $nobssel = $numobs;
   $FirstAutoscale = 1;
}

#-------------------------------------------------------------------------------------
sub ClearObs {
   if($numobs <= 0) { return; }
   foreach $i (0..$numobs-1) { $obsselect[$i] = 0; }
   $nobssel = 0;
   $FirstAutoscale = 1;
}

#-------------------------------------------------------------------------------------
sub ConfigureCurves {
   # each curve defined by Sat, OT, column in file, color, symbol type
   # this routine must be called whenever selected svs OR selected ot changes
   # when selected ot changes, the file $CFG{'datafile'} must be deleted
   # if not already called, call this routine: Refresh, Graph/Curves, Graph/Limits
   if($Reconfigure == 0) { return; }
   print LOG "Configure curves:";
   WaitCursor();
   Status("Configure curves from selected data...please wait...");
   $ncurv = 0;
   $dummy = 0;       # count colors
   # count symbols, if symbols 'on' (symbols[0]='none')
   if($CFG{'points'} eq 'on') { $ans = 1; } else { $ans = 0; }
   @curvSV = ();       # satellite for this curve
   @curvOT = ();       # obs type (from @obslist)
   @curvON = ();       # switch to turn off curve, on is non-zero
   @curvCol = ();      # column in $CFG{'datafile'} where data is found
   @curvLines = ();    # plot with lines or not (0)
   @curvSymbs = ();    # plot with symbols (1-?) or not (0)
   @curvColor = ();    # color to plot
   foreach $i (0..$numsvs-1) {
      if($svselect[$i] != 0) {                  # each satellite <-> file
         my $n=0;   # n just counts the columns <-> obstype
         foreach $j (0..$numobs-1) {
            if($obsselect[$j] != 0) {           # each obstype
               $curvSV[$ncurv] = $svlist[$i];
               $curvOT[$ncurv] = $obslist[$j];
               $curvON[$ncurv] = 1;
               $curvCol[$ncurv] = 3*($n+1);
               $n++;
               $curvColor[$ncurv] = $colors[$dummy];
               $dummy++; if($dummy > $#colors) { $dummy=0; }
               if($CFG{'lines'} eq 'on') { $curvLines[$ncurv] = 1; }
               else { $curvLines[$ncurv] = 0; }
               $curvSymbs[$ncurv] = $symbols[$ans];
               if($CFG{'points'} eq 'on') {
                  $ans++;
                  if($ans > $#symbols) { $ans=1; }
               }
               $ncurv++;
               print LOG " $ncurv:$svlist[$i]/$obslist[$j]";
            }
         }
      }
   }
   print LOG "\n";

   if($ncurv <= 0) { # TD message? popup?
      NormalCursor();
      Status('');
      return;
   }

   #print LOG "Number svs is $numsvs\n";
   my $file = $CFG{'Rinex'};
   if($ResCorfilename ne '') { $file = $ResCorfilename; }
   $filename = "$CFG{'datafile'}";
   if(not -e $filename) {
      Status("Configure curves from selected data...please wait...");
      $cmd = $CFG{'prgmdir'} . $SLASH . "RinexDump " . $file;
      foreach $i (0..$numsvs-1) {
         if($svselect[$i] != 0) { $cmd = $cmd . " $svlist[$i]"; }
      }
      foreach $j (0..$numobs-1) {
         if($obsselect[$j] != 0) { $cmd = $cmd . " $obslist[$j]"; }
      }
      $cmd = $cmd . " > $filename";
      print LOG "Collect data for plotting: Execute $cmd\n";
      $dummy = `$cmd`; if($CFG{'verbose'} eq 'on') { print LOG $dummy; }
   }

   if($FirstAutoscale == 1) { autoscale(); }
   if($Reconfigure == 1 && $UsingDefaults) { DefaultLabels(); }
   $Reconfigure = 0;
   NormalCursor();
   Status("Curves configured...Refresh?");
}

#-------------------------------------------------------------------------------------
sub DefaultLabels {
   print LOG "Configure default labels\n";
   WaitCursor();
   Status("Configure labels from selected data...please wait...");

   my (@ot,@sv,@un);
   my %SVseen = ();
   my %OTseen = ();
   my %UNseen = ();
   foreach $i (0..$ncurv-1) {
      if($curvON[$i] != 0) {
         # TD decide if they are plotted on left or right
         unless($SVseen{$curvSV[$i]}) {
            $SVseen{$curvSV[$i]} = 1;
            push @sv, $curvSV[$i];
         }
         unless($OTseen{$curvOT[$i]}) {
            $OTseen{$curvOT[$i]} = 1;
            push @ot, $curvOT[$i];
         }
         unless($UNseen{$ExtUnit{$curvOT[$i]}}) {
            $UNseen{$ExtUnit{$curvOT[$i]}} = 1;
            push @un, $ExtUnit{$curvOT[$i]};
         }
      }
   }

   #print LOG "Unique SV @sv\n";
   #print LOG "Unique OT @ot (@un)\n";
   if($#un > 0) {
      print LOG "WARNING: Inconsistent units on left axis (@un).\n";
   }
   $CFG{'Tlabel'} = "OTs (@ot) for Sats (@sv) vs. Time";
   $CFG{'Llabel'} = "@ot (@un)";
   $CFG{'Rlabel'} = '';
   $CFG{'Blabel'} ="GPS Seconds of Week $CFG{'Week'}";

   NormalCursor();
   $UsingDefaults = 1;
   Status("Labels configured.");
}

#-------------------------------------------------------------------------------------
sub CompObst {
   Status("Select new observation types to create...");
   $ans = $ETdb->Show();
   if($ans eq "Cancel") {
      Status('');
      return;
   }
   Status("New observation types have been selected..." .
      "go to Compute/Create to compute.");

   $dummy = 0;
   foreach $i (0..$#ExtOT) {
      if($ExtSelect[$i] != 0) {
         if($dummy == 0) { print LOG "Compute/New Selected New Obs:"; $dummy=1; }
         print LOG " $ExtOT[$i]";
      }
   }
   if($dummy == 1) { print LOG "\n"; }
}

#-------------------------------------------------------------------------------------
sub ExtDialogInit {
   if($#ExtOT == -1) {  # only do this once...
      $cmd = $CFG{'prgmdir'} . $SLASH . "ResCor -h > syntax.rc";
      print LOG "Get list of possible new obs types: Execute $cmd\n";
      $dummy = `$cmd`; if($CFG{'verbose'} eq 'on') { print LOG $dummy; }
      open FILE, "syntax.rc" or die "Error: could not open syntax.rc\n";
      my @opt = <FILE>;
      my @ExtROT;
      $ans = 0;
      for($i=0; $i <= $#opt; $i++) {
         chomp $opt[$i];
         if($opt[$i] =~ /OT Description/) { $ans = 1; }
         if($ans == 2) {
            if($opt[$i] =~ /End of list of extended observation types/) { last; }
            push @ExtROT, $opt[$i];
            #print LOG "Line $i is $opt[$i]\n";
         }
         if($ans == 1 && $opt[$i] =~ /^  -- --/) { $ans = 2; }
      }
      for($i=0; $i <= $#ExtROT; $i++) {
         push @ExtOT, substr($ExtROT[$i],2,2);
			#print "New OT ",substr($ExtROT[$i],2,2);
         my $desc = substr($ExtROT[$i],5,20);
			#print " desc ",$desc;
         #$desc =~ s/\s*$//;
			push @ExtDesc, $desc;
         my @fields = split(" ", substr($ExtROT[$i],26,length($ExtROT[$i])-26));
         #push @ExtUnit, $fields[0];
         $ExtUnit{$ExtOT[$i]} = $fields[0];
			#print " units ",$ExtUnit{$ExtOT[$i]},"\n";
         my $dep=0;
         for($j=1; $j <= $#fields; $j++) {
            if($fields[$j] eq 'L1') { $dep |= $DepL1; }
            if($fields[$j] eq 'L2') { $dep |= $DepL2; }
            if($fields[$j] eq 'P1') { $dep |= $DepP1; }
            if($fields[$j] eq 'P2') { $dep |= $DepP2; }
            if($fields[$j] eq 'EP') { $dep |= $DepEP; }
            if($fields[$j] eq 'PS') { $dep |= $DepPS; }
         }
         push @ExtDep, $dep;
         push @ExtSelect, 0;
         #for($j=1; $j <= $#fields; $j++) { print LOG "/$fields[$j]"; }  # depends
         #print LOG "\n";
      }
      #print LOG "Extended types:\n";
      #for($i=0; $i <= $#ExtROT; $i++) {
      #print LOG "$ExtOT[$i] $ExtDesc[$i] $ExtUnit{$ExtOT[$i]} dep(";
      #if($ExtDep[$i] & $DepL1) { print LOG " L1"; }
      #if($ExtDep[$i] & $DepL2) { print LOG " L2"; }
      #if($ExtDep[$i] & $DepP1) { print LOG " P1"; }
      #if($ExtDep[$i] & $DepP2) { print LOG " P2"; }
      #if($ExtDep[$i] & $DepEP) { print LOG " EP"; }
      #if($ExtDep[$i] & $DepPS) { print LOG " PS"; }
      #print LOG ")\n";
      #}

      #now make up the dialog box
      $ETdb = $w_top->DialogBox(
         -title => 'Select extended observation type for processing',
         -buttons => ['Ok','Cancel'],
         -default_button => 'Ok',
         -popover => $w_top,
         -overanchor=> 'n',
         -popanchor => 'n',
      );
      my (@cb);
      my $rows = 1+$#ExtOT/3;
      my ($r,$c,$cols)=(1,0,3);
      $ETdb->add('Label',
         -text => "Select new observation type",
         -justify => 'center')->grid(-columnspan => "$cols");
      my $f1=$ETdb->Frame(-borderwidth => 2, -relief => 'groove')->grid();
      foreach $i (0..$#ExtOT) {
         my $cb = $f1->Checkbutton(
            -text => $ExtOT[$i] . " : " . $ExtDesc[$i],
            -variable => \$ExtSelect[$i],
            -relief => 'flat')->grid(
            -sticky => 'w',
            -row => $r,
            -column => $c,
            -ipadx => '2');
         $r++;
         if($r > $rows) { $r = 1; $c++; }
      }
      my $cb1 = $f1->Button(
         -text => 'Clear',
         -width => '15',
         -relief => 'groove',
         -command => \&ClearExot)->grid(-columnspan => "$cols");
   }
}

#-------------------------------------------------------------------------------------
sub ClearExot {
   foreach $i (0..$#ExtOT) { $ExtSelect[$i] = 0; }
}

#-------------------------------------------------------------------------------------
sub CompNavf {
   Status("Get the name of a Rinex navigation file...");
   $filename = $w_top->getOpenFile(-title => 'Choose a Rinex nav file');
   if($filename eq '') { return; }
   Status("The Rinex navigation file is $filename");
   # ought to check that it is a Rinex nav file...
   $CFG{'nav'} = $filename;
}

#-------------------------------------------------------------------------------------
sub CompPosnSele {
   Status("Select a receiver position");
   $buffer='';
   $len=0;
   if (not open(F, "<positions.txt")) {
      $w_top->Dialog(
         -title => 'File \'positions.txt\' not found',
         -text => "Could not open the file \'positions.txt\'",
         -bitmap => 'error',
      )->Show;
      return;
   }
   $len = read(F, $buffer, 100000);
   #print LOG "Read $len bytes\n";
   close F;
   my $CPdb = $w_top->DialogBox(
      -title => "Compute/Receiver position",
      -buttons => ['Ok','New','Cancel'],
      -default_button => 'Ok',
      -popover => $w_top,
      -overanchor => 'c',
      -popanchor => 'c',
   );
   $CPdb->Label(
      -text => "Select a receiver position by highlighting\n"
         . "a selection and pushing \'Ok\'.\n\n"
         . "( The current Rinex observation file name is\n" . $CFG{'Rinex'}
         . " )\n",
      -justify => 'left',
   )->pack(-side => 'top',-anchor => 'w');
   my $list_win = $CPdb->Scrolled('Listbox',
      -setgrid => 'true',
      -selectmode => 'single',
      -scrollbars => 'e',
      -height => '20',   #lines
      -width => '40',   #characters
      );
   $list_win->pack(-expand => 'yes', -fill => 'both');
   #$list_win->bind('<Double-1>' =>
   #sub {
   #print LOG "You selected " . $_[0]->get('active') . "\n";
   #}
   #);
   my @list = split("\n", $buffer);
   $list_win->insert(0, @list);
   $ans = $CPdb->Show();
   #print LOG "ans is $ans\n";
   if($ans eq 'Ok') {
      #print LOG "You selected " . $list_win->get('active') . "\n";
      ($RxLabel,$RxX,$RxY,$RxZ,$RxComment) = split('\s+',$list_win->get('active'));
   }
   elsif($ans eq 'New') {
      Status("Compute/Receiver Position : Enter a new Receiver position");
      my $NRdb = $w_top->DialogBox(
         -title => 'Enter New Receiver',
         -buttons => ['Ok','Cancel'],
         -default_button => 'Ok',
         -popover => $w_top,
         -overanchor => 'c',
         -popanchor => 'c',
      );

      my ($x,$y,$z,$label,$comment)=('','','','name','');

      my $f1=$NRdb->Frame(-borderwidth => 2, -relief => 'flat'
         )->pack(-anchor => 'w');
      $NRdb->add('Label',-text => "\nECEF XYZ Coordinates (meters) :"
         )->pack(-anchor => 'w');
      my $f2=$NRdb->Frame(-borderwidth => 2, -relief => 'groove'
         )->pack(-anchor => 'w');
      $NRdb->add('Label',-text => "\nComment (optional)"
         )->pack(-anchor => 'w');
      my $f3=$NRdb->Frame(-borderwidth => 2, -relief => 'flat'
         )->pack(-anchor => 'w');

      $f1->Label(-text => "Site label for New Receiver (required, 4 characters) : "
         )->pack(-side => 'left');
      my $e1=$f1->Entry(-textvariable => \$label, -width => 5
         )->pack(-side => 'left');
      $f2->Label(-text => "X : ")->pack(-side => 'left');
      $f2->Entry(-textvariable => \$x, -width => 15)->pack(-side => 'left');
      $f2->Label(-text => "  Y : ")->pack(-side => 'left');
      $f2->Entry(-textvariable => \$y, -width => 15)->pack(-side => 'left');
      $f2->Label(-text => "  Z : ")->pack(-side => 'left');
      $f2->Entry(-textvariable => \$z, -width => 15)->pack(-side => 'left');
      #$f3->Label(-text => "Comment: ")->pack(-side => 'left');
      $f3->Entry(-textvariable => \$comment, -width => 60)->pack(-side => 'left');

      $e1->focus;
      $e1->icursor('end');
      $ans = $NRdb->Show();
      if($ans eq 'Ok') {
         #print LOG "New Rx position is $label, $x, $y, $z, $comment\n";
         $RxLabel = $label;
         $RxX = $x;
         $RxY = $y;
         $RxZ = $z;
         $RxComment = $comment;
         # TD add it to positions.txt
      }
   }
   print LOG "Selected Rx Position is $RxLabel: ($RxX, $RxY, $RxZ) $RxComment\n";
   Status('');
}

#-------------------------------------------------------------------------------------
sub CompPosnRAIM {
   PopNotice("Compute/Rx Position/RAIM",
      "Compute/Rx Position/RAIM is not yet implemented");
}

#-------------------------------------------------------------------------------------
sub CompConf {
   my ($cf,$ca)=(0,0);
   if($CFG{'Cforce'} eq 'on') { $cf=1; }
   if($CFG{'Callow'} eq 'on') { $ca=1; }
   Status("Modify computation configuration as preferred");
   my $CCdb = $w_top->DialogBox(
      -title =>  "Computation configuration",
      -buttons => ['Ok','Cancel'],
      -default_button => 'Ok',
      -popover => $w_top,
      -overanchor => 'c',
      -popanchor => 'c',
   );
   $CCdb->add('Label',-text => "Computation Configuration")->pack(-anchor => 'w');
   my $f2=$CCdb->Frame(-borderwidth => 2, -relief => 'flat')->pack(-anchor => 'w');
   $f2->Checkbutton(
         -text => "Allow C1 to replace missing P1",
         -variable => \$ca,
         -relief => 'flat')->pack(-anchor => 'w');

   my $f1=$CCdb->Frame(-borderwidth => 2, -relief => 'flat')->pack(-anchor => 'w');
   $f1->Checkbutton(
         -text => "Force C1 to replace P1",
         -variable => \$cf,
         -relief => 'flat')->pack(-anchor => 'w');

   $ans = $CCdb->Show();
   if($ans eq 'Ok') {
      if($ca) { $CFG{'Callow'} = 'on'; } else { $CFG{'Callow'} = 'off'; }
      if($cf) { $CFG{'Cforce'} = 'on'; } else { $CFG{'Cforce'} = 'off'; }
   }
   Status('');
}

#-------------------------------------------------------------------------------------
sub CompCrea {
   Status("Create new observation types by computation...");
   my ($depend,$have,$ok,$need)=(0,0,0,0);
   my @opt = ();  # options passed to ResCor
   $dummy = 0;    # count the number of selected ExtOT
   for($i=0; $i <= $#ExtOT; $i++) {
      if($ExtSelect[$i] != 0) { $dummy++; $depend |= $ExtDep[$i]; };
   }
   if($dummy == 0) {
      $msg = "There are no extended obs types defined.\n"
         . "Go to Compute/Select New Obs types to make a choice      ";
      PopNotice("Error: no new obs types",$msg);
      return;
   }

   foreach $i (0..$numobs-1) {
      if($obslist[$i] eq 'C1') { $have |= $DepC1; }
      if($obslist[$i] eq 'L1') { $have |= $DepL1; }
      if($obslist[$i] eq 'L2') { $have |= $DepL2; }
      if($obslist[$i] eq 'P1') { $have |= $DepP1; }
      if($obslist[$i] eq 'P2') { $have |= $DepP2; }
      if($CFG{'nav'} ne '') { $have |= $DepEP; }
      if($RxLabel ne '') { $have |= $DepPS; }
   }
   #if(($have & $DepC1) && ($have & $DepP1)) {
      #print LOG "Dep bits for C1 and P1 are " . sprintf("%X %X\n",$DepC1,$DepP1);
      #print LOG "old have is " . sprintf("%X",$have) . "\n";
      #$have |= $DepP1;
      #$have &= ~($DepC1);
      #print LOG "new have is " . sprintf("%X",$have) . "\n";
   #}
   $ok = $depend & $have;   # those we need AND have
   $need = $ok ^ $depend;   # those we need but do NOT have
   # can C1 substitute for P1?
   if(($need & $DepP1) && ($have & $DepC1)
      && ($CFG{'Callow'} eq 'on' || $CFG{'Cforce'} eq 'off')) {
      $need &= ~($DepP1);
   }
   if($need != 0) {
      my $needdata = 0;
      $msg = "The selected new observation types cannot            \n"
         . "be computed because some input is missing :\n\n";
      if($need & $DepL1) { $msg = $msg . " L1 is missing\n"; $needdata++; }
      if($need & $DepL2) { $msg = $msg . " L2 is missing\n"; $needdata++; }
      if($need & $DepP1) {
         if($have & $DepC1) {
            if($CFG{'Callow'} eq 'off') {
               $msg = $msg . " P1 is missing (Hint: Select 'Allow use of C1'\n"
               . "   under Compute/Configure)\n";
               $needdata++;
            }
         }
         else { $msg = $msg . " P1 is missing\n"; $needdata++; }
      }
      if($need & $DepP2) { $msg = $msg . " P2 is missing\n"; $needdata++; }
      #if($need & $DepEP) { $msg = $msg . " EP is missing\n"; }
      #if($need & $DepPS) { $msg = $msg . " PS is missing\n"; }
      #$msg = $msg . "\n";
      if($needdata != 0) { $msg = $msg
         . "\n-> Computation cannot proceed with this obs file.         \n";
      }
      if($need & $DepEP) { $msg = $msg
         . "\n-> Go to Compute/Nav File to select a navigation file.        \n";
      }
      if($need & $DepPS) { $msg = $msg
         . "\n-> Go to Compute/Rx Position to input a receiver position.         \n";
      }
      PopNotice("Error: Some input missing",$msg);
      #$ans = $w_top->messageBox(-title => 'Error: Some input missing',
      #-message => $msg, -type => 'OK', -icon => 'info');
      return;
   }

   # call ResCor
   WaitCursor();
   Status("Creating new observation types...please wait");
   $ResCorfilename = basename($CFG{'Rinex'}) . ".rc";
   print LOG "Create new obs types; output to file $ResCorfilename\n";
   push @opt, "-IF$CFG{'Rinex'}";
   push @opt, "-OF$ResCorfilename";
   if($CFG{'nav'} ne '') { push @opt, "--nav $CFG{'nav'}"; }
   if($RxX ne '') { push @opt, "--RxXYZ $RxX,$RxY,$RxZ"; }
   #TD allow this to be configured
   foreach $i (0..$#ExtOT) { if($ExtSelect[$i] != 0) {
      push @opt, "-AO$ExtOT[$i]";
      if($MayDebias{$ExtOT[$i]}) {
         push @opt, "--debias $ExtOT[$i],100";
      }
   } }
   # use C1 when P1 is not there
   if(($have & $DepC1) && $CFG{'Callow'} eq 'on') { push @opt, "--Callow"; }
   if($CFG{'Cforce'} eq 'on') { push @opt, "--Cforce"; }
   push @opt, "--verbose";

   $cmd = $CFG{'prgmdir'} . $SLASH . "ResCor " . " @opt";
   
   print LOG "Create new obs types: Execute $cmd\n";
   $buffer = `$cmd`;
   #print LOG "Result is\n$buffer\n";
   Status("New observation types are ready");
   $RinexRCSummary = $ResCorfilename . ".sum";
   $cmd = $CFG{'prgmdir'} . $SLASH . "RinSum -g -i" . $ResCorfilename
      . " > $RinexRCSummary";
   Status("Summarizing new obs file $filename...please wait...");
   print LOG "Summarize obs file: Execute $cmd\n";
   $dummy = `$cmd`; if($CFG{'verbose'} eq 'on') { print LOG $dummy; }
   if($CFG{'autoview'} eq 'on') { FileSumm(); }
   $ans = DataGather($RinexRCSummary);
   NormalCursor();
   Status("Summary complete for $filename.");
}

#-------------------------------------------------------------------------------------
sub GrapLims {
   #print LOG "Graph/Limits\n";
   ConfigureCurves();

   # put up a dialog box with number entry box
   Status("Graph/Limits: enter new limits, or autoscale (determine from data)");
   my $DLdb = $w_top->DialogBox(
      -title => 'Plotting Limits',
      -buttons => ['Ok','Auto','Cancel'],
      -default_button => 'Ok',
      -popover => $w_top,
      -overanchor => 'c',
      -popanchor => 'c',
   );

   ($YMin,$YMax,$XMin,$XMax)=($CFG{'YMin'},$CFG{'YMax'},$CFG{'XMin'},$CFG{'XMax'});

   $DLdb->add('Label',-text => "Y: data")->pack(-anchor => 'w');
   my $f1=$DLdb->Frame(-borderwidth => 2, -relief => 'groove')->pack(-anchor => 'w');
   $DLdb->add('Label',-text => "X: time")->pack(-anchor => 'w');
   my $f2=$DLdb->Frame(-borderwidth => 2, -relief => 'groove')->pack(-anchor => 'w');

   #$f1->Label(-text => "Y: data :    ")->pack(-side => 'left');
   my $e1=$f1->Entry(-textvariable => \$YMin, -width => 15)->pack(-side => 'left');
   $f1->Entry(-textvariable => \$YMax, -width => 15)->pack(-side => 'left');

   #$f2->Label(-text => "X: time : ")->pack(-side => 'left');
   $f2->Entry(-textvariable => \$XMin, -width => 15)->pack(-side => 'left');
   $f2->Entry(-textvariable => \$XMax, -width => 15)->pack(-side => 'left');

   $e1->focus;
   $e1->icursor('end');
   $ans = $DLdb->Show();
   if($ans eq 'Ok') {
      print LOG "Graph/Limits are Y: $YMin, $YMax, X: $XMin, $XMax\n";
      NewScale('fixed','Graph/Limits');
      Refresh();
   }
   elsif($ans eq 'Auto') {
      autoscale();
      Refresh();
   }
   else { Status(''); }
}

#-------------------------------------------------------------------------------------
sub AutoScale {
   my $fas=$FirstAutoscale;
   ConfigureCurves();
   if($fas == 0) { autoscale(); Refresh(); }
}

#-------------------------------------------------------------------------------------
# compute max and min y and x from file(s) -- consider time limits
sub autoscale {
   Status("Autoscale - redefine limits using data min/max");
   $filename = "$CFG{'datafile'}";
   open FILE, "$filename" or die "Error: autoscale could not open $filename\n";
   my ($first,$firstime,$thistime,$sat)=(1,1,0,'');
   my ($bw,$bs)=split(',',$CFG{'begin'});
   my ($ew,$es)=split(',',$CFG{'end'});
   while(<FILE>) {
      chomp;
      $_ =~ s/^\s_//;
      if(m/Rinexdump/) { next; }
      if(m/Week/) { next; }
      s/^\s+//;               # no leading white space
      @opt = split(/\s+/, $_);
      #$week = $opt[0];
      #$sow = $opt[1];
      next if($opt[0] < $bw);
      next if($opt[0] == $bw && $opt[1] < $bs);
      last if($opt[0] > $ew);
      last if($opt[0] == $ew && $opt[1] > $es);
      $sat = $opt[2];
      $thistime = 0;
      foreach $j (0..$ncurv-1) {
         if($curvON[$j] == 0) { next; }
         if($curvSV[$j] eq $sat) {
            $thistime = 1;
            if($first == 1) {
               $YMax = $opt[$curvCol[$j]];
               $YMin = $opt[$curvCol[$j]];
               $first = 0;
            }
            else {
               if($opt[$curvCol[$j]] < $YMin) { $YMin = $opt[$curvCol[$j]]; }
               if($opt[$curvCol[$j]] > $YMax) { $YMax = $opt[$curvCol[$j]]; }
            }
         }
      }
      if($thistime == 1) {
         if($firstime == 1) {
            $XMax = $opt[1];
            $XMin = $opt[1];
            $CFG{'Week'} = $opt[0];
            $firstime = 0;
         }
         else {
            $i = ($opt[0]-$CFG{'Week'})*604800 + $opt[1];
            if($i < $XMin) { $XMin = $i; }
            if($i > $XMax) { $XMax = $i; }
         }
      }
   }

   if($firstime == 1) {
      PopNotice("Graph/Autoscale","Autoscale finds no data (within time limits)!");
   }
   else {
      # NewScale requires $XMin,$XMax,$YMin,$YMax as input
      NewScale('auto','Autoscale');
   }
   if($UsingDefaults) { DefaultLabels(); }
}

#-------------------------------------------------------------------------------------
sub NiceScale {
   # call with args Min Max Ntics Npixels
   # results stored in $ScaleMin $ScaleStep $ScaleExp $ScaleN
   my ($dmin,$dmax,$ntic,$npix)=@_;
   #print LOG "NiceScale called with $dmin $dmax $ntic $npix\n";
   ($ScaleMin,$ScaleStep,$ScaleExp,$ScaleN,$ScalePPL)=(0,0,0,0);
   if($dmax < $dmin) { $dummy=$dmax; $dmax=$dmin; $dmin=$dummy; }
   if($npix < 2 || $npix > 10000) {
      print LOG "NiceScale: Number of pixels is unreasonable\n";
      return;
   }
   $ScalePPL = ($npix-1)/($ntic-1);        # pixels per label
   if($ScalePPL <= 0 || $ScalePPL >= $npix) {
      print LOG "NiceScale: Number of pixels per label is unreasonable\n";
      return;
   }
   $ScaleN = 1+($npix-1)/$ScalePPL;
   $dummy = abs($dmax);
   if(abs($dmin) > $dummy) { $dummy=abs($dmin); }
   if($dmax-$dmin <= 5.0e-8 * $dummy) {   # effectively equal limits
      if($dmax < 0)     { $dmax=0.0; }
      elsif($dmax == 0) { $dmax=1.0; }
      elsif($dmax > 0)  { $dmax=0.0; }
   }
   my $finter=$npix/$ScalePPL;
   #print LOG "PPL is $ScalePPL, and finter is $finter\n";
   my $scale=($dmax-$dmin)*1.00002/$finter;
   $ScaleExp = 0;
   while($scale <= 10) { $scale *= 10; $ScaleExp++; }
   while($scale > 100) { $scale /= 10; $ScaleExp--; }
   #print LOG "scale is $scale\n";
   #print LOG "Exponent is $ScaleExp\n";
   my $iunit;
   foreach $i (1..$#NiceUnits-1) {
      $iunit = $i;
      if($scale <= $NiceUnits[$i]) { last; }
   }
   #print LOG "NiceUnit[$iunit] is $NiceUnits[$iunit]\n";
   my ($istay,$aj,$tstep)=(1,0,0);
   while($istay == 1) {
      $ScaleStep = $NiceUnits[$iunit] / (10.0 ** $ScaleExp);
      $aj = 0;
      while(1) {
         $aj++;
         $dummy = int(($NiceUnits[$iunit]+0.1)/$aj);
         if($NiceUnits[$iunit]-0.1 > $aj*$dummy) { next; }
         $tstep = $ScaleStep/$aj;
         $dummy = $dmin/$tstep + $aj*(0.5/$ScalePPL - $finter*1.0e-5);
         $ScaleMin = $tstep * int($dummy);
         if($dummy < 0 && $dummy != int($dummy)) { $ScaleMin -= $tstep; }
         if($dmax < $ScaleMin + $ScaleStep*($finter*0.99999-(0.5/$ScalePPL))) {
            $istay=0; last;
         }
         $dummy = 1.0 - 1.0/($aj * $finter);
         # TD this fails occasionally: divide by zero
         if(($NiceUnits[$iunit]/$NiceUnits[$iunit+1])*$dummy < 0.7) {
            next;
         }
         $iunit++;
         if($iunit > 11) { $istay=0; last; }   # TD ??
      }
   }
   foreach $i (1,2) {
      $aj *= 10;
      if($NiceUnits[$iunit]-0.1 < $aj*int(($NiceUnits[$iunit]+0.1)/$aj)) {
         $ScaleExp--;
      }
   }

   #print LOG "NiceScale finds exp $ScaleExp ";
   #foreach $i (0..$ScaleN-1) {
   #   $dummy = $ScaleMin + $i * $ScaleStep;
   #   # $dummy are labels; multiply by 10**ScaleExp to make them all integers
   #   if($ScaleExp > 0) { print LOG sprintf(" %.$ScaleExp"."f",$dummy); }
   #   else { print LOG " $dummy"; }
   #}
   #print LOG "\n";
}

#-------------------------------------------------------------------------------------
sub GrapAxes {
   PopNotice("Graph/Axes","Graph/Axes is not yet implemented");
}

#-------------------------------------------------------------------------------------
sub GrapLabe {
   Status("Labels on the graph");
   my $GLdb = $w_top->DialogBox(
      -title =>  "Graph Labels",
      -buttons => ['Ok','Default','Clear'],
      -default_button => 'Ok',
      -popover => $w_top,
      -overanchor => 'c',
      -popanchor => 'c',
   );
   $GLdb->add('Label',-text => "Title:")->pack(-anchor => 'w');
   my $f1=$GLdb->Frame(-borderwidth => 2, -relief => 'groove')->pack(-anchor => 'w');
   $GLdb->add('Label',-text => "Bottom (X) axis label:")->pack(-anchor => 'w');
   my $f2=$GLdb->Frame(-borderwidth => 2, -relief => 'groove')->pack(-anchor => 'w');
   $GLdb->add('Label',-text => "Left (Y) axis label:")->pack(-anchor => 'w');
   my $f3=$GLdb->Frame(-borderwidth => 2, -relief => 'groove')->pack(-anchor => 'w');
   $GLdb->add('Label',-text => "Right (Y) axis label:")->pack(-anchor => 'w');
   my $f4=$GLdb->Frame(-borderwidth => 2, -relief => 'groove')->pack(-anchor => 'w');

   my $len = length($CFG{'Tlabel'});
   if($len < length($CFG{'Blabel'})) { $len = length($CFG{'Blabel'}); }
   if($len < length($CFG{'Llabel'})) { $len = length($CFG{'Llabel'}); }
   if($len < length($CFG{'Rlabel'})) { $len = length($CFG{'Rlabel'}); }
   if($len < 20) { $len = 20; }

   my $e1=
   $f1->Entry(-textvariable => \$CFG{'Tlabel'},-width => $len)->pack(-side => 'left');
   $f2->Entry(-textvariable => \$CFG{'Blabel'},-width => $len)->pack(-side => 'left');
   $f3->Entry(-textvariable => \$CFG{'Llabel'},-width => $len)->pack(-side => 'left');
   $f4->Entry(-textvariable => \$CFG{'Rlabel'},-width => $len)->pack(-side => 'left');

   $ans = $GLdb->Show();
   $e1->focus; $e1->icursor('end');
   if($ans eq 'Default') {
      DefaultLabels();
      GrapLabe();
   }
   if($ans eq 'Clear') {
      $CFG{'Tlabel'}='';
      $CFG{'Blabel'}='';
      $CFG{'Llabel'}='';
      $CFG{'Rlabel'}='';
      GrapLabe();
   }
   else {
      $UsingDefaults = 0;
   }
   Status('');
}

#-------------------------------------------------------------------------------------
sub GrapCurv {
   ConfigureCurves();

   if($ncurv <= 0) {
      $msg = "There are no curves defined.\n" .
         "Go to DataSet/Select to make a choice      ";
      PopNotice("Error: no curves",$msg);
      return;
   }

   Status("Curves to be plotted");
   #PopNotice("Graph/Curves","Graph/Curves is not yet implemented");
   my $GCdb = $w_top->DialogBox(
      -title => 'Curves defined for plotting',
      -buttons => ['Ok','Cancel'],
      -default_button => 'Ok',
      -popover => $w_top,
      -overanchor=> 'n',
      -popanchor => 'n',
   );

   $GCdb->add('Label', -text => "CURVES", -justify => 'center')->pack(-side => 'top');
   $GCdb->add('Label', -text =>
      ' N   On          Sat         OT       Color    Line      Symbol              ',
      #-font => 'C_small',
      -justify => 'left')->pack(-side => 'top');
   my @fr;   # frame for each curve
   my @sats = @curvSV;
   my @otss = @curvOT;
   my @swit = @curvON;
   my @line = @curvLines;
   my @symb = @curvSymbs;
   my @colo = @curvColor;
   my @cb;
   $dummy = '   ';
   foreach $i (0..$ncurv-1) {
      # frame for each curve
      $fr[$i]=$GCdb->Frame(-borderwidth => 2, -relief => 'groove'
         )->pack(-side => 'top');
      # label and entry for Sat
      $fr[$i]->Label(-text => sprintf("%2d",$i), -font => 'C_small'
         )->pack(-side => 'left');
      $fr[$i]->Checkbutton(-text => '  ', -variable => \$swit[$i],
         -relief => 'flat')->pack(-side => 'left');
      $fr[$i]->Label(-text => " ")->pack(-side => 'left');
      $fr[$i]->Entry(-textvariable => \$sats[$i], -width => 4)->pack(-side => 'left');
      $fr[$i]->Label(-text => " ")->pack(-side => 'left');
      $fr[$i]->Entry(-textvariable => \$otss[$i], -width => 3)->pack(-side => 'left');
      $fr[$i]->Label(-text => "  ")->pack(-side => 'left');
      # color - colored button calls up color selection dialog
      $cb[$i] = $fr[$i]->Button(-text => '  ',
         -width => 2,
         -background => $colo[$i],
         -activebackground => $colo[$i],
         -command => sub {
            $dummy = $fr[$i]->chooseColor(
               -title => 'choose a new color',
               -initialcolor => $colo[$i]);
            if($dummy ne '') {
               $colo[$i] = $dummy;
               $cb[$i]->configure('-background' => $colo[$i],
                               '-activebackground' => $colo[$i]);
            }
         })->pack(-side => 'left', -ipadx => '2');
      # line checkbox
      $fr[$i]->Checkbutton(-text => '', -variable => \$line[$i],
         -relief => 'flat')->pack(-side => 'left', -ipadx => '4');
      # symbol browseentry
      $fr[$i]->BrowseEntry(-label => ' ',
         -variable => \$symb[$i],
         -choices => \@symbols,
         -width => 6,
         -listwidth => 18,
         -state => 'readonly',
         )->pack(-side => 'left', -ipadx => '2');
      #space on the right
      $fr[$i]->Label(-text => " ")->pack(-side => 'left');
   }

   $ans = $GCdb->Show();
   Status('');
   if($ans eq "Cancel") { return; }

   $dummy = 0;
   foreach $i (0..$#curvON) {
      if($curvON[$i] != $swit[$i]) {
         if($dummy == 0) { print LOG "Graph/Curves toggles"; $dummy=1; }
         print LOG " $i:";
         if($swit[$i] == 0) { print LOG "OFF"; } else { print LOG "ON"; }
      }
   }
   if($dummy != 0) { print LOG "\n"; }
   @curvON = @swit;
   @curvLines = @line;
   @curvSymbs = @symb;
   @curvColor = @colo;
}

#sub GrapColo {
#   #print LOG "Graph/Colors\n";
#   $FCOLOR= $w_top->chooseColor(-title => "Choose color for plot",
#      -initialcolor => $FCOLOR);
#   print LOG "New color is $FCOLOR\n";
#}

#-------------------------------------------------------------------------------------
sub GrapZoom {
   # (for some reason, when called from menu, arguments are different)
   my $io;
   ($dummy, $io) = @_;           # ignore dummy here
   $dummy = $CFG{'zoomX'} / 2;

   my $size=$CFG{'XMax'}-$CFG{'XMin'};
   if($io eq 'In') { $size = -$size; }
   $XMin = $CFG{'XMin'} - $CFG{'zoomX'} * $size;
   $XMax = $CFG{'XMax'} + $CFG{'zoomX'} * $size;

   $size=$CFG{'YMax'}-$CFG{'YMin'};
   if($io eq 'In') { $size = -$size; }
   $YMin = $CFG{'YMin'} - $CFG{'zoomX'} * $size;
   $YMax = $CFG{'YMax'} + $CFG{'zoomX'} * $size;

   # NewScale requires $XMin,$XMax,$YMin,$YMax as input
   NewScale('fixed','Graph/Zoom');
   Refresh();
}

#-------------------------------------------------------------------------------------
sub GrapUnzo {
   if($OldXMin eq '') { return; }
   ($XMin,$XMax,$YMin,$YMax) = ($OldXMin,$OldXMax,$OldYMin,$OldYMax);
   # NewScale requires $XMin,$XMax,$YMin,$YMax as input
   NewScale('fixed','Graph/Unzoom');
   Refresh();
}

#-------------------------------------------------------------------------------------
sub GrapGnup {
   PopNotice("Graph/Gnuplot","Graph/Gnuplot is not yet implemented.");
}

#-------------------------------------------------------------------------------------
sub HelpTopi {
   #PopNotice("Help/Topics","Help/Topics is    \nnot yet implemented.");

   #if(! Exists($HTmw)) {  # not necessary
   my $HTmw = MainWindow->new();
   $HTmw->title("Help on RinexPlot");
   my $b = $HTmw->Button(
      -text => 'Close this Help window',
      -width => '40',
      -command => sub { $HTmw->withdraw },
      )->pack();
   my $t = $HTmw->Scrolled('ROText',
      -setgrid => 'true',
      -width => '65',
      -height => '30',
      -font => 'normal',
      -wrap => 'word',
      -scrollbars => 'e');
   $t->pack(qw/-expand yes -fill both/);

   # Set up display styles.
   $HTmw->fontCreate(qw/C_small -family courier   -size 10/);
   $HTmw->fontCreate(qw/C_big   -family courier   -size 14 -weight bold/);
   $HTmw->fontCreate(qw/C_vbig  -family helvetica -size 24 -weight bold/);
   $HTmw->fontCreate(qw/C_bold  -family courier -size 12 -weight bold -slant italic/);
   $t->tag(qw/configure bold    -font C_bold/);
   $t->tag(qw/configure big     -font C_big/);
   $t->tag(qw/configure verybig -font C_vbig/);
   $t->tag(qw/configure small   -font C_small/);
   if ($HTmw->depth > 1) {
   $t->tag(qw/configure color1 -background/ => '#a0b7ce'); #this color is "MSWin blue"
   $t->tag(qw/configure color2 -foreground red/);
   $t->tag(qw/configure raised -relief raised -borderwidth 1/);
   $t->tag(qw/configure sunken -relief sunken -borderwidth 1/);
   } else {
   $t->tag(qw/configure color1 -background black -foreground white/);
   $t->tag(qw/configure color2 -background black -foreground white/);
   $t->tag(qw/configure raised -background white -relief raised -bd 1/);
   $t->tag(qw/configure sunken -background white -relief sunken -bd 1/);
   }
   $t->tag(qw/configure bgstipple  -background black -borderwidth 0
      -bgstipple gray12/);
   $t->tag(qw/configure fgstipple  -fgstipple gray50/);
   $t->tag(qw/configure underline  -underline on/);
   $t->tag(qw/configure overstrike -overstrike on/);
   $t->tag(qw/configure right      -justify right/);
   $t->tag(qw/configure center     -justify center/);
   $t->tag(qw/configure super      -offset 4p -font C_small/);
   $t->tag(qw/configure sub        -offset -2p -font C_small/);
   $t->tag(qw/configure margins    -lmargin1 12m -lmargin2 6m -rmargin 10m/);
   $t->tag(qw/configure spacing     -spacing1 10p -spacing2 2p
      -lmargin1 12m -lmargin2 6m -rmargin 10m/);


   # now insert text
   $t->insert('0.0', "RinexPlot is a GUI for the GPS Toolkit (gpstk) utility "
      . "programs that will read, manipulate and plot data in a Rinex file.\n\n",
      'big');
   $t->insert('insert',"Here is the general flow of things:\n");
   $t->insert('insert',"  1.",'big');
   $t->insert('insert'," Select a Rinex observation file by choosing "
     . "'File/Obs file' from the main menu. Wait for the summary window to come up,"
     . " then close it to return to the main menu.\n");
   $t->insert('insert',"  2.",'big');
   $t->insert('insert'," (2-4 Optional) Select new observation types by choosing "
     . "'Compute/New Data types' from the main menu\n");
   $t->insert('insert',"  3.",'big');
   $t->insert('insert'," Choose a Rinex navigation file (Compute/Nav file)"
     . " and a Receiver position (Compute/Rx position)...these may or may not be "
     . "required, depending on the choices you made in Step 2 (Step 4 will inform"
     . " you if more input is needed.\n");
   $t->insert('insert',"  4.",'big');
   $t->insert('insert'," Compute the new types by choosing 'Compute/Create new data'."
     . " (This step is important! - if your new obs types do not show up under"
     . " Dataset/Obs types, you may have skipped this step.)\n");
   $t->insert('insert',"  5.",'big');
   $t->insert('insert'," Select dataset(s) to plot by choosing 'Dataset/Satellites'"
     . " AND 'Dataset/Obs types'; optionally also limit the plot in time using"
     . " 'Dataset/Times', and in other ways under Dataset/Configure.\n");
   $t->insert('insert',"  6.",'big');
   $t->insert('insert'," Now press 'Refresh' to re-draw the screen...whenever the "
     . "data and/or graph are changed, press Refresh to draw the new plot.\n");
   $t->insert('insert',"  7.",'big');
   $t->insert('insert'," Features of the plot, such as limits and colors, can be seen"
     . " and set under 'Graph' on the main menu.\n");
   $t->insert('insert',"  8.",'big');
   $t->insert('insert'," Preferences and switches can be set under 'Settings' on the"
     . " main menu.\n");
  #$t->insert('insert',"  9.",'big');
  #$t->insert('insert',"\n");
   $t->insert('insert',"\n(Usually you can look at the status bar at the bottom of "
     . "the window to get hints about what has just happened or what to do next.)\n");

   $t->insert('insert',"\nSwitches and options\n",'big');
   $t->insert('insert',"- Summary files automatically shown in window.\n");
   $t->insert('insert',"- Refresh automatically after selection/zoom/resizing.\n");
   $t->insert('insert',"- GPS satellites only.\n");
   $t->insert('insert',"- Output to a log file or the screen\n");
   $t->insert('insert',"- Directory of GPSTk programs\n");

   $t->insert('insert',"\nCommand line arguments:\n",'big');
   $t->insert('insert',"  Command line arguments may come in any order, but note that"
     . " many depend on others and have no effect unless those others are present."
     . "\n\n");
   $t->insert('insert',"All command line options:\n");
   $t->insert('insert',"  [Note pattern: --opt <arg> OR --opt=<arg>]\n",'small');
   $t->insert('insert',$CmdText,'small');

   $t->insert('insert',"\nMouse things\n",'big');
   $t->insert('insert',"- Click on any point in the graph; the status bar will show "
     . "coordinates (both screen and data).\n");
   $t->insert('insert',"- Click, drag and un-click to make a rectangle on the graph."
     . " A box pops up allowing you to zoom the plot to the rectangle.\n");

   $t->mark(qw/set insert 0.0/);

   #} else {       # HTmw already exists
   #$HTmw->deiconify();
   #$HTmw->raise();
   #}
}

#-------------------------------------------------------------------------------------
sub HelpAbou {
   #print LOG "Help/About\n";
   my $HAdb = $w_top->DialogBox(
      -title => 'About RinexPlot',
      -buttons => ['Ok'],
      -popover => $w_top,
      -overanchor => 'n',
      -popanchor => 'n',
   );
   $HAdb->add('Label', -text => $ABOUT_TEXT,
      -justify => 'left')->pack;
   $HAdb->add('Label', -text => $AUTHOR_TEXT,
      -justify => 'left')->pack;
   $HAdb->add('Label', -text => "Version is " . $VERSION . "\n",
      -justify => 'left')->pack;
   $HAdb->add('Label', -text => "Operating System is $^O",
      -justify => 'left')->pack;
   $HAdb->Show();
}

#-------------------------------------------------------------------------------------
sub SettPref {
   # menu tearoff (command line only - before creation) my $menutear = 1; ???
   # (to do this you would have to scan args before calling init() )
   my ($av,$ud,$lf,$zf)=(0,$CFG{'prgmdir'},$CFG{'log'},$CFG{'zoomX'});
   my ($vb,$lr,$lo,$so)=(0,0,0,0);
   if($CFG{'autoview'} eq 'on') { $av=1; }
   if($CFG{'verbose'} eq 'on') { $vb=1; }
   if($CFG{'keepdata'} eq 'on') { $lr=1; }
   if($CFG{'lines'} eq 'on') { $lo=1; }
   if($CFG{'points'} eq 'on') { $so=1; }
   Status("Modify settings of global configuration as preferred");
   my $SPdb = $w_top->DialogBox(
      -title =>  "Preferences",
      -buttons => ['Ok','Cancel'],
      -default_button => 'Ok',
      -popover => $w_top,
      -overanchor => 'c',
      -popanchor => 'c',
   );
   $SPdb->add('Label',-text => "Global Settings:")->pack(-anchor => 'w');
   # AutoView
   my $f1=$SPdb->Frame(-borderwidth => 2, -relief => 'groove')->pack(-anchor => 'w');
   $f1->Checkbutton(
         -text => "Autoview: automatically show summary file  ",
         -variable => \$av,
         -relief => 'flat')->pack(-anchor => 'w');
   # Log file name
   my $f2=$SPdb->Frame(-borderwidth => 2, -relief => 'groove')->pack(-anchor => 'w');
   $f2->Label(-text => " Output log file :"
      )->pack(-side => 'top', -anchor => 'w');
   $f2->Label(-text => " ")->pack(-side => 'left');
   if($lf eq '') { $lf="SCREEN"; }
   my $e1=$f2->Entry(-textvariable => \$lf, -width => '20')->pack(-side => 'left');
   $f2->Checkbutton(-text => "Verbose output", -variable => \$vb, -relief => 'flat',
      )->pack(-side => 'left');
   # GPSTk directory
   my $f3=$SPdb->Frame(-borderwidth => 2, -relief => 'groove')->pack(-anchor => 'w');
   $f3->Label(-text => " Directory of GPSTk utilities :"
      )->pack(-side => 'top', -anchor => 'w');
   $f3->Label(-text => " ")->pack(-side => 'left');
   my $e1=$f3->Entry(-textvariable => \$ud, -width => '37')->pack(-side => 'left');
   $e1->focus; $e1->icursor('end');
   # Default lines
   my $f4=$SPdb->Frame(-borderwidth => 2, -relief => 'groove')->pack(-anchor => 'w');
   $f4->Label(-text => "Default curves have:")->pack(-side => 'left');
   $f4->Checkbutton(
         -text => "Lines",
         -variable => \$lo,
         -relief => 'flat')->pack(-side => 'left');
   # Default symbols
   $f4->Checkbutton(
         -text => "Points     ",
         -variable => \$so,
         -relief => 'flat')->pack(-side => 'left');
   # Zoom factor
   my $f5=$SPdb->Frame(-borderwidth => 2, -relief => 'groove')->pack(-anchor => 'w');
   $f5->Label(-text => " Zoom factor (0 < zf < 1) :  ")->pack(-side => 'left');
   my $e2=$f5->Entry(-textvariable => \$zf, -width => '17')->pack(-side => 'left');
   $e2->icursor('end');
   # GPS only
   #my $f5=$SPdb->Frame(-borderwidth => 2, -relief => 'groove')->pack(-anchor => 'w');
   #$f5->Checkbutton(
   #      -text => "GPS satellites only                                        ",
   #      -variable => \$go,
   #      -relief => 'flat')->pack(-anchor => 'w');
   # Leave $CFG{'datafile'}
   my $f6=$SPdb->Frame(-borderwidth => 2, -relief => 'groove')->pack(-anchor => 'w');
   $f6->Checkbutton(
         -text => "Leave data file ($CFG{'datafile'}) after exit                 ",
         -variable => \$lr,
         -relief => 'flat')->pack(-anchor => 'w');

   # put it up
   $ans = $SPdb->Show();
   if($ans eq 'Ok') {
      if($av) { $CFG{'autoview'} = 'on'; } else { $CFG{'autoview'} = 'off'; }
      if($lo) { $CFG{'lines'} = 'on'; } else { $CFG{'lines'} = 'off'; }
      if($so) { $CFG{'points'} = 'on'; } else { $CFG{'points'} = 'off'; }
      $CFG{'zoomX'} = $zf;
      if($lr) { $CFG{'keepdata'} = 'on'; } else { $CFG{'keepdata'} = 'off'; }
      if($lf eq 'SCREEN') { $CFG{'log'} eq ''; } else {
         close LOG;
         $CFG{'log'} = $lf;
         open LOG, ">$CFG{'log'}" or die "Could not open log file $CFG{'log'}\n";
         autoflush LOG;
         print LOG "Log file for RinexPlot\n";
      }
      if($vb) { $CFG{'verbose'} = 'on'; }
      if($ud ne $CFG{'prgmdir'}) {
         my($ResCor,$RinSum,$RinexDump)=('ResCor','RinSum','RinexDump');
         if ($^O eq "MSWin32") {
            $ResCor='ResCor.exe';
            $RinSum='RinSum.exe';
            $RinexDump='RinexDump.exe';
         }
         if(not -e $ud . $SLASH . $RinSum
            || not -e $ud . $SLASH . $ResCor
            || not -e $ud . $SLASH . $RinexDump) {
            PopNotice("Error:","Error: GPSTk utilities not found in directory $ud.\n"
               . "Go to Settings/Preferences to change it");
         }
         else { $CFG{'prgmdir'} = $ud; }
      }
   }
   Status('');
}

#-------------------------------------------------------------------------------------
sub SettCurs {
   ## Open file that contains all available cursors
   ## Might have to change this if your cursorfont.h is elsewhere
   ## On Win32 systems look in C:\Perl\lib\site\Tk\X11\cursorfont.h
   my $cursorfile='/usr/X11R6/include/X11/cursorfont.h';
   if ($^O eq "MSWin32") {
      $cursorfile = '\\perl\\site\\lib\\Tk\\X11\\cursorfont.h';
   }
   my @cursors=();
   open (FH, $cursorfile) or die "Couldn't open cursor file.\n";
   while (<FH>) { push(@cursors, $1) if (/\#define XC_(\w+) /); }
   close(FH);

   my $HSdb = $w_top->DialogBox(
      -title => 'Available cursors',
      -buttons => ['Ok','Cancel'],
      -popover => $w_top,
      -overanchor => 'n',
      -popanchor => 'n',
   );
   my $lab = $HSdb->add('Label',-text =>
      "Select a cursor name by clicking\n" .
      "on it, then move the mouse onto\n" .
      "the canvas to see the result.\n" .
      "The default cursor is crosshair.\n" .
      "The current cursor is $cursor")->pack(-anchor => 'w');
   my $scroll = $HSdb->Scrollbar;
   my $lb = $HSdb->Listbox(-selectmode => 'single',
                   -yscrollcommand => [set => $scroll]);
   $scroll->configure(-command => [yview => $lb]);
   $scroll->pack(-side => 'right', -fill => 'y');
   $lb->pack(-side => 'left', -fill => 'both');
   $lb->insert('end', sort @cursors);
   $lb->bind('<ButtonPress-1>', 
      sub {
         $cursor = $lb->get($lb->curselection);
         $w_canvas->configure(-cursor => $cursor);
         $w_top->update;
         print LOG "Configure cursor $cursor\n";
      }
   );
   $ans = $HSdb->Show();
   if($ans eq 'Cancel') {
      $cursor = $crosshair;
      NormalCursor();
   }
}

#-------------------------------------------------------------------------------------
# this is the guts of OutputCommand() and SaveConfig()
# call with argument 'config' for config file format : ^key = value$
# otherwise for command line format :  (sp)--key value
sub ConfigString {
   ($ans) = @_;
   my $cmd='';
   my ($p1,$p2,$p3)=(' --',' ','');     # for the command line
   if($ans eq 'config') {               # for the config file
      $p1 = '';
      $p2 = ' = ';
      $p3 = "\n";
   }

   foreach $key (sort keys %CFG) {
      next if($CFG{$key} eq '');
      if($CFG{$key} =~ m/\s+/) {
         $cmd = $cmd . $p1 . $key . $p2 . "\"$CFG{$key}\"" . $p3;
      }
      else {
         $cmd = $cmd . $p1 . $key . $p2 . $CFG{$key} . $p3;
      }
   }
   #foreach $key (sort keys %OPT) {
   #   next if($OPT{$key}[0] eq '');
   #   foreach $i (@{$OPT{$key}}) {
   #      $cmd = $cmd . " --$key $i";
   #   }
   #}

   # --AO <OT>        extended obs type
   $dummy = 0;
   foreach $i (0..$#ExtOT) {
      if($ExtSelect[$i] != 0) {
         $dummy =1;
         $cmd = $cmd . $p1 . "AO" . $p2 . $ExtOT[$i] . $p3;
      }
   }
   # --create        call create only if --AO
   if($dummy == 1) {
      $cmd = $cmd . $p1 . "create" . $p2 . "on" . $p3;
   }
   # --sat <sat>      select sat
   $dummy = 0;
   foreach $i (0..$#svlist) {
      if($svselect[$i] != 0) {
         $cmd = $cmd . $p1 . "sat" . $p2 . $svlist[$i] . $p3;
         $dummy = 1;
      }
   }
   # --obs <ot>       select obs type
   $ans = 0;
   foreach $i (0..$#obslist) {
      if($obsselect[$i] != 0) {
         $cmd = $cmd . $p1 . "obs" . $p2 . $obslist[$i] . $p3;
         $ans = 1;
      }
   }
   # --refresh        call refresh only if --sat and --obs
   if($dummy != 0 && $ans != 0) {
      $cmd = $cmd . $p1 . "refresh" . $p2 . "on" . $p3;
   }

   if($ans ne 'config') {
      $cmd = $cmd . "\n";
   }

   return $cmd;
}

#-------------------------------------------------------------------------------------
sub OutputCommand {
   Status("Output the current configuration as a command line...");
   # ask user for name of file to write to ... or screen
   my $file = "LOG";
   my $SOdb = $w_top->DialogBox(
      -title =>  "Command line output",
      -buttons => ['Ok','Cancel'],
      -default_button => 'Ok',
      -popover => 'cursor',
      -overanchor => 'c',
      -popanchor => 'nw',
   );
   #$SOdb->add('Label',-text => "Command line output")->pack(-anchor => 'w');
   my $f1=$SOdb->Frame(-borderwidth => 2, -relief => 'groove')->pack(-anchor => 'w');
   $f1->Label(-text => " Output file name for command line\n"
      . " (SCREEN for stdout, LOG for logfile) :"
      )->pack(-side => 'top', -anchor => 'w');
   my $e1=$f1->Entry(-textvariable => \$file, -width => '30')->pack(-side => 'left');
   $ans = $SOdb->Show();
   if($ans eq 'Ok') {
      my $cmd;
      if($^O eq "MSWin32") { $cmd = "call RinexPlot"; }
      elsif($^O eq "linux") { $cmd = "perl RinexPlot.pl"; }
      else { $cmd = "RinexPlot"; }
   
      $msg = ConfigString();
      $cmd = $cmd . ' ' . $msg;

      if($file eq "SCREEN") {
         open(CMDOUT,">-") or die "Could not re-open STDOUT\n";
         print CMDOUT "$cmd";
      }
      elsif($file eq "LOG") {
         print LOG "$cmd";
      }
      else {
         open CMDOUT, ">$file" or die "Could not open log file $file\n";
         print CMDOUT "$cmd";
         close CMDOUT;
      }
   }
   Status('');
}

#-------------------------------------------------------------------------------------
sub Refresh {
   #print LOG "Refresh\n";
   ConfigureCurves();

   if($ncurv <= 0) {
      $msg = "There are no curves defined.\n" .
         "Go to DataSet/Select to make a choice      ";
      PopNotice("Error: no curves",$msg);
      return;
   }

   WaitCursor();
   Status("Refreshing the screen...");

   # clear the screen
   #$w_canvas->createRectangle(0,0,$CFG{'Lmargin'}+$CFG{'width'}+$CFG{'Rmargin'},
   #  $CFG{'Tmargin'}+$CFG{'height'}+$CFG{'Bmargin'},
   #  -outline => $BCOLOR, -fill => $BCOLOR);
   $w_canvas->delete("all");

   DrawBase();

   PlotData();

   NormalCursor();
   Status("Refresh done.");
}

#-------------------------------------------------------------------------------------
sub DrawBase {
   # draw around the matte
   $w_canvas->createRectangle($CFG{'Lmargin'}, $CFG{'Tmargin'},
      $CFG{'width'}+$CFG{'Lmargin'}, $CFG{'height'}+$CFG{'Tmargin'},
      -outline => 'black');

   print LOG "Draw base: limits are X: ";
   if($XScaleExp > 0) {
      print LOG sprintf("%.$XScaleExp"."f",$CFG{'XMin'}) . ", "
      . sprintf("%.$XScaleExp"."f",$CFG{'XMax'}) .  ", Y: ";
   }
   else { print LOG "$CFG{'XMin'},$CFG{'XMax'}, Y: "; }
   if($YScaleExp > 0) {
      print LOG sprintf("%.$YScaleExp"."f",$CFG{'YMin'}) . ", "
      . sprintf("%.$YScaleExp"."f",$CFG{'YMax'}) .  "\n";
   }
   else { print LOG "$CFG{'YMin'},$CFG{'YMax'}\n"; }

   print LOG "XScale : exp $XScaleExp ";
   foreach $i (0..$CFG{'BticN'}-1) {
      $dummy = $XScaleMin + $i * $XScaleStep;
      if($XScaleExp > 0) { print LOG sprintf(" %.$XScaleExp"."f",$dummy); }
      else { print LOG " $dummy"; }
   }
   print LOG "\n";
   print LOG "YScale : exp $YScaleExp ";
   foreach $i (0..$CFG{'LticN'}-1) {
      $dummy = $YScaleMin + $i * $YScaleStep;
      if($YScaleExp > 0) { print LOG sprintf(" %.$YScaleExp"."f",$dummy); }
      else { print LOG " $dummy"; }
   }
   print LOG "\n";

   # title above plot
   $ans = $w_canvas->createText($CFG{'Lmargin'}+$CFG{'width'}/2,$CFG{'Tmargin'}/2,
      -justify => 'center', -fill => 'black', -text => $CFG{'Tlabel'},
      # -font => 'fontname',
      );
   # bottom label
   $ans = $w_canvas->createText($CFG{'Lmargin'}+$CFG{'width'}/2,
      $CFG{'Tmargin'}+$CFG{'height'}+$CFG{'Bmargin'}-8,
      -justify => 'center', -fill => 'black', -text => $CFG{'Blabel'},
      # -font => 'fontname',
      );
   # left label
   $ans = $w_canvas->createText(0.28*$CFG{'Lmargin'}+2*length($CFG{'Llabel'}),
      0.6*$CFG{'Tmargin'}, -justify => 'left', -fill => 'black',
      -text => $CFG{'Llabel'},
      # -font => 'fontname',
      );
   # right label
   $ans = $w_canvas->createText(
      $CFG{'Lmargin'}+$CFG{'width'}+0.70*$CFG{'Rmargin'}-2*length($CFG{'Rlabel'}),
      0.6*$CFG{'Tmargin'}, -justify => 'right', -fill => 'black',
      -text => $CFG{'Rlabel'},
      # -font => 'fontname',
      );
   
   DrawLabels();
}

#-------------------------------------------------------------------------------------
sub DrawLabels {
   # tics and numeric labels
   # (M,M)   (M+W,M)
   # (M,M+H) (M+W,M+H)
   my ($min,$max)=(0,0);
   foreach $i (-1..$CFG{'BticN'}) {                             # X axes
      $dummy = Xdata2scr($XScaleMin+$i*$XScaleStep);
      if($dummy >= $CFG{'Lmargin'} && $dummy <= $CFG{'width'}+$CFG{'Lmargin'}) {
         # bottom tic
         $ans = $w_canvas->createLine($dummy,$CFG{'Tmargin'}+$CFG{'height'},$dummy,
            $CFG{'Tmargin'}+$CFG{'height'}-$XTicLen, -fill => 'black');
         # bottom numeric label
         $msg = sprintf("%d",$XScaleMin+$i*$XScaleStep);
         $ans = $w_canvas->createText($dummy, $CFG{'Tmargin'}+$CFG{'height'}+10,
            -justify => 'center', -fill => 'black', -text => $msg);
         # save for next if block
         if($min == 0) { $min = $dummy; }
         $max = $dummy;

         # top tic
         $ans = $w_canvas->createLine($dummy, $CFG{'Tmargin'}, $dummy,
            $CFG{'Tmargin'}+$XTicLen, -fill => 'black');
      }
   }
   if($ScaleFixed == 1) {        # numeric labels at ends
      my $limit=$CFG{'width'}/(2*$CFG{'BticN'});
      $msg = sprintf("%d",$CFG{'XMin'});
      if($min-$CFG{'Lmargin'} > $limit) { # don't print if too close to another label
         $ans = $w_canvas->createText(
            $CFG{'Lmargin'}, $CFG{'Tmargin'}+$CFG{'height'}+10,
            -justify => 'center', -fill => 'black', -text => $msg);
      }
      $msg = sprintf("%d",$CFG{'XMax'});
      if($CFG{'Lmargin'}+$CFG{'width'}-$max > $limit) {
         $ans = $w_canvas->createText(
            $CFG{'Lmargin'}+$CFG{'width'},$CFG{'Tmargin'}+$CFG{'height'}+10,
            -justify => 'center', -fill => 'black', -text => $msg);
      }
   }
   #($min,$max)=(0,0);
   foreach $i (-1..$CFG{'LticN'}) {                             # Y axes
      $dummy = Ydata2scr($YScaleMin+$i*$YScaleStep);
      if($dummy >= $CFG{'Tmargin'} && $dummy <= $CFG{'Tmargin'}+$CFG{'height'}) {
         # left tic
         $ans = $w_canvas->createLine(
            $CFG{'Lmargin'},$dummy,$CFG{'Lmargin'}+$YTicLen,$dummy,
            -fill => 'black');  # tics
         if($YScaleExp > 0) {
            $msg = sprintf(" %.$YScaleExp"."f",$YScaleMin+$i*$YScaleStep);
         }
         else { $msg = sprintf("%.0f",$YScaleMin+$i*$YScaleStep); }
         # left numeric label
         $ans = $w_canvas->createText($CFG{'Lmargin'}-2*length($msg)-5, $dummy,
            -justify => 'left', -fill => 'black', -text => $msg);
         # save for next if block
         #if($min == 0) { $min = $dummy; }
         #$max = $dummy;

         # right tic
         $ans = $w_canvas->createLine(
            $CFG{'Lmargin'}+$CFG{'width'}, $dummy,
            $CFG{'Lmargin'}+$CFG{'width'}-$YTicLen, $dummy, -fill => 'black');
      }
   }
   #if($ScaleFixed) {       # numeric labels at ends
   #   my $limit=20;           # need character size in pixels here...
   #   $msg = sprintf("%d",$CFG{'YMin'});
   #   if($min-$CFG{'Tmargin'} > $limit) {# don't print if too close to another label
   #      $ans = $w_canvas->createText($CFG{'Lmargin'}-2*length($msg)-5,
   #         $CFG{'Tmargin'}+$CFG{'height'},
   #         -justify => 'left', -fill => 'black', -text => $msg);
   #   }
   #   if($CFG{'Tmargin'}+$CFG{'height'}-$max > $limit) {
   #      $msg = sprintf("%d",$CFG{'YMax'});
   #      $ans = $w_canvas->createText($CFG{'Lmargin'}-2*length($msg)-5,
   #      $CFG{'Tmargin'}, -justify => 'left', -fill => 'black', -text => $msg);
   #   }
   #}
}

#-------------------------------------------------------------------------------------
sub PlotData { # plot the data
   my (@penup,@y,@prevx,@prevy);
   my ($x,$id);
   my ($xfr,$xto,$yfr,$yto,$tfl,$ffl);
   ($begW,$begS) = split(',',$CFG{'begin'});
   ($endW,$endS) = split(',',$CFG{'end'});
   my ($sat,$c,$week,$sow);
   my $sow0 = 0;
   $CFG{'Week'} = 0;

   # lift all pens
   foreach $j (0..$ncurv-1) { $penup[$j] = 1; }

   # open the data file and read
   $filename = "$CFG{'datafile'}";
   print LOG "Open $filename to plot ...";
   open FILE, "$filename" or die "Error: Refresh could not open $filename\n";
   Status("Refresh is plotting data in file $filename ...");

   # loop over data in this file
   while(<FILE>) {
      chomp;
      $_ =~ s/^\s_//;
      if(m/Rinexdump/) { next; }
      if(m/Week/) { next; }
      # data
      #print LOG "Read $_\n";
      s/^\s+//;               # no leading white space
      @opt = split(/\s+/, $_);
      $week = $opt[0];                    # GPS week
      $sow = $opt[1];                     # GPS seconds of week
      # ignore if outside time limits
      next if($week < $begW);
      next if($week == $begW && $sow < $begS);
      last if($week > $endW);
      last if($week == $endW && $sow > $endS);
      if($CFG{'Week'} == 0) { $CFG{'Week'} = $week; $sow0 = $sow; }
      $x = Xdata2scr($sow + ($week-$CFG{'Week'})*604800.);
      $sat = $opt[2];
      #print LOG "Sat is $sat   ncurves $ncurv\n";
      foreach $j (0..$ncurv-1) {
         if($curvON[$j] == 0) { next; }
         if($curvSV[$j] eq $sat) {
            $y[$j] = Ydata2scr($opt[$curvCol[$j]]);
				#print LOG "Data $sat: x=$opt[1] y=$opt[$curvCol[$j]] "
				#. "sx=$x sy=$y[$j] pen=$penup[$j]\n";
            if($penup[$j] == 1) {
               $penup[$j] = 0;
            }
            elsif($penup[$j] == 0) {
					#print LOG "Plot [$j]: ($prevx[$j],$prevy[$j]) to ($x,$y[$j])\n";
               $xfr = $prevx[$j]; $xto = $x; $yfr = $prevy[$j]; $yto = $y[$j];
               # clip
					#print LOG "Clip [$j]: ($xfr,$yfr,$xto,$yto,$tfl,$ffl)\n";
               if(Clip(\$xfr,\$yfr,\$xto,\$yto,\$tfl,\$ffl) != 0) {
                  $c = $curvColor[$j];
						#print LOG "Draw [$j]: ($xfr,$yfr) to ($xto,$yto)\n";
                  if($curvLines[$j] != 0) {
                     # draw line
                     $id = $w_canvas->createLine($xfr,$yfr,$xto,$yto,-fill => $c);
                  }
                  if($tfl != 0 && $curvSymbs[$j] ne 'none') {
                     # pt visible, symb selected -> draw symbol
                     # (no clipping here ... maybe should)
                     if($curvSymbs[$j] eq 'cross') {
                        $id = $w_canvas->createLine(  # why 2/3 you ask? I don't know
                           $xto-2,$yto-2,$xto+3,$yto+3,-fill => $c);
                        $id = $w_canvas->createLine(
                           $xto+2,$yto-2,$xto-3,$yto+3,-fill => $c);
                     }
                     elsif($curvSymbs[$j] eq 'plus') {
                        $id = $w_canvas->createLine(
                           $xto-2,$yto,$xto+3,$yto,-fill => $c);
                        $id = $w_canvas->createLine(
                           $xto,$yto-2,$xto,$yto+3,-fill => $c);
                     }
                     elsif($curvSymbs[$j] eq 'diamond') {
                        $id = $w_canvas->createLine(
                           $xto-3,$yto,$xto,$yto+3,-fill => $c);
                        $id = $w_canvas->createLine(
                           $xto,$yto+3,$xto+3,$yto,-fill => $c);
                        $id = $w_canvas->createLine(
                           $xto+3,$yto,$xto,$yto-3,-fill => $c);
                        $id = $w_canvas->createLine(
                           $xto,$yto-3,$xto-3,$yto,-fill => $c);
                     }
                     elsif($curvSymbs[$j] eq 'square') {
                        $id = $w_canvas->createLine(
                           $xto-2,$yto-2,$xto-2,$yto+2,-fill => $c);
                        $id = $w_canvas->createLine(
                           $xto-2,$yto+2,$xto+2,$yto+2,-fill => $c);
                        $id = $w_canvas->createLine(
                           $xto+2,$yto+2,$xto+2,$yto-2,-fill => $c);
                        $id = $w_canvas->createLine(
                           $xto+2,$yto-2,$xto-2,$yto-2,-fill => $c);
                     }
                     elsif($curvSymbs[$j] eq 'circle') {
                        $id = $w_canvas->createOval($xto-2,$yto-2,
                           $xto+2,$yto+2,-outline => $c);
                     }
                     elsif($curvSymbs[$j] eq 'del') {
                        $id = $w_canvas->createLine(
                           $xto,$yto+3,$xto+3,$yto-2,-fill => $c);
                        $id = $w_canvas->createLine(
                           $xto+3,$yto-2,$xto-2,$yto-2,-fill => $c);
                        $id = $w_canvas->createLine(
                           $xto-2,$yto-2,$xto,$yto+3,-fill => $c);
                     }
                     elsif($curvSymbs[$j] eq 'delta') {
                        $id = $w_canvas->createLine(
                           $xto-2,$yto+2,$xto+3,$yto+2,-fill => $c);
                        $id = $w_canvas->createLine(
                           $xto+3,$yto+2,$xto,$yto-3,-fill => $c);
                        $id = $w_canvas->createLine(
                           $xto,$yto-3,$xto-2,$yto+2,-fill => $c);
                     }
                  }
               }
            }
            $prevy[$j] = $y[$j];
            $prevx[$j] = $x;
            next;
         }
      }
   }
   close FILE;
   print LOG " done.\n";
}

#-------------------------------------------------------------------------------------
sub Clip {
   # (M,M)   (M+W,M)
   # (M,M+H) (M+W,M+H)
   #my $fflag=0;   # flag = 1,0 as inside, outside region
   #my $tflag=0;   # 'f' is from, 't' is to
   my $xf = shift;
   my $yf = shift;
   my $xt = shift;
   my $yt = shift;
   my $tflag = shift;
   my $fflag = shift;
   $$tflag = 0;
   $$fflag = 0;
   if($$xt >= $CFG{'Lmargin'} && $$xt <= $CFG{'Lmargin'}+$CFG{'width'}
      && $$yt >= $CFG{'Tmargin'} && $$yt <= $CFG{'Tmargin'}+$CFG{'height'}) {
         $$tflag = 1;
   }
   if($$xf >= $CFG{'Lmargin'} && $$xf <= $CFG{'Lmargin'}+$CFG{'width'}
      && $$yf >= $CFG{'Tmargin'} && $$yf <= $CFG{'Tmargin'}+$CFG{'height'}) {
         $$fflag = 1;
   }
   if($$tflag == 1 && $$fflag == 1) { return 1; }

   # Two line segments intersect at (x,y) where
   #                   * (x3,y3)
   #                   |
   #                   |
   # (x2,y2) * --------+-----------* (x1,y1)
   #                   |
   #                   |
   #                   |
   #                   |
   #                   * (x4,y4)
   #
   # and x = [(x1*y2-x2*y1)*(x3-x4)-(x3*y4-x4*y3)*(x1-x2)]
   #       / [(x1-x2)*(y3-y4)-(x3-x4)*(y1-y2)] ,
   #     y = [(x1*y2-x2*y1)*(y3-y4)-(x3*y4-x4*y3)*(y1-y2)]
   #       / [(x1-x2)*(y3-y4)-(x3-x4)*(y1-y2)]
   # Note correct reduction when horiz or vert: x=x3 when x3=x4, y=y1 when y1=y2, etc.

   my ($p,$x1,$y1,$x2,$y2);   # intersections
   my $npt=0;
   $ans = $$yt-$$yf; # y3-y4
   if($ans != 0) {
      $dummy = ($$xt*$$yf-$$xf*$$yt)/$ans;
      $msg = ($$xt-$$xf)/$ans;
      # intersect top    x2=$CFG{'Lmargin'}, x1=$CFG{'Lmargin'}+$CFG{'width'},
      #                  y1=y2=$CFG{'Tmargin'}
      $ans = $CFG{'Tmargin'};
      $p = int($ans*$msg-$dummy+0.5);
      if($p > $CFG{'Lmargin'} && $p < $CFG{'Lmargin'}+$CFG{'width'}
            && ($$yt-$ans)*($$yf-$ans) < 0) {
         $x1 = $p; $y1 = $CFG{'Tmargin'}; $npt = 1;
      }
      # intersect bottom x2=$CFG{'Lmargin'}, x1=$CFG{'Lmargin'}+$CFG{'width'},
      #                  y1=y2=$CFG{'Tmargin'}+$CFG{'height'}
      $ans = $CFG{'Tmargin'}+$CFG{'height'};
      $p = int($ans*$msg-$dummy+0.5);
      if($p > $CFG{'Lmargin'} && $p < $CFG{'Lmargin'}+$CFG{'width'}
            && ($$yt-$ans)*($$yf-$ans) < 0) {
         if($npt == 0) { $x1 = $p; $y1 = $CFG{'Tmargin'}+$CFG{'height'}; }
         else { $x2 = $p; $y2 = $CFG{'Tmargin'}+$CFG{'height'}; }
         $npt++;
      }
   }
   $ans = $$xt-$$xf;  # x3-x4
   if($ans != 0) {
      $dummy = ($$yt*$$xf-$$yf*$$xt)/$ans;
      $msg = ($$yt-$$yf)/$ans;
      # intersect left   x3=x4=$CFG{'Lmargin'},s
      #                  y3=$CFG{'Tmargin'}, $y4=$CFG{'Tmargin'}+$CFG{'height'}
      $ans = $CFG{'Lmargin'};
      $p = int($ans*$msg-$dummy+0.5);
      if($p > $CFG{'Tmargin'} && $p < $CFG{'Tmargin'}+$CFG{'height'}
            && ($$xt-$ans)*($$xf-$ans) < 0) {
         if($npt == 0) { $y1 = $p; $x1 = $CFG{'Lmargin'}; }
         else { $y2 = $p; $x2 = $CFG{'Lmargin'}; }
         $npt++;
      }

      # intersect right  x3=x4=$CFG{'Lmargin'}+$CFG{'width'},
      #                  y3=$CFG{'Tmargin'}, $y4=$CFG{'Tmargin'}+$CFG{'height'}
      $ans = $CFG{'Lmargin'}+$CFG{'width'};
      $p = int($ans*$msg-$dummy+0.5);
      if($p > $CFG{'Tmargin'} && $p < $CFG{'Tmargin'}+$CFG{'height'}
            && ($$xt-$ans)*($$xf-$ans) < 0) {
         if($npt == 0) { $y1 = $p; $x1 = $CFG{'Lmargin'}+$CFG{'width'}; }
         else { $y2 = $p; $x2 = $CFG{'Lmargin'}+$CFG{'width'}; }
         $npt++;
      }
   }

   if($npt == 0) { return 0; }          # no intersections

   if($$tflag == 1 && $$fflag == 0) {     # outside to inside
      $$xf = $x1; $$yf = $y1;
   }
   elsif($$tflag == 0 && $$fflag == 1) {  # inside to outside
      $$xt = $x1; $$yt = $y1;
   }
   else {                               # outside to outside
      if($npt!=2) {
         print LOG "WARNING: Clip finds outside to outside with"
                . " $npt intersections\n";
         return 0;
      }
      if((($$xt-$x1)*($$xt-$x1)+($$yt-$y1)*($$yt-$y1)) <
         (($$xf-$x1)*($$xf-$x1)+($$yf-$y1)*($$yf-$y1))) {
         $$xt = $x1; $$yt = $y1;
         $$xf = $x2; $$yf = $y2;
      }
      else {
         $$xt = $x2; $$yt = $y2;
         $$xf = $x1; $$yf = $y1;
      }
   }
   return 1;
}

#-------------------------------------------------------------------------------------
sub CanvConf {
   my $w = $w_canvas->width;
   my $h = $w_canvas->height;
   my $W = $CFG{'Lmargin'}+$CFG{'width'}+$CFG{'Rmargin'};
   my $H = $CFG{'Tmargin'}+$CFG{'height'}+$CFG{'Bmargin'};
   if($w != $CFG{'Lmargin'}+$CFG{'width'}+$CFG{'Rmargin'} ||
      $h != $CFG{'Tmargin'}+$CFG{'height'}+$CFG{'Bmargin'}) {
      $CFG{'width'} = $w - $CFG{'Lmargin'} - $CFG{'Rmargin'};
      $CFG{'height'} = $h - $CFG{'Tmargin'} - $CFG{'Bmargin'};
      Rates();
      if($firstcanvas == 1) {
         Status("Plot reconfigured to $CFG{'width'} x $CFG{'height'} ...Refresh?");
      }
      $firstcanvas = 1;
   }
}

#-------------------------------------------------------------------------------------
sub PopNotice {
   my ($dummy1, $dummy2) = @_;
   #$ans = $w_top->messageBox(
   #-title => $dummy1,
   #-message => $dummy2 . "        ",
   #-type => 'OK',
   #-icon => 'info',
   ##no popover for messageBox
   #);
   my $PNdb = $w_top->DialogBox(
      -title => $dummy1,
      -buttons => ['Ok'],
      -popover => $w_top,
      -overanchor => 'c',
      -popanchor => 'c',
   );
   $PNdb->add('Label', -text => $dummy2, -justify => 'left')->pack;
   $PNdb->Show();
}

#-------------------------------------------------------------------------------------
sub Status {
   ($dummy) = (@_);
   $statusbar->delete('0.0','end');
   $statusbar->insert('0.0',$dummy);
   $w_top->update;
}

#-------------------------------------------------------------------------------------
sub WaitCursor {
   $w_canvas->configure(-cursor => $waitcursor);
   $w_top->update;
}

#-------------------------------------------------------------------------------------
sub NormalCursor {
   $w_canvas->configure(-cursor => $cursor);
   $w_top->update;
}

#-------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------
sub mouse_events {
	# do not enter this routine unless screen is up and scale has been defined
	if($MainUp == 0 || $ScaleDefined == 0) { return; }

   my ($canv, $x, $y, $mousemsg) = @_;

   if($mousemsg eq "motion") {
      if($MAKE_RECT) {
         # first erase the old one
         $canv->delete($MOUSE_RECT);

         # where is the corner now?
         $RECT_X1 = $canv->canvasx($x);
         $RECT_Y1 = $canv->canvasy($y);

         # draw a new foreground-color rectangle
         $MOUSE_RECT = $canv->createRectangle(
            $RECT_X0,$RECT_Y0, $RECT_X1,$RECT_Y1,
            -outline => $MCOLOR );
      }
   }
   else {
      #print LOG "mouse$msg: (x,y) = ", $canv->canvasx($x), ", ", $canv->canvasy($y),
      #   " (datax,datay) = ", scr2Xdata($canv->canvasx($x)), " , ",
      #   scr2Ydata($canv->canvasy($y)),
      #   "\n";
      if($mousemsg eq "push left") {
         $RECT_X0 = $canv->canvasx($x);
         $RECT_Y0 = $canv->canvasy($y);
         #print LOG "push: RECT_X0 $RECT_X0, RECT_Y0 $RECT_Y0\n";
         $MAKE_RECT = 1;
      }
      if($mousemsg eq "rel left") {
         $RECT_X1 = $canv->canvasx($x);
         $RECT_Y1 = $canv->canvasy($y);
         #print LOG "rele: RECT_X1 $RECT_X1, RECT_Y1 $RECT_Y1\n";
         #print LOG "just checking: (x,y) = ", $RECT_X1, ", ", $RECT_Y1,
         #   " (datax,datay) = ", scr2Xdata($RECT_X1), " , ", scr2Ydata($RECT_Y1),
         #   "\n";
         Status("(x,y) = screen("
            . $RECT_X1 . ","
            . $RECT_Y1 . ") = data("
            . sprintf("%10.3f",scr2Xdata($RECT_X1)) . ","
            . sprintf("%.3f",scr2Ydata($RECT_Y1)) . ")");
         $canv->delete($MOUSE_RECT);
         $MAKE_RECT = 0;

         # ignore zero-area rectangles
         if(($RECT_X0 == $RECT_X1) || ($RECT_Y0 == $RECT_Y1)) {
            return;
         }

         $MOUSE_RECT = $canv->createRectangle(
            $RECT_X0,$RECT_Y0, $RECT_X1,$RECT_Y1,
            -outline => $MCOLOR );

         # put up a dialog asking what to do with the rectangle
         my $db = $w_top->DialogBox(
            -title => 'Mouse Rectangle',
            -buttons => ['Zoom', 'Cancel'],
            -default_button => 'Zoom',
            -popover => 'cursor', -overanchor => 'c', -popanchor => 'nw',
         );
         $db->add('Label', -text => "Zoom: are you sure?    ")->pack;
         $ans = $db->Show();
         if($ans eq "Zoom") {
            #print LOG "You said to zoom to mouse rectangle\n";
            #$canv->move($MOUSE_RECT, 100, 100);
            ($YMin, $YMax, $XMin, $XMax) =
               (scr2Ydata($RECT_Y1), scr2Ydata($RECT_Y0),
                scr2Xdata($RECT_X0), scr2Xdata($RECT_X1) );
            if($YMin > $YMax) {
               $ans = $YMax;
               $YMax = $YMin;
               $YMin = $ans;
            }
            if($XMin > $XMax) {
               $ans = $XMax;
               $XMax = $XMin;
               $XMin = $ans;
            }
            # NewScale requires $XMin,$XMax,$YMin,$YMax as input
            NewScale('fixed','Mouse zoom');
            Refresh();
         }
         if($ans eq "Cancel") {
            #print LOG "You said to cancel mouse rectangle\n";
            $canv->delete($MOUSE_RECT);
            Status('');
         }
      }
   }
}

#-------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------
# string CmdText defines configuration constants and options on command line,
# with their default values. See documentation of CmdText and sub CommandLine.
sub Defaults {
   # parse CmdText
   my @lines = split('\n',$CmdText);
   foreach (@lines) {
      last if(m/Example:/);
      next if(m/^#/);
      if(m/(can repeat)/) { $dummy='grow'; } else { $dummy=''; }
      s/--(\S+)\s.*\((.*)\)$/$1 = $2/;
      s/^\s+//;
      s/\s+$//;
      ($key, $val) = split(/ = /,$_,2);
      if($val eq '') { $key =~ s/ =$//; }
      #print "Line:",$_,"\n";
      #print "key |$key| value |$val|\n";
      #AddDefault($key,$val,$dummy);
      $Options{$key} = 1;         # this says 'key' is a valid option
      if($dummy eq 'grow') {
         $Grow{$key} = 1;         # this says 'key' belongs to OPT, not CFG
         if($val ne '') { push(@{$OPT{$key}}, $val); }   # OPT -- 1 to many
      }
      else {
         if($val ne '') { $CFG{$key} = $val; }            # CFG -- 1 to 1
      }
   }
   # TD this should belong to CFG also
   $ConfigFile = 'rp.cfg';
}

#-------------------------------------------------------------------------------------
# command line arguments -- see CmdText above for doc.
# process the command line, converting command line into key/val pairs,
# where --key=val --key val --key (val='on')
# don't allow -key because this makes val = (negative number) impossible
sub CommandLine {
   if($#ARGV == 0 && $ARGV[0] eq '--help') {
      print $CmdText,"\n";
      exit;
   }
   $LimitsSet = 0;                  # notice if limits are set
   $key = '';
   $val = '';
   while($#ARGV >= 0) {
      $dummy = shift @ARGV;
      if(substr($dummy,0,2) eq "--") {                # '--' => its key or key=value
         $dummy = substr($dummy,2,length($dummy)-2);     # remove --
         if($key ne '' && $val eq 'on') {                # was prev arg a key alone?
            AddToConfig($key,$val);
            $key = '';
            $val = '';
         }
         if($dummy =~ '=') {                             # is there an '='?
            ($key, $val) = split(/=/,$dummy,2);
            AddToConfig($key,$val);
            $key = '';
            $val = '';
         }
         else {                                          # no =; val is 'on' for now
            $key = $dummy;
            $val = 'on';
         }
      }
      else {                                          # no '--' => its val or an error
         if($key ne '' && $val eq 'on') {                # prev arg was --key
            $val = $dummy;
            AddToConfig($key,$val);
            $key = '';
            $val = '';
         }
         else {                                          # isolated and no '--' => err
            print "Unrecognized argument: $dummy\n";
         }
      }
   }
   # the last arg
   if($key ne '' && $val eq 'on') { AddToConfig($key,$val); }
}

#-------------------------------------------------------------------------------------
sub AddToConfig {
   my ($k,$v) = @_;
   if($Options{$k} == 1) {
      if($Grow{$k} == 1) {
         push(@{$OPT{$k}}, split(/\s+/,$v));
      }
      else {
         if($k eq 'prgmdir') {
            $v =~ s/\//$SLASH/g;
            $v =~ s/\\/$SLASH/g;
         }
         # save it
         $CFG{$k} = $v;
         # set flags here -- any option set by the user will pass here
         if($k eq 'XMin' or $k eq 'XMax' or $k eq 'YMin' or $k eq 'YMax') {
            $LimitsSet = 1;
         }
         if(($k eq 'Blabel' && $v ne 'GPS Seconds of Week') or
            ($k eq 'Tlabel' && $v ne 'Title') or
            $k eq 'Rlabel' or
            $k eq 'Llabel') {
            $UsingDefaults = 0;
         }
      }

      # open file and read as config; do here so later cmds may override
      if($k eq 'load') {
         my @c = @{$OPT{'load'}};
         while($#c >= 0) {
            $i = shift @c;
            last unless($i ne '');
            delete(@{$OPT{'load'}}[0]);
            LoadConfig($i);
         }
      }

      return 'ok';
   }
   else {
      print "Invalid configuration option: $k\n";
      return 'fail';
   }
}

#-------------------------------------------------------------------------------------
sub LoadConfig {
   ($filename) = @_;
   if($CFG{'verbose'}) {
      if($LogOpen) {
         print LOG "Attempt to open and read configuration file $filename\n";
      }
      else {
         print "Attempt to open and read configuration file $filename\n";
      }
   }

   $dummy = 0;                   # dummy will count the number of read errors
   $LimitsSet = 0;
   if(open(CONFIG, "$filename")) {
      while (<CONFIG>) {
         if($dummy > 20) {
            close CONFIG;
            if($LogOpen) {
               print LOG "\nAbort - Too many read errors loading configuration\n";
               print LOG "  File \"$filename\" may not be a configuration file.\n";
            }
            else {
               print "\nAbort - Too many read errors loading configuration\n";
               print "  File \"$filename\" may not be a configuration file.\n";
            }
            if($MainUp) {
               PopNotice("Error loading file",
               "Abort - Too many read errors while loading a\n" .
               "configuration file. This may not be a configuration file:\n" .
               "   $filename");
            }
            return 'fail';
         }
         chomp;                  # no newline
         s/^\s+//;               # no leading whitespace
         s/#.*//;                # no comments
         s/\s+$//;               # no trailing whitespace
         s/^--//;                # no leading --
         next unless length;     # anything left?
         unless(m/=/) {          # ignore if no =
            print "Warning: configuration option has no '=' : $_\n";
            $dummy++;
            next;
         }
         ($key, $val) = split(/\s*=\s*/, $_, 2);
         # nested conf files do not work
         if($key eq 'load') {
            print "Warning: nested --load <file> do not work\n";
            next;
         }
         if($val =~ m/^"/ && $val =~ m/"$/) {
            # remove quotes around values with whitespace
            $val =~ s/^"//;
            $val =~ s/"$//;
            $ans = AddToConfig($key,$val);
         }
         else {
            $ans = AddToConfig($key,$val);
         }
         if($ans eq 'fail') { $dummy++; }
      }
      close CONFIG;

      $ConfigFile = $filename;
      return 'ok';
   }
   else {
      # don't put a popup here b/c top level window may not exist yet.
      if($LogOpen) {
         print LOG "Error: Could not open configuration file $filename\n";
      }
      else {
         print "Error: Could not open configuration file $filename\n";
      }
      #$ConfigFile = '';
      return 'fail';
   }
}

#-------------------------------------------------------------------------------------
sub SaveConfig {
   my ($file)=@_;
   if($file eq '') { return 'ok'; }
   print LOG "Save the configuration to the file $file\n";

   my $cmd = ConfigString('config');

   if(open(CONFIG, ">$file")) {
      print CONFIG $cmd;
      close CONFIG;
      return 'ok';
   }
   else {
      return 'fail';
   }
}

#-------------------------------------------------------------------------------------
# process the config, used for command line processing and after config file loaded.
sub ProcessConfig {
   if($CFG{'log'} ne '' && $CFG{'log'} ne 'SCREEN') {
      open LOG, ">$CFG{'log'}" or die "Could not open log file $CFG{'log'}\n";
      autoflush LOG;
      $LogOpen = 1;
      if($CFG{'verbose'}) {
         print "RP Output directed to log file $CFG{'log'}\n";
         print LOG "Log file for RinexPlot\n";
      }
   }
   else { open(LOG,">-") or die "Could not re-open STDOUT\n"; }

   if($CFG{'Rinex'} ne '') {
      DataInputProcess($CFG{'Rinex'});

      my %index;
      my (@AOs,@Sats,@Obs)=((),(),());
      if(defined(@{$OPT{'AO'}})) { @AOs = @{$OPT{'AO'}}; }
      if(defined(@{$OPT{'sat'}})) { @Sats = @{$OPT{'sat'}}; }
      if(defined(@{$OPT{'obs'}})) { @Obs = @{$OPT{'obs'}}; }

      if($#AOs >= 0) {
         %index=();
         $j=0;
         foreach $i (@ExtOT) {
            unless($index{$i}) {
               $index{$i}=$j;
               $j++;
            }
         }
         $dummy = 0;
         foreach $i (@AOs) {
            $j = $index{$i};
            if($dummy == 0) { print LOG "Selected new obs types"; $dummy=1; }
            print LOG " $i";
            $ExtSelect[$j] = 1;
         }
         if($dummy==1) { print LOG "\n"; }
         if($CFG{'create'} eq 'on') { CompCrea(); }
      }
      if($#Sats >= 0) {
         %index=();
         $j=0;
         foreach $i (@svlist) {
            unless($index{$i}) {
               $index{$i}=$j;
               $j++;
            }
         }
         $dummy = 0;
         foreach $i (@Sats) {
            $j = $index{$i};
            if($j eq '') {
               if($dummy==1) { print LOG "\n"; }
               print LOG "Error: satellite $i is not found in file!\n";
               print LOG "  Satellites in file are: @svlist\n";
            }
            else {
               if($dummy == 0) { print LOG "Selected Sats:"; $dummy=1; }
               print LOG " $i";
               $svselect[$j] = 1;
            }
         }
         if($dummy==1) { print LOG "\n"; }
      }
      if($#Obs >= 0) {
         %index=();
         $j=0;
         foreach $i (@obslist) {
            unless($index{$i}) {
               $index{$i}=$j;
               $j++;
            }
         }
         $dummy = 0;
         foreach $i (@Obs) {
            $j = $index{$i};
            if($j eq '') {
               if($dummy==1) { print LOG "\n"; }
               print LOG "Error: obs type $i is not found in file!\n";
               print LOG "  Obs types in file are: @obslist\n";
            }
            else {
               if($dummy == 0) { print LOG "Selected Obs:"; $dummy=1; }
               print LOG " $i";
               $obsselect[$j] = 1;
            }
         }
         if($dummy==1) { print LOG "\n"; }
      }
      if($CFG{'refresh'} eq 'on' && $#Obs >= 0 && $#Sats >= 0) {
         # NewScale requires $XMin,$XMax,$YMin,$YMax as input
         ($XMin,$XMax,$YMin,$YMax)
            = ($CFG{'XMin'},$CFG{'XMax'},$CFG{'YMin'},$CFG{'YMax'});
         NewScale('fixed','Command line');
         if($LimitsSet == 1) {
            $FirstAutoscale = 0;
            $LimitsSet = 0;
         }
         else { $FirstAutoscale = 1; }
         $Reconfigure = 1;
         ConfigureCurves();
         Refresh();
      }
   }
   if($CFG{'help'} eq 'on') { HelpTopi(); }

   $LimitsSet = 0;
   # delete these, as they are used only for cmdline and config file input
   delete($OPT{'load'});
   delete($OPT{'AO'});
   delete($CFG{'create'});
   delete($OPT{'sat'});
   delete($OPT{'obs'});
   delete($CFG{'refresh'});
   delete($CFG{'help'});
}

#-------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------
# Create the menubar and everything under it.
sub create_menu {
  [
    [ 'cascade', '~File', -tearoff => $menutear, -menuitems =>
      [
        ['command', '~Load File ...', -accelerator => 'Ctrl-L',
           -command => \&FileLoad ],
        ['command', '~Save File', -accelerator => 'Ctrl-S', -command => \&FileSave ],
        ['command', 'Save ~As ...', -command => \&FileSaAs ],
        ['command', '~Output as command ...', -command => \&OutputCommand ],
        '',
        ['command', '~Rinex Obs File ...', -command => \&FileInpu ],
        '',
        ['command', 'View ~Data Summary ...', -command => \&FileSumm ],
        ['command', '~View Selections ...', -command => \&FileSele ],
        ['command', '~Clear all selections', -command => \&FileClea ],
        '',
        ['command', '~Exit',  -accelerator => 'Ctrl-Q', -command => \&FileExit ],
      ],
    ],

    [ 'cascade', '~Compute', -tearoff => $menutear, -menuitems =>
      [
        ['command', 'New ~Data types...', -command => \&CompObst ],
        ['command', '~Nav File...', -command => \&CompNavf ],
        ['cascade', '~Rx Position', -tearoff => $menutear, -menuitems =>
          [
            ['command', '~Select Rx...', -command => \&CompPosnSele ],
            ['command', '~RAIM Solution', -command => \&CompPosnRAIM ],
          ],
        ],
        ['command', 'Con~figure...', -command => \&CompConf ],
        '',
        ['command', '~Create new data', -command => \&CompCrea ],
      ],
    ],

    [ 'cascade', '~DataSet', -tearoff => $menutear, -menuitems =>
      [
        ['command', '~Satellites', -command => \&DataSats ],
        ['command', '~Obs types', -command => \&DataObst ],
        ['command', '~Times', -command => \&DataTime ],
        ['command', '~Configure', -command => \&DataConf ],
      ],
    ],

    [ 'cascade', '~Graph', -tearoff => $menutear, -menuitems =>
      [
        ['command', '~AutoScale', -accelerator => '   Ctrl-A',
           -command => \&AutoScale ],
        ['command', '~Limits', -command => \&GrapLims ],
        ['command', '~Axes', -command => \&GrapAxes ],
        ['command', 'La~bels', -command => \&GrapLabe ],
        ['command', '~Curves', -command => \&GrapCurv ],
        ['command', 'Zoom ~In', '-accelerator', '    Ctrl-I',
            -command => [ \&GrapZoom, 'dummy', 'In' ] ],
        ['command', 'Zoom ~Out', '-accelerator', '    Ctrl-O',
            -command => [ \&GrapZoom, 'dummy', 'Out' ] ],
        ['command', '~UnZoom', '-accelerator', '    Ctrl-U',
            -command => \&GrapUnzo ],
        '',
        ['command', '~Gnuplot', -command => \&GrapGnup ],
      ],
    ],

    # this makes Refresh just a button - no menu under it
    [ 'command', '~Refresh', -command => \&Refresh ],

    [ 'cascade', '~Settings', -tearoff => $menutear, -menuitems =>
      [
        ['command', '~Preferences', -command => \&SettPref ],
        ['command', '~Set Cursor', -command => \&SettCurs ],
      ],
    ],

    [ 'cascade', '~Help', -tearoff => $menutear, -menuitems =>
      [
        ['command', '~Topics', -command => \&HelpTopi ],
        ['command', '~About', -command => \&HelpAbou ],
      ],
    ],

  ]; 
} # end sub create_menu

#-------------------------------------------------------------------------------------
# initialize: create menu system and define bindings
sub init {
   create_screen();

   # key bindings
   $w_top->bind("<Control-Key-l>", \&FileLoad);
   $w_top->bind("<Control-Key-s>", \&FileSave);
   $w_top->bind("<Control-Key-a>", \&AutoScale);
   $w_top->bind("<Control-Key-q>", \&FileExit);
   $w_top->bind("<Control-Key-i>", [ \&GrapZoom, "In"] );
   $w_top->bind("<Control-Key-o>", [ \&GrapZoom, "Out"] );
   $w_top->bind("<Control-Key-u>", \&GrapUnzo);
   $w_top->bind("<Control-Key-r>", \&Refresh);

   # mouse bindings
   $w_canvas->CanvasBind("<ButtonPress-1>",
      [ \&mouse_events, Ev('x'), Ev('y'), "push left" ]);
   $w_canvas->CanvasBind("<ButtonPress-3>",
      [ \&mouse_events, Ev('x'), Ev('y'), "push right" ]);
   $w_canvas->CanvasBind("<ButtonRelease-1>",
      [ \&mouse_events, Ev('x'), Ev('y'), "rel left" ]);
   $w_canvas->CanvasBind("<ButtonRelease-3>",
      [ \&mouse_events, Ev('x'), Ev('y'), "rel right" ]);
   $w_canvas->CanvasBind("<Motion>",
      [ \&mouse_events, Ev('x'), Ev('y'), "motion" ]);

   # window resizing
   $w_canvas->CanvasBind("<Configure>", \&CanvConf);

   # fonts
   $w_top->fontCreate(qw/C_small -family courier   -size 10/);
   $w_top->fontCreate(qw/C_big   -family courier   -size 14 -weight bold/);
   $w_top->fontCreate(qw/C_vbig  -family helvetica -size 24 -weight bold/);
   $w_top->fontCreate(qw/C_bold -family courier -size 12 -weight bold -slant italic/);

   $MainUp = 1;
}

#-------------------------------------------------------------------------------------
sub create_screen {
   $w_top = MainWindow->new(-title =>'Rinex Plot');

   # menus
   $w_top->configure(-menu => $menubar = $w_top->Menu(-menuitems => create_menu));

   # status bar - putting statusbar before canvas means when resizing window,
   #              canvas shrinks but status bar stays
   $statusbar = $w_top->ROText(
      -height => 1,
      -relief => 'flat',
      -background => '#a0b7ce',
      -foreground => 'white',
      -borderwidth => 2
       )->pack(
      -expand => '0',
      -fill => 'both',
      -side => 'bottom',
      -anchor => 'w');
   $statusbar->insert('0.0', 'Welcome to Rinex Plot');

   # canvas
   $w_canvas = $w_top->Canvas(
      '-width'  => $CFG{'width'}+$CFG{'Lmargin'}+$CFG{'Rmargin'},
      '-height' => $CFG{'height'}+$CFG{'Tmargin'}+$CFG{'Bmargin'},
      '-border' => 1,
      # this has no effect ... '-relief' => 'solid',
      '-background' => $BCOLOR,
      '-cursor' => $cursor);
   $w_canvas->pack(-expand => '1', -anchor => 'nw', -fill => 'both');

   # need update here to prevent a <Configure> event that calls CanvConf at odd time
   $w_top->update;
}

#-------------------------------------------------------------------------------------
# execution begins here
#-------------------------------------------------------------------------------------
# define default CFG values
Defaults();

# process command line arguments
CommandLine();

# create menu and screen and define bindings
init();

# process any command line input
ProcessConfig();

# make sure utilities are available
my($ResCor,$RinSum,$RinexDump)=('ResCor','RinSum','RinexDump');
if ($^O eq "MSWin32") {
   $ResCor='ResCor.exe';
   $RinSum='RinSum.exe';
   $RinexDump='RinexDump.exe';
}
if(not -e $CFG{'prgmdir'} . $SLASH . $RinSum
   || not -e $CFG{'prgmdir'} . $SLASH . $ResCor
   || not -e $CFG{'prgmdir'} . $SLASH . $RinexDump) {
   PopNotice("Error: GPSTk not found",
      "Error: GPSTk utilities are not found in directory $CFG{'prgmdir'}.\n"
      . "Go to Settings/Preferences to change this");
}

MainLoop();
#-------------------------------------------------------------------------------------
