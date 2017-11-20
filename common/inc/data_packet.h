#ifndef __DATA_PACKET__
#define __DATA_PACKET__
#include <stdint.h>
#include <stdbool.h>


#define GW_EUI 		"0000000000bc614e"  		// eui  16½øÖÆÐ¡Ð´(0xff)
#define P_GW_EUI 	"P0000000000bc614e"
#define N_GW_EUI 	"N0000000000bc614e"


struct Options {
	char connection[100];
	int hacount;
	char* client_key_file;
	char* client_key_pass;
	char* server_key_file;
	char* client_private_key_file;
	int verbose;

};


typedef struct mhdr {
	int Mtype:3;
	int RFU:3;
	int Maior:2;
}MHDR;


struct access_request_data {
	MHDR mhdr;
	uint8_t MACPayload[18];
	uint32_t MIC; 
};


struct access_request_succeed {
	MHDR mhdr;
	uint8_t MACPayload[28];
	uint32_t MIC; 
};


struct access_up_stream_data {
	MHDR mhdr;
	uint8_t MACPayload[28];
	uint32_t MIC; 
};


struct access_down_stream_data {
	MHDR mhdr;
	uint8_t MACPayload[28];
	uint32_t MIC; 
};

struct up_stream_data_rxpks_head {
	uint32_t pv;
	uint32_t token;
	uint32_t actid;
	uint32_t gweui;
};

typedef struct up_stream_data_rxpx_head {
	uint8_t time[27];
	uint32_t tmst;
	uint32_t chan;
	uint32_t rfch;
	float freq;
	int32_t stat;
	uint8_t modu[5];
	uint8_t datr[10];
	uint8_t codr[3];
	int32_t rssi;
	float lsnr;
	uint32_t size;
}USDRH;

struct up_stream_data_rxpk {
	USDRH usdrh;
	uint8_t data[256];
};	

typedef struct up_stream_data_stat {
	uint8_t time[27];
	float lati;
	float longs;
	uint32_t alti;
	uint32_t rxnb;
	uint32_t rxok;
	uint32_t rxfw;
	float ackr;
	uint32_t dwnb;
	uint32_t txnb;
}USDS;	

struct up_stream_data_ack {
	uint8_t error[10];
};


typedef struct down_stream_data_rxpk_head {
	bool imme;
	uint32_t tmst;
	uint8_t time[27];
	float freq;
	uint32_t rfch;
	uint32_t powe;
	uint8_t modu[4];
	uint8_t datr[10];
	uint8_t codr[3];
	bool ipol;
	uint32_t size;
}DSDRH;	


typedef struct down_stream_data_rxpk {
	DSDRH dsdrh;
	uint8_t data[256];
	bool ncrc;
}DSDR;	


struct json_data_header {
	uint8_t pv;
	uint16_t token;
	uint8_t actid;
	uint64_t gweui;
}__attribute__ ((packed)); 
typedef struct json_data_header JSON_DATA_HEADER;


struct json_data_head {
	uint8_t pv;
	uint8_t token;
	uint8_t actid;
	uint8_t gweui[17];
}__attribute__ ((packed)); 
typedef struct json_data_head JSON_DATA_HEAD;

#endif
