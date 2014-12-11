#!/usr/bin/perl
# $Id: PRSplot.pl 89 2012-02-09 21:14:16Z btolman $
use strict;
use warnings;

package main;

my $P = PRSplot->new();
$P->Run(@ARGV);

#####################################################################################
package PRSplot;
#####################################################################################
# Name:    PRSplot
# Purpose: plot output of PRSolve (R3)
# Author:  B W Tolman
# Version  3.0 see below in sub Clear: my $version
# Created: June 2007
# Mods:
#####################################################################################
use File::stat;

#####################################################################################
# constructor
sub new {
   my $class = shift;
   my $self = {};
   bless($self,$class);
   $self->Clear();              # here is where the data is defined.

   return $self;
}

#####################################################################################
# Define all data and fill with defaults
sub Clear {
   my $self = shift;

   $self->{version} = '3.0 Dec 1, 2014';
   #$self->{debug}=1;

   my $gnu = "gnuplot";
   if($^O eq 'MSWin32') { $gnu = "start /B C:\\gnuplot\\bin\\wgnuplot"; }

   # define the syntax page, then from it define options and defaults
   # '--' defines arg, '<..>' makes it value (non-bool), and '(..)$' defines default
   # note that short options are possible but must be hardwired below
   # NB. RPV/SPV exist only here, not in PRSolve
   $self->{syntax} = <<"END_OF_SYNTAX";
Usage: PRSplot (or $0) (ver. $self->{version}) [options]
 Plot the position solutions in a PRSolve (pseudorange solution) output file; the
  plot may include the solution residuals (XYZ or NEU), the clock solution,
  the number of satellites, DOPs and the RMS residual. Scatter plots (X vs Y and
  Y vs Z) may also be produced.
 The default plot uses tag RPV and includes XYZ position residuals (vs average),
  clock(s), RMS residual of fit and number of satellites.

 Options (default):
# Input data
  --file <f>   input PRSolve log file <f> [-f] (prs.log)
  --desc <d>   use solution descriptor [defaults to first in PRSolve file][-d] ()
                  <d> may be regex used to find descriptor amoung those present
  --tag <t>    choices as follows [-t] (RPV)
                  RPF  RAIM ECEF XYZ solution
                  RPV  RAIM ECEF XYZ solution minus average position
                  RPR  RAIM ECEF XYZ solution minus apriori position
                  RNE  RAIM North-East-Up solution minus apriori position
                  SPS  Simple ECEF XYZ solution
                  SPV  Simple ECEF XYZ solution minus average position
                  SPR  Simple ECEF XYZ solution minus apriori position
                  SNE  Simple North-East-Up solution minus apriori position
# Edit data
  --beg <[w,]s>  ignore data before this GPS <week,sow> time [-b] (0,0)
  --end <[w,]s>  ignore data after this GPS <week,sow> time [-e] (9999,604800)
  --nosus        ignore 'suspect' solutions as well as invalid [NV] ones
  --no4          reject solutions using only 4 satellites
# What to plot [one or more must be given]
  --scatter    plot 'X vs Y' and 'Y vs Z' [or 'E vs N' and 'E vs U']
  --pos        plot position [residuals]
  --clk        plot clock bias for each system
  --rms        plot RMS residual, Nsats, and RAIM slope
  --dop        plot GDOP, PDOP, and Nsats
  --sats       plot only Nsats and the number of rejected satellites
# Details of plot(s)
  --gnuplot <g>   name, with path, of the gnuplot executable ($gnu)
  --yrange <l:h>  set range of left axis on position plots to l(low)-h(high) ()
  --y2range <l:h> set range of right axis on position plots to l(low)-h(high) ()
  --psize <s>     point size (0.5)
  --geometry <g>  plot geometry (size) in pixels [-g] (960x350)
  --long          plot with 'long' geometry 1200x400
  --nogrid        plot without grid
# Output options (MSWin is always dirty)
  --png <f>    output png to file <f>.png [choose only one plot] ()
  --font <f>   use font <f> on the .png output (Droid Sans Mono, 10)
  --dirty      leave behind gnuplot file prsplotpos.gp, etc.
  --quiet      don't print anything to screen [-q]
  --help       print this message and quit [-h]
END_OF_SYNTAX

   # remember two hashes - boolean args and args with a value
   $self->{boolArgs} = {};
   $self->{valueArgs} = {};

   # hash for tags, list for errors
   $self->{tags} = {};
   @{$self->{errors}} = ();

   # parse the syntax page
   my $arg;
   foreach (split /\n/,$self->{syntax}) {
      chomp;                   # NB chomp removes the current contents of $/ from EOL
      s/\r//g;                 # but $/ is OS-dependent, so a dos file on linux has \r
      if(m/^\s*#/) {
         $self->{debug} and print "Comment $_\n";
      }
      elsif(m/^\s+--/) {
         ($arg) = /^\s+--(\S+) /;
         $self->{debug} and print "arg: $arg ";
         if(m/^\s+--$arg\s+<[^>]+> / and m/ \(.*\)$/) { #m/ \([^\)]*\)$/) 
            ($self->{$arg}) = / \(([^)]*)\)$/;
            $self->{valueArgs}->{$arg} = 1;
            $self->{debug} and print " => '$self->{$arg}'\n";
         }
         else {
            $self->{$arg} = '';
            $self->{boolArgs}->{$arg} = 1;
            $self->{debug} and print " = bool\n";
         }
      }
      elsif($arg and $arg eq 'tag') {  # also pull out the descriptions of each tag
         my ($key,$value) = /\s+([RPFSNEV]{3})\s+(.*)/;
         $self->{tags}->{$key} = $value;
         $self->{debug} and print "tag: $key = $value\n";
      }
   }

   return;
}

#####################################################################################
# main program
sub Run {
   my $self = shift;

   return if $self->ProcessArgs(@_);

   if($self->{help}) {
      print $self->{syntax};
      print "\n";
      print $self->Dump();
      exit;
   }

   return if $self->ProcessInput();

   #print $self->Dump() if not $self->{quiet};

   return if $self->ProcessData();

   # any plots?
   if(not $self->{pos} and not $self->{clk} and not $self->{rms} and
      not $self->{dop} and not $self->{sats} and not $self->{scatter})
   {
      if(not $self->{quiet}) {
         print "Warning - no plots selected (--pos|clk|rms|dop|sats|scatter).\n";
         foreach(reverse @{$self->{solns}}) { print " Found descriptor: $_\n"; }
      }
      return;
   }

   # do it
   if($self->{pos})  { return if $self->Plot('pos'); }
   if($self->{clk})  { return if $self->Plot('clk'); }
   if($self->{rms})  { return if $self->Plot('rms'); }
   if($self->{dop})  { return if $self->Plot('dop'); }
   if($self->{sats}) { return if $self->Plot('sats'); }
   if($self->{scatter}) { return if $self->Scatter(); }

   # call rstats for clock difference(s)
   my @fields = split /\s+/,$self->{pdata}[0];
   #print "N fields is $#fields\n";
   if($#fields > 11) {
      my ($clk,$lab) = (0,$fields[10]. "-" . $fields[11]);
      if($self->{clk})  {
         if(not open(FPIPE, "| rstats -x 1 -y 3 -p 5 -b2 -l $lab")) {
            print "Can't open pipe to rstats";
            return;
         }
         foreach (@{$self->{pdata}}) {
            #push @{$self->{pdata}},join(' ',($sow, $nsv, $rej, $x, $y, $z,
            #          $rms, $slope, $gdop, $pdop, @clocklabels, @clocks));
            @fields = split /\s+/,$_;
            $clk = $fields[12]-$fields[13];
            print FPIPE "$fields[0] $lab $clk\n";
         }
         close FPIPE;
      }
   }

   return;
}

#####################################################################################
sub Errors {
   my $self = shift;
   foreach(@{$self->{errors}}) { print "Error: $_\n"; }
}

#####################################################################################
sub ProcessArgs {
   my $self = shift;

   #if($#_ == -1) { push @_,"--help"; }
   while($#_ > -1) {
      my $arg = shift;
      # short options
      #if($arg =~ m/-v/) { $arg = '--verbose'; }
      if($arg eq '-q') { $arg = '--quiet'; }
      if($arg eq '-h') { $arg = '--help'; }
      if($arg eq '-f') { $arg = '--file'; }
      if($arg eq '-b') { $arg = '--beg'; }
      if($arg eq '-e') { $arg = '--end'; }
      if($arg eq '-t') { $arg = '--tag'; }
      if($arg eq '-d') { $arg = '--desc'; }
      if($arg eq '-g') { $arg = '--geometry'; }
      if($arg eq 'scat') { $arg = '--scatter'; }
      # long options
      $arg =~ s/^--//;        # this makes the -- optional (!)
      if(not defined $self->{$arg}) {
         push @{$self->{errors}}, "Error - unknown option: $arg";
      }
      elsif(defined($self->{valueArgs}->{$arg})) {
         $self->{$arg} = shift;
      }
      else {
         $self->{$arg} = 1;
      }
   }

   if(@{$self->{errors}}) { $self->Errors(); return 'fail'; }

   return;
}

#####################################################################################
# Dump - dump the current configuration to a string and return it
sub Dump {
   my $self = shift;
   my $msg = "PRSplot (ver $self->{version}) configuration:\n";

   #foreach(keys %{$self->{tags}}) {
   #   if($self->{tags}->{$_} ne '') { $msg .= " tag $_ is $self->{tags}->{$_}\n"; }
   #}

   foreach(keys %{$self->{valueArgs}}) {
      if($self->{$_} ne '') {
         $msg .= " --$_ " .
                 ($self->{$_} =~ m/\s/ ? "\"$self->{$_}\"" : $self->{$_})
                 . "\n"; }
   }

   foreach(keys %{$self->{boolArgs}}) {
      if($self->{$_} ne '') { $msg .= " --$_\n"; }
   }

   return $msg;
}

#####################################################################################
# Process the input options
sub ProcessInput {
   my $self = shift;

   # file name required - this never reached
   if($self->{file} eq '') {
      push @{$self->{errors}}, "--file must be provided.";
   }

   # tag must exist
   if($self->{tag} eq '') {
      push @{$self->{errors}}, "--tag required.";
   }
   elsif(not defined $self->{tags}->{$self->{tag}}) {
      push @{$self->{errors}}, "invalid tag ($self->{tag}).";
   }

   # parse time limits
   if($self->{beg} =~ m/,/) {
      ($self->{begweek},$self->{beg}) = split /,/,$self->{beg};
   }
   if($self->{end} =~ m/,/) {
      ($self->{endweek},$self->{end}) = split /,/,$self->{end};
   }

   # fail?
   if(@{$self->{errors}}) { $self->Errors(); return 'fail'; }

   # initialize stuff
   @{$self->{pdata}} = ();
   @{$self->{solns}} = ();

   # wgnuplot on windows won't pipe
   if($^O eq 'MSWin32') { $self->{dirty} = 1; }

   return;
}

#####################################################################################
# Read the input data file and preprocess
sub ProcessData {
   my $self = shift;

   # does file exist and does it have data?
   my $istat = stat($self->{file});
   if(not $istat or $istat->size == 0) {
      push @{$self->{errors}},
            "Input file '$self->{file}' does not exist or is empty. Abort.";
      $self->Errors();
      return 'fail';
   }

   # slurp the file
   my $buffer;
   {
      local $/ = undef;
      if(not open(FPIN, "$self->{file}")) {
         push @{$self->{errors}}, "Can't open file $self->{file}";
         $self->Errors();
         return 'fail';
      }
      $buffer = <FPIN>;
      close FPIN;
   }
   my @blines = split /\n/,$buffer;

   # get the (descriptor) --------------------------------
   my ($n,$desc,%ns);

   # first get all descriptors with results (Weighted average ... RAIM solution)
   foreach (0..$#blines) {          # loop over lines, starting at end
      if($blines[$#blines-$_] =~ m/Weighted average/ and
         $blines[$#blines-$_] =~ m/RAIM solution/)
      {
         $n = $#blines-$_;
         $desc = $blines[$#blines-$_];
         $desc =~ s/Weighted average //;
         $desc =~ s/ RAIM solution//;
         push @{$self->{solns}}, $desc;
         $ns{$desc} = $n;
      }
      last if($blines[$#blines-$_] =~ m/^RPF/);      # quit looking
   }

   # note the first 'Weighted average...'
   if(not defined $n) {
      push @{$self->{errors}}, "Error - file is incomplete. Abort.";
      $self->Errors();
      return 'fail';
   }

   # pick the descriptor to use, order as in file
   if($self->{desc} eq "") {        # no input, take first found
      $self->{desc} = @{$self->{solns}}[-1];
   }
   elsif(grep {$_ eq $self->{desc}} @{$self->{solns}}) {
      print "desc '$self->{desc}' found!\n" ;
   }
   else {                           # search for closest match
      foreach(reverse @{$self->{solns}}) {
         if($_ =~ m/$self->{desc}/) {
            $self->{desc} = $_;
            last;
         }
      }
   }
   if(not $self->{quiet}) { print "Descriptor is $self->{desc}\n"; }

   # get the average solution and covariance
   $n = $ns{$self->{desc}};         # get the right line number
   my ($no,$sX,$sY,$sZ);
   my @fields = split /\s+/,$blines[$n+1];
   my $X = $fields[1];
   my $Y = $fields[2];
   my $Z = $fields[3];
   my $N = $fields[$#fields];
   $self->{debug} and print "X,Y,Z,N is $X,$Y,$Z,$N\n";

   ($no,$sX,$no,$no) = split /\s+/,$blines[$n+3];
   ($no,$no,$sX,$no,$no) = split /\s+/,$blines[$n+4];
   #print "sX is $sX\n";
   ($no,$no,$no,$sY,$no) = split /\s+/,$blines[$n+5];
   #print "sY is $sY\n";
   ($no,$no,$no,$no,$sZ) = split /\s+/,$blines[$n+6];
   #print "sZ is $sZ\n";
   $sX=sqrt($sX);
   $sY=sqrt($sY);
   $sZ=sqrt($sZ);

   if(not $self->{quiet}) {
      print "Weighted average position solution: ($N epochs)\n";
      print sprintf(" X %14.4f +/- %.4f m\n",$X,$sX);
      print sprintf(" Y %14.4f +/- %.4f m\n",$Y,$sY);
      print sprintf(" Z %14.4f +/- %.4f m\n",$Z,$sZ);

      foreach (9..$n) {
         if($blines[$#blines-$n-$_] =~ m/Weighted average/ and
            $blines[$#blines-$n-$_] =~ m/NEU position residuals/)
         {
            my ($nr,$ea,$up);
            ($no,$nr,$ea,$up) = split /\s+/,$blines[$#blines-$n-$_+1];
            ($no,$no,$sX,$no,$no) = split /\s+/,$blines[$#blines-$n-$_+4];
            ($no,$no,$no,$sY,$no) = split /\s+/,$blines[$#blines-$n-$_+5];
            ($no,$no,$no,$no,$sZ) = split /\s+/,$blines[$#blines-$n-$_+6];
            print "Weighted average NEU residual: ($N epochs)\n";
            print sprintf(" N %8.4f +/- %.4f m\n",$nr,sqrt($sX));
            print sprintf(" E %8.4f +/- %.4f m\n",$ea,sqrt($sY));
            print sprintf(" U %8.4f +/- %.4f m\n",$up,sqrt($sZ));
            last;
         }
      }
   }


   # process the file --------------------------------
   $N = 0;
   my ($sow,$Nbad) = (0,0);
   $self->{pdata} = ();
   $self->{week} = -1;
   $self->{rejects} = {};

   # RPV/SPV exist only here, not in PRSolve
   my $tag = $self->{tag};
   $tag = 'RPF' if $self->{tag} eq 'RPV';
   $tag = 'SPS' if $self->{tag} eq 'SPV';

   # + is a special character - must escape it for matches
   $desc = $self->{desc};
   $desc =~ s/\+/\\\+/g;

   # stuff for $self->{pdata}
   my ($nsv,$rej,$x,$y,$z,$rms,$slope,$gdop,$pdop)=(0,0,0,0,0,0,0,0,0);
   my @clocks = ();
   my @clocklabels = ();
   $sow = '0';
   foreach(@blines) {
      chomp;
      s/^\s+//;

      # epoch is complete - blank line and status is on - save data in lines
      if($_ eq '' and $sow ne '0') {
         # sow nsv x y z rms slope gdop pdop clk clk...
         push @{$self->{pdata}},join(' ',($sow, $nsv, $rej, $x, $y, $z,
                   $rms, $slope, $gdop, $pdop, @clocklabels, @clocks));
         #print "Save line: ", ${$self->{pdata}}[$N],"\n";
         $N++;

         # prepare for next epoch
         $sow='0'; @clocks=(); @clocklabels=();
         next;
      }

      next unless m/ $desc /;    # must be our solution
      next if m/ PFR /;          # for now...
      $self->{debug} and print "Good line (tag $tag) >$_<\n";

      @fields = split /\s+/, $_;

      # time
      if($fields[0] eq 'RPF' and $fields[2] eq 'NAV') {
         next if(defined $self->{begweek} and $fields[3] < $self->{begweek});
         last if(defined $self->{endweek} and $fields[3] > $self->{endweek});
         next if($fields[4] < $self->{beg});
         last if($fields[4] > $self->{end});
         # get week
         if($self->{week} == -1) { $self->{week} = $fields[3]; }
         # week rollover
         $sow = sprintf("%.3f", ($fields[3]-$self->{week})*604800.0+$fields[4]);
         $self->{debug} and print "week,sow : $self->{week},$sow\n";

         # get clocks
         @clocks = @clocklabels = ();
         $n=8;
         while(1) {
            last if($fields[$n] =~ m/\(/ or $n > $#fields);
            push @clocklabels, $fields[$n];
            push @clocks, $fields[$n+1];
            $n += 2;
         }
         if(not defined $self->{nclks}) { $self->{nclks} = ($n-8)/2; }
         $self->{debug} and print "clocks : ",join(' ',(@clocklabels,@clocks)),"\n";
      }

      # get position and rejects
      if($fields[0] eq $tag and ($fields[2] eq 'POS' or $fields[2] eq 'NAV')) {
         #$no = $_; $no =~ s/.*\s+\((-?\d+\D*)\) V$/$1/; print "Test is >$no<\n";
         # rejects
         if(m/NV$/ or ($self->{nosus}
               and (m/missed trop. corr./ or m/large slope/ or m/large RMS/))) {
            $self->{debug} and print "Reject >$_<\n";
            $Nbad++;
            $_ =~ s/.*\s+\((-?\d+\D*)\) NV$/$1/;
            # NB all three can appear in the same line
            if($_ =~ m/large RMS/) { $self->{rejects}->{'1(r)'}++; }
            if($_ =~ m/large slope/) { $self->{rejects}->{'1(s)'}++; }
            if($_ =~ m/missed trop. corr./) { $self->{rejects}->{'1(t)'}++; }
            $sow = '0';       # don't add to lines
            next;
         }

         # position
         $x = $fields[5];
         $y = $fields[6];
         $z = $fields[7];
         if($self->{tag} eq 'RPV' or $self->{tag} eq 'SPV') {
            $x = sprintf("%16.6f",$x - $X);
            $y = sprintf("%16.6f",$y - $Y);
            $z = sprintf("%16.6f",$z - $Z);
         }
         $self->{debug} and print "position : $x $y $z ($X $Y $Z)\n";
      }

      # get rest
      if($fields[0] eq 'RPF' and $fields[2] eq 'DAT') {    # DAT will come first
         $rej = $fields[6];  #Nused for now
      }
      if($fields[0] eq 'RPF' and $fields[2] eq 'RMS') {
         $nsv = $fields[5];   #Ngood
         $rej -= $nsv;
         if($self->{no4} and $nsv <= 4) {
            $Nbad++;
            $self->{rejects}->{'4svs'}++;
            $sow = '0';
            next;
         }
         $rms = $fields[6];
         $slope = $fields[10];
         $gdop = $fields[9];
         $pdop = $fields[8];
      }
   }

   if($N == 0) {
      push @{$self->{errors}},
         "No data to plot! (Are tag or desc not found in file?) Abort.";
   }
   #print "Lines:\n",join("\n",@{$self->{pdata}}),"\n";

   if(not $self->{quiet}) {
      if($Nbad) {
         print "There were $Nbad rejected or degraded epochs :\n";
         my %h;
         $h{'1(t)'} = 'possibly degraded; trop correction missed';
         $h{'1(r)'} = 'possibly degraded; RMS residual exceeded limit';
         $h{'2'} = 'possibly degraded; RMS residual exceeded limit';
         $h{'1(s)'} = 'possibly degraded; RAIM slope exceeded limit';
         $h{'1'} = 'possibly degraded; RAIM slope exceeded limit';
         $h{-1} = 'Algorithm failed to converge';
         $h{-2} = 'Algorithm found singularity';
         $h{-3} = 'Not enough good data';
         $h{-4} = 'No ephemeris';
         $h{'4svs'} = 'Only 4 satellites formed the solution';
         foreach(keys %{$self->{rejects}}) {
            if($_ =~ m/1\(/) {
               print "  $self->{rejects}->{$_} suspect with code $_ ($h{$_}).\n";
            }
            else {
               print "  $self->{rejects}->{$_} rejects with code $_ ($h{$_}).\n";
            }
         }
      }
      else {
         print "There were NO rejected epochs\n";
      }
   }

   if(@{$self->{errors}}) { $self->Errors(); return 'fail'; }
   return;
}

#####################################################################################
# Plot the data
sub Plot {
   my $self = shift;
   my $arg = shift;
   #print "arg in Plot is $arg\n";

   my $geo = $self->computeGeometry($arg);   # call for all: MSWin32 -> wgnuplot.ini

   #----------------------------------------------
   # open a pipe to gnuplot
   if($self->{dirty}) {
      if(not open(FPIPE, ">prsplot$arg.gp")) {
         push @{$self->{errors}}, "Can't open file prsplot$arg.gp";
         $self->Errors();
         return 'fail';
      }
      if(not $self->{quiet}) { print "Write to gnuplot file prsplot$arg.gp\n"; }
   }
   else {
      if(not open(FPIPE,
            "| $self->{gnuplot} -persist -geometry $geo > /dev/null 2>&1")) {
         push @{$self->{errors}}, "Can't open pipe to gnuplot";
         $self->Errors();
         return 'fail';
      }
   }

   #----------------------------------------------
   if($^O eq 'linux') {
      #print FPIPE "set term x11 enhanced font 'luxi sans,17'\n";
      #ubuntu gnuplot 4.3
      my ($ox,$oy,$sx,$sy);
      if($geo =~ m/[xX]/) {
         if($geo =~ m/\+/) {
            ($sx,$sy,$ox,$oy) = ($geo =~ /(\d+)[xX](\d+)\+(-?\d+)\+(-?\d+)/);
         }
         else {
            ($sx,$sy) = ($geo =~ /(\d+)[xX](\d+)/);
         }
      }
      else { ($ox,$oy) = ($geo =~ /\+?(-?\d+)\+(-?\d+)/); }
      # defaults
      $sx = 960 unless $sx; $sy = 350 unless $sy;
      $ox = 0 unless $ox; $oy = 0 unless $oy;
      #print "GEO $geo $sx X $sy + $ox + $oy\n";
      my $wtitle = uc($arg) . " '$self->{desc}' $self->{file}";
      if($self->{png} eq '') {
         print FPIPE "set term wxt enhanced size $sx,$sy title \"$wtitle\"\n";
         print FPIPE "set mouse\n";
         print FPIPE "#set term pngcairo enhanced size $sx,$sy "
            . "font \"$self->{font}\"\n";
         print FPIPE "#set output \"filename\"\n";
         print FPIPE "#unset mouse\n";
      }
      else {
         print FPIPE "set term pngcairo enhanced size $sx,$sy "
            . "font \"$self->{font}\"\n";
         print FPIPE "set output \"$self->{png}\"\n";
         print FPIPE "unset mouse\n";
         print "Write png to file $self->{png}\n";
      }
   }

   #----------------------------------------------
   # build title and y labels
   if($arg eq 'pos') {
      print FPIPE "set title \"$self->{tags}->{$self->{tag}}\\n" .
                           "PRSolve Solution $self->{desc} in $self->{file}\"\n";
      print FPIPE "set ylabel \"Meters\"\n";
   }
   if($arg eq 'clk') {
      print FPIPE "set title \"Clock Bias Solution\\n" .
                           "PRSolve Solution $self->{desc} in file $self->{file}\"\n";
      if($self->{nclks} > 1) {
         print FPIPE "set autoscale y2\n";
         print FPIPE "set ytics nomirror\n";
         print FPIPE "set y2tics\n";
      }
   }
   if($arg eq 'dop') {
      print FPIPE "set title \"GDOP, PDOP and Number Sats\\n" .
                           "PRSolve Solution $self->{desc} in $self->{file}\"\n";
      print FPIPE "set autoscale y2\n";
      print FPIPE "set ytics nomirror\n";
      print FPIPE "set y2tics\n";
      print FPIPE "set ylabel \"DOP\"\n";
      print FPIPE "set y2label \"Number Sats\"\n";
   }
   if($arg eq 'rms') {
      print FPIPE "set title \"RMS Residual, Number Sats and RAIM Slope\\n" .
                           "PRSolve Solution $self->{desc} in $self->{file}\"\n";
      print FPIPE "set autoscale y2\n";
      print FPIPE "set ytics nomirror\n";
      print FPIPE "set y2tics\n";
      print FPIPE "set ylabel \"RMS residual (meters) or Number Sats\"\n";
      print FPIPE "set y2label \"RAIM Slope (meters)\"\n";
   }
   if($arg eq 'sats') {
      print FPIPE "set title \"Satellites: number used and number rejected\"\n";
      print FPIPE "set ylabel \"Number of satellites\"\n";
   }

   # add other stuff
   print FPIPE "set xlabel \"GPS Seconds of Week $self->{week}\"\n";
   #print FPIPE "unset mouse\n";
   if(not $self->{nogrid}) { print FPIPE "set grid\n"; }
   if($self->{yrange} ne '') { print FPIPE "set yrange [$self->{yrange}]\n"; }
   if($self->{y2range} ne '') { print FPIPE "set y2range [$self->{y2range}]\n"; }
   my ($X,$Y,$Z) = ('X','Y','Z');   # key
   ($X,$Y,$Z) = ('N','E','U') if($self->{tag} eq 'RNE' or $self->{tag} eq 'SNE');
   my $ps = $self->{psize};         # point size
   my $N = 0;                       # number of curves = how many times to repeat data
   my $plot = "plot '-'";           # first 'file', replace with , ''
   my $axes = 'axes x1y2';

   # add plot description(s)
   if($arg eq 'pos') {
      print FPIPE "$plot using 1:6 t \"$Z\" with points ps $ps";
      $plot = ", ''";
      print FPIPE "$plot using 1:5 t \"$Y\" with points ps $ps";
      print FPIPE "$plot using 1:4 t \"$X\" with points ps $ps";
      $N += 3;
   }
   if($arg eq 'clk') {
      #print "$self->{pdata}[0]\n";
      my @fields = split /\s+/,$self->{pdata}[0];
      my $nclks = $self->{nclks};
      my @lab;
      for(1..$nclks) { push @lab,$fields[9+$_]; }
      #print "Clock labels are ",join(' ',@lab),"\n";
      print FPIPE "set ylabel \"$lab[0] Clock (meters)\"\n";
      if($nclks > 1)  { print FPIPE "set y2label \"Other Clock(s) (meters)\"\n"; }
      for(0..$nclks-1) {
         my $n = 11+$nclks+$_;
         print FPIPE "$plot using 1:$n $axes t \"$lab[$_]\" with linespoints ps $ps";
         $plot = ", ''";
         $axes = '';
         $N++;
      }
   }
   if($arg eq 'rms') {
      print FPIPE "$plot using 1:7 t \"RMS\" with lines";
      $plot = ", ''";
      print FPIPE "$plot using 1:2 t \"Nsat\" with lines";
      print FPIPE "$plot using 1:8 $axes t \"Slope\" with lines";
      $N += 3;
   }
   if($arg eq 'dop') {
      print FPIPE "$plot using 1:9 t \"GDOP\" with lines";
      $plot = ", ''";
      print FPIPE "$plot using 1:10 t \"PDOP\" with lines";
      print FPIPE "$plot using 1:2 $axes t \"Nsat\" with lines";
      $N += 3;
   }
   if($arg eq 'sats') {
      print FPIPE "$plot using 1:2 t \"used\" with lines";
      $plot = ", ''";
      print FPIPE "$plot using 1:3 t \"rejected\" with linespoints lt 3 pt 7";
      $N = 2;
   }
   print FPIPE "\n";

   foreach(1..$N) {
      foreach (@{$self->{pdata}}) { print FPIPE "$_\n"; }
      print FPIPE "e\n";
   }

   close FPIPE;

   #----------------------------------------------
   # run gnuplot
   #my $gp_options;
   #if($^O eq 'MSWin32') {
   #   $gp_options = "-e \"pause -1 \\\"Close plot window\\\" -e exit"; }
   #else { $gp_options = " -persist -geometry $geo"; }
   #if($self->{dirty}) { system "$self->{gnuplot} $gp_options prsplot.gp"; }
   if($self->{dirty}) {
      if($^O eq 'MSWin32') {
         system "$self->{gnuplot} prsplot$arg.gp "
            . "-e \"pause -1 \\\"Close prsplot$arg window\\\" -e exit";
      }
      else {
         system "$self->{gnuplot} -persist -geometry $geo prsplot$arg.gp "
            . "> /dev/null 2>&1";
      }
   }

   return;
}

#####################################################################################
# do the scatter plots
sub Scatter {
   my $self = shift;

   my $geo = $self->computeGeometry('scat');   # call for all: MSWin32 -> wgnuplot.ini

   #----------------------------------------------
   # Two scatter plots
   my $ps = $self->{psize};
   foreach(0..1) {
      # open a pipe to gnuplot
      my $file = "prsplot" . ($_ == 0 ? 'XY' : 'YZ') . '.gp';
      if($self->{dirty}) {
         if(not open(FPIPE, ">$file")) {
            push @{$self->{errors}}, "Can't open file $file";
            $self->Errors();
            return 'fail';
         }
         if(not $self->{quiet}) { print "Write to gnuplot file $file\n"; }
      }
      else {
         if(not open(FPIPE, "| $self->{gnuplot} -persist -geometry $geo "
               . "> /dev/null 2>&1")) {
            push @{$self->{errors}}, "Can't open file $file";
            $self->Errors();
            return 'fail';
         }
      }

      #----------------------------------------------
      if($^O eq 'linux') {
         #print FPIPE "set term x11 enhanced font 'luxi sans,17'\n";
         #ubuntu gnuplot 4.3
         my ($ox,$oy,$sx,$sy);
         if($geo =~ m/[xX]/) {
            if($geo =~ m/\+/) {
               ($sx,$sy,$ox,$oy) = ($geo =~ /(\d+)[xX](\d+)\+(-?\d+)\+(-?\d+)/);
            }
            else {
               ($sx,$sy) = ($geo =~ /(\d+)[xX](\d+)/);
            }
         }
         else { ($ox,$oy) = ($geo =~ /\+?(-?\d+)\+(-?\d+)/); }
         # defaults
         $sx = 640 unless $sx; $sy = 480 unless $sy;
         $ox = 0 unless $ox; $oy = 0 unless $oy;
         #print "GEO $geo $sx X $sy + $ox + $oy\n";
         my $wtitle = "'$self->{desc}' $self->{file}";
         if($self->{tag} eq 'RNE' or $self->{tag} eq 'SNE') {
            $wtitle = ($_ == 0 ? "N-E ":"Up-E ") . $wtitle;
         } else {
            $wtitle = ($_ == 0 ? "X-Y ":"Y-Z ") . $wtitle;
         }
         print FPIPE "set term wxt enhanced size $sx,$sy title \"$wtitle\"\n";
         print FPIPE "set mouse\n";
         print FPIPE "#set term pngcairo enhanced size $sx,$sy "
            . "font \"$self->{font}\"\n";
         print FPIPE "#set output \"filename\"\n";
         print FPIPE "#unset mouse\n";
      }
      if(not $self->{nogrid}) { print FPIPE "set grid\n"; }
      print FPIPE "set title \"$self->{tags}->{$self->{tag}}\\n" .
                              "PRSolve $self->{desc} in file $self->{file}\"\n";

      # sow nsv nrej x y z clk rms slope glo
      my $cols = '4:5';
      if($self->{tag} eq 'RNE' or $self->{tag} eq 'SNE') {
         if($_ eq 0) {
            print FPIPE "set xlabel \"East residual (m)\"\n";
            print FPIPE "set ylabel \"North residual (m)\"\n";
            $cols = '5:4';
         }
         else {
            print FPIPE "set xlabel \"East residual (m)\"\n";
            print FPIPE "set ylabel \"Up residual (m)\"\n";
            $cols = '5:6';
         }
      }
      else {
         if($_ eq 0) {
            print FPIPE "set xlabel \"ECEF X (m)\"\n";
            print FPIPE "set ylabel \"ECEF Y (m)\"\n";
            $cols = '4:5';
         }
         else {
            print FPIPE "set xlabel \"ECEF Y (m)\"\n";
            print FPIPE "set ylabel \"ECEF Z (m)\"\n";
            $cols = '5:6';
         }
      }
      #print FPIPE "unset mouse\n";
      if($self->{yrange} ne '') {
         print FPIPE "set xrange [$self->{yrange}]\n";
         print FPIPE "set yrange [$self->{yrange}]\n";
      }
      my $lt = $_+1;
      print FPIPE "plot '-' using $cols t \"\" with points lt $lt ps $ps";
      print FPIPE "\n";
      foreach (@{$self->{pdata}}) { print FPIPE "$_\n"; }
      print FPIPE "e\n";
      close FPIPE;

      #----------------------------------------------
      # run gnuplot
      if($self->{dirty}) {
         if($^O eq 'MSWin32') {
            system "$self->{gnuplot} $file "
               . "-e \"pause -1 \\\"Close $file window\\\" -e exit";
         }
         else {
            system "$self->{gnuplot} -persist -geometry $geo $file > /dev/null 2>&1";
         }
      }
   }

   return;
}

#####################################################################################
sub computeGeometry {
   my $self = shift;
   my $arg = shift;
   my $geo;

   if($^O eq 'MSWin32') {
      if($self->{long}) { $self->{geometry} = '1200x400'; }
      if(open(FPINI, "$ENV{GNUPLOT}\\wgnuplot.ini")) {
         local $/ = undef;
         my $gpbuff = <FPINI>;
         close FPINI;
         if(open(FPINI, ">$ENV{GNUPLOT}\\wgnuplot.ini.bak")) {
            print FPINI "$gpbuff";
            close FPINI;
            if($self->{geometry} =~ m/-?9?[ucl][clr]/
               or $self->{geometry} =~ m/-?16[abcd][abcd]/) {
            #print "copy $ENV{GNUPLOT}\\gp$geo.ini $ENV{GNUPLOT}\\wgnuplot.ini\n";
            `copy $ENV{GNUPLOT}\\gp$self->{geometry}.ini $ENV{GNUPLOT}\\wgnuplot.ini`;
            }
            elsif(open(FPINI, ">$ENV{GNUPLOT}\\wgnuplot.ini")) {
               #print "gnuplot ini:\n$gpbuff\n";
               my ($ox,$oy,$sx,$sy);
               if($self->{geometry} =~ m/[xX]/) {
                  if($self->{geometry} =~ m/\+/) {
                     ($sx,$sy,$ox,$oy)
                        = ($self->{geometry} =~ /(\d+)[xX](\d+)\+(-?\d+)\+(-?\d+)/);
                  }
                  else {
                     ($sx,$sy)
                        = ($self->{geometry} =~ /(\d+)[xX](\d+)/);
                  }
               }
               else { ($ox,$oy) = ($self->{geometry} =~ /\+?(-?\d+)\+(-?\d+)/); }
               # defaults
               $sx = 960 unless $sx; $sy = 350 unless $sy;
               $ox = 0 unless $ox; $oy = 0 unless $oy;
               #print "GEO $self->{geometry} $sx X $sy + $ox + $oy\n";
               my @gplines = split /\n/, $gpbuff;
               foreach (@gplines) {
                  if(m/GraphOrigin/) { print FPINI "GraphOrigin=$ox $oy\n"; }
                  elsif(m/GraphSize/) { print FPINI "GraphSize=$sx $sy\n"; }
                  else { print FPINI "$_\n"; }
               }
               close FPINI;
            }
            else { print "Can't open wgnuplot.ini for writing\n"; }
         }
         else { print "Can't open wgnuplot.ini.bak\n"; }
      }
      $geo = '';
   }
   else {
      $geo = "$self->{geometry}";
      if($arg eq 'scat') { $geo = '640x480'; }
      if($self->{long}) { $geo = '1200x400'; }
   }

   return $geo;
}

1; # end of package PRSplot;

#####################################################################################
