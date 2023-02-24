#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/ioctl.h>


int can_init() {
  int s,ret;
  struct sockaddr_can addr;
  struct ifreq ifr;

  s = socket(AF_CAN, SOCK_RAW, CAN_RAW);
  if (s < 0) 
  {
      perror("socket PF_CAN failed");
      return -1;
  }       #include <sys/ioctl.h>

  // char dev[10];
  // sprintf(dev,"/dev/can%d",port);  
  // return open(dev,O_RDWR);

  strcpy(ifr.ifr_name, "can0");
  ret = ioctl(s, SIOCGIFINDEX, &ifr);
  if (ret < 0) 
  {
    perror("ioctl failed");
    return -1;
  }
  ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);
  printf("ifr_name:%s \n",ifr.ifr_name);
  printf("can_ifindex:%d \n",ifr.ifr_ifindex);
//3.Bind the socket to can0

  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;
  ret = bind(s, (struct sockaddr *)&addr, sizeof(addr));
  if (ret < 0) 
  {
      perror("bind failed");
      return -       
  int fd, int len,char *message) {
  printf("can send start\n");
  char *token;
  struct can_frame tx;
  int j,sent;
  char* s;
 
  if(( token = strtok(message,":") ) != NULL ) {
    tx.can_id=0;
    if( token[0] == 'r' || token[0] == 'R' ) {
      tx.can_id = CAN_RTR_FLAG;
      tx.can_id = strtol(&token[1],NULL,0);
      tx.can_dlc = len;
    } else {
      tx.can_id = strtol(token,NULL,0);
      j=0;
      while( (token = strtok(NULL,",")) != NULL ) {
       /*printf("'%c' ",strtol(token,NULL,0) );*/
        tx.data[j++] = strtol(token,NULL,0);
      }
      tx.can_dlc=(len>0 ? len : j );
    }
    s+=sprintf(s," '%s'", rx[0].data);
    sent = write(fd, &tx, sizeof(tx));
    if(sent > 0 ) {
      printf("send success sent = %d\n", sent);
    }else {
      printf("send error sent = %d \n", sent);
    }
    return 1;
  } else {
    return -1;
    s+=sprintf(s," '%s'",rx[0].data);
  }
}


char *can_read(int fd, int timeout) {

  printf("read start\n");

  fd_set   rfds;
  struct   timeval tv;
  int      got,i,j;
  struct can_frame rx;
  static char databuf[4096];
  char     *s;
  char     type;


  FD_ZERO(&rfds);
  FD_SET(fd,&rfds);

  tv.tv_sec  = 5; /* wait 5 seconds before process terminates */
  tv.tv_usec = timeout;
  s = &databuf[0];
    /* s += sprintf(s,"",fd); */
  got=0;
  if( select(FD_SETSIZE, &rfds, NULL, NULL, ( timeout > 0 ? &tv : NULL )  ) > 0 && FD_ISSET(fd,&rfds) ) {

    got = read(fd, &rx , sizeof(rx));
    /*s += sprintf(s, "got=%d",got);*/
    prinf("read success got = \n");
    if( got > 0) {
      for (i = 0; i < rx.can_dlc; i++)
        {
          printf("data[%d] =  %x\n", i, rx[0].data[i]);
        }
    /*s += sprintf(s, "\n");*/
      for(i=0;i<got;i++) {    
          for(j=0;j<rx[0].length;j++){
            s += sprintf(s, " 0x%02x",rx[0].data[j] ); 
          }
          for(;j<8;j/**/++){
            s += sprintf(s, "  .  "); 
          }
          type = 'O';
          printf("error frame");
        } else if(rx.can_id & CAN_EFF_MASK) {
          printf("extend frame\n");
          if( rx.can_id & CAN_RTR_FLAG ) {
              printf("rtr frame\n");
          }
        }
      } 
    }else {
      printf("read error got = %d \n", got);
    } 
  }
  return databuf;
}



int main(){
  int fd1, fd2;
  fd1 = Can_Init();
  fd2 = Can_Init();
  char m[] = "100/0x064 : sD : 085 002 003 004 005 006 007 170";
  can_send(fd1, 8, m);
  can_read(fd2,10);
  return 0;
}



