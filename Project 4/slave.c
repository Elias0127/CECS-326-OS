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
    // Check if the correct number of command-line arguments are provided
    if (argc != 4)
    {
        printf("Slave execution failed due to wrong number of arguments.\n");
        exit(1);
    }

    // Start of the slave process execution
    printf("Slave begins execution\n");

    // Parse command-line arguments
    int child_num = atoi(argv[1]); // Child number
    char *shm_name = argv[2];      // Name of the shared memory segment
    char *sem_name = argv[3];      // Name of the semaphore

    // Displaying the received parameters
    printf("I am child number %d, received shared memory name %s and semaphore name %s.\n", child_num, shm_name, sem_name);

    // Generate a unique key for the shared memory segment
    key_t key = ftok(shm_name, 65);

    // Access the existing shared memory segment
    int shmid = shmget(key, sizeof(struct CLASS), 0666);
    if (shmid == -1)
    {
        perror("Shared memory access failed in slave");
        exit(1);
    }

    // Attach the shared memory segment to the slave process
    struct CLASS *shm_ptr = (struct CLASS *)shmat(shmid, NULL, 0);

    // Open the semaphore for synchronization
    sem_t *sem = sem_open(sem_name, 0);
    if (sem == SEM_FAILED)
    {
        perror("Semaphore opening failed in slave");
        exit(1);
    }

    // Acquire the semaphore lock before modifying the shared memory
    sem_wait(sem);

    // Write the child number to the shared memory and update the index
    int slot = shm_ptr->index;
    shm_ptr->response[slot] = child_num;
    shm_ptr->index++;

    // Release the semaphore lock after the modification is done
    sem_post(sem);

    // Report the slot number and updated index
    printf("I have written my child number to slot %d and updated index to %d.\n", slot, slot + 1);

    // Detach the shared memory segment from the slave process
    shmdt(shm_ptr);

    // Close the semaphore
    sem_close(sem);

    // Indicate the termination of the slave process
    printf("Child %d closed access to shared memory and terminates.\n", child_num);
    return 0;
}
