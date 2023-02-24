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
#include <asm/ptrace.h>

static int sock_fd;

int caninit()
{
	struct sockaddr_can addr;
	struct ifreq ifr;
  

	sock_fd = socket(AF_CAN, SOCK_RAW, CAN_RAW);  
	//dump_stack();
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

	return sock_fd;
} 

int main()
{
	int ret;
	struct can_frame frame[5] = {{0}};
	sock_fd = caninit();

  

	int max_test_count = 5;

  /* frame[0].can_id = 0x123;
  frame[0].can_dlc = 8; */

	frame[0].data[0] = (unsigned char)0x50;
	frame[0].data[1] = (unsigned char)2;
	frame[0].data[2] = (unsigned char)3;
	frame[0].data[3] = (unsigned char)4;
	frame[0].data[4] = (unsigned char)5;
	frame[0].data[5] = (unsigned char)6;
	frame[0].data[6] = (unsigned char)7;
	frame[0].data[7] = (unsigned char)0xaa;

	frame[1].data[0] = (unsigned char)0x51;
	frame[1].data[1] = (unsigned char)7;
	frame[1].data[2] = (unsigned char)6;
	frame[1].data[3] = (unsigned char)5;
	frame[1].data[4] = (unsigned char)4;
	frame[1].data[5] = (unsigned char)3;
	frame[1].data[6] = (unsigned char)2;
	frame[1].data[7] = (unsigned char)0x55;

	frame[2].data[0] = (unsigned char)0x52;
	frame[2].data[1] = (unsigned char)7;
	frame[2].data[2] = (unsigned char)6;
	frame[2].data[3] = (unsigned char)5;
	frame[2].data[4] = (unsigned char)4;
	frame[2].data[5] = (unsigned char)3;
	frame[2].data[6] = (unsigned char)2;
	frame[2].data[7] = (unsigned char)0x55;

	frame[3].data[0] = (unsigned char)0x53;
	frame[3].data[1] = (unsigned char)7;
	frame[3].data[2] = (unsigned char)6;
	frame[3].data[3] = (unsigned char)5;
	frame[3].data[4] = (unsigned char)4;
	frame[3].data[5] = (unsigned char)3;
	frame[3].data[6] = (unsigned char)2;
	frame[3].data[7] = (unsigned char)0x55;

	frame[4].data[0] = (unsigned char)0x54;
	frame[4].data[1] = (unsigned char)7;
	frame[4].data[2] = (unsigned char)6;
	frame[4].data[3] = (unsigned char)5;
	frame[4].data[4] = (unsigned char)4;
	frame[4].data[5] = (unsigned char)3;
	frame[4].data[6] = (unsigned char)2;
	frame[4].data[7] = (unsigned char)0x55;

	for (int i = 0; i < max_test_count; i++)
	{	
		frame[i].can_id = 0x123 + i;
  		frame[i].can_dlc = 8;
		ret = (int)write(sock_fd, &frame[i], sizeof(frame[i]));
		if (ret < 0)
		{
			printf("send frame %d error!", i);
			return -1;
		}	
	}
	close(sock_fd);
	return 0;
}
