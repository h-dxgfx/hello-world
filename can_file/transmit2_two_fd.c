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

static int caninit()
{	
	int sock_fd;
	struct ifreq ifr;
	struct sockaddr_can addr;

	sock_fd = socket(AF_CAN, SOCK_RAW, CAN_RAW);  
	if (sock_fd < 0)
	{
		printf("transmit2_two_fd socket 1 create error!");
		return -1;
	}

	strcpy(ifr.ifr_name, "can0");
	ioctl(sock_fd, SIOCGIFINDEX, &ifr);

	ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);
/* 	printf("ifr_name:%s \n",ifr.ifr_name);
	printf("can_ifindex:%d \n",ifr.ifr_ifindex); */

	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if(bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0 )
	{
		printf("transmit2_two_fd bind 1 error!\n");
		return -1;
	}

	return sock_fd;
}

int transmit2_two_fd()
{
    int sock_fd, s;
	struct can_filter rfilter[1];
    int send, got, ret, max_test_count = 5;
	struct can_frame rx_frame, frame[5] = {{0}};
  
    printf("run transmit2_two_fd start!\n");
	printf("transmit2_two_fd function, father pid: %d, my pid: %d \n", getppid(), getpid());

	s = caninit();
	sock_fd = caninit();

	rfilter[0].can_id = 0x123;
	rfilter[0].can_mask = CAN_SFF_MASK;

	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

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
	int j = 0;

    if (0 == fork()) {
		while (1){
			i++;
			printf("transmit2_two_fd son process, father pid: %d, my pid: %d \n", getppid(), getpid());
			printf("two_fd message %dth send\n", i);
			send = write(sock_fd, &frame, sizeof(struct can_frame) ); 
			if(send < 0) {
				printf("send error!\n");
			}
			if (i >= 3)
			{
                break;
			}
			usleep(100000); 
		}
    } else {
        while (1){
			printf("transmit2_one_fd father process, father pid: %d, my pid: %d \n", getppid(), getpid());
            got = read(s, &rx_frame, sizeof(rx_frame));
			printf("got= %d", got);
			j++;
            if (got > 0) {
				printf("receive success!\n");
                printf("rx_frame: id=%d len=%d  \n", rx_frame.can_id, rx_frame.can_dlc);
                for (int i = 0; i < rx_frame.can_dlc; i++){
                    printf("rx_frame data[%d]=%x\n", i, rx_frame.data[i]);
                }
            } else {
                printf("-> got nothing\n");
            }
			usleep(100000);
            printf("j = %d\n", j);
			if (j >= 3)
			{
                break;
			}
            close(sock_fd);
            return 0;
	    } 
    }
    close(sock_fd);
    printf("run transmit2_two_fd successful end!\n");
    return 0;
}
