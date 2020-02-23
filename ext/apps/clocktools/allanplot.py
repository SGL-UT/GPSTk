#!/usr/bin/env python
# $Id: allanplot 1210 2008-04-22 03:46:29Z ocibu $
# A routine to plot the output of the oallandev and ohadamarddev.
#
import sys, string, time, datetime, numpy, matplotlib, pylab

from optparse import OptionParser
usage="Usage: allanplot [OPTIONS] args \nGenerates loglog plot of allan deviations.\n \nargs can be a list of 0+ outputs of oallandev or ohadamarddev \nallanplot also acceps stdin"
parser = OptionParser(usage)
parser.add_option("-d", "--debug", help="Increase the debugLevel", \
                  default=0, dest="debugLevel", action="count")

parser.add_option("-t", "--title", help="Specify a title for the plot",\
                  dest="title", type="string", action="store")

parser.add_option("-l", "--legend", dest="legend", action="count",
                  help="Include a legend.")

parser.add_option("-s", "--save-figure", \
                  dest="saveFig", action="store", type="string",\
                  help="Save the figure to the indicated file")

parser.add_option("-c", "--cesium-specs", \
                  dest="cesiumSpecs", action="count", \
                  help="Plot stability specifications for Agilent 5071A")

(options, args) = parser.parse_args()


# A key handler for matplotlib
def press(event):
    if event.key=='q' or event.key==' ':
        pylab.close()

# Here we start generating the plots
fig = pylab.figure()
pylab.connect('key_press_event', press)
pylab.clf()
rExtent=0.8
if(options.legend):
    rExtent=0.69
ax1 = fig.add_axes([0.1, 0.1, rExtent, 0.8])
if (options.title == None):
    options.title ="Frequency Stability"
ax1.set_title(options.title)
ax1.set_xlabel("averaging time, tau (seconds)")
ax1.set_ylabel("Overlapping Allan Deviation, sigma")


color=("blue", "lime", "orange", "green","blue","yellow","black","purple","gray","green","lime","maroon","olive","silver", "teal")
label=color
offList=([],[])
marker=("+",".",",")
markerIndex=0
colorIndex=0

if(len(args)):
    for i in range (0,len(args),1):
        file = open(args[i])
        del offList
        offList=([],[])
        for line in file:
            line = line.strip()
            if len(line)==0: continue
            if line[0] == "#": continue
            words=line.split()
            offList[0].append(float(words[0]))
            offList[1].append(float(words[1]))
        ax1.loglog(offList[0],offList[1],marker=marker[markerIndex],color=color[colorIndex], label=file.name, linestyle='-', linewidth=4)
        markerIndex=(markerIndex+1)%3
        colorIndex=(colorIndex+1)%15
else:
    file=sys.stdin
    for line in file:
        line = line.strip()
        if len(line)==0: continue
        if line[0] == "#": continue
        words=line.split()
        offList[0].append(float(words[0]))
        offList[1].append(float(words[1]))
    ax1.loglog(offList[0],offList[1],marker=marker[markerIndex],color=color[colorIndex], label=file.name, linestyle='-', linewidth=4)
    markerIndex=markerIndex+1
    colorIndex=colorIndex+1

v=ax1.axis()
ax1.grid(True, linewidth=1)
ax1.xaxis.grid(True, 'minor', linewidth=0.5)
ax1.yaxis.grid(True, 'minor', linewidth=0.5)
if options.cesiumSpecs:
    ax1.loglog([.01, .1, 1, 10, 100, 1000, 10000, 100000, 432000],[.000000000075, .000000000012, .000000000005, .0000000000035, .00000000000085, .00000000000027, .000000000000085, .000000000000027, .00000000000001],marker=marker[markerIndex],color='red', label="Cesium Specs",markersize=4, linestyle='-', linewidth=4)
    ax1.axhline(.00000000000001,color='red', label="Flicker Floor", linewidth=4)

ax1.axis(v)

if options.legend:
    ax1.legend(numpoints=2, pad=0.1, labelsep = 0, handlelen=0.005,
                   handletextsep=0.01, axespad=0.0, loc=(1,0))
    #if you want to change the table so that the labels are not just the file names
    #then make changes to the commented section of code below
    #ax1.legend(('Sel Smo','Unsel Smo','Sel Rinex','Unsel Rinex','5071A Specs','5071A Flicker Floor'),numpoints=2, pad=0.1, labelsep = 0, handlelen=0.005, handletextsep=0.01, axespad=0.0, loc=(1,0))

if (options.saveFig == None):
    pylab.show()
else:
   pylab.savefig(options.saveFig)
