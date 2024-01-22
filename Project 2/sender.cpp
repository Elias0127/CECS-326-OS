#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

using namespace std;

// Define constants for the message key and message types
#define MESSAGE_KEY 1234
#define SENDER_MSG_TYPE 1
#define RECEIVER_MSG_TYPE 2

// Structure for the message to be sent/received
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
        cerr << "Usage: ./sender <message_queue_id>" << endl;
        return 1;
    }

    // Convert argument to integer for message queue ID and get the process ID
    int msqid = stoi(argv[1]);
    pid_t pid = getpid();

    // Initial output of sender process
    cout << "Sender, PID " << pid << ", begins execution" << endl;
    cout << "Sender with PID " << pid << " received message queue id " << msqid << " through commandline argument" << std::endl;
    cout << "Sender with PID " << pid << ": Please input your message" << endl;

    // Read the message from the user and send it to the message queue
    message msg;
    msg.msg_type = SENDER_MSG_TYPE;
    cin.getline(msg.msg_text, 100);
    msgsnd(msqid, &msg, sizeof(msg.msg_text), 0);

    // Output the message that was sent to the message queue
    cout << "Sender with PID " << pid << " sent the following message into message queue:" << endl;
    cout << msg.msg_text << endl;

    // Wait and receive the acknowledgement message from the receiver
    msgrcv(msqid, &msg, sizeof(msg.msg_text), RECEIVER_MSG_TYPE, 0);
    cout << "Sender with PID " << pid << " receives the following acknowledgement message" << endl;
    cout << msg.msg_text << endl;

    // Print termination message
    cout << "Sender with PID " << pid << " terminates" << endl;

    return 0;
}