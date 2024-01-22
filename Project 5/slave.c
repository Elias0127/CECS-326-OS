#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include "myShm.h"

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Slave execution failed due to wrong number of arguments.\n");
        exit(1);
    }

    printf("Slave begins execution\n");

    int child_num = atoi(argv[1]);
    char *shm_name = argv[2];
    char *sem_name = argv[3];

    printf("I am child number %d, received shared memory name %s and semaphore name %s.\n", child_num, shm_name, sem_name);

    key_t key = ftok(shm_name, 65);

    int shmid = shmget(key, sizeof(struct CLASS), 0666);
    if (shmid == -1)
    {
        perror("Shared memory access failed in slave");
        exit(1);
    }

    struct CLASS *shm_ptr = (struct CLASS *)shmat(shmid, NULL, 0);

    // Open the named semaphore for I/O control
    sem_t *io_sem = sem_open(sem_name, 0);
    if (io_sem == SEM_FAILED)
    {
        perror("Named semaphore opening failed in slave");
        exit(1);
    }

    // Open the named semaphore for index control
    char index_sem_name[] = "/index_sem";
    sem_t *index_sem = sem_open(index_sem_name, 0);
    if (index_sem == SEM_FAILED)
    {
        perror("Index semaphore opening failed in slave");
        exit(1);
    }

    // Acquire the index semaphore lock before modifying the shared memory
    sem_wait(index_sem);

    // Critical section for modifying shared memory
    int slot = shm_ptr->index;
    shm_ptr->response[slot] = child_num;
    shm_ptr->index++;

    // Release the index semaphore lock
    sem_post(index_sem);

    // Acquire the I/O semaphore lock for I/O operation
    sem_wait(io_sem);
    printf("Child %d wrote its number to slot %d.\n", child_num, slot);
    sem_post(io_sem);

    shmdt(shm_ptr);
    sem_close(io_sem);
    sem_close(index_sem);

    printf("Child %d closed access to shared memory and terminates.\n", child_num);
    return 0;
}
