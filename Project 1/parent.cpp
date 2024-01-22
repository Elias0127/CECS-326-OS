#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char *argv[])
{
    // Check if there are any command-line arguments provided
    if (argc < 2)
    {
        cout << "Please provide names as command line arguments.\n";
        return 1;
    }

    // Calculate the number of children from the number of command-line arguments
    int numChildren = argc - 1;
    cout << "I have " << numChildren << " children.\n";

    for (int i = 1; i <= numChildren; ++i)
    {
        // Create a new process
        pid_t pid = fork();

        // Check if fork() was successful
        if (pid < 0)
        {
            cout << "Fork failed.\n";
            return 1;
        }
        else if (pid == 0)
        {
            // Convert child number to string for passing as argument to execvp
            string childNumber = to_string(i);

            // Prepare the argument list for the execvp call
            char *args[] = {(char *)"./child", (char *)childNumber.c_str(), argv[i], NULL};

            // Replace the current process image with the new one
            execvp(args[0], args);

            // If execvp returns, it must have failed.
            cout << "Exec failed.\n";
            return 1;
        }
    }
    // Parent process continues to generate the next child

    // Wait for all child processes to terminate
    for (int i = 1; i <= numChildren; ++i)
    {
        // Wait for a child process to terminate
        wait(NULL);
    }

    cout << "All child processes terminated. Parent exits.\n";
    return 0;
}
