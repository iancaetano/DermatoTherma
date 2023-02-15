/*
 * crc.c
 *
 *  Created on: 6 Mar 2020
 *      Author: fehd
 *
 *  This routines closly follow the wikipedia article on CRC computation:
 *  	https://en.wikipedia.org/wiki/Computation_of_cyclic_redundancy_checks
 *  The CRC algorithm is a variant of the "CRC-CCITT" 16-bit CRC, which uses the polynomial "x^16+x^12+x^5+1".
 *  In order to detect missing zero bytes, the X.25 variant is chosen to compute the CRC:
 *  - Initial 16bits are XORed with 0xFFFF (also called inital value).
 *  - The last 16bits of the message are XORed with 0xFFFF, too.
 *  - The LSB bit of each data bit is considered the MSB for the CRC computation (also called reversed computation).
 *    This corresponds to serial links with LSB-first transmissions and allows for shift-register-based decoding in those cases.
 */

#include "crc.h"

#define CRC_USE_TABLE_BASED_ALGO // if defined, a precomputed table is used -> faster, but uses 2k more RAM or FLASH
#define	CRC_USE_FLASH_TBL        // if defined together with CRC_USE_TABLE_BASED_ALGO, the table in crc_tbl.c is put to the flash to save RAM space

/*
 * source: https://en.wikipedia.org/wiki/Computation_of_cyclic_redundancy_checks
 * setup: CCITT polynomial in X.25 configuration
 * Least significant bit of the data bits is shifted out first (little-endian) -> like UART transmissions
 * x^16+x^12+x^5+1 = 1000 0100 0000 1000 (1) = 0x8408 -> reversed generator polynomial
 *
 */

#ifndef CRC_USE_TABLE_BASED_ALGO
static uint16_t crc_on_the_fly(uint8_t* data, size_t len) {
	uint16_t rem;

	rem = 0xffff; // XOR first two data bytes with ones to detect missing leading zeros

	for(size_t i=0; i<len; i++) {
		rem  = rem ^ data[i];
		for(size_t j=0; j < 8; j++) {   // Assuming 8 bits per byte
			if(rem & 0x0001) {   		// if rightmost (most significant) bit is set
				rem  = (rem >> 1) ^ 0x8408;
			} else {
				rem = rem >> 1;
			}
		}
	}
	rem = ~rem; // XOR last byte with 0xffff to detect missing trailing zeros
  return rem;
}
#endif

static uint16_t crc_tbl(const uint8_t* data, size_t len) {

#ifdef CRC_USE_FLASH_TBL

#include "crc_tbl.c"
    	static const uint16_t little_endian_table[256] = CRC_TBL_INIT;

#else

    	static bool is_init = false;
    	static uint16_t little_endian_table[256];

        if (!is_init) { // Do we need to initialize tables?
    	little_endian_table[0] = 0;
    	for(uint32_t i = 0; i < 256; i++) {
    		uint16_t rem = i;

    		for(size_t j=0; j < 8; j++) {   // Assuming 8 bits per byte
				if(rem & 0x0001) {   		// if rightmost (most significant) bit is set
					rem  = (rem >> 1) ^ 0x8408;
				} else {
					rem = rem >> 1;
				}
			}
    		little_endian_table[i] = rem;

    	}
    	is_init = true;
    }

#endif

	uint16_t rem  = 0xffff; // XOR first data byte with ones to detect missing leading zeros
	for(size_t i=0; i<len; i++) {
		rem = (rem >> 8) ^ little_endian_table[ data[i] ^ (rem&0x00ff) ];
	}
	rem = ~rem; // XOR last byte with 0xffff to detect missing trailing zeros
  return rem;
}

/******************************************************************************/
// Public methods
/******************************************************************************/

uint16_t crc_ccitt16x25(const uint8_t* data, size_t len)
{
	uint16_t crc;

#ifdef CRC_USE_TABLE_BASED_ALGO
	crc = crc_tbl(data, len);
#else
	crc = crc_on_the_fly(data, len);
#endif
	return crc;
}


bool crc_test_ok(void)
{
	uint8_t d[] = {"\0CRC works!\0"};
	uint16_t crc_works = 0x6D2A; // CRC of "\0CRC works!\0" without string termination
	uint16_t crc = crc_ccitt16x25(d, sizeof(d)-1);

	return crc == crc_works;
}
