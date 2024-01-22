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
    // Ensure the correct number of command-line arguments are provided
    if (argc != 4)
    {
        printf("Usage: ./master <number_of_children> <shared_memory_name> <semaphore_name>\n");
        exit(1);
    }

    // Start of the master process execution
    printf("Master begins execution\n");

    // Parse command-line arguments
    int n = atoi(argv[1]);    // Number of child processes
    char *shm_name = argv[2]; // Name of the shared memory segment
    char *sem_name = argv[3]; // Name of the semaphore

    // Generate a unique key for the shared memory segment
    key_t key = ftok(shm_name, 65);

    // Create the shared memory segment
    int shmid = shmget(key, sizeof(struct CLASS), 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        perror("Shared memory creation failed");
        exit(1);
    }

    // Attach the shared memory segment to the master process
    struct CLASS *shm_ptr = (struct CLASS *)shmat(shmid, NULL, 0);
    shm_ptr->index = 0; // Initialize the index in the shared memory

    printf("Master created a shared memory segment named %s\n", shm_name);

    // Create a POSIX semaphore for synchronization
    sem_t *sem = sem_open(sem_name, O_CREAT, 0644, 1);
    if (sem == SEM_FAILED)
    {
        perror("Semaphore creation failed");
        exit(1);
    }

    printf("Master created a semaphore named %s\n", sem_name);

    // Fork child processes and execute the slave program
    for (int i = 1; i <= n; i++)
    {
        if (fork() == 0) // Child process
        {
            char child_num_str[10];
            sprintf(child_num_str, "%d", i); // Convert child number to string
            execl("./slave", "slave", child_num_str, shm_name, sem_name, NULL);
            exit(0);
        }
    }

    printf("Master created %d child processes to execute slave\n", n);
    printf("Master waits for all child processes to terminate\n");

    // Wait for all child processes to terminate
    for (int i = 0; i < n; i++)
        wait(NULL);

    printf("Master received termination signals from all %d child processes\n", n);
    printf("Updated content of shared memory segment after access by child processes:\n");

    // Print the contents of the shared memory segment
    for (int i = 0; i < shm_ptr->index; i++)
        printf("%d ", shm_ptr->response[i]);
    printf("\n");

    // Detach and remove the shared memory segment
    shmdt(shm_ptr);
    shmctl(shmid, IPC_RMID, NULL);

    // Close and unlink the semaphore
    sem_close(sem);
    sem_unlink(sem_name);

    printf("Master removed the semaphore\n");
    printf("Master closed access to shared memory, removed shared memory segment, and is exiting\n");

    return 0;
}
