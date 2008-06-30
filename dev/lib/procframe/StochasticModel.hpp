#pragma ident "$Id$"

/**
 * @file StochasticModel.hpp
 * Base class to define stochastic models, plus implementations
 * of common ones.
 */

#ifndef STOCHASTICMODEL_HPP
#define STOCHASTICMODEL_HPP

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



#include "DayTime.hpp"
#include "DataStructures.hpp"



namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This is a base class to define stochastic models. It computes the
       *  elements of Phi and Q matrices corresponding  to a constant
       *  stochastic model.
       *
       * @sa RandomWalkModel, WhiteNoiseModel, PhaseAmbiguityModel
       *
       */
   class StochasticModel
   {
   public:

         /// Default constructor
      StochasticModel() {};


         /// Get element of the state transition matrix Phi
      virtual double getPhi()
      { return 1.0; };


         /// Get element of the process noise matrix Q
      virtual double getQ()
      { return 0.0; };


         /** This method provides the stochastic model with all the available
          *  information and takes appropriate actions. By default, it does
          *  nothihg.
          *
          * @param type       Type of variable.
          * @param sat        Satellite.
          * @param gData      Data object holding the data.
          *
          */
      virtual void Prepare( const TypeID& type,
                            const SatID& sat,
                            gnssSatTypeValue& gData )
      { return; };


         /** This method provides the stochastic model with all the available
          *  information and takes appropriate actions. By default, it does
          *  nothihg.
          *
          * @param type       Type of variable.
          * @param sat        Satellite.
          * @param gData      Data object holding the data.
          *
          */
      virtual void Prepare( const TypeID& type,
                            const SatID& sat,
                            gnssRinex& gData )
      { return; };


         /// Destructor
      virtual ~StochasticModel() {};

   };



      /** This class compute the elements of Phi and Q matrices corresponding 
       *  to a random walk stochastic model.
       *
       * @sa StochasticModel, ConstantModel, WhiteNoiseModel
       *
       * \warning RandomWalkModel objets store their internal state, so you
       * MUST NOT use the SAME object to process DIFFERENT data streams.
       *
       */
   class RandomWalkModel : public StochasticModel
   {
   public:

         /// Default constructor. By default sets a very high Qprime and
         /// both previousTime and currentTime are DayTime::BEGINNING_OF_TIME.
      RandomWalkModel()
         : qprime(90000000000.0), previousTime(DayTime::BEGINNING_OF_TIME),
           currentTime(DayTime::BEGINNING_OF_TIME) {};


         /** Common constructor
          *
          * @param qp         Process spectral density: d(variance)/d(time) or
          *                   d(sigma*sigma)/d(time).
          * @param prevTime   Value of previous epoch
          *
          * \warning Beware of units: Process spectral density units are
          * sigma*sigma/time, while other models take plain sigma as input.
          * Sigma units are usually given in meters, but time units MUST BE
          * in SECONDS.
          *
          */
      RandomWalkModel( double qp,
                       const DayTime& prevTime=DayTime::BEGINNING_OF_TIME,
                       const DayTime& currentTime=DayTime::BEGINNING_OF_TIME )
         : qprime(qp), previousTime(prevTime), currentTime(prevTime) {};


         /** Set the value of previous epoch
          *
          * @param prevTime   Value of previous epoch
          *
          */
      RandomWalkModel& setPreviousTime(const DayTime& prevTime)
      { previousTime = prevTime; return (*this); }


         /** Set the value of current epoch
          *
          * @param currTime   Value of current epoch
          *
          */
      RandomWalkModel& setCurrentTime(const DayTime& currTime)
      { currentTime = currTime; return (*this); }


         /** Set the value of process spectral density.
          *
          * @param qp         Process spectral density: d(variance)/d(time) or
          *                   d(sigma*sigma)/d(time).
          *
          * \warning Beware of units: Process spectral density units are
          * sigma*sigma/time, while other models take plain sigma as input.
          * Sigma units are usually given in meters, but time units MUST BE
          * in SECONDS.
          *
          */
      RandomWalkModel& setQprime(double qp)
      { qprime = qp; return (*this); }


         /// Get element of the process noise matrix Q
      virtual double getQ();


         /** This method provides the stochastic model with all the available
          *  information and takes appropriate actions.
          *
          * @param type       Type of variable.
          * @param sat        Satellite.
          * @param gData      Data object holding the data.
          *
          */
      virtual void Prepare( const TypeID& type,
                            const SatID& sat,
                            gnssSatTypeValue& gData );


         /** This method provides the stochastic model with all the available
          *  information and takes appropriate actions.
          *
          * @param type       Type of variable.
          * @param sat        Satellite.
          * @param gData      Data object holding the data.
          *
          */
      virtual void Prepare( const TypeID& type,
                            const SatID& sat,
                            gnssRinex& gData );


         /// Destructor
      virtual ~RandomWalkModel() {};


   private:

         /// Process spectral density
      double qprime;


         /// Epoch of previous measurement
      DayTime previousTime;


         /// Epoch of previous measurement
      DayTime currentTime;

   };



      /** This class compute the elements of Phi and Q matrices corresponding 
       *  to a white noise stochastic model.
       *
       * @sa StochasticModel, ConstantModel, RandomWalkModel
       *
       */
   class WhiteNoiseModel : public StochasticModel
   {
   public:


         /** Common constructor
          *
          * @param sigma   Standard deviation (sigma) of white noise process
          *
          */
      WhiteNoiseModel( double sigma = 300000.0 )
         : variance(sigma*sigma) {};


         /// Set the value of white noise sigma
      WhiteNoiseModel& setSigma(double sigma)
      { variance = sigma*sigma; return (*this); }


         /// Get element of the state transition matrix Phi
      virtual double getPhi()
      { return 0.0; };


         /// Get element of the process noise matrix Q
      virtual double getQ()
      { return variance; };


         /// Destructor
      virtual ~WhiteNoiseModel() {};


   private:


         /// White noise variance
      double variance;

   };



      /** This class compute the elements of Phi and Q matrices corresponding 
       *  to a phase ambiguity variable: Constant stochastic model within 
       *  cycle slips and white noise stochastic model when a cycle slip
       *  happens.
       *
       * @sa StochasticModel, ConstantModel, WhiteNoiseModel
       *
       * \warning By default, this class expects each satellite to have
       * 'TypeID::satArc' data inserted in the GNSS Data Structure. Such data 
       * are generated by 'SatArcMarker' objects. Use 'setWatchSatArc()'
       * method to change this behaviour and use cycle slip flags directly.
       */
   class PhaseAmbiguityModel : public StochasticModel
   {
   public:


         /** Common constructor
          *
          * @param sigma   Standard deviation (sigma) of white noise process
          *
          */
      PhaseAmbiguityModel( double sigma = 2e7 )
         : variance(sigma*sigma), cycleSlip(false), watchSatArc(true) {};


         /// Set the value of white noise sigma
      PhaseAmbiguityModel& setSigma(double sigma)
      { variance = sigma*sigma; return (*this); }


         /** Feed the object with information about occurrence of cycle slips.
          *
          * @param cs   Boolean indicating if there is a cycle slip at current
          *             epoch.
          *
          */
      PhaseAmbiguityModel& setCS(bool cs)
      { cycleSlip = cs; return (*this); };


         /// Set whether satellite arc will be used instead of cycle slip flag
      PhaseAmbiguityModel& setWatchSatArc(bool watchArc)
      { watchSatArc = watchArc; return (*this); }


         /// Get element of the state transition matrix Phi
      virtual double getPhi();


         /// Get element of the process noise matrix Q
      virtual double getQ();


         /** This method provides the stochastic model with all the available
          *  information and takes appropriate actions.
          *
          * @param type       Type of variable.
          * @param sat        Satellite.
          * @param gData      Data object holding the data.
          *
          */
      virtual void Prepare( const TypeID& type,
                            const SatID& sat,
                            gnssSatTypeValue& gData );


         /** This method provides the stochastic model with all the available
          *  information and takes appropriate actions.
          *
          * @param type       Type of variable.
          * @param sat        Satellite.
          * @param gData      Data object holding the data.
          *
          */
      virtual void Prepare( const TypeID& type,
                            const SatID& sat,
                            gnssRinex& gData );


         /// Destructor
      virtual ~PhaseAmbiguityModel() {};


   private:


         /// White noise variance
      double variance;

         /// Boolean stating if there is a cycle slip at current epoch
      bool cycleSlip;

         /// Whether satellite arcs will be watched. False by default
      bool watchSatArc;

         /// Map holding information regarding every satellite
      std::map<SatID, double> satArcMap;

   };



      //@}

}
#endif // STOCHASTICMODEL_HPP
