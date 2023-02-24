#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <string.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/select.h>
#include <sys/time.h>


static int caninit()
{   int sock_fd;
	struct sockaddr_can addr;
	struct ifreq ifr;
  
	sock_fd = socket(AF_CAN, SOCK_RAW, CAN_RAW);  
	if (sock_fd < 0)
	{
		printf("transelect socket create error!");
	}

	strcpy(ifr.ifr_name, "can0");
	ioctl(sock_fd, SIOCGIFINDEX, &ifr);

	ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);

	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if(bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0 )
	{
		printf("transelect bind error!");
	}
    return sock_fd;
} 

int can_send(int fd, int len,char *message) {
  char *token;
  struct can_frame tx;
  int j, sent;
 
  if(( token = strtok(message,":") ) != NULL ) {
    tx.can_id=0;
    if( token[0] == 'r' || token[0] == 'R' ) {
    tx.can_id = CAN_RTR_FLAG;
    tx.can_id = strtol(&token[1],NULL,0);
    tx.can_dlc=len;
    } else {
    tx.can_id = strtol(token,NULL,0);
    j=0;
    while( (token = strtok(NULL,",")) != NULL ) {
        tx.data[j++] = strtol(token,NULL,0);
      }
      tx.can_dlc = (len>0 ? len : j );
    }
    sent = write(fd, &tx, sizeof(tx));
    if (sent > 0){
      printf("send success sent = %d\n", sent);
      for (int i = 0; i < tx.can_dlc; i++)
      {
        printf("tx.data[%d] = %x", i, tx.data[i]);
      }
    }else {
      printf("send error sent = %d \n", sent);
    }
    return 1;
  } else {
   return -1;
  }
}


void can_read(int fd, int timeout) {
  printf("do receive!\n");
  fd_set   rfds;
  struct   timeval tv;
  int      got,i,j;
  struct can_frame rx[80] = {{0}};
  static char databuf[4096];
  char     type;

  FD_ZERO(&rfds);
  FD_SET(fd,&rfds);

  tv.tv_sec  = 0; /* wait 5 seconds before process terminates */
  tv.tv_usec = timeout;
  /* s += sprintf(s,"",fd); */
  got=0;
  got = select(FD_SETSIZE, &rfds, NULL, NULL, NULL);
  if (got > 0 ) {
    if( FD_ISSET(fd, &rfds) ) {
      got = write(fd, &rx, sizeof(struct can_frame));
      if (got > 0){
          printf("send success got = %d\n", got);
          for (i = 0; i < rx[0].can_dlc; i++)
          {
            printf("rx[0].data[%d] = %x \n", i, rx[0].data[i]);
          }
          if(rx[i].can_id & CAN_ERR_FLAG) {
            type = 'O';
          } else if(rx[i].can_id & CAN_EFF_FLAG) {
            type = 'e';
          } else {
            type = '.';
          }
          if( rx[i].can_id & CAN_RTR_FLAG ) {
            printf("remote frame!\n");
          }
      } else {
        printf("send error! got = %d\n", got);
      }
      usleep(100000);
    }
  }else {
    printf("socket busy!\n");
  }    
}


int main(){
  int fd1, fd2;
  fd1 = caninit();
  fd2 = caninit();
  char m[] = "100/0x064 : sD : 085 002 003 004 005 006 007 170";
  can_send(fd1, 8, m);
  can_read(fd2,10);
  return 0;
}
