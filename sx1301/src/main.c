#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>#include <signal.h>
#include "rx.h"
#include "parse_data.h"#include "sx1276api.h"
extern int gpio_fd;pthread_t thread[6]; 
 
void *sx1301_recive() //线程函数
{
  printf ("sx1301_recive thread start\n");
   
  rec_loop();
  pthread_exit(NULL);
}

void *up_packet()
{
	printf ("up_packet thread start\n");
	lgw_up_packet();
	pthread_exit(NULL);
}

void *up_send_msg()
{
	printf ("up_send_msg thread start\n");
	up_send();
	pthread_exit(NULL);
}
void *down_msg_receive()
{
  printf ("down_msg_receive thread start\n");
  msg_rec();  
  pthread_exit(NULL);
}

void *down_parse_data()
{
  printf ("down_parse_data thread start\n");
  
  data_process();
  pthread_exit(NULL);
}
 void *down_send()     {        printf ("down_send thread start\n");         send_msg();        pthread_exit(NULL);     }
void thread_create(void)
{
  int error;
  /*创建线程*/
  error = pthread_create(&thread[0], NULL,sx1301_recive, NULL);
  if(error != 0)
  {
	  printf("create up_packet failed!\n");
	  //return -1;
  }
  usleep(500*1000);
  error = pthread_create(&thread[1], NULL,up_packet, NULL);
  if(error != 0)
  {
	  printf("create sx1301_recive failed!\n");
	  //return -1;
  }
  usleep(500*1000);
  error = pthread_create(&thread[2], NULL,up_send_msg, NULL);
  if(error != 0)
  {
	  printf("create sx1301_recive failed!\n");
	  //return -1;
  }
  usleep(500*1000);
  error = pthread_create(&thread[3], NULL,down_msg_receive, NULL);
  if(error != 0)
  {
	  printf("create down_msg_receive failed!\n");
	  //return -1;
  }
  usleep(500*1000);
  error = pthread_create(&thread[4], NULL,down_parse_data, NULL);
  if(error != 0)
  {
	  printf("create down_parse_data failed!\n");
	  //return -1;
  }   usleep(500*1000);
  error = pthread_create(&thread[5], NULL,down_send, NULL);
  if(error != 0)
  {
	  printf("create down_send failed!\n");
	  //return -1;
  }
}void Stop(int signo)   {      printf("oops! stop!!!\n");      SX1276OnDio0Irq();	close(gpio_fd);	spi_close();    _exit(0);  }  

void thread_wait(void)
{
  /*等待线程结束*/
  pthread_join(thread[0],NULL);#if 1
  pthread_join(thread[1],NULL);
  pthread_join(thread[2],NULL);
  pthread_join(thread[3],NULL);
  pthread_join(thread[4],NULL);   pthread_join(thread[5],NULL);#endif
}

int main()
{   signal(SIGINT, Stop);   rx_init();   parse_init();    
  thread_create();
  thread_wait();
  return 0;
}