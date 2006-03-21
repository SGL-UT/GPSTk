#include <string>
#include <vector>
#include <map>

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

 * (c) 2005 Martin Vermeer

 To Do:
 - Use of ssi and lli bits?
 - Remove limitation that unknowns must be same across iterations
 - Ionosphere models: use IONEX and QIF method
 - Satellite center-of-mass correction 
 - Receiver antenna phase delay patterns
 - Earth tides
 - Output of vectors to be read by other software
 
 */


using namespace std;
using namespace gpstk;


    void
stationData(RinexObsData const & rod, bool const phase, 
	vector < RinexPrn > &prnVec,
	vector < double >&rangeVecL1, vector < double >&rangeVecL2)
{
    RinexObsData::RinexPrnMap::const_iterator it;
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
	    double const rangeL1 = (*itL1).second.data;
	    double const rangeL2 = (*itL2).second.data;
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
	double const lambda1, double const lambda2)
{
#define WITHIN_HALF_CYCLE(Lx) \
    ((Lx < -0.5 ? Lx += 1.0 : (Lx > 0.5 ? Lx -= 1.0 : Lx = Lx)))

    double L1 = DDobs[1] / lambda1; 
    double L2 = DDobs[2] / lambda2;
    double L5 = L1 - L2;
    L1 = L1 - int(L1);
    L2 = L2 - int(L2);
    L5 = L5 - int(L5);

    WITHIN_HALF_CYCLE(L1);
    WITHIN_HALF_CYCLE(L2);
    WITHIN_HALF_CYCLE(L5);

    cout << setw(5) << L1 << " " << L2 << " | " << L5;

    bool const L1fixable = L1 <  0.2 && L1 >  -0.2;
    bool const L2fixable = L2 <  0.2 && L2 >  -0.2;
    bool L5fixable = L5 < 0.07 && L5 > -0.07;

    if (L1fixable && L2fixable && L5fixable) {
	DDobs[1] -= L1 * lambda1;
	DDobs[2] -= L2 * lambda2;
	cout << " FIX" << endl;
	return FIX_BOTH;
    }
 
    L5fixable = L5 < 0.1 && L5 > -0.1;
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
	int const MaxUnkn, int const unknowns)
{
    if (solveType == SOLVE_COORDS) {
	// Fix "fixed" unknowns:
	for (int k = 0; k < unknowns; k++) {
	    if (fixed[k] == FIX_BOTH)
		N(k,k) += 1.0E8; // absolutely fixed
	    if (fixed[k] == FIX_WIDELANE)
		N(k,k) *= 1.25; // upgraded
	}
    } else { // SOLVE_AMBS
	for (int k=0; k < MaxUnkn; k++) {
	    N(k,k) += 1.0E8; // Keep coordinates fixed (to iono free values)
	}
    }

    // Copy over to correctly sized matrices
    gpstk::Matrix <double> NN(unknowns, unknowns);
    gpstk::Matrix <double> bb(unknowns, 3);
    for (int k = 0; k < unknowns; k++) {
	for (int m = 0; m < unknowns; m++)
	    NN(k,m) = N(k,m);
	// Suppress unused unknowns, if any
	if (NN(k,k) == 0.0)
	    NN(k,k) = 1.0;
	for (int ot = 0; ot < 3; ot++)
	    bb(k,ot) = b(k,ot);
    }

    N2 = gpstk::inverse(NN);
    for (int k = 0; k < unknowns; k++) {
	if (N2(k,k) < 0.0)
	    cout << "Negative diagonal element " 
		<< k << ": " << N2(k,k) << endl;
    }
    b2 = bb;
}


main(int argc, char *argv[])
{
#define EPH_RANGE(C,T,X,S) ((precise ? \
	C.ComputeAtReceiveTime(T.time,X,S.prn,sp3store) : \
	C.ComputeAtReceiveTime(T.time,X,S.prn,bcestore)))

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

    bool phase;		// Process carrier phase data (instead of P code data)
    bool truecov;	// Use true DD covariances (inst. of no correlations)
    bool precise;       // Use precise ephemeris (inst. of broadcast)
    bool iono;		// Use nav file iono model
    bool tropo;		// Estimate troposphere parameters
    bool vecmode;	// Solve vector (inst. of end point coords)
    bool debug;
    double refsat_elev; // Minimum elevation of the reference satellite.
    			// Good value: 30.0
    double cutoff_elev;	// cut-off elevation. Good value: 10.0
    int MaxUnkn; 	// The number of std. unknowns. 3 for baseline est.,
    			// 6 for two endpoint positions, 8 for tropo est. too.

    char s[80];
    std::ifstream conf;
    conf.open("vecsol.conf", ios::in);
    conf >> phase;		conf.getline(s, 80);
    conf >> truecov;		conf.getline(s, 80);
    conf >> precise;		conf.getline(s, 80);
    conf >> iono;		conf.getline(s, 80);
    conf >> tropo;		conf.getline(s, 80);
    conf >> vecmode;		conf.getline(s, 80);
    conf >> debug;		conf.getline(s, 80);
    conf >> refsat_elev;	conf.getline(s, 80);
    conf >> cutoff_elev;	conf.getline(s, 80);
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
    cout << "Use carrier phases:       " << phase << endl;
    cout << "Use true correlations:    " << truecov << endl;
    cout << "Use precise ephemeris:    " << precise << endl;
    cout << "Use broadcast iono model: " << iono << endl;
    cout << "Use tropospheric est.:    " << tropo << endl;
    cout << "Vector mode:              " << vecmode << endl;
    cout << "Debugging mode:           " << debug << endl;
    cout << "Ref sat elevation limit:  " << refsat_elev << endl;
    cout << "Cut-off elevation:        " << cutoff_elev << endl;
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
	    }
	    catch(...) {
		cerr << "Something wrong with SP3 files." << endl << endl;
		exit(-1);
	    }
	}
	
	gpstk::Matrix <double> x0(MaxDim, 3, 0.0);
	gpstk::Vector <FixType> fixed(MaxDim, FIX_NONE);

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

	Position t1(roh1.antennaPosition, Position::Cartesian);
	Position t2(roh2.antennaPosition, Position::Cartesian);
	Position const t10(t1);  // To keep unknowns invariant
	Position const t20(t2);
	Position Pos1, Pos2;
	string const name1(roh1.markerName);
	string const name2(roh2.markerName);

	double crit(1.0);
	double const limit = (phase ? 0.01 : 0.02);
	for (int l = 0; (crit > limit) && l < 2; l++) {
	    // Iteration loop. Important! The unknowns are expected to 
	    // remain _identical_ across iterations.
	    cout << "Iteration: " << l << endl;
	    
	    // Map pointing from PRN to obs. eq. element position
	    map <RinexPrn, int> CommonSatsPrev;

	    RinexPrn OldRefSat;

	    map <RinexPrn, double> DDobsPrev;
	    map <RinexPrn, double> SecsPrev;
	    
	    // Open and read the observation files one epoch at a time.
	    // Compute a contribution to normal matrix and right hand side
	    RinexObsStream roffs1(argv[1]);
	    RinexObsStream roffs2(argv[2]);
	    roffs1.exceptions(ios::failbit);
	    roffs2.exceptions(ios::failbit);

	    RinexObsHeader roh1, roh2;
	    RinexObsData rod1, rod2;

	    roffs1 >> roh1;
	    roffs2 >> roh2;

	    if (l > 0) {
		t1 = t1 + Pos1;
		if (!vecmode)
		    t2 = t2 + Pos2;
	    }
	    Geodetic g1(t1, &geoid);
	    Geodetic g2(t2, &geoid);
	    
	    cout << name1 << ": " << t1 << endl 
		 << name2 << ": " << t2 << endl << endl;
	    // Print also geographic coords
	    Position t1g(t1);
	    Position t2g(t2);
	    cout << name1 << ": " << t1g.asGeodetic() << endl 
		 << name2 << ": " << t2g.asGeodetic() << endl;

	    gpstk::Matrix <double> N(MaxDim, MaxDim, 0.0);
	    gpstk::Matrix <double> b(MaxDim, 3, 0.0);
	    int observations(0), rejections(0);
	    double RMS(0);
	    // points to _after_ the last unknown
	    int unknowns = MaxUnkn;

	    while (roffs1 >> rod1 && roffs2 >> rod2) { // Epoch loop
		// Make sure we have a common epoch:
		while (rod1.time > rod2.time)
		    roffs2 >> rod2;
		while (rod1.time < rod2.time)
		    roffs1 >> rod1;
		double const Secs = rod1.time.hour() * 3600
		    + rod1.time.minute() * 60 + rod1.time.second();

		if (rod1.epochFlag < 2 && rod2.epochFlag < 2)
		    // Observations are good
		{
		    map <RinexPrn, int> CommonSats;
		    CommonSats.clear();
		    vector <RinexPrn> prnVec_1, prnVec_2;
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
		    RinexPrn RefSat;
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
			if (prnVec_2[i].prn > 0) {

			    // Invariant over iterations! Uses t10, t20
			    double dummy = EPH_RANGE(CER2, rod2, t10,
				    prnVec_2[i]);
			    double const riseVel1 = CER2.svPosVel.v.dot(t10);
			    Elev10[i] = CER2.elevation;
			    bool const elev1OK = CER2.elevation > refsat_elev;

			    dummy = EPH_RANGE(CER2, rod2, t20,
				    prnVec_2[i]);
			    double const riseVel2 = CER2.svPosVel.v.dot(t20);
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
			if (prnVec_2[i].prn > 0 
				&& Elev10[i] > cutoff_elev
				&& Elev20[i] > cutoff_elev) {
			    double r2 = EPH_RANGE(CER2, rod2, t2,
				    prnVec_2[i]);
			    double trop2 =
				trop.correction(t2, CER2.svPosVel.x, rod2.time);
			    r2 += trop2;
			    
			    for (int j = 0; j != prnVec_1.size(); j++) {
				if (prnVec_1[j].prn > 0
				    && prnVec_1[j].prn == prnVec_2[i].prn)
				{
				    // This sat is visible from both
				    // stations
			    	    double r1 = EPH_RANGE(CER1, rod1, t1,
				    prnVec_1[j]);
				    double trop1 = trop.correction(t1,
						CER1.svPosVel.x, rod1.time);
				    r1 += trop1;
				    
				    // Between-station diffs
				    double diffL1 = rangeVecL1_1[j] - rangeVecL1_2[i];
				    double diffL2 = rangeVecL2_1[j] - rangeVecL2_2[i];
				    // Subtract out approx values
				    double rdiffL1 = lambda1 * diffL1 - (r1 - r2);
				    double rdiffL2 = lambda2 * diffL2 - (r1 - r2);
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
					if (RefSat != OldRefSat) {
					    // Ref sat change; invalidates
					    // unknowns
					    CommonSatsPrev.clear();
					    cout << "New ref sat:" << RefSat << endl;
					    //DDobsValid.clear();
					    //SecsValid.clear();
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

					RinexPrn ThisSat = prnVec_1[j];
					if (CommonSatsPrev.find(ThisSat) ==
					    CommonSatsPrev.end()) {
					    // New satellite
					    CommonSats[ThisSat] = unknowns;
					    if (phase) {
						cout << endl << "New unknown " 
						    << unknowns << " == " 
						    << ThisSat << endl;

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
						//reject = (std::abs(DDobs[0]) > 1.0);
					    }
					    // Initialize previous obs for
					    // triple diff comp
					    DDobsPrev[ThisSat] = DDobs[0];
					    SecsPrev[ThisSat] = Secs;
					    reject = false;
					} else {

					    // Already known sat, copy
					    // forward
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
					    timebase = (timebase > 120 ? 120 :
							timebase);
					    double res 
						= (DDobs[0] - DDobsPrev[ThisSat]) / timebase;
					    // Cycle slips will show up here
					    // over 30 s as 0.0226 and
					    // 0.0131, respectively.
					    reject =
						(std::abs(res) *
						 sqrt(2.0 / (Qref + q_)) >
						(phase ? 0.001 : 0.1));
					    
					    if (!reject)
						RMS += res * res;
					    else {
						rejections++;
					    }
					    if (debug) {
						cout << ThisSat << ":";
						if (reject)
						    cout << "REJ [" <<
							Elev10[i] << ":" <<
							Elev20[i] << "] ";
						cout << setprecision(4) << res
						<< " ";
					    }
					    observations++;
					}

					if (!reject) {
					    // Update "last good" obs
					    DDobsPrev[ThisSat] = DDobs[0];
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

				    }	// end other-than-ref sats
				    // processing

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
	    cout << "Triple-diff RMS [m/s]:    " << sqrt(RMS /
						     (observations -
						      rejections)) << endl;

	    if (debug)
		cout << "Start of b vector:" << setw(6) << endl
		    << "b0: " << b(0,0) << " " << b(1,0) << " " << b(2,0) << endl
		    << "b1: " << b(0,1) << " " << b(1,1) << " " << b(2,1) << endl
		    << "b2: " << b(0,2) << " " << b(1,2) << " " << b(2,2) 
		    << endl << endl;

	    // Here we solve the normal equations and print solution
	    gpstk::Matrix <double> NN(unknowns, unknowns);
	    gpstk::Matrix <double> bb(unknowns, 3);
	    Solve(N, b, NN, bb, SOLVE_COORDS, fixed, MaxUnkn, unknowns);

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
		endl;
	    Pos1 = Position(sol(0,0), sol(1,0), sol(2,0));
	    Position PosCorr0 = Pos1;
	    Position PosCorr1 = Position(sol(0,1), sol(1,1), sol(2,1));
	    Position PosCorr2 = Position(sol(0,2), sol(1,2), sol(2,2));
	    if (!vecmode) {
		Pos2 = Position(sol(3,0), sol(4,0), sol(5,0));
		PosCorr0 = PosCorr0 - Pos2;
		PosCorr1 = PosCorr1 - Position(sol(3,1), sol(4,1), sol(5,1));
		PosCorr2 = PosCorr2 - Position(sol(3,2), sol(4,2), sol(5,2));
	    }
	    cout << "Iono free: " << PosCorr0 << " <--" << endl;
	    if (debug || iono) {
		cout << "Freq. 1:   " << PosCorr1 << endl;
		cout << "Freq. 2:   " << PosCorr2 << endl;
	    }
	    cout << endl;

	    // Manhattan distance for iteration stop:
	    crit = std::abs(PosCorr0[0]) + std::abs(PosCorr0[1]) 
		 + std::abs(PosCorr0[2]);

	    cout << "Standard deviations (unscaled):" << endl;
	    for (int k = 0; k < MaxUnkn; k++)
		cout << sqrt(NN(k, k)) << " ";
	    cout << endl;

	    Position vec(t1 - t2);
	    cout << "A priori vector:" << endl << vec << endl;
	    vec = vec + PosCorr0;	// define += operator!
	    cout << "A posteriori vector:" << endl << vec << endl << endl;

	    if (tropo) {
		cout <<
		    "Troposheric corr. parameters (fraction of full effect):"
		    << endl;

		cout << sol(MaxUnkn - 2, 0) << " " << sol(MaxUnkn - 1, 0) << endl;
		cout << "Standard deviations (unscaled):" << endl;
		cout << sqrt(NN(MaxUnkn - 2, MaxUnkn - 2)) << " "
		     << sqrt(NN(MaxUnkn - 1, MaxUnkn - 1)) << endl <<
		    endl;
	    }

	    // Ambiguity fixing on the unknowns, keeping coords as known:
	    if (phase) {
		Solve(N, b, NN, bb, SOLVE_AMBS, fixed, MaxUnkn, unknowns);
		sol = NN * bb;

		int fixedunknowns = 0;
		int widelanes = 0;
		cout << " DD bias fixes (fractional cycles):" << endl;
		for (int k = MaxUnkn; k < unknowns; k++) {
		    cout << "[" << sqrt(NN(k,k)) << "] ";
		    cout << k << ": ";
		    vector <double> x0vec(3);
		    x0vec[1] = x0(k, 1) + sol(k, 1);
		    x0vec[2] = x0(k, 2) + sol(k, 2);
		    FixType f = phaseCycles(x0vec, lambda1, lambda2);
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
