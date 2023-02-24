
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <net/if.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <errno.h>
#include <asm-generic/int-ll64.h>



#define SEQN	9 /* number of messages for one sequence, one write call */
#define CAN_MSG_LENGTH 8
#define extd 0




static int sock_fd;
struct ifreq ifr;
struct sockaddr_can addr;


int main(int argc, char **argv)
{
	struct can_frame frame;
	int ret;
	int c;
	char *pname;
	extern char *optarg;
	extern int optind;

    pname = *argv;

	/*******************创建套接字***********************/
	sock_fd = socket(AF_CAN, SOCK_RAW, CAN_RAW);
	if (sock_fd < 0) {
        perror("socket create error ! \n");
        return -1;
    }
	//将套接字与can0绑定
	strcpy(ifr.ifr_name,"can0");
	ioctl(sock_fd, SIOCGIFINDEX, &ifr);  //设置设备为can0
	ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0 )
    {
        perror("bind error!\n");
        return -1;
    }

	struct can_filter rfilter[1];  // 定义过滤规则数组，结构体 can_filter 是库函数定义好的
    rfilter[0].can_id = 0x123;
    rfilter[0].can_mask = CAN_SFF_MASK; // 标准帧 (SFF: standard frame format)

    setsockopt(sock_fd, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

	ret = read(sock_fd, &frame, sizeof(frame));

	int i;
	static int iseq = 8;
	static __u64 seq;
	//seq = &(frame.data[0]);		
	unsigned int id = 0x123;	//10：291	
	
    if( id != frame.can_id) {
		printf("wrong message Id received: %0x, expected %0x, in seq %lu\n",
		frame.can_id, id, (unsigned long)seq);
        id = frame.can_id; //try to synchronize 同步 
    }
    if( iseq != frame.can_dlc ) {
		printf("wrong message length: %d, expected %d, in seq %lu\n", frame.can_dlc, iseq, (unsigned long)seq);
    } else {
		__u8 *address;
		i = 1;
		//  check also data content, must be sequence number 
		//    data in the frame is little endian
		//  
		switch(frame.can_dlc) {
		case 0:
			break;
		case 1:
			//i = ((seq & 0xff) == &(frame.data[0]));
			break;
		case 2:
			//address = &(frame.data[0]) + 1;
			//i = ((seq & 0xffff) == address);
			break;
		case 3:
			break;
		case 4:
			//address = &(frame.data[0])+3;
			//i = ((seq & 0xffffffff) == address);
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			//  data = *(unsigned long long *)&frame.data[0]; 
			//  is evaluating 5 bytes enough? 
			// address = frame.data[0] + (frame.data[1] * 256)
			// 	+ (frame.data[2] * 65536) + (frame.data[3] * 16777216L)
			// 	+ (frame.data[4] * 4294967296L);

			//printf(" address %lu\n", (unsigned long)address);
			address = &(frame.data[0]) + 4;

			//获取到传输数据的第五位数据的地址
			//根据地址获取当前地址的值，判断这个值和传入的值是否相等来判断数据是否断了
			//int datacurr = *(address)
			seq = *address;
			i = ((seq) == frame.data[4]);
			break;
		}
		if(!i) {
			printf("----wrong data %ld, %d exp: %ld\n",
				(unsigned long)address, frame.can_dlc, (unsigned long)seq);
		}
		if( ++iseq == SEQN) {
			printf("输入数据正确\n");
			// iseq = 0;
			// seq++;
			// printf(" seq %lu\n", (unsigned long)seq);
		}
    }
    // if (( extd && (id == (536870906  ))) 
	//  || (!extd && (id == (     2042  )))  ) {
	// // reset message id to 0 
	// id = 0;
    // } else {
	// 	id++;
    // }

	printf("sock_fd= %d\n", sock_fd);
    return 0;
}