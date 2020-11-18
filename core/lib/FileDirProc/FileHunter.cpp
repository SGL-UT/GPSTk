//==============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 * @file FileHunter.cpp
 * Find all files matching a specification.
 */

#include "FileHunter.hpp"
#include "YDSTime.hpp"
#include "CivilTime.hpp"
#include "GPSWeekSecond.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;

// headers for directory searching interface
#ifndef _WIN32
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>

#else
#include <io.h>
#include <direct.h>
#define PATH_MAX _MAX_PATH
#endif

namespace gpstk
{

   FileHunter::FileHunter(const string& filespec)
   {
      try
      {
         init(filespec);
      }
      catch (FileHunterException& e)
      {
         GPSTK_RETHROW(e);
      }
   }


   FileHunter::FileHunter(const FileSpec& filespec)
   {
      try
      {
         init(filespec.getSpecString());
      }
      catch (FileHunterException& e)
      {
         GPSTK_RETHROW(e);
      }
   }


   FileHunter& FileHunter::newHunt(const string& filespec)
   {
      try
      {
         init(filespec);
      }
      catch (FileHunterException& e)
      {
         GPSTK_RETHROW(e);
      }
      return *this;
   }


   FileHunter& FileHunter::setFilter(const FileSpec::FileSpecType fst,
                                     const vector<string>& filter)
   {
      std::string  fileSpecType;
      if (filter.empty())
      {
         FileHunterException exc("FileHunter::setFilter with empty filter"
                                 " would result in no matches");
         GPSTK_THROW(exc);
      }
      try
      {     // ensure a valid file spec type
         fileSpecType = FileSpec::convertFileSpecType(fst);
      }
      catch (FileSpecException& fse)
      {
         FileHunterException fhe(fse.getText(), fse.getErrorId());
         GPSTK_THROW(fhe);
      } 
         // try to find the field in the fileSpecList.
      vector<FileSpec>::iterator itr = fileSpecList.begin();
      while (itr != fileSpecList.end())
      {
         if ((*itr).hasField(fst))
            break;
         itr++;
      }
         // found the field - add the filter.
      if (itr != fileSpecList.end())
      {
         filterList.push_back(FilterPair(fst, filter));
      }         
      else  // didn't find it - throw an exception
      {
         FileHunterException fhe("The FileSpec does not have a field: " +
                                 fileSpecType);
         GPSTK_THROW(fhe);
      }
      return *this;
   }


   vector<string> FileHunter::find(const CommonTime& start,
                                   const CommonTime& end,
                                   const FileSpec::FileSpecSortType fsst,
                                   enum FileChunking chunk) const
   {
         // ensure proper time order
      if (end < start)
      {
         FileHunterException fhe("The times are specified incorrectly");
         GPSTK_THROW(fhe);
      }
         // move start time back to a boundary defined by file chunking
      CommonTime exStart;
      switch (chunk)
      {
         case WEEK:
         {
            GPSWeekSecond tmp(start);
            tmp.sow = 0.0;
            exStart = tmp;
            YDSTime yds(end);
            break;
         }
         case DAY:
         {
            YDSTime tmp(start);
            tmp.sod = 0.0;
            exStart = tmp;
            break;
         }
         case HOUR:
         {
            CivilTime tmp(start);
            tmp.minute = 0;
            tmp.second = 0.0;
            exStart = tmp;
            break;
         }
         case MINUTE:
         {
            CivilTime tmp(start);
            tmp.second = 0.0;
            exStart = tmp;
            break;
         }
      }
      exStart.setTimeSystem(start.getTimeSystem());

         // Set min and max years for progressive coarse time filtering
      int minY, maxY;
      YDSTime tmpStart(start);
      YDSTime tmpEnd(end);
      minY = tmpStart.year;
      maxY = tmpEnd.year;

      vector<string> toReturn;
         // Seed the return vector with an empty string which will be
         // appended to with the root directory or drive, depending on
         // your O/S.  This being empty is a termination condition for
         // an inner loop.
      toReturn.push_back(string());
         // complete file spec string, i.e. full path,
         // i.e. aggregation of fileSpecList for final time filtering.
      string fileSpecStr;

      try
      {
         vector<FileSpec>::const_iterator fsIter = fileSpecList.begin();

#ifdef _WIN32
         if (fsIter != fileSpecList.end())
         {
               // If Windows, we should seed it with the drive spec
            toReturn[0] = (*fsIter).getSpecString();
            fileSpecStr = (*fsIter).getSpecString(); 
            fsIter++;
         }
#endif
         while (fsIter != fileSpecList.end())
         {
            vector<string> toReturnTemp;
            vector<FileSpec>::const_iterator next = fsIter;
            next++;
            bool expectDir = (next != fileSpecList.end());

            fileSpecStr += string(1, slash) + fsIter->getSpecString();
            for (size_t i = 0; i < toReturn.size(); i++)
            {
                  // Search for the next entries
               //cerr << "Dir = " << toReturn[i] << endl;
               vector<string> newEntries =
                     searchHelper(toReturn[i], *fsIter, expectDir);

                  // After getting the potential entries, filter
                  // them based on the user criteria
               filterHelper(newEntries, *fsIter);

                  // For each new entry, check the time (if possible)
                  // then add it if it's in the correct time range.
                  // this is why we need to enter an empty string to 
                  // seed toReturn
               vector<string>::const_iterator entryIter = newEntries.begin();
               for ( ; entryIter != newEntries.end(); entryIter++)
               {
                     // To avoid extra processing, immediately attempt
                     // to filter-out new entries whose year is not
                     // within the valid year range
                  if (coarseTimeFilter(*entryIter, *fsIter, minY, maxY))
                  {
                     //cerr << "Filtered out entry: " << *entryIter << endl;
                     continue;
                  }
                  string newPath = toReturn[i] + string(1,slash) + *entryIter;
                  //cerr << "  " << newPath << endl;
                  toReturnTemp.push_back(newPath);
               }
            }

            toReturn = toReturnTemp;

               // If toReturn is ever empty, there are no matches
            if (toReturn.empty())
               return toReturn;

            fsIter = next;
         } // while (itr != fileSpecList.end())

            // Sort the list by the file spec of the last field
         fsIter--;
         (*fsIter).sortList(toReturn, fsst);
      }
      catch (gpstk::Exception& exc)
      {
         FileHunterException nexc(exc);
         GPSTK_THROW(nexc);
      }
         // Filter by fully-determined time
      vector<string> filtered;
      try
      {
         FileSpec fullSpec(fileSpecStr);
         for (unsigned i = 0; i < toReturn.size(); i++)
         {
            CommonTime fileTime = fullSpec.extractCommonTime(toReturn[i]);
            if ((fileTime >= exStart) && (fileTime <= end))
            {
               filtered.push_back(toReturn[i]);
            }
         }
      }
      catch (gpstk::Exception& exc)
      {
         FileHunterException nexc(exc);
         GPSTK_THROW(nexc);
      }
      return filtered;
   }


   void FileHunter::init(const string& filespec)
   {
         // debug
      try
      {
         if (filespec.empty())
         {
            FileHunterException exc("FileHunter: empty file spec is invalid");
            GPSTK_THROW(exc);
         }
         fileSpecList.clear();
         filterList.clear();

         string fs(filespec);
         
            // first, check if the file spec has a leading '/'.  if not
            // prepend the current directory to it.
#ifndef _WIN32
         if (fs[0] != slash)
         {
            char* cwd = getcwd(NULL, PATH_MAX);

            if (cwd == NULL)
            {
               FileHunterException fhe("Cannot get working directory");
               GPSTK_THROW(fhe);
            }
            string wd(cwd);
               // append a trailing slash if needed
            if (wd[wd.size()-1] != slash)
               wd += std::string(1,slash);
            fs.insert(0, wd);
            free(cwd);
         }
            // Append a closing slash so the breakdown algorithm has a
            // means to terminate.
         if (fs[fs.size()-1] != '/') fs += std::string(1,'/');
#else
            // If Windows, then check for leading drive name.
            // If not leading drivename, then prepend current working directory.
         if (fs[1]!=':')
         {
            char* cwdW = _getcwd(NULL, PATH_MAX);
            if (cwdW == NULL)
            {
               FileHunterException fhe("Cannot get working directory");
               GPSTK_THROW(fhe);
            }
            string wdW(cwdW);
            
               // append a trailing slash if needed
            if (wdW[wdW.size()-1] != '\\')
               wdW += std::string(1,'\\');
            fs.insert(0, wdW);
            free(cwdW);
         }
            // Append a closing slash so the breakdown algorithm has a
            // means to terminate.
         if (fs[fs.size()-1] != '\\') fs += std::string(1,'\\');
#endif

            // break down the filespec directory by directory into the
            // storage vector
         while (!fs.empty())
         {
#ifndef _WIN32
            if (fs[0] != slash)
            {
               FileHunterException fhe("Unexpected character: " + 
                                       fs.substr(0,1));
               GPSTK_THROW(fhe);
            }
            else
            {
               // erase the leading slash
               fs.erase(0, 1);
            }
            string::size_type slashpos = fs.find(slash);
            FileSpec tempfs(fs.substr(0, slashpos));

               // debug
            //printf("FileHunter.init():  fs, slashpos, tempfs = '%s', %d, '%s'.\n",
            //   fs.c_str(),(int)slashpos,tempfs.getSpecString().c_str());

            if (slashpos != string::npos)
            {
               fileSpecList.push_back(tempfs);
            }
            fs.erase(0, slashpos);
#else       
               // for Windows erase the leading backslash, if present
            if (fs[0] == '\\') fs.erase(0,1);
            string::size_type slashpos;
            slashpos = fs.find('\\');
            FileSpec tempfs(fs.substr(0, slashpos));
            
            if (slashpos!=string::npos) fileSpecList.push_back(tempfs);
            fs.erase(0, slashpos);
#endif
         }
      }
      catch (FileHunterException &e)
      {
         GPSTK_RETHROW(e);
      }
      catch (FileSpecException &e)
      {
         FileHunterException fhe(e);
         fhe.addText("Error in the file spec");
         GPSTK_THROW(fhe);
      }
      catch (Exception &e)
      {
         FileHunterException fhe(e);
         GPSTK_THROW(fhe);
      }
      catch (std::exception &e)
      {
         FileHunterException fhe("std::exception caught: " + string(e.what()));
         GPSTK_THROW(fhe);
      }
      catch (...)
      {
         FileHunterException fhe("unknown exception caught");
         GPSTK_THROW(fhe);
      }
   } // init


   vector<string> FileHunter::searchHelper(const string& directory,
                                           const FileSpec& fs,
                                           bool expectDir) const
   {
      try
      {
         vector<string> toReturn;

            // generate a search string
         string searchString = fs.createSearchString();
#ifndef _WIN32
            // open the dir
         DIR* theDir;

         //cerr << "In searchHelper() before opendir()" << endl;
          
            // The first clause is a special kludge for Cygwin
            // referencing DOS drive structures
         //if (searchString.compare("cygdrive")==0)
         //{
         //   std::string tempFS =  std::string(1,slash) + searchString;
         //   theDir = opendir(tempFS.c_str());
         //}
         //else

         if (directory.empty())
            theDir = opendir(std::string(1,slash).c_str());
         else
            theDir = opendir(directory.c_str());

         //cerr << "In searchHelper() after opendir()" < endl;
         
         if (theDir == NULL)
         {
            FileHunterException fhe("Cannot open directory: " + directory);
            GPSTK_THROW(fhe);
         }
         
            // get each dir/file entry and compare it to the search string
         struct dirent* entry;
         
         while ( (entry = readdir(theDir)) != NULL)
         {
            string filename(entry->d_name);
            
            //cerr << "Testing '" << filename << "'" << endl;
            
            if ((filename.length() == searchString.length()) &&
                (filename != ".") && (filename != "..") && 
                isLike(filename, searchString, '*', '+', '?'))
            {
                  // Determine if entry is a directory
               bool isDir = false;
               if (entry->d_type == DT_DIR)
               {
                  isDir = true;
               }
               else if ( (entry->d_type == DT_UNKNOWN)
                         || (entry->d_type == DT_LNK))
               {
                  string fullname(directory + slash + filename);
                  struct stat statBuf;
                  int rc = stat(fullname.c_str(), &statBuf);
                  if (0 == rc)
                  {
                     if (S_ISDIR(statBuf.st_mode))
                     {
                        isDir = true;
                     }
                  }
               }
               if (expectDir == isDir)
               {
                  toReturn.push_back(filename);
               }
            }
         }
            // use filespec for extra verification?
         
            // cleanup
         if (closedir(theDir) != 0)
         {
            FileHunterException fhe("Error closing directory: " + 
                                    directory);
            GPSTK_THROW(fhe);
         }
#endif
#ifdef _WIN32
            // say 'hi' to old school MS io
         char* cwd = _getcwd(NULL, PATH_MAX);
         _chdir(directory.c_str());
         
         struct _finddata_t c_file;
         long hFile;
         
         if ( (hFile = _findfirst( searchString.c_str(), &c_file )) != -1 )
         {
            std::string filename(c_file.name);
            bool isDir = (c_file.attrib & _A_SUBDIR);
            if ((filename != ".") && (filename != ".."))
            {
               if (expectDir == isDir)
               {
                  toReturn.push_back(filename);
               }
            }
            while( _findnext( hFile, &c_file ) == 0 )
            {
               isDir = (c_file.attrib & _A_SUBDIR);
               filename = std::string(c_file.name);
               if ((filename != ".") && (filename != ".."))
               {
                  if (expectDir == isDir)
                  {
                     toReturn.push_back(filename);
                  }
               }
            }
         }
         _findclose(hFile);
         _chdir(cwd);
#endif
         return toReturn;
      }
      catch (Exception& e)
      {
         FileHunterException fhe(e);
         fhe.addText("Search failed");
         GPSTK_THROW(fhe);
      }
      catch (std::exception& e)
      {
         FileHunterException fhe("std::exception caught: " + string(e.what()));
         fhe.addText("Search failed");
         GPSTK_THROW(fhe);
      }
      catch (...)
      {
         FileHunterException fhe("unknown exception");
         fhe.addText("Search failed");
         GPSTK_THROW(fhe);         
      }
   }  // searchHelper()


   void FileHunter::filterHelper(vector<std::string>& fileList, 
                                 const FileSpec& fs) const
  {
         // go through the filterList.  If the filespec has
         // any fields to filter, remove matches from fileList

         // for each element in the filter....
      vector<FilterPair>::const_iterator filterItr = filterList.begin();
      while (filterItr != filterList.end())
      {
         try
         {
               // if the file spec has that element...
            if (fs.hasField((*filterItr).first))
            {
                  // then search through the file list and
                  // remove any files that don't match the filter.
               vector<string>::iterator fileListItr = fileList.begin();
               while (fileListItr != fileList.end())
               {
                     // thisField holds the part of the file name
                     // that we're searching for
                  string thisField;
                  thisField = fs.extractField(*fileListItr,
                                              (*filterItr).first);
                  vector<string>::const_iterator filterStringItr =
                        (*filterItr).second.begin();

                     // the iterator searches each element of the filter
                     // and compares it to thisField.  If there's a match
                     // then keep it.  if there's no match, delete it.
                  while (filterStringItr != (*filterItr).second.end())
                  {
                     if (thisField == rightJustify(*filterStringItr,
                                                   thisField.size(),
                                                   '0'))
                     {
                        break;
                     }
                     filterStringItr++;
                  }

                  if (filterStringItr == (*filterItr).second.end())
                     fileListItr = fileList.erase(fileListItr);
                  else
                     fileListItr++;
               }
            }
         }
         catch (FileSpecException& fse)
         {
            FileHunterException fhe(fse);
            GPSTK_THROW(fhe)
         }
         filterItr++;
      }  
   }  // filterHelper()


   bool FileHunter::coarseTimeFilter(
         const string& filename,
         const FileSpec& fs,
         int minY,
         int maxY) const
   {
      try
      {
         TimeTag::IdToValue tags;
         TimeTag::getInfo(filename, fs.getSpecString(), tags);
         TimeTag::IdToValue::const_iterator tagIter = tags.begin();
         for ( ; tagIter != tags.end(); tagIter++)
         {
            switch (tagIter->first)
            {
               case 'Y':
               {
                  long year = asInt(tagIter->second);
                  return ((year < minY) || (year > maxY));
               }
               case 'y':
               {
                  long year = asInt(tagIter->second);
                  if (year < 1970)
                  {
                     year += (year >= 69) ? 1900 : 2000;
                  }
                  return ((year < minY) || (year > maxY));
               }
               //case 'F':  // Full-week filtering might also be nice
            }
         }
      }
      catch (...)
      { }
      return false;
   }


   void FileHunter::dump(ostream& o) const
   {
      vector<FileSpec>::const_iterator itr = fileSpecList.begin();
      while (itr != fileSpecList.end())
      {
         (*itr).dump(o);
         itr++;
      }
   }

} // namespace
