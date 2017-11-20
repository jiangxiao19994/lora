#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "ipc.h"



int msg_creat(key_t key)
{	
    int msgid = msgget(key, IPC_CREAT|IPC_EXCL|0666);
    if (msgid == -1) {
       
        return -1;
    } else {
    
    }

    return msgid;
}
int msg_get(key_t key)
{
    int msgid = msgget(key, 0666);
    if (msgid == -1) {
       
        return -1;
    }
    return msgid;
}

void msg_del(int msgid)
{
    msgctl(msgid, IPC_RMID, NULL);
}


int msg_up_send(int msgid, char *mseg, int len, int types)
{
    struct {
        long types;
        char data[1024];
    }mymsg;
    
    mymsg.types = types;
	memcpy(mymsg.data,mseg,len);

    int ret = msgsnd(msgid, &mymsg, len, IPC_NOWAIT);
	if (ret == -1) {
      
        return -1;
    }
    return 0;
}

int msg_up_recv(int msgid, char *mseg, int len, int types)
{
    struct {
        long types;
        char data[1024];
    }mymsg;

    ssize_t ret = msgrcv(msgid, &mymsg, sizeof(mymsg.data), types, IPC_NOWAIT);// IPC_NOWAIT
    if (ret == -1) {
       
        return -1;
    }
	
	memcpy(mseg,mymsg.data,len);

    return ret;
}

int msg_down_recv(int msgid, char *mseg, int len, int types)
{
    struct {
        long types;
        char data[1024];
    }mymsg;

    ssize_t ret = msgrcv(msgid, &mymsg, sizeof(mymsg.data), types, 0); // IPC_NOWAIT
    if (ret == -1) {
        perror("msgrcv:");
        return -1;
    }
   
	memcpy(mseg,mymsg.data,len);
	
    return ret;
}

int msg_down_send(int msgid, char *mseg, int len, int types)
{
    struct {
        long types;
        char data[1024];
    }mymsg;

    mymsg.types = types;
	memcpy(mymsg.data,mseg,len);

    int ret = msgsnd(msgid, &mymsg, len, IPC_NOWAIT);
	if (ret == -1) {
      
        return -1;
    }
	
    return 0;
}

