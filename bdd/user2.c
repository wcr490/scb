#include <semaphore.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

sem_t* mySem;
int shmem;

void readDataFromSHM()
{
    sem_wait(mySem);
    
    printf("Hooked semaphore\n");
    void* addr = mmap(NULL, 1024, PROT_READ, MAP_SHARED, shmem, 0);    
    char buffer[64] = {0};
    memcpy(buffer, addr, 64);

    printf("data from shmem: %s\n", buffer);

    sem_post(mySem);
}

void SigHandler(int signum, siginfo_t* info, void* arg)
{
    printf("Signal recieved\n");
    int param = info->si_value.sival_int;
    if(param)
    {
        readDataFromSHM();
    }    
}

int main()
{
    //Determine sigaction to handel SIGFPE
    struct sigaction act = { 0 };
    act.sa_sigaction = SigHandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGFPE, &act, 0);

    //Initialize Semaphore and Shared memory
    mySem =  sem_open("MySemaphore", O_CREAT, 0777, 1);
    shmem = shm_open("myShm", O_CREAT | O_RDWR, 0777);
    ftruncate(shmem, 1024);
    
    printf("user2 pid: %d\n", getpid());

    printf("waiting for signal...\n");
    pause();

    printf("waiting for signal...\n");
    pause();

    shm_unlink("myShm");
    sem_close(mySem);
}