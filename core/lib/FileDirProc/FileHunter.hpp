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
 * @file FileHunter.hpp
 * Find all files matching a specification.
 */

#ifndef GPSTK_FILEHUNTER_HPP
#define GPSTK_FILEHUNTER_HPP

#include "FileSpec.hpp"

namespace gpstk
{
      /// @ingroup FileDirProc
      //@{

      /// The only exception that class FileHunter throws.
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(FileHunterException, gpstk::Exception);

      /** FileHunter is a class designed to find files matching specified
       * criteria.  The specified criteria is threefold:
       * 1.  The File Specification.  This is a path and/or file description
       *  which is the most important search criteria.  For instance, the
       *  specification for a Rinex Obs file might be:
       *    /archive/ADMS%3n/RINEXOBS/S%2n%t%3jA.%2y0
       *  where the '%#char' elements denote fields such as year, day, station
       *  number, and so on.  In this example, FileHunter is smart enough to
       *  search all station directories (ADMS401, ADMS402, etc) but it won't
       *  search any directories (or subdirectories) other than ones that
       *  match the initial specification.
       * 2. Start and end times.  If you simply call find(), FileHunter will
       *  return all files that match the specification.  You can specify
       *  CommonTimes to only return files whose dates (according to the file
       *  name) fall between those times.  Note that you can tell find() to
       *  return the list of files ascending, descending, or unsorted
       *  (unsorted will return them in disk order).
       * 3. Filters.  Filters can be used on any field that your file
       *  specification includes.  The most appropriate use of this is to
       *  filter for a certain set of stations, but it will allow you to
       *  filter ANY field (day, year, etc) that is in the file specification.
       *  
       * For more information about file specifications, see the comments in
       * the FileSpec class.  Also check the file FileSpecTest in the test
       * subdirectory below this one for examples.
       *
       * Limitations:  If a file specification has two or more of the same
       * field defined, only the first one is used.  This is only if they
       * repeat in a single directory or path.  For instance, this would be ok:
       * /ADMS%3n/FILE%3n but in this case: /ADMS%3n/%3j%3j  only the first
       * %3j would be used for the day field.
       * Also, wildcards cannot be used.  Sorry - it's a limitation of how
       * regular expressions are used.
       * Finally, if you specify a version field, all files matching
       * will be returned.  You can either use extractField() on the
       * returned files to determine the latest version or select the
       * version you want returned by using setFilter().
       *
       * @deprecated This class has been scheduled for removal in the
       * first numbered release of Q4 2020.  Please migrate all use of
       * this class to FileSpecFind instead.  New code should not use
       * this class.
       */
   class FileHunter
   {
   public:

         /// This describes how the files to be searched are chunked
         /// i.e. a DAY chunking means one file is written per day
      enum FileChunking
      {
         WEEK,
         DAY,
         HOUR,
         MINUTE
      };
      
         /// Pairs of FileSpecTypes and lists of strings for filtering
         /// the search results.
      typedef std::pair<FileSpec::FileSpecType, std::vector<std::string> >
      FilterPair;

         /** Constructs a FileHunter using a file specification.  This
          * can be a full or relative path and/or a file specification.
          * See FileSpec for details on how to format a file specification.
          * @param[in] filespec File specification string to use
          * @throw FileHunterException when there's a problem with the filespec
          */
      FileHunter(const std::string& filespec);

         /** Constructs a FileHunter using a FileSpec.
          * @param[in] filespec File specification to use
          * @throw FileHunterException when there's a problem with the filespec
          */
      FileHunter(const FileSpec& filespec);

         /** Rather than building a new file hunter, this lets you change
          * the filespec you're searching for.
          * @param[in] filespec File specification string to use
          * @throw FileHunterException when there's a problem with the filespec
          */
      FileHunter& newHunt(const std::string& filespec);

         /** Changes the file spec you're searching for in FileHunter.
          * @param[in] filespec File specification to use
          * @throw FileHunterException when there's an error in the filespec
          */
      FileHunter& newHunt(const FileSpec& filespec)
      { return newHunt(filespec.getSpecString()); }

         /** Filters FOR the strings in \a filter in the field specified
          * by \a fst when searching.
          * @param[in] fst A FileSpecType you want to run a filter on
          * @param[in] filter a list of strings to search for.in the filter
          * @throw FileHunterException when \a fst can't be found.
          */
      FileHunter& setFilter(const FileSpec::FileSpecType fst,
                            const std::vector<std::string>& filter);

         /** Search for files.  Set \a start and \a end
          * for specifying times of files (according to their file names)
          * for returning.
          * \warning This method is NOT MT-Safe.
          * @param[in] start the start time to limit the search
          * @param[in] end the end time to limit the search
          * @param[in] fsst set to change the order the list is returned
          * @param[in] chunk the type of file chunking to use to select files
          * @return a list of files matching the file specification,
          *   start and end times, and filters ordered according to fsst.
          *   This list can be empty if no files are found.
          * @throw FileHunterException when there's a problem searching.
          */
      std::vector<std::string> 
      find(const gpstk::CommonTime& start = gpstk::CommonTime::BEGINNING_OF_TIME,
           const gpstk::CommonTime& end = gpstk::CommonTime::END_OF_TIME,
           const FileSpec::FileSpecSortType fsst = FileSpec::ascending,
           enum FileChunking chunk = DAY) const;

         /** Output the file specification into a semi-readable format
          *
          * @param[in,out] o Output stream
          */
      void dump(std::ostream& o) const;

   private:
         // disallow these
      FileHunter();
      FileHunter(const FileHunter& fh);
      FileHunter& operator=(const FileHunter& fh);

   protected:

         /** Prepare the hunter for searches; share code between
          * the constructor and newHunt()
          *
          * @param[in] filespec File specification for which to search
          * @throw FileHunterException if initialization failed
          */
      void init(const std::string& filespec);

         /** Search for the given file spec fragment in the given directory.
          * \warning This method is NOT MT-Safe.
          *
          * @param[in] directory Directory in which to search
          * @param[in] fs FileSpec File specification fragment
          *    for which to search
          * @param[in] expectDir Whether the returned elements should be
          *    directories based on the complete file specification
          * @return a list of matching directories/files
          * @throw FileHunterException if an error occurred during search
          */
      std::vector<std::string> 
      searchHelper(const std::string& directory,
                   const FileSpec& fs,
                   bool expectDir) const;

         /** If a filter is set, remove items from the specified file list
          * that do not satisfy the filter.
          *
          * @param[in,out] fileList List of files to filter
          * @param[in] fs FileSpec used to locate filterable items
          * @throw FileHunterException if an error occurs during filtering
          */
      void filterHelper(std::vector<std::string>& fileList, 
                        const FileSpec& fs) const;

         /** Attempt to determine a year based on the supplied filename
          * and FileSpec, and then, based on that year and on the specified
          * year limits, decide if the filename should be filtered-out.
          *
          * @param[in] filename Filename to potentially filter
          * @param[in] fs FileSpec against which to filter filename
          * @param[in] minY Minimum valid year
          * @param[in] maxY Maximum valid year
          * @return true if the filename should be filtered, false otherwise
          */
      bool coarseTimeFilter(const std::string& filename,
                            const FileSpec& fs,
                            int minY,
                            int maxY) const;

         /// Holds the broken down list of the file specification for searching
      std::vector<FileSpec> fileSpecList;

         /// Stores the list of things to filter for
      std::vector<FilterPair> filterList;

   }; // class FileHunter

      //@}

} // namespace gpstk

#endif  // GPSTK_FILEHUNTER_HPP
