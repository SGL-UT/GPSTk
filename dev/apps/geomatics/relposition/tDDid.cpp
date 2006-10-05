//------------------------------------------------------------------------------------
// tDDid.cpp  Test class DDid
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// includes
// system
//#include <time.h>
#include <iostream>
#include <iomanip>
//#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cstdlib>

// geomatics lib
#include "GSatID.hpp"
#include "DDid.hpp"

//------------------------------------------------------------------------------------
using namespace gpstk;
using namespace std;

//------------------------------------------------------------------------------------
// Global data -- for this module only

//------------------------------------------------------------------------------------
int main(void)
{
   int i,j,ii,jj;
   vector<string> sites;
   vector<GSatID> sats;
   vector<DDid> dds;
   GSatID sat;
   sat.setfill('0');

   sites.push_back("EURO");
   sites.push_back("OEM4");
   sites.push_back("Fred");
   sites.push_back("Mill");
   sites.push_back("Junk");
   sites.push_back("Stuf");
   sats.push_back(GSatID(1));
   sats.push_back(GSatID(2));
   sats.push_back(GSatID(3));
   sats.push_back(GSatID(5));
   sats.push_back(GSatID(7));
   sats.push_back(GSatID(11));
   sats.push_back(GSatID(13));
   sats.push_back(GSatID(14));
   sats.push_back(GSatID(15));
   sats.push_back(GSatID(17));
   sats.push_back(GSatID(18));
   sats.push_back(GSatID(19));
   sats.push_back(GSatID(20));
   sats.push_back(GSatID(21));
   sats.push_back(GSatID(22));
   sats.push_back(GSatID(23));
   sats.push_back(GSatID(24));
   sats.push_back(GSatID(25));
   sats.push_back(GSatID(26));
   sats.push_back(GSatID(28));
   sats.push_back(GSatID(29));
   sats.push_back(GSatID(30));
   sats.push_back(GSatID(31));
   // form all DDs
   for(i=0; i<sites.size(); i++) {
      for(j=i+1; j<sites.size(); j++) {
         for(ii=0; ii<sats.size(); ii++) {
            for(jj=ii+1; jj<sats.size(); jj++) {
               DDid d(sites[i],sites[j],sats[ii],sats[jj]);
               //cout << "Create " << d << endl;
               dds.push_back(d);
            }
         }
      }
   }

   //DDid dj,djj;
   DDid dj = DDid(sites[4],sites[5],sats[2],sats[12]);
   DDid djj = DDid(sites[5],sites[4],sats[2],sats[12]);
   for(i=0; i<37; i++) {
      j = int((double(rand())/RAND_MAX)*dds.size());
      jj = int((double(rand())/RAND_MAX)*dds.size());
      dj = dds[j];
      djj = dds[jj];
      ii = DDid::compare(dj,djj);
      cout << "compare(" << dj << ", " << djj << ") = " << ii << endl;
   }

   dj = DDid(sites[4],sites[5],sats[2],sats[12]);
   djj = DDid(sites[5],sites[4],sats[2],sats[12]);
   cout << "compare(" << dj << ", " << djj << ") = " << DDid::compare(dj,djj) << endl;
   djj = DDid(sites[4],sites[5],sats[12],sats[2]);
   cout << "compare(" << dj << ", " << djj << ") = " << DDid::compare(dj,djj) << endl;
   djj = DDid(sites[5],sites[4],sats[12],sats[2]);
   cout << "compare(" << dj << ", " << djj << ") = " << DDid::compare(dj,djj) << endl;
   djj = DDid(sites[4],sites[5],sats[2],sats[12]);
   cout << "compare(" << dj << ", " << djj << ") = " << DDid::compare(dj,djj) << endl;

   sort(dds.begin(),dds.end());
   //cout << "Sorted list of DDs:" << endl;
   //for(i=0; i<dds.size(); i++) cout << setw(4) << i << " " << dds[i] << endl;

   vector<DDid>::const_iterator it;
   djj = DDid(sites[4],sites[5],sats[2],sats[12]);
   cout << "Find one: " << djj << endl;
   it = find(dds.begin(),dds.end(),djj);
   if(it != dds.end()) cout << "Found it: " << *it << endl;

   djj = DDid(sites[5],sites[4],sats[12],sats[2]);
   cout << "Find another one: " << djj << endl;
   it = find(dds.begin(),dds.end(),djj);
   if(it != dds.end()) cout << "Found it: " << *it << endl;

   djj = DDid(sites[5],sites[4],sats[2],sats[12]);
   cout << "Find yet another one: " << djj << endl;
   it = find(dds.begin(),dds.end(),djj);
   if(it != dds.end()) cout << "Found it: " << *it << endl;

   map<DDid,int> DDMap;
   dj = DDid(string("ASWA"), string("CTRA"), GSatID(30), GSatID(14)); DDMap[dj] = 10;
   dj = DDid(string("ASWA"), string("CTRA"), GSatID(11), GSatID(25)); DDMap[dj] = 9;
   dj = DDid(string("ASWA"), string("CTRA"), GSatID(25), GSatID(14)); DDMap[dj] = 8;
   dj = DDid(string("ASWA"), string("CTRA"), GSatID(23), GSatID(14)); DDMap[dj] = 7;
   dj = DDid(string("ASWA"), string("CTRA"), GSatID(21), GSatID(14)); DDMap[dj] = 6;
   dj = DDid(string("ASWA"), string("CTRA"), GSatID(18), GSatID(14)); DDMap[dj] = 5;
   dj = DDid(string("ASWA"), string("CTRA"), GSatID(15), GSatID(14)); DDMap[dj] = 4;
   dj = DDid(string("ASWA"), string("CTRA"), GSatID(11), GSatID(14)); DDMap[dj] = 3;
   dj = DDid(string("ASWA"), string("CTRA"), GSatID( 9), GSatID(14)); DDMap[dj] = 2;
   dj = DDid(string("ASWA"), string("CTRA"), GSatID( 3), GSatID(14)); DDMap[dj] = 1;
   map<DDid,int>::const_iterator mit;
   cout << endl << "Here is the map:" << endl;
   for(mit=DDMap.begin(); mit != DDMap.end(); mit++) {
      cout << "  key: " << mit->first << " value: " << mit->second << endl;
   }
   dj = DDid(string("ASWA"), string("CTRA"), GSatID(14), GSatID(25));
   cout << "Try to find " << dj << "   ";
   mit = DDMap.find(dj);
   if(mit == DDMap.end()) cout << " failed!" << endl;
   else  cout << " ok - found " << mit->first << " " << mit->second << endl;

   return 0;
}
