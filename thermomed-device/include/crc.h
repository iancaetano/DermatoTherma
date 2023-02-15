/*
 * crc.h
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

#ifndef CRC_NR_H_
#define CRC_NR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


uint16_t crc_ccitt16x25(const uint8_t* data, size_t len);

bool crc_test_ok(void); // for debugging

#ifdef __cplusplus
}
#endif

#endif /* CRC_NR_H_ */