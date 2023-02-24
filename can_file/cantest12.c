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
//#include <fstream>

#define ip_can_up "ip link set can0 up"
#define ip_can_down "ip link set can0 down"
#define ip_can_bitrate "ip link set can0 type can bitrate 250000 loopback on"




int main()
{
  int ret,sock_fd;

  struct sockaddr_can addr;
  struct ifreq ifr;
  struct can_frame frame;

/*   system(ip_can_down);
  system(ip_can_bitrate);
  system(ip_can_up); */

  sock_fd = socket(AF_CAN, SOCK_RAW, CAN_RAW);  
  if (sock_fd < 0)
  {
    printf("socket create error!");
    return -1;
  }

  strcpy(ifr.ifr_name, "can0");
  ioctl(sock_fd, SIOCGIFINDEX, &ifr);

  ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);
	printf("ifr_name:%s \n",ifr.ifr_name);
	printf("can_ifindex:%d \n",ifr.ifr_ifindex);

  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;

  if(bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0 )
  {
    printf("bind error!");
    return -1;
  }

  setsockopt(sock_fd, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);   //only send do not receive

  unsigned long int test_count = 0;

  frame.can_id = 0x100;
  frame.can_dlc = 8;

  frame.data[0] = (unsigned char)0;
  frame.data[1] = (unsigned char)0;
  frame.data[2] = (unsigned char)0;
  frame.data[3] = (unsigned char)0;
  frame.data[4] = (unsigned char)0;
  frame.data[5] = (unsigned char)0;
  frame.data[6] = (unsigned char)0;
  frame.data[7] = (unsigned char)0;
  do {
    frame.data[0] = (unsigned char)test_count % 0x100;
    frame.data[1] = (unsigned char)(test_count >> 8) % 0x100;
    frame.data[2] = (unsigned char)(test_count >> 16) % 0x100;
    frame.data[3] = (unsigned char)(test_count >> 24) % 0x100;
    
	  //ret = (int)write(sock_fd, &frame, 1);
    ret = (int)write(sock_fd, &frame, sizeof(struct can_frame));
    printf("ret = %d", ret);

    if (ret != sizeof(struct can_frame))
    {
      printf("Send Error!");
      break;
    }	
	  test_count++;
  }
  while(1);
  usleep(1000000);


  return 0;
}
