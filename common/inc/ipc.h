/*************************************************************************
	> File Name: ipc.h
	> Author: sachin 
	> Mail: 273672427@qq.com
	> Created Time: 
 ************************************************************************/

#ifndef _IPC_H
#define _IPC_H
#include "data_packet.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define UP_KEY   ftok("/usr/work", 'a')
#define DOWN_KEY ftok("/usr/work", 'c')

typedef  struct phy_payload {
	MHDR mhdrs;
	uint8_t mac_payload[0];
}PHY_PAYLOAD;

typedef struct msg_up {
	USDRH usdrh;
	uint8_t mac_data[512];
	
}MSG_UP;

typedef struct msg_down {
	DSDRH dsdrh;
	uint8_t mac_data[512];
	bool ncrc;
}MSG_DOWN;

typedef struct {
	uint8_t data_down[2408];
}DATA_DOWN;

int msg_creat(key_t key);
int msg_get(key_t key);
void msg_del(int msgid);
int msg_up_send(int msgid, char *mseg, int len,int types);
int msg_down_recv(int msgid, char *mseg, int len, int types);
int msg_up_recv(int msgid, char *mseg, int len, int types);
int msg_down_send(int msgid, char *mseg, int len, int types);

#endif

