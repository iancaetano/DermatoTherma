/*
 * framing.c
 *
 *  Created on: Mar 20, 2018
 *      Author: fehd
 */

#include "frm.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

#include <stdio.h>

#include "crc.h"

#define SIZE(arrayname) (sizeof(arrayname)/sizeof(arrayname[0]))


//#define B_SOF 0x7E
//#define B_ESC 0x7D
//#define R_EOF 0x7C

#define B_SOF 0x7B
#define B_ESC 0x5C
#define R_EOF 0x7D
#define B_XOR 0x20

#define NB_FRM_DELIMITERS 2 // SOF + EOF
#define NB_CRC (sizeof(frm_crc_t))
#define NB_MINIMAL_MESSAGE_LENGTH (NB_FRM_DELIMITERS + NB_CRC)

static void start_new_frame(frm_decode_contex_t* dc)
{
	dc->state = FRM_ST_DEC_RECEIVE_BYTE;
	dc->rx_cnt = 0;
}

static void store_new_byte(frm_decode_contex_t* dc, byte* data, byte b)
{
	assert(NB_CRC >= 1);

	byte *crc_bytes;
	crc_bytes = (byte *)&dc->crc; // holds last (two) received bytes -> only corresponds to crc bytes if a full packet has been received

	size_t i = 0;
	if(dc->rx_cnt >= NB_CRC) {
		data[dc->rx_cnt-NB_CRC] = crc_bytes[i];
	}
	i++;
	for(; i<NB_CRC; i++) {
		crc_bytes[i-1] = crc_bytes[i];
	}
	crc_bytes[NB_CRC-1] = b;
	dc->rx_cnt++;
}

/******************************************************************************/
// Public methods
/******************************************************************************/

void frm_init_decode_context(frm_contex_t* c)
{
	assert(c != NULL);

	c->dec.state = FRM_ST_DEC_WAIT_FOR_NEW_FRAME;
	c->dec.rx_cnt = 0;
}

void frm_init_encode_context(frm_contex_t* c, void (*txfun)(void* par, byte b), void* par)
{
	assert(c != NULL);
	assert(txfun != NULL);

	c->enc.txfun = txfun;
	c->enc.par = par;
//	  c->enc.state = FRM_ST_ENC_IDLE;
}

//void frm_encode_start(frm_contex_t* c)
//{
//	assert(c != NULL);
//	assert(c->enc.state != FRM_ST_ENC_TRANSMITTING);
//
//	c->enc.state = FRM_ST_ENC_TRANSMITTING;
//}
//
//void frm_encode_byte(frm_contex_t* c, byte b)
//{
//	assert(c != NULL);
//	assert(c == FRM_ST_ENC_TRANSMITTING);
//
//	c->enc.state = FRM_ST_ENC_IDLE;
//}
//
//void frm_encode_end(frm_contex_t* c)
//{
//	assert(c != NULL);
//	assert(c == FRM_ST_ENC_TRANSMITTING);
//
//	c->enc.state = FRM_ST_ENC_IDLE;
//}

#ifdef FRM_TEST_FUNCTIONALITY
static void printashex(byte* buf, size_t len);
#endif

void frm_encode_message(frm_contex_t* c, const byte* data, size_t len)
{
	assert(c != NULL);
	assert(data != NULL);
//	assert(c->enc.state != FRM_ST_ENC_TRANSMITTING);

	frm_crc_t crc;
	frm_encode_contex_t* ec = &c->enc;
	crc = crc_ccitt16x25(data, len);
#ifdef FRM_TEST_FUNCTIONALITY
	printf("crc tx: "); printashex((byte *)&crc, NB_CRC); printf("\n");
#endif

	ec->txfun(ec->par, B_SOF);
	for(size_t i = 0; i < len + NB_CRC; i++) {
		byte b;
		if(i<len) {
			// send payload
			b = data[i];
		} else {
			// append CRC
			byte *crc_bytes;
			crc_bytes = (byte *)&crc;
			b = crc_bytes[i-len];
		}
		switch(b) {
			case B_SOF:
			case B_ESC:
			case R_EOF:
				ec->txfun(ec->par, B_ESC);
				ec->txfun(ec->par, b ^ B_XOR);
				break;
			default:
				ec->txfun(ec->par, b);
				break;
		}
	}
	ec->txfun(ec->par, R_EOF);
}

size_t frm_decode(frm_contex_t* c, byte* buf, size_t buflen, byte b)
{
	assert(c != NULL);
	assert(buf != NULL);

	bool end_of_message;
	size_t rx_data_len;

	frm_decode_contex_t* dc = &c->dec;

	end_of_message = false;
	switch(dc->state) {
	case FRM_ST_DEC_WAIT_FOR_NEW_FRAME:
		if(b == B_SOF) {
			start_new_frame(dc);
		}
		break;
	case FRM_ST_DEC_RECEIVE_BYTE:
		switch(b) {
		case B_SOF:
			start_new_frame(dc);
			break;
		case B_ESC:
			dc->state = FRM_ST_DEC_RECEIVE_ESCAPED_BYTE;
			break;
		case R_EOF:
			dc->state = FRM_ST_DEC_WAIT_FOR_NEW_FRAME;
			if(dc->rx_cnt >= NB_CRC) { // zero data packet is valid, but CRC must be included
				end_of_message = true;
				rx_data_len = dc->rx_cnt-NB_CRC;
			}
			break;
		default:
			dc->state = FRM_ST_DEC_RECEIVE_BYTE;
			if(dc->rx_cnt < buflen+NB_CRC) {
				store_new_byte(dc, buf, b);
			} else {
				dc->state = FRM_ST_DEC_WAIT_FOR_NEW_FRAME;
			}
			break;
		}
		break;
	case FRM_ST_DEC_RECEIVE_ESCAPED_BYTE:
		switch(b) {
		case B_SOF:
			start_new_frame(dc);
			break;
		case B_ESC:   // error -> reinit
		case R_EOF:
			dc->state = FRM_ST_DEC_WAIT_FOR_NEW_FRAME;
			break;
		default:
			dc->state = FRM_ST_DEC_RECEIVE_BYTE;
			if(dc->rx_cnt < buflen+NB_CRC) {
				store_new_byte(dc, buf, b ^ B_XOR);
			} else {
				dc->state = FRM_ST_DEC_WAIT_FOR_NEW_FRAME;
			}
			break;
		}
		break;
	case FRM_ST_DEC_UDEF:  // error -> reinit
	default:
		dc->state = FRM_ST_DEC_WAIT_FOR_NEW_FRAME;
		break;
	}

	size_t retval = 0;
	if(end_of_message) {
		frm_crc_t crc;
		crc = crc_ccitt16x25(buf, rx_data_len);
		if(dc->crc == crc) {
			retval = rx_data_len;
		}
#ifdef FRM_TEST_FUNCTIONALITY
		printf("crc rx: "); printashex((byte *)&dc->crc, NB_CRC); printf("\n");
		printf("crc cp: "); printashex((byte *)&crc, NB_CRC); printf("\n");
#endif
	}
	return retval;
}


/********* Test functions ************/
#ifdef FRM_TEST_FUNCTIONALITY

typedef struct {
	byte* combuf;
	size_t* com_write_idx;
} txfun_par_t;

static void printashex(byte* buf, size_t len)
{
	for (size_t i = 0; i < len; i++) {
		printf("%02X ", buf[i]);
	}
}

static void txfun_dummy(void* par, byte b)
{
	txfun_par_t* txfun_par = (txfun_par_t*)par;
	txfun_par->combuf[*txfun_par->com_write_idx] = b;
	(*txfun_par->com_write_idx)++;
}

static frm_test_stat do_randomized_test_iterations(size_t data_len, uint32_t n_iter)
/*
 *  Simulates the encoding and decoding of n_iter many messages with a random payload of
 *  length data_len bytes. Initializes the context only once for all the messages.
 */
{

	byte txdata[data_len];
	byte rxbuf[NB_FRM_DELIMITERS + 2*data_len];

	// buffer to simulate conncetion between encoder and decoder
	byte combuf[NB_FRM_DELIMITERS + 2*data_len];
	size_t com_write_idx;
	size_t com_read_idx;

	txfun_par_t txfun_par;
	txfun_par.combuf = combuf;
	txfun_par.com_write_idx = &com_write_idx;

	// instantiate encoder and decoder
	frm_contex_t c;
	frm_init_encode_context(&c, txfun_dummy, &txfun_par);
	frm_init_decode_context(&c);

	// stats
	frm_test_stat stat;
	stat.n_pass = 0;
	stat.n_fail = 0;

	for(uint32_t i = 0; i < n_iter; i++) {
		com_read_idx = 0;
		com_write_idx = 0;

		printf("#### iteration %d ####\n", i);

		// generate test payload
		for(size_t ii = 0; ii < data_len; ii++)
		{
			txdata[ii] = rand() % 0xff;
		}

		// encode message
		frm_encode_message(&c, txdata, sizeof(txdata));

		printf("payload tx:    "); printashex(txdata, sizeof(txdata)); printf("\n");
		printf("frame     : "); printashex(combuf, sizeof(combuf)); printf("\n");

		// decode correct message
		size_t n_rx = 0;
		while(n_rx == 0
			  && com_read_idx <= com_write_idx) {
			n_rx = frm_decode(&c, rxbuf, sizeof(rxbuf), combuf[com_read_idx]);
			com_read_idx++;
		}
		bool successful_decode_indicated = n_rx == data_len;

		printf("payload rx:    "); printashex(rxbuf, n_rx); printf("\n");

		// compare with original test payload
		bool data_equal = true;
		for(size_t ii = 0; ii < data_len; ii++)
		{
			data_equal &= txdata[ii] == rxbuf[ii];
		}

		// corrupt raw byte
		size_t idx_corrupt_byte = rand() % com_write_idx;
		combuf[idx_corrupt_byte] ^= (rand() % 0xfe)+1;

		printf("frame cor : "); printashex(combuf, sizeof(combuf)); printf("\n");

		// decode corrupted message
		com_read_idx = 0;
		n_rx = 0;
		while(n_rx == 0
			  && com_read_idx <= com_write_idx) {
			n_rx = frm_decode(&c, rxbuf, sizeof(rxbuf), combuf[com_read_idx]);
			com_read_idx++;
		}
		bool corruption_detected = n_rx == 0;

		printf("payload rx:    "); printashex(rxbuf, n_rx); printf("\n");

		// update test statistic
		if(successful_decode_indicated
				&& data_equal
				&& corruption_detected)
		{
			stat.n_pass++;
		} else {
			stat.n_fail++;
		}
		printf("=> suc=%d, dec=%d, cor=%d\n", successful_decode_indicated,data_equal,corruption_detected);
	}
	return stat;
}

frm_test_stat frm_test(size_t payload_len, uint32_t n_iter)
{
	frm_test_stat stat;

	stat = do_randomized_test_iterations(payload_len, n_iter);

	return stat;
}

#endif