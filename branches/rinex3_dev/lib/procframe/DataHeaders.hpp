#pragma ident "$Id$"

/**
 * @file DataHeaders.hpp
 * Set of several headers to be used with data structures.
 */

#ifndef DATAHEADERS_HPP
#define DATAHEADERS_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008
//
//============================================================================


#include <string>
#include "SatID.hpp"
#include "TypeID.hpp"
#include "SourceID.hpp"
#include "DayTime.hpp"
#include "Triple.hpp"



namespace gpstk
{

      /** @addtogroup DataStructures */

      //@{

      /// Set of several headers to be used with data structures.

      /// Defines a header containing just the source of data
   struct sourceHeader
   {

         /// The only field is a SourceID object
      SourceID source;


         /// Default constructor
      sourceHeader() {};


         /// Explicit constructor
      sourceHeader( const SourceID::SourceType& st,
                    const std::string& name )
      { source.type = st; source.sourceName = name; };


         /// Explicit constructor
      sourceHeader(const SourceID& sI)
         : source(sI)
      {};


         /// Copy constructor
      sourceHeader(const sourceHeader& sH)
      { source = sH.source; };


         /// Assignment operator
      virtual sourceHeader& operator=(const sourceHeader& right);


         /// Assignment operator from a SourceID
      virtual sourceHeader& operator=(const SourceID& right)
      { source = right; return *this; };


         /// Convenience output method for sourceHeader
      virtual std::ostream& dump(std::ostream& s) const;


         /// Destructor
      virtual ~sourceHeader() {};


   }; // End of struct 'sourceHeader'



      /// stream output for sourceHeader
   std::ostream& operator<<( std::ostream& s,
                             const sourceHeader& sh );



      /// Defines a header containing the source and epoch of data
   struct sourceEpochHeader : public sourceHeader
   {

         /// Field containing the epoch of data
      DayTime epoch;


         /// Default constructor
      sourceEpochHeader() {};


         /// Explicit constructor
      sourceEpochHeader( const SourceID::SourceType& st,
                         const std::string& sourcename,
                         const DayTime& time )
         : epoch(time)
      { source.type = st; source.sourceName = sourcename; };


         /// Explicit constructor
      sourceEpochHeader( const SourceID& sI,
                         const DayTime& time )
         : epoch(time)
      { source = sI; };


         /// Explicit constructor from parent class
      sourceEpochHeader( const sourceHeader& sh,
                         const DayTime& time )
         : sourceHeader(sh), epoch(time)
      {};


         /// Copy constructor
      sourceEpochHeader( const sourceEpochHeader& seh )
         : epoch(seh.epoch)
      { source = seh.source; };


         /// Assignment operator
      virtual sourceEpochHeader& operator=(const sourceEpochHeader& right);


         /// Assignment operator from a sourceHeader
      virtual sourceEpochHeader& operator=(const sourceHeader& right)
      { source = right.source; return (*this); };


         /// Assignment operator from a SourceID
      virtual sourceEpochHeader& operator=(const SourceID& right)
      { source = right; return (*this); };


         /// Convenience output method for sourceEpochHeader
      virtual std::ostream& dump(std::ostream& s) const;


         /// Destructor
      virtual ~sourceEpochHeader() {};


   }; // End of struct 'sourceEpochHeader'



      /// stream output for sourceEpochHeader
   std::ostream& operator<<( std::ostream& s,
                             const sourceEpochHeader& seh );



      /// Defines a header containing the source and epoch of data, plus extra
      /// data extracted from a Rinex file
   struct sourceEpochRinexHeader : public sourceEpochHeader
   {

         /// String describing the antenna type
      std::string antennaType;


         /// Triple holding the antenna position
      Triple antennaPosition;


         /// The RINEX epoch flag assigned to this epoch
      short epochFlag;


         /// Default constructor
      sourceEpochRinexHeader() {};


         /// Explicit constructor
      sourceEpochRinexHeader( const SourceID::SourceType& st,
                              const std::string& sourcename,
                              const DayTime& time,
                              const std::string& antType,
                              const Triple& antPos,
                              const short& flag )
         : antennaType(antType), antennaPosition(antPos), epochFlag(flag)
      { source.type = st; source.sourceName = sourcename; epoch = time; };


         /// Explicit constructor
      sourceEpochRinexHeader( const SourceID& sI,
                              const DayTime& time,
                              const std::string& antType,
                              const Triple& antPos,
                              const short& flag )
         : antennaType(antType), antennaPosition(antPos), epochFlag(flag)
      { source = sI; epoch = time; };


         /// Explicit constructor
      sourceEpochRinexHeader( const sourceHeader& sh,
                              const DayTime& time,
                              const std::string& antType,
                              const Triple& antPos,
                              const short& flag );


         /// Explicit constructor from parent class sourceEpochHeader
      sourceEpochRinexHeader(const sourceEpochHeader& seh)
         : sourceEpochHeader(seh)
      {};


         /// Explicit constructor from parent class plus extra parameters
      sourceEpochRinexHeader( const sourceEpochHeader& seh,
                              const std::string& antType,
                              const Triple& antPos,
                              const short& flag )
         : sourceEpochHeader(seh), antennaType(antType),
           antennaPosition(antPos), epochFlag(flag)
      {};


         /// Copy constructor
      sourceEpochRinexHeader(const sourceEpochRinexHeader& serh)
         : antennaType(serh.antennaType), antennaPosition(serh.antennaPosition),
           epochFlag(serh.epochFlag)
      { source = serh.source; epoch = serh.epoch; };


         /// Assignment operator
      virtual sourceEpochRinexHeader& operator=(
                                          const sourceEpochRinexHeader& right);


         /// Assignment operator from a sourceEpochHeader
      virtual sourceEpochRinexHeader& operator=(const sourceEpochHeader& right)
      { source = right.source; epoch = right.epoch; return (*this); };


         /// Assignment operator from a sourceHeader
      virtual sourceEpochRinexHeader& operator=(const sourceHeader& right)
      { source = right.source; return (*this); };


         /// Assignment operator from a SourceID
      virtual sourceEpochRinexHeader& operator=(const SourceID& right)
      { source = right; return (*this); };


         /// Convenience output method for sourceEpochRinexHeader
      virtual std::ostream& dump(std::ostream& s) const;


         /// Destructor
      virtual ~sourceEpochRinexHeader() {};


   }; // End of struct 'sourceEpochRinexHeader'



      /// stream output for sourceEpochRinexHeader
   std::ostream& operator<<( std::ostream& s,
                             const sourceEpochRinexHeader& serh );



      /// Defines a header containing the source and type of data
   struct sourceTypeHeader : public sourceHeader
   {

         /// Field describing the type of data being held
      TypeID type;


         /// Default constructor
      sourceTypeHeader() {};


         /// Explicit constructor
      sourceTypeHeader( const SourceID::SourceType& st,
                        const std::string& sourcename,
                        const TypeID& datatype )
         : type(datatype)
      { source.type = st; source.sourceName = sourcename; };


         /// Explicit constructor
      sourceTypeHeader( const SourceID& sI,
                        const TypeID& datatype )
         : type(datatype)
      { source = sI; };


         /// Explicit constructor from parent class
      sourceTypeHeader( const sourceHeader& sh,
                        const TypeID& datatype )
         : sourceHeader(sh), type(datatype)
      {};


         /// Copy constructor
      sourceTypeHeader( const sourceTypeHeader& sth )
         : type(sth.type)
      { source = sth.source; };


         /// Assignment operator from a sourceTypeHeader
      virtual sourceTypeHeader& operator=(const sourceTypeHeader& right);


         /// Assignment operator from a sourceHeader
      virtual sourceTypeHeader& operator=(const sourceHeader& right)
      { source = right.source; return (*this); };


         /// Assignment operator from a SourceID
      virtual sourceTypeHeader& operator=(const SourceID& right)
      { source = right; return (*this); };


         /// Convenience output method for sourceTypeHeader
      virtual std::ostream& dump(std::ostream& s) const;


         /// Destructor
      virtual ~sourceTypeHeader() {};


   }; // End of struct 'sourceTypeHeader'



      /// stream output for sourceTypeHeader
   std::ostream& operator<<( std::ostream& s,
                             const sourceTypeHeader& sth );



      /// Defines a header containing the source and satellite ID of data
   struct sourceSatHeader : public sourceHeader
   {

         /// Field holding the satellite ID this data belongs to
      SatID satellite;


         /// Default constructor
      sourceSatHeader() {};


         /// Explicit constructor
      sourceSatHeader( const SourceID::SourceType& st,
                       const std::string& sourcename,
                       const SatID& sat )
         : satellite(sat)
      { source.type = st; source.sourceName = sourcename; };


         /// Explicit constructor
      sourceSatHeader( const SourceID& sI,
                       const SatID& sat )
         : satellite(sat)
      { source = sI; };


         /// Explicit constructor from parent class
      sourceSatHeader( const sourceHeader& sh,
                       const SatID& sat )
         : sourceHeader(sh), satellite(sat)
      {};


         /// Copy constructor
      sourceSatHeader( const sourceSatHeader& ssh )
         : satellite(ssh.satellite)
      { source = ssh.source; };


         /// Assignment operator from a sourceSatHeader
      virtual sourceSatHeader& operator=(const sourceSatHeader& right);


         /// Assignment operator from a sourceHeader
      virtual sourceSatHeader& operator=(const sourceHeader& right)
      { source = right.source; return (*this); };


         /// Assignment operator from a SourceID
      virtual sourceSatHeader& operator=(const SourceID& right)
      { source = right; return (*this); };


         /// Convenience output method for sourceSatHeader
      virtual std::ostream& dump(std::ostream& s) const;


         /// Destructor
      virtual ~sourceSatHeader() {};


   }; // End of struct 'sourceSatHeader'



      /// stream output for sourceSatHeader
   std::ostream& operator<<( std::ostream& s,
                             const sourceSatHeader& ssh );



      /// Defines a header containing the source, satellite ID and epoch of data
   struct sourceEpochSatHeader : public sourceEpochHeader
   {

         /// Field holding the satellite ID this data belongs to
      SatID satellite;


         /// Default constructor
      sourceEpochSatHeader() {};


         /// Explicit constructor
      sourceEpochSatHeader( const SourceID::SourceType& st,
                            const std::string& sourcename,
                            const DayTime& time,
                            const SatID& sat )
         : satellite(sat)
      { source.type = st; source.sourceName = sourcename; epoch = time; };


         /// Explicit constructor
      sourceEpochSatHeader( const SourceID& sI,
                            const DayTime& time,
                            const SatID& sat )
         : satellite(sat)
      { source = sI; epoch = time; };


         /// Explicit constructor
      sourceEpochSatHeader( const sourceHeader& sh,
                            const DayTime& time,
                            const SatID& sat )
         : satellite(sat)
      { source = sh.source; epoch = time; };


         /// Explicit constructor from parent class
      sourceEpochSatHeader( const sourceEpochHeader& seh,
                            const SatID& sat )
         : satellite(sat)
      { source = seh.source; epoch = seh.epoch; };


         /// Copy constructor
      sourceEpochSatHeader( const sourceEpochSatHeader& sesh )
         : satellite(sesh.satellite)
      { source = sesh.source; epoch = sesh.epoch; };


         /// Assignment operator from a sourceEpochSatHeader
      virtual sourceEpochSatHeader& operator=(
                                          const sourceEpochSatHeader& right );


         /// Assignment operator from a sourceEpochHeader
      virtual sourceEpochSatHeader& operator=(const sourceEpochHeader& right)
      { source = right.source; epoch = right.epoch; return (*this); };


         /// Assignment operator from a sourceHeader
      virtual sourceEpochSatHeader& operator=(const sourceHeader& right)
      { source = right.source; return (*this); };


         /// Assignment operator from a SourceID
      virtual sourceEpochSatHeader& operator=(const SourceID& right)
      { source = right; return (*this); };


         /// Convenience output method for sourceEpochSatHeader
      virtual std::ostream& dump(std::ostream& s) const;


         /// Destructor
      virtual ~sourceEpochSatHeader() {};


   }; // End of struct 'sourceEpochSatHeader'



      /// stream output for sourceEpochSatHeader
   std::ostream& operator<<( std::ostream& s,
                             const sourceEpochSatHeader& sesh );



      /// Defines a header containing the source, type and epoch of data
   struct sourceEpochTypeHeader : public sourceEpochHeader
   {

         /// Field describing the type of data being held
      TypeID type;


         /// Default constructor
      sourceEpochTypeHeader() {};


         /// Explicit constructor
      sourceEpochTypeHeader( const SourceID::SourceType& st,
                             const std::string& sourcename,
                             const DayTime& time,
                             const TypeID& datatype )
         : type(datatype)
      { source.type = st; source.sourceName = sourcename; epoch = time; };


         /// Explicit constructor
      sourceEpochTypeHeader( const SourceID& sI,
                             const DayTime& time,
                             const TypeID& datatype )
         : type(datatype)
      { source = sI; epoch = time; };


         /// Explicit constructor
      sourceEpochTypeHeader( const sourceHeader& sh,
                             const DayTime& time,
                             const TypeID& datatype )
         : type(datatype)
      { source = sh.source; epoch = time; };


         /// Explicit constructor from parent class
      sourceEpochTypeHeader( const sourceEpochHeader& seh,
                             const TypeID& datatype )
         : type(datatype)
      { source = seh.source; epoch = seh.epoch; };


         /// Copy constructor
      sourceEpochTypeHeader( const sourceEpochTypeHeader& sesh )
         : type(sesh.type)
      { source = sesh.source; epoch = sesh.epoch; };


         /// Assignment operator from a sourceEpochTypeHeader
      virtual sourceEpochTypeHeader& operator=(
                                          const sourceEpochTypeHeader& right );


         /// Assignment operator from a sourceEpochHeader
      virtual sourceEpochTypeHeader& operator=(const sourceEpochHeader& right)
      { source = right.source; epoch = right.epoch; return (*this); };


         /// Assignment operator from a sourceHeader
      virtual sourceEpochTypeHeader& operator=(const sourceHeader& right)
      { source = right.source; return (*this); };


         /// Assignment operator from a SourceID
      virtual sourceEpochTypeHeader& operator=(const SourceID& right)
      { source = right; return (*this); };


         /// Convenience output method for sourceEpochTypeHeader
      virtual std::ostream& dump(std::ostream& s) const;


         /// Destructor
      virtual ~sourceEpochTypeHeader() {};


   }; // End of struct 'sourceEpochTypeHeader'



      /// Stream output for sourceEpochTypeHeader
   std::ostream& operator<<( std::ostream& s,
                             const sourceEpochTypeHeader& seth );


      //@}


}  // End of namespace gpstk
#endif   // DATAHEADERS_HPP
