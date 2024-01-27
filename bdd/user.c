#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <string.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <limits.h>
#include <errno.h>
#include "block_drv_ioctl.h"

sem_t* mySem;
int shmem;

void sendDataToSHM(char* buffer)
{
    sem_wait(mySem);

    printf("Hooked semaphore\n");
    void* addr = mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, shmem, 0);

    memset(addr, 0, 1024);

    memcpy(addr, buffer, strlen(buffer));
    
    sem_post(mySem);

}

int main(int argc, char* argv[])
{
    mknod("/dev/DanishevskiyNS0", S_IFBLK, makedev(252,1));

    int device = open("/dev/DanishevskiyNS0", O_RDWR, 0777);
    if(device < 0)
    {
        printf("ERROR: Error while opening device\n");
    }

    //Initialize Semaphore and Shared memory
    mySem =  sem_open("MySemaphore", O_CREAT, 0777, 1);
    shmem = shm_open("myShm", O_CREAT | O_RDWR, 0777);
    ftruncate(shmem, 1024);

    //Getting App2 process pid using argv
    char *p;
    long convert = 0;
    errno = 0;
    pid_t pid = 0;
    convert = strtol(argv[1], &p, 0);
    if (errno != 0 || *p != '\0' || convert > INT_MAX || convert < INT_MIN)
    {
        printf("ERROR: Invalid process id for user2\n");
    }
    else
    {
        pid = (pid_t)convert;
    }
    printf("second process pid: %d\n",pid);
    
    //Initialize information value for user2
    union sigval val;
    val.sival_int = 1; 

    //IOCTL
    block_drv_ioctl_data data = {0};


    printf("----IOCTL_BLOCK_DRV_SET----\n");
    data.InputData = "Old data for Driver";
    data.InputLength = strlen(data.InputData) + 1;
    ioctl(device, IOCTL_BLOCK_DRV_SET, &data);

    
    printf("----IOCTL_BLOCK_DRV_GET_AND_SET----\n");
    data.OutputData = malloc(1024);
    data.OutputLength = 1024;
    memset(data.OutputData, 0, 1024);
    data.InputData = "New data for Driver";
    data.InputLength = strlen(data.InputData);

    ioctl(device, IOCTL_BLOCK_DRV_GET_AND_SET, &data);
    printf("data from driver: %s\n", data.OutputData);

    sendDataToSHM(data.OutputData);
    printf("sending signal...\n");
    sigqueue(pid, SIGFPE, val);
    
       
    printf("----IOCTL_BLOCK_DRV_GET----\n");
    memset(data.OutputData, 0, data.OutputLength);

    ioctl(device, IOCTL_BLOCK_DRV_GET, &data);
    printf("data from driver: %s\n", data.OutputData);

    sendDataToSHM(data.OutputData);
    printf("sending signal...\n");
    sigqueue(pid, SIGFPE, val);


    printf("----IOCTL_BLOCK_DBG_MESSAGE----\n");
    ioctl(device, IOCTL_BLOCK_DBG_MESSAGE);


    shm_unlink("myShm");
    sem_close(mySem);
    sem_unlink("mySemaphore");

    close(device);

    return 0;
}