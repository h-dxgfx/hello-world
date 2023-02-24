#include "transelect.h"


int main()
{   
    printf("main function : father pid: %d, my pid: %d \n", getppid(), getpid());
    transelect();
    sleep(1);

    return 0;
}
