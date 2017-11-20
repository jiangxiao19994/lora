#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include "gpio.h"
#include "sx1276api.h"

#define SPI_SPEED       10000000
#define SPI_DEV_PATH    "/dev/sx1278"

int sx1278_fd;
int cs_fd;

int sx1278_group = 0;
int sx1278_num = 23;
void spi_close(void)
{
	close(sx1278_fd);
}

void spi_init(int fd)
{
	
    int a=0, b=0;
    int i;
	
	cs_fd = fd;
	if(cs_fd < 0)
	{
		printf("open sx1278 failed!\n");
	}
	
	sx1278_fd = open(SPI_DEV_PATH, O_RDWR);
    if (sx1278_fd < 0) {
        printf("open failed!\n");
    }
    else
    {
        printf("sx1278 open success!\n");
    }
    /* setting SPI mode to 'mode 0' */
    i = SPI_MODE_0;
    a = ioctl(sx1278_fd, SPI_IOC_WR_MODE, &i);
    b = ioctl(sx1278_fd, SPI_IOC_RD_MODE, &i);
    if ((a < 0) || (b < 0)) {  
        close(sx1278_fd);
		printf("set MODE FAILED!\n");
    }

    /* setting SPI max clk (in Hz) */
    i = SPI_SPEED;
    a = ioctl(sx1278_fd, SPI_IOC_WR_MAX_SPEED_HZ, &i);
    b = ioctl(sx1278_fd, SPI_IOC_RD_MAX_SPEED_HZ, &i);
    if ((a < 0) || (b < 0)) {
        printf("set clk FAILED!\n");
        close(sx1278_fd);
      
    }

    /* setting SPI to MSB first */
    i = 0;
    a = ioctl(sx1278_fd, SPI_IOC_WR_LSB_FIRST, &i);
    b = ioctl(sx1278_fd, SPI_IOC_RD_LSB_FIRST, &i);
    if ((a < 0) || (b < 0)) {
        printf("set MSB FAILED!\n");
        close(sx1278_fd);
       
    }

    /* setting SPI to 8 bits per word */
    i = 0;
    a = ioctl(sx1278_fd, SPI_IOC_WR_BITS_PER_WORD, &i);
    b = ioctl(sx1278_fd, SPI_IOC_RD_BITS_PER_WORD, &i);
    if ((a < 0) || (b < 0)) {
        printf("set bits FAILED!\n");
        close(sx1278_fd);
        
    }
    set_gpio_ouput(cs_fd, 2 , 1);
	set_gpio_value(cs_fd, 2, 1, 1);
	set_gpio_ouput(cs_fd, 0 , 23);
	set_gpio_value(cs_fd, 0, 23, 1);
	set_gpio_ouput(cs_fd, 1 , 12);
	set_gpio_value(cs_fd, 1, 12, 1);
	set_gpio_ouput(cs_fd, 1 , 17);
	set_gpio_value(cs_fd, 1, 17, 1);
	set_gpio_ouput(cs_fd, 1 , 16);
	set_gpio_value(cs_fd, 1, 16, 1);
	set_gpio_ouput(cs_fd, 1 , 23);
	set_gpio_value(cs_fd, 1, 23, 1);
	set_gpio_ouput(cs_fd, 1 , 20);
	set_gpio_value(cs_fd, 1, 20, 1);
	set_gpio_ouput(cs_fd, 0 , 6);
	set_gpio_value(cs_fd, 0, 6, 1);
}

void select_cs(int group, int num)
{
    sx1278_group = group;
    sx1278_num = num;
}



void SX1276WriteBuffer(uint8_t addr, uint8_t *buffer, uint8_t size)
{
	struct sx1278_data tmp;
	int i;
	
	tmp.addr = addr;
	tmp.count = size;
	
	for(i=0; i<size; i++)
	{
		tmp.buf[i] = *(buffer+i);
	}
	
	set_gpio_value(cs_fd, sx1278_group, sx1278_num, 0);
	ioctl(sx1278_fd, SPI_IOC_WRITE, &tmp);
	set_gpio_value(cs_fd, sx1278_group, sx1278_num, 1);
}

void SX1276ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
	int i;
	struct sx1278_data tmp;
	
	tmp.addr = addr;
	tmp.count = size;
	for(i=0; i<size; i++)
	{
		tmp.buf[i] = '\0';
	}
	set_gpio_value(cs_fd, sx1278_group,sx1278_num, 0);
	ioctl(sx1278_fd, SPI_IOC_READ, &tmp);

	for(i=0; i<size; i++)
	{
		*(buffer+i) = tmp.buf[i];
	}

	set_gpio_value(cs_fd, sx1278_group, sx1278_num, 1);
}
