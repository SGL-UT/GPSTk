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

#include "GlobalTropModel.hpp"
#include "MJD.hpp"

namespace gpstk
{
   // ------------------------------------------------------------------------
   // GlobalTropModel

   // Constants for Global mapping functions
   const double GlobalTropModel::ADryMean[55] = {
      +1.2517e+02, +8.503e-01, +6.936e-02, -6.760e+00, +1.771e-01,
      +1.130e-02, +5.963e-01, +1.808e-02, +2.801e-03, -1.414e-03,
      -1.212e+00, +9.300e-02, +3.683e-03, +1.095e-03, +4.671e-05,
      +3.959e-01, -3.867e-02, +5.413e-03, -5.289e-04, +3.229e-04,
      +2.067e-05, +3.000e-01, +2.031e-02, +5.900e-03, +4.573e-04,
      -7.619e-05, +2.327e-06, +3.845e-06, +1.182e-01, +1.158e-02,
      +5.445e-03, +6.219e-05, +4.204e-06, -2.093e-06, +1.540e-07,
      -4.280e-08, -4.751e-01, -3.490e-02, +1.758e-03, +4.019e-04,
      -2.799e-06, -1.287e-06, +5.468e-07, +7.580e-08, -6.300e-09,
      -1.160e-01, +8.301e-03, +8.771e-04, +9.955e-05, -1.718e-06,
      -2.012e-06, +1.170e-08, +1.790e-08, -1.300e-09, +1.000e-10 };

   const double GlobalTropModel::BDryMean[55] = {
      +0.000e+00, +0.000e+00, +3.249e-02, +0.000e+00, +3.324e-02,
      +1.850e-02, +0.000e+00, -1.115e-01, +2.519e-02, +4.923e-03,
      +0.000e+00, +2.737e-02, +1.595e-02, -7.332e-04, +1.933e-04,
      +0.000e+00, -4.796e-02, +6.381e-03, -1.599e-04, -3.685e-04,
      +1.815e-05, +0.000e+00, +7.033e-02, +2.426e-03, -1.111e-03,
      -1.357e-04, -7.828e-06, +2.547e-06, +0.000e+00, +5.779e-03,
      +3.133e-03, -5.312e-04, -2.028e-05, +2.323e-07, -9.100e-08,
      -1.650e-08, +0.000e+00, +3.688e-02, -8.638e-04, -8.514e-05,
      -2.828e-05, +5.403e-07, +4.390e-07, +1.350e-08, +1.800e-09,
      +0.000e+00, -2.736e-02, -2.977e-04, +8.113e-05, +2.329e-07,
      +8.451e-07, +4.490e-08, -8.100e-09, -1.500e-09, +2.000e-10 };
       
   const double GlobalTropModel::ADryAmp[55] = {
      -2.738e-01, -2.837e+00, +1.298e-02, -3.588e-01, +2.413e-02,
      +3.427e-02, -7.624e-01, +7.272e-02, +2.160e-02, -3.385e-03,
      +4.424e-01, +3.722e-02, +2.195e-02, -1.503e-03, +2.426e-04,
      +3.013e-01, +5.762e-02, +1.019e-02, -4.476e-04, +6.790e-05,
      +3.227e-05, +3.123e-01, -3.535e-02, +4.840e-03, +3.025e-06,
      -4.363e-05, +2.854e-07, -1.286e-06, -6.725e-01, -3.730e-02,
      +8.964e-04, +1.399e-04, -3.990e-06, +7.431e-06, -2.796e-07,
      -1.601e-07, +4.068e-02, -1.352e-02, +7.282e-04, +9.594e-05,
      +2.070e-06, -9.620e-08, -2.742e-07, -6.370e-08, -6.300e-09,
      +8.625e-02, -5.971e-03, +4.705e-04, +2.335e-05, +4.226e-06,
      +2.475e-07, -8.850e-08, -3.600e-08, -2.900e-09, +0.000e+00 };
       
   const double GlobalTropModel::BDryAmp[55] = {
      +0.000e+00, +0.000e+00, -1.136e-01, +0.000e+00, -1.868e-01,
      -1.399e-02, +0.000e+00, -1.043e-01, +1.175e-02, -2.240e-03,
      +0.000e+00, -3.222e-02, +1.333e-02, -2.647e-03, -2.316e-05,
      +0.000e+00, +5.339e-02, +1.107e-02, -3.116e-03, -1.079e-04,
      -1.299e-05, +0.000e+00, +4.861e-03, +8.891e-03, -6.448e-04,
      -1.279e-05, +6.358e-06, -1.417e-07, +0.000e+00, +3.041e-02,
      +1.150e-03, -8.743e-04, -2.781e-05, +6.367e-07, -1.140e-08,
      -4.200e-08, +0.000e+00, -2.982e-02, -3.000e-03, +1.394e-05,
      -3.290e-05, -1.705e-07, +7.440e-08, +2.720e-08, -6.600e-09,
      +0.000e+00, +1.236e-02, -9.981e-04, -3.792e-05, -1.355e-05,
      +1.162e-06, -1.789e-07, +1.470e-08, -2.400e-09, -4.000e-10 };
       
   const double GlobalTropModel::AWetMean[55] = {
      +5.640e+01, +1.555e+00, -1.011e+00, -3.975e+00, +3.171e-02,
      +1.065e-01, +6.175e-01, +1.376e-01, +4.229e-02, +3.028e-03,
      +1.688e+00, -1.692e-01, +5.478e-02, +2.473e-02, +6.059e-04,
      +2.278e+00, +6.614e-03, -3.505e-04, -6.697e-03, +8.402e-04,
      +7.033e-04, -3.236e+00, +2.184e-01, -4.611e-02, -1.613e-02,
      -1.604e-03, +5.420e-05, +7.922e-05, -2.711e-01, -4.406e-01,
      -3.376e-02, -2.801e-03, -4.090e-04, -2.056e-05, +6.894e-06,
      +2.317e-06, +1.941e+00, -2.562e-01, +1.598e-02, +5.449e-03,
      +3.544e-04, +1.148e-05, +7.503e-06, -5.667e-07, -3.660e-08,
      +8.683e-01, -5.931e-02, -1.864e-03, -1.277e-04, +2.029e-04,
      +1.269e-05, +1.629e-06, +9.660e-08, -1.015e-07, -5.000e-10 };
       
   const double GlobalTropModel::BWetMean[55] = {
      +0.000e+00, +0.000e+00, +2.592e-01, +0.000e+00, +2.974e-02,
      -5.471e-01, +0.000e+00, -5.926e-01, -1.030e-01, -1.567e-02,
      +0.000e+00, +1.710e-01, +9.025e-02, +2.689e-02, +2.243e-03,
      +0.000e+00, +3.439e-01, +2.402e-02, +5.410e-03, +1.601e-03,
      +9.669e-05, +0.000e+00, +9.502e-02, -3.063e-02, -1.055e-03,
      -1.067e-04, -1.130e-04, +2.124e-05, +0.000e+00, -3.129e-01,
      +8.463e-03, +2.253e-04, +7.413e-05, -9.376e-05, -1.606e-06,
      +2.060e-06, +0.000e+00, +2.739e-01, +1.167e-03, -2.246e-05,
      -1.287e-04, -2.438e-05, -7.561e-07, +1.158e-06, +4.950e-08,
      +0.000e+00, -1.344e-01, +5.342e-03, +3.775e-04, -6.756e-05,
      -1.686e-06, -1.184e-06, +2.768e-07, +2.730e-08, +5.700e-09 };
       
   const double GlobalTropModel::AWetAmp[55] = {
      +1.023e-01, -2.695e+00, +3.417e-01, -1.405e-01, +3.175e-01,
      +2.116e-01, +3.536e+00, -1.505e-01, -1.660e-02, +2.967e-02,
      +3.819e-01, -1.695e-01, -7.444e-02, +7.409e-03, -6.262e-03,
      -1.836e+00, -1.759e-02, -6.256e-02, -2.371e-03, +7.947e-04,
      +1.501e-04, -8.603e-01, -1.360e-01, -3.629e-02, -3.706e-03,
      -2.976e-04, +1.857e-05, +3.021e-05, +2.248e+00, -1.178e-01,
      +1.255e-02, +1.134e-03, -2.161e-04, -5.817e-06, +8.836e-07,
      -1.769e-07, +7.313e-01, -1.188e-01, +1.145e-02, +1.011e-03,
      +1.083e-04, +2.570e-06, -2.140e-06, -5.710e-08, +2.000e-08,
      -1.632e+00, -6.948e-03, -3.893e-03, +8.592e-04, +7.577e-05,
      +4.539e-06, -3.852e-07, -2.213e-07, -1.370e-08, +5.800e-09 };
       
   const double GlobalTropModel::BWetAmp[55] = {
      +0.000e+00, +0.000e+00, -8.865e-02, +0.000e+00, -4.309e-01,
      +6.340e-02, +0.000e+00, +1.162e-01, +6.176e-02, -4.234e-03,
      +0.000e+00, +2.530e-01, +4.017e-02, -6.204e-03, +4.977e-03,
      +0.000e+00, -1.737e-01, -5.638e-03, +1.488e-04, +4.857e-04,
      -1.809e-04, +0.000e+00, -1.514e-01, -1.685e-02, +5.333e-03,
      -7.611e-05, +2.394e-05, +8.195e-06, +0.000e+00, +9.326e-02,
      -1.275e-02, -3.071e-04, +5.374e-05, -3.391e-05, -7.436e-06,
      +6.747e-07, +0.000e+00, -8.637e-02, -3.807e-03, -6.833e-04,
      -3.861e-05, -2.268e-05, +1.454e-06, +3.860e-07, -1.068e-07,
      +0.000e+00, -2.658e-02, -1.947e-03, +7.131e-04, -3.506e-05,
      +1.885e-07, +5.792e-07, +3.990e-08, +2.000e-08, -5.700e-09 };

   const double GlobalTropModel::Ageoid[55] = {
      -5.6195e-01,-6.0794e-02,-2.0125e-01,-6.4180e-02,-3.6997e-02,
      +1.0098e+01,+1.6436e+01,+1.4065e+01,+1.9881e+00,+6.4414e-01,
      -4.7482e+00,-3.2290e+00,+5.0652e-01,+3.8279e-01,-2.6646e-02,
      +1.7224e+00,-2.7970e-01,+6.8177e-01,-9.6658e-02,-1.5113e-02,
      +2.9206e-03,-3.4621e+00,-3.8198e-01,+3.2306e-02,+6.9915e-03,
      -2.3068e-03,-1.3548e-03,+4.7324e-06,+2.3527e+00,+1.2985e+00,
      +2.1232e-01,+2.2571e-02,-3.7855e-03,+2.9449e-05,-1.6265e-04,
      +1.1711e-07,+1.6732e+00,+1.9858e-01,+2.3975e-02,-9.0013e-04,
      -2.2475e-03,-3.3095e-05,-1.2040e-05,+2.2010e-06,-1.0083e-06,
      +8.6297e-01,+5.8231e-01,+2.0545e-02,-7.8110e-03,-1.4085e-04,
      -8.8459e-06,+5.7256e-06,-1.5068e-06,+4.0095e-07,-2.4185e-08 };

   const double GlobalTropModel::Bgeoid[55] = {
      +0.0000e+00,+0.0000e+00,-6.5993e-02,+0.0000e+00,+6.5364e-02,
      -5.8320e+00,+0.0000e+00,+1.6961e+00,-1.3557e+00,+1.2694e+00,
      +0.0000e+00,-2.9310e+00,+9.4805e-01,-7.6243e-02,+4.1076e-02,
      +0.0000e+00,-5.1808e-01,-3.4583e-01,-4.3632e-02,+2.2101e-03,
      -1.0663e-02,+0.0000e+00,+1.0927e-01,-2.9463e-01,+1.4371e-03,
      -1.1452e-02,-2.8156e-03,-3.5330e-04,+0.0000e+00,+4.4049e-01,
      +5.5653e-02,-2.0396e-02,-1.7312e-03,+3.5805e-05,+7.2682e-05,
      +2.2535e-06,+0.0000e+00,+1.9502e-02,+2.7919e-02,-8.1812e-03,
      +4.4540e-04,+8.8663e-05,+5.5596e-05,+2.4826e-06,+1.0279e-06,
      +0.0000e+00,+6.0529e-02,-3.5824e-02,-5.1367e-03,+3.0119e-05,
      -2.9911e-05,+1.9844e-05,-1.2349e-06,-7.6756e-09,+5.0100e-08 };

   const double GlobalTropModel::APressMean[55] = {
      +1.0108e+03,+8.4886e+00,+1.4799e+00,-1.3897e+01,+3.7516e-03,
      -1.4936e-01,+1.2232e+01,-7.6615e-01,-6.7699e-02,+8.1002e-03,
      -1.5874e+01,+3.6614e-01,-6.7807e-02,-3.6309e-03,+5.9966e-04,
      +4.8163e+00,-3.7363e-01,-7.2071e-02,+1.9998e-03,-6.2385e-04,
      -3.7916e-04,+4.7609e+00,-3.9534e-01,+8.6667e-03,+1.1569e-02,
      +1.1441e-03,-1.4193e-04,-8.5723e-05,+6.5008e-01,-5.0889e-01,
      -1.5754e-02,-2.8305e-03,+5.7458e-04,+3.2577e-05,-9.6052e-06,
      -2.7974e-06,+1.3530e+00,-2.7271e-01,-3.0276e-04,+3.6286e-03,
      -2.0398e-04,+1.5846e-05,-7.7787e-06,+1.1210e-06,+9.9020e-08,
      +5.5046e-01,-2.7312e-01,+3.2532e-03,-2.4277e-03,+1.1596e-04,
      +2.6421e-07,-1.3263e-06,+2.7322e-07,+1.4058e-07,+4.9414e-09 };

   const double GlobalTropModel::BPressMean[55] = {
      +0.0000e+00,+0.0000e+00,-1.2878e+00,+0.0000e+00,+7.0444e-01,
      +3.3222e-01,+0.0000e+00,-2.9636e-01,+7.2248e-03,+7.9655e-03,
      +0.0000e+00,+1.0854e+00,+1.1145e-02,-3.6513e-02,+3.1527e-03,
      +0.0000e+00,-4.8434e-01,+5.2023e-02,-1.3091e-02,+1.8515e-03,
      +1.5422e-04,+0.0000e+00,+6.8298e-01,+2.5261e-03,-9.9703e-04,
      -1.0829e-03,+1.7688e-04,-3.1418e-05,+0.0000e+00,-3.7018e-01,
      +4.3234e-02,+7.2559e-03,+3.1516e-04,+2.0024e-05,-8.0581e-06,
      -2.3653e-06,+0.0000e+00,+1.0298e-01,-1.5086e-02,+5.6186e-03,
      +3.2613e-05,+4.0567e-05,-1.3925e-06,-3.6219e-07,-2.0176e-08,
      +0.0000e+00,-1.8364e-01,+1.8508e-02,+7.5016e-04,-9.6139e-05,
      -3.1995e-06,+1.3868e-07,-1.9486e-07,+3.0165e-10,-6.4376e-10 };

   const double GlobalTropModel::APressAmp[55] = {
      -1.0444e-01,+1.6618e-01,-6.3974e-02,+1.0922e+00,+5.7472e-01,
      -3.0277e-01,-3.5087e+00,+7.1264e-03,-1.4030e-01,+3.7050e-02,
      +4.0208e-01,-3.0431e-01,-1.3292e-01,+4.6746e-03,-1.5902e-04,
      +2.8624e+00,-3.9315e-01,-6.4371e-02,+1.6444e-02,-2.3403e-03,
      +4.2127e-05,+1.9945e+00,-6.0907e-01,-3.5386e-02,-1.0910e-03,
      -1.2799e-04,+4.0970e-05,+2.2131e-05,-5.3292e-01,-2.9765e-01,
      -3.2877e-02,+1.7691e-03,+5.9692e-05,+3.1725e-05,+2.0741e-05,
      -3.7622e-07,+2.6372e+00,-3.1165e-01,+1.6439e-02,+2.1633e-04,
      +1.7485e-04,+2.1587e-05,+6.1064e-06,-1.3755e-08,-7.8748e-08,
      -5.9152e-01,-1.7676e-01,+8.1807e-03,+1.0445e-03,+2.3432e-04,
      +9.3421e-06,+2.8104e-06,-1.5788e-07,-3.0648e-08,+2.6421e-10 };

   const double GlobalTropModel::BPressAmp[55] = {
      +0.0000e+00,+0.0000e+00,+9.3340e-01,+0.0000e+00,+8.2346e-01,
      +2.2082e-01,+0.0000e+00,+9.6177e-01,-1.5650e-02,+1.2708e-03,
      +0.0000e+00,-3.9913e-01,+2.8020e-02,+2.8334e-02,+8.5980e-04,
      +0.0000e+00,+3.0545e-01,-2.1691e-02,+6.4067e-04,-3.6528e-05,
      -1.1166e-04,+0.0000e+00,-7.6974e-02,-1.8986e-02,+5.6896e-03,
      -2.4159e-04,-2.3033e-04,-9.6783e-06,+0.0000e+00,-1.0218e-01,
      -1.3916e-02,-4.1025e-03,-5.1340e-05,-7.0114e-05,-3.3152e-07,
      +1.6901e-06,+0.0000e+00,-1.2422e-02,+2.5072e-03,+1.1205e-03,
      -1.3034e-04,-2.3971e-05,-2.6622e-06,+5.7852e-07,+4.5847e-08,
      +0.0000e+00,+4.4777e-02,-3.0421e-03,+2.6062e-05,-7.2421e-05,
      +1.9119e-06,+3.9236e-07,+2.2390e-07,+2.9765e-09,-4.6452e-09 };

   const double GlobalTropModel::ATempMean[55] = {
      +1.6257e+01,+2.1224e+00,+9.2569e-01,-2.5974e+01,+1.4510e+00,
      +9.2468e-02,-5.3192e-01,+2.1094e-01,-6.9210e-02,-3.4060e-02,
      -4.6569e+00,+2.6385e-01,-3.6093e-02,+1.0198e-02,-1.8783e-03,
      +7.4983e-01,+1.1741e-01,+3.9940e-02,+5.1348e-03,+5.9111e-03,
      +8.6133e-06,+6.3057e-01,+1.5203e-01,+3.9702e-02,+4.6334e-03,
      +2.4406e-04,+1.5189e-04,+1.9581e-07,+5.4414e-01,+3.5722e-01,
      +5.2763e-02,+4.1147e-03,-2.7239e-04,-5.9957e-05,+1.6394e-06,
      -7.3045e-07,-2.9394e+00,+5.5579e-02,+1.8852e-02,+3.4272e-03,
      -2.3193e-05,-2.9349e-05,+3.6397e-07,+2.0490e-06,-6.4719e-08,
      -5.2225e-01,+2.0799e-01,+1.3477e-03,+3.1613e-04,-2.2285e-04,
      -1.8137e-05,-1.5177e-07,+6.1343e-07,+7.8566e-08,+1.0749e-09 };

   const double GlobalTropModel::BTempMean[55] = {
      +0.0000e+00,+0.0000e+00,+1.0210e+00,+0.0000e+00,+6.0194e-01,
      +1.2292e-01,+0.0000e+00,-4.2184e-01,+1.8230e-01,+4.2329e-02,
      +0.0000e+00,+9.3312e-02,+9.5346e-02,-1.9724e-03,+5.8776e-03,
      +0.0000e+00,-2.0940e-01,+3.4199e-02,-5.7672e-03,-2.1590e-03,
      +5.6815e-04,+0.0000e+00,+2.2858e-01,+1.2283e-02,-9.3679e-03,
      -1.4233e-03,-1.5962e-04,+4.0160e-05,+0.0000e+00,+3.6353e-02,
      -9.4263e-04,-3.6762e-03,+5.8608e-05,-2.6391e-05,+3.2095e-06,
      -1.1605e-06,+0.0000e+00,+1.6306e-01,+1.3293e-02,-1.1395e-03,
      +5.1097e-05,+3.3977e-05,+7.6449e-06,-1.7602e-07,-7.6558e-08,
      +0.0000e+00,-4.5415e-02,-1.8027e-02,+3.6561e-04,-1.1274e-04,
      +1.3047e-05,+2.0001e-06,-1.5152e-07,-2.7807e-08,+7.7491e-09 };

   const double GlobalTropModel::ATempAmp[55] = {
      -1.8654e+00,-9.0041e+00,-1.2974e-01,-3.6053e+00,+2.0284e-02,
      +2.1872e-01,-1.3015e+00,+4.0355e-01,+2.2216e-01,-4.0605e-03,
      +1.9623e+00,+4.2887e-01,+2.1437e-01,-1.0061e-02,-1.1368e-03,
      -6.9235e-02,+5.6758e-01,+1.1917e-01,-7.0765e-03,+3.0017e-04,
      +3.0601e-04,+1.6559e+00,+2.0722e-01,+6.0013e-02,+1.7023e-04,
      -9.2424e-04,+1.1269e-05,-6.9911e-06,-2.0886e+00,-6.7879e-02,
      -8.5922e-04,-1.6087e-03,-4.5549e-05,+3.3178e-05,-6.1715e-06,
      -1.4446e-06,-3.7210e-01,+1.5775e-01,-1.7827e-03,-4.4396e-04,
      +2.2844e-04,-1.1215e-05,-2.1120e-06,-9.6421e-07,-1.4170e-08,
      +7.8720e-01,-4.4238e-02,-1.5120e-03,-9.4119e-04,+4.0645e-06,
      -4.9253e-06,-1.8656e-06,-4.0736e-07,-4.9594e-08,+1.6134e-09 };

   const double GlobalTropModel::BTempAmp[55] = {
      +0.0000e+00,+0.0000e+00,-8.9895e-01,+0.0000e+00,-1.0790e+00,
      -1.2699e-01,+0.0000e+00,-5.9033e-01,+3.4865e-02,-3.2614e-02,
      +0.0000e+00,-2.4310e-02,+1.5607e-02,-2.9833e-02,-5.9048e-03,
      +0.0000e+00,+2.8383e-01,+4.0509e-02,-1.8834e-02,-1.2654e-03,
      -1.3794e-04,+0.0000e+00,+1.3306e-01,+3.4960e-02,-3.6799e-03,
      -3.5626e-04,+1.4814e-04,+3.7932e-06,+0.0000e+00,+2.0801e-01,
      +6.5640e-03,-3.4893e-03,-2.7395e-04,+7.4296e-05,-7.9927e-06,
      -1.0277e-06,+0.0000e+00,+3.6515e-02,-7.4319e-03,-6.2873e-04,
      -8.2461e-05,+3.1095e-05,-5.3860e-07,-1.2055e-07,-1.1517e-07,
      +0.0000e+00,+3.1404e-02,+1.5580e-02,-1.1428e-03,+3.3529e-05,
      +1.0387e-05,-1.9378e-06,-2.7327e-07,+7.5833e-09,-9.2323e-09 };

   const double GlobalTropModel::Factorial[19] = {
      1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800, 39916800,
      479001600, 6227020800, 87178291200, 1307674368000, 20922789888000,
      355687428096000, 6402373705728000 };

   // Compute and return the full tropospheric delay. The receiver height, 
   // latitude and time must has been set before using the appropriate
   // methods.
   // @param elevation Elevation of satellite as seen at receiver, in degrees
   double GlobalTropModel::correction(double elevation) const       
      throw(InvalidTropModel)
   {
      try { testValidity(); }
      catch(InvalidTropModel& e) { GPSTK_RETHROW(e); }

      // Global mapping functions good down to 3 degrees of elevation
      if(elevation < 3.0) { return 0.0; }

      double map_dry(GlobalTropModel::dry_mapping_function(elevation));
      double map_wet(GlobalTropModel::wet_mapping_function(elevation));

      // Compute total tropospheric delay
      double tropDelay((GlobalTropModel::dry_zenith_delay() * map_dry) +
                       (GlobalTropModel::wet_zenith_delay() * map_wet));

      return tropDelay;

   }  // end GlobalTropModel::correction(elevation)

   // Compute and return the full tropospheric delay, given the
   // positions of receiver and satellite.
   //
   // This version is more useful within positioning algorithms, where
   // the receiver position may vary; it computes the elevation (and
   // other receiver location information as height and latitude) and
   // passes them to appropriate methods.
   //
   // You must set time using method setReceiverDOY() before calling
   // this method.
   //
   // @param RX  Receiver position.
   // @param SV  Satellite position.
   double GlobalTropModel::correction(const Position& RX, const Position& SV)
      throw(InvalidTropModel)
   {
      try {
         double p;
         p = RX.getAltitude();         if(p != height) setReceiverHeight(p);
         p = RX.getGeodeticLatitude(); if(p != latitude) setReceiverLatitude(p);
         p = RX.getLongitude();        if(p != longitude) setReceiverLongitude(p);
      }
      catch(GeometryException& e) {
         validHeight = validLat = valid = false;
         GPSTK_RETHROW(e);
      }

      try { testValidity(); }
      catch(InvalidTropModel& e) { GPSTK_RETHROW(e); }

      double c;
      try {
         c = GlobalTropModel::correction(RX.elevationGeodetic(SV));
      }
      catch(InvalidTropModel& e) { GPSTK_RETHROW(e); }

      return c;

   }  // end GlobalTropModel::correction(RX,SV)

   // Compute and return the zenith delay for hydrostatic (dry) component of
   // the troposphere. Use the Saastamoinen value.
   // Ref. Davis etal 1985 and Leick, 3rd ed, pg 197.
   double GlobalTropModel::dry_zenith_delay(void) const
      throw(InvalidTropModel)
   {
      try { testValidity(); } catch(InvalidTropModel& e) { GPSTK_RETHROW(e); }
      return SaasDryDelay(press,latitude,height);

   }  // end GlobalTropModel::dry_zenith_delay()

   // Compute and return the zenith delay for wet component of
   // the troposphere. Ref. Leick, 3rd ed, pg 197, Leick, 4th ed, pg 482.
   double GlobalTropModel::wet_zenith_delay(void) const
         throw(InvalidTropModel)
   {
      double T = temp + CELSIUS_TO_KELVIN;
      double pwv = 0.01 * humid * ::exp(-37.2465 + (0.213166-0.000256908*T)*T);
      return (0.0122 + 0.00943 * pwv);
   }

   // Compute and return the mapping function for hydrostatic (dry) component of
   // the troposphere.
   // @param elevation Elevation of satellite as seen at receiver, in degrees
   double GlobalTropModel::dry_mapping_function(double elevation) const
      throw(InvalidTropModel)
   {
      try { testValidity(); } catch(InvalidTropModel& e) { GPSTK_RETHROW(e); }
      if(elevation < 3.0) { return 0.0; }

      double clat = ::cos(latitude*DEG_TO_RAD);
      double phh, c11h, c10h;

      static const double bh = 0.0029;
      static const double c0h = 0.062;
      if(latitude < 0) {
         phh = PI;
         c11h = 0.007;
         c10h = 0.002;
      }
      else {
         phh = 0.0;
         c11h = 0.005;
         c10h = 0.001;
      }
      double ch = c0h + ((::cos(dayfactor + phh)+1.0)*c11h/2.0 + c10h)*(1.0-clat);

      double am(0.0), aa(0.0);
      for(int i=0; i<55; i++) {
         am += (ADryMean[i]*aP[i] + BDryMean[i]*bP[i]) * 1.0e-5;
         aa += (ADryAmp[i]*aP[i] + BDryAmp[i]*bP[i]) * 1.0e-5;
      }
      double ah = am + aa*::cos(dayfactor);

      double sine = ::sin(elevation*DEG_TO_RAD);
      //std::cout << "sine " << std::fixed << std::setprecision(16) << sine
      // << std::endl;
      //std::cout << "ah bh ch " << std::fixed << std::setprecision(16)
      // << ah << " " << bh << " " << ch << std::endl;
      double map = (1.0 + ah/(1.0 + bh/(1.0 + ch)))
                 / (sine + ah/(sine + bh/(sine + ch)));
      //std::cout << "map0 " << std::fixed << std::setprecision(16) << map
      // << std::endl;

      // height correction
      static const double a_ht = 2.53e-5;
      static const double b_ht = 5.49e-3;
      static const double c_ht = 1.14e-3;

      map += ( (1.0/sine) - (1.0  + a_ht/(1.0  + b_ht/(1.0  + c_ht)))
                          / (sine + a_ht/(sine + b_ht/(sine + c_ht)))
             ) * (height/1000.0);

      return map;

   }  // end GlobalTropModel::dry_mapping_function()

   // Compute and return the mapping function for wet component of the
   // troposphere.
   // param elevation Elevation of satellite as seen at receiver,
   //
   // computing the derivative - do it numerically instead
   // note that sin[elev] = cos[zenith]
   //             f(1)                               a
   //   map = ------------      where f(x) = x + ---------
   //         f(sin[elev])                             b
   //                                            x + -----
   //                                                x + c
   //
   //                a (x+c)    x(x^2 + xc + b) + a(x+c)
   // f(x) = x + ------------ = ------------------------
   //            x^2 + xc + b        x^2 + xc + b    
   //
   //        x^3 + x^2 c + x(a+b) + ac
   //      = -------------------------
   //               x^2 + xc + b
   //
   // so            -f(1)f'(x)    -map(x)f'(x)                    N'D-D'N
   //     map'(x) = ---------- = -------------,     where f'(x) = -------
   //                 f^2(x)         f(x)                           D^2
   //
   //                [3x^2+2xc+a+b][x^2+xc+b]-[2x+c][x^3+x^2c+x(a+b)+ac]
   //     f'(x) = x' --------------------------------------------------------------
   //                            [x^2 + xc + b]^2
   //
   //                   [3x^4 + x^3(5c) + x^2(a+4b+2c^2) + x(a+b+2bc) + ab+b^2]
   //                + [-2x^4 - x^3(3c) - x^2(2a+2b+c^2) - x(c(3a+b)) - ac^2]
   //           = x'  ----------------------------------------------------------
   //                   x^4 + x^3(2c) + x^2(2b+c^2) + x(2bc) + b^2
   //
   //                x^4 + x^3(2c) + x^2(-a+2b+c^2) + x(a+b+3bc+3ac) + ab+b^2-ac^2
   //           = x' -------------------------------------------------------------
   //                  x^4 + x^3(2c) + x^2(2b+c^2) + x(2bc) + b^2
   //
   double GlobalTropModel::wet_mapping_function(double elevation) const
      throw(InvalidTropModel)
   {
      try { testValidity(); }
      catch(InvalidTropModel& e) { GPSTK_RETHROW(e); }

      if(elevation < 3.0) { return 0.0; }

      static const double bw = 0.00146;
      static const double cw = 0.04391;

      double am(0.0), aa(0.0);
      for(int i=0; i<55; i++) {
         am += (AWetMean[i]*aP[i] + BWetMean[i]*bP[i]) * 1.0e-5;
         aa += (AWetAmp[i]*aP[i] + BWetAmp[i]*bP[i]) * 1.0e-5;
      }
      double aw = am + aa*::cos(dayfactor);

      double sine = ::sin(elevation*DEG_TO_RAD);
      //std::cout << "sine " << std::fixed << std::setprecision(16) << sine
      // << std::endl;
      //std::cout << "aw bw cw " << std::fixed << std::setprecision(16)
      // << aw << " " << bw << " " << cw << std::endl;
      double f1(1.0 + aw/(1.0 + bw/(1.0 + cw)));
      double f(sine + aw/(sine + bw/(sine + cw)));
      double map = f1/f;

      //// NB might be easier numerically... map' = map(elev+eps)-map(elev-eps)/2eps
      //if(doDeriv) {
      //   double apb(aw+bw);
      //   double cw2(cw*cw);
      //   double tmp(sine*(sine + 2*cw) + 2*bw+cw2);
      //   double fpN(sine*(sine*(tmp-aw) + apb*(1.+3*cw)) + bw*apb-aw*cw2);
      //   double fpD(sine*(sine*tmp + 2*bw));
      //   double sinep(::cos(elevation*DEG_TO_RAD));
      //   deriv = - map * (sinep*fpN/fpD) / f;
      //}

      return map;

   }  // end GlobalTropModel::wet_mapping_function()

   // Compute the pressure and temperature at height, and the undulation,
   // for the given position and time.
   void GlobalTropModel::getGPT(double& P, double& T, double& U)
      throw(InvalidTropModel)
   {
      try { testValidity(); }
      catch(InvalidTropModel& e) { GPSTK_RETHROW(e); }
      
      int i;

      // undulation and orthometric height
      U = 0.0;
      for(i=0; i<55; i++) U += (Ageoid[i]*aP[i] + Bgeoid[i]*bP[i]);
      double orthoht(height - U);
      if(orthoht > 44247.) GPSTK_THROW(InvalidTropModel(
                           "Invalid Global trop model: Rx Height is too large"));

      // press at geoid
      double am(0.0),aa(0.0),v0;
      for(i=0; i<55; i++) {
         am += (APressMean[i]*aP[i] + BPressMean[i]*bP[i]);
         aa += (APressAmp[i]*aP[i] + BPressAmp[i]*bP[i]);
      }
      v0 = am + aa * ::cos(dayfactor);
      
      // pressure at height
      // NB this implies any orthoht > 1/2.26e-5 == 44247.78m is invalid!
      P = v0 * ::pow(1.0-2.26e-5*orthoht,5.225);

      // temper on geoid
      am = aa = 0.0;
      for(i=0; i<55; i++) {
         am += (ATempMean[i]*aP[i] + BTempMean[i]*bP[i]);
         aa += (ATempAmp[i]*aP[i] + BTempAmp[i]*bP[i]);
      }
      v0 = am + aa * ::cos(dayfactor);

      // temp at height
      T = v0 - 6.5e-3 * orthoht;
   }

   // Define the receiver height; this is required before calling
   // correction() or any of the zenith_delay routines.
   // @param ht   Height of the receiver above mean sea level, in meters.
   void GlobalTropModel::setReceiverHeight(const double& ht)
   {
      if(height != ht) {
         height = ht; 
         validHeight = true;
         validCoeff = false;
         setValid();          // calls updateGTMCoeff()
      }
   }

   // Define the receiver latitude; this is required before calling
   // correction() or any of the zenith_delay routines.
   // @param lat  Latitude of receiver, in degrees.
   void GlobalTropModel::setReceiverLatitude(const double& lat)
   {
      if(latitude != lat) {
         latitude = lat;
         validLat = true;
         validCoeff = false;
         setValid();          // calls updateGTMCoeff()
      }
   }

   // Define the receiver longitude; this is required before calling
   // correction() or any of the zenith_delay routines.
   // @param lat  Longitude of receiver, in degrees East.
   void GlobalTropModel::setReceiverLongitude(const double& lon)
   {
      if(longitude != lon) {
         longitude = lon;
         validLon = true;
         validCoeff = false;
         setValid();          // calls updateGTMCoeff()
      }
   }

   // Define the day of year; this is required before calling
   // correction() or any of the zenith_delay routines.
   // @param mjd double MJD
   void GlobalTropModel::setTime(const double& mjd)
   {
      double df(TWO_PI*(mjd - 44266.0)/365.25);       // -44239 + 1 - 28
      if(df != dayfactor) {
         dayfactor = df;
         validDay = true;
         validCoeff = false;
         setValid();
      }
   }

   // Define the day of year; this is required before calling
   // correction() or any of the zenith_delay routines.
   // @param time  CommonTime of interest
   void GlobalTropModel::setTime(const CommonTime& time)
   {
      double mjd = static_cast<MJD>(time).mjd;
      setTime(mjd);
   }

   // Define the day of year; this is required before calling
   // correction() or any of the zenith_delay routines.
   // @param doy Day of year (year does not matter)
   void GlobalTropModel::setDayOfYear(const int& doy)
   {
      double mjd = 44266.0 + (double)doy;       // year doesn't matter
      setTime(mjd);
   }

   // Convenient method to set all non-weather model parameters in one call
   // @param time  CommonTime of interest
   // @param rxPos Receiver position object.
   void GlobalTropModel::setParameters(const CommonTime& time, const Position& rxPos)
   {
      validDay = validHeight = validLat = validLon = validCoeff = false;
      setTime(time);
      setReceiverHeight(rxPos.getHeight());
      setReceiverLatitude(rxPos.getGeodeticLatitude());
      setReceiverLongitude(rxPos.getLongitude());

      setValid();          // calls updateGTMCoeff()
   }

   // Must update coeff when latitude or lon changes
   void GlobalTropModel::updateGTMCoeff(void)
   {
      if(!validLon || !validLat) return;

      // compute Legendre functions and spherical harmonics
      int i,j,k;
      double sinlat(::sin(latitude*DEG_TO_RAD));
      for(i=0; i<=9; i++) {
         for(j=0; j<=i; j++) {
            int ir((i-j)/2);
            double sum(0.0),term;
            int sign(1);
            for(k=0; k<=ir; k++) {
               term = sign*(((Factorial[2*i-2*k] / Factorial[k]) / Factorial[i-k])
                        / Factorial[i-j-2*k]) * ::pow(sinlat,i-j-2*k);
               sum += term;
               sign = -sign;
            }
            P[i][j] = (1.0/(::pow(2.0,i)) * ::sqrt(::pow(1.0-sinlat*sinlat,j)) * sum);
         }
      }

      // spherical harmonics
      double rlon(longitude*DEG_TO_RAD);
      i = 0;
      for(j=0; j<=9; j++) {
         for(k=0; k<=j; k++) {
            aP[i] = P[j][k] * ::cos(k*rlon);
            bP[i] = P[j][k] * ::sin(k*rlon);
            i++;
         }
      }

   }

   // Utility to test valid flags
   void GlobalTropModel::testValidity(void) const throw(InvalidTropModel)
   {
      if(!valid) {
         if(!validLat)
            GPSTK_THROW(InvalidTropModel("Invalid Global trop model: Rx Latitude"));
         if(!validHeight)
            GPSTK_THROW(InvalidTropModel("Invalid Global trop model: Rx Height"));
         if(!validDay)
            GPSTK_THROW(InvalidTropModel("Invalid Global trop model: day of year"));

         GPSTK_THROW(InvalidTropModel("Valid flag corrupted in Global trop model"));
      }
   }

} // end namespace gpstk
