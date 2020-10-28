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

#include "TimeString.hpp"
#include "FileSpecFind.hpp"
#ifndef WIN32
#include <glob.h>
#define PATH_SEP_STRING "/"
#else
#include "build_config.h" // for getFileSep
#define PATH_SEP_STRING "/\\"
#include <windows.h>
#include <shlwapi.h>
/// Copied from glob(3) man page, here to minimize changes to code under windows
typedef struct
{
   size_t gl_pathc; ///< Count of paths matched so far
   char **gl_pathv; ///< List of matched pathnames
   size_t gl_offs;  ///< Slots to reserve in gl_pathv (unused)
} glob_t;
// These are defined to minimize changes, the code doesn't actually
// implement different behavior that is implied by these flags.
#define GLOB_ERR     0x0001 ///< Immediate return on read error
#define GLOB_NOSORT  0x0002 ///< Don't sort paths
#define GLOB_TILDE   0x0000 ///< meaningless in windows
#define GLOB_NOSPACE    1   ///< Ran out of memory.
#define GLOB_ABORTED    2   ///< Read error.
#define GLOB_NOMATCH    3   ///< No matches found.
#endif

using namespace std;


#ifdef WIN32
/** Part of a windows implementation of glob.  Performs basic pattern matching.
 * @param[in] pattern The original globbing pattern to match.
 * @param[in] test The string to match.
 * @return true if test exactly matches pattern (i.e. substring
 *   matches do not count.
 */
static bool winMatchRE(const std::string& pattern, const std::string& test)
{
      // change wildcards to regular expressions, making sure to match
      // the whole string.
   std::string patternRE = "^" + gpstk::StringUtils::change(pattern,".","\\.") +
      "$";
   patternRE = gpstk::StringUtils::change(patternRE,"*",".*");
   patternRE = gpstk::StringUtils::change(patternRE,"?",".");
   std::regex re(patternRE, std::regex::basic);
   std::smatch m;
   std::regex_search(test, m, re);
   return !m.empty();
}


/**
 * Part of a windows implementation of glob.  Performs the directory
 * scouring and stores the data in a list, which the actual glob()
 * function turns back into the struct that POSIX uses.
 * This is a recursive function that will attempt to match one
 * directory layer at a time to improve performance.
 * @param[in] pattern The globbing pattern to match.
 * @param[out] results The files matching pattern.
 */
static void winGlob(const char *pattern, std::list<std::string>& results)
{
   WIN32_FIND_DATA findFileData; 
   HANDLE hFindFile;
   std::string patternStr(pattern);
   if (patternStr.empty())
      return;
   std::string::size_type pos = patternStr.find_first_of("*?["), pos2;
      /** @note We don't use getFileSep because both forward slash and
       * backslash are supported.  This does lead to potential issues
       * however if someone were to put a forward slash in an actual
       * file or directory name.  Not sure how to support such a
       * circumstance, though. */
   pos = patternStr.find_last_of(PATH_SEP_STRING, pos);
   if (pos != std::string::npos)
   {
      pos++; // skip over the file separator.
      pos2 = patternStr.find(PATH_SEP_STRING, pos);
      std::string pathSearch = patternStr.substr(0,pos2);
         // turn POSIX glob pattern into windows pattern before trying to find
      pathSearch = gpstk::StringUtils::change(pathSearch, "[0-9]", "?");
      std::string matchPattern = patternStr.substr(pos,pos2-pos);
      hFindFile = FindFirstFile(pathSearch.c_str(), &findFileData);
      if (hFindFile != INVALID_HANDLE_VALUE) 
      { 
         do 
         {
               // Check to make sure the results match the globbing
               // pattern, since windows globbing is much less
               // sophisticated than POSIX.
            if (winMatchRE(matchPattern, findFileData.cFileName))
            {
                  // Keep the path that matches at the current level
               std::string match(
                  patternStr.substr(0,pos) + findFileData.cFileName);
                  // Add the remaining pattern, if any, for recursive search
               std::string newPattern(
                  match + gpstk::getFileSep() + patternStr.substr(pos2+1));
                  // check if we have more directory layers to go in
                  // the pattern and the match is a directory.
               if ((pos2 != std::string::npos) &&
                   (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
               {
                     // recurse into the matching directory
                  winGlob(newPattern.c_str(), results);
               }
               else if (pos2 == std::string::npos)
               {
                     // add the match to our results
                  results.push_back(match);
               }
            }
         } while(FindNextFile(hFindFile, &findFileData)); 
         FindClose(hFindFile); 
      }
   }
   else
   {
         // If we're here, then we're looking for a file with no path
         // and no wildcards, so just try to match the pattern
         // directly.
      hFindFile = FindFirstFile(pattern, &findFileData);
      if (hFindFile != INVALID_HANDLE_VALUE)
      {
         results.push_back(pattern);
      }
   }
}


/** Implement what is hoped is an adequate windows version of POSIX glob().
 * @param[in] pattern The globbing pattern to match.
 * @param[in] flags Unused, otherwise see glob(3).
 * @param[in] errfunc Unused, otherwise see glob(3).
 * @param[out] pglob The matching files on the file system.
 * @return 0 on success, GLOB_NOSPACE if out of memory, GLOB_NOMATCH if no
 *   files are found.
 */
static int glob(const char *pattern, int flags,
                int (*errfunc) (const char *epath, int eerrno),
                glob_t *pglob)
{
   pglob->gl_pathc = 0;
   pglob->gl_pathv = nullptr;
   std::list<std::string> results;
   winGlob(pattern, results);
   if (results.empty())
      return GLOB_NOMATCH;
   pglob->gl_pathc = results.size();
   pglob->gl_pathv = (char**)malloc(pglob->gl_pathc * sizeof(char*));
   if (pglob->gl_pathv == nullptr)
         return GLOB_NOSPACE;
   unsigned j = 0;
   for (const auto& i : results)
   {
      pglob->gl_pathv[j] = (char*)malloc(i.length() + 1);
      if (pglob->gl_pathv[j] == nullptr)
         return GLOB_NOSPACE;
      strcpy(pglob->gl_pathv[j], i.c_str());
      j++;
   }
   return 0;
}


/** Free memory allocated by glob().
 * @param[in,out] pglob A glob_t structure whose contents were
 *   allocated by glob().
 * @post Memory is freed, pointers are set to null, count set to 0.
 */
static void globfree(glob_t *pglob)
{
   for (unsigned i = 0; i < pglob->gl_pathc; i++)
   {
      free(pglob->gl_pathv[i]);
   }
   free(pglob->gl_pathv);
   pglob->gl_pathv = nullptr;
   pglob->gl_pathc = 0;
}
#endif

namespace gpstk
{
   list<string> FileSpecFind ::
   find(const string& fileSpecString,
        const gpstk::CommonTime& start,
        const gpstk::CommonTime& end,
        const gpstk::FileSpec::FSTStringMap& fsts)
   {
         // This first pile of code replaces text tokens of frequently
         // unknown size with fixed sizes.  If you use FileSpec to
         // create a filename that has a token %x or %4x and you don't
         // have the "text" value set in fsts, FileSpec will not fill
         // in any default.  It will for most other tokens, however.
      const gpstk::FileSpec::FileSpecType
         fsType = gpstk::FileSpec::text,
         fsSel = gpstk::FileSpec::selected;
      gpstk::FileSpec::FSTStringMap dummyFSTS(fsts);
      string spec(fileSpecString);
      unsigned textLen = dummyFSTS[fsType].length();
      if (textLen == 0)
      {
            // If we don't have a text value set in fsts, make one up.
            // It likely won't match anyway.
         dummyFSTS[fsType] = "Z";
         textLen = 1;
      }
      string textTok = gpstk::FileSpec::convertFileSpecType(fsType);
      string textLenS = gpstk::StringUtils::asString(textLen);
      unsigned selLen = dummyFSTS[fsSel].length();
      if (selLen == 0)
      {
            // If we don't have a selection value set in fsts, make
            // one up.  It likely won't match anyway.
         dummyFSTS[fsSel] = "Z"; // whatever, no meaning behind it
         selLen = 1;
      }
      string selTok = gpstk::FileSpec::convertFileSpecType(fsSel);
      string selLenS = gpstk::StringUtils::asString(textLen);
         // Just change the arbitrary text token to the value we have,
         // since there's at most one value to match.
      gpstk::StringUtils::change(spec, "%"+textTok, dummyFSTS[fsType]);
      gpstk::StringUtils::change(spec, "%"+selTok, "%"+selLenS+selTok);
         // Fill in other defaults, which will get set to a fixed
         // width by FileSpec.
      for (unsigned i = gpstk::FileSpec::unknown;
           i < gpstk::FileSpec::firstTime; i++)
      {
         gpstk::FileSpec::FileSpecType fst = (gpstk::FileSpec::FileSpecType)i;
         if ((fst == FileSpec::fixed) || (fst == FileSpec::unknown))
            continue;
         if (dummyFSTS.find(fst) == dummyFSTS.end())
            dummyFSTS[fst] = "";
      }

      Filter filter;

      return findGlob(start, end, spec, dummyFSTS, filter);
   }


   std::list<std::string> FileSpecFind ::
   find(const std::string& fileSpec,
        const CommonTime& start,
        const CommonTime& end,
        const Filter& filter)
   {
         // The filter can contain multiple values.
         // "Cowardly refusing to implement variable width FileSpec"
      FileSpec::FSTStringMap dummyFSTS;
         // still gotta fill it, though, because even fixed width
         // tokens won't get translated if there's no value, e.g. %5n
         // will stay %5n after converting to string if there's no
         // station value in dummyFSTS
      for (unsigned i = gpstk::FileSpec::unknown;
           i < gpstk::FileSpec::firstTime; i++)
      {
         gpstk::FileSpec::FileSpecType fst = (gpstk::FileSpec::FileSpecType)i;
         if ((fst == FileSpec::fixed) || (fst == FileSpec::unknown))
            continue;
         dummyFSTS[fst] = "";
      }
      return findGlob(start, end, fileSpec, dummyFSTS, filter);
   }


   string FileSpecFind ::
   transToken(const string& token)
   {
         // chew through a FileSpec string changing tokens to glob patterns
      string rv;
      string::size_type spos = 0;
      string textTok = gpstk::FileSpec::convertFileSpecType(
         gpstk::FileSpec::text);
      string selTok = gpstk::FileSpec::convertFileSpecType(
         gpstk::FileSpec::selected);
      string stnTok = gpstk::FileSpec::convertFileSpecType(
         gpstk::FileSpec::station);
      while (true)
      {
         string::size_type ppos = token.find('%', spos);
         if (ppos == string::npos)
         {
            rv += token.substr(spos, ppos);
            break;
         }
         rv += token.substr(spos, ppos-spos);
         string::size_type fpos = token.find_first_not_of("0123456789", ppos+1);
         unsigned long len = 0;
         char *end;
         if ((fpos - ppos) > 0)
         {
            len = std::strtoul(&token[ppos+1], &end, 10);
         }
         if (len == 0)
         {
            rv += "*";
         }
         else
         {
            if ((token[fpos] == textTok[0]) || // arbitrary text
                (token[fpos] == selTok[0]) ||  // selected
                (token[fpos] == stnTok[0]))    // station ID
            {
                  // text values, just use single character matches for
                  // the appropriate length
               rv += string(len, '?');
            }
            else
            {
                  // is there a more efficient way to make multiple
                  // copies of a string?
               for (unsigned long c = 0; c < len; c++)
               {
                     // numeric value, match the range for however many
                     // characters the token width specifies.
                  rv += "[0-9]";
               }
            }
         }
         spos = fpos+1;
      }
      return rv;
   }


      /* This function recurses into a path, each level of recursion
       * mapping to a level of the path hierarchy in which FileSpec
       * tokens exist.
       * e.g.
       *                   1         2         3
       *         0123456789012345678901234567890123456789
       * spec:   /data/%04Y/%05n/%03j/...
       * Level 0 ^^^^^^^^^^^^^^^^^^^^
       * Level 1                      ^^^^
       * Level 2                           ^^^^
       *
       * The function works by picking out the path in spec up to and
       * including the first token after pos.  This segment of the
       * path (thisSpec in the code) is then used in two ways.  First,
       * the spec is used to create a pair of times that are fromTime
       * and toTime reduced to the granularity available at that level
       * (fromTimeMatch and toTimeMatch).  At Level 0 in the above
       * example, that would reduce the fromTime and toTime values to
       * just the year.
       *
       * Second, the path segment is converted into a globbing pattern
       * which is then used to find all files/directories on the file
       * system that could *potentially* match the spec.  A couple of
       * potential match examples for Level 0 above:
       *    /data/9999
       *    /data/2018
       * NOT a match:
       *    /data/NOPE
       *    /data/something
       *    /data/9123s
       *
       * Results from the glob function are then processed using
       * FileSpec to get the time using thisSpec which is then
       * compared with the fromTimeMatch and toTimeMatch computed
       * above to make sure that the path falls within the requested
       * time span at least to the time granularity available at that
       * level (again, the granularity at level 0 would be year).
       *
       * Results that pass the time span test are then rolled back
       * into a recursive call to findGlob, and this is where
       * "matched" and "pos" come into play.
       *
       * Recursive calls use the results from glob() that pass the
       * time test as the "matched" data, and the length of thisSpec
       * as pos so that the called findGlob will replace the current
       * path segment with the current match.  All of this is done so
       * that when recursing into subdirectories, ONLY those
       * subdirectories that match the desired range are explored
       * further.
       *
       * Example:
       * .....Level 0..........................
       * spec         : /data/%04Y/%05n/%03j/nsh-FOO-%5n-%1r-%04Y-%03j-%02H%02M%02S.xml
       * matched      : 
       * pos          : 0
       * thisSpec     : /data/%04Y
       * pattern      : /data/[0-9][0-9][0-9][0-9]
       * fromTimeMatch: 2018 1 0.000000
       * toTimeMatch  : 2018 1 0.000000
       * glob matches:
       *    /data/2017
       *       2017 1 0.000000  not a time range match
       *    /data/2018
       *       2018 1 0.000000  TIME RANGE MATCHED
       *
       *
       * .....Level 1..........................
       * spec        : /data/%04Y/%05n/%03j/nsh-FOO-%5n-%1r-%04Y-%03j-%02H%02M%02S.xml
       * matched      : /data/2018
       * pos          : 38
       * thisSpec     : /data/%04Y/%05n
       * pattern      : /data/2018/[0-9][0-9][0-9][0-9][0-9]
       * fromTimeMatch: 2018 1 0.000000
       * toTimeMatch  : 2018 1 0.000000
       * glob matches:
       *    /data/2018/85789
       *       2018 1 0.000000  TIME RANGE MATCHED
       *
       *
       * .....Level 2..........................
       * spec         : /data/%04Y/%05n/%03j/nsh-FOO-%5n-%1r-%04Y-%03j-%02H%02M%02S.xml
       * matched      : /data/2018/85789
       * pos          : 43
       * thisSpec     : /data/%04Y/%05n/%03j
       * pattern      : /data/2018/85789/[0-9][0-9][0-9]
       * fromTimeMatch: 2018 211 0.000000
       * toTimeMatch  : 2018 212 0.000000
       * glob matches:
       *    /data/2018/85789/218
       *       2018 218 0.000000  not a time range match
       *    /data/2018/85789/211
       *       2018 211 0.000000  TIME RANGE MATCHED
       * 
       * 
       * .....Level 3..........................
       * spec         : /data/%04Y/%05n/%03j/nsh-FOO-%5n-%1r-%04Y-%03j-%02H%02M%02S.xml
       * matched      : /data/2018/85789/211
       * pos          : 48
       * thisSpec     : /data/%04Y/%05n/%03j/nsh-FOO-%5n-%1r-%04Y-%03j-%02H%02M%02S.xml
       * pattern      : /data/2018/85789/211/nsh-FOO-[0-9][0-9][0-9][0-9][0-9]-[0-9]-[0-9][0-9][0-9][0-9]-[0-9][0-9][0-9]-[0-9][0-9][0-9][0-9][0-9][0-9].xml
       * fromTimeMatch: 2018 211 0.000000
       * toTimeMatch  : 2018 212 0.000000
       * glob matches:
       *    /data/2018/85789/211/nsh-FOO-85789-1-2018-211-184500.xml
       *       2018 211 67500.000000  TIME RANGE MATCHED
       */
   list<string> FileSpecFind ::
   findGlob(const gpstk::CommonTime& fromTime,
            const gpstk::CommonTime& toTime,
            const string& spec,
            const gpstk::FileSpec::FSTStringMap& dummyFSTS,
            const Filter& filter,
            const string& matched,
            string::size_type pos)
   {
      list<string> rv;
         // level 0:
         // /data/%04Y/%05n/%03j/nsh-FOO-%5n-%1r-%04Y-%03j-%02H%02M%02S.xml
         //      12   3
         // 1 = stoppos
         // 2 = stokpos
         // 3 = srest
         //
         // level 3:
         // /data/%04Y/%05n/%03j/nsh-FOO-%5n-%1r-%04Y-%03j-%02H%02M%02S.xml
         //                     1        2                                 3
         // 1 = stoppos
         // 2 = stokpos
         // 3 = srest

         // find the first part of the path that contains a FileSpec token
      string::size_type stokpos = spec.find('%', pos);
         // find the beginning of the remaining path (subdirectory or
         // file within the directory starting at stokpos)
      string::size_type stoppos = min(
         stokpos, spec.find_last_of(PATH_SEP_STRING, stokpos));
         // srest is the first character of the "rest" of the path
         // i.e. lower depths in the tree.
      string::size_type srest =
         (stokpos == string::npos
          ? string::npos
          : spec.find_first_of(PATH_SEP_STRING, stokpos+1));
         // thisSpec is JUST the part of the path that we've already searched
      string thisSpec(spec.substr(0, srest));
         // patternSpec takes the parts of the original spec that have
         // already been matched and replaces that with the "matched"
         // string which fixes us into the specific directory tree
         // we're searching with glob
      string patternSpec(thisSpec);
      patternSpec.replace(0, pos, matched);
         // pattern is the same as patternSpec but with all the
         // FileSpec tokens replaced with glob patterns.
      string pattern = transToken(patternSpec);
         // currentSpec and currentSpecScanner contain the file spec
         // for just the currently searched directory.  We do this to
         // minimize the amount of effort done in matching, because
         // we're doing recursive searches there's zero reason to
         // check upper level directories against the filter, or time
         // range if there are no time-related spec types at this
         // level, as they will have already been checked.
      string currentSpec = thisSpec.substr(stoppos+1);
      FileSpec currentSpecScanner(currentSpec);
      bool checkTime = currentSpecScanner.hasTimeField();

         // Use the current spec to turn our time range into something
         // that will be useable to match at this level in the
         // heirarchy.
      gpstk::CommonTime fromTimeMatch, toTimeMatch;
      gpstk::FileSpec specScanner(thisSpec);
      if (checkTime)
      {
         string fromString = specScanner.toString(fromTime, dummyFSTS);
         string toString = specScanner.toString(toTime, dummyFSTS);
         fromTimeMatch = specScanner.extractCommonTime(fromString);
         toTimeMatch = specScanner.extractCommonTime(toString);
            // Make sure our conditions can be met, i.e. from <= t < to
            // by adding 1/10th of a second to "to" if from and to are the same
         if (toTimeMatch == fromTimeMatch)
            toTimeMatch += 0.1;
      }

         // Find all the files that match the pattern at this level.
      glob_t globbuf;
      int g = glob(pattern.c_str(), GLOB_ERR|GLOB_NOSORT|GLOB_TILDE, nullptr,
                   &globbuf);
      for (size_t i = 0; i < globbuf.gl_pathc; i++)
      {
         bool timeMatched = true;
         if (checkTime)
         {
               // check if the matched files/directories are within
               // the search time
            gpstk::CommonTime fileTime =
               specScanner.extractCommonTime(globbuf.gl_pathv[i]);
            timeMatched = ((fromTimeMatch <= fileTime) &&
                           (fileTime < toTimeMatch));
         }
         if (timeMatched)
         {
               // File's/directory's time is within the desired span.
               // Allow optional filter to determine whether to recurse further
            bool matchedFilter = true;
            if (!filter.empty())
            {
                  // Iterate through the filter data, attempting to
                  // match the filter values with the spec.  Do NOT
                  // use the increment operator on the iterator as
                  // part of a for loop as that will cause the loop to
                  // get stuck at the end of the multimap.
               Filter::const_iterator fi = filter.begin();
                  // Keep the value of the most recently extracted
                  // field so we don't spend time extracting it over
                  // and over if multiple values for a given
                  // FileSpecType are specified.
               std::string fieldVal;
                  // Keep track of the FileSpecType associated with
                  // the value in fieldVal
               FileSpec::FileSpecType lastFST = FileSpec::unknown;
               while (fi != filter.end())
               {
                     // Check to see if the current directory level of
                     // the file spec contains the FileSpecType
                     // pointed to by the filter iterator.  We compare
                     // the FST against lastFST first because it's
                     // really fast compared to hasField().
                  if ((fi->first == lastFST) ||
                      currentSpecScanner.hasField(fi->first))
                  {
                        // thisSpec, which only matches the current
                        // directory level, contains this particular
                        // FileSpecType, so check to see if the value
                        // matches.
                     if (fi->first != lastFST)
                     {
                           // extract the field value from the path
                           // which we haven't done yet.
                        fieldVal = specScanner.extractField(globbuf.gl_pathv[i],
                                                            fi->first);
                        lastFST = fi->first;
                     }
                     if (fi->second == fieldVal)
                     {
                           // field value in the path matches, so move
                           // on to the next FileSpecType
                        fi = filter.upper_bound(fi->first);
                     }
                     else
                     {
                           // try the next filter value
                        fi++;
                           // but check to make sure we haven't
                           // exhausted all possible matches for a
                           // Filtered field.
                        if ((fi == filter.end()) ||
                            (fi->first != lastFST))
                        {
                           matchedFilter = false;
                           break;
                        }
                     }
                  }
                  else
                  {
                        // currentSpec, which only matches the current
                        // directory level, doesn't contain this
                        // particular FileSpecType, so skip to the
                        // next FileSpecType.
                     fi = filter.upper_bound(fi->first);
                  }
               }
            } // if (!filter.empty())

            if (matchedFilter)
            {
                  // If srest is npos, that means there is no more path
                  // depth and no more recursion to process.
               if (srest == string::npos)
               {
                  rv.push_back(globbuf.gl_pathv[i]);
               }
               else
               {
                     // Still more path depth to go, recurse.
                  list<string> toAdd =
                     findGlob(fromTime, toTime, spec, dummyFSTS, filter,
                              globbuf.gl_pathv[i], thisSpec.length());
                  rv.splice(rv.end(), toAdd);
               }
            } // if (matchedFilter)
         } // if ((fromTimeMatch <= fileTime) && (fileTime < toTimeMatch))
      } // for (size_t i = 0; i < globbuf.gl_pathc; i++)
      globfree(&globbuf);
      return rv;
   }
}

