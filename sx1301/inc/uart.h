#ifndef _UART_H
#define _UART_H

void set_speed(int fd, int speed);
int set_Parity(int fd,int databits,int stopbits,int parity);
#endif