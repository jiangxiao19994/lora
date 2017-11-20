/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Implements a FIFO buffer

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#ifndef __FIFO_H__
#define __FIFO_H__

#include <stdbool.h>
#include <stdint.h>
#include "ipc.h"
/*!
 * FIFO structure
 */
typedef struct Fifo_s
{
    uint16_t   Begin;
    uint16_t   End;
    char       *Data;
    uint16_t   Size;
    uint16_t   len;
}Fifo_t;
uint16_t FifoNext( Fifo_t *fifo, uint16_t index );
/*!
 * Initializes the FIFO structure
 *
 * \param [IN] fifo   Pointer to the FIFO object
 * \param [IN] buffer Buffer to be used as FIFO
 * \param [IN] size   Size of the buffer
 */
void FifoInit( Fifo_t *fifo, char *buffer, uint16_t size );

/*!
 * Pushes data to the FIFO
 *
 * \param [IN] fifo Pointer to the FIFO object
 * \param [IN] data Data to be pushed into the FIFO
 */
void FifoPush( Fifo_t *fifo, char data );

/*!
 * Pops data from the FIFO
 *
 * \param [IN] fifo Pointer to the FIFO object
 * \retval data     Data popped from the FIFO
 */
char FifoPop( Fifo_t *fifo );

/*!
 * Flushes the FIFO
 *
 * \param [IN] fifo   Pointer to the FIFO object
 */
void FifoFlush( Fifo_t *fifo );

/*!
 * Checks if the FIFO is empty
 *
 * \param [IN] fifo   Pointer to the FIFO object
 * \retval isEmpty    true: FIFO is empty, false FIFO is not empty
 */
bool IsFifoEmpty( Fifo_t *fifo );

/*!
 * Checks if the FIFO is full
 *
 * \param [IN] fifo   Pointer to the FIFO object
 * \retval isFull     true: FIFO is full, false FIFO is not full
 */
bool IsFifoFull( Fifo_t *fifo );

void PushBuffer(Fifo_t *fifo, char *buffer, int len);
void PopBuffer(Fifo_t *fifo, char *buffer, int len);
#endif // __FIFO_H__
