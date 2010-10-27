#pragma ident "$Id$"

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Copyright 2006, The University of Texas at Austin
//
//============================================================================

#include <map>
#include <list>
#include <set>
#include <algorithm>
#include <iomanip>

#include "FICFilterOperators.hpp"
#include "FileFilterFrame.hpp"

#include "DiffFrame.hpp"
#include "FICData.hpp"
#include "FICStream.hpp"

using namespace std;
using namespace gpstk;

class FICDiff : public DiffFrame
{
public:
   FICDiff(char* arg0)
         : DiffFrame(arg0, 
		     string("binary FIC")),
	             diagOption(0,string("diagnostic"),
				string("Detect and output systematic") + 
				string(" differences."),
				false)
  {
     diagOption.setMaxCount(1);
  }

protected:
   virtual void process();
 
private:

   void diagnoseDifferences(const string filename1,
	 		    const FileFilterFrame<FICStream, FICData>& f1,
	 		    const string filename2,
	                    const FileFilterFrame<FICStream, FICData>& f2);

   void printBlockNumTable(const string filename1,
                           const list<FICData>& flist);

   void printBlockPRNTable(const string filename1,
                           const list<FICData>& flist);

   void printFICDataDiff(const FICData& lhs, const FICData& rhs);

   void printDiffSummary(const string fileName1,
	 	         const list<FICData>& flist1,
		 	 const string fileName2,
			 const list<FICData>& flist2);

   double matchScore(const FICData& rhs, const FICData& lhs);

   CommandOptionNoArg diagOption;
};

void FICDiff::process()
{
   try
   {
      string fname1 = inputFileOption.getValue()[0];
      string fname2 = inputFileOption.getValue()[1];

      FileFilterFrame<FICStream, FICData> ff1(fname1);
      FileFilterFrame<FICStream, FICData> ff2(fname2);

      ff1.sort(FICDataOperatorLessThanFull());
      ff2.sort(FICDataOperatorLessThanFull());

      if (diagOption.getCount()==0)
      {
	 pair< list<FICData>, list<FICData> > difflist = 
	    ff1.diff(ff2, FICDataOperatorLessThanFull());

	 if (difflist.first.empty() && difflist.second.empty())
	    exit(0);

	 list<FICData>::iterator itr = difflist.first.begin();
	 while (itr != difflist.first.end())
	 {
	    (*itr).dump(cout << '<');
	    itr++;
	 }

	 cout << endl;

	 itr = difflist.second.begin();
	 while (itr != difflist.second.end())
	 {
	    (*itr).dump(cout << '>');
	    itr++;
	 }
      }
      else
         diagnoseDifferences(fname1, ff1, fname2, ff2);

   }
   catch(Exception& e)
   {
      cout << e << endl
           << endl
           << "Terminating.." << endl;
   }
   catch(exception& e)
   {
      cout << e.what() << endl
           << endl
           << "Terminating.." << endl;
   }
   catch(...)
   {
      cout << "Unknown exception... terminating..." << endl;
   }

}

void FICDiff::printBlockNumTable(const string fileName1,
				 const list<FICData>& flist)
{

   map<long, long> blockCount;
   blockCount[9] = blockCount[109] = blockCount[62] =  blockCount[162] = 0;

   for (list<FICData>::const_iterator i = flist.begin();
        i != flist.end(); i++)
      blockCount[(*i).blockNum]++;


   cout << endl << "Count by block number for " << fileName1 << endl << endl;\
 
   cout << setw(6) << "Block" << setw(7) << "Count" << endl;
   for (map<long, long>::iterator i=blockCount.begin();
	  i!=blockCount.end(); i++)
   {
     cout << setw(6) << i->first << setw(7) << i->second << endl;
   }
   
   cout << endl;

}


void FICDiff::printBlockPRNTable(const string fileName1,
				 const list<FICData>& flist)
{
  set<long> prnList, blockList;
   map<long, map<long, long> > bptable;
 
   cout << endl;
   cout << "Nav message breakout by PRN for " <<  fileName1 << endl << endl;

   for (list<FICData>::const_iterator i = flist.begin();
        i != flist.end(); i++)
   {
     long block = (*i).blockNum;
     long prn;

     switch (block) {

	case 109: 
	  prn = (*i).i[1];
	  break;

	case 9:
	  prn = static_cast<long>((*i).f[19]);
	  break;

	case 162:
	  prn = (*i).i[0];
	  break;

	case 62:
	  prn = (*i).i[3];
	  break;
     }

     prnList.insert(prn);
     blockList.insert(block);

     map<long, long>::iterator k;
     map<long, map<long, long> >::iterator j = bptable.find(block);
     if (j == bptable.end()) // No blocks of this type have been processed yet
       bptable[block][prn]=1;
     else
     {
       k = (*j).second.find(prn);
       if (k == (*j).second.end()) // Not sats with that block have been proc'd
	 bptable[block][prn]=1;
       else
         bptable[block][prn]+=1;
     }       
   }

   int prnwidth=5, countwidth=4;
   cout << setw(prnwidth+7) << "BLOCK" << endl;
   cout << setw(prnwidth) << "  PRN";
   for (set<long>::iterator b = blockList.begin();
	b!=blockList.end(); b++)
     cout << setw(countwidth) << *b;
   cout << endl;
   
   for (set<long>::iterator p = prnList.begin(); p!=prnList.end();p++)
   {
     cout << setw(prnwidth) << *p;

      for (set<long>::iterator b = blockList.begin();
	   b!=blockList.end(); b++)
      {
	 map<long, long>::iterator k = bptable[*b].find(*p);
	 if (k==bptable[*b].end())
	   cout << setw(countwidth) << 0;
	 else
	   cout << setw(countwidth) << (*k).second;
      }
      cout << endl;

   }
   cout << endl;
}

double FICDiff::matchScore(const FICData& lhs, const FICData& rhs)
{

   if ( (lhs.blockNum != rhs.blockNum) ||
        (lhs.f.size() != rhs.f.size()) ||
        (lhs.i.size() != rhs.i.size()) ||
        (lhs.c.size() != rhs.c.size()) )
   return 0.;

   double totalItems = lhs.f.size() +  lhs.i.size() + lhs.c.size(),
     totalScore = 0.;

   for (size_t k = 0; k < lhs.f.size(); k++)
      if (lhs.f[k] == rhs.f[k]) totalScore += 1.;

   for (size_t k = 0; k < lhs.i.size(); k++)
      if (lhs.i[k] == rhs.i[k]) totalScore += 1.;

   for (size_t k = 0; k < lhs.c.size(); k++)
      if (lhs.c[k] == rhs.c[k]) totalScore += 1.;
 
   return totalScore/totalItems;   

}

void FICDiff::printFICDataDiff(const FICData& lhs, const FICData& rhs)
{
   if (lhs.blockNum != rhs.blockNum)
   {
      cout << "Block difference. LHS is " << lhs.blockNum  << ", RHS is " 
           << rhs.blockNum;
      return;
   }

   if  (lhs.f.size() != rhs.f.size())
   {
      cout << "Float vector size different. " 
           << "LHS is " << lhs.f.size()
           << "RHS is " << rhs.f.size();
      return;
   }

   if  (lhs.i.size() != rhs.i.size())
   {
      cout << "Integer vector size different. " 
           << "LHS is " << lhs.i.size()
           << "RHS is " << rhs.i.size();
      return;
   }

   if  (lhs.c.size() != rhs.c.size())
   {
      cout << "Character vector size different. " 
           << "LHS is " << lhs.c.size()
           << ", RHS is " << rhs.c.size();
      return;
   }

   for (size_t k = 0; k < lhs.f.size(); k++)
      if (lhs.f[k] != rhs.f[k])
      {
        cout << endl << setprecision(16) 
	     << "LHS.f[" << k << "] = " << left << setw(20) << lhs.f[k] << endl
             << "RHS.f[" << k << "] = " << rhs.f[k] << endl
             << "LHS - RHS = " 
             << lhs.f[k]-rhs.f[k] << "." << endl
	     << "Block " << lhs.blockNum << ", field " << lhs.getElementLabel('f',k) << "." << endl;
        if ((lhs.blockNum==62) && (k==12) && (lhs.f[6]>0) && (lhs.f[6]<33))
	   cout << setprecision(5) << "Semimajor axis difference: "
                << lhs.f[k]*lhs.f[k] - rhs.f[k]*rhs.f[k] << " meters." << endl;
      } 
 
   for (size_t k = 0; k < lhs.i.size(); k++)
      if (lhs.i[k] != rhs.i[k])
      { 
        cout << endl 
	     << "LHS.i[" << k << "] = " << left << setw(20) << lhs.i[k] << endl
             << "RHS.i[" << k << "] = " << rhs.i[k] << endl
             << "Block " << lhs.blockNum << ", field " << lhs.getElementLabel('i',k) << "." << endl;
      } 
 

   for (size_t k = 0; k < lhs.c.size(); k++)
      if (lhs.c[k] != rhs.c[k])
      {
        cout << endl << "LHS.c[" << k << "] = " << lhs.c[k]
             << "RHS.c[" << k << "] = " << rhs.c[k] << endl;
      } 
 

   return;

}

void FICDiff::printDiffSummary(const string fileName1,
			       const list<FICData>& flist1,
			       const string fileName2,
			       const list<FICData>& flist2)
{
   // Make modifiable copies.
   list<FICData> lhs(flist1), rhs(flist2); 

   // Look for matches.
   long totalMatches=0;
   map <long, long> exactMatchCnts;
   map <long, long>::iterator mci;

   list<FICData>::iterator i_lhs, i_rhs;
   size_t recordNumber=0;

   for (i_lhs=lhs.begin(); i_lhs != lhs.end(); i_lhs++, recordNumber++)
   {
      FICData findme = *i_lhs;
      i_rhs=find(rhs.begin(), rhs.end(), findme);
      if (i_rhs!=rhs.end()) // we have an exact match
      {
         totalMatches++;
	 mci = exactMatchCnts.find((*i_lhs).blockNum);
         if (mci==exactMatchCnts.end())
	    exactMatchCnts[(*i_lhs).blockNum]=0;
         else 
	    exactMatchCnts[(*i_lhs).blockNum]+=1; 
      } 
      else // else we did not find an exact match
      {
	 list<FICData>::iterator i_rhs_match=rhs.begin();
	 double maximumScore=0.;

	 for (i_rhs=rhs.begin(); i_rhs !=rhs.end(); i_rhs++)
	 {
	    double score = matchScore(*i_lhs, *i_rhs);
	    //        cout << " got a score of " << score << "." <<endl;
	    if (score>maximumScore)
	    {
	      i_rhs_match = i_rhs;
	      maximumScore = score;
	    }

	 }

         cout << endl << "---------------------------------------" << endl;

	 cout << endl << "Record: " << recordNumber << endl;

	 cout << "Fuzzy match score: " << setprecision(4)
	      << maximumScore*100 << "\%" << endl;

	 printFICDataDiff(*i_lhs, *i_rhs_match);
	 cout << endl;
      }     
   }

   cout << endl << "---------------------------------------" << endl;

   cout << endl << endl << "Exact matches:" << totalMatches
        << " total." << endl << endl;


   cout << setw(6) << "Exact matches by block:" << endl;
   cout << "Block" << setw(7) << "Count" << endl;
   for (map<long, long>::iterator i=exactMatchCnts.begin();
	  i!=exactMatchCnts.end(); i++)
   {
     cout << setw(6) << i->first << setw(7) << i->second << endl;
   }
   
   cout << endl << endl;
   

}

void FICDiff::diagnoseDifferences(const string fileName1,
				  const FileFilterFrame<FICStream, FICData>& f1,
				  const string fileName2,
				  const FileFilterFrame<FICStream, FICData>& f2)
{
  list<FICData> ficList1 = f1.getData(),
                ficList2 = f2.getData();

  printBlockNumTable( fileName1, ficList1); 
  printBlockNumTable( fileName2, ficList2); 
  
  printBlockPRNTable( fileName1, ficList1); 
  printBlockPRNTable( fileName2, ficList2); 

  printDiffSummary(fileName1, ficList1, fileName2, ficList2); 
  
}


int main(int argc, char* argv[])
{
   try
   {
      FICDiff m(argv[0]);
      if (!m.initialize(argc, argv))
         return 0;
      if (!m.run())
         return 1;
      
      return 0;
   }
   catch(Exception& e)
   {
      cout << e << endl;
   }
   catch(exception& e)
   {
      cout << e.what() << endl;
   }
   catch(...)
   {
      cout << "unknown error" << endl;
   }
   return 0;
}
