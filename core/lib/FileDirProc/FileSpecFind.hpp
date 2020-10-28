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

#ifndef FILESPECFIND_HPP
#define FILESPECFIND_HPP

#include <list>
#include <string>
#include "CommonTime.hpp"
#include "FileSpec.hpp"

class FileSpecFind_T;

namespace gpstk
{
      /** Find existing files matching specified criteria.
       *
       *   \li FileSpecFind returns files matching [start, end).
       *   \li FileSpecFind is a utility class not requiring instantiation.
       *   \li FileSpecFind works in the context of automounter.
       *   \li FileSpecFind works with relative paths.
       *   \li FileSpecFind is designed to be fairly fast.
       *
       * Example with text spec token (%x):
       * @code{.cpp}
       *    gpstk::CommonTime fromTime(gpstk::YDSTime(2018, 211, 0));
       *    gpstk::CommonTime toTime(gpstk::YDSTime(2018, 212, 0));
       *    string site("FOO");
       *    gpstk::FileSpec::FSTStringMap textStore;
       *    textStore[gpstk::FileSpec::text] = site;
       *    std::list<std::string> fileList = gpstk::FileSpecFind::find("/data/%04Y/%05n/%03j/nsh-%x-%5n-%1r-%04Y-%03j-%02H%02M%02S.xml", fromTime, toTime, textStore);
       * @endcode
       *
       *
       * Example without text spec token:
       * @code{.cpp}
       *    gpstk::FileSpec::FSTStringMap fsts;
       *    fromTime = gpstk::YDSTime(2017,1,0);
       *    toTime = gpstk::YDSTime(2017,2,0);
       *    std::list<std::string> fileList = gpstk::FileSpecFind::find(
       *       "/archive/%04Y/%05n/%05n-%04Y%03j-%1r%1t.raw",
       *       fromTime, toTime, fsts);
       * @endcode
       */
   class FileSpecFind
   {
   public:
         /// Data type for storing desired FileSpec values.
      using Filter = std::multimap<FileSpec::FileSpecType, std::string>;

         /** Search for existing files matching a given file spec and
          * time range.  May be used for file spec strings that
          * contain (non-time) tokens with no width specified,
          * provided specific values are added to fsts to fix the
          * width.
          * @param[in] fileSpec The FileSpec that the files should
          *   match.
          * @param[in] start Files/directories that precede this time
          *   will be ignored.
          * @param[in] end Files/directories that are after this time
          *   will be ignored.
          * @param[in] fsts Filler values for fileSpec that have no
          *   fixed width.  For example, "text" tokens may have no
          *   width specified in fileSpec, e.g. "%x", in which case a
          *   value should be added here to derive a length from.
          * @warning You will not get any matches for file names that
          *   contain tokens with no width (e.g. "%x") if you do not
          *   specify a text value in fsts.
          * @return A list of matching file names.
          */
      static std::list<std::string> find(
         const std::string& fileSpec,
         const CommonTime& start,
         const CommonTime& end,
         const FileSpec::FSTStringMap& fsts = FileSpec::FSTStringMap());

         /// @copydoc find(const std::string&,const CommonTime&,const CommonTime&,const FileSpec::FSTStringMap&)
      static std::list<std::string> find(
         const FileSpec& fileSpec,
         const CommonTime& start,
         const CommonTime& end,
         const FileSpec::FSTStringMap& fsts = FileSpec::FSTStringMap())
      { return find(fileSpec.getSpecString(), start, end, fsts); }

         /** Search for existing files matching a given file spec,
          * time range, and set of allowed FileSpec token values.
          * @param[in] fileSpec The FileSpec that the files should
          *   match.
          * @param[in] start Files/directories that precede this time
          *   will be ignored.
          * @param[in] end Files/directories that are after this time
          *   will be ignored.
          * @param[in] filter Set of allowable values for tokens
          *   present in fileSpec (values for tokens not present will
          *   be ignored).
          * @return A list of matching file names.
          */
      static std::list<std::string> find(
         const std::string& fileSpec,
         const CommonTime& start,
         const CommonTime& end,
         const Filter& filter);

         /// @copydoc find(const std::string&,const CommonTime&,const CommonTime&,const Filter&)
      static std::list<std::string> find(
         const FileSpec& fileSpec,
         const CommonTime& start,
         const CommonTime& end,
         const Filter& filter)
      { return find(fileSpec.getSpecString(), start, end, filter); }

   private:
         /** Translates FileSpec formatting tokens into glob expressions.
          * @param[in] token A string containing FileSpec formatting
          *   tokens e.g. %04Y.
          * @return a string with all of the known formatting tokens
          *   replaced with glob patterns that have a reasonable
          *   chance of matching names of existing files generated
          *   using the FileSpec.  For example, %04Y would become
          *   [0-9][0-9][0-9][0-9] */
      static std::string transToken(const std::string& token);

         /** Recursive (into subdirectories) function for find that
          * uses glob.  Look at the .cpp file for a more detailed
          * description of how this works.
          * @param[in] start Files/directories that precede this time
          *   will be ignored.
          * @param[in] end Files/directories that are after this time
          *   will be ignored.
          * @param[in] spec The string representation of the FileSpec
          *   that the files should match.
          * @param[in] dummyFSTS Filler values for the file spec when
          *   creating dummy file names to get appropriate time ranges.
          * @param[in] matched A string containing already-matched
          *   paths when recursion occurs.  This string replaces
          *   spec[0] through spec[pos] when doing searches.
          * @param[in] pos Starting position (from 0) into spec to
          *   start looking for FileSpec tokens.  Used to support
          *   recursion.
          * @return A list of matching file names.
          */
      static std::list<std::string> findGlob(
         const CommonTime& start,
         const CommonTime& end,
         const std::string& spec,
         const FileSpec::FSTStringMap& dummyFSTS,
         const Filter& filter,
         const std::string& matched = "",
         std::string::size_type pos = 0);

      friend class ::FileSpecFind_T;
   };
}

#endif // FILESPECFIND_HPP
