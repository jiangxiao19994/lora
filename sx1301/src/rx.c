#include <stdint.h>		
#include <stdbool.h>	
#include <stdio.h>		
#include <string.h>		
#include <signal.h>		
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>		

#include "loragw_hal.h"
#include "loragw_reg.h"
#include "loragw_aux.h"
#include "rx.h"

#include "gpio.h"
#include "fifo.h"
#include "ipc.h"
#include "data_packet.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define DEBUG   (1)

#define TEST_RX1_FREQ 471500000
#define TEST_RX2_FREQ 472300000

#define MSG_SEND 1
#define MSG_REC  2

static float Rx_freq[RX_GROUP_NUM]={RX_GROP0_FREQ,RX_GROP1_FREQ,RX_GROP2_FREQ,RX_GROP3_FREQ,
                                    RX_GROP4_FREQ,RX_GROP5_FREQ,RX_GROP6_FREQ,RX_GROP7_FREQ,
                                    RX_GROP8_FREQ,RX_GROP9_FREQ,RX_GROP10_FREQ,RX_GROP11_FREQ};

enum lgw_radio_type_e radio_type = LGW_RADIO_TYPE_SX1255;
static int group_num;
static int rx_freq[2];
static int channel_num = RX_CHANNEL_NUM;

Fifo_t fifo_buffer;
Fifo_t sx1301_fifo;

char fifo_dat[100*(sizeof(MSG_UP))];
char sx1301_data[200*(sizeof(struct lgw_pkt_rx_s))];

int pkt_num = 0;
int count;
int up_key_id;

extern char * base64_encode( const unsigned char * bindata, char * base64, int binlength );

int offset_tab[RX_CHANNEL_NUM][3] = {
	{-400000, 0, 0,},
	{-200000, 0, 0,},
	{      0, 0, 0,},
	{ 200000, 0, 0,},
	{-400000, 1, 0,},
	{-200000, 1, 0,},
	{      0, 1, 0,},
	{ 200000, 1, 0,},
};

/******************************sx1301复位,启动sx1301时先进行复位***********/
void reset_lgw()
{
    int fd;
    
	fd = open_gpio();
    usleep(100000);
    set_gpio_ouput(fd, 0,26);
    usleep(100000);
    set_gpio_value(fd, 0,26,1);
    usleep(100000);
    set_gpio_value(fd, 0,26,0);
    usleep(100000);
    set_gpio_input(fd, 0,26);
	close_gpio(fd);
}

/********配置网关的8个信道的接收频率*************************************/
void write_rx_cfg(int groupnum)
{
    FILE* fd;

	fd = fopen("./rx_config.txt","w");

	fprintf(fd,"rx_group_num:%d\n",groupnum);  
    fprintf(fd,"rx1_freq:%d\n",TEST_RX1_FREQ);   
    fprintf(fd,"rx2_freq:%d\n",TEST_RX2_FREQ);

    fclose(fd);
}

/***********启动时，设置sx1301的接收频率*******************************/
void read_rx_cfg()
{
    FILE* fd;
    int k;
	char buf[100];
	char cfg_buf[20];
	
	char*strp = NULL;
	char*cfgp=NULL;
	
	int len=0;
    int cfg_len=0;

	fd = fopen("./rx_config.txt","r");
	while((strp =fgets(buf,100,fd)) != NULL)
	{
       if(!DEBUG) 
       {
            if((strp=strstr(buf,"rx_group_num:"))!=NULL)
            {
                cfg_len = 2;
                len = strlen("rx_group_num:");
                for(k=0; k<cfg_len; k++)
                {
                    cfg_buf[k] = *(strp+len+k);
                }
                cfg_buf[k] = '\0';
                cfgp = cfg_buf;
                group_num = atoi(cfgp);
                rx_freq[0] = Rx_freq[group_num] + 400000;
                rx_freq[1] = Rx_freq[group_num] + (2*400000) +400000;
            }
        }
        else
        {
            if((strp=strstr(buf,"rx1_freq:"))!=NULL)
            {
                cfg_len = 9;
                len = strlen("rx1_freq:");
                for(k=0; k<cfg_len; k++)
                {
                    cfg_buf[k] = *(strp+len+k);
                }
                cfg_buf[k] = '\0';
                cfgp = cfg_buf;
                rx_freq[0] = atoi(cfgp)+400000;
            }
            else if((strp=strstr(buf,"rx2_freq:"))!=NULL)
            {
                cfg_len = 9;
                len = strlen("rx2_freq:");
                for(k=0; k<cfg_len; k++)
                {
                    cfg_buf[k] = *(strp+len+k);
                }
                cfg_buf[k] = '\0';
                cfgp = cfg_buf;
                rx_freq[1] = atoi(cfgp)+400000;
            }
       }
	}
	fclose(fd);
}

/**********选择射频芯片********************************/
void select_radio()
{
    if((rx_freq[0] > RX_470_LOW_FREQ) && (rx_freq[0] < RX_470_HIGH_FREQ))
	{
	    radio_type = LGW_RADIO_TYPE_SX1255;
	}
	else
	{
        radio_type = LGW_RADIO_TYPE_SX1257;
	}
}

/*********************设置sx1301的接收模式**********************/
void set_rx()
{
    struct lgw_conf_board_s boardconf;
	struct lgw_conf_rxrf_s rfconf;
	struct lgw_conf_rxif_s ifconf;
	
	int i;
	
    memset(&rfconf, 0, sizeof(rfconf));
	memset(&ifconf, 0, sizeof(ifconf));
	
	for(i=0; i<8; i++){
		offset_tab[i][2] = rx_freq[i/4];
	}
	
    boardconf.lorawan_public = true;
    boardconf.clksrc = 1;
    lgw_board_setconf(boardconf);
    
	for(i=0; i<channel_num; i++){
		rfconf.enable = true;
		rfconf.freq_hz = offset_tab[i][2];
		rfconf.type = radio_type;
		rfconf.rssi_offset = 0;
		rfconf.tx_enable = false;
		lgw_rxrf_setconf(offset_tab[i][1], rfconf);

		ifconf.enable = true;
		ifconf.rf_chain = offset_tab[i][1];
		ifconf.freq_hz = offset_tab[i][0];
		ifconf.datarate = DR_LORA_MULTI;
		lgw_rxif_setconf(i, ifconf); 
	}
	
	/* set configuration for LoRa 'stand alone' channel */
	memset(&ifconf, 0, sizeof(ifconf));
	ifconf.enable = true;
	ifconf.rf_chain = 0;
	ifconf.freq_hz = 0;
	ifconf.bandwidth = BW_250KHZ;
	ifconf.datarate = DR_LORA_SF10;
	lgw_rxif_setconf(8, ifconf); /* chain 8: LoRa 250kHz, SF10, on f0 MHz */
	
	/* set configuration for FSK channel */
	memset(&ifconf, 0, sizeof(ifconf));
	ifconf.enable = true;
	ifconf.rf_chain = 1;
	ifconf.freq_hz = 0;
	ifconf.bandwidth = BW_250KHZ;
	ifconf.datarate = 64000;
	lgw_rxif_setconf(9, ifconf); /* chain 9: FSK 64kbps, on f1 MHz */
}

void printf_cfg_msg()
{
    int i;
    for(i=0;i<channel_num/2;i++)
    {
        printf("channel %d freq: %d \r\n", i, (rx_freq[0]-400000) + i*200000);
    }
    for(i=0;i<channel_num/2;i++)
    {
        printf("channel %d freq: %d \r\n", i+4, (rx_freq[1]-400000) + i*200000);
    }
}


/************封装从终端接收到的数据,主要是填充结构体 MSG_UP *************************/
void packet_data(struct lgw_pkt_rx_s *pkt)
{
				struct lgw_pkt_rx_s *p; 
				MSG_UP rec_up;
				char base64[512];
				
				p = pkt;
			if(p->status ==STAT_CRC_OK)
			{
				memset(rec_up.usdrh.datr, 0, ARRAY_SIZE(rec_up.usdrh.datr));
				memset(rec_up.usdrh.codr, 0, ARRAY_SIZE(rec_up.usdrh.codr));
				memset(rec_up.usdrh.modu, 0, ARRAY_SIZE(rec_up.usdrh.modu));
				memset(rec_up.usdrh.time, 0, ARRAY_SIZE(rec_up.usdrh.time));
				memset(rec_up.mac_data, 0, ARRAY_SIZE(rec_up.mac_data));
				
				rec_up.usdrh.chan = p->if_chain;
				rec_up.usdrh.rfch = p->rf_chain;
				rec_up.usdrh.freq = ((offset_tab[p->if_chain][2]+offset_tab[p->if_chain][0])/1e6);
				rec_up.usdrh.tmst = p->count_us;
				rec_up.usdrh.lsnr = p->snr;
				rec_up.usdrh.size = p->size;
				rec_up.usdrh.rssi = (int32_t)p->rssi;
				
				switch (p->status)
				{
					case STAT_CRC_OK: rec_up.usdrh.stat = 1; break;
					case STAT_CRC_BAD: rec_up.usdrh.stat = -1; break;
					case STAT_NO_CRC:  rec_up.usdrh.stat = 0; break;
					default : break;
				}
				
				switch (p->coderate) 
				{
					case CR_LORA_4_5: memcpy(rec_up.usdrh.codr,"4/5",strlen("4/5")); break;
					case CR_LORA_4_6: memcpy(rec_up.usdrh.codr,"2/3",strlen("2/3")); break;
					case CR_LORA_4_7: memcpy(rec_up.usdrh.codr,"4/7",strlen("4/7")); break;
					case CR_LORA_4_8: memcpy(rec_up.usdrh.codr,"1/2",strlen("1/2")); break;
					default: printf(" coderate?");break;
				}
				
				switch (p-> modulation) 
				{
					case MOD_LORA: memcpy(rec_up.usdrh.modu,"LORA",strlen("LORA")); break;
					case MOD_FSK:  memcpy(rec_up.usdrh.modu,"FSK",strlen("FSK"));  break;
					default: printf(" modulation?");break;
				}
				
				switch (p->datarate) 
				{
					case DR_LORA_SF7: memcpy(rec_up.usdrh.datr,"SF7BW125",strlen("SF7BW125")); break;
					case DR_LORA_SF8: memcpy(rec_up.usdrh.datr,"SF8BW125",strlen("SF8BW125")); break;
					case DR_LORA_SF9: memcpy(rec_up.usdrh.datr,"SF9BW125",strlen("SF9BW125")); break;
					case DR_LORA_SF10: memcpy(rec_up.usdrh.datr,"SF10BW125",strlen("SF10BW125")); break;
					case DR_LORA_SF11: memcpy(rec_up.usdrh.datr,"SF11BW125",strlen("SF11BW125")); break;
					case DR_LORA_SF12: memcpy(rec_up.usdrh.datr,"SF12BW125",strlen("SF12BW125")); break;
					default: printf(" datarate?");break;
				}
				switch (p->bandwidth)
				{
					case BW_125KHZ: //strcat(rec_up.usdrh.datr,"BW125"); break;
					case BW_250KHZ: 
					case BW_500KHZ:
					default:break;
				}
				memcpy(rec_up.usdrh.time,"2013-03-31T16:21:17.528002Z",strlen("2013-03-31T16:21:17.528002Z"));
				
				base64_encode( p->payload, base64, p->size );
				
				memcpy(rec_up.mac_data, base64, 512);
				
				PushBuffer( &fifo_buffer, (char *)&rec_up,sizeof(MSG_UP) );
		    }
				
}

/*****************将封装好的数据发送至封装json包的进程*****************************/
void up_send()
{
	    MSG_UP send_msg;
		while(1)
		{
			if(IsFifoEmpty(&fifo_buffer) == false)
			{
				PopBuffer(&fifo_buffer,(char *)&send_msg, sizeof(MSG_UP));
			
				if ( msg_up_send(up_key_id, (char *)&send_msg, sizeof(send_msg), MSG_SEND) != 0 )
				{
					printf("MSG send failed!\r\n");
					printf("MSG ID: %d\r\n",up_key_id);
				}
				else
				{
					printf("MSG ID: %d\r\n",up_key_id);
				}
			}
		}
}

void rx_init()
{
    reset_lgw();

	FifoInit(&sx1301_fifo,sx1301_data,ARRAY_SIZE(sx1301_data));
    FifoInit(&fifo_buffer,fifo_dat,ARRAY_SIZE(fifo_dat));
    
	up_key_id = msg_get(UP_KEY);
	if(up_key_id == -1)
	{
		up_key_id = msg_creat(UP_KEY);
		{
			if(up_key_id == -1)
			{
				printf("msg_creat up_key_msg fail\n");
			}
			else
			{
				printf("msg_creat up_key_msg success\n");
			}
		}
	}
	else
	{
		printf("msg_up_key is created \n");
		printf("MSG ID : %d \n",up_key_id);
	}
}
/*********接收从封装JSON包进程下发的数据包*************/
int rec_loop()
{
	struct lgw_pkt_rx_s rxpkt[4]; 
	int i;
	int nb_pkt;
	int j;
	struct lgw_pkt_rx_s *p;
	
	
    read_rx_cfg();
    select_radio();
    set_rx();
    printf_cfg_msg();
    
	if ((i = lgw_start()) != LGW_HAL_SUCCESS) 
	{
		printf("*** Impossible to start concentrator ***\n");
		return -1;
	}
	
	while (1) 
	{
		
		nb_pkt = lgw_receive(ARRAY_SIZE(rxpkt), rxpkt);
		
		

		if (nb_pkt == 0) {
			wait_ms(10);
		} 
		else 
		{	
			for(i=0; i < nb_pkt; ++i) 
			{
				PushBuffer(&sx1301_fifo, (char *)&rxpkt[i], sizeof(struct lgw_pkt_rx_s));
				
			}
		}
	}
	
    lgw_stop();

	return 0;
}

/******将接收到的终端数据封装成固定的数据格式*************/
void lgw_up_packet()
{
	int j;
	struct lgw_pkt_rx_s rxpkt;
	struct lgw_pkt_rx_s *p = &rxpkt;
	while(1)
	{
		if(IsFifoEmpty(&sx1301_fifo) == false)
		{
			PopBuffer(&sx1301_fifo,(char*)&rxpkt, sizeof(struct lgw_pkt_rx_s));
			sx1301_fifo.len --;	
			printf("sx1301 buffer len : %d\n", sx1301_fifo.len);
			#if 1
				
			packet_data(&rxpkt);
				
			#endif
			#if 1
				printf("\nRcv pkt #%d >>", pkt_num);
				printf("freq:%d\n", offset_tab[p->if_chain][2]+offset_tab[p->if_chain][0]);
				if (p->status == STAT_CRC_OK) 
				{
					printf(" if_chain:%2d", p->if_chain);
					printf(" tstamp:%010u", p->count_us);
					printf(" size:%3u", p->size);
					switch (p-> modulation) 
					{
						case MOD_LORA: printf(" LoRa"); break;
						case MOD_FSK: printf(" FSK"); break;
						default: printf(" modulation?");
					}
					switch (p->datarate) 
					{
						case DR_LORA_SF7: printf(" SF7"); break;
						case DR_LORA_SF8: printf(" SF8"); break;
						case DR_LORA_SF9: printf(" SF9"); break;
						case DR_LORA_SF10: printf(" SF10"); break;
						case DR_LORA_SF11: printf(" SF11"); break;
						case DR_LORA_SF12: printf(" SF12"); break;
						default: printf(" datarate?");
					}
					switch (p->coderate) 
					{
						case CR_LORA_4_5: printf(" CR1(4/5)"); break;
						case CR_LORA_4_6: printf(" CR2(2/3)"); break;
						case CR_LORA_4_7: printf(" CR3(4/7)"); break;
						case CR_LORA_4_8: printf(" CR4(1/2)"); break;
						default: printf(" coderate?");
					}
					printf("\n");
					printf(" RSSI:%+6.1f SNR:%+5.1f (min:%+5.1f, max:%+5.1f) payload:\n", p->rssi, p->snr, p->snr_min, p->snr_max);
					
					for (j = 0; j < p->size; ++j) 
					{
						printf(" %02X", p->payload[j]);
					}
					printf(" #\n");
				} 
				else if (p->status == STAT_CRC_BAD) 
				{
					printf(" if_chain:%2d", p->if_chain);
					printf(" tstamp:%010u", p->count_us);
					printf(" size:%3u\n", p->size);
					printf(" CRC error, damaged packet\n\n");
				} 
				else if (p->status == STAT_NO_CRC)
				{
					printf(" if_chain:%2d", p->if_chain);
					printf(" tstamp:%010u", p->count_us);
					printf(" size:%3u\n", p->size);
					printf(" no CRC\n\n");
				} 
				else 
				{
					printf(" if_chain:%2d", p->if_chain);
					printf(" tstamp:%010u", p->count_us);
					printf(" size:%3u\n", p->size);
					printf(" invalid status ?!?\n\n");
				}
			#endif
		}
		
		}
	}			
/* --- EOF ------------------------------------------------------------------ */
