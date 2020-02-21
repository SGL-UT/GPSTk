/* 
 * File:   Rinex3Datum.h
 * Author: alex
 *
 * Created on April 3, 2015, 1:35 PM
 */

#ifndef RINEX3DATUM_H
#define	RINEX3DATUM_H

/// A structure used to store a single RINEX Data point.

struct Rinex3Datum
{
  inline Rinex3Datum(double data = 0, short lli = 0, short ssi = 0, bool isEmpty = false) : 
    data(data), lli(lli), ssi(ssi), isEmpty(isEmpty) { } ///< constructor
  double data;  ///< The actual data point.
  short lli;    ///< See the RINEX Spec. for an explanation.
  short ssi;    ///< See the RINEX Spec. for an explanation.
  bool isEmpty;
};

#endif	/* RINEX3DATUM_H */

