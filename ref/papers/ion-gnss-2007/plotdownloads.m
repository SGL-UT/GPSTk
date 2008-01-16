
[x]=load('sfdownloaddata.txt');

% downloads
dmax = 1000;
d = x(:,3)/dmax;

% bandwidth in Gb / month
gmax=10;
g = x(:,4)/gmax;


eopen('sfdownloads.eps')
eglobpar;

eXAxisSouthLabelText = 'Month';
eXAxisSouthScaleType = 1;
eYAxisWestLabelText='Downloads per month';
eYAxisEastLabelText='Gigabytes transfered per month';
eXAxisNorthValueVisible=0;
%eXAxisSouthVisible=0;
eXAxisSouthValueVisible=0;

%

[x1,y1]=ebar(d,.35,1,2);
[x2,y2]=ebar(g,.35,2,2);


eXAxisNorthVisible=0;
eYAxisEastVisible=0;
eXAxisSouthVisible=0;
eYAxisWestVisible=0;

eaxes([0 1 13],[0 .2 1]);

eplot(x1,y1,'Downloads/month',-1,[1 0 0])
eplot(x2,y2,'Gigabytes/month',-1,[0 1 0])
eplot

eXAxisNorthVisible=1;
eYAxisEastVisible=1;
eXAxisSouthVisible=1;
eYAxisWestVisible=1;
eXAxisSouthValueVisible=0;

eaxes([0 1 13],[0 100 dmax], [0 0 0], [0 2 gmax]);


eclose
eview
ebitmap