#!/usr/bin/env python
# A routine to plot the output of ddGen.

def s2pt(time_str, format_str):
    """String to plot time: Computes the matplotlib time from the indicated string and format.
    This assumes the input time is in UTC."""
    import datetime
    from matplotlib import dates
    return dates.date2num(datetime.datetime.strptime(time_str, format_str))

def read_ddGen_ddr(args):
    """Reads the output of the ddGen program specified in options.input_file,
    for the SVs in args.sv above the elevation in args.elev and returns a dict of a
    dict of a dict of an array. yeah. Note that args.sv is a list of ints with the empty list implying 'all'
    The two dicts returned are as follows:
    prn = int
    cc = carrier/code: "L1 P", "L1 Y", "L2 Y", "L1 C/A"....
    t = float, gregorian date
    ddr = float, double difference residual
    type is obs type: "phase" or "range"
    ddr_map{prn}{type}{cc}[[t,ddr],...]
    elev_map{prn}[[t,elev],...]
    """
    ddr_map = {}
    elev_map = {}

    # This parses the 'raw' output lines of ddGen
    import re
    ddr_re = re.compile("(?P<time>\d{4} +\d{1,3} \d{2}:\d{2}:\d{2})(?P<fsec>\.\d) +"
                        "(?P<cc>L\d [0-9A-Z\/]+) +(?P<obs>\S+) +"
                        "(?P<sv1>\d{1,2}) +(?P<sv2>\d{1,2}) +"
                        "(?P<elev1>\d+\.\d+) +(?P<elev2>\d+\.\d+) +"
                        "(?P<ddr>-?\d+\.\d+) +(?P<hh>\d\d)")

    for line in args.input_file:
        line = line.strip()
        if args.debug>2:
            print line
        if len(line)==0 or line[0] =='#': continue

        c = ddr_re.match(line)
        if c:
            if args.debug>2: print c.groupdict()
            if (c.group('hh') != "00" and (not args.plot_unhealthy)):
                if debug>1: print "ignoring:",line
                continue
            t = s2pt(c.group('time'),  "%Y %j %H:%M:%S") + float(c.group('fsec'))/86400.0
            ddr = float(c.group('ddr'))
            cc = c.group('cc') # code carrier
            obs = c.group('obs') # "range/phase/doppler"
            sv = int(c.group('sv2')) # higher (reference) SV
            elev = float(c.group('elev2'))
            if args.sv and sv not in args.sv: continue
            if elev < args.elev: continue

            if sv not in ddr_map:
                ddr_map[sv] = {}
            if obs not in ddr_map[sv]:
                ddr_map[sv][obs] = {}
            if cc not in ddr_map[sv][obs]:
                ddr_map[sv][obs][cc] = []
            ddr_map[sv][obs][cc].append([t,ddr])

            if sv not in elev_map:
                elev_map[sv] = []
            if len(elev_map[sv]) > 0 and elev_map[sv][-1][0] != t:
                elev_map[sv].append([t,elev])

    from numpy import array
    for sv in ddr_map:
        for t in ddr_map[sv]:
            for cc in ddr_map[sv][t]:
                ddr_map[sv][t][cc] = array(ddr_map[sv][t][cc])

    return ddr_map, elev_map


def parse_args():
    import sys
    import argparse
    parser = argparse.ArgumentParser(description="Plots the output of ddGen.")

    parser.add_argument("input_file", metavar='fn', nargs='?', type=argparse.FileType('r'), default=sys.stdin,
                        help="Input data file, defaults to stdin.",)

    parser.add_argument("--type", dest="type", default="phase",
                      help="What type of obs to plot (range,phase,doppler). The default is %(default)s.")

    parser.add_argument("--carrier", dest="carrier", default="L1",
                      help="What carrier to plot (L1,L2,L5). The default is %(default)s.")

    parser.add_argument("--code", dest="code", default="GPSC/A",
                      help="What code to plot (GPSP,GPSY,GPSC/A). The default is %(default)s.")

    parser.add_argument("-d", "--debug", default=0, dest="debug", action="count",
                      help="Increase the debug.")

    parser.add_argument("-l", "--legend", dest="legend", action="store_true",
                      help="Include a legend on the plot.")

    parser.add_argument("-u","--no-unhlthy", dest="no_unhealthy", action="store_true",
                      help="Do not plot data from unhealthy SVs.")

    parser.add_argument("--sv", dest="sv", action="append", type=int,
                      help="What sv to plot. The default is to plot all.")

    parser.add_argument("--elev", dest="elev", default=0,
                      help="Ignore data from below this elevation. The default is %(default)s.")

    parser.add_argument("-t", dest="title",
                        help="Specify a title for the plot. Defaults to the name of the input stream.")         

    parser.add_argument("-f", dest="saveFig",
                      help="Save the figure to %(dest)s")

    parser.add_argument("-y", dest="ylim", type=float,
                      help="Fix the y range on the plot to be (%(dest)s,-%(dest)s).")

    args = parser.parse_args()

    if (args.title == None):
        args.title = args.input_file.name

    if (args.debug):
        print args

    return args


if __name__ == "__main__":
    args = parse_args()
    ddr_map, elev_map = read_ddGen_ddr(args)

    from pylab import *
    # A key handler for matplotlib
    def press(event):
        if event.key=='q' or event.key==' ':
            close()

    # Here we start generating the plots
    fig = figure()
    connect('key_press_event', press)
    yprops = dict(rotation=90, horizontalalignment='right', verticalalignment='center',
                  family='monospace', x=-0.01)
    scale_props = dict(horizontalalignment="right", verticalalignment="bottom", size=8, family="sans-serif") 

    ax1 = fig.add_axes([0.08, 0.10, 0.80, 0.85])

    req_cc = args.carrier + " " + args.code
    for sv in ddr_map:
        for ot in ddr_map[sv]:
            if args.type != 'all' and args.type != ot: continue
            for cc in ddr_map[sv][ot]:
                if cc != req_cc: continue
                time = ddr_map[sv][ot][cc][:,0]
                ddr = ddr_map[sv][ot][cc][:,1]
                ax1.plot_date(time, ddr, ".", label="{} {} {}".format(sv,cc,ot))

    if args.legend:
        ax1.legend(numpoints=2, pad=0.1, labelsep = 0, handlelen=0.005, handletextsep=0.01, axespad=0.0, loc=(1,0))
        leg = gca().get_legend()
        ltext = leg.get_texts()
        llines = leg.get_lines()
        lframe = leg.get_frame()
        lframe.set_facecolor('0.4')
        setp(ltext, size=8, family="sans-serif")
        setp(llines, linewidth=2)
        leg.draw_frame(False)

    ax1.set_ylabel('Double Difference (meters)', **yprops)
    ax1.grid(True)
    ax1.xaxis.set_major_formatter(DateFormatter("%02H:%02M\n%03j"))
    xlabels=ax1.get_xticklabels()
    ylabels=ax1.get_yticklabels()
    setp(xlabels, fontsize=10, family='sans-serif')
    setp(ylabels, fontsize=10, family='sans-serif')
    if args.ylim:
        ylim((-args.ylim,args.ylim))

    ax1.set_title(args.title)
    if args.sv:  sv='all'
    else:            sv=args.sv
    figtext(0.05, 0.01, "{0} {1} {2} SV:{3}".format(
        args.carrier, args.code, args.type, sv))
        
    if (args.saveFig == None):
        show()
    else:
       pylab.savefig(args.saveFig)
