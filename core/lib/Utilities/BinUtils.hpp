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

/**
 * @file BinUtils.hpp
 * Binary manipulation functions
 */

#ifndef BINUTILS_HPP
#define BINUTILS_HPP

#include <cstring>
#include <algorithm>
#include "gpstkplatform.h"

#ifdef __sun
#include <arpa/nameser_compat.h>
#elif defined (__CYGWIN__)
#include <sys/param.h>
#elif defined (_AIX)
#include <sys/machine.h>
#endif

#include "Exception.hpp"
#include <memory.h>

#ifndef BYTE_ORDER
#ifdef _MSC_VER
#define LITTLE_ENDIAN 1
#define BIG_ENDIAN 0
#define BYTE_ORDER LITTLE_ENDIAN
#else
#error "Platform doesn't support BYTE_ORDER?"
#endif
#endif

/** @defgroup binutilsgroup Binary Data Manipulation Tools */

namespace gpstk
{
      /**
       * Binary data utilities.
       *
       * These functions and macros are to be used for manipulating
       * binary data.
       *
       * @note the implementation of the byte swapping routines here
       *   has been chosen for both performance and correctness.  The
       *   implementation of integer byte swapping, for example,
       *   cannot be used for floating point types.
       */
   namespace BinUtils
   {
         /// @ingroup binutilsgroup
         //@{

         // Why so damn many functions when we used to use templates?
         // Because of optimization, but mostly due to issues of correctness.

         /** Converts Intel little-endian to host byte order.
          * @param[in] p a pointer to the buffer containing the
          *   little-endian value.
          * @param[out] v the value in host byte order.
          * @param[in] pos an optional offset (bytes) into p to decode from.
          */
      inline void buitohs  (const void* p, uint16_t& v, unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buitohs
      inline void buitohl  (const void* p, uint32_t& v, unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buitohs
      inline void buitohll (const void* p, uint64_t& v, unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buitohs
      inline void buitohss (const void* p, int16_t& v,  unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buitohs
      inline void buitohsl (const void* p, int32_t& v,  unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buitohs
      inline void buitohsll(const void* p, int64_t& v,  unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buitohs
      inline void buitohf  (const void* p, float& v,    unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buitohs
      inline void buitohd  (const void* p, double& v,   unsigned pos = 0);
      
         /** Converts Network (big-endian) to host byte order.
          * @param[in] p a pointer to the buffer containing the
          *   big-endian value.
          * @param[out] v the value in host byte order.
          * @param[in] pos an optional offset (bytes) into p to decode from.
          */
      inline void buntohs  (const void* p, uint16_t& v, unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buntohs
      inline void buntohl  (const void* p, uint32_t& v, unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buntohs
      inline void buntohll (const void* p, uint64_t& v, unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buntohs
      inline void buntohss (const void* p, int16_t& v,  unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buntohs
      inline void buntohsl (const void* p, int32_t& v,  unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buntohs
      inline void buntohsll(const void* p, int64_t& v,  unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buntohs
      inline void buntohf  (const void* p, float& v,    unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buntohs
      inline void buntohd  (const void* p, double& v,   unsigned pos = 0);


         /** Converts host byte order to Intel little-endian.
          * @param[in,out] p a pointer to the buffer where the converted
          *   value will be stored.
          * @param[in] v the value in host byte order.
          * @param[in] pos an optional offset (bytes) into p to store
          *   the encoded value.
          */
      inline void buhtois  (void* p, uint16_t v,  unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buhtois
      inline void buhtoil  (void* p, uint32_t v,  unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buhtois
      inline void buhtoill (void* p, uint64_t v,  unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buhtois
      inline void buhtoiss (void* p, int16_t v,   unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buhtois
      inline void buhtoisl (void* p, int32_t v,   unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buhtois
      inline void buhtoisll(void* p, int64_t v,   unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buhtois
      inline void buhtoif  (void* p, float v,     unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buhtois
      inline void buhtoid  (void* p, double v,    unsigned pos = 0);
      
         /** Converts host byte order to Network byte order (big-endian).
          * @param[in,out] p a pointer to the buffer where the converted
          *   value will be stored.
          * @param[in] v the value in host byte order.
          * @param[in] pos an optional offset (bytes) into p to store
          *   the encoded value.
          */
      inline void buhtons  (void* p, uint16_t v,  unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buhtons
      inline void buhtonl  (void* p, uint32_t v,  unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buhtons
      inline void buhtonll (void* p, uint64_t v,  unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buhtons
      inline void buhtonss (void* p, int16_t v,   unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buhtons
      inline void buhtonsl (void* p, int32_t v,   unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buhtons
      inline void buhtonsll(void* p, int64_t v,   unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buhtons
      inline void buhtonf  (void* p, float v,     unsigned pos = 0);
         /// @copydoc gpstk::BinUtils::buhtons
      inline void buhtond  (void* p, double v,    unsigned pos = 0);

         /**
          * Decode the item specified from the string and convert it
          * from network byte order to host byte order.
          * @param[in] str the string from which to obtain data.
          * @param[in] pos offset into the string from which to pull the data.
          * @warn This function does not check for appropriate string length.
          */
      template <class T>
      inline T decodeVar( const std::string& str,
                          std::string::size_type pos );

         /**
          * Decode the item specified from the string and convert it
          * from little-endian byte order to host byte order.
          * @param[in] str the string from which to obtain data.
          * @param[in] pos offset into the string from which to pull the data.
          * @warn This function does not check for appropriate string length.
          */
      template <class T>
      inline T decodeVarLE( const std::string& str,
                            std::string::size_type pos );

         /**
          * Decode and remove the item specified from the head of the string
          * and convert it from network byte order to host byte order.
          * @param[in,out] str the string from which to obtain data.
          * @warn This function does not check for appropriate string length.
          */
      template <class T>
      inline T decodeVar( std::string& str );

         /**
          * Decode and remove the item specified from the head of the string
          * and convert it from little-endian byte order to host byte order.
          * @param[in,out] str the string from which to obtain data.
          * @warn This function does not check for appropriate string length.
          */
      template <class T>
      inline T decodeVarLE( std::string& str );

         /** 
          * Add the network ordered binary representation of a var to the
          * the given string.
          * @param[in] v the object of type T to convert to a string.
          */
      template<class T>
      inline std::string encodeVar( const T& v );

         /** 
          * Add the network ordered binary representation of a var to the
          * the given string.
          * @param[in] v the object of type T to convert to a string.
          * @param[in,out] str the string where the encoded data
          *   will be stored.
          * @param[in] pos the position in str where the encoded
          *   variable will be stored.
          */
      template<class T>
      inline void encodeVar( const T& v, std::string& str, size_t pos=0 );

         /** 
          * Add the little-endian binary representation of a var to the
          * the given string.
          * @param[in] v the object of type T to convert to a string.
          */
      template<class T>
      inline std::string encodeVarLE( const T& v );

         /** 
          * Add the little-endian binary representation of a var to the
          * the given string.
          * @param[in] v the object of type T to convert to a string.
          * @param[in,out] str the string where the encoded data
          *   will be stored.
          * @param[in] pos the position in str where the encoded
          *   variable will be stored.
          */
      template<class T>
      inline void encodeVarLE( const T& v, std::string& str, size_t pos=0 );

         /// This is thrown when there is an error processing a CRC
         /// @ingroup exceptiongroup
      NEW_EXCEPTION_CLASS(CRCException, Exception);

         /**
          * Count the set bits in an 32-bit unsigned integer.
          * Originated due to need in EngNav::checkParity
          */
      inline unsigned short countBits(uint32_t v);

         /// Reflects the lower \a bitnum bits of \a crc
      inline unsigned long reflect (unsigned long crc, 
                                    int bitnum);

         /// Encapsulate parameters for CRC computation
      class CRCParam
      {
      public:
            /** Constructor.
             * @param[in] o polynomial order w/o leading '1' bit.
             * @param[in] p CRC polynomial without the leading '1' bit.
             * @param[in] i initial CRC initial value.
             * @param[in] f final final XOR value.
             * @param[in] d kind of algorithm, true = no augmented zero bits.
             * @param[in] ri reflect the data bytes before processing.
             * @param[in] ro reflect the CRC result before final XOR.
             */
         CRCParam(int o, unsigned long p, unsigned long i, unsigned long f,
                  bool d, bool ri, bool ro);

         int order;              ///< CRC polynomial order w/o leading '1' bit.
         unsigned long polynom;  ///< CRC polynomial w/o the leading '1' bit.
         unsigned long initial;  ///< initial CRC initial value.
         unsigned long final;    ///< final final XOR value.
         bool direct;            ///< kind of algorithm, true = no augmented zero bits.
         bool refin;             ///< reflect the data bytes before processing.
         bool refout;            ///< reflect the CRC result before final XOR.
      };

         /// CCITT CRC parameters
      extern const CRCParam CRCCCITT;
         /// CRC-16 parameters
      extern const CRCParam CRC16;
         /// CRC-32 parameters
      extern const CRCParam CRC32;
         /// CRC-24Q parameters
      extern const CRCParam CRC24Q;

         /**
          * Compute CRC (suitable for polynomial orders from 1 to 32).
          * Does bit-by-bit computation (brute-force, no look-up
          * tables).
          * @param[in] data data to process CRC on.
          * @param[in] len length of data to process (in bytes).
          * @param[in] params see documentation of CRCParam:w
          * @return the CRC value
          */
      inline uint32_t computeCRC(const unsigned char *data,
                                 unsigned long len,
                                 const CRCParam& params);

         /**
          * Calculate an Exclusive-OR Checksum on the string \a str.
          * @param[in] str The encoded data for which the checksum is
          *   to be computed.  The size of this string must be
          *   evenly divisible by wordSize.
          * @param[in] wordSize The size of the checksum in bytes.
          * @return the calculated checksum in the same byte order as \a str.
          * @throws gpstk::InvalidParameter if there is a partial word at 
          *  the end of \a str.
          */
      inline std::string xorChecksum(const std::string& str, unsigned wordSize)
         throw(gpstk::InvalidParameter);

         //@}

      inline unsigned short countBits(uint32_t v)
      {
            // Stolen from http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
         uint32_t c; // store the total here
         static const int S[] = {1, 2, 4, 8, 16}; // Magic Binary Numbers
         static const uint32_t B[] = {0x55555555, 0x33333333, 0x0F0F0F0F,
                                      0x00FF00FF, 0x0000FFFF};

            // ...and if we were to turn this into a loop, it would
            // totally defeat the purpose.  The point here is to be
            // FAST.
         c = v;
         c = ((c >> S[0]) & B[0]) + (c & B[0]);
         c = ((c >> S[1]) & B[1]) + (c & B[1]);
         c = ((c >> S[2]) & B[2]) + (c & B[2]);
         c = ((c >> S[3]) & B[3]) + (c & B[3]);
         c = ((c >> S[4]) & B[4]) + (c & B[4]);

         return c;
      }



      template <class T>
      inline T decodeVar( const std::string& str,
                          std::string::size_type pos )
      {
         T rv;
#if BYTE_ORDER == LITTLE_ENDIAN
         std::string scopy(str, pos, sizeof(T));
         std::reverse(scopy.begin(), scopy.end());
         std::memcpy(&rv, scopy.c_str(), sizeof(T));
#else
         std::memcpy(&rv, &str[pos], sizeof(T));
#endif
         return rv;
      }



      template <class T>
      inline T decodeVarLE( const std::string& str,
                            std::string::size_type pos )
      {
         T rv;
#if BYTE_ORDER == BIG_ENDIAN
         std::string scopy(str, pos, sizeof(T));
         std::reverse(scopy.begin(), scopy.end());
         std::memcpy(&rv, scopy.c_str(), sizeof(T));
#else
         std::memcpy(&rv, &str[pos], sizeof(T));
#endif
         return rv;
      }

      template <class T>
      inline T decodeVar( std::string& str )
      {
         T t = gpstk::BinUtils::decodeVar<T>(str, 0);
         str.erase( 0, sizeof(T) );
         return t;
      }

      template <class T>
      inline T decodeVarLE( std::string& str )
      {
         T t = gpstk::BinUtils::decodeVarLE<T>(str, 0);
         str.erase( 0, sizeof(T) );
         return t;
      }

      template<class T>
      inline std::string encodeVar( const T& v )
      {
         std::string rv((const char*)&v, sizeof(v));
#if BYTE_ORDER == LITTLE_ENDIAN
         std::reverse(rv.begin(), rv.end());
#endif
         return rv;
      }


      template<class T>
      inline void encodeVar( const T& v, std::string& str, size_t pos )
      {
         str.replace(pos, sizeof(T), reinterpret_cast<const char*>(&v),
                     sizeof(T));
#if BYTE_ORDER == LITTLE_ENDIAN
         std::reverse(str.begin()+pos, str.begin()+pos+sizeof(T));
#endif
      }


      template<class T>
      inline std::string encodeVarLE( const T& v )
      {
         std::string rv((const char*)&v, sizeof(v));
#if BYTE_ORDER == BIG_ENDIAN
         std::reverse(rv.begin(), rv.end());
#endif
         return rv;
      }


      template<class T>
      inline void encodeVarLE( const T& v, std::string& str, size_t pos )
      {
         str.replace(pos, sizeof(T), reinterpret_cast<const char*>(&v),
                     sizeof(T));
#if BYTE_ORDER == BIG_ENDIAN
         std::reverse(str.begin()+pos, str.begin()+pos+sizeof(T));
#endif
      }


      inline unsigned long reflect (unsigned long crc, 
                                    int bitnum)
      {
         unsigned long i, j = 1, crcout = 0;

         for (i = (unsigned long)1 << (bitnum - 1); i; i >>= 1)
         {
            if (crc & i)
            {
               crcout |= j;
            }
            j <<= 1;
         }
         return (crcout);
      }


         // This code "stolen" from Sven Reifegerste (zorci@gmx.de).
         // Found at http://rcswww.urz.tu-dresden.de/~sr21/crctester.c
         // from link at http://rcswww.urz.tu-dresden.de/~sr21/crc.html
      inline uint32_t computeCRC(const unsigned char *data,
                                 unsigned long len,
                                 const CRCParam& params)
      {
         uint32_t i, j, c, bit;
         uint32_t crc = params.initial;

            // at first, compute constant bit masks for whole CRC and
            // CRC high bit
         uint32_t crcmask = 
            ((((uint32_t)1 << (params.order - 1)) - 1) << 1) | 1;
         uint32_t crchighbit = (uint32_t)1 << (params.order - 1);

         if (crc && params.direct)
         {
            for (i = 0; i < (uint32_t)params.order; i++)
            {
               bit = crc & 1;
               if (bit)
               {
                  crc ^= params.polynom;
               }
               crc >>= 1;
               if (bit)
               {
                  crc |= crchighbit;
               }
            }
         }


         for (i = 0; i < len; i++)
         {
            c = (uint32_t) * data++;
            if (params.refin)
            {
               c = reflect(c, 8);
            }

            for (j = 0x80; j; j >>= 1)
            {
               bit = crc & crchighbit;
               crc <<= 1;
               if (c & j)
               {
                  crc |= 1;
               }
               if (bit)
               {
                  crc ^= params.polynom;
               }
            }
         }

         for (i = 0; i < (uint32_t)params.order; i++)
         {
            bit = crc & crchighbit;
            crc <<= 1;
            if (bit)
            {
               crc ^= params.polynom;
            }
         }

         if (params.refout)
         {
            crc = reflect(crc, params.order);
         }
         crc ^= params.final;
         crc &= crcmask;

         return crc;
      }

      std::string xorChecksum(const std::string& str, unsigned wordSize)
         throw(gpstk::InvalidParameter)
      {
         size_t strSize = str.size();
         std::string rv(wordSize, 0);
         
         if (strSize % wordSize != 0)
         {
            gpstk::InvalidParameter ip("Incomplete word in string.");
            GPSTK_THROW(ip);
         }
         
         for (size_t i = 0; (i + wordSize - 1) < strSize; i += wordSize)
         {
            for (size_t j = 0; j < wordSize; j++)
            {
               rv[j] ^= str[i+j];
            }
         }
         
         return rv;
      }


      inline void buitohs(const void* p, uint16_t& v, unsigned pos)
      {
         const uint8_t *cp = static_cast<const uint8_t*>(p) + pos;
         const uint16_t *tp = reinterpret_cast<const uint16_t*>(cp);
#if BYTE_ORDER == LITTLE_ENDIAN
         v = *tp;
#else
         v = (((*tp & 0x00ff) << 8) |
              ((*tp & 0xff00) >> 8));
#endif
      }


      inline void buitohl(const void* p, uint32_t& v, unsigned pos)
      {
         const uint8_t *cp = static_cast<const uint8_t*>(p) + pos;
         const uint32_t *tp = reinterpret_cast<const uint32_t*>(cp);
#if BYTE_ORDER == LITTLE_ENDIAN
         v = *tp;
#else
         v = (((*tp & 0x000000ff) << 24) |
              ((*tp & 0x0000ff00) << 8) |
              ((*tp & 0x00ff0000) >> 8) |
              ((*tp & 0xff000000) >> 24));
#endif
      }


      inline void buitohll(const void* p, uint64_t& v, unsigned pos)
      {
         const uint8_t *cp = static_cast<const uint8_t*>(p) + pos;
         const uint64_t *tp = reinterpret_cast<const uint64_t*>(cp);
#if BYTE_ORDER == LITTLE_ENDIAN
         v = *tp;
#else
         v = (((*tp & 0x00000000000000ffULL) << 56) |
              ((*tp & 0x000000000000ff00ULL) << 40) |
              ((*tp & 0x0000000000ff0000ULL) << 24) |
              ((*tp & 0x00000000ff000000ULL) << 8) |
              ((*tp & 0x000000ff00000000ULL) >> 8) |
              ((*tp & 0x0000ff0000000000ULL) >> 24) |
              ((*tp & 0x00ff000000000000ULL) >> 40) |
              ((*tp & 0xff00000000000000ULL) >> 56));
#endif
      }


      inline void buitohss(const void* p, int16_t& v, unsigned pos)
      {
         const uint8_t *cp = static_cast<const uint8_t*>(p) + pos;
         const int16_t *tp = reinterpret_cast<const int16_t*>(cp);
#if BYTE_ORDER == LITTLE_ENDIAN
         v = *tp;
#else
            // mask after shift so sign extension doesn't break stuff
         v = (((*tp & 0x00ff) << 8) |
            ((*tp >> 8) & 0x00ff));
#endif
      }
      
      
      inline void buitohsl(const void* p, int32_t& v, unsigned pos)
      {
         const uint8_t *cp = static_cast<const uint8_t*>(p) + pos;
         const int32_t *tp = reinterpret_cast<const int32_t*>(cp);
#if BYTE_ORDER == LITTLE_ENDIAN
         v = *tp;
#else
            // mask after shift so sign extension doesn't break stuff
         v = (((*tp & 0x000000ff) << 24) |
              ((*tp & 0x0000ff00) << 8) |
              ((*tp & 0x00ff0000) >> 8) |
              ((*tp >> 24) & 0x000000ff));
#endif
      }


      inline void buitohsll(const void* p, int64_t& v, unsigned pos)
      {
         const uint8_t *cp = static_cast<const uint8_t*>(p) + pos;
         const int64_t *tp = reinterpret_cast<const int64_t*>(cp);
#if BYTE_ORDER == LITTLE_ENDIAN
         v = *tp;
#else
            // mask after shift so sign extension doesn't break stuff
         v = (((*tp & 0x00000000000000ffULL) << 56) |
              ((*tp & 0x000000000000ff00ULL) << 40) |
              ((*tp & 0x0000000000ff0000ULL) << 24) |
              ((*tp & 0x00000000ff000000ULL) << 8) |
              ((*tp & 0x000000ff00000000ULL) >> 8) |
              ((*tp & 0x0000ff0000000000ULL) >> 24) |
              ((*tp & 0x00ff000000000000ULL) >> 40) |
              ((*tp >> 56) & 0x00000000000000ffULL));
#endif
      }


      inline void buitohf(const void* p, float& v, unsigned pos)
      {
         const uint8_t *cp = static_cast<const uint8_t*>(p) + pos;
         uint8_t *vp = reinterpret_cast<uint8_t*>(&v);
#if BYTE_ORDER == LITTLE_ENDIAN
         std::memcpy(vp, cp, sizeof(float));
#else
         vp[0] = cp[3];
         vp[1] = cp[2];
         vp[2] = cp[1];
         vp[3] = cp[0];
#endif
      }


      inline void buitohd(const void* p, double& v, unsigned pos)
      {
         const uint8_t *cp = static_cast<const uint8_t*>(p) + pos;
         uint8_t *vp = reinterpret_cast<uint8_t*>(&v);
#if BYTE_ORDER == LITTLE_ENDIAN
         std::memcpy(vp, cp, sizeof(double));
#else
         vp[0] = cp[7];
         vp[1] = cp[6];
         vp[2] = cp[5];
         vp[3] = cp[4];
         vp[4] = cp[3];
         vp[5] = cp[2];
         vp[6] = cp[1];
         vp[7] = cp[0];
#endif
      }

         // network to host conversions

      inline void buntohs(const void* p, uint16_t& v, unsigned pos)
      {
         const uint8_t *cp = static_cast<const uint8_t*>(p) + pos;
         const uint16_t *tp = reinterpret_cast<const uint16_t*>(cp);
#if BYTE_ORDER == BIG_ENDIAN
         v = *tp;
#else
         v = (((*tp & 0x00ff) << 8) |
            ((*tp & 0xff00) >> 8));
#endif
      }


      inline void buntohl(const void* p, uint32_t& v, unsigned pos)
      {
         const uint8_t *cp = static_cast<const uint8_t*>(p) + pos;
         const uint32_t *tp = reinterpret_cast<const uint32_t*>(cp);
#if BYTE_ORDER == BIG_ENDIAN
         v = *tp;
#else
         v = (((*tp & 0x000000ff) << 24) |
            ((*tp & 0x0000ff00) << 8) |
            ((*tp & 0x00ff0000) >> 8) |
            ((*tp & 0xff000000) >> 24));
#endif
      }


      inline void buntohll(const void* p, uint64_t& v, unsigned pos)
      {
         const uint8_t *cp = static_cast<const uint8_t*>(p) + pos;
         const uint64_t *tp = reinterpret_cast<const uint64_t*>(cp);
#if BYTE_ORDER == BIG_ENDIAN
         v = *tp;
#else
         v = (((*tp & 0x00000000000000ffULL) << 56) |
            ((*tp & 0x000000000000ff00ULL) << 40) |
            ((*tp & 0x0000000000ff0000ULL) << 24) |
            ((*tp & 0x00000000ff000000ULL) << 8) |
            ((*tp & 0x000000ff00000000ULL) >> 8) |
            ((*tp & 0x0000ff0000000000ULL) >> 24) |
            ((*tp & 0x00ff000000000000ULL) >> 40) |
            ((*tp & 0xff00000000000000ULL) >> 56));
#endif
      }


      inline void buntohss(const void* p, int16_t& v, unsigned pos)
      {
         const uint8_t *cp = static_cast<const uint8_t*>(p) + pos;
         const int16_t *tp = reinterpret_cast<const int16_t*>(cp);
#if BYTE_ORDER == BIG_ENDIAN
         v = *tp;
#else
            // mask after shift so sign extension doesn't break stuff
         v = (((*tp & 0x00ff) << 8) |
            ((*tp >> 8) & 0x00ff));
#endif
      }
      
      
      inline void buntohsl(const void* p, int32_t& v, unsigned pos)
      {
         const uint8_t *cp = static_cast<const uint8_t*>(p) + pos;
         const int32_t *tp = reinterpret_cast<const int32_t*>(cp);
#if BYTE_ORDER == BIG_ENDIAN
         v = *tp;
#else
            // mask after shift so sign extension doesn't break stuff
         v = (((*tp & 0x000000ff) << 24) |
            ((*tp & 0x0000ff00) << 8) |
            ((*tp & 0x00ff0000) >> 8) |
            ((*tp >> 24) & 0x000000ff));
#endif
      }


      inline void buntohsll(const void* p, int64_t& v, unsigned pos)
      {
         const uint8_t *cp = static_cast<const uint8_t*>(p) + pos;
         const int64_t *tp = reinterpret_cast<const int64_t*>(cp);
#if BYTE_ORDER == BIG_ENDIAN
         v = *tp;
#else
            // mask after shift so sign extension doesn't break stuff
         v = (((*tp & 0x00000000000000ffULL) << 56) |
            ((*tp & 0x000000000000ff00ULL) << 40) |
            ((*tp & 0x0000000000ff0000ULL) << 24) |
            ((*tp & 0x00000000ff000000ULL) << 8) |
            ((*tp & 0x000000ff00000000ULL) >> 8) |
            ((*tp & 0x0000ff0000000000ULL) >> 24) |
            ((*tp & 0x00ff000000000000ULL) >> 40) |
            ((*tp >> 56) & 0x00000000000000ffULL));
#endif
      }


      inline void buntohf(const void* p, float& v, unsigned pos)
      {
         const uint8_t *cp = static_cast<const uint8_t*>(p) + pos;
         uint8_t *vp = reinterpret_cast<uint8_t*>(&v);
#if BYTE_ORDER == BIG_ENDIAN
         std::memcpy(vp, cp, sizeof(float));
#else
         vp[0] = cp[3];
         vp[1] = cp[2];
         vp[2] = cp[1];
         vp[3] = cp[0];
#endif
      }


      inline void buntohd(const void* p, double& v, unsigned pos)
      {
         const uint8_t *cp = static_cast<const uint8_t*>(p) + pos;
         uint8_t *vp = reinterpret_cast<uint8_t*>(&v);
#if BYTE_ORDER == BIG_ENDIAN
         std::memcpy(vp, cp, sizeof(double));
#else
         vp[0] = cp[7];
         vp[1] = cp[6];
         vp[2] = cp[5];
         vp[3] = cp[4];
         vp[4] = cp[3];
         vp[5] = cp[2];
         vp[6] = cp[1];
         vp[7] = cp[0];
#endif
      }

         // host to intel conversions

      inline void buhtois(void* p, uint16_t v, unsigned pos)
      {
         uint8_t *cp = static_cast<uint8_t*>(p) + pos;
         uint16_t *tp = reinterpret_cast<uint16_t*>(cp);
#if BYTE_ORDER == LITTLE_ENDIAN
         *tp = v;
#else
         *tp = (((v & 0x00ff) << 8) |
            ((v & 0xff00) >> 8));
#endif
      }


      inline void buhtoil(void* p, uint32_t v, unsigned pos)
      {
         uint8_t *cp = static_cast<uint8_t*>(p) + pos;
         uint32_t *tp = reinterpret_cast<uint32_t*>(cp);
#if BYTE_ORDER == LITTLE_ENDIAN
         *tp = v;
#else
         *tp = (((v & 0x000000ff) << 24) |
            ((v & 0x0000ff00) << 8) |
            ((v & 0x00ff0000) >> 8) |
            ((v & 0xff000000) >> 24));
#endif
      }


      inline void buhtoill(void* p, uint64_t v, unsigned pos)
      {
         uint8_t *cp = static_cast<uint8_t*>(p) + pos;
         uint64_t *tp = reinterpret_cast<uint64_t*>(cp);
#if BYTE_ORDER == LITTLE_ENDIAN
         *tp = v;
#else
         *tp = (((v & 0x00000000000000ffULL) << 56) |
                ((v & 0x000000000000ff00ULL) << 40) |
                ((v & 0x0000000000ff0000ULL) << 24) |
                ((v & 0x00000000ff000000ULL) << 8) |
                ((v & 0x000000ff00000000ULL) >> 8) |
                ((v & 0x0000ff0000000000ULL) >> 24) |
                ((v & 0x00ff000000000000ULL) >> 40) |
                ((v & 0xff00000000000000ULL) >> 56));
#endif
      }


      inline void buhtoiss(void* p, int16_t v, unsigned pos)
      {
         uint8_t *cp = static_cast<uint8_t*>(p) + pos;
         int16_t *tp = reinterpret_cast<int16_t*>(cp);
#if BYTE_ORDER == LITTLE_ENDIAN
         *tp = v;
#else
            // mask after shift so sign extension doesn't break stuff
         *tp = (((v & 0x00ff) << 8) |
                ((v >> 8) & 0x00ff));
#endif
      }
      
      
      inline void buhtoisl(void* p, int32_t v, unsigned pos)
      {
         uint8_t *cp = static_cast<uint8_t*>(p) + pos;
         int32_t *tp = reinterpret_cast<int32_t*>(cp);
#if BYTE_ORDER == LITTLE_ENDIAN
         *tp = v;
#else
            // mask after shift so sign extension doesn't break stuff
         *tp = (((v & 0x000000ff) << 24) |
                ((v & 0x0000ff00) << 8) |
                ((v & 0x00ff0000) >> 8) |
                ((v >> 24) & 0x000000ff));
#endif
      }


      inline void buhtoisll(void* p, int64_t v, unsigned pos)
      {
         uint8_t *cp = static_cast<uint8_t*>(p) + pos;
         int64_t *tp = reinterpret_cast<int64_t*>(cp);
#if BYTE_ORDER == LITTLE_ENDIAN
         *tp = v;
#else
            // mask after shift so sign extension doesn't break stuff
         *tp = (((v & 0x00000000000000ffULL) << 56) |
                ((v & 0x000000000000ff00ULL) << 40) |
                ((v & 0x0000000000ff0000ULL) << 24) |
                ((v & 0x00000000ff000000ULL) << 8) |
                ((v & 0x000000ff00000000ULL) >> 8) |
                ((v & 0x0000ff0000000000ULL) >> 24) |
                ((v & 0x00ff000000000000ULL) >> 40) |
                ((v >> 56) & 0x00000000000000ffULL));
#endif
      }


      inline void buhtoif(void* p, float v, unsigned pos)
      {
         uint8_t *cp = static_cast<uint8_t*>(p) + pos;
         uint8_t *vp = reinterpret_cast<uint8_t*>(&v);
#if BYTE_ORDER == LITTLE_ENDIAN
         std::memcpy(cp, vp, sizeof(float));
#else
         cp[0] = vp[3];
         cp[1] = vp[2];
         cp[2] = vp[1];
         cp[3] = vp[0];
#endif
      }


      inline void buhtoid(void* p, double v, unsigned pos)
      {
         uint8_t *cp = static_cast<uint8_t*>(p) + pos;
         uint8_t *vp = reinterpret_cast<uint8_t*>(&v);
#if BYTE_ORDER == LITTLE_ENDIAN
         std::memcpy(cp, vp, sizeof(double));
#else
         cp[0] = vp[7];
         cp[1] = vp[6];
         cp[2] = vp[5];
         cp[3] = vp[4];
         cp[4] = vp[3];
         cp[5] = vp[2];
         cp[6] = vp[1];
         cp[7] = vp[0];
#endif
      }

         // host to network conversions

      inline void buhtons(void* p, uint16_t v, unsigned pos)
      {
         uint8_t *cp = static_cast<uint8_t*>(p) + pos;
         uint16_t *tp = reinterpret_cast<uint16_t*>(cp);
#if BYTE_ORDER == BIG_ENDIAN
         *tp = v;
#else
         *tp = (((v & 0x00ff) << 8) |
            ((v & 0xff00) >> 8));
#endif
      }


      inline void buhtonl(void* p, uint32_t v, unsigned pos)
      {
         uint8_t *cp = static_cast<uint8_t*>(p) + pos;
         uint32_t *tp = reinterpret_cast<uint32_t*>(cp);
#if BYTE_ORDER == BIG_ENDIAN
         *tp = v;
#else
         *tp = (((v & 0x000000ff) << 24) |
            ((v & 0x0000ff00) << 8) |
            ((v & 0x00ff0000) >> 8) |
            ((v & 0xff000000) >> 24));
#endif
      }


      inline void buhtonll(void* p, uint64_t v, unsigned pos)
      {
         uint8_t *cp = static_cast<uint8_t*>(p) + pos;
         uint64_t *tp = reinterpret_cast<uint64_t*>(cp);
#if BYTE_ORDER == BIG_ENDIAN
         *tp = v;
#else
         *tp = (((v & 0x00000000000000ffULL) << 56) |
                ((v & 0x000000000000ff00ULL) << 40) |
                ((v & 0x0000000000ff0000ULL) << 24) |
                ((v & 0x00000000ff000000ULL) << 8) |
                ((v & 0x000000ff00000000ULL) >> 8) |
                ((v & 0x0000ff0000000000ULL) >> 24) |
                ((v & 0x00ff000000000000ULL) >> 40) |
                ((v & 0xff00000000000000ULL) >> 56));
#endif
      }


      inline void buhtonss(void* p, int16_t v, unsigned pos)
      {
         uint8_t *cp = static_cast<uint8_t*>(p) + pos;
         int16_t *tp = reinterpret_cast<int16_t*>(cp);
#if BYTE_ORDER == BIG_ENDIAN
         *tp = v;
#else
            // mask after shift so sign extension doesn't break stuff
         *tp = (((v & 0x00ff) << 8) |
                ((v >> 8) & 0x00ff));
#endif
      }
      
      
      inline void buhtonsl(void* p, int32_t v, unsigned pos)
      {
         uint8_t *cp = static_cast<uint8_t*>(p) + pos;
         int32_t *tp = reinterpret_cast<int32_t*>(cp);
#if BYTE_ORDER == BIG_ENDIAN
         *tp = v;
#else
            // mask after shift so sign extension doesn't break stuff
         *tp = (((v & 0x000000ff) << 24) |
                ((v & 0x0000ff00) << 8) |
                ((v & 0x00ff0000) >> 8) |
                ((v >> 24) & 0x000000ff));
#endif
      }


      inline void buhtonsll(void* p, int64_t v, unsigned pos)
      {
         uint8_t *cp = static_cast<uint8_t*>(p) + pos;
         int64_t *tp = reinterpret_cast<int64_t*>(cp);
#if BYTE_ORDER == BIG_ENDIAN
         *tp = v;
#else
            // mask after shift so sign extension doesn't break stuff
         *tp = (((v & 0x00000000000000ffULL) << 56) |
                ((v & 0x000000000000ff00ULL) << 40) |
                ((v & 0x0000000000ff0000ULL) << 24) |
                ((v & 0x00000000ff000000ULL) << 8) |
                ((v & 0x000000ff00000000ULL) >> 8) |
                ((v & 0x0000ff0000000000ULL) >> 24) |
                ((v & 0x00ff000000000000ULL) >> 40) |
                ((v >> 56) & 0x00000000000000ffULL));
#endif
      }


      inline void buhtonf(void* p, float v, unsigned pos)
      {
         uint8_t *cp = static_cast<uint8_t*>(p) + pos;
         uint8_t *vp = reinterpret_cast<uint8_t*>(&v);
#if BYTE_ORDER == BIG_ENDIAN
         std::memcpy(cp, vp, sizeof(float));
#else
         cp[0] = vp[3];
         cp[1] = vp[2];
         cp[2] = vp[1];
         cp[3] = vp[0];
#endif
      }


      inline void buhtond(void* p, double v, unsigned pos)
      {
         uint8_t *cp = static_cast<uint8_t*>(p) + pos;
         uint8_t *vp = reinterpret_cast<uint8_t*>(&v);
#if BYTE_ORDER == BIG_ENDIAN
         std::memcpy(cp, vp, sizeof(double));
#else
         cp[0] = vp[7];
         cp[1] = vp[6];
         cp[2] = vp[5];
         cp[3] = vp[4];
         cp[4] = vp[3];
         cp[5] = vp[2];
         cp[6] = vp[1];
         cp[7] = vp[0];
#endif
      }

   } // end namespace BinUtils
} // end namespace gpstk

#endif
