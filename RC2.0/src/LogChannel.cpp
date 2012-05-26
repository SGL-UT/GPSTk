#pragma ident "$Id$"

/**
 * @file LogChannel.cpp
 * 
 */
 
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
//  Wei Yan - Chinese Academy of Sciences . 2011
//
//============================================================================

#include "LogChannel.hpp"
#include "NumberFormatter.hpp"

namespace gpstk
{
   using namespace std;


   std::string LogChannel::getLogText(const LogMessage& msg)
   {
      string text;

      DayTime time = msg.time;
      
      string::const_iterator it = pattern.begin();
      string::const_iterator end = pattern.end();
      while (it != end)
      {
         if (*it == '%')
         {
            if (++it != end)
            {
               switch (*it)
               {
               case 's': text.append(msg.source); break;
               case 't': text.append(msg.text); break;
               case 'l': NumberFormatter::append(text, (int) msg.level); break;
               case 'p': text.append(LogLevelName(msg.level)); break;
               case 'q': text += LogLevelName(msg.level).at(0); break;
               case 'U': text.append(msg.file); break;
               case 'u': NumberFormatter::append(text, msg.line); break;
               case 'd': NumberFormatter::append0(text, time.day(), 2); break;
               case 'e': NumberFormatter::append(text, time.day()); break;
               case 'f': NumberFormatter::append(text, time.day(), 2); break;
               case 'm': NumberFormatter::append0(text, time.month(), 2); break;
               case 'n': NumberFormatter::append(text, time.month()); break;
               case 'o': NumberFormatter::append(text, time.month(), 2); break;
               case 'y': NumberFormatter::append0(text, time.year() % 100, 2); break;
               case 'Y': NumberFormatter::append0(text, time.year(), 4); break;
               case 'H': NumberFormatter::append0(text, time.hour(), 2); break;
               case 'M': NumberFormatter::append0(text, time.minute(), 2); break;
               case 'S': NumberFormatter::append0(text, (int)time.second(), 2); break;
               default: text += *it;
               }
               ++it;
            }
         }
         else text += *it++;
      }

      return text; 
   }  // LogChannel::log(const LogMessage& msg)
   
}   // End of namespace gpstk

