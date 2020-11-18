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
 * @file FileFilterFrame.hpp
 * Wrapper for gpstk::FileSpecFind and gpstk::FileFilter
 */

#ifndef GPSTK_FILEFILTERFRAME_HPP
#define GPSTK_FILEFILTERFRAME_HPP

#include "FileSpec.hpp"
#include "FileFilter.hpp"
#include "FileSpecFind.hpp"
#include "FileUtils.hpp"

namespace gpstk
{
      /// @ingroup FileDirProc
      //@{

      /**
       * This class is a wrapper for FileSpecFind and FileFilter, allowing you
       * to easily pick what files you want to process and how to process
       * them.  In the constructors, providing a start and end time will
       * allow FileSpecFind to pick the correct files for that date range.
       * After the object is built, it is ready for filtering.
       *
       * Use the filter(), sort(), unique() and touch() operators to run
       * whatever operations you want to on the data, then write out the 
       * files when you're done.  You can also get access to the data with
       * getData() and run whatever other post processing tools you would like
       * to.  
       *
       * See the examples in FileFilterFrameTest.cpp for a demonstration.
       */
   template <class FileStream, class FileData>
   class FileFilterFrame : public FileFilter<FileData>
   {
   public:
         /** Default constructor.
          * @throw Exception */
      FileFilterFrame(const gpstk::CommonTime& start = 
                      gpstk::CommonTime::BEGINNING_OF_TIME,
                      const gpstk::CommonTime& end = 
                      gpstk::CommonTime::END_OF_TIME);

         /** Takes a list of files to open in lieu of day times.
          * @throw Exception */
      FileFilterFrame(const std::vector<std::string>& fileList,
                      const gpstk::CommonTime& start = 
                      gpstk::CommonTime::BEGINNING_OF_TIME,
                      const gpstk::CommonTime& end = 
                      gpstk::CommonTime::END_OF_TIME);

         /** Takes a file name for a single file filter.
          * @throw Exception when there's a file error. */
      FileFilterFrame(const std::string& filename, 
                      const gpstk::CommonTime& start = 
                      gpstk::CommonTime::BEGINNING_OF_TIME,
                      const gpstk::CommonTime& end = 
                      gpstk::CommonTime::END_OF_TIME);

         /** Uses the FileSpec to retrieve files.  Use filter like you would
          * in FileSpecFind, to filter FOR stations, receivers, etc.
          * @throw Exception when there's a file error. */
      FileFilterFrame(const FileSpec& spec, 
                      const gpstk::CommonTime& start = 
                      gpstk::CommonTime::BEGINNING_OF_TIME,
                      const gpstk::CommonTime& end = 
                      gpstk::CommonTime::END_OF_TIME,
                      const FileSpecFind::Filter& filter =
                      FileSpecFind::Filter());

         /** Gets the files from the file spec and the time, then adds
          * the data to the filter. Use filter like you would
          * in FileSpecFind, to filter FOR stations, receivers, etc.
          * @throw Exception */
      FileFilterFrame& 
      newSource(const FileSpec& filespec, 
                const gpstk::CommonTime& start = 
                gpstk::CommonTime::BEGINNING_OF_TIME,
                const gpstk::CommonTime& end = 
                gpstk::CommonTime::END_OF_TIME,
                const FileSpecFind::Filter& filter =
                FileSpecFind::Filter());

         /** Reads in the file and adds the data to the filter.
          * @throw Exception */
      FileFilterFrame& 
      newSource(const std::string& filename, 
                const gpstk::CommonTime& start = 
                gpstk::CommonTime::BEGINNING_OF_TIME,
                const gpstk::CommonTime& end = 
                gpstk::CommonTime::END_OF_TIME);

         /** Takes a list of files to open in lieu of day times
          * @throw Exception */
      FileFilterFrame&
      newSource(const std::vector<std::string>& fileList,
                const gpstk::CommonTime& start = 
                gpstk::CommonTime::BEGINNING_OF_TIME,
                const gpstk::CommonTime& end = 
                gpstk::CommonTime::END_OF_TIME);

      virtual ~FileFilterFrame() {}

         /**
          * Writes the data to the file outputFile, truncating the
          * output file if it already exists unless append is true.
          * This can throw an exception when there's a file error.
          * @return true when it works.
          * @warning This will not write out headers for files that need them,
          * and files that depend on header data will (like RINEX) will not
          * be written correctly with this function.  Use 
          * FileFilterFrameWithHeader for those file types.
          * @throw Exception
          */
      bool writeFile(const std::string& outputFile,
                     const bool append = false) const;

         /**
          * Writes the data to the supplied stream.
          * This can throw an exception when there's a file error.
          * @return true when it works.
          * @warning This will not write out headers for files that need them,
          * and files that depend on header data will (like RINEX) will not
          * be written correctly with this function.  Use 
          * FileFilterFrameWithHeader for those file types.
          * @throw Exception
          */
      bool writeFile(FileStream& stream) const;

   protected:
         /**  Run init() to load the data into the filter.
          * @throw Exception */
      void init(const FileSpecFind::Filter& filter = FileSpecFind::Filter());


   protected:   
         /// The file spec for this filter
      FileSpec fs;
         /// the start and end dates for the filter.
      gpstk::CommonTime startTime, endTime;

   };

      //@}

   template <class FileStream, class FileData>
   FileFilterFrame<FileStream,FileData> :: 
   FileFilterFrame(const gpstk::CommonTime& start,
                   const gpstk::CommonTime& end)
         : startTime(start), endTime(end)
   {}

   template <class FileStream, class FileData>
   FileFilterFrame<FileStream,FileData> :: 
   FileFilterFrame(const std::vector<std::string>& fileList,
                   const gpstk::CommonTime& start,
                   const gpstk::CommonTime& end)
         : startTime(start), endTime(end)
   {
      typename std::vector<std::string>::const_iterator itr;
      for (itr = fileList.begin(); itr != fileList.end(); itr++)
      {
         fs.newSpec(*itr);
         init();
      }
   }

   template <class FileStream, class FileData>
   FileFilterFrame<FileStream,FileData> :: 
   FileFilterFrame(const std::string& filename, 
                   const gpstk::CommonTime& start,
                   const gpstk::CommonTime& end)
         : fs(filename), startTime(start), endTime(end)
   {
      init();
   }

   template <class FileStream, class FileData>
   FileFilterFrame<FileStream,FileData> :: 
   FileFilterFrame(const FileSpec& spec, 
                   const gpstk::CommonTime& start,
                   const gpstk::CommonTime& end,
                   const FileSpecFind::Filter& filter)
         : fs(spec), startTime(start), endTime(end)
   {
      init(filter);
   }

   template <class FileStream, class FileData>
   FileFilterFrame<FileStream, FileData>& 
   FileFilterFrame<FileStream,FileData> :: 
   newSource(const FileSpec& filespec, 
             const gpstk::CommonTime& start,
             const gpstk::CommonTime& end,
             const FileSpecFind::Filter& filter)
   {
      startTime = start;
      endTime = end;

      fs = filespec;
      init(filter);
      return *this;
   }

   template <class FileStream, class FileData>
   FileFilterFrame<FileStream, FileData>& 
   FileFilterFrame<FileStream,FileData> :: 
   newSource(const std::string& filename, 
             const gpstk::CommonTime& start,
             const gpstk::CommonTime& end)
   {
      startTime = start;
      endTime = end;
   
      fs.newSpec(filename);
      init();
      return *this;
   }

   template <class FileStream, class FileData>
   FileFilterFrame<FileStream, FileData>& 
   FileFilterFrame<FileStream,FileData> :: 
   newSource(const std::vector<std::string>& fileList, 
             const gpstk::CommonTime& start,
             const gpstk::CommonTime& end)
   {
      startTime = start;
      endTime = end;
   
      typename std::vector<std::string>::const_iterator itr;
      for (itr = fileList.begin(); itr != fileList.end(); itr++)
      {
         fs.newSpec(*itr);
         init();
      }
      return *this;
   }

   template <class FileStream, class FileData>
   void
   FileFilterFrame<FileStream,FileData> :: 
   init(const FileSpecFind::Filter& filter)
   {
         // find the files
      std::list<std::string> listOfFiles = 
         FileSpecFind::find(fs, startTime, endTime, filter);

         // for each file, read it into the filter
      for (const auto& i : listOfFiles)
      {
         FileStream s(i.c_str());

         if (s.good())
         {
            FileData data;
            while (s >> data)
               this->addData(data);
         }
      }
   }

   template <class FileStream, class FileData>
   bool FileFilterFrame<FileStream,FileData> :: 
   writeFile(const std::string& str,
             const bool append) const
   {
      if (!this->dataVec.empty())
      {
            // make the directory (if needed)
         std::string::size_type pos = str.rfind('/');
         if (pos != std::string::npos)
            gpstk::FileUtils::makeDir(str.substr(0,pos).c_str(), 0755);      

         std::ios::openmode mode = std::ios::out;
         if (append)
            mode |= (std::ios::app|std::ios::ate);
      
         FileStream stream(str.c_str(), mode);
         return writeFile(stream);
      }

      return true;
   }

   template <class FileStream, class FileData>
   bool FileFilterFrame<FileStream,FileData> :: 
   writeFile(FileStream& stream) const
   {
      if (!this->dataVec.empty())
      {
         stream.exceptions(std::ios::failbit);
      
         typename std::list<FileData>::const_iterator index;
         for(index = this->dataVec.begin(); index != this->dataVec.end(); index++)
            (*index).putRecord(stream);
      }

      return true;
   }

}  // namespace gpstk

#endif // GPSTK_FILEFILTERFRAME_HPP
