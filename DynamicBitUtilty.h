#pragma once

#include <cstdint>

/** *********DynamicBitUtilty *********
  * This is a collection of functions usefull for operating on dynamicly sized bit fields
  * Currently only supports bit fields backed by uint32_t but features planes exists for
  * similar functions for uint64_t to be added.
  */


namespace Common
{
    namespace DynamicBitUtility
    {

        // Sets the bit at index 'bit' in the bitfield 'bits' to value
        inline void setBit( uint32_t *bits, unsigned bit, uint32_t value )
        {
            unsigned elem = bit / 32;
            bit = bit % 32;
            value &= 1;

            uint32_t mask = ~(1 << bit);

            bits[elem] = (bits[elem] & mask) | ((value&1) << bit);
        }

        // Returns the bit at index 'bit' in the bitfield 'bits'
        inline uint32_t getBit( const uint32_t *bits, unsigned bit )
        {
            unsigned elem = bit / 32;
            bit = bit % 32;
            
            uint32_t mask = 1<< bit;

            return !!(bits[elem] & mask);
        }

        // Sets the bit, and returns the old value
        inline uint32_t swapBit( uint32_t *bits, unsigned bit, uint32_t value )
        {
            unsigned elem = bit / 32;
            bit = bit % 32;
            value &= 1;

            uint32_t mask = ~(1 << bit);

            uint32_t tmp = bits[elem];
            bits[elem]= (tmp & mask) |  ((value&1) << bit);

            return !!(tmp & ~mask);
        }
    }
}