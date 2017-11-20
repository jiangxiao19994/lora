#include "stdio.h"
#include "string.h"
#include <unistd.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "pthread.h"
#include "fifo.h"
#include "ipc.h"
#include "radio.h"
#include "data_packet.h"
#include "parse_data.h"
 #include "sx1276api.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz                                                            //  3: Reserved]
#define LORA_SPREADING_FACTOR                       12        // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         5         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        true
#define RF_FREQUENCY                                471500000 // Hz
#define TX_OUTPUT_POWER                             20        // dBm

#define SX1278_NUM                                  1

int gpio_fd;
Fifo_t down_buf;
//Fifo_t send_buf;

static RadioEvents_t RadioEvents;

char down_buffer[DOWN_BUFFER_LEN*(sizeof(MSG_DOWN))];
//char send_buffer[20*(sizeof(TX_PACKET))];

int down_id;

unsigned int  sx1278_freq = RF_FREQUENCY;
unsigned char sx1278_sf = LORA_SPREADING_FACTOR;
unsigned char sx1278_cal = LORA_CODINGRATE;
unsigned char sx1278_flag[8] = {1 , 1 ,1, 1, 1, 1, 1, 1};

int s1278flag[] = {0,  0,  0,  0,  0,  0,  0,  0};
int cs_group[] =  {0,  0,  1,  1,  1,  1,  1,  0};
int cs_num[] =    {23, 23, 12, 17, 16, 23, 20, 6};

extern int base64_decode( const char * base64, unsigned char * bindata );


void parse_pkt_msg(MSG_DOWN msg,TX_PACKET *pkt)
{
	MSG_DOWN tmp = msg;
	char *cal_str;
	unsigned char bindata[2050];
	int i;

	pkt->header = PACKET_HEADER ;
	pkt->end = PACKET_TAIL;
	pkt->freq = (tmp.dsdrh.freq * 1e6);
	
	size_t bytes = base64_decode( tmp.mac_data, bindata );	
	
	memset(pkt->data, '\0', ARRAY_SIZE(pkt->data));
	memcpy(pkt->data, bindata, bytes);

	pkt->len = bytes;
	switch (tmp.dsdrh.datr[2])
	{
		case '7': pkt->sf = 0x07; break;
		case '8': pkt->sf = 0x08; break;
		case '9': pkt->sf = 0x09; break;
		case '1':
		{
			switch (tmp.dsdrh.datr[3])
			{
				case '0': pkt->sf = 0x0a; break;
				case '1': pkt->sf = 0x0b; break;
				case '2': pkt->sf = 0x0c; break;
			}
		}
		default: break;
	}
	
	 cal_str = (char *)malloc(sizeof(char) * (ARRAY_SIZE(tmp.dsdrh.codr) + 1));
	 strncpy(cal_str, tmp.dsdrh.codr, ARRAY_SIZE(tmp.dsdrh.codr));

	 if (strncmp(cal_str, "4/5", 3) == 0)
	 {
		 pkt->cal = 0x01;
		 free(cal_str);
	 }
	 else
	 {
		 
	 }
}


void msg_quene_init()

{
	down_id= msg_get(DOWN_KEY);
	if(down_id== -1)
	{
		down_id= msg_creat(DOWN_KEY);
		{
			if(down_id== -1)
			{
				printf("msg_creat down_key_msg fail\n");
			}
			else
			{
				printf("msg_creat down_key_msg success\n");
			}
		}
	}
	else
	{
		printf("msg_down_key is created \n");
		printf("MSG ID : %d \n", down_id);
	}
}


void msg_rec()
{
	MSG_DOWN rec_data;
	int ret;
	while(1)
	{
		ret = msg_down_recv(down_id,(char *)&rec_data, sizeof(rec_data), 2);
		printf("msg_down_recv ret=%d\n",ret);
		if (ret > 0)
		{
			printf("PushBuffer Size=%d\n",down_buf.Size);
			PushBuffer(&down_buf,(char *)&rec_data,sizeof(MSG_DOWN));
		}
		
	}
}

void OnRadioTxDone()
{
	printf("send success!\n");
}

void send_sx1278(TX_PACKET *tx_data)
{
    char sx1278_status[8];//判断8个sx1278的发送是否完成  
    int i;//索引号,0代表第0个通道的sx1278发射模块的状态，0:代表发送状态就绪，1:正在发射
    int flag = 1;
    while(flag)
    {
        read(gpio_fd, sx1278_status, 8);
        for(i=0; i<SX1278_NUM; i++)
        {
            if(sx1278_status[i] == 0)
            {
                select_cs(cs_group[i],cs_num[i]);
                if(tx_data->freq != sx1278_freq)
                {
                    sx1278_freq = tx_data->freq;
                    SX1276SetChannel( sx1278_freq );
                }
                if((tx_data->sf != sx1278_sf) || (tx_data->cal != sx1278_cal))
                {
                    sx1278_sf = tx_data->sf;
                    sx1278_cal = tx_data->cal;
               
                    SX1276SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                                     sx1278_sf, sx1278_cal,
                                                     LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                                     true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
                }

                if(s1278flag[i] == 0)
                {
                    s1278flag[i] = 1;
                    
                }
                else
                {
                    SX1276OnDio0Irq();
                }
                write(gpio_fd,&i,1 );
				printf("Radio.Send sx1278_sf=%d\n",sx1278_sf);
				printf("Radio.Send sx1278_cal=%d\n",sx1278_cal);
				printf("Radio.Send tx_data.len=%d\n",tx_data->len);
                SX1276Send(tx_data->data,tx_data->len);//负载长度不能大于255
                printf("Radio.Send succeed!!!\n");
                flag = 0;
                break;
            }  
       }
    }
}
//fd:gpio的fd

void fifo_init()
{
	FifoInit(&down_buf, down_buffer, ARRAY_SIZE(down_buffer));
	//FifoInit(&send_buf, send_buffer, ARRAY_SIZE(send_buffer));
}

void sx1278_init()
{
    int i;
    
    RadioEvents.TxDone = OnRadioTxDone;
   
    for(i=0; i<SX1278_NUM; i++)
    {
        
        select_cs(cs_group[i],cs_num[i]);
        SX1276Reset(i);
        SX1276Init( &RadioEvents);
        SX1276SetPublicNetwork(true);
           
        SX1276SetChannel( RF_FREQUENCY );
        SX1276SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                      LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                      LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                      true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
    }

}
void data_process()
{

	MSG_DOWN snd_data;
	TX_PACKET packet_data;
	int i;

	while(1)
	{	
		sleep(1);
		printf("IsFifoEmpty(&down_buf)=%d\n",IsFifoEmpty(&down_buf));
		if(IsFifoEmpty(&down_buf) == false)
		{

			PopBuffer(&down_buf, (char *)&snd_data, sizeof(MSG_DOWN));
			printf("snd_data.dsdrh.freq=%f\n",snd_data.dsdrh.freq);
			printf("snd_data.dsdrh.rfch=%d\n",snd_data.dsdrh.rfch);

			parse_pkt_msg(snd_data, &packet_data);
			//printf("9\n");
			send_sx1278(&packet_data);
           // PushBuffer(&send_buf,(char *)&packet_data,sizeof(TX_PACKET));
#if 1
          	printf("\n+++++++++++++++++++++++++++++++++++++\n");
            printf("TX freq :  %d   \n", packet_data.freq);
            printf("TX sf   :  %d   \n", packet_data.sf);
            printf("TX cal  :  %d   \n", packet_data.cal);
            printf("TX len  :  %d   \n", packet_data.len);
            printf("TX data[%d]:", packet_data.len-9);
			for(i=0; i<packet_data.len-9; i++)
				printf("0x%02x,",packet_data.data[i]);
			printf("\n+++++++++++++++++++++++++++++++++++++\n\n");

           	
#endif	      
        }
    }
 
	    close(gpio_fd);
}

void send_msg()
{
    TX_PACKET packet_data;
    while(1)
    {
       // if(IsFifoEmpty(&send_buf) == false)
        {
           // PopBuffer(&send_buf, (char *)&packet_data, sizeof(TX_PACKET));
           // send_sx1278(&packet_data);
        }
    }
}

void parse_init()
{
    fifo_init();
    msg_quene_init();
    
    gpio_fd = open_gpio();
	spi_init(gpio_fd);
    printf("spi init success!\n");
	sx1278_init();
    printf("sx1278 init success!\n");
}

