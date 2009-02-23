#pragma ident "$Id$"

#define xypoint pair<double, double>
#define xylist vector<xypoint>
#define serieslistelement pair<string,xylist*>
// serieslist = list< pair< string, vector< pair<double,double> >* > >
#define serieslist list< serieslistelement >
