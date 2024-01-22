#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <unistd.h>
#include "myShm.h"

int main(int argc, char *argv[])
{
    // Check if the correct number of command-line arguments are provided
    if (argc != 3)
    {
        printf("Usage: ./master <number_of_children> <shared_memory_name>\n");
        exit(1);
    }

    printf("Master begins execution\n");

    // Convert command-line arguments to appropriate data types
    int n = atoi(argv[1]);
    char *shm_name = argv[2];

    // Generate a unique key for the shared memory segment
    key_t key = ftok(shm_name, 65);

    // Create a shared memory segment
    int shmid = shmget(key, sizeof(struct CLASS), 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        perror("Shared memory creation failed");
        exit(1);
    }

    // Attach the shared memory segment to address space of the master process
    struct CLASS *shm_ptr = (struct CLASS *)shmat(shmid, NULL, 0);
    // Initialize the index in shared memory to 0
    shm_ptr->index = 0;

    printf("Master created a shared memory segment named %s\n", shm_name);
    printf("Master created %d child processes to execute slave\n", n);

    // Fork n child processes
    for (int i = 1; i <= n; i++)
    {
        if (fork() == 0)
        { // Child process
            char child_num_str[10];
            sprintf(child_num_str, "%d", i);
            // Execute the slave program in the child process
            execl("./slave", "slave", child_num_str, shm_name, NULL);
            exit(0);
        }
    }

    printf("Master waits for all child processes to terminate\n");
    // Wait for all child processes to terminate
    for (int i = 0; i < n; i++)
        wait(NULL);

    printf("Master received termination signals from all %d child processes\n", n);
    printf("Updated content of shared memory segment after access by child processes:\n");
    // Print the content of the shared memory segment
    for (int i = 0; i < shm_ptr->index; i++)
        printf("%d ", shm_ptr->response[i]);
    printf("\n");

    // Detach the shared memory segment from the address space of the master process
    shmdt(shm_ptr);
    // Remove the shared memory segment
    shmctl(shmid, IPC_RMID, NULL);
    printf("Master removed shared memory segment, and is exiting\n");

    return 0;
}
