GITLABSGL / gpstk

 User activity
Project
Files
Commits
Network
Graphs
Issues 51
Merge Requests 0
Wiki
Settings
master

 gpstk   core   lib   GNSSEph   TabularSatStore.hpp
2f6a6a60   Doxygen-ify comments and some code reformatting Browse Code »
johnk 7 days ago  
 TabularSatStore.hpp 36.8 KB EditRawBlameHistoryPermalink Remove
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42
43
44
45
46
47
48
49
50
51
52
53
54
55
56
57
58
59
60
61
62
63
64
65
66
67
68
69
70
71
72
73
74
75
76
77
78
79
80
81
82
83
84
85
86
87
88
89
90
91
92
93
94
95
96
97
98
99
100
101
102
103
104
105
106
107
108
109
110
111
112
113
114
115
116
117
118
119
120
121
122
123
124
125
126
127
128
129
130
131
132
133
134
135
136
137
138
139
140
141
142
143
144
145
146
147
148
149
150
151
152
153
154
155
156
157
158
159
160
161
162
163
164
165
166
167
168
169
170
171
172
173
174
175
176
177
178
179
180
181
182
183
184
185
186
187
188
189
190
191
192
193
194
195
196
197
198
199
200
201
202
203
204
205
206
207
208
209
210
211
212
213
214
215
216
217
218
219
220
221
222
223
224
225
226
227
228
229
230
231
232
233
234
235
236
237
238
239
240
241
242
243
244
245
246
247
248
249
250
251
252
253
254
255
256
257
258
259
260
261
262
263
264
265
266
267
268
269
270
271
272
273
274
275
276
277
278
279
280
281
282
283
284
285
286
287
288
289
290
291
292
293
294
295
296
297
298
299
300
301
302
303
304
305
306
307
308
309
310
311
312
313
314
315
316
317
318
319
320
321
322
323
324
325
326
327
328
329
330
331
332
333
334
335
336
337
338
339
340
341
342
343
344
345
346
347
348
349
350
351
352
353
354
355
356
357
358
359
360
361
362
363
364
365
366
367
368
369
370
371
372
373
374
375
376
377
378
379
380
381
382
383
384
385
386
387
388
389
390
391
392
393
394
395
396
397
398
399
400
401
402
403
404
405
406
407
408
409
410
411
412
413
414
415
416
417
418
419
420
421
422
423
424
425
426
427
428
429
430
431
432
433
434
435
436
437
438
439
440
441
442
443
444
445
446
447
448
449
450
451
452
453
454
455
456
457
458
459
460
461
462
463
464
465
466
467
468
469
470
471
472
473
474
475
476
477
478
479
480
481
482
483
484
485
486
487
488
489
490
491
492
493
494
495
496
497
498
499
500
501
502
503
504
505
506
507
508
509
510
511
512
513
514
515
516
517
518
519
520
521
522
523
524
525
526
527
528
529
530
531
532
533
534
535
536
537
538
539
540
541
542
543
544
545
546
547
548
549
550
551
552
553
554
555
556
557
558
559
560
561
562
563
564
565
566
567
568
569
570
571
572
573
574
575
576
577
578
579
580
581
582
583
584
585
586
587
588
589
590
591
592
593
594
595
596
597
598
599
600
601
602
603
604
605
606
607
608
609
610
611
612
613
614
615
616
617
618
619
620
621
622
623
624
625
626
627
628
629
630
631
632
633
634
635
636
637
638
639
640
641
642
643
644
645
646
647
648
649
650
651
652
653
654
655
656
657
658
659
660
661
662
663
664
665
666
667
668
669
670
671
672
673
674
675
676
677
678
679
680
681
682
683
684
685
686
687
688
689
690
691
692
693
694
695
696
697
698
699
700
701
702
703
704
705
706
707
708
709
710
711
712
713
714
715
716
717
718
719
720
721
722
723
724
725
726
727
728
729
730
731
732
733
734
735
736
737
738
739
740
741
742
743
744
745
746
747
748
749
750
751
752
753
754
755
756
757
758
759
760
761
762
763
764
765
766
767
768
769
770
771
772
773
774
775
776
777
778
779
780
781
782
783
784
785
786
787
788
789
790
791
792
793
794
795
796
797
798
799
800
801
802
803
804
805
806
807
808
809
810
811
812
813
814
815
816
817
818
819
820
821
822
823
824
825
826
827
828
829
830
831
832
833
834
835
836
837
838
839
840
841
842
843
844
845
846
847
848
849
850
851
852
853
854
855
856
857
858
859
860
861
862
863
864
865
866
867
868
869
870
871
872
873
874
875
876
877
878
879
880
881
882
883
884
885
886
887
888
889
890
891
892
893
894
895
896
897
898
899
900
901
902
903
904
905
906
907
908
909
910
911
912
913
914
915
916
917
918
919
920
921
922
923
924
925
926
927
928
929
930
931
932
933
934
935
936
937
938
939
940
941
942
943
944
945
946
947
948
949
950
951
952
953
954
955
956
957
958
959
960
961
962
963
964
965
966
967
968
969
970
971
972
973
974
975
976
977
978
979
980
981
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

/** @file TabularSatStore.hpp
 * Store a tabular list of data (for example precise ephemeris data
 * from an SP3 file) for several satellites, and compute values at any
 * timetag from this table. */

#ifndef GPSTK_TABULAR_SAT_STORE_INCLUDE
#define GPSTK_TABULAR_SAT_STORE_INCLUDE

#include <map>
#include <iostream>
#include <cmath>

#include "Exception.hpp"
#include "SatID.hpp"
#include "CommonTime.hpp"
#include "TimeString.hpp"
#include "Xvt.hpp"
#include "CivilTime.hpp"
//#include "logstream.hpp"      // TEMP

namespace gpstk
{

      /// @ingroup GNSSEph
      //@{

      /** Store a table of data vs time for each of several
       * satellites.  The data are stored as DataRecords, one for each
       * satellite,time.  The getValue(sat, t) routine interpolates
       * the table for sat at time t and returns the result as a
       * DataRecord object.
       * @note this is an abstract class b/c getValue() and others are
       *   pure virtual.
       * @note this class (dump()) requires that
       *   operator<<(DataRecord) be defined. */
   template <class DataRecord>
   class TabularSatStore
   {

         // typedefs had to be set to public for python bindings to
         // compile these were originally in the protected block.
   public:
         // the data tables
         /// std::map with key=CommonTime, value=DataRecord
      typedef std::map<CommonTime, DataRecord> DataTable;

         /// std::map with key=SatID, value=DataTable
      typedef std::map<SatID, DataTable> SatTable;

         // member data
   protected:

         /** the data tables:
          * std::map<SatID, std::map<CommonTime, DataRecord> > */
      SatTable tables;

         /** Time system of tables; default and initial value is
          * TimeSystem::Any.  Set and maintained by derived classes,
          * using set and checkTimeSystem(); Any call to add records
          * with a specific system sets it; then other add.. or
          * get.. calls will throw if the time systems do not
          * match. */
      TimeSystem storeTimeSystem;

         /** Flags indicating that various data are present and may be
          * accessed with getValue(t) or other routines, via
          * interpolation of the data tables.  Defaults are false, but
          * deriving class should define in c'tor. */
      bool havePosition, haveVelocity, haveClockBias, haveClockDrift;

         /** Flag to check for data gaps (default false).  If this
          * flag is enabled, data gaps wider than member data
          * gapInterval will throw an InvalidRequest exception in
          * getValue() methods. */
      bool checkDataGap;

         /// Smallest time interval (seconds) that constitutes a data gap.
      double gapInterval;

         /** Flag to check the length of available interpolation
          * interval (default false) If this flag is enabled,
          * interpolation intervals shorter than member data
          * maxInterval will throw an InvalidRequest exception in
          * getValue() methods. */
      bool checkInterval;

         /** Maximum total time interval (seconds) allowed for
          * interpolation.  For example, with dt=900s and 10-point
          * Lagrange interpolation, this should be
          * (10-1)*900s+1=8101s. */
      double maxInterval;

      typedef typename DataTable::const_iterator DataTableIterator;

         // member functions
   public:
         /// Default constructor
      TabularSatStore() throw()
      : storeTimeSystem(TimeSystem::Any),
         havePosition(false), haveVelocity(false),
         haveClockBias(false), haveClockDrift(false),
         checkDataGap(false), checkInterval(false)
      {}
         /// Destructor
      virtual ~TabularSatStore() {}

         /** Return data value for the given satellite at the given
          * time (usually via interpolation of the data table).
          * @param[in] sat the SatID of the satellite of interest
          * @param[in] ttag the time (CommonTime) of interest
          * @return object of type DataRecord containing the data value(s).
          * @throw InvalidRequest if data value cannot be computed,
          *   for example because
          *  1. the time t does not lie within the time limits of the
          *     data table
          *  2. checkDataGap is true and there is a data gap
          *  3. checkInterval is true and the interval is larger than
          *     maxInterval
          * @note this function is pure virtual, making the class abstract.
          *
          * Derived objects can implement similar routines, for example:
          * Triple getPosition(const SatID& sat, const CommonTime& t)
          *    throw(InvalidRequest);
          * Triple getVelocity(const SatID& sat, const CommonTime& t)
          *    throw(InvalidRequest);
          * Triple getAccel(const SatID& sat, const CommonTime& t)
          *    throw(InvalidRequest);
          * double getClockBias(const SatID& s, const CommonTime& t)
          *    throw(InvalidRequest);
          * double[2] getClock(const SatID& sat, const CommonTime& t)
          *    throw(InvalidRequest);
          * @note Xvt getXvt(const SatID& sat, const CommonTime& t)
          *    throw(InvalidRequest);
          *   will be provided by another class which inherits this one.
          */
      virtual DataRecord getValue(const SatID& sat, const CommonTime& ttag)
         const throw(InvalidRequest) = 0;

         /** Locate the given time in the DataTable for the given
          * satellite.  Return two const iterators it1 and it2
          * (it1<it2) giving the range of 2*nhalf points, nhalf on
          * each side of the given time.  Note that a range is
          * returned even if the input time exactly matches one of the
          * times in the table; in this 'exact match' case the
          * matching time will be at either it1->first (if input
          * parameter exactReturn is true) or (it1+nhalf-1) or
          * (it1+nhalf) (if exactReturn is false).  This routine is
          * used to select data from the table for interpolation; note
          * that DataTable is a map<CommonTime,DataRecord>.
          * @param[in] sat satellite of interest
          * @param[in] ttag time of interest, e.g. where interpolation
          *   will be conducted
          * @param[in] nhalf number of table points desired on each
          *   side of ttag
          * @param[out] it1 const reference to const_iterator, points to
          *   the interval begin
          * @param[out] it2 const reference to const_iterator, points
          *   to the interval end
          * @param[in] exactReturn if true and exact match is found,
          *   return immediately, with the matching time at it1
          *   (==it1->first) [default is true].
          * @return true if ttag matches a time in the table and
          *   exactReturn was true, then the matching time is at it1
          *   (it2 is undefined); if exactReturn was false, then the
          *   range (it1,it2) is valid and the matching time is at
          *   either (it1+nhalf-1) or (it1+nhalf).
          * @throw InvalidRequest if the satellite is not found in the
          *   tables, or there is inadequate data
          * @throw InvalidRequest if GapInterval is set and there is a
          *   data gap larger than the max
          * @throw InvalidRequest if MaxInterval is set and the
          *   interval is too wide
          */
      virtual bool getTableInterval(const SatID& sat,
                                    const CommonTime& ttag,
                                    const int& nhalf,
                                    typename DataTable::const_iterator& it1,
                                    typename DataTable::const_iterator& it2,
                                    bool exactReturn=true)
         const throw(InvalidRequest)
      {
         try
         {
            static const char *fmt=
               " at time %F/%.3g %4Y/%02m/%02d %2H:%02M:%.3f %P";

               // find the DataTable for this sat
            typename std::map<SatID, DataTable>::const_iterator satit;
            satit = tables.find(sat);
            if(satit == tables.end())
            {
               InvalidRequest
                  e("Satellite " + gpstk::StringUtils::asString(sat) +
                    " not found.");
               GPSTK_THROW(e);
            }

               // this is the data table for the sat
            const DataTable& dtable(satit->second);

               // cannot interpolate with one point
            if(dtable.size() < 2)
            {
               InvalidRequest e("Inadequate data (size < 2) for satellite " +
                                gpstk::StringUtils::asString(sat) +
                                printTime(ttag,fmt));
               GPSTK_THROW(e);
            }

               // find the timetag in this table

               /** @note throw here if time systems do not match and
                * are not "Any" */
            it1 = dtable.find(ttag);
               // is it an exact match?
            bool exactMatch(it1 != dtable.end());

               // user must decide whether to return with exact value;
               // e.g. without velocity data, user needs the interval
               // to compute v from x data
            if(exactMatch && exactReturn)
               return true;

               // lower_bound points to the first element with key >= ttag
            it1 = it2 = dtable.lower_bound(ttag);
            if (it1 == dtable.end())
            {
               InvalidRequest e("No data in time range for satellite " +
                                gpstk::StringUtils::asString(sat) +
                                printTime(ttag,fmt));
               GPSTK_THROW(e);
            }

               // ttag is <= first time in table
            if(it1 == dtable.begin())
            {
                  // at table begin but its an exact match && an
                  // interval of only 2
               if(exactMatch && nhalf==1)
               {
                  ++(it2 = it1);
                  return exactMatch;
               }
               InvalidRequest e("Inadequate data before(1) requested time for"
                                " satellite " +
                                gpstk::StringUtils::asString(sat) +
                                printTime(ttag,fmt));
               GPSTK_THROW(e);
            }

               // move it1 down by one
            if(--it1 == dtable.begin())
            {
                  // if an interval of only 2
               if(nhalf==1)
               {
                  ++(it2 = it1);
                  return exactMatch;
               }
               InvalidRequest e("Inadequate data before(2) requested time for"
                                " satellite " +
                                gpstk::StringUtils::asString(sat) +
                                printTime(ttag,fmt));
               GPSTK_THROW(e);
            }

               //LOG(INFO) << "OK, have interval " << printTime(it1->first,"%F/%g") <<
               //" <= " <<printTime(ttag,"%F/%g")<< " < " <<printTime(it2->first,"%F/%g");

               // now have it1->first <= ttag < it2->first and it2 ==
               // it1+1 check for gap between these two table entries
               // surrounding ttag
            if(checkDataGap && (it2->first-it1->first) > gapInterval)
            {
               InvalidRequest e("Gap at interpolation time for satellite " +
                                gpstk::StringUtils::asString(sat) +
                                printTime(ttag,fmt));
               GPSTK_THROW(e);
            }

               // now expand the interval to include 2*nhalf timesteps
            for(int k=0; k<nhalf-1; k++)
            {
               bool last(k==nhalf-2); // true only on the last iteration
                  // move left by one; if require full interval && out
                  // of room on left, fail
               if(--it1 == dtable.begin() && !last)
               {
                  InvalidRequest
                     e("Inadequate data before(3) requested time for"
                       " satellite " + gpstk::StringUtils::asString(sat) +
                       printTime(ttag,fmt));
                  GPSTK_THROW(e);
               }
                  //LOG(INFO) << k << " expand left " << printTime(it1->first,"%F/%g");

               if(++it2 == dtable.end())
               {
                  if(exactMatch && last && it1 != dtable.begin())
                  {
                        // exact match && at end of interval && with
                        // room to move down

                        // move interval down by one
                     it2--;
                     it1--;
                  }
                  else
                  {
                     InvalidRequest
                        e("Inadequate data after(2) requested time for"
                          " satellite " + gpstk::StringUtils::asString(sat) +
                          printTime(ttag,fmt));
                     GPSTK_THROW(e);
                  }
               }
                  //LOG(INFO) << k << " expand right " << printTime(it2->first,"%F/%g");
            }

               // check that the interval is not too large
            if(checkInterval && (it2->first - it1->first) > maxInterval)
            {
               InvalidRequest e("Interpolation interval too large for"
                                " satellite " +
                                gpstk::StringUtils::asString(sat) +
                                printTime(ttag,fmt));
               GPSTK_THROW(e);
            }

            return exactMatch;
         }
         catch(InvalidRequest& ir)
         {
            GPSTK_RETHROW(ir);
         }
      }

         /** Version of getTableInterval() which does not require the
          * time of interest to lie in the center of the interval,
          * with nhalf points on either side.  (See getTableInterval()
          * for more documentation.) It may be used in conjunction
          * with SimpleLagrangeInterpolation(), which likewise does
          * not require the centered interval.
          *
          * @note this routine IS NOT and MUST NOT be used in
          * SP3EphemerisStore for two reasons. First, because
          * velocities and accelerations are computed (in
          * PositionSatStore::getValue()) for ephemerides, such as
          * IGS's, that provide only position. This means that the
          * conventional LagrangeInterpolation() routine(s), which
          * include ones that compute derivatives (see MiscMath.hpp)
          * must be used, and they require a centered
          * interval. Second, tests have shown that interpolating with
          * a non-centered interval can yield errors as large as 5cm
          * vertical and 3cm east, too large for precise positioning.
          *
          * @param[in] sat satellite of interest
          * @param[in] ttag time of interest, e.g. where interpolation
          *   will be conducted
          * @param[in] nhalf number of table points desired on each
          *   side of ttag
          * @param it1 const reference to const_iterator, points to
          *   the interval begin
          * @param it2 const reference to const_iterator, points to
          *   the interval end
          * @param[in] exactReturn if true and exact match is found,
          *   return immediately, with the matching time at it1
          *   (==it1->first) [default is true].
          * @return true if ttag matches a time in the table and
          *   exactReturn was true, then the matching time is at it1
          *   (it2 is undefined); if exactReturn was false, then the
          *   range (it1,it2) is valid and the matching time is at
          *   either (it1+nhalf-1) or (it1+nhalf).
          * @throw InvalidRequest if the satellite is not found in the
          *   tables, or there is inadequate data
          * @throw InvalidRequest if GapInterval is set and there is a
          *   data gap larger than the max
          * @throw InvalidRequest if MaxInterval is set and the
          *   interval is too wide */
      virtual bool getNonCenteredTableInterval(const SatID& sat,
                                               const CommonTime& ttag,
                                               const int& nhalf,
                                               typename DataTable::const_iterator& it1,
                                               typename DataTable::const_iterator& it2,
                                               bool exactReturn=true)
         const throw(InvalidRequest)
      {
         try
         {
               // find the DataTable for this sat
            typename std::map<SatID, DataTable>::const_iterator satit;
            satit = tables.find(sat);
            if(satit == tables.end())
            {
               InvalidRequest ir("Satellite " +
                                 gpstk::StringUtils::asString(sat) +
                                 " not found.");
               GPSTK_THROW(ir);
            }

               // this is the data table for the sat
            const DataTable& dtable(satit->second);
            static const char *fmt=" at time %4Y/%02m/%02d %2H:%02M:%02S";

               // find the timetag in this table
               /** @note throw here if time systems do not match and
                * are not "Any" */
            it1 = dtable.find(ttag);
               // is it an exact match?
            bool exactMatch(it1 != dtable.end());

               // user must decide whether to return with exact value;
               // e.g. without velocity data, user needs the interval
               // to compute v from x data
            if(exactMatch && exactReturn)
               return true;

               // lower_bound points to the first element with key >= ttag
            it1 = it2 = dtable.lower_bound(ttag);

               // Should we allow to predict data?
            if(it1 == dtable.end())
            {
               InvalidRequest ir("No data for the requested time of satellite "
                                 + gpstk::StringUtils::asString(sat)
                                 + printTime(ttag,fmt));
               GPSTK_THROW(ir);
            }

            if(int(dtable.size()) < 2*nhalf)
            {
               InvalidRequest ir("Inadequate data for satellite " +
                                 gpstk::StringUtils::asString(sat) +
                                 printTime(ttag,fmt));
               GPSTK_THROW(ir);
            }

               // check for gap only when exactMatch==false
            if(!exactMatch && checkDataGap)
            {
               typename DataTable::const_iterator lit,rit;
               lit = rit = it2;

               if(rit!=dtable.begin())
               {
                  lit--;
                  if((rit->first-lit->first) > gapInterval)
                  {
                     InvalidRequest ir(
                        "Gap at interpolation time for satellite " +
                        gpstk::StringUtils::asString(sat) +
                        printTime(ttag,fmt));
                     GPSTK_THROW(ir);
                  }
               }
               else
               {
                  if((rit->first - ttag) > gapInterval)
                  {
                     InvalidRequest ir(
                        "Gap may produce bad interpolation precision for"
                        " satellite " + gpstk::StringUtils::asString(sat) +
                        printTime(ttag,fmt));
                     GPSTK_THROW(ir);
                  }
               }
            }

               // try to get the beginning of the window
            int i, ileft(nhalf);
            for(i = 0; i < nhalf; i++)
            {
               if(it1 == dtable.begin())
                  break;
               it1--;
               ileft--;
            }

               // try to get the ending of the window
            int iright = nhalf - 1 + ileft;
            for(i = 0; i < (nhalf-1+ileft); i++)
            {
               it2++;
               if(it2 == dtable.end())
               {
                  it2--;
                  break;
               }
               iright--;
            }

               // adjust the beginning of the window again
            int ileft2 = iright;
            for(i = 0; i < iright; i++)
            {
               if(it1 == dtable.begin())
                  break;
               it1--;
               ileft2--;
            }

               // We have checked that there are enough data, so ileft2 should
               // always be zero.
            if(ileft2 > 0)
            {
               InvalidRequest ir("This should never happen");
               GPSTK_THROW(ir);
            }

               // check that the interval is not too large
            if(checkInterval && (it2->first - it1->first) > maxInterval)
            {
               InvalidRequest ir("Interpolation interval too large for"
                                 " satellite " +
                                 gpstk::StringUtils::asString(sat) +
                                 printTime(ttag,fmt));
               GPSTK_THROW(ir);
            }

            return exactMatch;
         }
         catch(InvalidRequest& ir)
         {
            GPSTK_RETHROW(ir);
         }
      }

         // interface like that of XvtStore

         /** Dump information about the object to an ostream.
          * @param[in] os ostream to receive the output; defaults to std::cout
          * @param[in] detail integer level of detail to provide;
          *   allowed values are
          *    0: number of satellites, time step and time limits, flags, and
          *       gap and interval flags and values
          *    1: number of data/sat
          *    2: above plus all the data tables */
      virtual void dump(std::ostream& os = std::cout, int detail = 0) const
         throw()
      {
         os << " Dump of TabularSatStore(" << detail << "):" << std::endl;
         if(detail >= 0)
         {
            static std::string
               fmt("%4F %w %10.3g %4Y/%02m/%02d %2H:%02M:%02S %P");

            os << "  Data stored for " << nsats() << " satellites"
               << std::endl
               << "  Time span of data: ";
            CommonTime initialTime(getInitialTime());
            initialTime.setTimeSystem(TimeSystem::Any);
            os << "  Initial time is " << printTime(initialTime,fmt)
               << std::endl;
            CommonTime finalTime(getFinalTime());
            if(initialTime == CommonTime::END_OF_TIME ||
               finalTime == CommonTime::BEGINNING_OF_TIME)
               os << "(there are no time limits)" << std::endl;
            else
               os << " FROM " << printTime(initialTime,fmt) << " TO "
                  << printTime(finalTime,fmt) << std::endl;

            os << "  This store contains:"
               << (havePosition ? "":" not") << " position,"
               << (haveVelocity ? "":" not") << " velocity,"
               << (haveClockBias ? "":" not") << " clock bias, and"
               << (haveClockDrift ? "":" not") << " clock drift data."
               << std::endl
               << "  Checking for data gaps? " << (checkDataGap ? "yes":"no");
            if(checkDataGap)
               os << "; gap interval is "
                  << std::fixed << std::setprecision(2) << gapInterval;
            os << std::endl;
            os << "  Checking data interval? " << (checkInterval ? "yes":"no");
            if(checkInterval)
               os << "; max interval is "
                  << std::fixed << std::setprecision(2) << maxInterval;
            os << std::endl;

            if(detail > 0)
            {
               typename SatTable::const_iterator it;
               for(it=tables.begin(); it!=tables.end(); it++)
               {
                  os << "   Sat " << it->first << " : "
                     << it->second.size() << " records.";

                  if(detail == 1)
                  {
                     os << std::endl;
                     continue;
                  }

                     /** @note cannot access DataRecord without
                      * operator<<(DataRecord) */
                  os << "   Data:" << std::endl;
                  typename DataTable::const_iterator jt;
                  for(jt=it->second.begin(); jt!=it->second.end(); jt++)
                  {
                     os << " " << printTime(jt->first,fmt)
                        << " " << gpstk::StringUtils::asString(it->first)
                        << " " << jt->second /// @note requires operator<<(DataRecord)
                        << std::endl;
                  }
               }
            }
         }
         os << " End dump of TabularSatStore." << std::endl;
      }

         /** Edit the data tables, removing data outside the indicated
          * time interval.
          * @param[in] tmin defines the beginning of the time interval
          * @param[in] tmax defines the end of the time interval */
      void edit(const CommonTime& tmin,
                const CommonTime& tmax = CommonTime::END_OF_TIME)
         throw()
      {
            // loop over satellites
         typename SatTable::iterator it;
         for(it=tables.begin(); it!=tables.end(); it++)
         {
               //sat = it->first;
            DataTable& dtab(it->second);
            typename DataTable::iterator jt;

               // delete everything above tmax
               // jt points to the earliest time > tmax
            jt = dtab.upper_bound(tmax);
            if(jt != dtab.end())
               dtab.erase(jt,dtab.end());

               // delete everything before tmin
               // jt points to the earliest time >= tmin
            jt = dtab.lower_bound(tmin);
            if(jt != dtab.begin() && --jt != dtab.begin())
               dtab.erase(dtab.begin(),jt);
         }
      }

         // remaining functions are not virtual

         /// Remove all data and reset time limits
      inline void clear() throw()
      {
         typename std::map<SatID, DataTable>::iterator satit;
         for(satit=tables.begin(); satit!=tables.end(); ++satit)
            satit->second.clear();
         tables.clear();
      }

         /// Return true if the given SatID is present in the store
      virtual bool isPresent(const SatID& sat) const throw()
      { return (tables.find(sat) != tables.end()); }

         /** Determine if the input TimeSystem conflicts with the
          * stored TimeSystem.
          * @param[in] ts TimeSystem to compare with stored TimeSystem
          * @throw InvalidRequest if time systems are inconsistent */
      void checkTimeSystem(const TimeSystem& ts) const throw(InvalidRequest)
      {
         if(ts != TimeSystem::Any && storeTimeSystem != TimeSystem::Any
            && ts != storeTimeSystem)
         {
            InvalidRequest ir("Conflicting time systems: " +
                              ts.asString() + " - " +
                              storeTimeSystem.asString());
            GPSTK_THROW(ir);
         }
      }

         /** Get the earliest time of data in the data tables.
          * @return the earliest time
          * @throw InvalidRequest if the store is empty. */
      CommonTime getInitialTime() const throw()
      {
         CommonTime initialTime(CommonTime::END_OF_TIME);
         if(tables.size() == 0) return initialTime;

            // loop over satellites
         typename SatTable::const_iterator it;
         for(it=tables.begin(); it!=tables.end(); it++)
         {
            const DataTable& dtab(it->second);
            typename DataTable::const_iterator jt;
               // update new time limits
            if(dtab.begin() != dtab.end())
            {
               jt = dtab.begin();
               if(jt->first < initialTime) initialTime = jt->first;
            }
         }

         return initialTime;
      }

         /** Get the latest time of data in the data tables.
          * @return the latest time */
      CommonTime getFinalTime() const throw()
      {
         CommonTime finalTime(CommonTime::BEGINNING_OF_TIME);
         if(tables.size() == 0)
            return finalTime;

            // loop over satellites
         typename SatTable::const_iterator it;
         for(it=tables.begin(); it!=tables.end(); it++)
         {
            const DataTable& dtab(it->second);
            typename DataTable::const_iterator jt;
               // update new time limits
            if(dtab.begin() != dtab.end())
            {
               --(jt = dtab.end());
               if(jt->first > finalTime)
                  finalTime = jt->first;
            }
         }

         return finalTime;
      }

         // end interface like that of XvtStore (also hasVelocity()
         // and isPresent())

         /** Get the earliest time of data in the store for the given
          * satellite.
          * @return the first time. */
      CommonTime getInitialTime(const SatID& sat) const throw()
      {
         CommonTime initialTime(CommonTime::END_OF_TIME);
         if(tables.size() == 0)
            return initialTime;

         typename SatTable::const_iterator it(tables.find(sat));
         if(it == tables.end())
            return initialTime;

         return (it->second.begin())->first;
      }

         /** Get the latest time of data in the store for the given satellite.
          * @return the last time. */
      CommonTime getFinalTime(const SatID& sat) const throw()
      {
         CommonTime finalTime(CommonTime::BEGINNING_OF_TIME);
         if(tables.size() == 0)
            return finalTime;

         typename SatTable::const_iterator it(tables.find(sat));
         if(it == tables.end())
            return finalTime;

         typename DataTable::const_iterator jt(it->second.end());
         --jt;
         return jt->first;
      }

         /** Dump the data in a subset of the tables as defined by iterators.
          * Note that the interval includes both it1 and it2. */
      void dumpInterval(typename DataTable::const_iterator& it1,
                        typename DataTable::const_iterator& it2,
                        std::ostream& os = std::cout) const throw()
      {
         const char *fmt="%4Y/%02m/%02d %2H:%02M:%02S";
         typename DataTable::const_iterator it(it1);
         while(1)
         {
            os << " " << printTime(it->first,fmt)
               << " " << it->second << std::endl;
            if(it == it2) break;
            ++it;
         }
      }

         /// Does this store contain position, etc data stored in the tables?
      bool hasPosition() const throw() { return havePosition; }
      bool hasVelocity() const throw() { return haveVelocity; }
      bool hasClockBias() const throw() { return haveClockBias; }
      bool hasClockDrift() const throw() { return haveClockDrift; }

         /// Get number of satellites available
      inline int nsats(void) const throw() { return tables.size(); }

         /// Is the given satellite present?
      bool hasSatellite(const SatID& sat) const throw()
      { return isPresent(sat); }

         /// Get a list (std::vector) of SatIDs present in the store
      std::vector<SatID> getSatList(void) const throw()
      {
         std::vector<SatID> satlist;
         typename SatTable::const_iterator it;
         for(it=tables.begin(); it != tables.end(); ++it)
         {
            if(it->second.size() > 0)
               satlist.push_back(it->first);
         }
         return satlist;
      }

         /// Get the total number of data records in the store
      inline int ndata(void) const throw()
      {
         int n(0);
         typename SatTable::const_iterator sit;
         for(sit=tables.begin(); sit != tables.end(); ++sit)
         {
            n += sit->second.size();
         }
         return n;
      }

         /// Get the number of data records for the given sat
      inline int ndata(const SatID& sat) const throw()
      {
         typename SatTable::const_iterator it(tables.find(sat));
         if(it == tables.end())
         {
            return 0;
         }
         else
         {
            return it->second.size();
         }
      }

         /// Get the number of data records for the given satellite system
      inline int ndata(const SatID::SatelliteSystem& sys) const throw()
      {
         int n(0);
         typename SatTable::const_iterator sit;
         for(sit=tables.begin(); sit != tables.end(); ++sit)
         {
            if(sit->first.system == sys)
               n += sit->second.size();
         }
         return n;
      }

         /// same as ndata()
      inline int size(void) const throw() { return ndata(); }

         /** compute the nominal timestep of the data table for the
          * given satellite
          * @return 0 if satellite is not found, else the nominal
          *   timestep in seconds. */
      double nomTimeStep(const SatID& sat) const throw()
      {
            // get the table for this sat
         typename SatTable::const_iterator it(tables.find(sat));

            // not found or empty
         if(it == tables.end() || it->second.size() == 0)
            return 0.0;

            // save the most frequent N step sizes
         static const int N=3;
         int i,ndt[N]={0,0,0};
         double dt[N],del;

            // loop over the table
         typename DataTable::const_iterator jt(it->second.begin());
         CommonTime prevT(jt->first);
         ++jt;
         while(jt != it->second.end())
         {
            del = jt->first - prevT;
            if(del > 1.0e-8)
            {
               for(i=0; i<N; i++)
               {
                  if(ndt[i] == 0)
                  {
                     dt[i] = del;
                     ndt[i] = 1;
                     break;
                  }
                  if(fabs(del-dt[i]) < 1.0e-8)
                  {
                     ndt[i]++;
                     break;
                  }
               }
            }
            prevT = jt->first;
            ++jt;
         }

            // find the most frequent interval
         del = dt[0];
         for(i=1; i<N; i++)
         {
            if(ndt[i] > ndt[0])
            {
               del = dt[i];
               ndt[0] = ndt[i];
            }
         }

         return del;
      }

         /// Is gap checking on?
      bool isDataGapCheck(void) throw() { return checkDataGap; }

         /// Disable checking of data gaps.
      void disableDataGapCheck(void) throw() { checkDataGap = false; }

         /// Get current gap interval.
      double getGapInterval(void) throw() { return gapInterval; }

         /// Set gap interval and turn on gap checking
      void setGapInterval(double interval) throw()
      { checkDataGap = true; gapInterval = interval; }

         /// Is interval checking on?
      bool isIntervalCheck(void) throw() { return checkInterval; }

         /// Disable checking of maximum interval.
      void disableIntervalCheck(void) throw() { checkInterval = false; }

         /// Get current maximum interval.
      double getMaxInterval(void) throw() { return maxInterval; }

         /// Set maximum interval and turn on interval checking
      void setMaxInterval(double interval) throw()
      {
         checkInterval = true;
         maxInterval = interval;
      }

         /// get the store's time system
      TimeSystem getTimeSystem(void) throw() { return storeTimeSystem; }

         /// set the store's time system
      void setTimeSystem(const TimeSystem& ts) throw()
      { storeTimeSystem = ts; }

   };

      //@}

}  // End of namespace gpstk

#endif // GPSTK_TABULAR_SAT_STORE_INCLUDE