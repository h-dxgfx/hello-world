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
/* 	printf("ifr_name:%s \n",ifr.ifr_name);
	printf("can_ifindex:%d \n",ifr.ifr_ifindex); */

	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if(bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0 )
	{
		printf("transelect bind error!");
	}

    return sock_fd;
} 

int transelect()
{
    printf("run transelect start!\n");
	printf("transelect function father pid: %d, my pid: %d \n", getppid(), getpid());
	int ret, got, sock_fd, count = 0;
	struct can_frame rx_frame, frame[5] = {{0}};
    fd_set rfds;		/* file descriptors for select() */
    fd_set wfds;		/* file descriptors for select() */
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

    while(1) {
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_SET(sock_fd, &rfds);		/* watch on fd for CAN */
        FD_SET(sock_fd, &wfds);		/* watch on fd for CAN */
        FD_SET(0, &rfds);		/* watch on fd for stdin */


	    got = select(FD_SETSIZE, &rfds, &wfds, NULL, NULL);
        if (got > 0 ) {

            if( FD_ISSET(sock_fd, &wfds) ) {
                got = write(sock_fd, &frame, sizeof(struct can_frame));
                if (got > 0){
                    printf("send success got = %d\n", got);
                }
                usleep(100000);
                count++;
                if (count >= 1)
                {
                    printf("transelect finish\n");
                    break;
                } 
            } 
            if( FD_ISSET(sock_fd, &rfds) ) {
                got = read(sock_fd, &rx_frame, sizeof(rx_frame));
                if(got > 0)
                {
                    printf("ID=0x%X DLC=%d data[0]=0x%X\n", rx_frame.can_id,
                    rx_frame.can_dlc, rx_frame.data[0]);
                    for (int i = 0; i < rx_frame.can_dlc; i++){
                        printf("data[%d]= %x\n", i, rx_frame.data[i]);
                    }
                }
            }
            if( FD_ISSET(0, &rfds) ) {
            int c;
            (void)read(0 , &c, 1);
            printf(" key = %x\n", c);
            } 
        }    
        usleep(100000);
    }
    close(sock_fd);
    printf("run transelect successful end!\n");
    return 0;
}
