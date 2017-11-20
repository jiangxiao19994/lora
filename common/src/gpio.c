#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>
#include  "gpio.h"

#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))

#define GPIO_IOC_MAGIC					'G'
#define IOCTL_GPIO_SETOUTPUT			_IOW(GPIO_IOC_MAGIC, 0, int)                   
#define IOCTL_GPIO_SETINPUT				_IOW(GPIO_IOC_MAGIC, 1, int)
#define IOCTL_GPIO_SETVALUE				_IOW(GPIO_IOC_MAGIC, 2, int) 
#define IOCTL_GPIO_GETVALUE				_IOR(GPIO_IOC_MAGIC, 3, int)
#define GPIO_IOC_MAXNR					3

typedef struct {        
    int pin;        
    int data;
}am335x_gpio_arg;
int open_gpio(void)
{
    
    int fd;
    fd = open("/dev/am335x_gpio",O_RDWR);
    if(fd < 0)
    {
        printf("open failed!\r\n");
        return -1;
    }    else    {        printf("gpio open success!\r\n");    }
    return fd;
}

int set_gpio_ouput(int fd, int group, int num)
{
    am335x_gpio_arg arg;
    
    arg.pin = GPIO_TO_PIN(group, num);
    arg.data = 0;
    ioctl(fd,IOCTL_GPIO_SETOUTPUT,&arg);
    
    return 0;
}

int set_gpio_input(int fd, int group, int num)
{
    am335x_gpio_arg arg;
    
    arg.pin = GPIO_TO_PIN(group, num);
    arg.data = 1;
    ioctl(fd,IOCTL_GPIO_SETINPUT,&arg);
    return 0;
}

int set_gpio_value(int fd, int group, int num, int value)
{
    am335x_gpio_arg arg;
    
    arg.pin = GPIO_TO_PIN(group, num);
    arg.data = value;
    ioctl(fd,IOCTL_GPIO_SETOUTPUT,&arg);
    return 0;
}

int get_gpio_value(int fd, int group, int num)
{
    am335x_gpio_arg arg;
    
    arg.pin = GPIO_TO_PIN(group, num);
    ioctl(fd,IOCTL_GPIO_SETOUTPUT,&arg);
    return arg.data;
}

int close_gpio(int fd)
{
    close(fd);
    return 0;
}


