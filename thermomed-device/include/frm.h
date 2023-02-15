#ifndef FRM_H_
#define FRM_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This module provides basic framing (packetizing) functionalities for byte based serial interfaces
 *  where frames have to be delimited via in-band byte stuffing.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

//#define FRM_TEST_FUNCTIONALITY

typedef uint8_t byte;

/*typedef enum {
	FRM_ST_ENC_UDEF = 0,
	FRM_ST_ENC_IDLE,
	FRM_ST_ENC_TRANSMITTING,
} frm_encode_state_t;*/

typedef enum {
	FRM_ST_DEC_UDEF = 0,
	FRM_ST_DEC_WAIT_FOR_NEW_FRAME,
	FRM_ST_DEC_RECEIVE_BYTE,
	FRM_ST_DEC_RECEIVE_ESCAPED_BYTE,
} frm_decode_state_t;

typedef struct {
	void (*txfun)(void* par, byte b);
	void* par;
//	frm_encode_state_t state;
} frm_encode_contex_t;

typedef uint16_t frm_crc_t;

typedef struct {
	size_t rx_cnt;
	frm_crc_t crc;
	frm_decode_state_t state;
} frm_decode_contex_t;

typedef struct {
	frm_encode_contex_t enc;
	frm_decode_contex_t dec;
} frm_contex_t;

/* Call before using frm_encode.
 * The function frm_encode() calls txfun() for each encoded byte that has to be transmitted.
 */
void frm_init_encode_context(frm_contex_t* c, void (*txfun)(void* par, byte b), void* par);

/*
 * call before using frm_decode
 */
void frm_init_decode_context(frm_contex_t* c);

/*
 * Encodes the bytes in data of length len and calls txfun() for each encoded byte at least once.
 * Therefore no internal message buffers are used, but indeed blocking behavior depends on txfun().
 */
void frm_encode_message(frm_contex_t* c,
				const byte* data,
				size_t len);

/*
 * Decodes a byte b into the buffer rxbuf of maximal length len.
 * If there has been a full valid frame decoded, it returns the total number of decoded bytes stored in rxbuf.
 * Otherwise, the return value is zero.
 * Call the function always with the same context dc, which must be initialized before the first use with frm_init_decode_context().
 */
size_t frm_decode(frm_contex_t* c,
				  byte* rxbuf,
				  size_t len,
				  byte b);

/********* Test functions ************/
#ifdef FRM_TEST_FUNCTIONALITY

typedef struct {
	uint32_t n_fail;
	uint32_t n_pass;
} frm_test_stat;

frm_test_stat frm_test(size_t payload_len, uint32_t n_iter);

#endif

#ifdef __cplusplus
}
#endif

#endif /* FRM_H_ */
