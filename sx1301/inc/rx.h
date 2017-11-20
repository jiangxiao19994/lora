
#ifndef _RX_H
#define _RX_H

#define RX_GROUP_NUM 12
#define RX_START_FREQ 470300000
#define RX_CHANNEL_NUM  8
#define RX_STEUP  (RX_CHANNEL_NUM*200000)

#define RX_GROP0  0
#define RX_GROP1  1
#define RX_GROP2  2
#define RX_GROP3  3
#define RX_GROP4  4
#define RX_GROP5  5
#define RX_GROP6  6
#define RX_GROP7  7
#define RX_GROP8  8
#define RX_GROP9  9
#define RX_GROP10  10
#define RX_GROP11  11





#define RX_GROP0_FREQ  (RX_START_FREQ)
#define RX_GROP1_FREQ  (RX_GROP0_FREQ+(1*RX_STEUP))
#define RX_GROP2_FREQ  (RX_GROP0_FREQ+(2*RX_STEUP))
#define RX_GROP3_FREQ  (RX_GROP0_FREQ+(3*RX_STEUP))
#define RX_GROP4_FREQ  (RX_GROP0_FREQ+(4*RX_STEUP))
#define RX_GROP5_FREQ  (RX_GROP0_FREQ+(5*RX_STEUP))
#define RX_GROP6_FREQ  (RX_GROP0_FREQ+(6*RX_STEUP))
#define RX_GROP7_FREQ  (RX_GROP0_FREQ+(7*RX_STEUP))
#define RX_GROP8_FREQ  (RX_GROP0_FREQ+(8*RX_STEUP))
#define RX_GROP9_FREQ  (RX_GROP0_FREQ+(9*RX_STEUP))
#define RX_GROP10_FREQ  (RX_GROP0_FREQ+(10*RX_STEUP))
#define RX_GROP11_FREQ  (RX_GROP0_FREQ+(11*RX_STEUP))


#define RX_470_LOW_FREQ 400000000
#define RX_470_HIGH_FREQ 510000000

int rec_loop();
void lgw_up_packet();
void up_send();
void rx_init();
#endif

