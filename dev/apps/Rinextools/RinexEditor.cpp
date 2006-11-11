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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

/**
 * @file RinexEditor.cpp
 * Edit Rinex observation files.
 * class REditCmd encapsulates commands passed to the Rinex Editor
 */

//------------------------------------------------------------------------------------
// TD Do better at catching exceptions

//------------------------------------------------------------------------------------
#include <vector>
#include <algorithm>
#include <time.h>

#include "RinexEditor.hpp"

#include "MathBase.hpp"
#include "StringUtils.hpp"
#include "RinexObsStream.hpp"

using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
string RinexEditVersion("3.1 8/29/2006");

//------------------------------------------------------------------------------------
// find the index of first occurance of item t (of type T) in vector<T> v;
// i.e. v[index]=t  Return -1 if t is not found.
template<class T> int index(const std::vector<T> v, const T& t) 
{
   for(int i=0; i<v.size(); i++) {
      if(v[i] == t) return i;
   }
   return -1;
}

//------------------------------------------------------------------------------------
// REditCmd member functions
//------------------------------------------------------------------------------------
// constructor from a string, pass by value to avoid changing original
REditCmd::REditCmd(string s)
{
   type = INVALID;

   //*oflog << "\nREC: raw cmd is " << s << endl;
      // ignore leading '-'s
   while(s.size() && (s[0]=='-' || (s[0]==' '||s[0]=='\t'))) s.erase(0,1);
   if(s.size() < 2) return;

      // separate type and the rest
   string tag=s.substr(0,2);
   field = s.substr(2,s.size()-2);
   //*oflog << "REC: tag and field are " << tag << " " << field << endl;

      // first identify the type
   if(tag=="IF") type = IF;
   else if(tag=="OF") type = OF;
   else if(tag=="ID") type = ID;
   else if(tag=="OD") type = OD;
   else if(tag=="HD") type = HD;
   else if(tag=="AO") type = AO;
   else if(tag=="TN") type = TN;
   else if(tag=="TB") type = TB;
   else if(tag=="TE") type = TE;
   else if(tag=="TT") type = TT;
   else if(tag=="DA") type = DA;
   else if(tag=="DO") type = DO;
   else if(tag=="DS") type = DS;
   else if(tag=="DD") type = DD;
   else if(tag=="SD") type = SD;
   else if(tag=="SS") type = SS;
   else if(tag=="SL") type = SL;
   else if(tag=="BZ") type = BZ;
   else if(tag=="BD") type = BD;
   else if(tag=="BS") type = BS;
   else if(tag=="BL") type = BL;
   //else its not an RE command

      // defaults
   bias = -99.99;
   SV = RinexSatID(33,SatID::systemGPS);
   sign = 0;
   inOT = -1;
   time = DayTime::BEGINNING_OF_TIME;

      // bail if invalid
   if(type==INVALID) return;

      // BZ needs nothing more
   if(type==BZ) return;

      // break field into subfields
   vector<string> subfield;
   string::size_type pos;
   while(field.size() > 0) {
      pos = field.find(",");
      if(pos==string::npos) pos=field.size();
      if(pos==0) subfield.push_back(" ");
      else subfield.push_back(field.substr(0,pos));
      if(pos >= field.size()) break;
      field.erase(0,pos+1);
   };

   //if(REDebug) *oflog << "REC: subfields are:";
   //if(REDebug) for(int i=0; i<subfield.size(); i++) *oflog << " " << subfield[i];
   //if(REDebug) *oflog << endl;

      // TN just needs time spacing
   if(type==TN) {
      bias = StringUtils::asDouble(subfield[0]);
      // validate?
      //if(REDebug) *oflog << "REC: TN returns" << endl;
      return;
   }

      // TT just needs delta time
   if(type==TT) {
      bias = StringUtils::asDouble(subfield[0]);
      // validate?
      //if(REDebug) *oflog << "REC: TT returns" << endl;
      return;
   }

      // get (optional) sign
   if(type==DA || type==DS || type==DD || type==SL || type==BD) {
      if(subfield[0][0]=='+') { sign=+1; subfield[0].erase(0,1); }
      if(subfield[0][0]=='-') { sign=-1; subfield[0].erase(0,1); }
   }
   //if(REDebug) *oflog << "REC: sign is " << sign << endl;
 
      // field = filename, OT, or header info
   if(type==IF || type==OF || type==ID || type==OD || type==HD
         || type==AO || type==DO) {
      field = subfield[0];
      //if(REDebug) *oflog << "REC: field is " << field << endl;
      if(type==HD) {            // inOT = int(first character)
         char c=field[0];
         inOT = int(toupper(c));
         //if(REDebug) *oflog << "REC: HD inOT is " << char(inOT) << endl;
         if(inOT!='F' && inOT!='P' && inOT!='R' && inOT!='O' && inOT!='A' &&
            inOT!='M' && inOT!='N' && inOT!='C' && inOT!='D')
               { type=INVALID; return; }
         field.erase(0,1);
      }
      if(type!=OF || subfield.size()==1) return;
      subfield.erase(subfield.begin());
   }
   else field = string(" ");

      // get an SV
   if(type >= DS) {
      SV.fromString(subfield[0]);
      //if(REDebug) *oflog << "REC: PRN is " << SV << endl;
         // default (incl. error) is GPS
         // let prn==-1 denote 'delete all SV of this system'
      if((type==DS || type==SL) && SV.id == -1) ;   // ok
      else if(SV.system == SatID::systemGPS && (SV.id<=0 || SV.id>32))
         { type=INVALID; return; }
      if(type==DS && subfield.size()==1) return;
      subfield.erase(subfield.begin());
   }

      // get an OT
   if(type >= DD) {
      field = subfield[0];
         // TD have a bool valid(string) function or bool valid(RinexObsType)
      RinexObsHeader::RinexObsType rot=RinexObsHeader::convertObsType(field);
      if(rot.type==string("UN")) { type=INVALID; return; }
      //if(REDebug) *oflog << "REC: processed OT is " << rot.type << endl;
      subfield.erase(subfield.begin());
   }

      // get a time
   if(subfield.size()==2 || subfield.size()==3) {
      time.setGPSfullweek(StringUtils::asInt(subfield[0]),
         StringUtils::asDouble(subfield[1]));
   }
   if(subfield.size()==6 || subfield.size()==7) {
      time.setYMDHMS(StringUtils::asInt(subfield[0]), StringUtils::asInt(subfield[1]),
         StringUtils::asInt(subfield[2]), StringUtils::asInt(subfield[3]),
         StringUtils::asInt(subfield[4]), StringUtils::asDouble(subfield[5]));
   }
   //if(REDebug) *oflog << "REC: time is "
   //<< time.printf("%4Y/%2m/%2d %2H:%2M:%.4f") << endl;
   // test validity?

      // bias
   if(type >= SD) {
      //if(REDebug) *oflog << "REC: bias field is " << subfield.back() << endl;
      bias = StringUtils::asDouble(subfield.back().c_str());
      //if(REDebug) *oflog << "REC: bias is " << bias << endl;
   }

}   // end REditCmd::REditCmd(string)

//------------------------------------------------------------------------------------
REditCmd::~REditCmd(void)
{
}

//------------------------------------------------------------------------------------
void REditCmd::Dump(ostream& os, string msg)
{
   string label[]={"INVALID","IF","OF","ID","OD","HD","TN","TB","TE","TT","AO",
      "DA","DO","DS","DD","SD","SS","SL","BD","BS","BL","BZ" };
   if(msg.size()) os << msg;
   os << " type=" << label[type] << ", sign=" << sign << ", SV="
      << StringUtils::asString(SV)
      << ", inOT=" << inOT
      << ", field=" << field
      << ", bias=" << fixed << setprecision(3) << bias
      << ", time = " << time.printf("%4Y/%2m/%2d %2H:%2M:%.4f") << endl;
}

//------------------------------------------------------------------------------------
// RinexEditor member functions
//------------------------------------------------------------------------------------
RinexEditor::RinexEditor(void)
{
   Decimate = 0.0;
   TimeTol = 0.001;
   BegTime = DayTime::BEGINNING_OF_TIME;
   EndTime = DayTime::END_OF_TIME;
   REVerbose = REDebug = BiasZeroData = FillOptionalHeader = HDDeleteOldComments
      = false;
   Skip = false;
   IVLast = IVInterval = IVTable = false;
   for(int i=0; i<9; i++) ndt[i]=-1;
   oflog = &cout;
}

//------------------------------------------------------------------------------------
RinexEditor::~RinexEditor(void)
{
   Cmds.erase(Cmds.begin(),Cmds.end());
   OneTimeCmds.erase(OneTimeCmds.begin(),OneTimeCmds.end());
   CurrentCmds.erase(CurrentCmds.begin(),CurrentCmds.end());
}

//------------------------------------------------------------------------------------
// Return 0 ok, -1 no input file name, -2 no output file name
int RinexEditor::ParseCommands(void)
{
   bool flag;
   int iret=0;
      // first scan command list for BZ,HDf,TN,TT,TB,TE,IF,OF,ID,OD
   for(int i=0; i<Cmds.size(); i++) {
      switch(Cmds[i].type) {
         case REditCmd::TN:
            Decimate = Cmds[i].bias;
            IVInterval = true;
            //if(REDebug) Cmds[i].Dump(*oflog,string("set TN with this cmd"));
            Cmds[i].type = REditCmd::INVALID;
            break;
         case REditCmd::TT:
            TimeTol = Cmds[i].bias;
            if(REDebug) Cmds[i].Dump(*oflog,string("set TT with this cmd"));
            Cmds[i].type = REditCmd::INVALID;
            break;
         case REditCmd::TB:
            BegTime = Cmds[i].time;
            IVTable = true;
            //if(REDebug) Cmds[i].Dump(*oflog,string("set TB with this cmd"));
            Cmds[i].type = REditCmd::INVALID;
            break;
         case REditCmd::TE:
            EndTime = Cmds[i].time;
            IVLast = IVTable = true;
            //if(REDebug) Cmds[i].Dump(*oflog,string("set TE with this cmd"));
            Cmds[i].type = REditCmd::INVALID;
            break;
         case REditCmd::IF:
            InputFile = Cmds[i].field;
            //if(REDebug) Cmds[i].Dump(*oflog,string("set IF with this cmd"));
            Cmds[i].type = REditCmd::INVALID;
            break;
         case REditCmd::OF:
            if(Cmds[i].time == DayTime::BEGINNING_OF_TIME) {
               OutputFile = Cmds[i].field;
               //if(REDebug) Cmds[i].Dump(*oflog,string("set OF with this cmd"));
               Cmds[i].type = REditCmd::INVALID;
            }
            break;
         case REditCmd::ID:
            InputDir = Cmds[i].field;
            //if(REDebug) Cmds[i].Dump(*oflog,string("set ID with this cmd"));
            Cmds[i].type = REditCmd::INVALID;
            break;
         case REditCmd::OD:
            OutputDir = Cmds[i].field;
            //if(REDebug) Cmds[i].Dump(*oflog,string("set OD with this cmd"));
            Cmds[i].type = REditCmd::INVALID;
            break;
         case REditCmd::BZ:
            BiasZeroData = true;
            //if(REDebug) Cmds[i].Dump(*oflog,string("set BZ with this cmd"));
            Cmds[i].type = REditCmd::INVALID;
            break;
         case REditCmd::HD:
            flag = true;
            switch(Cmds[i].inOT) {
               case int('F'): FillOptionalHeader=true; break;
               case int('D'): HDDeleteOldComments=true; break;
               case int('P'): HDProgram=Cmds[i].field; break;
               case int('R'): HDRunBy=Cmds[i].field; break;
               case int('O'): HDObserver=Cmds[i].field; break;
               case int('A'): HDAgency=Cmds[i].field; break;
               case int('M'): HDMarker=Cmds[i].field; break;
               case int('N'): HDNumber=Cmds[i].field; break;
               case int('C'): HDComments.push_back(Cmds[i].field); break;
               default: flag=false; break;
            }
            if(flag) {
               //if(REDebug) Cmds[i].Dump(*oflog,string("set HD rec with this cmd"));
               Cmds[i].type = REditCmd::INVALID;
            }
            break;
         default: break;
      }
   }

      // require an input file name
   if(InputFile.empty()) iret -= 1;
   else if(!InputDir.empty()) InputFile = InputDir + string("/") + InputFile;

      // now iterate over the list in reverse, deleting INVALID commands.
   deque<REditCmd>::iterator jt,it=Cmds.begin();
   while(it != Cmds.end()) {
      if(it->type == REditCmd::INVALID) {
         //if(REDebug) it->Dump(*oflog,string("Erase this INVALID command:"));
         it = Cmds.erase(it);
      }
      else it++;
   }

      // sort on time
   sort(Cmds.begin(),Cmds.end(),REditCmdLessThan());

      // iterate over the command list, make sure first OF command has no time tag
   it = Cmds.begin();
   if(OutputFile.empty()) {
      while(it != Cmds.end()) {
         if(it->type==REditCmd::OF) {
            if(OutputFile.empty()) {
               OutputFile = it->field;
            //if(REDebug) it->Dump(*oflog,string("Let this command set begin time"));
               BegTime = it->time;
               it->time = DayTime::BEGINNING_OF_TIME;
            }
         }
         else { IVLast=true; break; }
         it++;
      }
   }
   if(OutputFile.empty()) {   // error
      iret -= 2;
   }
   else if(!OutputDir.empty()) OutputFile = OutputDir + string("/") + OutputFile;

   if(iret) return iret;

      // iterate again, ensure that - commands have corresponding +
   deque<REditCmd> newCmds;
   it = Cmds.begin();
   while(it != Cmds.end()) {
      if(it->sign == -1) {
         if(REDebug) it->Dump(*oflog,string("This one needs a +"));
         flag=false;
         if(it != Cmds.begin()) {
            jt = it;
            bool last=((--jt)==Cmds.begin());
            while(1) {
               if(jt->type==it->type && jt->SV==it->SV && jt->field==it->field) {
                  if(REDebug) jt->Dump(*oflog,string("Is this the one ?"));
                  flag = true;
                  break;
               }
               if(last) break;
               last = (--jt==Cmds.begin());
            }
         }
         if(!flag) {
            REditCmd re(*it);
            re.sign = 1;
            re.time = BegTime;
            newCmds.push_back(re);
            if(REDebug) re.Dump(*oflog,string("Add this new command:"));
         }
      }
      it++;
   }

      // add new commands and sort again
   it = newCmds.begin();
   while(it != newCmds.end()) {
      if(REDebug) it->Dump(*oflog,string("this is a new command:"));
      Cmds.push_back(*it);
      it++;
   }
   sort(Cmds.begin(),Cmds.end(),REditCmdLessThan());

   if(REDebug)
      for(it=Cmds.begin(); it != Cmds.end(); it++)
         it->Dump(*oflog,string("final"));

      // have to set the IVTable flag...
   if(!IVTable) for(it=Cmds.begin(); it != Cmds.end(); it++) {
      if(it->type==REditCmd::DS || it->type==REditCmd::DA || it->type==REditCmd::DS ||
         it->type==REditCmd::DO || it->type==REditCmd::AO || it->type==REditCmd::DD)
            { IVTable = true; break; }
   }

   return 0;
}

//------------------------------------------------------------------------------------
// leading -'s are ok
void RinexEditor::AddCommand(string cmd)
{
   REditCmd r(cmd);
   if(r.valid()) Cmds.push_back(r);
}

//------------------------------------------------------------------------------------
// Adds valid commands to C and removes from args; leading -'s are ok
void RinexEditor::AddCommandLine(vector<string>& args)
{
   if(args.size()==0) return;
   //if(REDebug) *oflog << "\nBefore stripping RE cmds, there are (" << args.size()
   //<< ") tokens." << endl;
   vector<string>::iterator it=args.begin();
   while(it != args.end()) {
      REditCmd r(*it);
      if(r.valid()) {
         Cmds.push_back(r);
         //if(REDebug) *oflog << "Erase command " << *it << endl;
         it = args.erase(it);
      }
      else {
         //if(REDebug) *oflog << "Its not an RE command: " << *it << endl;
         it++;
      }
   }
   //if(REDebug) *oflog << "\nAfter stripping RE cmds, tokens (" << args.size()
   //<< ") are:" << endl;
   //if(REDebug) for(unsigned int j=0; j<args.size(); j++) *oflog << args[j] << endl;
}

//------------------------------------------------------------------------------------
// NB does not fill optional records, even when -HDf (EditObs will).
int RinexEditor::EditHeader(RinexObsHeader& RHInput, RinexObsHeader& RHOutput)
{
      // save the input header
   RHIn = RHOutput = RHInput;
      // get the obstypes
   ObsTypes = RHInput.obsTypeList;
      // iterate over the list (in reverse), applying, then deleting, AO, DO
      // and DS<SV> commands
   deque<REditCmd>::iterator it=Cmds.begin();
   while(it != Cmds.end()) {
      if(it->type==REditCmd::AO || it->type==REditCmd::DO) {
         //if(REDebug) it->Dump(*oflog,string("Apply and Erase this AO/DO command:"));
         RinexObsHeader::RinexObsType rot=RinexObsHeader::convertObsType(it->field);
         vector<RinexObsHeader::RinexObsType>::iterator jt;
         jt = find(ObsTypes.begin(),ObsTypes.end(),rot);
         if(jt != ObsTypes.end() && it->type==REditCmd::DO) {
            ObsTypes.erase(jt);
         }
         if(jt == ObsTypes.end() && it->type==REditCmd::AO) {
            ObsTypes.push_back(rot);
         }
         it = Cmds.erase(it);
      }
      else if(it->type==REditCmd::DS
            && it->time==DayTime::BEGINNING_OF_TIME) {
         //if(REDebug) it->Dump(*oflog,string("Apply and Erase this DS command:"));
         if(index(DelSV,it->SV) == -1) DelSV.push_back(it->SV);
         it = Cmds.erase(it);
      }
      else it++;
   }

   RHOutput.obsTypeList = ObsTypes;

      // fill records in output header
   DayTime currtime;
   time_t timer;
   struct tm *tblock;
   timer = time(NULL);
   tblock = localtime(&timer);
   currtime.setYMDHMS(1900+tblock->tm_year,1+tblock->tm_mon,
      tblock->tm_mday,tblock->tm_hour,tblock->tm_min,tblock->tm_sec);
   RHOutput.date = currtime.printf("%04Y/%02m/%02d %02H:%02M:%02S");
   { // figure out system -- anything else will be up to caller
      bool gps,glo,tra,geo;
      if(find(DelSV.begin(),DelSV.end(),RinexSatID(-1,SatID::systemGPS)) != DelSV.end())
         gps=false; else gps=true;
      if(find(DelSV.begin(),DelSV.end(),RinexSatID(-1,SatID::systemGlonass)) != DelSV.end())
         glo=false; else glo=true;
      if(find(DelSV.begin(),DelSV.end(),RinexSatID(-1,SatID::systemTransit)) != DelSV.end())
         tra=false; else tra=true;
      if(find(DelSV.begin(),DelSV.end(),RinexSatID(-1,SatID::systemGeosync)) != DelSV.end())
         geo=false; else geo=true;
      if(!glo && !tra && !geo) RHOutput.system.system = RinexSatID::systemGPS;
      if(!gps && !tra && !geo) RHOutput.system.system = RinexSatID::systemGlonass;
      if(!gps && !glo && !geo) RHOutput.system.system = RinexSatID::systemTransit;
      if(!gps && !glo && !tra) RHOutput.system.system = RinexSatID::systemGeosync;
   }
   if(HDDeleteOldComments) {
      RHOutput.commentList.clear();
      RHOutput.valid ^= RinexObsHeader::commentValid;
   }
   if(!HDProgram.empty()) RHOutput.fileProgram = HDProgram;
   if(!HDRunBy.empty()) RHOutput.fileAgency = HDRunBy;
   if(!HDObserver.empty()) RHOutput.observer = HDObserver;
   if(!HDAgency.empty()) RHOutput.agency = HDAgency;
   if(!HDMarker.empty()) RHOutput.markerName = HDMarker;
   if(!HDNumber.empty()) RHOutput.markerNumber = HDNumber;
   if(HDComments.size()) RHOutput.commentList.insert(RHOutput.commentList.end(),
      HDComments.begin(),HDComments.end());
   RHOutput.commentList.push_back(string("Edited by GPSTK Rinex Editor ver ") +
      RinexEditVersion+string(" on ") + RHOutput.date);
   RHOutput.valid |= RinexObsHeader::commentValid;

      // invalidate header records
   if(IVTable && (RHOutput.valid & RinexObsHeader::numSatsValid))
      RHOutput.valid ^= RinexObsHeader::numSatsValid;
   if(IVTable && (RHOutput.valid & RinexObsHeader::prnObsValid))
      RHOutput.valid ^= RinexObsHeader::prnObsValid;
   if(IVLast && (RHOutput.valid & RinexObsHeader::lastTimeValid))
      RHOutput.valid ^= RinexObsHeader::lastTimeValid;
   if(IVInterval && (RHOutput.valid & RinexObsHeader::intervalValid))
      RHOutput.valid ^= RinexObsHeader::intervalValid;

   RHOut = RHOutput;  // save this header; if(FillOptionalHeader) mod RHOut in EditObs

   return 0;
}

//------------------------------------------------------------------------------------
// will fill header (after writing) when -HDf found.
// Return -2 error
//        -1 quit
//         0 DO NOT write the output obs ROOut
//         1 DO NOT write the output obs ROOut, but close and re-open the output file
//         2 DO write the output obs ROOut
//         3 DO write the output obs ROOut, but first close and re-open output file
int RinexEditor::EditObs(RinexObsData& ROIn, RinexObsData& ROOut)
{
      // check that stored input header is valid...but do only once!
   //if(!RHIn.valid() || !RHOut.valid()) return -2;
   bool NewFile=false;

      // test time limits
      // TD some comment blocks have blank epochs...
   if(ROIn.time-BegTime < -TimeTol) return 0;
   if(ROIn.time-EndTime >  TimeTol) return -1;

      // when embedded comments found, just copy and go on
   if(ROIn.epochFlag != 0 && ROIn.epochFlag != 1) {
      ROOut = ROIn;
      return 2;
   }

      // decimate the data // TD consider clock bias?
   if(Decimate > 0.0) {
         // if BegTime is unset, make it the first of the week
      if(BegTime == DayTime::BEGINNING_OF_TIME)
         BegTime.setGPSfullweek(ROIn.time.GPSfullweek(),0.0);
      double dt=ABS(ROIn.time - BegTime);
      dt -= Decimate*long(0.5+dt/Decimate);
      if(ABS(dt) > TimeTol) return 0;
   }

      // TD allow callback to fill new obs types here...
   
      // scan command list, updating current, onetime command lists,
      // delete-SV list, Skip, NewFile
      // delete the command after processing it
   double dt;
   while(Cmds.size() > 0) {
      dt = Cmds[0].time - ROIn.time;
      if(dt < -TimeTol || ABS(dt) < TimeTol) {  // commands in present and past
         if(REDebug) Cmds[0].Dump(*oflog,
               Cmds[0].time.printf("%4Y/%2m/%2d %2H:%2M:%.4f")
               + string(": Process (now) : "));
         switch(Cmds[0].type) {
            case REditCmd::DA:
               if(Cmds[0].sign > 0) Skip=true;
               if(Cmds[0].sign < 0) Skip=false;
               break;
            case REditCmd::OF:
               OutputFile = Cmds[0].field;
               if(!OutputDir.empty()) OutputFile = OutputDir + string("/")
                  + OutputFile;
               NewFile = true;
               break;
            case REditCmd::DS:
               if(Cmds[0].sign > 0 && index(DelSV,Cmds[0].SV) == -1)
                  DelSV.push_back(Cmds[0].SV);
               if(Cmds[0].sign < 0)
                  DelSV.erase(find(DelSV.begin(),DelSV.end(),Cmds[0].SV));
               if(Cmds[0].sign == 0 && ABS(dt) < TimeTol)
                  OneTimeCmds.push_back(Cmds[0]);
               if(Cmds[0].sign != 0 && REDebug) {
                  *oflog << "DS: DelSV is";
                  for(int itemp=0; itemp<DelSV.size(); itemp++)
                     *oflog << " " << DelSV[itemp];
                  *oflog << endl;
               }
               break;
            case REditCmd::DD:
            case REditCmd::SS:
            case REditCmd::SL:
            case REditCmd::SD:
            case REditCmd::BD:
               if(Cmds[0].sign > 0) CurrentCmds.push_back(Cmds[0]);
               if(Cmds[0].sign < 0) CurrentCmds.erase(find(CurrentCmds.begin(),
                   CurrentCmds.end(),Cmds[0]));
               if(Cmds[0].sign == 0 && ABS(dt) < TimeTol)
                  OneTimeCmds.push_back(Cmds[0]);
               break;
            default:
               if(REDebug) Cmds[0].Dump(*oflog,
                     Cmds[0].time.printf("%4Y/%2m/%2d %2H:%2M:%.4f")
                     + string(": This command not implemented! : "));
               break;
         }   // end switch(type)

            // delete this command
         if(REDebug) Cmds[0].Dump(*oflog,
               Cmds[0].time.printf("%4Y/%2m/%2d %2H:%2M:%.4f")
               + string(": Delete (old) : "));
         Cmds.pop_front();
      }
      else break;              // this command (and all others) is in future
   }

      // clear out anything old
   ROOut.obs.clear();

      // if not writing out, return here
   if(Skip && !NewFile) return 0;
   if(Skip && NewFile) return 1;

      // copy data over to new obs structure
   RinexObsData::RinexDatum datum;                       // place holder and zero
   datum.data = 0.0;
   datum.lli = datum.ssi = 0;
   RinexObsData::RinexObsTypeMap otmap;           // place holder for ROOut.obs.second

   for(int j=0; j<RHOut.obsTypeList.size(); j++)  // loop over obstypes (out) in otmap
      otmap.insert(std::map<RinexObsHeader::RinexObsType,
         RinexObsData::RinexDatum>::value_type(RHOut.obsTypeList[j],datum) );

      // loop over prns, create otmap and then insert it with the correct prn
   int nsvs=0;
   RinexObsData::RinexSatMap::iterator it;
   RinexObsData::RinexObsTypeMap::iterator jt,kt;
   for(it=ROIn.obs.begin(); it != ROIn.obs.end(); ++it) {
      // loop over prn=it->first, ObsTypeMap=it->second
      if(find(DelSV.begin(),DelSV.end(),it->first) != DelSV.end()) {
         if(REDebug) *oflog << "Deleted sat " << it->first
            << " at " << ROIn.time << endl;
         continue;
      }
      RinexSatID p(-1,it->first.system);
      if(find(DelSV.begin(),DelSV.end(),p) != DelSV.end()) continue;
      for(int j=0; j<RHOut.obsTypeList.size(); j++) { // loop over obstypes
         jt = otmap.find(RHOut.obsTypeList[j]);  // jt points to ObsTypeMap output
         kt = it->second.find(RHOut.obsTypeList[j]);  // kt points to ObsTypeMap input
         if(kt==it->second.end())                        // not found
            jt->second = datum;
         else
            jt->second = kt->second;
      }
      // TD should test for all zero data -> delete this SV.
      ROOut.obs.insert(std::map<RinexSatID,
         RinexObsData::RinexObsTypeMap>::value_type(it->first,otmap) );
   }

   ROOut.time = ROIn.time;
   if(!NewFile) {
      PrevEpoch = CurrEpoch;
      CurrEpoch = ROOut.time;
   }
   ROOut.clockOffset = ROIn.clockOffset;
   ROOut.epochFlag = ROIn.epochFlag;

      // apply current commands
   vector<REditCmd>::iterator cit;
   RinexObsData::RinexSatMap::reverse_iterator roit;
   for(cit=CurrentCmds.begin(); cit != CurrentCmds.end(); cit++) {
      if(REDebug) cit->Dump(*oflog,string("Current : "));
         // modify all satellites in this system
      if(cit->type == REditCmd::SL && cit->SV.id == -1) {
         for(roit=ROOut.obs.rbegin(); roit!=ROOut.obs.rend(); roit++) {
            if(cit->SV.system == roit->first.system) {
               jt = (roit->second).find(RinexObsHeader::convertObsType(cit->field));
               if(jt != roit->second.end()) jt->second.lli = int(cit->bias);
            }
         }
      }
      it = ROOut.obs.find(cit->SV);
      if(it != ROOut.obs.end()) {
         jt = (it->second).find(RinexObsHeader::convertObsType(cit->field));
         if(jt != it->second.end()) {
            if(cit->type == REditCmd::DD) jt->second.data = 0.0;
            if(cit->type == REditCmd::BD) {
               if(BiasZeroData || ABS(jt->second.data) > 0.001)
                  jt->second.data += cit->bias;
            }
            if(cit->type == REditCmd::SL) jt->second.lli = int(cit->bias);
         }
      }
   }

      // apply one-time commands
   vector<REditCmd>::reverse_iterator irt;
   for(irt=OneTimeCmds.rbegin(); irt != OneTimeCmds.rend(); irt++) {
      if(REDebug) irt->Dump(*oflog,string("1-time : "));
         // delete all satellites in this system
      if((irt->type == REditCmd::DS || irt->type == REditCmd::SL)
            && irt->SV.id == -1) {
         for(roit=ROOut.obs.rbegin(); roit!=ROOut.obs.rend(); roit++) {
            if(irt->SV.system == roit->first.system) {
               if(irt->type == REditCmd::DS) ROOut.obs.erase(roit->first);
               if(irt->type == REditCmd::SL) {
                  jt=(roit->second).find(RinexObsHeader::convertObsType(irt->field));
                  if(jt != roit->second.end()) jt->second.lli = int(irt->bias);
               }
            }
         }
      }
      else {
         it = ROOut.obs.find(irt->SV);
         if(it != ROOut.obs.end()) {
            if(irt->type == REditCmd::DS) {                   // delete SV altogether
               ROOut.obs.erase(it);
            }
            else {
               jt = (it->second).find(RinexObsHeader::convertObsType(irt->field));
               if(jt != it->second.end()) {
                  if(irt->type == REditCmd::DD) jt->second.data = 0.0;
                  if(irt->type == REditCmd::SD) jt->second.data = irt->bias;
                  if(irt->type == REditCmd::SS) jt->second.ssi = int(irt->bias);
                  if(irt->type == REditCmd::SL) jt->second.lli = int(irt->bias);
                  if(irt->type == REditCmd::BD) {
                     if(BiasZeroData || ABS(jt->second.data) > 0.001)
                        jt->second.data += irt->bias;
                  }
                  if(irt->type == REditCmd::BS) jt->second.ssi += int(irt->bias);
                  if(irt->type == REditCmd::BL) jt->second.lli += int(irt->bias);
               }
            }
         }
      }
      OneTimeCmds.pop_back();
   }

   ROOut.numSvs = ROOut.obs.size();

      // update estimate of dt
   if(FillOptionalHeader) {
      if(PrevEpoch.year() != 1) {
         dt = CurrEpoch-PrevEpoch;
         for(int i=0; i<9; i++) {
            if(ndt[i] <= 0) { bestdt[i]=dt; ndt[i]=1; break; }
            if(ABS(dt-bestdt[i]) < 0.0001) { ndt[i]++; break; }
            if(i == 8) {
               int k = 0;
               int nleast=ndt[k];
               for(int j=1; j<9; j++) if(ndt[j] <= nleast) {
                  k=j; nleast=ndt[j];
               }
               ndt[k]=1; bestdt[k]=dt;
            }
         }
      }
   }

   if(NewFile) return 3;
   return 2;
}

//------------------------------------------------------------------------------------
string GetTempFileName(void)
{
#ifdef _MSC_VER
   char newname[L_tmpnam];
   if(!tmpnam(newname)) {
#else
   char newname[]="RETemp.XXXXXX";
   if(mkstemp(newname)==-1) {
#endif
      return string("");
   }
   return string(newname);
}

//------------------------------------------------------------------------------------
// assumes TempFile has been written with RHOut, and info is in config
// Return 0 or -1 if could not open/delete files
int RinexEditor::FillHeaderAndReplaceFile(string& TempFile, string& TrueOutputFile)
{
   int i,j;
      // compute interval
   for(i=1,j=0; i<9; i++) if(ndt[i]>ndt[j]) j=i;
   double dt = bestdt[j];
      // modify the header
   RHOut.version = 2.1; RHOut.valid |= RinexObsHeader::versionValid;
   RHOut.interval = dt; RHOut.valid |= RinexObsHeader::intervalValid;
   RHOut.lastObs = CurrEpoch; RHOut.valid |= RinexObsHeader::lastTimeValid;
      // now the table
   RHOut.numSVs = table.size(); RHOut.valid |= RinexObsHeader::numSatsValid;
   RHOut.numObsForSat.clear();
   vector<TableData>::iterator tit;
   for(tit=table.begin(); tit!=table.end(); ++tit) {
      RHOut.numObsForSat.insert(map<RinexSatID,
            vector<int> >::value_type(tit->prn,tit->nobs));
   }
   RHOut.valid |= RinexObsHeader::prnObsValid;

      // callback
   i = BeforeWritingFilledHeader(RHOut);
   if(i) return -2;

      // here you need to validate the RHOut header

      // now re-open the file and replace the header
   RinexObsHeader rhjunk;
   RinexObsStream ROutStr(TrueOutputFile.c_str(), ios::out);
   RinexObsStream InAgain(TempFile.c_str());
   InAgain.exceptions(ios::failbit);

//*oflog << "Opened " << TempFile << " for input and " << TrueOutputFile << " for output.\n";
   InAgain >> rhjunk;
   ROutStr << RHOut;

   RinexObsData robs;
   while(InAgain >> robs) {
      if(robs.time < BegTime) continue;
      if(robs.time > EndTime) break;
      ROutStr << robs;
   }
   InAgain.close();
   ROutStr.close();

      // delete the temporary
   if( remove(TempFile.c_str()) != 0) {
      //*oflog << "Error: Could not remove existing file: " << TempFile << endl;
      return -1;
   }
   
   return 0;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// Return -1 failed to open file
//        -2 failed to read input file correctly
//        -3 input file not obs
//        -4 failed to fill header and replace original file
//        -5 could not create temporary file
//        -6 callback to BeforeEditHeader returned error
//        -7 callback to AfterEditHeader returned error
//        -8 callback to BeforeEditObs returned error
//        -9 callback to BeforeWritingHeader returned error
//       -10 callback to BeforeWritingObs returned error
// will replace header after filling using temp file
int RinexEditor::EditFile(void)
{
   int iret,Noutput;
   RinexObsHeader rhin,rhout;
   RinexObsData roin,roout;
   string TrueOutputFile,TempFile;

   if(REVerbose) *oflog << "EditFile: Reading " << InputFile
      << ", and Writing " << OutputFile << endl;

      // open input file
   RinexObsStream ROFin(InputFile.c_str());
   if(!ROFin) {
      if(REVerbose) *oflog << "RinexEditor::EditFile could not open input file "
         << InputFile << endl;
      cerr << "RinexEditor::EditFile could not open input file " << InputFile << endl;
      if(REVerbose) *oflog << "RinexEditor::EditFile could not open input file "
         << InputFile << endl;
      return -1;
   }
   ROFin.exceptions(ios::failbit);
   if(REDebug) *oflog << "Opened input file " << InputFile << endl;

      // read header
   try {
      ROFin >> rhin;
   }
   catch(gpstk::FFStreamError& e) {
      cerr << "Caught an FFStreamError while reading header:\n" << e.getText(0)
         << endl;
      if(REVerbose) *oflog << "Caught an FFStreamError while reading header:\n"
         << e.getText(0) << endl;
      return -2;
   }
   catch(gpstk::Exception& e) {
      cerr << "Caught an exception while reading header:\n" << e.getText(0) << endl;
      if(REVerbose) *oflog << "Caught an exception while reading header:\n"
         << e.getText(0) << endl;
      return -2;
   }
   if(REDebug) *oflog << "Read input header" << endl;

      // check its type
   if(rhin.fileType[0] != 'O') {
      cerr << "Input file type is not Observation\n" << endl;
      if(REVerbose) *oflog << "Input file type is not Observation\n" << endl;
      return -3;
   }
   if(REVerbose) {
      *oflog << "Input header:\n";
      rhin.dump(*oflog);
   }

      // callback before editing input header
   iret = BeforeEditHeader(rhin);
   if(iret) return -6;

      // edit header
   EditHeader(rhin,rhout);
   if(REVerbose) *oflog << "Edit header done" << endl;

      // callback after calling EditHeader (pass output header)
   iret = AfterEditHeader(rhout);
   if(iret) return -7;

      // if header is to be filled, write to a temporary file
   TrueOutputFile = OutputFile;
   if(FillOptionalHeader) {
//*oflog << "FillOptionalHeader" << endl;
      OutputFile = GetTempFileName();
//*oflog << "Got temp file name " << OutputFile << endl;
      if(OutputFile.empty()) {
         cerr << "Could not create temporary file name - abort\n";
         if(REVerbose) *oflog << "Could not create temporary file name - abort\n";
         return -5;
      }
      if(!OutputDir.empty()) OutputFile = OutputDir + string("/") + OutputFile;
      TempFile = OutputFile;
   }

      // open output file
//*oflog << "Open output file " << OutputFile << endl;
   RinexObsStream ROFout(OutputFile.c_str(), ios::out);
   if(!ROFout) {
      cerr << "RinexEditor::EditFile could not open output file "
         << OutputFile << endl;
      if(REVerbose) *oflog << "RinexEditor::EditFile could not open output file "
         << OutputFile << endl;
      return -1;
   }
   ROFout.exceptions(ios::failbit);

      // loop over epochs, reading input and writing to output
   Noutput = 0;
//*oflog << "Start loop over output epochs" << endl;
   while (1) {

         // read next observation epoch
      try {
         ROFin >> roin;
      }
      catch(gpstk::FFStreamError& e) {
         cerr << "RinexEditor::EditFile caught an FFStreamError while reading obs:\n"
            << e << endl;
         if(REVerbose) *oflog
            << "RinexEditor::EditFile caught an FFStreamError while reading obs:\n"
               << e << endl;
         return -2;
      }
      catch(gpstk::Exception& e) {
         cerr << "RinexEditor::EditFile caught an exception while reading obs:\n"
            << e << endl;
         if(REVerbose) *oflog
            << "RinexEditor::EditFile caught an exception while reading obs:\n"
               << e << endl;
         return -2;
      }
      if(!ROFin) iret = -1;
      else {
         if(REDebug) {
            *oflog << "Epoch: " << roin.time << ", Flag " << roin.epochFlag
               << ", clk " << roin.clockOffset << endl;
            roin.dump(*oflog);
         }

         // callback after reading input obs
         // and before calling EditObs (pass input obs)
         iret = BeforeEditObs(roin);
         if(iret) { iret=-8; break; }

         iret = EditObs(roin,roout);
         // Return -2 error
         //        -1 quit (EOF or time limit reached)
         //         0 DO NOT write the output obs ROOut
         //         1 DO NOT write the output obs ROOut,
         //            but close and re-open the output file
         //         2 DO write the output obs ROOut
         //         3 DO write the output obs ROOut,
         //            but first close and re-open the output file
      }
      if(REDebug) {
         *oflog << "EditObs returned " << iret << endl;
         roout.dump(*oflog);
      }

      if(iret == -2) break;                           // error => abort

      if(iret == -1 || iret == 1 || iret == 3) {      // new output file
            // close this output file
         ROFout.close();
            // fill the optional header records
         if(FillOptionalHeader) {
            if(Noutput > 0) {
               if(FillHeaderAndReplaceFile(TempFile,TrueOutputFile) != 0) {
                  cerr << "Failed to fill header and replace file - abort\n";
                  if(REVerbose)
                     *oflog << "Failed to fill header and replace file - abort\n";
                  return -4;
               }
               else if(REVerbose) *oflog << "Added header to " << TempFile
                  << " and put in " << TrueOutputFile << endl;
            }

            if(iret != -1) {        // not EOF => going on to another file
               TrueOutputFile = OutputFile;
               OutputFile = GetTempFileName();
               if(OutputFile.empty()) {
                  cerr << "Could not create temporary file name - abort\n";
                  if(REVerbose)
                     *oflog << "Could not create temporary file name - abort\n";
                  return -5;
               }
               if(!OutputDir.empty())
                  OutputFile = OutputDir + string("/") + OutputFile;
               TempFile = OutputFile;
               if(REVerbose) *oflog << "New temp file is " << TempFile
                  << ", and true output file is " << TrueOutputFile << endl;
            }

         }  // end if FillOptionalHeader
         else {
            TrueOutputFile = OutputFile;
         }

         if(iret == -1) {                  // quit
            if(REVerbose)
               *oflog << "Finished processing obs file " << InputFile << endl;
            iret = 0;
            break;
         }

            // open the new output file
         ROFout.open(OutputFile.c_str(), ios::out);
         Noutput = 0;
         if(REVerbose) *oflog << "New output file " << TrueOutputFile
            << " (really " << OutputFile << ") at time " << roin.time << endl;

      }  // end if new output file

         // write to output
      if(iret > 1) {                // not EOF nor error
         if(Noutput == 0) {
            rhout.firstObs = roout.time;
            // callback before writing out header (pass output header)
            iret =  BeforeWritingHeader(rhout);
            if(iret) return -9;

            ROFout << rhout;
            if(REVerbose) {
               *oflog << "Dump output header (iret is " << iret << "):\n";
               rhout.dump(*oflog);
            }
               // prepare for next file
            RHOut = rhout;
            table.clear();
            for(int i=0; i<9; i++) ndt[i]=-1;
         }
   
         // callback just before writing output obs (pass reference to output obs)
         // return value of BeforeWritingObs determines what is written:
         // if return <0 abort
         //            0 write nothing
         //            1 write the obs data structure (note that the caller may set
         //               roout.epochFlag to determine what is output : 0,1 are data,
         //               while 2,3,4 or 5, is for in-line header auxHeader only)
         //           >1 write BOTH header data (in auxHeader, setting
         //               epochFlag=return) AND obs data
         roout.auxHeader.clear();
         iret = BeforeWritingObs(roout);
         if(iret < 0) return -10;
         if(iret > 1) {             // write auxiliary header info first
            int flag=roout.epochFlag, nsvs=roout.numSvs;
            roout.epochFlag = iret;
            roout.numSvs = roout.auxHeader.NumberHeaderRecordsToBeWritten();
               // write out the header records
            ROFout << roout;
            Noutput++;
               // prepare to write obs
            roout.epochFlag = flag;
            roout.numSvs = nsvs;
         }

            // add count of valid obs to table for header
            // -- have to do it here b/c BeforeWritingObs has just filled it
         if(FillOptionalHeader) {
            int k,n=RHOut.obsTypeList.size();
            RinexObsData::RinexSatMap::const_iterator pit;
            RinexObsData::RinexObsTypeMap::const_iterator pjt;
            for(pit=roout.obs.begin(); pit != roout.obs.end(); ++pit) {
               vector<TableData>::iterator ptab;
               ptab = find(table.begin(),table.end(),TableData(pit->first,n));
               if(ptab == table.end()) {
                  table.push_back(TableData(pit->first,n));
                  ptab = find(table.begin(),table.end(),TableData(pit->first,n));
               }
               for(pjt=pit->second.begin(); pjt!=pit->second.end(); pjt++) {
                  for(k=0; k<n; k++) if(RHOut.obsTypeList[k] == pjt->first) break;
                  if(pjt->second.data != 0.0) ptab->nobs[k]++;
               }
            }
         }

         // now write out the obs
         if(REDebug) {
            *oflog << "Write this obs to output:\n";
            roout.dump(*oflog);
         }
         ROFout << roout;
         Noutput++;
      }

   }   // end while loop over epochs

   return iret;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
void DisplayRinexEditUsage(ostream& os)
{
   os << "\nRinex Editor commands:\n";
   os << " ==============================================================="
      << "===============\n";
   os << " Commands begin with a '-' or '/', followed by an identifier, "
      << "then data fields.\n";
   os << " Fields beyond the initial 2- or 3-character identifier are comma "
      << "delimited.\n";
   os << " <SV> gives a satellite; SV=<PRN><System(optional)> "
      << "eg. 19G or 19 = PRN 19 GPS.\n";
   os << " <System> is a single character (G=GPS, R=GLONASS, T=Transit, "
      << "S=Geosynchronous).\n";
   os << " <OT> gives a Rinex observation type, e.g. L1 or P2 (case sensitive).\n";
   os << " <time> gives a time; time=<week,sow> OR "
      << "time=<year,mon,day,hour,min,second>.\n";
   os << "\n";
   os << " File I/O:\n";
   os << " =========\n";
   os << " -IF<file>       Input Rinex observation file name (required)\n";
   os << " -ID<dir>        Directory in which to find input file\n";
   os << " -OF<file>       Output Rinex file name (required, or -OF<file>,<time>)\n";
   os << " -OF<f>,<time>   At time=<time>, close output file and open another "
      << "named <f>\n";
   os << " -OD<dir>        Directory in which to put output file(s)\n";
   os << "\n";
   os << " Output Rinex header fields:\n";
   os << " ===========================\n";
   os << " -HDf            If present, fill optional records in the output Rinex "
      << "header\n";
   os << "                  (NB EditObs() and EditFile() will do this, but NOT "
      << "EditHeader().)\n";
   os << " -HDp<program>   Set output Rinex header 'program' field\n";
   os << " -HDr<run_by>    Set output Rinex header 'run by' field\n";
   os << " -HDo<observer>  Set output Rinex header 'observer' field\n";
   os << " -HDa<agency>    Set output Rinex header 'agency' field\n";
   os << " -HDm<marker>    Set output Rinex header 'marker' field\n";
   os << " -HDn<number>    Set output Rinex header 'number' field\n";
   os << " -HDc<comment>   Add comment to output Rinex header "
      << "(more than one allowed).\n";
   os << " -HDdc           Delete all comments in output Rinex header\n";
   os << "     (NB -HDdc cannot delete comments created by *subsequent* "
      << "-HDc commands)\n";
   os << "\n";
   os << " -AO<OT>         Add observation type OT to header and observation data\n";
   os << "\n";
   os << " General edit commands:\n";
   os << " ======================\n";
   os << " -TB<time>       Begin time: reject data before this time "
      << "(also used for decimation)\n";
   os << " -TE<time>       End   time: reject data after this time\n";
   os << " -TT<dt>         Tolerance in comparing times, in seconds (default=1ms)\n";
   os << " -TN<dt>         Decimate data to epochs = Begin + integer*dt "
      << "(within tolerance)\n";
   os << "\n";
   os << " Specific edit commands:\n";
   os << " =======================\n";
   os << " (Generally each '+' command (e.g DA+,<time>) has a corresponding "
      << "'-' command,\n";
   os << "  and vice-versa; if not, End-of-file or Begin-of-file is assumed.\n";
   os << "  Note commands at one time are applied AFTER other commands of the "
      << "same type.)\n";
   os << "\n";
   os << "     Delete commands:\n";
   os << " -DA+<time>      Delete all data beginning at this time\n";
   os << " -DA-<time>      Stop deleting data at this time\n";
   os << " -DO<OT>         Delete observation type OT entirely "
      << "(including in header)\n";
   os << " -DS<SV>         Delete all data for satellite SV entirely "
      << "(SV may be system only)\n";
   os << " -DS<SV>,<time>  Delete all data for satellite SV at this single time "
      << "(only)\n";
   os << " -DS+<SV>,<time> Delete all data for satellite SV beginning at this time\n";
   os << " -DS-<SV>,<time> Stop deleting all data for satellite SV at this time\n";
   os << "     (NB DS commands with SV=system (only) delete all satellites of that "
      << "system.)\n";
   os << " -DD<SV,OT,t>    Delete a single Rinex data(SV,OT,t) at time <t>\n";
   os << " -DD+<SV,OT,t>   Delete all (SV,OT) data, beginning at time <t>\n";
   os << " -DD-<SV,OT,t>   Stop deleting all (SV,OT) data at time <t>\n";
   os << "     (NB deleting data for one OT means setting it to zero - here and in "
      << "Rinex)\n";
   os << "\n";
   os << "     Set commands:\n";
   os << " -SD<SV,OT,t,d>  Set data(SV,OT,t) to <d> at time <t>\n";
   os << " -SS<SV,OT,t,s>  Set ssi(SV,OT,t) to <s> at time <t>\n";
   os << " -SL+<SV,OT,t,l> Set all lli(SV,OT,t) to <l> at time <t>\n";
   os << " -SL-<SV,OT,t,l> Stop setting lli(SV,OT,t) to <l> at time <t>"
      << " (',<l>' is optional)\n";
   os << " -SL<SV,OT,t,l>  Set lli(SV,OT,t) to <l> at the single time <t> (only)\n";
   os << "     (NB SL commands with SV=system (only) modify all satellites of that "
      << "system.)\n";
   os << "\n";
   os << "     Bias commands:\n";
   os << "   (NB. BD commands apply only when data is non-zero, "
      << "unless -BZ appears)\n";
   os << " -BZ             Apply bias data commands (BD) even when data is zero\n";
   os << " -BD<SV,OT,t,d>  Add the value of <d> to data(SV,OT,t) at time <t>\n";
   os << " -BD+<SV,OT,t,d> Add value of <d> to data(SV,OT) beginning at time <t>\n";
   os << " -BD-<SV,OT,t,d> Stop adding <d> to data(SV,OT) at time <t> (',<d>' optional)\n";
   os << " -BS<SV,OT,t,s>  Add the value of <s> to ssi(SV,OT,t) at time <t>\n";
   os << " -BL<SV,OT,t,l>  Add the value of <l> to lli(SV,OT,t) at time <t>\n";
   os << "\n End of Rinex Editor commands.\n";
   os << " ====================================================================="
      << "=========\n";
   os << endl;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
