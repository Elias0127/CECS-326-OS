#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include "myShm.h"

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Usage: ./master <number_of_children> <shared_memory_name> <semaphore_name>\n");
        exit(1);
    }

    printf("Master begins execution\n");

    int n = atoi(argv[1]);
    char *shm_name = argv[2];
    char *sem_name = argv[3];

    key_t key = ftok(shm_name, 65);

    int shmid = shmget(key, sizeof(struct CLASS), 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        perror("Shared memory creation failed");
        exit(1);
    }

    struct CLASS *shm_ptr = (struct CLASS *)shmat(shmid, NULL, 0);
    shm_ptr->index = 0;

    printf("Master created a shared memory segment named %s\n", shm_name);

    // Create a named semaphore for I/O control
    sem_t *sem = sem_open(sem_name, O_CREAT, 0644, 1);
    if (sem == SEM_FAILED)
    {
        perror("Semaphore creation failed");
        exit(1);
    }

    // Create a second named semaphore for index control
    char index_sem_name[] = "/index_sem";
    sem_t *index_sem = sem_open(index_sem_name, O_CREAT, 0644, 1);
    if (index_sem == SEM_FAILED)
    {
        perror("Index semaphore creation failed");
        exit(1);
    }

    printf("Master created a semaphore named %s\n", sem_name);

    for (int i = 1; i <= n; i++)
    {
        if (fork() == 0)
        {
            char child_num_str[10];
            sprintf(child_num_str, "%d", i);
            execl("./slave", "slave", child_num_str, shm_name, sem_name, NULL);
            exit(0);
        }
    }

    printf("Master created %d child processes to execute slave\n", n);

    for (int i = 0; i < n; i++)
        wait(NULL);

    printf("Master received termination signals from all %d child processes\n", n);
    printf("Updated content of shared memory segment after access by child processes:\n");

    for (int i = 0; i < shm_ptr->index; i++)
        printf("%d ", shm_ptr->response[i]);
    printf("\n");

    shmdt(shm_ptr);
    shmctl(shmid, IPC_RMID, NULL);

    sem_close(sem);
    sem_unlink(sem_name);

    // Close and unlink the index semaphore
    sem_close(index_sem);
    sem_unlink(index_sem_name);

    printf("Master closed semaphores and shared memory, and is exiting\n");

    return 0;
}
