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
	struct sockaddr_can addr;
	struct ifreq ifr;
  
	sock_fd = socket(AF_CAN, SOCK_RAW, CAN_RAW);  
	if (sock_fd < 0)
	{
		printf("socket create error!");
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
		printf("bind error!");
		return -1;
	}

	setsockopt(sock_fd, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);   //only send do not receive

	return sock_fd;
} 

int transmit()
{
	int ret, sock_fd, max_test_count = 5;
	struct can_frame frame[5] = {{0}};
	sock_fd = caninit();
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

    printf("run transmit start!\n");
	printf("transmit function father pid: %d, my pid: %d \n", getppid(), getpid());

    if (0 == fork()) {
	    printf("this is son process check can0\n");
		printf("transmit son process, father pid: %d, my pid: %d \n", getppid(), getpid());
	    system("ip -d -s link show can0");
    } else {
        while (1) {
			printf("transmit father process father pid: %d, my pid: %d \n", getppid(), getpid());
            ret = write(sock_fd, &frame[0], sizeof(frame[0]));
            if (ret <= 0) {
                printf("write error ret= %d\n", ret);
                close(sock_fd);
                exit(1);
            }
			i++;
			if (i >= 1)
			{
                close(sock_fd);
                printf("run transmit successful end!\n");
                return 0;
			}
            usleep(100000);
        }
    }
    close(sock_fd);
    printf("run transmit successful end!\n");
	return 0;
}