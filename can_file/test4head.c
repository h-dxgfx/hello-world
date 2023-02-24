#include "transmit.h"
#include "transelect.h"
#include "transmit2_one_fd.h"
#include "transmit2_two_fd.h"

int main()
{   
    printf("main function : father pid: %d, my pid: %d \n", getppid(), getpid());
    transmit();
    transelect();
    transmit2_one_fd();
    sleep(1);
    transmit2_two_fd();

    return 0;
}
