// read and play with a matrix
//------------------------------------------------------------------------------------
#include "Exception.hpp"
//#define RANGECHECK 1          // this must be before include Matrix/Vector*
#include "Matrix.hpp"
#include "SRIMatrix.hpp"
#include "SparseMatrix.hpp"
//#include "Epoch.hpp"
#include "random.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <limits>
#include <unistd.h>       // isatty

using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// TODO
//
//------------------------------------------------------------------------------------
// RESULTS
///local/Code/work/lsa>jam && ./smtest --sci -p 2 -w 9 NGA26B.it1.mat  --timing  
//stored git version 22Apr15-125-g975e814 in /local/Code/work/src/svn_version.hpp
//Run all tests
//Column 5 of input SM: len=1701, N=2 (509,1.00e+00) (1028,1.00e+00)
//Row 2 of input SM: len=823, N=2 (728,1.00e+00) (822,-3.65e-03)
//Timing row/colCopy net 0.000 tot 7
//Timing Multiply(M) net 19.602 tot 27
//Timing Multiply(SM) net 7.829 tot 35
//>>>>>>>>>>>> Multiply is ~2x faster
//Timing transpose(M) net 0.069 tot 35
//Timing transpose(SM) net 0.024 tot 35
//>>>>>>>>>>>> Transpose is fast
//Test 5 computes a transformation: Part*Cov*transpose(Part)
//SData has density 0.992
//SPartials has density 0.002 >>>>>>>>>>>>>>>>>>> very sparse
//Timing tranpose(Part)*D(M) net 0.017 tot 35
//Timing tranpose(Part)*D(SM) net 0.008 tot 35
//>>>>>>>>>>>> Transpose is very very fast - this is very low density matrix
//Timing M * transpose(M) net 7.310 tot 42
//Timing SM * transpose(SM) manual net 0.036 tot 42
//>>>>>>>>>>>>  SM * tranpose(SM) is 200 times faster
//Timing SM * transpose(SM) function net 0.035 tot 42
//>>>>>>>>>>>>  the function is barely faster than just tranpose and op*
//Timing Transform(M) net 22.349 tot 65
//Timing Transform-manual(SM) net 0.103 tot 65
//>>>>>>>>>>>> P * C * PT is 200x faster, don't bother with a function

//>jam smtest && ./smtest NGA26B.it1.mat -p 2 -w 9 --sci --timing  -t 3
//stored git version 23Jun15-9-g6e6bd6f in /local/Code/work/src/svn_version.hpp
//Run test 3
//SData(1701), density 0.992
//SPartials(1701,822), density 0.002
//SMCov(1701,1701), density 0.002
//Timing lowerCholesky(Matrix) net 5.237e+00 tot 19
//Timing lowerCholesky(SparseMatrix) net 1.498e-01 tot 19
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> lowerCholesky 30x faster
//Difference ?= 0 : 6.939e-18
//Timing invViaCholesky-inverseLT(M) net 3.669e+00 tot 22
//Timing invViaCholesky-transpose(M) net 3.804e-02 tot 22
//Timing invViaCholesky-mult(M) net 3.523e+01 tot 58
//Timing invViaCholesky-inverseLT(SM) net 3.900e-02 tot 58
//Timing invViaCholesky-transpose(SM) net 1.582e-02 tot 58
//Timing invViaCholesky-mult(SM) net 1.514e-01 tot 58
//>>>>>>>>>>> inverseLT 100x faster, transpose 2xfaster, multiply 20x faster
//Difference invLT ?= 0 : 1.819e-12
//Difference invViaChol ?= 0 : 2.384e-07
//>>>>>>>>>>>>>>>>>>>>>> invViaCholesky is wrong
//Timing inverseCholesky(M) net 5.093e+01 tot 109
//Timing inverseViaCholesky(SM) net 4.396e-01 tot 109

///local/Code/work/lsa>jam && ./smtest --sci -p 2 -w 9 NGA26B.it1.mat -t 6 --timing
//Run test 6
//SData density 0.992
//SPartials density 0.002
//SMCov has density 0.002
//Timing SVD net 53.668 tot 61
//Timing LUD net 13.826 tot 75
//>>>>>>>>>>>>>> not bad - is it worth implmenting these in SM?
// NB can't speed SVD LUD HH up - just cast to Matrix and use regular ones.

///local/Code/work/lsa>jam smtest && ./smtest NGA26B.it1.mat --timing -t 6
//Run test 6
//SData(1701), density 0.992
//SPartials(1701,822), density 0.002
//SMCov(1701,1701), density 0.002
//Timing SVD net 5.433e+01 tot 62       this is SVD of MCov - no SM version
//Timing LUD(M) net 1.491e+01 tot 77       this is LUD of MCov - no SM version
//Timing inverseLUD(M) net 5.384e+01 tot 131
//>>>>>>>>>>>>>>> here's a test M vs SM for Partials - not MCov
//Timing Householder(M) net 1.543e+00 tot 132
//Timing Householder(SM) net 2.016e-01 tot 133  speed up by 7.5x

///local/Code/work/lsa>jam smtest && ./smtest NGA26B.it1.mat --timing -t 7
//stored git version 22Apr15-130-gc54bf2f
//Run test 7
//Timing SrifMU(M) net 1.864e+00 tot 9
//Timing SrifMU(SM) net 5.486e-01 tot 10
//>>>>>>>>>>>>>>>>>> SRIFMU is 3.4x faster

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// functions
//------------------------------------------------------------------------------------
void PrintMatrix(string msg, Matrix<double> M, int prec, int width, bool sci=false)
{
   cout << msg << " (" << M.rows() << "," << M.cols() << "):\n"
      << (sci ? scientific : fixed) << setprecision(prec) << setw(width) << M << endl;
}

void PrintVector(string msg, Vector<double> V, int prec, int width, bool sci=false)
{
   cout << msg << " (" << V.size() << "): "
      << (sci ? scientific : fixed) << setprecision(prec) << setw(width) << V << endl;
}

void PrintMatrix(string msg,SparseMatrix<double> SM,int prec,int width,bool sci=false)
{
   cout << msg << " (" << SM.rows() << "," << SM.cols() << ")"
     << (SM.isEmpty() ? " (empty)":"") << " density="
     << fixed << setprecision(3) << SM.density() << " :\n"
     << (sci ? scientific : fixed) << setprecision(prec) << setw(width) << SM << endl;
}

void PrintVector(string msg,SparseVector<double> SV,int prec,int width,bool sci=false)
{
   cout << msg << " (" << SV.size() << ")" << (SV.isEmpty() ? " (empty)":"")
     << " density=" << fixed << setprecision(3) << SV.density() << " :  "
     << (sci ? scientific : fixed) << setprecision(prec) << setw(width) << SV << endl;
}

//------------------------------------------------------------------------------------
string& stripTrailing(string& str, const string& xx, string::size_type n=string::npos)
{
   string::size_type pos=str.length()-xx.length();
   if(pos > str.length() || xx.empty()) return str;
   while(n > 0 && str.rfind(xx,pos) == pos && str.length() > 0) {
      str.erase(pos, string::npos);
      n--;
      pos = str.length() - xx.length();
   }
   return str;
}

//------------------------------------------------------------------------------------
string& stripLeading(string& str, const string& xx, string::size_type n=string::npos)
{
   string::size_type pos=0;
   if(xx.empty()) return str;
   while(n > 0 && str.find(xx,pos) == pos && str.length() > 0) {
      str.erase(pos, xx.length());
      n--;
   }
   return str;
}

//------------------------------------------------------------------------------------
vector<string> split(const string& str, const char delim)
{
   vector<string> fields;
   string tmpstr(str), tmp;

   stripLeading(tmpstr,string(1,delim));
   stripTrailing(tmpstr,string(1,delim));
   if(tmpstr.empty()) return fields;

   string::size_type epos, pos = tmpstr.find_first_not_of(delim);
   if(pos != string::npos) {
      while(tmpstr.size() > 0) {
         epos = tmpstr.find(delim,pos);
         if(epos == string::npos) tmp = string(tmpstr, pos, epos);
         else                     tmp = string(tmpstr, pos, epos-pos);
         stripLeading(tmpstr,tmp);
         stripLeading(tmpstr,string(1,delim));
         fields.push_back(tmp);
      };
   }
   else fields.push_back(tmpstr);

   return fields;
}

//------------------------------------------------------------------------------------
// return -3 invalid type
//        -1 file open fail
//         0 no matrix found
//         1 or 2 on success, as found 1 or 2 matrices
int ReadMatrix(Matrix<double>& M1, Matrix<double>& M2, string& file)
{
   static const bool debug(false);

   // open file
   istream *pin;                 // do it this way for Windows...
   if(!file.empty()) {
      pin = new ifstream(file.c_str());
      if(pin->fail()) {
         if(debug) cout << "Could not open file " << file << " .. abort.\n";
         return -1;
      }
   }
   else {
      file = string("stdin");
      pin = &cin;
   }

   // read loop
   bool doFill(false);
   int rows(0),cols(0),ii(0),jj(0),count(0);
   const int ntypes=7;
   enum Type{GEN,LOW,UPT,DIA,SYM,SQU,UNK};
   string labels[ntypes]={"GEN","LOW","UPT","DIA","SYM","SQU","UNK"};
   Type types[ntypes]={GEN,LOW,UPT,DIA,SYM,SQU,UNK};
   Type type;

   char ch;
   const int BUFF_SIZE=32767;
   char buffer[BUFF_SIZE];
   int i,j,nrc,iret(0);
   string line;
   vector<string> F;
   Matrix<double> Mtmp;
   while(pin->getline(buffer,BUFF_SIZE)) {
      //if(pin->eof() || !pin->good()) break;
      line = buffer;

      stripTrailing(line,"\n");
      stripTrailing(line,"\r");
      stripTrailing(line," ");
      stripLeading(line," ");

      // strip trailing comments
      string::size_type pos=0;
      pos = line.find('#',pos);
      if(pos != string::npos) {
         if(debug) cout << "found comment " << line.substr(pos) << endl;
         line.erase(pos, string::npos);
         stripTrailing(line," ");
      }

      //change(line,"\t"," ");     // replace tabs
      pos=0;
      while(1) {
         pos = line.find('\t',pos);
         if(pos != string::npos) line[pos] = ' ';
         else break;
      }

      // split on ' '
      F = split(line,' ');
      if(debug) cout << "line with " << F.size() << " fields: /"
                                             << line << "/" << endl;
      if(F.size() == 0) continue;

      if(F[0].substr(0,1) == "#") {
         if(debug) cout << "comment-continue" << endl;
         continue;
      }

      else if(F[0].substr(0,2) == "t=" || F[0].substr(0,2) == "T=" ||
              F[0].substr(0,2) == "r=" || F[0].substr(0,2) == "R=" ||
              F[0].substr(0,2) == "c=" || F[0].substr(0,2) == "C=")
      {
         for(i=0; i<3; i++) {
            if(F[i].substr(0,2) == "t=" || F[0].substr(0,2) == "T=") {
               for(j=0; j<ntypes; j++) {
                  if(F[i].substr(2,3) == labels[j]) {
                     type = types[j];
                     if(debug) cout << "Found type: " << labels[j] << endl;
                     break;
                  }
               }
               if(j == ntypes) {
                  if(debug) cout << "Invalid type: " << line << endl;
                  return -3;
               }
            }
            else if(F[i].substr(0,2) == "r=" || F[0].substr(0,2) == "R=") {
               rows = atoi(F[i].substr(2).c_str());
               if(debug) cout << "Found rows " << rows << endl;
            }
            else if(F[i].substr(0,2) == "c=" || F[0].substr(0,2) == "C=") {
               cols = atoi(F[i].substr(2).c_str());
               if(debug) cout << "Found cols " << cols << endl;
            }
         }
      }
      else if(F[0] == ":::") {
         if(debug) cout << "Found :::" << endl;
         doFill = true;
         Mtmp = Matrix<double>(rows,cols,0.0);

         // check that rows,cols are consistent with type

         // compute the number of elements needed to fill matrix
         switch(type) {
            case LOW: case UPT: case SYM: nrc = (rows*(rows+1))/2; break;
            case DIA:                     nrc = rows; break;
            case SQU: case GEN: default:  nrc = rows*cols; break;
         }
      }
      else if(doFill) {
         for(i=0; i<F.size(); i++) {
            // TD test that its a number?

            Mtmp(ii,jj) = strtod(F[i].c_str(),0);
            count++;

            // is this the end of the matrix?
            if(count == nrc) {
               if(debug) cout << "Matrix is full " << count << " iret " << iret<<endl;
               if(iret == 0) M1 = Mtmp;
               if(iret == 1) M2 = Mtmp;
               doFill = false;
               if(++iret > 1) break;           // only 2 for now
               // reset
               Mtmp = Matrix<double>();
               rows = cols = ii = jj = nrc = count = 0;
            }
            else {
               // increment indexes
               jj++;
               switch(type) {
                  case LOW: if(jj>ii) { ii++; jj=0; } break;
                  case UPT: if(jj>=cols) { ii++; jj=ii; } break;
                  case DIA: ii=jj; break;
                  case SYM:
                     Mtmp(jj-1,ii) = Mtmp(ii,jj-1);
                     if(jj>ii) { ii++; jj=0; }
                     break;
                  case SQU: case GEN: default:
                     if(jj>=cols) { ii++; jj=0; } break;
               }
            }
         }
      }
      else { cout << "Warning - ignoring this line /" << line << "/" << endl; }
   }

   // close file
   if(pin != &cin) {
      ((ifstream *)pin)->close();
      delete pin;
   }

   if(M1.rows() == 0 || M1.cols() == 0) return -2;
   else if(doFill) cout << "Warning - reading second matrix failed.\n";
   
   return iret;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
string cleanMatrixString(const Matrix<double>& M, int prec, int width, bool sci,
   bool sym=false)
{
   ostringstream oss;
   oss << (sci ? scientific : fixed) << setprecision(prec);
   for(int i=0; i<M.rows(); i++) {
      for(int j=0; j<(sym ? i+1 : M.cols()); j++) {
         //if(M(i,j) == 0)
         if(::fabs(M(i,j)) < 1.e-10) //::pow(10.0,-prec))
            oss << " " << setw(width) << "0";
         else
            oss << " " << setw(width) << M(i,j);
      }
      if(i < M.rows()-1) oss << endl;
   }
   return oss.str();
}

//------------------------------------------------------------------------------------
// print timing information - initialize with blank msg
void timing(std::string msg)
{
   static clock_t totaltime;
   //static gpstk::Epoch wallbegin, wallend;

   //if(msg.empty()) wallbegin.setLocalTime();
   //else {
      //wallend.setLocalTime();
      cout << "Timing " << msg << " net " << std::scientific << std::setprecision(3)
            << double(clock()-totaltime)/double(CLOCKS_PER_SEC)
            //<< " tot " << int(wallend - wallbegin)
            << endl;
   //}
   totaltime = clock();
}

//------------------------------------------------------------------------------------
int BadOption(string& arg) {
   cout << "Error: " << arg << " requires argument. Abort.\n";
   return -1;
}

//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
try {
   clock_t maintotaltime=clock();
   bool help,dosci,dotime,verbose;
   unsigned int i,j,k,prec=3,width=7,nconst=0,npart=0;
   int ntest;
   string filename;
   ostream *pout;
   ostringstream oss;

   timing("");       // initialize with blank msg

   // defaults
   dosci=verbose=dotime=false;
   ntest = -1;       // -1 means all

   // get command line
   help = false;  //(argc <= 1);           // can't do this and pipe into it
   for(i=1; i<argc; i++) {
      string arg(argv[i]);
      if(argv[i][0] == '-') {
         string arg(argv[i]);
         if(arg == string("--help") || arg == string("-h"))
            help = true;
         else if(arg == string("--prec") || arg == string("-p")) {
            if(i==argc-1) return BadOption(arg);
            prec = atoi(argv[++i]);
         }
         else if(arg == string("--width") || arg == string("-w")) {
            if(i==argc-1) return BadOption(arg);
            width = atoi(argv[++i]);
         }
         else if(arg == string("--sci"))
            dosci = true;
         else if(arg == string("--verbose") || arg == string("-v"))
            verbose = true;
         else if(arg == string("--timing"))
            dotime = true;
         else if(arg == string("--test") || arg == string("-t"))
            ntest = atoi(argv[++i]);
         else {
            cout << "Ignore unknown option: " << arg << endl;
         }
      }
      else {
         filename = string(argv[i]);
      }
   }

   // no file and no stdin
   if(filename.empty() && isatty(fileno(stdin))) help = true;

   if(help) {
      cout << "Prgm smtest: Test SparseMatrix class\n"
      << "Usage: matrix [options] <file>\n"
      << " Options:\n"
      << "   --test|-t <n>  Run test n (all)\n"
      << "   --timing    output timing information\n"
      << "   --prec <p>  output using precision p (" << prec << ")\n"
      << "   --width <w> output using width w (" << width << ")\n"
      << "   --sci       scientific output (else fixed)\n"
      << "   --verbose   verbose output\n"
      << "   --help      print this and quit\n"
      << "\n"
      << " Input file of form:\n"
      << "    # this is a comment: specs, then :::, then one row per line\n"
      << "    t=GEN r=9 c=5\n"
      << "    # NB t=type:GEN/SQU/SYM/DIA/LOW/UPT, r=rows, c=cols\n"
      << "    :::\n"
      << "    1.0  0.0 -1.0  0.0  0.0\n"
      << "    0.0  1.0  0.0 -1.0  0.0\n"
      << "    (...for r=9 rows)\n"
      << " NB Use eqnout from lsasolver = partials, data, mcov\n"
      ;
      return -1;
   }
   if(dotime) timing("cmdline");

   int ninput;
   Matrix<double> MF,MF2;

   // read input file ----------------------------------------------
   ninput = ReadMatrix(MF,MF2,filename);
   if(ninput < 0) {
      if(ninput == -1) cout << "Error: could not open file " << filename << endl;
      if(ninput == -2) cout << "Error: input file " << filename << " has wrong format"
         << endl;
      return ninput;
   }
   if(dotime) timing("reading");

   // nice trick
   ofstream savefmt;       // get this
   cout << (dosci ? scientific : fixed) << setprecision(prec) << setw(width);
   if(ntest >= 0) cout << "Run test " << ntest << endl;
   else           cout << "Run all tests" << endl;
   savefmt.copyfmt(cout);

   // split into Part, Data, MCov ----------------------------------
   Vector<double> Data(MF.colCopy(MF.cols()-1));
   Matrix<double> Partials(MF,0,0,MF.rows(),MF.cols()-1), MCov(MF2);
   SparseVector<double> SData(Data);
   SparseMatrix<double> SPartials(Partials), SMCov(MF2), SMF(MF);

   // echo the input
   if(verbose) {
      PrintMatrix("Input Matrix from file " + filename + " = Partials || Data",
                        MF,prec,width,dosci);
      PrintMatrix("Input SMatrix = SPartials || SData",SMF,prec,width,dosci);
      if(ninput > 1) PrintMatrix("Second Matrix (SMCov) from file "+filename,
                                    SMCov,prec,width,dosci);
   }
   cout << "SData(" << SData.size() << "), density "
         << fixed << setprecision(3) << SData.density() << endl;
   cout << "SPartials(" << SPartials.rows() << "," << SPartials.cols()
         << "), density " << fixed << SPartials.density() << endl;
   if(ninput > 1)
      cout << "SMCov(" << SMCov.rows() << "," << SMCov.cols() << "), density "
         << fixed << SMCov.density() << endl;
   if(dotime) timing("setup");
   
   Matrix<double> A,B,C;
   SparseMatrix<double> SA,SB,SC;
   SparseVector<double> SV,SVA,SVB,SVC;
   Vector<double> V,VA,VB,VC;

   if(dotime) timing("start of tests");

   // do the tests
   if(ntest == 0) return 0;
   if(ntest == -1 || ntest == 1) {                 // TEST 1 - row/col Copy
      if(verbose) cout << " -------------- Test 1 -----------------\n";
      SV = SMF.colCopy(5);
      cout << "Column 5 of input SM: " << SV.dump(2,true) << endl;
      SV = SMF.rowCopy(2);
      cout << "Row 2 of input SM: " << SV.dump(2,true) << endl;
      if(dotime) timing("row/colCopy");

      V = Vector<double>(SV);
      A = Matrix<double>(SMF);
      if(dotime) timing("cast");
      cout << "Difference M ?= 0 : " << scientific
               << maxabs(SparseMatrix<double>(A)-SMF) << endl;
      cout << "Difference V ?= 0 : " << scientific
               << maxabs(SparseVector<double>(V)-SV) << endl;
   }

   if(ntest == -1 || ntest == 2) {                 // TEST 2 - multiplication
      if(verbose) cout << " -------------- Test 2 -----------------\n";

      // create a random matrix with rows = Partials.cols --------------
      Matrix<double> R(Partials.cols(),Partials.rows());
      for(i=0; i<R.rows(); i++)
         for(j=0; j<R.cols(); j++)
            R(i,j) = 2.0*(Rand()-0.5);
      if(verbose) PrintMatrix("Random Matrix",R,prec,width,dosci);
      if(dotime) timing("create Random");

      // --------------------------- SM * M
      A = Partials*R;
      if(dotime) timing("Multiply(M)");
      SA = SPartials*R;
      if(dotime) timing("Multiply(SM)");
      cout << "Difference ?= 0 : " << scientific << maxabs(A-SA) << endl;
      if(verbose) {
         PrintMatrix("Partials * Random",A,prec,width,dosci);
         PrintMatrix("Spartials * Random",SA,prec,width,dosci);
         PrintMatrix("Difference - all zero?",(A-SA),prec,0,dosci);
      }

      // M * SM
      A = R*Partials;
      if(dotime) timing("Multiply(M)");
      SA = R*SPartials;
      if(dotime) timing("Multiply(SM)");
      cout << "Difference ?= 0 : " << scientific << maxabs(A-SA) << endl;
      if(verbose) {
         PrintMatrix("Random * Partials\n",A,prec,width,dosci);
         PrintMatrix("Random * SPartials",SA,prec,width,dosci);
         PrintMatrix("Difference - all zero?",(A-SA),prec,0,dosci);
      }
   }

   if(ntest == -1 || ntest == 3) {                 // TEST 3 - Cholesky
      if(verbose) cout << " -------------- Test 3 -----------------\n";
      if(ninput != 2 || MCov.rows() != MCov.cols()) {
         cout << "Cannot do test 3, input is not a square matrix.\n";
      }
      else {
      cout << "Input matrix has density "
            << fixed << setprecision(3) << SMCov.density() << endl;

      // compute lowerCholesky
      A = lowerCholesky(MCov);
      if(dotime) timing("lowerCholesky(Matrix)");

      SA = lowerCholesky(SMCov);
      if(dotime) timing("lowerCholesky(SparseMatrix)");

      cout << "Difference ?= 0 : " << scientific << maxabs(A-SA) << endl;
      if(verbose) {
         cout << "Cholesky(M)\n" << cleanMatrixString(A,prec,width,dosci) << endl;
         PrintMatrix("Cholesky(SM)",SA,prec,width,dosci);
      }
      if(dotime) timing("finish lowerCholesky");

      // invert
      B = inverseLT(A);
      if(dotime) timing("invViaCholesky-inverseLT(M)");
      A = transpose(B);
      if(dotime) timing("invViaCholesky-transpose(M)");
      C = A*B;
      if(dotime) timing("invViaCholesky-mult(M)");

      SB = inverseLT(SA);
      if(dotime) timing("invViaCholesky-inverseLT(SM)");
      SA = transpose(SB);
      if(dotime) timing("invViaCholesky-transpose(SM)");
      SC = matrixTimesTranspose(SA);
      if(dotime) timing("invViaCholesky-mult(SM)");

      cout << "Difference invLT ?= 0 : " << scientific << maxabs(B-SB) << endl;
      cout << "Difference invViaChol ?= 0 : " << scientific << maxabs(C-SC) << endl;
      if(verbose) {
         cout << "inverseLT(lowerCholesky)\n"
               << cleanMatrixString(B,prec,width,dosci) << endl;
         cout << "invInput = trans(invLT)*invLT\n"
               << cleanMatrixString(C,prec,width,dosci) << endl;
         cout << "inv*Input = unity?\n" << cleanMatrixString(C*MCov,prec,width,dosci)
               << endl;
         PrintMatrix("inverseLT(lowerCholesky(SM))",SB,prec,width,dosci);
         PrintMatrix("invInput = trans(invLT)*invLT(SM)",SC,prec,width,dosci);
         SB = SC*SMCov;
         cout << "Zeroize at 1.e-15\n"; SB.zeroize(1.e-15);
         PrintMatrix("inv*Input = unity? (SM)",SB,prec,width,dosci);
      }
      if(dotime) timing("finish inverse Cholesky");

      A = inverseCholesky(MCov);
      B = A*MCov;
      if(dotime) timing("inverseCholesky(M)");
      SA = inverseViaCholesky(SMCov);
      SB = SA*SMCov;
      if(dotime) timing("inverseViaCholesky(SM)");
      if(verbose) {
         cout << "Zeroize at 1.e-15\n"; SB.zeroize(1.e-15);
         PrintMatrix("inv*Input==unity?(SM)",SB,prec,width,dosci);
      }
      if(dotime) timing("finish invViaCholesky");

      }  // end else
   }

   if(ntest == -1 || ntest == 4) {                 // TEST 4 - transpose
      if(verbose) cout << " -------------- Test 4 -----------------\n";
      A = transpose(MF);
      if(dotime) timing("transpose(M)");
      SA = transpose(SMF);
      if(dotime) timing("transpose(SM)");
      cout << "Difference ?= 0 maxabs : " << scientific << maxabs(A-SA) << endl;
      cout << "Difference ?= 0 minabs : " << scientific << minabs(A-SA) << endl;
      cout << "Difference ?= 0 min : " << scientific << min(A-SA) << endl;
      cout << "Difference ?= 0 max : " << scientific << max(A-SA) << endl;
      if(verbose) {
         PrintMatrix("tranpose(M)", A, prec, width, dosci);
         PrintMatrix("tranpose(SM)", SA, prec, width, dosci);
         //cout << SA.dump(2,true) << endl;
         PrintMatrix("Difference - all zero?",(A-SA),prec,0,dosci);
      }
      cout << "Cast SM to M\n";
      B = Matrix<double>(SA);
      if(verbose) PrintMatrix("Diff ?= all zero?",A-B,2,8,true);
      cout << "Cast M to SM\n";
      SB = SparseMatrix<double>(A);
      cout << "Difference ?= 0 : " << scientific << maxabs(SB-SA) << endl;
      if(verbose) PrintMatrix("Difference - all zero?",(SB-SA),prec,0,dosci);
   }

   if(ntest == -1 || ntest == 5) {                 // TEST 5 - transform
      if(verbose) cout << " -------------- Test 5 -----------------\n";
      // feed it an --eqnout = partials||data; mcov
      if(ninput != 2 || MF.rows() != MF2.rows() || MF2.rows() != MF2.cols()) {
         cout << "Cannot do test 5 - need two matricies for M*C*MT\n";
      }
      else {
         cout << "Test 5 computes a transformation: Part*Cov*transpose(Part)\n";

         Vector<double> VA(transpose(Partials) * Data);
         if(dotime) timing("tranpose(Part)*D(M)");
         SparseVector<double> SVA(transpose(SPartials) * SData);
         if(dotime) timing("tranpose(Part)*D(SM)");

         SVC = VA-SVA;
         cout << "Difference ?= 0 : " << scientific << maxabs(SVC) << endl;
         if(verbose) {
            cout << "Diff ?= zero ";
            cout.copyfmt(savefmt); cout << SVC << endl;
         }
         B = transpose(Partials);
         SB = transpose(SPartials);
         if(dotime) timing("clean");

         A = B * transpose(B);
         if(dotime) timing("M * transpose(M)");
         SA = SB * transpose(SB);
         if(dotime) timing("SM * transpose(SM) manual");
         cout << "Difference ?= 0 : " << scientific << maxabs(A-SA) << endl;
         SA = matrixTimesTranspose(SB);
         if(dotime) timing("SM * transpose(SM) function");
         cout << "Difference ?= 0 : " << scientific << maxabs(A-SA) << endl;

         A = B * MCov * transpose(B);
         if(dotime) timing("Transform(M)");
         SA = SB * SMCov * transpose(SB);
         if(dotime) timing("Transform-manual(SM)");
         cout << "Difference ?= 0 : " << scientific << maxabs(A-SA) << endl;
         // don't bother - too slow
         //SA = transform(SB,SMCov);
         //if(dotime) timing("Transform(SM)");
         //cout << "Difference ?= 0 : " << scientific << maxabs(A-SA) << endl;

         if(verbose) {
            PrintMatrix("Transform(M)",A,prec,width,dosci);
            PrintMatrix("Transform(SM)",SA,prec,width,dosci);
            //cout << SA.dump(2,true) << endl;
            PrintMatrix("Difference - all zero?",(A-SA),prec,0,dosci);
         }

      }
   }

   if(ntest == -1 || ntest == 6) {                 // TEST 6 - SVD, LUD, HH
      // NB can't speed SVD,LUD up - just cast to Matrix and use regular ones.
      if(verbose) cout << " -------------- Test 6 SVD ------\n";
      if(ninput != 2) cout << "Cannot do test 6 - need second matrix\n";
      else {
      A = Matrix<double>(SMCov);

      SVD<double> svd;
      svd(A);
      svd.sort(true);
      SB = SparseMatrix<double>(svd.V);
      SC = SparseMatrix<double>(svd.U);
      if(verbose) {
         cout << "SVD of MCov:\n";
         PrintMatrix("SVD: V",SB,prec,width,dosci);
         PrintMatrix("SVD: U",SC,prec,width,dosci);
         PrintVector("SVD: S",svd.S,prec,width,dosci);
         //cout << "SVD: V " << SB.dump(2,true) << endl;
         //cout << "SVD: U " << SC.dump(2,true) << endl;
      }
      if(dotime) timing("SVD");

      LUDecomp<double> MLUD;
      MLUD(A);
      if(dotime) timing("LUD(M)");
      if(verbose) {
         cout << "LUD of MCov:\n";
         cout << "LU(M): LU " << SparseMatrix<double>(MLUD.LU).dump(2,true) << endl;
         oss.str(""); oss << "LU(M): Pivot [";
         for(i=0; i<MLUD.Pivot.size(); i++) oss << " " << MLUD.Pivot(i);
         cout << oss.str() << " ]" << endl;
      }

      B = inverseLUD(A);
      if(dotime) timing("inverseLUD(M)");
      if(verbose) {
         cout << "inverseLU(M): " << SparseMatrix<double>(B).dump(2,true) << endl;
         cout << "inv*Input = unity?\n" << cleanMatrixString(B*A,prec,width,dosci)
               << endl;
      }

      // can speed up HH
      Householder<double> HHA;
      HHA(Partials);
      if(dotime) timing("Householder(M)");

      SB = SparseHouseholder(SPartials);
      if(dotime) timing("Householder(SM)");

      if(verbose) {
         cout << "HH of Partials:\n";
         cout << "HH(M): " << SparseMatrix<double>(HHA.A).dump(2,true) << endl;
         cout << "HH(SM): " << SB.dump(2,true) << endl;
         PrintMatrix("Difference - all zero?",(SB-HHA.A),prec,0,dosci);
      }

      }  // end else
   }

   if(ntest == -1 || ntest == 7) {                 // TEST 7 - SRIFMU
      if(verbose) cout << " -------------- Test 7 SRIFMU ---\n";
      // MF / SMF are Partials||data

      A = Matrix<double>();
      V = Vector<double>();
      SrifMU(A, V, MF);
      if(dotime) timing("SrifMU(M)");

      B = Matrix<double>();
      VC = Vector<double>();
      SrifMU(B, VC, SMF);              // NB state(VC) and cov(B) are NOT sparse
      if(dotime) timing("SrifMU(SM)");

      if(verbose) {
         PrintMatrix("SRIF R(M)",A,prec,width,dosci);
         PrintMatrix("SRIF R(SM)",SparseMatrix<double>(B),prec,width,dosci);
         PrintMatrix("Difference-all zero?",(SparseMatrix<double>(A)-B),prec,0,dosci);
         // NB the subtle difference between above and ...SparseMatrix<double>(A-B)...
         PrintVector("SRIF  Z(M)",V,prec,width,dosci);
         PrintVector("SRIF Z(SM)",VC,prec,width,dosci);
         PrintVector("Difference-all zero?",SparseVector<double>(V-VC),prec,0,dosci);
         // also want the last column of MF = residuals
         VA = MF.colCopy(MF.cols()-1);
         PrintVector("SRIF resid(M)",VA,prec,width,dosci);
         SVA = SMF.colCopy(MF.cols()-1);
         PrintVector("SRIF resid(SM)",SVA,prec,width,dosci);
         PrintVector("SRIF resid(M-SM)",SVA-VA,prec,width,dosci);
      }
      cout << "Difference R ?= 0 : " << scientific
               << maxabs(SparseMatrix<double>(A)-B) << endl;
      cout << "Difference Z ?= 0 : " << scientific
               << maxabs(SparseVector<double>(V)-VC) << endl;
      cout << "Difference resid ?= 0 : " << scientific
               << maxabs(SparseVector<double>(VA)-SVA) << endl;
   }
   cout << endl << " smtest timing: " << fixed << setprecision(5)
      << double(clock()-maintotaltime)/double(CLOCKS_PER_SEC)
      << " seconds." << endl;

	return 0;
}
catch (Exception& e) {
   cout << "Prgm smtest caught an exception:\n" << e.what() << endl;
   return -1;
}
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
