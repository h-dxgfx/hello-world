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

static int sock_fd, s;

int caninit()
{
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_filter rfilter[1];
  

	sock_fd = socket(AF_CAN, SOCK_RAW, CAN_RAW);  
	if (sock_fd < 0)
	{
		printf("socket 1 create error!");
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
		printf("bind 1 error!\n");
		return -1;
	}

	//setsockopt(sock_fd, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);   //only send do not receive
	rfilter[0].can_id = 0x123;
	rfilter[0].can_mask = CAN_SFF_MASK;
	//设置过滤规则
	setsockopt(sock_fd, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));
	
} 

int main()
{
	int send, got, ret, max_test_count = 5;
	struct can_frame rx_frame, frame[5] = {{0}};

	frame[0].can_id = 0x123;
	frame[0].can_dlc = 8;

	frame[0].data[0] = (unsigned char)0x50;
	frame[0].data[1] = (unsigned char)0x1FF;
	frame[0].data[2] = (unsigned char)500;
	frame[0].data[3] = (unsigned char)4;
	frame[0].data[4] = (unsigned char)5;
	frame[0].data[5] = (unsigned char)6;
	frame[0].data[6] = (unsigned char)7;
	frame[0].data[7] = (unsigned char)0xaa;

    int i = 0;

    if (0 == fork()) {
		while (1){
            caninit();
			printf("this is father process send message\n");
			printf("fast xmit message %dth send\n", i);
			i++;
			send = write(sock_fd, &frame, sizeof(struct can_frame) ); 
			if(send < 0) {
				printf("send error!\n");
			}
			if (i >= 100)
			{
				break;
			}
            close(sock_fd);
			usleep(500000); 
		}
    } else {
        while (1){
			printf("do receive!\n");
            caninit();
            got = read(sock_fd, &rx_frame, sizeof(rx_frame));
			printf("got= %d", got);
            if (got > 0) {
				printf("receive success!\n");
                printf("rx_frame: id=%ld len=%d  \n", rx_frame.can_id, rx_frame.can_dlc);
                for (int i = 0; i < sizeof(rx_frame); i++){
                    printf("rx_frame data[%d]=%x\n", i, rx_frame.data[i]);
                }
            } else {
                printf("-> got nothing\n");
            }
            close(sock_fd);
			usleep(100000);
	    } 
    }

    close(sock_fd);
    return 0;
}
