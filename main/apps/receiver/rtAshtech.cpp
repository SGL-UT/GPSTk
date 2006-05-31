#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <list>
#include <map>

#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

#include "DayTime.hpp"
#include "UnixSerialPort.hpp"
#include "Exception.hpp"
#include "CommandOption.hpp"
#include "CommandOptionParser.hpp"
#include "AshtechMessage.hpp"
#include "ScreenControl.hpp"
#include "RollingFile.hpp"
#include "RinexObsData.hpp"
#include "RinexObsStream.hpp"
#include "RinexNavData.hpp"
#include "RinexNavStream.hpp"
#include "RinexObsRoller.hpp"
#include "RinexNavRoller.hpp"

using namespace std;
using namespace gpstk;

bool saveLogMessages=true;
list<string> logList;

RollingFile *logFile   = 0;
RollingFile *matlabObs = 0;

void log(const string& message)
{
   DayTime currentEpoch;
   string entry = currentEpoch.printf(" %02m/%02d/%04Y %02H:%02M:%04.1f - ") +
      message;
   logList.push_back(entry);
   if (saveLogMessages)
   {
      logFile->write(entry+"\n");
   }   
}

void matlabify(const RinexObsData& rod)
{
   string time=rod.time.printf("%F %9.2g ");
   
   RinexObsData::RinexPrnMap::const_iterator i_sat;
   RinexObsData::RinexObsTypeMap::const_iterator i_obs;
   
   for (i_sat=rod.obs.begin(); i_sat!=rod.obs.end(); i_sat++)
   {
      if (matlabObs->write(time, rod.time))
      {
         log("Opened output file: " + 
         matlabObs->getCurrentFilename());
      }

      
      short PRNID = i_sat->first.prn;

      i_obs = i_sat->second.find(RinexObsHeader::C1);      
      double C1 = i_obs->second.data;

      i_obs = i_sat->second.find(RinexObsHeader::P1);      
      double P1 = i_obs->second.data;


      i_obs = i_sat->second.find(RinexObsHeader::P2);      
      double P2 = i_obs->second.data;


      i_obs = i_sat->second.find(RinexObsHeader::L1);      
      double L1 = i_obs->second.data;


      i_obs = i_sat->second.find(RinexObsHeader::L2);      
      double L2 = i_obs->second.data;


      i_obs = i_sat->second.find(RinexObsHeader::D1);      
      double D1 = i_obs->second.data;


      i_obs = i_sat->second.find(RinexObsHeader::D2);      
      double D2 = i_obs->second.data;

      i_obs = i_sat->second.find(RinexObsHeader::S1);      
      double S1 = i_obs->second.data;


      i_obs = i_sat->second.find(RinexObsHeader::S2);      
      double S2 = i_obs->second.data;

      matlabObs->write(StringUtils::asString(PRNID),rod.time);
      matlabObs->write(" ",rod.time);
      matlabObs->write(StringUtils::asString(C1,3),rod.time);
      matlabObs->write(" ",rod.time);
      matlabObs->write(StringUtils::asString(P1,3),rod.time);
      matlabObs->write(" ",rod.time);
      matlabObs->write(StringUtils::asString(P2,3),rod.time);
      matlabObs->write(" ",rod.time);
      matlabObs->write(StringUtils::asString(L1,3),rod.time);
      matlabObs->write(" ",rod.time);
      matlabObs->write(StringUtils::asString(L2,3),rod.time);
      matlabObs->write(" ",rod.time);
      matlabObs->write(StringUtils::asString(D1,3),rod.time);
      matlabObs->write(" ",rod.time);
      matlabObs->write(StringUtils::asString(D2,3),rod.time);
      matlabObs->write(" ",rod.time);
      matlabObs->write(StringUtils::asString(S1,1),rod.time);
      matlabObs->write(" ",rod.time);
      matlabObs->write(StringUtils::asString(S2,1),rod.time);
      matlabObs->write("\n",rod.time);      
   }
   
   return;
}

void printLog(const int num2show=10)
{      
   list<string>::iterator i = logList.end();
   for (int k=0; k<num2show; k++) 
      if (i!=logList.begin()) i--;

   cout << "Log Messages" << endl;
   cout << "-----------------------------------------------------------------------------" << endl;

   int lines2go=num2show;
   for (;i!=logList.end();i++,lines2go--) 
      cout << (*i) << endl;
   while ((lines2go--)>0) cout << endl;
   cout << "-----------------------------------------------------------------------------" << endl;

}

RinexObsHeader defineObsHeader(void)
{

   using namespace std;
   string hdrTemplateFile("rinex.obs.template");
   
   RinexObsHeader hdr;

gpstk::RinexObsStream instrm(hdrTemplateFile.c_str(), ios::in);
   if (instrm >> hdr)
      log("Read obs header template: "+hdrTemplateFile);
   else
      log("Could not use obs header template from "+hdrTemplateFile);
   
   return hdr;
   
}

RinexNavHeader defineNavHeader(void)
{
   using namespace std;
   string hdrTemplateFile("rinex.nav.template");
   
   RinexNavHeader hdr;

   gpstk::RinexNavStream instrm(hdrTemplateFile.c_str(), ios::in);
   if (instrm >> hdr)
      log("Read nav header template: "+hdrTemplateFile);
   else
      log("Could not use nav header template from "+hdrTemplateFile);
   
   return hdr;
}

int main(int argc, char *argv[])
{
   
   try {

      logFile   = new RollingFile("ash%03j%02y.log");

      // Define default variables here *********************************************
      // TO DO: Read default variables from an initialization (say, .INI) file
  
      bool verbose=false;
      bool saveRawMessages=false;
      bool saveMatlabObs=false;
      
      saveLogMessages=true;

      string rawMessageFileSpec("ash%03j%02y.raw");
      string rinexObsFileSpec("site%03j.%02yo");
      string rinexNavFileSpec("site%03j.%02yn");
      string matlabObsFileSpec("obs%03j%02y.txt");
      string serialPort("/dev/ttyS0");
 
      DayTime currentEpoch;
      bool gotGPSEpoch = false;

      // Process user options********************************************************   
 
      CommandOptionNoArg helpOption('h', "help", "Print help usage");
      CommandOptionNoArg verboseOption('v', "verbose", "Increased diagnostic messages");
      CommandOptionNoArg rawOption('r', "raw", "Record raw observations");
      CommandOptionNoArg logOption('l', "log", "Record log entries");
      CommandOptionNoArg matlabOption('t', "text", "Record observations as simple text files");
      CommandOptionWithAnyArg portOption('p', "port", "Serial port to use");
      CommandOptionWithAnyArg rinexObsFileSpecOption('o',"rinex-obs","Naming convention for RINEX obs files");
      CommandOptionWithAnyArg rinexNavFileSpecOption('n',"rinex-nav","Naming convention for RINEX nav message files");
      CommandOptionWithAnyArg textObsFileSpecOption('T',"text-obs","Naming convention for obs in simple text files");

      // TO DO. Limit the number of times the options can be called as appropriate.

      CommandOptionParser cop("Records observations from an Ashtech Z-XII receiver.");
      cop.parseOptions(argc, argv);

      if (helpOption.getCount() || cop.hasErrors())
      {
         if (cop.hasErrors())
            cop.dumpErrors(cout);
         cop.displayUsage(cout);
         exit(0);
      }

      saveRawMessages=(rawOption.getCount()>0);
      saveLogMessages=(logOption.getCount()>0);
      verbose=(verboseOption.getCount()>0);

      if(rinexObsFileSpecOption.getCount()>0)
         rinexObsFileSpec = rinexObsFileSpecOption.getValue()[0];

      if(rinexNavFileSpecOption.getCount()>0)
         rinexNavFileSpec = rinexNavFileSpecOption.getValue()[0];

      saveMatlabObs=(matlabOption.getCount()>0) ||
         (textObsFileSpecOption.getCount()>0) ;

      if(textObsFileSpecOption.getCount()>0)
         matlabObsFileSpec = textObsFileSpecOption.getValue()[0];

      if (portOption.getCount()>0)
         serialPort = portOption.getValue()[0];
      
      RollingFile rawFiles(rawMessageFileSpec);
      RinexObsRoller obsFiles(rinexObsFileSpec, defineObsHeader() );
      RinexNavHeader rinexNavHeader=defineNavHeader();
      RinexNavRoller navFiles(rinexNavFileSpec, rinexNavHeader);
      matlabObs = new RollingFile(matlabObsFileSpec);
      
      // Acquire the port *****************************************************

      UnixSerialPort port(serialPort.c_str());
         //UnixSerialPort port("/dev/ttyS1");

      // Setup the receiver ***************************************************
      log("Requesting iono, trop info");
      port.write("$PASHQ,ION\r\n",12);

      log("Requesting ephemeris from all PRNs in track.");
      port.write("$PASHQ,EPB\r\n",12);


         // Define loop variables *********************************************
      ssize_t readSize=-333;
      const size_t buffSize=1400;
      char buff[buffSize]="";
      const size_t buff2size=400;
      char buff2[buff2size]="";
      string msgBuffer;

      bool readStream=true;
      ssize_t count=0;
      int mnum = 0;
      short currentDoy = currentEpoch.DOY(), lastDoy = -1;
      
         // Queues 
      list<AshtechMessage> obsQ, emptyObsQ; // Observations
      AshtechMessage ionMessage;            // last ION message (iono constants + UTC info)
      bool gotION=false;                    // Have we received an ION message?
      
         // Storage to remember which PRN is on which tracker
      map<int,int> trackerMap, lastTrackerMap, emptyTrackerMap;
      
      bool gotObsData = false;
      int currentSequence = -1;
      long totalCharsRead=0;
      long totalMsgLengthsProcessed=0;

      log("Collection program started");

         // Infinite loop, polling the port **********************************
      while (readStream)
      {
         readSize = port.read(buff, buffSize-1);
         totalCharsRead += readSize;

         msgBuffer.append(buff,readSize);

         // Output to the user *****************************************
         clearScreen();
         printTitle();
         cout << endl;

         // Display channel numbers ************************************
         cout << "Channel  ";
         for (int kk=1; kk<=12; kk++)
            cout << setw(3) << kk << "  ";
         cout << endl << "PRN      ";
         for (int kk=1; kk<=12; kk++)
            if (lastTrackerMap[kk]) 
               cout << setw(3) << lastTrackerMap[kk] << "  ";
            else 
               cout << " --  ";
         cout << endl << endl;                                         
        
         // Display comm status *****************************************
         if (verbose)
         {   
            cout << "Number characters just read on serial port: ";
            cout << readSize;
            cout << endl;

            cout << "Total number of characters ever read:       ";
            cout << totalCharsRead;
            cout << endl;

            cout << "Total length of msgs processed:             ";
            cout << totalMsgLengthsProcessed;
            cout << endl;
         }

         cout << "Number of unprocessed characters in buffer: ";
         cout << msgBuffer.size() << endl;

         cout << "Today's message count:                      ";
         cout << mnum << endl;

         cout << "Number of unproceseed obs:                  ";
         cout << obsQ.size() << endl;
         
         cout << endl;
         
         if (verbose) printLog(15);
         else printLog(10);
         
         int actualReadCount;
         size_t loc;
                 
         /* Remove whole messages from the buffer ***************************/
         while ((loc=msgBuffer.find("$PASHR,",0))!=string::npos)
         {
            string temp=msgBuffer.substr(0,loc);
            totalMsgLengthsProcessed += temp.size();
            
            AshtechMessage msg(temp, 
                               AshtechMessage::STREAM,
                               AshtechMessage::ASCII);
            msgBuffer.erase(0,loc+7);
            mnum++;
            
            // Classify 
            AshtechMessage::ObsType thisType = msg.getType();
                        
            // Possibly write out old data 
            if (msg.isObs())
	    {
               trackerMap[msg.getTracker()]=msg.getPRN();
               
               int thisSequence = msg.getSequence();

                  // Trigger dumping the data to files
               if ((thisSequence !=currentSequence) && gotObsData)
	       {
                  // First write out the RINEX obs data
                  RinexObsData rod;
                  rod = 
                     AshtechMessage::convertToRinexObsData(obsQ,currentEpoch);

                  if (rod.obs.empty())
                     log("Empty obs");
                  
                  if (obsFiles.write(rod,rod.time))
                  {
                     log("Opened output file: " + 
                         obsFiles.getCurrentFilename());
                  }

                     // Second write obs to the MATLAB/Octave matrix format
                  if (saveMatlabObs)
                     matlabify(rod);
 
                  // Last, delete these observations
                  // TODO Find cleaner way to empty these data structures
                  // delete() is likely the answer but it's tricky.
                  obsQ = emptyObsQ;
                  lastTrackerMap = trackerMap;
                  trackerMap = emptyTrackerMap;
               }

	    }

            // Stuff away data for later  
            switch (thisType) 
            {
               case AshtechMessage::PBEN :
                  break;

               case AshtechMessage::MPC  :
               case AshtechMessage::MCA  :
               case AshtechMessage::MCL  :
               case AshtechMessage::MP1  :
               case AshtechMessage::MP2  :
                  obsQ.push_back(msg);
                  currentSequence = msg.getSequence();
                  gotObsData = true;
                  break;

               case AshtechMessage::SNAV :
               case AshtechMessage::EPB  :
                  log("Got ephemeris for PRN " +
                  StringUtils::asString(msg.getPRN()));
                  break;
                  
               case AshtechMessage::SALM :
               case AshtechMessage::ALB  :
                  break;

               case AshtechMessage::ION  :
                  ionMessage = msg;
                  gotION=true;
                  log("Got iono, UTC info from RX");
                     //AshtechMessage::updateNavHeader(msg,rinexNavHeader);
                     //log("Length:"+StringUtils::asString(temp.size()));
                     //navFiles.setRinexNavHeader(rinexNavHeader);
                  break;

            } // End of case

            if (thisType==AshtechMessage::PBEN)
            {
               currentEpoch = msg.getEpoch(currentEpoch);
	       gotGPSEpoch= true;
	    }

            // Trigger the writing of nav data
            if ((thisType==AshtechMessage::EPB)&&(gotION))
            {
	      try{
                  RinexNavData rnd = AshtechMessage::convertToRinexNavData(msg, currentEpoch);
                  log("Converted a nav message");
                  if (navFiles.write(rnd, currentEpoch))
                  {
                    log("Opened output file: " + navFiles.getCurrentFilename());
                  }
                  log("Wrote nav message");
	      }
              catch(...)
	      {
                 log("Error converting nav message.");
              }
	    }
              
            if (saveRawMessages)
            {
               if (rawFiles.write(temp))
               {
                  log("Opened output file: " + rawFiles.getCurrentFilename());
               }
            }
            
         } // Remove each whole message from the buffer

            // Has the day of year rolled over?
         currentDoy = currentEpoch.DOY();
         
         if (lastDoy!=currentDoy)
         {
            mnum=0;
         }

         lastDoy = currentDoy;

         usleep(1*1024);      
      }
      
   }
   catch(const gpstk::Exception& x)
   {
      cerr << "Caught exception! " << endl;
      cerr << x << endl;
   }
   
   exit(0);
   
}
