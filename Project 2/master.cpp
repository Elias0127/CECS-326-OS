#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

// Define a message key for the message queue
#define MESSAGE_KEY 1234

int main(int argc, char *argv[])
{
    // Ensure the user provides the number of senders/receivers as an argument
    if (argc != 2)
    {
        cerr << "Usage: ./master <number_of_senders_and_receivers>" << endl;
        return 1;
    }

    // Convert argument to integer representing number of senders and receivers
    int n = stoi(argv[1]);
    pid_t pid;

    // Initial output of master process
    cout << "Master, PID " << getpid() << ", begins execution" << endl;

    // Create (or get) a message queue and print its ID
    int msqid = msgget(MESSAGE_KEY, 0666 | IPC_CREAT);
    cout << "Master acquired a message queue, id " << msqid << endl;

    // Fork n child processes to act as senders
    for (int i = 0; i < n; ++i)
    {
        pid = fork();
        // This block will be executed by the child process
        if (pid == 0) 
        {
            char mqid[10];
            // Convert message queue ID to string
            snprintf(mqid, sizeof(mqid), "%d", msqid);
            // Execute the sender program with the message queue ID as an argument
            execl("./sender", "sender", mqid, NULL);
            // Exit child process after execl
            exit(0);                                   
        }
    }
    cout << "Master created " << n << " child processes to serve as sender" << endl;

    // Fork n child processes to act as receivers
    for (int i = 0; i < n; ++i)
    {
        pid = fork();
        // This block will be executed by the child process
        if (pid == 0)
        {
            char mqid[10];
            // This block will be executed by the child process
            snprintf(mqid, sizeof(mqid), "%d", msqid);
            // Execute the receiver program with the message queue ID as an argument
            execl("./receiver", "receiver", mqid, NULL);
            // Exit child process after execl
            exit(0);                                    
        }
    }
    cout << "Master created " << n << " child processes to serve as receiver" << endl;

    // Master process waits for all sender and receiver child processes to terminate
    cout << "Master waits for all child processes to terminate" << endl;
    for (int i = 0; i < 2 * n; ++i)
    {
        wait(NULL);
    }

    // Print termination message and remove the message queue
    cout << "Master received termination signals from all child processes, removed message queue, and terminates" << endl;
    // Remove the message queue
    msgctl(msqid, IPC_RMID, NULL); 

    return 0;
}