#ifndef SX1278API_H
#define SX1278API_H

#define SPI_IOC_MAGIC			'k'
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
/* Read / Write of SPI mode (SPI_MODE_0..SPI_MODE_3) */
#define SPI_IOC_RD_MODE			_IOR(SPI_IOC_MAGIC, 1, __u8)
#define SPI_IOC_WR_MODE			_IOW(SPI_IOC_MAGIC, 1, __u8)

#define SPI_IOC_READ			_IOR(SPI_IOC_MAGIC, 5, __u8)
#define SPI_IOC_WRITE		_IOW(SPI_IOC_MAGIC, 5, __u8)

/* Read / Write SPI bit justification */
#define SPI_IOC_RD_LSB_FIRST		_IOR(SPI_IOC_MAGIC, 2, __u8)
#define SPI_IOC_WR_LSB_FIRST		_IOW(SPI_IOC_MAGIC, 2, __u8)

/* Read / Write SPI device word length (1..N) */
#define SPI_IOC_RD_BITS_PER_WORD	_IOR(SPI_IOC_MAGIC, 3, __u8)
#define SPI_IOC_WR_BITS_PER_WORD	_IOW(SPI_IOC_MAGIC, 3, __u8)


/* Read / Write SPI device default max speed hz */
#define SPI_IOC_RD_MAX_SPEED_HZ		_IOR(SPI_IOC_MAGIC, 4, __u32)
#define SPI_IOC_WR_MAX_SPEED_HZ		_IOW(SPI_IOC_MAGIC, 4, __u32)

#define SPI_MODE_MASK		(SPI_CPHA | SPI_CPOL | SPI_CS_HIGH \
				| SPI_LSB_FIRST | SPI_3WIRE | SPI_LOOP \
				| SPI_NO_CS | SPI_READY)
				
						
struct sx1278_data {
	unsigned char buf[256];
	unsigned char addr;
	unsigned char count;
	
};

void spi_init(int fd);
void spi_close(void);
void SX1276WriteBuffer(uint8_t addr, uint8_t *buffer, uint8_t size);
void SX1276ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size );
void SX1276Write( uint8_t addr, uint8_t data );
uint8_t SX1276Read( uint8_t addr );

void select_cs(int group, int num);
#endif
