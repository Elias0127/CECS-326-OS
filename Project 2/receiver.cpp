#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

using namespace std;

#define MESSAGE_KEY 1234
#define SENDER_MSG_TYPE 1
#define RECEIVER_MSG_TYPE 2

struct message
{
    // Type of message (sender or receiver type)
    long msg_type;
    // Message content
    char msg_text[100];
};

int main(int argc, char *argv[])
{
    // Ensure the user provides the message queue ID as an argument
    if (argc != 2)
    {
        cerr << "Usage: ./receiver <message_queue_id>" << endl;
        return 1;
    }
    // Convert argument to integer for message queue ID and get the process ID
    int msqid = stoi(argv[1]);
    pid_t pid = getpid();

    // Initial output of receiver process
    cout << "Receiver, PID " << pid << ", begins execution" << endl;
    cout << "Receiver with PID " << pid << " received message queue id " << msqid << " through commandline argument" << endl;

    // Wait and receive the message from the sender
    message msg;
    msgrcv(msqid, &msg, sizeof(msg.msg_text), SENDER_MSG_TYPE, 0);

    // Output the message that was received from the message queue
    cout << "Receiver with PID " << pid << " retrieved the following message from message queue:" << endl;
    cout << msg.msg_text << endl;

    // Prepare and send acknowledgement message back to the sender
    strcpy(msg.msg_text, "Receiver with PID acknowledges receipt of message");
    msg.msg_type = RECEIVER_MSG_TYPE;
    msgsnd(msqid, &msg, sizeof(msg.msg_text), 0);

    // Print termination message
   cout << "Receiver with PID " << pid << " terminates" << endl;

    return 0;
}
