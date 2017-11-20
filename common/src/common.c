#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <sys/time.h>

void DelayMs(unsigned int value)
{
    usleep(1000 * value);
}




 

