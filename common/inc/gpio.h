#ifndef _LORA_GPIO_H
#define _LORA_GPIO_H

int open_gpio(void);
int set_gpio_ouput(int fd,int group, int num);
int set_gpio_input(int fd,int group, int num);
int set_gpio_value(int fd,int group, int num, int value);
int get_gpio_value(int fd,int group, int num);
int close_gpio(int fd);

#endif
