#ifndef __PARSE_DATA_H__
#define __PARSE_DATA_H__

#define DATA_BUFFER_LEN    512
#define TURN_DATA_LEN      512
#define PACKET_HEADER      (0x7b)
#define PACKET_TAIL        (0x7e)
#define DOWN_BUFFER_LEN    100
typedef struct tx_packet{	unsigned char        header;	unsigned char        len;	unsigned char        sf;	unsigned char        cal;	int 		         freq;	unsigned int         rfch;	unsigned char        data[DATA_BUFFER_LEN];	unsigned char        end;}TX_PACKET;
void msg_rec();
void data_process(); void send_msg(); void parse_init();

#endif