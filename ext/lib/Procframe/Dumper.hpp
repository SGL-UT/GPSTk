//============================================================================
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
//  Copyright 2004, The University of Texas at Austin
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2009, 2011
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file Dumper.hpp
 * This class dumps the values inside a GNSS Data Structure (GDS).
 */

#ifndef GPSTK_DUMPER_HPP
#define GPSTK_DUMPER_HPP

#include <ostream>
#include "ProcessingClass.hpp"


namespace gpstk
{

      /// @ingroup DataStructures 
      //@{


      /** This class dumps the values inside a GNSS Data Structure (GDS), and
       *  therefore is meant to be used with the GDS objects found in
       *  "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       *       // Create the input obs file stream
       *    RinexObsStream rin("ebre0300.02o");
       *
       *   gnssRinex gRin;
       *
       *   Dumper dumpObj;
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> dumpObj;
       *   }
       * @endcode
       *
       * The "Dumper" object will visit every satellite in the GNSS Data
       * Structure that is "gRin" and will print the information associated
       * with it to a pre-designated place.
       *
       * By default "Dumper" will print all information to 'std::cout' output
       * stream (which is a 'std::ostream'), and will include the epoch and the
       * station, as well as the type associated with each data value.
       *
       * The default behavior may be changed using the appropriate constructors
       * and/or methods. For instance, in order to dump information to file
       * 'model.out', not including station ID, and including only data of
       * satellite arc, cycle slip flag, and prefit residuals of code and phase,
       * we could do the following:
       *
       * @code
       *
       *       //// INITIALIZATION CODE HERE FOR OTHER OBJECTS...
       *
       *       // Create the output file stream
       *    ofstream dumperFile;
       *    dumperFile.open( "model.out", ios::out );
       *
       *       // You make want to set an specific precision for printing
       *    dumperFile << fixed << setprecision( 3 );
       *
       *   gnssRinex gRin;
       *
       *   Dumper dumpObj( dumperFile );
       *
       *      // Deactivate SourceID printing
       *   dumpObj.setPrintSourceID( false );
       *
       *      // Limit TypeIDs to print. It is a good idea to start with 
       *      // 'setType()' to clear all TypeIDs previously set, and then
       *      // follow adding new TypeIDs using 'addType()'.
       *   dumpObj.setType(TypeID::satArc);
       *   dumpObj.addType(TypeID::CSL1);
       *   dumpObj.addType(TypeID::prefitC);
       *   dumpObj.addType(TypeID::prefitL);
       *
       *   while(rin >> gRin)
       *   {
       *      try
       *      {
       *         gRin  >> basicM
       *               >> correctObs
       *               >> compWindup
       *               >> computeTropo
       *               >> linear1      // Compute combinations
       *               >> pcFilter
       *               >> markCSLI2
       *               >> markCSMW
       *               >> markArc
       *               >> linear2      // Compute prefit residuals
       *               >> dumpObj
       *               >> pppSolver;
       *      }
       *      catch(...)
       *      {
       *         cerr << "Unknown exception at epoch: " << time << endl;
       *         continue;
       *      }
       *
       * @endcode
       *
       * Please note that, in order to dump a given TypeID, it must be present
       * in the GNSS Data Structure.
       *
       * A nice feature of "Dumper" objects is that they return the incoming
       * GDS without altering it, so they can be inserted wherever you need
       * them.
       */
   class Dumper : public ProcessingClass
   {
   public:

         /// Default constructor
      Dumper()
         : outStr(&std::cout), printType(true), printTime(true),
           printStation(true)
      { };


         /** Common constructor
          *
          * @param out           Stream object used for output.
          * @param printtype     Flag to print TypeID's.
          * @param printtime     Flag to print CommonTime's.
          * @param printstation  Flag to print SourceID's.
          *
          */
      Dumper( std::ostream& out,
              int printtype = true,
              bool printtime = true,
              bool printstation = true )
         : outStr(&out), printType(printtype), printTime(printtime),
           printStation(printstation)
      { };


         /** Dumps data from a satTypeValueMap object.
          *
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( satTypeValueMap& gData )
         throw(ProcessingException);


         /** Dumps data from a gnnsSatTypeValue object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process( gnssSatTypeValue& gData )
         throw(ProcessingException)
      { gnssRinex gRin(gData); Process(gRin); return gData; };


         /** Dumps data from a gnnsRinex object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process( gnssRinex& gData )
         throw(ProcessingException);


         /// Returns pointer to stream object used for output.
      virtual std::ostream* getOutputStream(void) const
      { return outStr; };


         /** Sets stream object used for output.
          *
          * @param out           Stream object used for output.
          */
      virtual Dumper& setOutputStream( std::ostream& out )
      { outStr = &out; return (*this); };


         /// Returns flag controlling TypeID printing.
      virtual bool getPrintTypeID(void) const
      { return printType; };


         /** Sets flag controlling TypeID printing.
          *
          * @param printtype     Flag to print TypeID's.
          */
      virtual Dumper& setPrintTypeID( bool printtype )
      { printType = printtype; return (*this); };


         /// Returns flag controlling CommonTime printing.
      virtual bool getPrintTime(void) const
      { return printTime; };


         /** Sets flag controlling CommonTime printing.
          *
          * @param printtime     Flag to print CommonTime's.
          */
      virtual Dumper& setPrintTime( bool printtime )
      { printTime = printtime; return (*this); };


         /// Returns flag controlling SourceID printing.
      virtual bool getPrintSourceID(void) const
      { return printStation; };


         /** Sets flag controlling SourceID printing.
          *
          * @param printstation  Flag to print SourceID's.
          */
      virtual Dumper& setPrintSourceID( bool printstation )
      { printStation = printstation; return (*this); };


         /** Method to set the TypeID to be printed.
          *
          * @param type      TypeID of data values to be printed.
          *
          * \warning The previously set type values will be deleted. If this
          * is not what you want, see method addType.
          *
          * \warning If no TypeIDs are specified, then ALL TypeIDs present in
          * the GDS will be printed.
          */
      virtual Dumper& setType( const TypeID& type )
      { printTypeSet.clear(); printTypeSet.insert(type); return (*this); };


         /** Method to add a TypeID to be printed.
          *
          * @param type          TypeID of data values to be added to the ones
          *                      being printed.
          */
      virtual Dumper& addType( const TypeID& type )
      { printTypeSet.insert(type); return (*this); };


         /** Method to set a set of TypeIDs to be printed.
          *
          * @param printSet       TypeIDSet of data values to be printed.
          *
          * \warning The previously set type values will be deleted. If this
          * is not what you want, see method addDiffType.
          */
      virtual Dumper& setTypeSet( const TypeIDSet& printSet )
      { printTypeSet.clear(); printTypeSet = printSet; return (*this); };


         /** Method to add a set of TypeIDs to be printed.
          *
          * @param printSet      TypeIDSet of data values to be added to the
          *                      ones being printed.
          */
      virtual Dumper& addTypeSet( const TypeIDSet& printSet );


         /// Method to clear the set of TypeIDs to be printed. If you do this,
         /// all TypeIDs that are present in GDS will be printed.
      virtual Dumper& clearTypeSet( void )
      { printTypeSet.clear(); return (*this); };


         /// Method to get the set of TypeIDs to be printed.
      virtual TypeIDSet getTypeSet(void) const
      { return printTypeSet; };


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~Dumper() {};


   private:


         /// Stream object used for output.
      std::ostream* outStr;


         /// Control flag for printing TypeID's.
      bool printType;


         /// Control flag for printing CommonTime's.
      bool printTime;


         /// Control flag for printing SourceID's.
      bool printStation;


         /// Set of TypeID's to print
      TypeIDSet printTypeSet;


         /// Print TypeIDs information.
      void printTypeID( const typeValueMap& tvMap );


   }; // End of class 'Dumper'

      //@}

}  // End of namespace gpstk

#endif  // GPSTK_DUMPER_HPP
