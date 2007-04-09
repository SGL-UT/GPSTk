/* vim: set cindent shiftwidth=4: */
#include <string>
#include <vector>
#include <map>

#include <limits.h>
#include <float.h>

#include "geometry.hpp"

#include "Matrix.hpp"
#include "MatrixOperators.hpp"

#include "RinexObsBase.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsData.hpp"
#include "RinexObsStream.hpp"

#include "RinexNavBase.hpp"
#include "RinexNavHeader.hpp"
#include "RinexNavData.hpp"
#include "RinexNavStream.hpp"

#include "SP3Base.hpp"
#include "SP3Header.hpp"
#include "SP3Data.hpp"
#include "SP3Stream.hpp"

#include "TropModel.hpp"
#include "WGS84Geoid.hpp"
#include "IonoModelStore.hpp"

#include "BCEphemerisStore.hpp"
#include "SP3EphemerisStore.hpp"
#include "EphemerisRange.hpp"
#include "icd_200_constants.hpp"


/**
 * @file vecsol.cpp 
 * Vector solution using dual-frequency carrier phases. Double difference
 * algorithm with proper weights, iteration with attempted ambiguity
 * resolution, crude outlier rejection, elevation sine weighting. 
 * Alternatively also code processing provided.
 * The configuration file is 'vecsol.conf'; broadcast or precise ephemeris
 * are in files 'vecsol.nav' and 'vecsol.eph', respectively.

 * LGPL (see COPYING). No furry animals were harmed in the coding 
 * of this software.

 * (c) 2005-2006 Martin Vermeer


GPS data in RINEX format, as well as precise orbits, can be downloaded
from the SOPAC site: http://sopac.ucsd.edu/cgi-bin/dbDataByDate.cgi

Site co-ordinates for testing can be had from:
http://sopac.ucsd.edu/sites/getSiteInfo.html


 To Do:
 - Use of ssi and lli bits?
 - Remove limitation that unknowns must be same across iterations
 - Ionosphere models: use IONEX
 - Satellite center-of-mass correction 
 - Receiver antenna phase delay patterns
 - Earth tides
 - Output of vectors to be read by other software
 
 */


using namespace std;
using namespace gpstk;
using gpstk::StringUtils::asString;
using gpstk::transpose;

    void
stationData(RinexObsData const & rod, bool const phase, 
	vector < SatID > &prnVec,
	vector < double >&rangeVecL1, vector < double >&rangeVecL2)
{
    RinexObsData::RinexSatMap::const_iterator it;
    for (it = rod.obs.begin(); it != rod.obs.end(); it++) {
	RinexObsData::RinexObsTypeMap otmap;
	RinexObsData::RinexObsTypeMap::const_iterator itL1, itL2;
	otmap = (*it).second;
	if (phase) {
	    itL1 = otmap.find(RinexObsHeader::L1);
	    itL2 = otmap.find(RinexObsHeader::L2);
	} else {
	    itL1 = otmap.find(RinexObsHeader::P1);
	    itL2 = otmap.find(RinexObsHeader::P2);
	}
	if (itL1 != otmap.end() && itL2 != otmap.end()) {
	    double /* const */ rangeL1 = (*itL1).second.data;
	    double /* const */ rangeL2 = (*itL2).second.data;

	    // here we would have Loss of Lock Indicator
	    // and Signal Strength Indicator: =0, >5 are OK
	    // cout << (*itL2).second.lli << endl;
	    // cout << (*itL2).second.ssi << endl;

	    // Discard empty data
	    if (rangeL1 != 0.0 && rangeL2 != 0.0) {
		prnVec.push_back((*it).first);
		rangeVecL1.push_back(rangeL1);
		rangeVecL2.push_back(rangeL2);
	    }
	}
    }
}


enum FixType {
    FIX_NONE,
    FIX_WIDELANE,
    FIX_BOTH
};


FixType phaseCycles(vector <double> & DDobs,
	double const lambda1, double const lambda2, double const wt1, 
	double const wt2)
{
#define WITHIN_HALF_CYCLE(Lx)  Lx = Lx - int(Lx); \
    (Lx < -0.5 ? Lx += 1.0 : (Lx > 0.5 ? Lx -= 1.0 : Lx = Lx))

    double L1 = DDobs[1] / lambda1; 
    double L2 = DDobs[2] / lambda2;
    double L5 = L1 - L2;

    WITHIN_HALF_CYCLE(L1);
    WITHIN_HALF_CYCLE(L2);
    WITHIN_HALF_CYCLE(L5);

    cout << fixed << setprecision(8) << L1 << " " << L2 << " | " << L5;

    bool const L1fixable = L1 <  0.1 && L1 >  -0.1;
    bool const L2fixable = L2 <  0.1 && L2 >  -0.1;
    bool const L5fixable = L5 < 0.07 && L5 > -0.07;
    double const L3 = wt1 * lambda1 * L1 + wt2 * lambda2 * L2;
    // Unit for L3: m
    bool const L3fixable = L3 < 0.02 && L3 > -0.02;
    
    if (L1fixable && L2fixable && L5fixable) {
	DDobs[1] -= L1 * lambda1;
	DDobs[2] -= L2 * lambda2;
	cout << " FIX" << endl;
	return FIX_BOTH;
    }

    if (L5fixable && L3fixable) {
	DDobs[1] -= L1 * lambda1;
	DDobs[2] -= L2 * lambda2;
	cout << " QIF" << endl;
	return FIX_BOTH;
    }

    if (L5fixable) {
	DDobs[1] -= 0.5 * L5 * lambda1;
	DDobs[2] += 0.5 * L5 * lambda2;
	cout << " Widelane" << endl;
	return FIX_WIDELANE;
    }

    cout << endl;
    return FIX_NONE;
}


enum SolveType {
    SOLVE_AMBS,
    SOLVE_COORDS
};


void Solve(gpstk::Matrix <double> N, gpstk::Matrix <double> const b, 
	gpstk::Matrix <double> & N2, gpstk::Matrix <double> & b2, 
	SolveType const solveType, 
	gpstk::Vector <FixType> const fixed, 
	int const MaxUnkn, int const unknowns, bool const tropo)
{
    if (solveType == SOLVE_COORDS) {
	// Fix "fixed" unknowns:
	for (int k = MaxUnkn; k < unknowns; k++) {
	    if (fixed[k] == FIX_BOTH)
		N(k,k) += 1.0E8; // absolutely fixed
	    // Note: we store separately the bias unknowns (ambiuguities) for L1 and
	    // L2. However, in the normal matrix we have them jointly. This limits
	    // what we can do here with weighting in case of a widelane fix.
	    // Obviously it would be better to have them separately in the normal 
	    // matrix also, but then the solution effort would be 8x as expensive
	    // numerically.
	    if (fixed[k] == FIX_WIDELANE)
		N(k,k) *= 1.25; // upgraded
	}
    } else { // SOLVE_AMBS
	int const endCoords = ( tropo ? MaxUnkn - 2 : MaxUnkn );
	for (int k = 0; k < endCoords; k++) {
	    N(k,k) += 1.0E8; // Keep only coords fixed (to earlier solved values)
	}
    }
    // Soft-constrain tropo params. This is tricky, as this has to be scaled
    // with the variance of unit weight for the GPS observations. Use
    // conservative values:
    if (tropo) {
	N(MaxUnkn - 1, MaxUnkn - 1) += 0.01;
	N(MaxUnkn - 2, MaxUnkn - 2) += 0.01;
    }

    // Copy over to correctly sized matrices
    gpstk::Matrix <double> N1(unknowns, unknowns);
    gpstk::Matrix <double> bb(unknowns, 3);
    for (int k = 0; k < unknowns; k++) {
	for (int m = 0; m < unknowns; m++)
	    N1(k,m) = N(k,m);
	// Suppress unused unknowns, if any
	if (N1(k,k) == 0.0)
	    N1(k,k) = 1.0;
	for (int ot = 0; ot < 3; ot++)
	    bb(k,ot) = b(k,ot);
    }

    N2 = gpstk::inverse(N1);
    double big, small;
    cout << setprecision(10) << "Condition number(" << solveType << "):" 
	<< condNum(N1, big, small) << endl;

    cout << "Largest, smallest eigenvalue:" << " [" << big << " > " 
	<< small << ']' << endl;
    // Is it my imagination, or does SVD sometimes fail to order the
    // eigenvalues properly?  -- MV 01.03.2006
    gpstk::SVD<double> svd;
    svd(N1);
    double big2 = 0;
    for (int k = 0; k < unknowns; k++)
	if (svd.S(k) > big2)
	   big2 = svd.S(k);
    if (big != big2) { 
	// List all the eigenvalues; this shouldn't happen
	for (int k = 0; k < unknowns; k++)
	    cout << k << ':' << svd.S(k) << ' ';
	cout << endl;
    }
    
    for (int k = 0; k < unknowns; k++) {
	if (N2(k,k) < 0.0)
	    cout << "Negative diagonal element " 
		<< k << ": " << N2(k,k) << endl;
    }
    b2 = bb;
}


bool Reduce(gpstk::Matrix <double> & N, gpstk::Matrix <double> & sol, 
	Matrix <double> x0,
	int const k, int const j, int const l, 
	int const dir1, int const dir2, int const dir3, 
	int const MaxUnkn, int const unknowns)
{
    // Here, the relationships ("closures") that may exist between different 
    // DD real-valued amvbiguities (biases) are used as condition equations 
    // to least-squares adjust (improve) them. 
    // The condition equation coefficient matrix is B, the condition
    // quantity ("zero variate") is y.
    gpstk::Matrix <double> B(1, unknowns);
    gpstk::Matrix <double> BT(unknowns, 1);
    gpstk::Matrix <double> y, BN, NBT, BNBT, InvBNBT;
    int m;

    // dir1/2/3 contain the "directions" of Double Diffs used, 
    // i.e., +1 or -1.
    for (m = 0; m < unknowns; m++) {
	if (m == k)
	    B(0, m) = dir1;
	else if (m == j)
	    B(0, m) = dir2;
	else if (m == l)
	    B(0, m) = dir3;
	else
	    B(0, m) = 0.0;
    }

    // Least squares condition eqs. adjustment machinery
    BT = transpose(B);
    BN = B * N;
    NBT = transpose(BN);
    BNBT = BN * BT;
    InvBNBT = BNBT; // Make sure size is always right
    if (BNBT(0, 0) == 0.0) {
	cout << "Warning: Zero Variance!" << endl;
	InvBNBT(0, 0) = 0.0;
    } else
	InvBNBT(0, 0) = 1.0 / BNBT(0, 0);

    // Create replacement of proper dimensions:
    gpstk::Matrix <double> x0b(unknowns, 3);
    for (int i = 0; i < unknowns; i++)
	for (int m = 0; m < 3; m++)
	x0b(i, m) = x0(i, m);

    y = B * (sol + x0b);

    if (abs(y(0, 0)) < 0.1) {
        sol = sol - NBT * InvBNBT * y;
        N = N - NBT * InvBNBT * BN;
    	cout << "DD ambigs.: " << k << ' ' << j << ' ' << l << endl;
    	cout << "Directions: " << dir1 << ' ' << dir2 << ' ' << dir3 << endl;
        cout << "Closures:   " << y << endl;
	cout << "(Previous): " << B * x0b << endl;
	return true;
    }
    return false;
}


Triple Rotate(Triple const UEN, Triple const R)
    {
	// Rotates an Up-East-North antenna offset to geocentric XYZ
	// (This should be part of the GPStk library)
	double const rad = R.mag();
	double const rho = sqrt(R[0] * R[0] + R[1] * R[1]);
	double const sf = R[2] / rad;
	double const cf = rho / rad;
	double const sl = R[1] / rho;
	double const cl = R[0] / rho;
	Triple a;
	a[0] = cl * cf * UEN[0] - sl * UEN[1] - cl * sf * UEN[2];
	a[1] = sl * cf * UEN[0] + cl * UEN[1] - sl * sf * UEN[2];
	a[2] =      sf * UEN[0]               +      cf * UEN[2];
	return a;
    }


Triple permanentTide(double const phi)
{
    // Based on McCarthy (1996). Permanent part of the solid Earth tide,
    // computed with conventional Love number, as has been the practice.
    // Note that this is only meant to reduce measured GPS co-ordinates in a
    // way that makes them comparable to ITRF published co-ordinates.
    Triple disp;
    double const c = cos(phi * DEG_TO_RAD);
    disp [0] = 0.6026 * 0.19844 * (1.5 * c*c - 1.0);
    disp [1] =  0.0;
    disp [2] = 0.0831 * 0.19844 * 1.5 * sin(2.0 * phi);
    return disp;
}


    int main(int argc, char *argv[])
    {
#define EPH_RANGE(C,T,X,S) ((precise ? \
	    C.ComputeAtReceiveTime(T.time,X,S,sp3store) : \
	    C.ComputeAtReceiveTime(T.time,X,S,bcestore)))

	BCEphemerisStore bcestore;
	SP3EphemerisStore sp3store;
	WGS84Geoid geoid;
	IonoModelStore ion;
	CorrectedEphemerisRange CER1, CER2;
	SimpleTropModel trop;

	if (argc != 3) {
	    cerr << "Usage:" << endl;
	    cerr << "   " << argv[0] <<
		" <RINEX Obs file 1> <RINEX Obs file 2>" <<
		endl;
	    cerr << "Edit vecsol.conf, vecsol.nav and vecsol.eph for input" <<
		endl;
	    exit(-1);
	}

	int obsMode;   // 0,2 = code, 1,3 = phase; 0,1 = iono free, 2,3 L1+L2
	bool phase;    // Process carrier phase data (instead of P code data)
	bool ionoFree; // Compute iono free (instead of L1 + L2)
	bool truecov;  // Use true DD covariances (inst. of no correlations)
	bool precise;  // Use precise ephemeris (inst. of broadcast)
	bool iono;     // Use nav file iono model
	bool tropo;    // Estimate troposphere parameters
	bool vecmode;  // Solve vector (inst. of end point coords)
	bool debug;
	double refsat_elev; // Minimum elevation of the reference satellite.
			    // Good value: 30.0
	double cutoff_elev; // cut-off elevation. Good value: 10.0
	int MaxUnkn;   // The number of std. unknowns. 3 for baseline est.,
			    // 6 for two endpoint positions, 8 for tropo est. too.
	// rejection criteria, m/s, m
	double PTDrej, CTDrej, DDrej = 1.0; 
	bool reduce;   // Reduce out dependencies between DD biases

	char s[80];
	std::ifstream conf;
	conf.open("vecsol.conf", ios::in);
	conf >> obsMode;	conf.getline(s, 80);
	phase = obsMode == 1 || obsMode == 3;
	ionoFree = obsMode == 0 || obsMode == 1;
	conf >> truecov;	conf.getline(s, 80);
	conf >> precise;	conf.getline(s, 80);
	conf >> iono;		conf.getline(s, 80);
	conf >> tropo;		conf.getline(s, 80);
	conf >> vecmode;	conf.getline(s, 80);
	conf >> debug;		conf.getline(s, 80);
	conf >> refsat_elev;	conf.getline(s, 80);
	conf >> cutoff_elev;	conf.getline(s, 80);
	conf >> PTDrej >> CTDrej; 
	conf.getline(s, 80);
	conf >> reduce;		conf.getline(s, 80);
	conf.close();

	if (vecmode)
	    MaxUnkn = 3;
	else
	    MaxUnkn = 6;
	if (tropo)
	    MaxUnkn += 2;

	cout << endl;
	cout << "Configuration data from vecsol.conf" << endl;
	cout << "-----------------------------------" << endl;
	cout << "Use carrier phases:             " << phase << endl;
	cout << "Compute ionosphere-free:        " << ionoFree << endl;
	cout << "Use true correlations:          " << truecov << endl;
	cout << "Use precise ephemeris:          " << precise << endl;
	cout << "Use broadcast iono model:       " << iono << endl;
	cout << "Use tropospheric est.:          " << tropo << endl;
	cout << "Vector mode:                    " << vecmode << endl;
	cout << "Debugging mode:                 "  << debug << endl;
	cout << "Ref sat elevation limit:        " << refsat_elev << endl;
	cout << "Cut-off elevation:              " << cutoff_elev << endl;
	cout << "TD rej. limits (phase, code):   " << PTDrej << " "
						   << CTDrej << endl;
	cout << "Reduce out DD dependencies:     " << reduce << endl;
	cout << endl;

	int const MaxDim(phase ? 1000 : MaxUnkn); // For reserving array space
	int const MaxSats(30);    // Same

	const double gamma((L1_FREQ / L2_FREQ) * (L1_FREQ / L2_FREQ));
	const double L1_F2(L1_FREQ * L1_FREQ), L2_F2(L2_FREQ * L2_FREQ);
	const double LDIF_F2(L1_F2 - L2_F2);
	// Weights for adding L1 and L2 pseudo-ranges into metric iono free
	const double wt1(L1_F2 / LDIF_F2);
	const double wt2(-L2_F2 / LDIF_F2);
	double lambda1, lambda2;
	if (phase) {
	    lambda1 = C_GPS_M / L1_FREQ;
	    lambda2 = C_GPS_M / L2_FREQ;
	} else {
	    lambda1 = 1.0;		// Already in metres
	    lambda2 = 1.0;
	}

	try {
	    if (!precise) {
		try {
		    // Read nav file(s) and store unique list of ephemerides
		    string filename;
		    std::ifstream nav;
		    nav.open("vecsol.nav", ios::in);
		    while (nav >> filename) {
			cout << "Nav file: " << filename;
			// Comment:
			if (filename[0] == '#') {
			    cout << " skipped" << endl;
			    nav.getline(s, 80);
			} else {
			    cout << endl;
			    RinexNavStream rnffs(filename.c_str());
			    rnffs.exceptions(ios::failbit);
			    RinexNavData rne;
			    RinexNavHeader hdr;

			    rnffs >> hdr;
			    if (iono)
				ion.addIonoModel(DayTime::BEGINNING_OF_TIME, 
					IonoModel(hdr.ionAlpha, hdr.ionBeta));
			    while (rnffs >> rne)
				bcestore.addEphemeris(rne);
			}
		    }
		    bcestore.SearchNear();
		}
		catch(...) {
		    cerr << "Something wrong with nav files." << endl << endl;
		    exit(-1);
		}
	    } else {
		try{
		    // Precise ephemerides:
		    string filename;
		    std::ifstream eph;
		    eph.open("vecsol.eph", ios::in);
		    while (eph >> filename) {
			cout << "Eph file: " << filename;
			// Comment:
			if (filename[0] == '#') {
			    cout << " skipped" << endl;
			    eph.getline(s, 80);
			} else {
			    cout << endl;
			    sp3store.loadFile(filename.c_str());
			}
		    }
		sp3store.dump(1, cout);
		}
		catch(...) {
		    cerr << "Something wrong with SP3 files." << endl << endl;
		    exit(-1);
		}
	    }
	    
	    gpstk::Matrix <double> x0(MaxDim, 3, 0.0);
	    gpstk::Vector <FixType> fixed(MaxDim, FIX_NONE);
	    gpstk::Vector <SatID> FromSat(MaxDim), ToSat(MaxDim);
	    
	    // Get station positions from RINEX headers:
	    RinexObsHeader roh1, roh2;

	    try {
		RinexObsStream roffs1(argv[1]);
		RinexObsStream roffs2(argv[2]);
		roffs1.exceptions(ios::failbit);
		roffs2.exceptions(ios::failbit);

		roffs1 >> roh1;
		roffs2 >> roh2;
	    }
	    catch(...) {
		cerr << "Something wrong with obs files." << endl << endl;
		exit(-1);
	    }
            // Here we should input pre-given coordinates (if any) that
	    //  will override those from the RINEX header
	    bool coords = true;
	    string const name1(roh1.markerName);
	    string const name2(roh2.markerName);
	    Position XYZ1, XYZ2;
	    std::ifstream coord1, coord2;
	    try {
		coord1.exceptions(ios::failbit);
		coord2.exceptions(ios::failbit);
		coord1.open((name1 + ".crd").c_str(), ios::in);
		coord2.open((name2 + ".crd").c_str(), ios::in);
	    }
	    catch(...) {
		cout << endl;
		cout << "Did not find / cannot open coordinate files." << endl << endl;
		coords = false;
	    }
	    if (coords) {
		// Here the given coordinates (of BENCHMARK!) are used if available:
		coord1 >> XYZ1[0] >> XYZ1[1] >> XYZ1[2];
		coord2 >> XYZ2[0] >> XYZ2[1] >> XYZ2[2];
		coord1.close();
		coord2.close();
		roh1.antennaPosition = XYZ1;
		roh2.antennaPosition = XYZ2;
		cout << "Positions       : " << setprecision(12) << Triple(XYZ1) << endl;
		cout << "From files      : " << setprecision(12) << Triple(XYZ2) << endl << endl;
	    }
	    Triple AO1 = Rotate(roh1.antennaOffset, roh1.antennaPosition);
	    Triple AO2 = Rotate(roh2.antennaOffset, roh2.antennaPosition);

	    cout << "Geocentric      : " << AO1 << endl 
		 << "antenna offsets : " << AO2 << endl << endl;

	    // Receiver provided offset to be re-subtracted 
	    bool apply_clockOffset1(roh1.receiverOffsetValid &&
				    roh1.receiverOffset);
	    bool apply_clockOffset2(roh2.receiverOffsetValid &&
				    roh2.receiverOffset);
	    // However, sometimes the header record is missing and yet the
	    // data contains valid offsets (Huh? Ask Werner).
	    // (WinPrism's Ashtech rinexer seems to need this, and
	    // doesn't seem to harm otherwise)
	    apply_clockOffset1 = true;
	    apply_clockOffset2 = true;

#if 0
	    // Low hanging fruit
	    Triple PT1 =
		permanentTide(Position(roh1.antennaPosition).geodeticLatitude());
	    Triple PT2 =
		permanentTide(Position(roh2.antennaPosition).geodeticLatitude());
	    cout << "Tides:" << PT1 << " " << PT2 << endl;
#else
	    Triple PT1, PT2;	    
#endif
	    // t1, t2 represent now antenna (ARP) positions.
	    // (roh1/2.antennaPosition is named wrong, it is benchmark pos!)
	    Position t1(roh1.antennaPosition + AO1, Position::Cartesian);
	    Position t2(roh2.antennaPosition + AO2, Position::Cartesian);

	    cout << "Data interval: " << roh1.interval << "," <<
		roh2.interval << endl;
	    cout << "Generated by:  " << 
		roh1.fileProgram << ", " << roh2.fileProgram << endl;

	    // How was this RINEX generated?
	    bool javad1 = roh1.fileProgram.find("Pinnacle") != string::npos;
	    bool javad2 = roh2.fileProgram.find("Pinnacle") != string::npos;
	    // Note: we compute the reduction by comparing L1 with the
	    // computed range. This is only valid if L1, L2 were 'gauged' to
	    // pseudoranges, which is the case for Pinnacle RINEX. If not,
	    // we would have to use one of the code observables instead. See
	    // code further below.
	    if (javad1 || javad2) {
		cout << "RINEX file was not reduced for clock offset." << endl;
		cout << "We do the reduction ourselves." << endl << endl;
	    }
	    
	    Position const t10(t1);  // To keep unknowns invariant
	    Position const t20(t2);
	    Position Pos1, Pos2;

	    double crit(1.0);
	    double const limit = (phase ? 0.0001 : 0.001);
	    for (int l = 0; (crit > limit) && l < 25; l++) {
		// Iteration loop. Important! The unknowns are expected to
		// remain _identical_ across iterations.
		cout << "Iteration: " << l << endl;
		
		// Map pointing from PRN to obs. eq. element position
		map <SatID, int> CommonSatsPrev;

		SatID OldRefSat;

		map <SatID, vector<double> > DDobsPrev;
		map <SatID, double> SecsPrev;
		
		// Open and read the observation files one epoch at a time.
		// Compute a contribution to normal matrix and right hand
		// side
		RinexObsStream roffs1(argv[1]);
		RinexObsStream roffs2(argv[2]);
		roffs1.exceptions(ios::failbit);
		roffs2.exceptions(ios::failbit);

		RinexObsHeader dummy1, dummy2;
		RinexObsData rod1, rod2;

		// Only skip over header this time:
		roffs1 >> dummy1;
		roffs2 >> dummy2;

		// Improve antenna positions from previous iteration:
		if (l > 0) {
		    t1 = t1 + 0.5 * Pos1;
		    if (!vecmode)
			t2 = t2 + 0.5 * Pos2;
		}
		Geodetic g1(t1, &geoid);
		Geodetic g2(t2, &geoid);

		// Output bench mark (not: antenna) positions: (Published
		// GPS positions are always reduced for solid Earth tides)
		cout << name1 << ": " << Position(Triple(t1) - AO1 - PT1) << endl 
		     << name2 << ": " << Position(Triple(t2) - AO2 - PT2) << endl << endl;
		// Print also geographic coords:
		Position t1g(Triple(t1) - AO1 - PT1);
		Position t2g(Triple(t2) - AO2 - PT2);
		cout << name1 << ": " << t1g.asGeodetic() << endl 
		     << name2 << ": " << t2g.asGeodetic() << endl;

		// Update these for output at program end:
		XYZ1 = t1g.asECEF();
		XYZ2 = t2g.asECEF();

		gpstk::Matrix <double> N(MaxDim, MaxDim, 0.0);
		gpstk::Matrix <double> b(MaxDim, 3, 0.0);
		int observations(0), rejections(0), rej_DD(0);
		double TD_RMS(0), DD_RMS(0), Iono_RMS(0);
		// points to _after_ the last unknown
		int unknowns = MaxUnkn;
		
		while (roffs1 >> rod1 && roffs2 >> rod2) { // Epoch loop
		    // Make sure we have a common epoch:
		    while (rod1.time > rod2.time + 0.1 && roffs2 >> rod2) { }
		    while (rod1.time + 0.1 < rod2.time && roffs1 >> rod1) { }
		    double sync_err = rod2.time.secOfDay() - rod1.time.secOfDay();
		    if (abs(sync_err) > 0.001) {
			cout << "Synchronization Error: " << 
			    std::setprecision(6) << sync_err << " sec" << endl;
		    }
		    double Secs = rod1.time.secOfDay();

		    // Experimental for WinPrism's RINEX
		    if (apply_clockOffset1)
			rod1.time -= rod1.clockOffset;
		    if (apply_clockOffset2)
			rod2.time -= rod2.clockOffset;

		    if (rod1.epochFlag < 2 && rod2.epochFlag < 2)
			// Observations are good
		    {
			map <SatID, int> CommonSats;
			CommonSats.clear();
			vector <SatID> prnVec_1, prnVec_2;
			vector <double> rangeVecL1_1, rangeVecL2_1;
			vector <double> rangeVecL1_2, rangeVecL2_2;

			// First station
			stationData(rod1, phase, prnVec_1, rangeVecL1_1,
				    rangeVecL2_1);
			// Second station
			stationData(rod2, phase, prnVec_2, rangeVecL1_2,
				    rangeVecL2_2);

			/// Process station pairs

			// for construction of double diffs
			SatID RefSat;
			bool hasRefSat(false);

			double ref_rdiffL1, ref_rdiffL2;
			vector <double> ref_A(MaxUnkn);
			gpstk::Matrix <double> A(MaxDim, MaxSats, 0.0);
			gpstk::Matrix <double> Obs(MaxSats, 3, 0.0);
			// For var-cov modelling
			vector <double> Q(MaxSats), 
			       Elev10(MaxSats), Elev20(MaxSats);
			double Qref;
			int nObs(0);

			// Find out the highest satellite:
			double best(0.0);
			int bestIdx(0);
			bool stickWithOld = false;
			for (int i = 0; i != prnVec_2.size(); i++)
			    if (prnVec_2[i].id > 0) {

				// Invariant over iterations! Uses t10, t20
				double dummy = EPH_RANGE(CER2, rod2, t10,
					prnVec_2[i]);
				double const riseVel1 = CER2.svPosVel.v.dot(t10);
				//double const riseVel1 =
				//	-CER2.svPosVel.v.dot(CER2.svPosVel.x);
				Elev10[i] = CER2.elevation;
				bool const elev1OK = CER2.elevation > refsat_elev;

				dummy = EPH_RANGE(CER2, rod2, t20,
					prnVec_2[i]);
				double const riseVel2 = CER2.svPosVel.v.dot(t20);
				//double const riseVel2 =
				//	-CER2.svPosVel.v.dot(CER2.svPosVel.x);
				Elev20[i] = CER2.elevation;
				bool const elev2OK = CER2.elevation > refsat_elev;

				double const riseVel = 0.5 * (riseVel1 + riseVel2);
				
				if (elev1OK && elev2OK 
					&& riseVel > best
					&& !stickWithOld) {
				    best = riseVel;
				    bestIdx = i;
				}
				// Hang on to same ref sat if still high enough 
				if (OldRefSat == prnVec_2[i] 
				    && CER2.elevation > refsat_elev) {
				    bestIdx = i;
				    stickWithOld = true;
				}
			    }

			for (int ii = 0; ii != prnVec_2.size(); ii++) {
			    // Reshuffle... 
			    int i = (ii + bestIdx) % prnVec_2.size();
			    if (prnVec_2[i].id > 0 
				    && Elev10[i] > cutoff_elev
				    && Elev20[i] > cutoff_elev) {
				double r2 = EPH_RANGE(CER2, rod2, t2,
					prnVec_2[i]);
				double trop2 =
				    trop.correction(t2, CER2.svPosVel.x, rod2.time);
				r2 += trop2;
				
				for (int j = 0; j != prnVec_1.size(); j++) {
				    if (prnVec_1[j].id > 0
					&& prnVec_1[j].id == prnVec_2[i].id)
				    {
					// This sat is visible from both
					// stations
					double r1 = EPH_RANGE(CER1, rod1, t1,
					prnVec_1[j]);
					double trop1 = trop.correction(t1,
						    CER1.svPosVel.x, rod1.time);
					r1 += trop1;
					
					// Between-station diffs to each satellite
					double diffL1 = rangeVecL1_1[j] - rangeVecL1_2[i];
					double diffL2 = rangeVecL2_1[j] - rangeVecL2_2[i];
					// Subtract out approx values; this
					// value now roughly reflects the
					// inter-station bias difference and
					// is thus nearly the same for all
					// sats
					double rdiffL1 = lambda1 * diffL1 - (r1 - r2);
					double rdiffL2 = lambda2 * diffL2 - (r1 - r2);

					// The clock offset problem for
					// Javad / Pinnacle:
					double rr1, rr2;
					// Range rates:
					if (javad1) 
					    rr1 = CER1.svPosVel.v.dot(CER1.cosines);
					if (javad2) 
					    rr2 = CER2.svPosVel.v.dot(CER2.cosines);
					// Clock corrections:
					double cc1 = lambda1 * rangeVecL1_1[j] - r1;
					double cc2 = lambda1 * rangeVecL1_2[i] - r2;
					rdiffL1 -= (rr1 * cc1 - rr2 * cc2) / C_GPS_M;
					rdiffL2 -= (rr1 * cc1 - rr2 * cc2) / C_GPS_M;

					if (iono) {
					    // Ionospheric corrections:
					    double const 
						ionoL1_1 = ion.getCorrection(
						    rod1.time, g1, CER1.elevation,
						    CER1.azimuth, IonoModel::L1);
					    double const
						ionoL2_1 = ion.getCorrection(
						    rod1.time, g1, CER1.elevation,
						    CER1.azimuth, IonoModel::L2);
					    double const
						ionoL1_2 = ion.getCorrection(
						    rod2.time, g2, CER2.elevation,
						    CER2.azimuth, IonoModel::L1);
					    double const
						ionoL2_2 = ion.getCorrection(
						    rod2.time, g2, CER2.elevation,
						    CER2.azimuth, IonoModel::L2);
					    // Apply them:
					    if (phase) {
						rdiffL1 += ionoL1_1 - ionoL1_2;
						rdiffL2 += ionoL2_1 - ionoL2_2;
					    } else {
						rdiffL1 -= ionoL1_1 - ionoL1_2;
						rdiffL2 -= ionoL2_1 - ionoL2_2;
					    }
					}

					vector <double> A_(MaxDim, 0.0);
					// Store obs. coefficients for
					// coordinates
					for (int k = 0; k < 3; k++)
					    A_[k] = CER1.cosines[k];
					if (!vecmode) {
					    for (int k = 3; k < 6; k++)
						A_[k] = -CER2.cosines[k - 3];
					}
					// Tropo estimation
					if (tropo) {
					    A_[MaxUnkn - 2] = trop1;
					    A_[MaxUnkn - 1] = -trop2;
					}

					// Weight coefficient of this obs:
					double const q_ = 1.0 / 
					    sin(DEG_TO_RAD * CER1.elevation) + 1.0 /
					    sin(DEG_TO_RAD * CER2.elevation);
					// Build obs. coefs for satellite
					// ambiguities
					if (!hasRefSat) {
					    RefSat = prnVec_1[j];
					    hasRefSat = true;

					    // Ref sat change; invalidates
					    // unknowns
					    if (RefSat != OldRefSat) {
						CommonSatsPrev.clear();
						cout << "New ref sat:" <<
						    RefSat << endl;
					    }

					    ref_rdiffL1 = rdiffL1;
					    ref_rdiffL2 = rdiffL2;
					    Qref = q_;
					    for (int k = 0; k < MaxUnkn; k++)
						ref_A[k] = A_[k];
					} else {
					    // Construct inter-sat diffs
					    bool reject(false);
					    FixType fix(FIX_NONE);
					    vector <double> DDobs(3);
					    DDobs[1] = rdiffL1 - ref_rdiffL1;
					    DDobs[2] = rdiffL2 - ref_rdiffL2;
					    
					    // iono free observable, unit metres
					    DDobs[0] =
						wt1 * DDobs[1] + wt2 * DDobs[2];
					    for (int k = 0; k < MaxUnkn; k++)
						A_[k] -= ref_A[k];

					    SatID ThisSat = prnVec_1[j];
					    if (CommonSatsPrev.find(ThisSat) ==
						CommonSatsPrev.end()) {
						// New satellite
						CommonSats[ThisSat] = unknowns;
						if (phase) {
						    FromSat[unknowns] = RefSat;
						    ToSat [unknowns] = ThisSat;
						    cout << endl << "New unknown " 
							<< unknowns << " == " 
							<< asString(FromSat[unknowns])
							<< " -> "
							<< asString(ToSat[unknowns])
							<< endl;
						    
						    // create new DD ambiguity unknown
						    A_[unknowns] = 1.0;
						    // Initial approx. ambiguities
						    if (l == 0)
							for (int k = 0; k < 3; k++)
							    x0(unknowns,k) = 
								DDobs[k];

						    for (int k = 0; k < 3; k++)
							DDobs[k] -= 
							    x0(unknowns,k);
						    unknowns++;
						} else {
						    // Rough test double diffs
						    reject =
							(std::abs(DDobs[0]) > DDrej);
						}
						// Initialize previous obs for
						// triple diff comp
						DDobsPrev[ThisSat] = DDobs;
						SecsPrev[ThisSat] = Secs;
						reject = false;
					    } else {

						// Already known sat, copy forward
						CommonSats[ThisSat] =
						    CommonSatsPrev[ThisSat];
						if (phase) {
						    A_[CommonSats[ThisSat]] = 1.0;
						    for (int k = 0; k < 3; k++)
							DDobs[k] -= 
							    x0(CommonSats[ThisSat],k);
						}

						// Triple difference testing

						double timebase 
						    = Secs - SecsPrev[ThisSat];
						timebase = 
						    (timebase > 10 * roh1.interval ?
							0.000001 : roh1.interval);
						double res 
						    = (DDobs[0] -
							    DDobsPrev[ThisSat][0]) / timebase;
						// Cycle slips will show up here
						// over 30 s as 0.0226 and
						// 0.0131, respectively.
						reject =
						    (std::abs(res) *
						     sqrt(2.0 / (Qref + q_)) >
						    (phase ? PTDrej : CTDrej));

						// Include rough DD test
						// too: this occurs esp.
						// with newly appearing sats
						if (!reject && 
						    std::abs(DDobs[0]) > DDrej) {
						    if (debug)
							cout << "DD rej:" 
							     << DDobs[0];
						    rej_DD++;
						    reject = true;
						}

						if (!reject) {
						    // Accumulate statistics
						    TD_RMS += res * res;
						    DD_RMS +=
							DDobs[0] * DDobs[0];
						    double Iono = 
							(DDobs[2] - DDobs[1]) 
							/ wt1;
						    Iono_RMS += Iono * Iono;
							
						}
						else {
						    rejections++;
						}
						if (debug) {
						    cout << ThisSat << ":";
						    if (reject)
							cout << "REJ [" <<
							    Elev10[i] << ":" <<
							    Elev20[i] << "] ";
						    cout << setprecision(4) << res << " "; 
						}
						observations++;
					    }

					    if (!reject) {
						// Update "last good" obs
						DDobsPrev[ThisSat] = DDobs;
						SecsPrev[ThisSat] = Secs;

						// Collect per-obs quantities
						// into per-epoch tables
						for (int k = 0; k < unknowns; k++)
						    A(k, nObs) = A_[k];
						for (int k = 0; k < 3; k++)
						    Obs(nObs, k) = DDobs[k];
						Q[nObs] = q_;
						nObs++;
					    }

					}	// end other-than-ref sats processing

				    } // end sats visible from both stations
				}
			    }
			}		// All obs for this epoch processed

			if (nObs > 0) {
			    gpstk::Matrix <double> Qmat(nObs, nObs, 0.0);
			    for (int io = 0; io < nObs; io++) {
				if (truecov)
				    for (int jo = 0; jo < nObs; jo++)
					Qmat(io, jo) = Qref;
				Qmat(io, io) += Q[io];
			    }
			    Qmat = gpstk::inverse(Qmat);

			    // Right hand side vector
			    for (int k = 0; k < unknowns; k++)
				for (int io = 0; io < nObs; io++) {
				    for (int jo = 0; jo < nObs; jo++)
					for (int ot = 0; ot < 3; ot++)
					    b(k, ot) += A(k, io) * Obs(jo, ot)
						* Qmat(io, jo);

				}

			    // Normal eqs fill-in with full weight matrix per-epoch
			    for (int k = 0; k < unknowns; k++) {
				for (int io = 0; io < nObs; io++) {
				    if (A(k, io) != 0.0) { // Optimize
					for (int m = 0; m < unknowns; m++) {
					    for (int jo = 0; jo < nObs; jo++)
						N(k, m) += A(k, io) * A(m, jo) *
						    Qmat(io, jo);
					}
				    }
				}
			    }
			} // Normals building

			cout << endl;
			cout << "epoch " << rod1.time;
			cout << " unkn " << unknowns;
			cout << " obs  " << nObs;
			cout << " sats " << CommonSats.size() << endl;

			CommonSatsPrev = CommonSats;
			OldRefSat = RefSat;

		    }		// End usable data

		}			// End loop through each epoch

		cout << endl;
		cout << "Total unknowns:           " << unknowns << endl;
		cout << "Observations:             " << observations << endl;
		cout << "Rejected:                 " << rejections;
		cout << " or " << 100.0 * rejections / observations << "%" << endl;
		cout << " of which DD-based:       " << rej_DD;
		cout << "  or " << 100.0 * rej_DD / observations << "%" << endl;
		cout << "Triple-diff RMS [m/s]:    " << sqrt(TD_RMS /
							 (observations -
							  rejections)) << endl;
		float const DDrms = sqrt(DD_RMS / (observations - rejections));
		cout << "Double-diff RMS [m]:      " << DDrej << endl;
		// Three-sigma criterion, generous:
		DDrej = 2.0 * 3.0 * DDrms;
		cout << "Iono RMS on L1 [m]:       " << sqrt(Iono_RMS /
							 (observations -
							  rejections)) <<
								endl << endl;

		if (debug) {
		    cout << "Start of b vector:" << setprecision(10) << endl;
		    cout << "b0: " << b(0,0) << " " << b(1,0) << " " << b(2,0)
			<< endl;
		    cout << "b1: " << b(0,1) << " " << b(1,1) << " " << b(2,1)
			<< endl;
		    cout << "b2: " << b(0,2) << " " << b(1,2) << " " << b(2,2)
			<< endl << endl;
		}

		// Here we solve the normal equations and print solution
		gpstk::Matrix <double> NN(unknowns, unknowns);
		gpstk::Matrix <double> bb(unknowns, 3);
		Solve(N, b, NN, bb, SOLVE_COORDS, fixed, MaxUnkn, unknowns,
			tropo);

		cout << "Weight coefficient matrix:" << endl;
		for (int i = 0; i < MaxUnkn; i++) {
		    for (int j = 0; j < MaxUnkn; j++) {
			cout << setw(10) << NN(i, j) << " ";
		    }
		    cout << endl;
		}
		cout << endl;

		// Solution:
		gpstk::Matrix <double> sol = NN * bb;

		cout << "Solution (correction to inter-station vector):" <<
		    setprecision(5) << endl;
		Triple PosCorr0 = Triple(sol(0,0), sol(1,0), sol(2,0));
		Triple PosCorr1 = Triple(sol(0,1), sol(1,1), sol(2,1));
		Triple PosCorr2 = Triple(sol(0,2), sol(1,2), sol(2,2));
		if (ionoFree)
		    Pos1 = PosCorr0;
		else
		    Pos1 = 0.5 * (PosCorr1 + PosCorr2);
		if (!vecmode) {
		    PosCorr0 = PosCorr0 - Triple(sol(3,0), sol(4,0), sol(5,0));
		    PosCorr1 = PosCorr1 - Triple(sol(3,1), sol(4,1), sol(5,1));
		    PosCorr2 = PosCorr2 - Triple(sol(3,2), sol(4,2), sol(5,2));
		    if (ionoFree)
			Pos2 = Pos1 - Position(PosCorr0);
		    else
			Pos2 = Pos1 + 0.5 * Position(PosCorr1 + PosCorr2);
		}
		cout << "Iono free: " << PosCorr0 << endl;
		cout << "Freq. 1:   " << PosCorr1 << endl;
		cout << "Freq. 2:   " << PosCorr2 << endl;
		cout << endl;

		// Manhattan distance for iteration stop (is there a
		// std method for this?):
		if (ionoFree)
		    crit = std::abs(PosCorr0[0]) + std::abs(PosCorr0[1]) 
			 + std::abs(PosCorr0[2]);
		else
		    crit = 0.5 * 
			  (std::abs(PosCorr1[0]) + std::abs(PosCorr1[1]) 
			 + std::abs(PosCorr1[2]) + std::abs(PosCorr2[0]) 
			 + std::abs(PosCorr2[1]) + std::abs(PosCorr2[2]));

		cout << "Standard deviations (unscaled):" << endl;
		for (int k = 0; k < MaxUnkn; k++)
		    cout << sqrt(NN(k, k)) << " ";
		cout << endl;

		// Again: published vectors must be conventionally 
		// reduced for tide.
		// And published vector must be inter-benchmark:
		Position vec = Position(Triple(t1) - AO1 - PT1) 
		    	     - Position(Triple(t2) - AO2 - PT2);
		cout << "A priori vector:" << endl << vec << endl;
		cout << "A posteriori vector:" << endl 
		     << Position(Triple(vec) + PosCorr0)
		     << " (Iono free)" << endl;
		Triple PosCorrMean(PosCorr1 + PosCorr2);
		PosCorrMean = 0.5 * PosCorrMean;
		cout << Position(Triple(vec) + PosCorrMean) 
		     << " (L1 + L2)" << endl;
		cout << endl;

		if (tropo) {
		    cout <<
			"Tropospheric corr. parameters (fraction of full effect):"
			<< endl;

		    cout << sol(MaxUnkn - 2, 0) << " " << sol(MaxUnkn - 1, 0) << endl;
		    cout << "Standard deviations (unscaled):" << endl;
		    cout << sqrt(NN(MaxUnkn - 2, MaxUnkn - 2)) << " "
			 << sqrt(NN(MaxUnkn - 1, MaxUnkn - 1)) << endl <<
			endl;
		}

		// Ambiguity fixing on the unknowns, keeping coords as known:
		if (phase) {
		    Solve(N, b, NN, bb, SOLVE_AMBS, fixed, MaxUnkn,
			    unknowns, tropo);
		    sol = NN * bb;

		    // Here we use the relationships between DD ambiguities, 
		    // e.g. (G18-G6) - (G26-G6) - (G26-G18) = 0
		    // for a condition equation adjustment on NN, bb
		    int k, j, l, dir1, dir2, dir3;
		    int closures = 0;
		    SatID Free1, Free2;
		    for (k = MaxUnkn; k < unknowns; k++)
			for (j = k + 1; j < unknowns; j++) {
			    dir1 = 0;
			    if (FromSat[k] == FromSat[j]) {
				dir1 = 1; 
				dir2 = -1;
				Free1 = ToSat[k];
				Free2 = ToSat[j];
			    }
			    if (ToSat[k] == ToSat[j]) {
				dir1 = -1; 
				dir2 = 1;
				Free1 = FromSat[k];
				Free2 = FromSat[j];
			    }
			    if (FromSat[k] == ToSat[j]) {
				dir1 = 1; 
				dir2 = 1;
				Free1 = ToSat[k];
				Free2 = FromSat[j];
			    }
			    if (ToSat[k] == FromSat[j]) {
				dir1 = -1; 
				dir2 = -1;
				Free1 = FromSat[k];
				Free2 = ToSat[j];
			    }

			    if (dir1 != 0)
				for (l = j + 1; l < unknowns; l++) {
			    	    dir3 = 0;
				    if (Free1 == FromSat[l] && Free2 == ToSat[l])
					dir3 = 1;
				    if (Free1 == ToSat[l] && Free2 == FromSat[l])
					dir3 = -1;
				    if (reduce && dir3 != 0) {
					if (Reduce(NN, sol, x0, k, j, l, 
						dir1, dir2, dir3, 
						MaxUnkn, unknowns))
					    closures++;
				    }
				}
			}

		    cout << endl << "No. of closures: " << closures << endl << endl;
		    int fixedunknowns = 0;
		    int widelanes = 0;
		    cout << "DD bias fixes (fractional cycles):" << endl;
		    for (int k = MaxUnkn; k < unknowns; k++) {
			cout << "[" << setprecision(8) << sqrt(NN(k,k)) << "] ";
			cout << k << " (";
			cout << asString(FromSat[k]) << " -> " 
			     << asString(ToSat[k]) << "): ";
			vector <double> x0vec(3);
			x0vec[1] = x0(k, 1) + sol(k, 1);
			x0vec[2] = x0(k, 2) + sol(k, 2);
			FixType f = phaseCycles(x0vec, lambda1, lambda2, wt1, wt2);
			// remember to correct iono-free too
			x0(k, 0) = wt1 * x0vec[1] + wt2 * x0vec[2];
			x0(k, 1) = x0vec[1];
			x0(k, 2) = x0vec[2];
			if (f == FIX_BOTH)
			    fixedunknowns++;
			if (f == FIX_WIDELANE) 
			    widelanes++;
			fixed[k] = f;
		    }
		    cout << endl;
		    cout << "Fixed:      " << fixedunknowns << "  "
			 << 100.0 * fixedunknowns / (unknowns - MaxUnkn) << "%" << endl;
		    cout << "Widelanes:  " << widelanes << "  "
			 << 100.0 * widelanes / (unknowns - MaxUnkn) << "%" << endl;
		    cout << endl;
		}

	    } // iteration loop end

	    cout << "Writing coordinate(s) to file(s)..." << endl;
	    std::ofstream coord1o, coord2o;
	    try {
		coord1o.exceptions(ios::failbit);
		coord1o.open((name1 + ".crd").c_str(), ios::out | ios::trunc);
		coord1o << setprecision(12) << 
		    XYZ1[0] << ' ' << XYZ1[1] << ' ' << XYZ1[2] << endl;
		coord1o.close();
		if (!vecmode || !coords) {
		    coord2o.exceptions(ios::failbit);
		    coord2o.open((name2 + ".crd").c_str(), ios::out | ios::trunc);
		    coord2o << setprecision(12) << 
			XYZ2[0] << ' ' << XYZ2[1] << ' ' << XYZ2[2] << endl;
		    coord2o.close();
		}
	    }
	    catch(...) {
		cerr << "Exception writing coordinate file(s)" << endl;
	    }
	        
	    cout << "Finished." << endl;
    	}
        catch(Exception & e) {
    	    cerr << e << endl;
	}
	catch(...) {
	    cerr << "Caught an unexpected exception." << endl;
	}

	exit(0);
 
    }


