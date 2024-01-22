#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include "myShm.h"

int main(int argc, char *argv[])
{
    // Check if the correct number of command-line arguments are provided
    if (argc != 3)
    {
        printf("Slave execution failed due to wrong number of arguments.\n");
        exit(1);
    }

    printf("Slave begins execution\n");

    // Convert command-line arguments to appropriate data types
    int child_num = atoi(argv[1]);
    char *shm_name = argv[2];

    printf("I am child number %d, received shared memory name %s.\n", child_num, shm_name);

    // Generate a unique key for the shared memory segment
    key_t key = ftok(shm_name, 65);
    // Access the existing shared memory segment
    int shmid = shmget(key, sizeof(struct CLASS), 0666);
    if (shmid == -1)
    {
        perror("Shared memory access failed in slave");
        exit(1);
    }

    // Attach the shared memory segment to the address space of the slave process
    struct CLASS *shm_ptr = (struct CLASS *)shmat(shmid, NULL, 0);

    // Write the child number into the next available slot in the shared memory
    int slot = shm_ptr->index;
    shm_ptr->response[slot] = child_num;
    shm_ptr->index++;

    printf("I have written my child number to slot %d and updated index to %d.\n", slot, slot + 1);
    // Detach the shared memory segment from the address space of the slave process
    shmdt(shm_ptr);

    printf("Child %d closed access to shared memory and terminates.\n", child_num);
    return 0;
}
